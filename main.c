#define WIN32_LEAN_AND_MEAN

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

#include <windows.h>
#include <winuser.h>
#include <tlhelp32.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI WindowThread(LPVOID lpParam)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = "Overlay";
    wc.hInstance = hInstance;
    
    RegisterClass(&wc);
    
    HWND hOverlay = CreateWindowEx(0, wc.lpszClassName, wc.lpszClassName, WS_CAPTION, 0, 0, 500, 500,
                                 NULL, NULL, hInstance, NULL);
    
    ShowWindow(hOverlay, SW_SHOW);
    UpdateWindow(hOverlay);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}

void panic(const char *msg) {
  fprintf(stderr, "[!] %s\n", msg);
  _getch();
  exit(1);
}

int main() {
  HANDLE hProcess = NULL;
  HWND hOverlay = NULL;
  const char *processName = "sauerbraten.exe";
  DWORD processId = 0;
  uintptr_t modBase = 0;

  // get process id
  HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  if (!hProcessSnap)
    panic("Couldn't get process list snapshot.");

  PROCESSENTRY32 procEntry = {.dwSize = sizeof(PROCESSENTRY32)};

  if (!Process32First(hProcessSnap, &procEntry)) {
    CloseHandle(hProcessSnap);
    panic("Couldn't get first process in list.");
  }

  do {
    if (strcmp(processName, procEntry.szExeFile) == 0) {
      processId = procEntry.th32ProcessID;
      break;
    }
  } while (Process32Next(hProcessSnap, &procEntry));

  CloseHandle(hProcessSnap);

  if (processId == 0)
    panic("Couldn't get process ID.");

  printf("[+] Found process ID: %lu.\n", processId);

  // get main module
  HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processId);

  if (!hModuleSnap)
    panic("Couldn't get module list snapshot.");

  MODULEENTRY32 modEntry = {.dwSize = sizeof(MODULEENTRY32)};

  if (!Module32First(hModuleSnap, &modEntry)) {
    CloseHandle(hModuleSnap);
    panic("Couldn't get first module in list.");
  }

  do {
    if (strcmp(processName, modEntry.szModule) == 0) {
      modBase = (uintptr_t)modEntry.modBaseAddr;
      break;
    }
  } while (Module32Next(hModuleSnap, &modEntry));

  CloseHandle(hModuleSnap);

  if (modBase == 0)
    panic("Couldn't get module base.");

  printf("[+] Got module base: 0x%zX.\n", modBase);

  // get process handle
  hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

  if (!hProcess)
    panic("Couldn't get process handle.");

  printf("[+] Got process handle: 0x%zX.\n", (size_t)hProcess);

  // run overlay in a separate thread cause we ball
  CreateThread(NULL, 0, WindowThread, NULL, 0, NULL);

  while (!(GetAsyncKeyState(VK_END) & 1)) {
    printf("run\n");
    Sleep(1000 / 60);
  }

  return 0;
}