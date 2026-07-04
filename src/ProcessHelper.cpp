#include "../includes/ProcessHelper.h"
#include <windows.h>

void *ProcessHelper::GetFunc(const char *dll, const char *func) {
    HMODULE hMod = GetModuleHandleA(dll);
    if (hMod) {
        return (void *)GetProcAddress(hMod, func);
    }
    return nullptr;
}
