// InterceptDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#define INTERCEPTDLL_EXPORTS
#include "InterceptDLL.h"
#include <string>
#include <sstream>


DWORD attachedPID;
HHOOK hKeyHook;
std::ostringstream currentCommand;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	std::ostringstream oss;
	switch(ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			attachedPID = GetCurrentProcessId();
			oss << "Attached to PID " << attachedPID;
			showMessage(oss.str().c_str());

			CreateThread(NULL, NULL, ListenToKeys, NULL, NULL, NULL);
			break;
		case DLL_PROCESS_DETACH:
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
	}
	return true;
}


LRESULT WINAPI KeyEvent(int nCode, WPARAM wParam, LPARAM lParam) {
	if(IsWindowInFocus()) {
		if ((nCode == HC_ACTION) && ((wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN))) {
			KBDLLHOOKSTRUCT kbdStruct = *((KBDLLHOOKSTRUCT *) lParam);
			BYTE keyState[256];
			WCHAR buffer[16];

			GetKeyboardState((PBYTE) &keyState);
			ToUnicode(kbdStruct.vkCode, kbdStruct.scanCode, (PBYTE) &keyState, (LPWSTR) &buffer, sizeof(buffer) / 2, 0);

			unsigned int keyCode = (unsigned int) buffer[0];

			if(keyCode == 13) {
				currentCommand.seekp(0, std::ios::end);
				int commandLength = currentCommand.tellp();
				if(commandLength != 0) {
					std::ostringstream msg;
					msg << "Command Executed: " << currentCommand.str();
					showMessage(msg.str().c_str());
				}

				currentCommand.str("");
				currentCommand.clear();
				currentCommand.seekp(0, std::ios::beg);
			} else if(keyCode == 8) {
				currentCommand.str(currentCommand.str().erase());
				currentCommand.seekp(0, std::ios::end);
			} else {
				currentCommand << ((char) buffer[0]);
			}
		}
	}

	return CallNextHookEx(hKeyHook, nCode, wParam, lParam);
}




DWORD WINAPI ListenToKeys(LPVOID lpPtr) {
	hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC) KeyEvent, GetModuleHandle(NULL), 0);

	MSG message;
	while (GetMessage(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	UnhookWindowsHookEx(hKeyHook);
	return true;
}

BOOL IsWindowInFocus() {
	DWORD focusedPID;

	HWND hFocusedWindow = GetForegroundWindow();
	GetWindowThreadProcessId(hFocusedWindow, &focusedPID);

	return (focusedPID == attachedPID);
}

DLLIMPORT void showMessage(const char* message) {
	MessageBoxA(NULL, message, "InterceptDLL", MB_OK);
}