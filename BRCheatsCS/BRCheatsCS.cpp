#include <Windows.h>
#include <clocale>

#include "Hooks.h"

extern "C" BOOL WINAPI _CRT_INIT(HMODULE module, DWORD reason, LPVOID reserved);

BOOL APIENTRY DllEntryPoint(HMODULE module, DWORD reason, LPVOID reserved)
{
    if (!_CRT_INIT(module, reason, reserved))
        return FALSE;

    if (reason == DLL_PROCESS_ATTACH) {
        hooks = std::make_unique<Hooks>(module);
        std::setlocale(LC_ALL, ".utf8");
    }

    return TRUE;
}
