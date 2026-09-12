#include "../../inc/main.h"
#include "script.h"

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        scriptRegister(hInstance, ScriptMain);
    }

    if (reason == DLL_PROCESS_DETACH)
    {
        scriptUnregister(hInstance);
    }

    return TRUE;
}