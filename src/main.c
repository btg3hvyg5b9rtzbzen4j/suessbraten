#define WIN32_LEAN_AND_MEAN

#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

#include "game.h"
#include "process.h"
#include "overlay.h"

static void ErrorAndExit(const char *msg)
{
#ifdef DEBUG
    printf("Error: %s\n", msg);
    _getch();
#endif
    exit(1);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE _hPrevInstance,
                   PSTR _pCmdLine, int _nCmdShow)
{
#ifdef DEBUG
    AllocConsole();
    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
#endif

    const char *processName = "sauerbraten.exe";
    const char *windowName = "Cube 2: Sauerbraten";

    DWORD processId = GetProcessIdFromName(processName);
    if (!processId)
    {
        ErrorAndExit("Couldn't get process id.");
    }

    uintptr_t moduleBase = GetProcessModuleBase(processId);
    if (moduleBase == -1)
    {
        ErrorAndExit("Couldn't get module base.");
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess)
    {
        ErrorAndExit("Couldn't get process handle.");
    }

    HWND hOverlay = CreateOverlay(hInstance);
    if (!hOverlay)
    {
        ErrorAndExit("Couldn't create overlay.");
    }

    HWND hTarget = FindWindow(NULL, windowName);
    if (!hTarget)
    {
        ErrorAndExit("Couldn't find target window.");
    }

    const int wait = 1000 / GetFps();

    /*
     *  MAIN LOOP
     */
    while (!(GetAsyncKeyState(VK_END) & 1))
    {
        // Exit the program if the target window was closed.
        if (!IsWindow(hTarget))
        {
            break;
        }

        int playerCount = GetPlayerCount(hProcess, moduleBase);
        Player *entityList = GetEntityList(hProcess, moduleBase, playerCount);
        Matrix4 viewMatrix = GetViewMatrix(hProcess, moduleBase);
        Matrix4 projMatrix = GetProjectionMatrix(hProcess, moduleBase);
        COORD screenSize = GetScreenSize(hTarget);

        g_overlayData.playerCount = 0;

        for (int i = 1; i < playerCount; i++)
        {
            Vector2 screenPos;
            if (WorldToScreen(entityList[i].headPos, &screenPos, viewMatrix, projMatrix, screenSize.X, screenSize.Y))
            {
                g_overlayData.playerPositions[g_overlayData.playerCount] = (COORD){(SHORT)screenPos.x, (SHORT)screenPos.y};
                g_overlayData.playerCount++;
            }
        }

        free(entityList);
        UpdateOverlay(hOverlay, hTarget);
        Sleep(wait);
    }

#ifdef DEBUG
    fclose(fp);
    FreeConsole();
#endif

    CloseHandle(hProcess);
    DestroyWindow(hOverlay);

    return 0;
}