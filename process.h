#ifndef PROCESS_H
#define PROCESS_H

#include <windows.h>

const DWORD GetProcessIdFromName(const char* processName);
uintptr_t GetProcessModuleBase(DWORD processId);

#endif