#ifndef GAME_H
#define GAME_H

#include <windows.h>

#include "math.h"

#define O_VIEW_MATRIX 0x32CFC0
#define O_PROJ_MATRIX 0x32D000
#define O_ENTITY_LIST 0x346C90
#define O_PLAYER_COUNT 0x346C9C

typedef struct
{
    Vector3 headPos;
} Player;

int GetPlayerCount(HANDLE hProcess, uintptr_t moduleBase);
Player *GetEntityList(HANDLE hProcess, uintptr_t moduleBase, int count);
Matrix4 GetViewMatrix(HANDLE hProcess, uintptr_t moduleBase);
Matrix4 GetProjectionMatrix(HANDLE hProcess, uintptr_t moduleBase);

#endif