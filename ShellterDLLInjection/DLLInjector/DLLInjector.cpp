// DLLInjector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include "DLLInjector.h"
#include <tlhelp32.h>
#include <sys/stat.h>
#include <string>


int main() {
	const char* procName = "powershell.exe";
	DWORD procID = getProcID(procName);

	if (procID != 0) {
		printf("Process ID: %d\n", procID);

		char dllPath[MAX_PATH] = { 0 };
		GetFullPathNameA("..\\Debug\\InterceptDLL.dll", MAX_PATH, dllPath, NULL);

		printf("DLL Path: %s\n", dllPath);

		if (fileExists(dllPath)) {
			printf("DLL Exists!\n");
			BOOL injectionResult = injectDLL(procID, dllPath);
			printf("Injection %sed!\n", injectionResult ? "succeed" : "fail");
		}
	}

	getchar();
	return 0;
}

BOOL injectDLL(DWORD procID, const char* dllPath) {
	const int dllPathSize = (int) strlen(dllPath);

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

	LPVOID loadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle(L"Kernel32"), "LoadLibraryA");
	LPVOID dllMemAddr = (LPVOID)VirtualAllocEx(hProc, NULL, dllPathSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	SIZE_T bytesWritten;
	BOOL writeSuccess = WriteProcessMemory(hProc, dllMemAddr, dllPath, dllPathSize, &bytesWritten);
	if (writeSuccess && (dllPathSize == bytesWritten)) {
		HANDLE hThread = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)loadLibraryAddr, dllMemAddr, NULL, NULL);
		if (hThread != NULL) {
			printf("Success: the remote thread was successfully created. Handle: %p\n", hThread);
			WaitForSingleObject(hThread, INFINITE);
			return true;
		}
		else {
			printf("Error: the remote thread could not be created.\n");
		}
	}
	else {
		printf("Error: Only %d of %d bytes were written.\n", bytesWritten, dllPathSize);
	}

	return false;
}

bool fileExists(char *name) {
	struct stat buffer;
	return (stat(name, &buffer) == 0);
}


DWORD getProcID(const char* name) {
	size_t size = strlen(name) + 1;
	wchar_t* procName = new wchar_t[size];
	size_t outSize;
	mbstowcs_s(&outSize, procName, size, name, size - 1);

	PROCESSENTRY32 procEntry;
	HANDLE toolhelpSnapshot;
	BOOL retVal, foundProc = false;

	toolhelpSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (toolhelpSnapshot == INVALID_HANDLE_VALUE) {
		printf("Error: Unable to create toolhelp snapshot!\n");
		return false;
	}

	procEntry.dwSize = sizeof(PROCESSENTRY32);

	retVal = Process32First(toolhelpSnapshot, &procEntry);
	while (retVal) {
		if (!wcscmp(procEntry.szExeFile, procName)) {
			return procEntry.th32ProcessID;
		}
		retVal = Process32Next(toolhelpSnapshot, &procEntry);
	}

	return 0;
}