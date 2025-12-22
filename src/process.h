#ifndef PROCESS_H
#define PROCESS_H

#include <windows.h>

/**
 * Gets the process id for the given process.
 * @returns The process id of the process.
 */
DWORD GetProcessIdFromName(const char *processName);

/**
 * Gets the module base address for the given process.
 * @param processId The target process id.
 */
uintptr_t GetProcessModuleBase(DWORD processId);

#endif