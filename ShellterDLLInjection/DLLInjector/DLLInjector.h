DWORD getProcID(const char* name);
BOOL injectNTDLL(DWORD procID, const char* dllPath);
BOOL injectDLL(DWORD procID, const char* dllPath);
bool fileExists(char *name);