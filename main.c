#define WIN32_LEAN_AND_MEAN

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

#include <windows.h>
#include <winuser.h>
#include <tlhelp32.h>

#define COLORKEY RGB(59, 22, 0)

// offsets
#define O_PROJ_MATRIX 0x32D000
#define O_VIEW_MATRIX 0x32CFC0

// globals
HWND g_targetHwnd;
HWND g_overlayHwnd;
HANDLE g_processHandle;
uintptr_t g_moduleBase;

COORD g_testDraw;

typedef struct {
    float x, y, z;
} vector3;

typedef struct {
    float x, y, z, w;
} vector4;

typedef struct {
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;
} matrix4f;

matrix4f g_viewMatrix;
matrix4f g_projMatrix;

matrix4f matrix4f_mul(matrix4f a, matrix4f b) {
    matrix4f r;

    r.m00 = a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20 + a.m03 * b.m30;
    r.m01 = a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21 + a.m03 * b.m31;
    r.m02 = a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22 + a.m03 * b.m32;
    r.m03 = a.m00 * b.m03 + a.m01 * b.m13 + a.m02 * b.m23 + a.m03 * b.m33;

    r.m10 = a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20 + a.m13 * b.m30;
    r.m11 = a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31;
    r.m12 = a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32;
    r.m13 = a.m10 * b.m03 + a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33;

    r.m20 = a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20 + a.m23 * b.m30;
    r.m21 = a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31;
    r.m22 = a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32;
    r.m23 = a.m20 * b.m03 + a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33;

    r.m30 = a.m30 * b.m00 + a.m31 * b.m10 + a.m32 * b.m20 + a.m33 * b.m30;
    r.m31 = a.m30 * b.m01 + a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31;
    r.m32 = a.m30 * b.m02 + a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32;
    r.m33 = a.m30 * b.m03 + a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33;

    return r;
}

