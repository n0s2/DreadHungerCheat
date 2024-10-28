// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "windows.h"
#include "engine.h"
#include "cheat.h"

void* dll = nullptr;
bool err = false;

bool removeDll() {
    while(1){
        if (GetAsyncKeyState(VK_END) & 1) {
            FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(dll), 0);
        }
        Sleep(20);
    }
}

BOOL WINAPI DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(hModule);
        dll = hModule;
        err = EngineInit() && CheatInit();
        if (err) {
            MessageBoxA(NULL, "hello", "f", MB_OK);
            auto t = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(removeDll), NULL, 0, NULL);
            if (t) { CloseHandle(t); }
        }
        return err;
    }
    case DLL_PROCESS_DETACH: {
        if(err) removeCheat();
        return true;
    }

    }
    return TRUE;
}

