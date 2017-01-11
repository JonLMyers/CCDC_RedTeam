#pragma once

#ifdef INTERCEPTDLL_EXPORTS
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

DLLIMPORT void showMessage(const char* message);
DWORD WINAPI ListenToKeys(LPVOID lpPtr);
BOOL IsWindowInFocus();
LRESULT WINAPI KeyEvent(int nCode, WPARAM wParam, LPARAM lParam);