vector4 matrix4f_vector4_mul(vector4 v, matrix4f m) {
    return (vector4){v.x * m.m00 + v.y * m.m10 + v.z * m.m20 + v.w * m.m30,
                     v.x * m.m01 + v.y * m.m11 + v.z * m.m21 + v.w * m.m31,
                     v.x * m.m02 + v.y * m.m12 + v.z * m.m22 + v.w * m.m32,
                     v.x * m.m03 + v.y * m.m13 + v.z * m.m23 + v.w * m.m33};
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT:
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        SetTextColor(hdc, RGB(0, 255, 0));
        SetBkColor(hdc, RGB(0, 0, 0));

        HBRUSH bg = CreateSolidBrush(COLORKEY);

        FillRect(hdc, &ps.rcPaint, bg);

        if (g_testDraw.X > 0 && g_testDraw.Y > 0) {
            HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
            RECT testRect = {g_testDraw.X - 10, g_testDraw.Y - 10,
                             g_testDraw.X + 10, g_testDraw.Y + 10};
            FillRect(hdc, &testRect, brush);
            DeleteObject(brush);
        }

        DeleteObject(bg);

        EndPaint(hwnd, &ps);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

COORD world_to_screen(vector3 pos, BOOL *visible) {
    ReadProcessMemory(g_processHandle, (LPCVOID)(g_moduleBase + O_PROJ_MATRIX),
                      &g_projMatrix, sizeof(g_projMatrix), 0);

    ReadProcessMemory(g_processHandle, (LPCVOID)(g_moduleBase + O_VIEW_MATRIX),
                      &g_viewMatrix, sizeof(g_viewMatrix), 0);

    // combine proj + view matrix
    matrix4f pvMatrix = matrix4f_mul(g_viewMatrix, g_projMatrix);

    // turn to clip space
    vector4 csp =
        matrix4f_vector4_mul((vector4){pos.x, pos.y, pos.z, 1.f}, pvMatrix);

    if (csp.w < 0.0f) {
        *visible = FALSE;
        return (COORD){};
    }

    // normalize to device coordinates
    vector3 ndc = {csp.x / csp.w, csp.y / csp.w, csp.z / csp.w};

    RECT screen;
    GetClientRect(g_targetHwnd, &screen);

    int screenWidth = screen.right - screen.left;
    int screenHeight = screen.bottom - screen.top;

    float sx = (ndc.x + 1.0f) * screenWidth / 2.0f;
    float sy = (1.0f - ndc.y) * screenHeight / 2.0f;

    *visible = TRUE;

    return (COORD){sx, sy};
}

void panic(const char *msg) {
    fprintf(stderr, "[!] %s\n", msg);
    _getch();
    exit(1);
}

typedef struct {
    DWORD pid;
    HWND hwnd;
} HandleData;

BOOL is_main_window(HWND handle) {
    return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) {
    HandleData *data = (HandleData *)lParam;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    if (pid != data->pid || !is_main_window(hwnd)) {
        return TRUE;
    }

    data->hwnd = hwnd;
    return FALSE;
}

void CALLBACK WinEventProc(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                           LONG idObject, LONG idChild, DWORD dwEventThread,
                           DWORD dwmsEventTime) {
    if (idObject != OBJID_WINDOW)
        return;

    if (hwnd == g_targetHwnd) {
        RECT rc;
        GetWindowRect(hwnd, &rc);
        int x = rc.left;
        int y = rc.top;
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;

        // Example: move your window to stay docked to the target
        SetWindowPos(g_overlayHwnd, NULL, x, y, w, h,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE _hPrevInstance,
                   PSTR _pCmdLine, int nCmdShow) {
    const char *processName = "sauerbraten.exe";
    DWORD processId;

    /*
     *   prepare console for debug output
     */
    AllocConsole();
    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);

    /*
     *   get process id
     */
    HANDLE hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (!hProcSnap)
        panic("Couldn't get process list snapshot.");

    PROCESSENTRY32 pe32 = {.dwSize = sizeof(PROCESSENTRY32)};

    if (!Process32First(hProcSnap, &pe32)) {
        CloseHandle(hProcSnap);
        panic("Couldn't get first process in list.");
    }

    do {
        if (strcmp(processName, pe32.szExeFile) == 0) {
            processId = pe32.th32ProcessID;
            break;
        }
    } while (Process32Next(hProcSnap, &pe32));

    CloseHandle(hProcSnap);

    if (processId == 0)
        panic("Couldn't get process ID.");

    printf("[+] Found process ID: %lu.\n", processId);

    /*
     *   get module base
     */

    HANDLE hModSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);

    if (!hModSnap)
        panic("Couldn't get module list snapshot.");

    MODULEENTRY32 modEntry = {.dwSize = sizeof(MODULEENTRY32)};

    if (!Module32First(hModSnap, &modEntry)) {
        CloseHandle(hModSnap);
        panic("Couldn't get first module in list.");
    }

    do {
        if (strcmp(processName, modEntry.szModule) == 0) {
            g_moduleBase = (uintptr_t)modEntry.modBaseAddr;
            break;
        }
    } while (Module32Next(hModSnap, &modEntry));

    CloseHandle(hModSnap);

    if (g_moduleBase == 0)
        panic("Couldn't get module base.");

    printf("[+] Got module base: 0x%zX.\n", g_moduleBase);

    /*
     *   get handle
     */
    g_processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!g_processHandle)
        panic("Couldn't get process handle.");
    printf("[+] Got process handle: 0x%zX.\n", (size_t)g_processHandle);

    /*
     *   create overlay
     */

    HandleData handleData = {.pid = processId};
    EnumWindows(EnumWindowsCallback, (LPARAM)&handleData);
    g_targetHwnd = handleData.hwnd;

    /* make sure we follow the target window */
    SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                    NULL, WinEventProc, processId, 0,
                    WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    RECT targetRect;
    GetWindowRect(g_targetHwnd, &targetRect);
    int targetWidth = targetRect.right - targetRect.left;
    int targetHeight = targetRect.bottom - targetRect.top;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = "Overlay";
    wc.hInstance = hInstance;

    RegisterClass(&wc);

    g_overlayHwnd = CreateWindowEx(
        WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST, wc.lpszClassName,
        wc.lpszClassName, WS_POPUP, targetRect.left, targetRect.top,
        targetWidth, targetHeight, NULL, NULL, hInstance, NULL);

    SetLayeredWindowAttributes(g_overlayHwnd, COLORKEY, 127, LWA_COLORKEY);

    ShowWindow(g_overlayHwnd, SW_SHOW);
    UpdateWindow(g_overlayHwnd);

    /*
     *   run program and handle overlay messages
     */
    MSG msg;

    while (!(GetAsyncKeyState(VK_END) & 1)) {
        if (PeekMessage(&msg, g_overlayHwnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        BOOL visible;
        g_testDraw = world_to_screen((vector3){0.f, 0.f, 0.f}, &visible);

        InvalidateRect(g_overlayHwnd, NULL, TRUE);
        UpdateWindow(g_overlayHwnd);

        Sleep(1000 / 240);
    }

    return 0;
}