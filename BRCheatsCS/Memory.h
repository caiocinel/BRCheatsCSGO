#pragma once

#include <array>
#include <memory>
#include <string>
#include <type_traits>
#include <Windows.h>
#include <Psapi.h>
#include "Hacks/Tickbase.h"

#include "SDK/ModelInfo.h"
#include "SDK/SteamAPI.h"

class ClientMode;
class Entity;
class GameEventDescriptor;
class GameEventManager;
class Input;
class ItemSystem;
class KeyValues;
class MemAlloc;
class MoveHelper;
class MoveData;
class PlayerResource;
class ViewRender;
class WeaponSystem;
class IViewRenderBeams;
class MemAlloc;
class ClientState;
class ISteamClient;

struct ActiveChannels;
struct Channel;
struct GlobalVars;
struct GlowObjectManager;
struct Trace;
struct Vector;
class IViewRenderBeams;
class ClientState;



class Memory {
public:
    Memory() noexcept;

    uintptr_t present;
    uintptr_t reset;

    ClientMode* clientMode;
    Input* input;
    GlobalVars* globalVars;
    GlowObjectManager* glowObjectManager;
    ClientState* clientState;
	void* WriteUsercmdDeltaToBufferReturn;
	uintptr_t WriteUsercmd;
	
    bool* disablePostProcessing;

    std::add_pointer_t<void __fastcall(const char*)> loadSky;
    std::add_pointer_t<void __fastcall(const char*, const char*)> setClanTag;
    uintptr_t cameraThink;
    std::add_pointer_t<bool __stdcall(const char*)> acceptMatch;
    std::add_pointer_t<bool __cdecl(Vector, Vector, short)> lineGoesThroughSmoke;
    int(__fastcall* getSequenceActivity)(void*, StudioHdr*, int);
    bool(__thiscall* isOtherEnemy)(Entity*, Entity*);
    uintptr_t hud;
    int*(__thiscall* findHudElement)(uintptr_t, const char*);
    int(__thiscall* clearHudWeapon)(int*, int);
    std::add_pointer_t<ItemSystem* __cdecl()> itemSystem;
    void(__thiscall* setAbsOrigin)(Entity*, const Vector&);
    uintptr_t listLeaves;
    int* dispatchSound;
    uintptr_t traceToExit;
    ViewRender* viewRender;
    uintptr_t drawScreenEffectMaterial;
    std::add_pointer_t<bool __stdcall(const char*, const char*)> submitReport;
    uint8_t* fakePrime;
	 IViewRenderBeams* renderBeams;
	 void(__thiscall* setAbsAngle)(Entity*, const Vector&);
    uintptr_t UpdateState;
    uintptr_t CreateState;
    uintptr_t InvalidateBoneCache;
    MemAlloc* memalloc;
    ISteamGameCoordinator* SteamGameCoordinator;
    ISteamUser* SteamUser;
    int(__cdecl* RandomInt)(int min, int max);
    std::add_pointer_t<void __cdecl(const char* msg, ...)> debugMsg;
    std::add_pointer_t<void __cdecl(const std::array<std::uint8_t, 4>& color, const char* msg, ...)> conColorMsg;
    float* vignette;
    int(__thiscall* equipWearable)(void* wearable, void* player);
    int* predictionRandomSeed;
    MoveData* moveData;
    MoveHelper* moveHelper;
    std::uintptr_t keyValuesFromString;
    KeyValues*(__thiscall* keyValuesFindKey)(KeyValues* keyValues, const char* keyName, bool create);
    void(__thiscall* keyValuesSetString)(KeyValues* keyValues, const char* value);
    WeaponSystem* weaponSystem;
    std::uintptr_t newFunctionClientDLL;
    std::uintptr_t newFunctionEngineDLL;
    std::uintptr_t newFunctionStudioRenderDLL;
    std::uintptr_t newFunctionMaterialSystemDLL;

	
    std::add_pointer_t<const char** __fastcall(const char* playerModelName)> getPlayerViewmodelArmConfigForPlayerModel;
    GameEventDescriptor* (__thiscall* getEventDescriptor)(GameEventManager* _this, const char* name, int* cookie);
    ActiveChannels* activeChannels;
    Channel* channels;
    PlayerResource** playerResource;
    const wchar_t*(__thiscall* getDecoratedPlayerName)(PlayerResource* pr, int index, wchar_t* buffer, int buffsize, int flags);

    void setOrAddAttributeValueByName(std::uintptr_t attributeList, const char* attribute, float value) const noexcept
    {
#ifdef _WIN32
        __asm movd xmm2, value
        setOrAddAttributeValueByNameFunction(attributeList, attribute);
#else

#endif
    }

    void setOrAddAttributeValueByName(std::uintptr_t attributeList, const char* attribute, int value) const noexcept
    {
        setOrAddAttributeValueByName(attributeList, attribute, *reinterpret_cast<float*>(&value) /* hack, but CSGO does that */);
    }
private:
    void(__thiscall* setOrAddAttributeValueByNameFunction)(std::uintptr_t, const char* attribute);

    static std::uintptr_t findPattern(const wchar_t* module, const char* pattern) noexcept
    {
        static auto id = 0;
        ++id;

        if (HMODULE moduleHandle = GetModuleHandleW(module)) {
            if (MODULEINFO moduleInfo; GetModuleInformation(GetCurrentProcess(), moduleHandle, &moduleInfo, sizeof(moduleInfo))) {
                auto start = static_cast<const char*>(moduleInfo.lpBaseOfDll);
                const auto end = start + moduleInfo.SizeOfImage;

                auto first = start;
                auto second = pattern;

                while (first < end && *second) {
                    if (*first == *second || *second == '?') {
                        ++first;
                        ++second;
                    } else {
                        first = ++start;
                        second = pattern;
                    }
                }

                if (!*second)
                    return reinterpret_cast<std::uintptr_t>(start);
            }
        }
        MessageBoxA(NULL, ("Failed to find pattern #" + std::to_string(id) + '!').c_str(), "Cheat", MB_OK | MB_ICONWARNING);
        return 0;
    }
};

inline std::unique_ptr<const Memory> memory;
