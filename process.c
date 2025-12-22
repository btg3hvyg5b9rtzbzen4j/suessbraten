#include "process.h"

#include <TlHelp32.h>

const DWORD GetProcessIdFromName(const char *processName) {
    DWORD processId = 0;
    HANDLE hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (!hProcSnap)
        return 0;

    PROCESSENTRY32 pe32 = {.dwSize = sizeof(PROCESSENTRY32)};

    if (!Process32First(hProcSnap, &pe32)) {
        CloseHandle(hProcSnap);
        return 0;
    }

    do {
        if (strcmp(processName, pe32.szExeFile) == 0) {
            processId = pe32.th32ProcessID;
            break;
        }
    } while (Process32Next(hProcSnap, &pe32));

    CloseHandle(hProcSnap);

    return processId;
}

uintptr_t GetProcessModuleBase(DWORD processId) {
    uintptr_t moduleBase = -1;
    HANDLE hModSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);

    if (!hModSnap)
        return -1;

    MODULEENTRY32 modEntry = {.dwSize = sizeof(MODULEENTRY32)};

    if (!Module32First(hModSnap, &modEntry)) {
        CloseHandle(hModSnap);
        return -1;
    }

    do {
        if (modEntry.th32ProcessID == processId) {
            moduleBase = (uintptr_t)modEntry.modBaseAddr;
            break;
        }
    } while (Module32Next(hModSnap, &modEntry));

    CloseHandle(hModSnap);

    return moduleBase;
}