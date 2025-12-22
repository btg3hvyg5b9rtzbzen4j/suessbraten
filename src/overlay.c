#include "overlay.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
    {

        PostQuitMessage(0);
        exit(0);
        return 0;
    }
    case WM_PAINT:
    {

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        SetTextColor(hdc, RGB(0, 255, 0));
        SetBkColor(hdc, RGB(0, 0, 0));

        HBRUSH bg = CreateSolidBrush(COLORKEY);

        FillRect(hdc, &ps.rcPaint, bg);

        DeleteObject(bg);

        EndPaint(hwnd, &ps);
        return 0;
    }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

HWND CreateOverlay(HINSTANCE hInstance)
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = "Overlay";
    wc.hInstance = hInstance;

    RegisterClass(&wc);

    HWND hOverlay = CreateWindowEx(
        WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST, wc.lpszClassName,
        wc.lpszClassName, WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    SetLayeredWindowAttributes(hOverlay, COLORKEY, 127, LWA_ALPHA);
    ShowWindow(hOverlay, SW_SHOW);
    UpdateWindow(hOverlay);

    return hOverlay;
}

void UpdateOverlay(HWND hOverlay, HWND hTarget)
{
    MSG msg = {0};

    if (PeekMessage(&msg, hOverlay, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    RECT tClientRect = {0};
    GetClientRect(hTarget, &tClientRect);

    POINT tScreenCoords = {0};
    tScreenCoords.x = 0;
    tScreenCoords.y = 0;
    ClientToScreen(hTarget, &tScreenCoords);

    SetWindowPos(hOverlay, NULL, tScreenCoords.x, tScreenCoords.y,
                 tClientRect.right, tClientRect.bottom, 0);
}

int GetFps()
{
    DEVMODE dm = {0};
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
    return dm.dmDisplayFrequency;
}