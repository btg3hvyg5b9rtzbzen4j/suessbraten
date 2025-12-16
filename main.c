#define WIN32_LEAN_AND_MEAN

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

#include <windows.h>
#include <tlhelp32.h>

void panic(const char *msg) {
  fprintf(stderr, "[!] %s\n", msg);
  _getch();
  exit(1);
}

DWORD getProcessId(const char *processName) {
  HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  if (!hProcessSnap) panic("Couldn't get process list snapshot.");

  PROCESSENTRY32 procEntry = {.dwSize = sizeof(PROCESSENTRY32)};

  if (!Process32First(hProcessSnap, &procEntry)) {
    CloseHandle(hProcessSnap);
    panic("Couldn't get first process in list.");
  }

  DWORD pid = 0;
  do {
    if (strcmp(processName, procEntry.szExeFile) == 0) {
      pid = procEntry.th32ProcessID;
      break;
    }
  } while (Process32Next(hProcessSnap, &procEntry));
  CloseHandle(hProcessSnap);

  if (pid == 0) panic("Couldn't get process ID.");

  return pid;
}

uintptr_t getModuleBase(DWORD pid, const char *moduleName) {
  HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

  if (!hModuleSnap) panic("Couldn't get module list snapshot.");

  MODULEENTRY32 modEntry = {.dwSize = sizeof(MODULEENTRY32)};

  if (!Module32First(hModuleSnap, &modEntry)) {
    CloseHandle(hModuleSnap);
    panic("Couldn't get first module in list.");
  }

  uintptr_t modBase = 0;
  do {
    if (strcmp(moduleName, modEntry.szModule) == 0) {
      modBase = (uintptr_t)modEntry.modBaseAddr;
      break;
    }
  } while (Module32Next(hModuleSnap, &modEntry));

  CloseHandle(hModuleSnap);

  if (modBase == 0) panic("Couldn't get module base.");

  return modBase;
}

int main() {
  const char* processName = "sauerbraten.exe";

  DWORD pid = getProcessId(processName);
  printf("[+] Found process ID: %lu\n", pid);

  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  printf("[+] Got process handle: 0x%zX\n", (size_t)hProcess);

  uintptr_t modBase = getModuleBase(pid, processName);
  printf("[+] Got module base: 0x%zX\n", modBase);

  

  CloseHandle(hProcess);
  _getch();
  return 0;
}