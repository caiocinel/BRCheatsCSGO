    #pragma once

#include <d3d9.h>
#include <memory>
#include <type_traits>
#include <Windows.h>

#include "Hooks/MinHook.h"
#include "Hooks/VmtHook.h"
#include "Hooks/VmtSwap.h"
#include "Hooks/vfunc_hook.hpp"
#include "SDK/SteamAPI.h"


struct SoundInfo;

// Easily switch hooking method for all hooks, choose between MinHook/VmtHook/VmtSwap
using HookType = MinHook;


class Hooks {
public:
    Hooks(HMODULE module) noexcept;

    void install() noexcept;
    void uninstall() noexcept;

    WNDPROC originalWndProc;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*)> originalPresent;
    std::add_pointer_t<HRESULT __stdcall(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)> originalReset;
    std::add_pointer_t<int __fastcall(SoundInfo&)> originalDispatchSound;
    
    

    HookType bspQuery;
    HookType client;
    HookType clientMode;
    HookType engine;
    HookType modelRender;
    HookType panel;
    HookType sound;
    HookType surface;
    HookType viewRender;
    vfunc_hook gc_hook;
	HookType gameEventManager;
	VmtSwap networkChannel;
    HookType svCheats;
private:
    HMODULE module;
    HWND window;
};

namespace indexhooks
{
    constexpr auto send_message = 0;
    constexpr auto retrieve_message = 2;
}

inline std::unique_ptr<Hooks> hooks;
