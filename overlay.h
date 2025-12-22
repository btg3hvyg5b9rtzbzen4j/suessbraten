#ifndef OVERLAY_H
#define OVERLAY_H

#include <windows.h>

#define COLORKEY RGB(69, 69, 69)

HWND CreateOverlay(HINSTANCE hInstance);
void UpdateOverlay(HWND hOverlay, HWND hTarget);
int GetFps();

#endif