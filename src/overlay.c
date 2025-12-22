#include "overlay.h"

OverlayData g_overlayData = {0};

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

        // Clear background
        HBRUSH bg = CreateSolidBrush(COLORKEY);
        FillRect(hdc, &ps.rcPaint, bg);
        DeleteObject(bg);

        // Draw players
        HBRUSH ellipseBrush = CreateSolidBrush(RGB(0, 255, 0));
        SelectObject(hdc, ellipseBrush);

        for (int i = 0; i < g_overlayData.playerCount; i++)
        {
            int x = (int)g_overlayData.playerPositions[i].X;
            int y = (int)g_overlayData.playerPositions[i].Y;

            if (x <= 0 || y <= 0)
            {
                continue;
            }

            Ellipse(hdc, x - 5, y - 5, x + 5, y + 5);
        }

        DeleteObject(ellipseBrush);

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

    SetLayeredWindowAttributes(hOverlay, COLORKEY, 127, LWA_COLORKEY);
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

    InvalidateRect(hOverlay, NULL, FALSE);
    UpdateWindow(hOverlay);
}

int GetFps()
{
    DEVMODE dm = {0};
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
    return dm.dmDisplayFrequency;
}

COORD GetScreenSize(HWND hTarget)
{
    RECT tClientRect = {0};
    GetClientRect(hTarget, &tClientRect);

    COORD size = {0};
    size.X = tClientRect.right;
    size.Y = tClientRect.bottom;

    return size;
}