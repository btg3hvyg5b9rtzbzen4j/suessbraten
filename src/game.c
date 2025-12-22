#include "game.h"

int GetPlayerCount(HANDLE hProcess, uintptr_t moduleBase)
{
    int playerCount;
    ReadProcessMemory(hProcess, (LPCVOID)(moduleBase + O_PLAYER_COUNT), &playerCount, sizeof(int), NULL);
    return playerCount;
}

Player *GetEntityList(HANDLE hProcess, uintptr_t moduleBase, int count)
{
    uintptr_t entityListAddress;
    ReadProcessMemory(hProcess, (LPCVOID)(moduleBase + O_ENTITY_LIST), &entityListAddress, sizeof(uintptr_t), NULL);

    uintptr_t *entities = (uintptr_t *)malloc(count * sizeof(uintptr_t));
    ReadProcessMemory(hProcess, (LPCVOID)(entityListAddress), entities, count * sizeof(uintptr_t), NULL);

    Player *playerEntities = (Player *)malloc(count * sizeof(Player));

    for (int i = 0; i < count; i++)
    {
        ReadProcessMemory(hProcess, (LPCVOID)(entities[i]), &playerEntities[i], sizeof(Player), NULL);
    }

    free(entities);
    return playerEntities;
}

Matrix4 GetViewMatrix(HANDLE hProcess, uintptr_t moduleBase)
{
    Matrix4 viewMatrix = {0};
    ReadProcessMemory(hProcess, (LPCVOID)(moduleBase + O_VIEW_MATRIX), &viewMatrix, sizeof(Matrix4), NULL);
    return viewMatrix;
}

Matrix4 GetProjectionMatrix(HANDLE hProcess, uintptr_t moduleBase)
{
    Matrix4 projMatrix = {0};
    ReadProcessMemory(hProcess, (LPCVOID)(moduleBase + O_PROJ_MATRIX), &projMatrix, sizeof(Matrix4), NULL);
    return projMatrix;
}