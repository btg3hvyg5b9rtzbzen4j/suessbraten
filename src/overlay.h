#ifndef OVERLAY_H
#define OVERLAY_H

#include <windows.h>

#define COLORKEY RGB(69, 69, 69)

/**
 * Creates the overlay window.
 * @param hInstance The HINSTANCE from WinMain
 * @returns A handle to the overlay window.
 */
HWND CreateOverlay(HINSTANCE hInstance);

/**
 * Re-positions, re-sizes and re-draws the overlay.
 * @param hOverlay The handle to the overlay window.
 * @param hTarget The handle to the target window.
 */
void UpdateOverlay(HWND hOverlay, HWND hTarget);

/**
 * Get the FPS of the current monitor.
 */
int GetFps();

#endif