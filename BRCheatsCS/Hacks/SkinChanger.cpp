#include <algorithm>
#include <cwctype>
#include <fstream>

#include "../Interfaces.h"
#include "../SDK/Entity.h"
#include "SkinChanger.h"
#include "../Config.h"
#include "../SDK/Cvar.h"
#include "../SDK/ConVar.h"

#include "../SDK/Client.h"
#include "../SDK/ClientClass.h"
#include "../SDK/Engine.h"
#include "../SDK/FrameStage.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/Localize.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/EntityList.h"
#include "../SDK/FileSystem.h"
#include "../SDK/Entity.h"
#include "../nSkinz/Utilities/vmt_smart_hook.hpp"
#include "../SDK/GameEvent.h"

#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include "../imgui/imgui.h"

#include "../imgui/imgui_impl_dx9.h"

static std::wstring toUpperWide(const std::string& s) noexcept
{
    std::wstring upperCase(s.length(), L'\0');
    const auto newLen = mbstowcs(upperCase.data(), s.c_str(), s.length());
    if (newLen != static_cast<std::size_t>(-1))
        upperCase.resize(newLen);
    std::transform(upperCase.begin(), upperCase.end(), upperCase.begin(), [](wchar_t w) { return std::towupper(w); });
    return upperCase;
}

static std::vector<SkinChanger::PaintKit> skinKits{ { 0, "-", L"-" } };
static std::vector<SkinChanger::PaintKit> gloveKits;
static std::vector<SkinChanger::PaintKit> stickerKits{ { 0, "None", L"NONE" } };

static void initializeKits() noexcept
{
    static bool initalized = false;
    if (initalized)
        return;
    initalized = true;

    const auto itemSchema = memory->itemSystem()->getItemSchema();

    struct KitWeapon {
        KitWeapon(int paintKit, WeaponId weaponId, const char* iconPath) noexcept : paintKit{ paintKit }, weaponId{ weaponId }, iconPath{ iconPath } {}
        int paintKit;
        WeaponId weaponId;
        const char* iconPath;
    };

    std::vector<KitWeapon> kitsWeapons;
    kitsWeapons.reserve(itemSchema->alternateIcons.numElements);

    for (const auto& node : itemSchema->alternateIcons) {
        // https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/shared/econ/econ_item_schema.cpp#L325-L329
        if (const auto encoded = node.key; (encoded & 3) == 0)
            kitsWeapons.emplace_back(int((encoded & 0xFFFF) >> 2), WeaponId(encoded >> 16), node.value.simpleName.data());
    }

    std::sort(kitsWeapons.begin(), kitsWeapons.end(), [](const auto& a, const auto& b) { return a.paintKit < b.paintKit; });

    skinKits.reserve(itemSchema->paintKits.lastAlloc);
    gloveKits.reserve(itemSchema->paintKits.lastAlloc);
    for (const auto& node : itemSchema->paintKits) {
        const auto paintKit = node.value;

        if (paintKit->id == 0 || paintKit->id == 9001) // ignore workshop_default
            continue;

        if (paintKit->id >= 10000) {
            std::wstring name;
            std::string iconPath;

            if (const auto it = std::lower_bound(kitsWeapons.begin(), kitsWeapons.end(), paintKit->id, [](const auto& p, auto id) { return p.paintKit < id; }); it != kitsWeapons.end() && it->paintKit == paintKit->id) {
                if (const auto itemDef = itemSchema->getItemDefinitionInterface(it->weaponId)) {
                    name = interfaces->localize->findSafe(itemDef->getItemBaseName());
                    name += L" | ";
                }
                iconPath = it->iconPath;
            }

            name += interfaces->localize->findSafe(paintKit->itemName.data() + 1);
            gloveKits.emplace_back(paintKit->id, std::move(name), std::move(iconPath), paintKit->rarity);
        }
        else {
            for (auto it = std::lower_bound(kitsWeapons.begin(), kitsWeapons.end(), paintKit->id, [](const auto& p, auto id) { return p.paintKit < id; }); it != kitsWeapons.end() && it->paintKit == paintKit->id; ++it) {

                const auto itemDef = itemSchema->getItemDefinitionInterface(it->weaponId);
                if (!itemDef)
                    continue;

                std::wstring name = interfaces->localize->findSafe(itemDef->getItemBaseName());
                name += L" | ";
                name += interfaces->localize->findSafe(paintKit->itemName.data() + 1);
                skinKits.emplace_back(paintKit->id, std::move(name), it->iconPath, std::clamp(itemDef->getRarity() + paintKit->rarity - 1, 0, (paintKit->rarity == 7) ? 7 : 6));
            }
        }
    }

    std::sort(skinKits.begin() + 1, skinKits.end());
    skinKits.shrink_to_fit();
    std::sort(gloveKits.begin(), gloveKits.end());
    gloveKits.shrink_to_fit();
}

static std::unordered_map<std::string, const char*> iconOverrides;

enum class StickerAttribute {
    Index,
    Wear,
    Scale,
    Rotation
};

static auto s_econ_item_interface_wrapper_offset = std::uint16_t(0);

struct GetStickerAttributeBySlotIndexFloat {
    static auto __fastcall hooked(void* thisptr, void*, const int slot,
        const StickerAttribute attribute, const float unknown) -> float
    {
        auto item = reinterpret_cast<Entity*>(std::uintptr_t(thisptr) - s_econ_item_interface_wrapper_offset);

        const auto defindex = item->itemDefinitionIndex();

        auto config = get_by_definition_index(defindex);

        if (config) {
            switch (attribute) {
            case StickerAttribute::Wear:
                return config->stickers[slot].wear;
            case StickerAttribute::Scale:
                return config->stickers[slot].scale;
            case StickerAttribute::Rotation:
                return config->stickers[slot].rotation;
            default:
                break;
            }
        }
        return m_original(thisptr, nullptr, slot, attribute, unknown);
    }

    inline static decltype(&hooked) m_original;
};

struct GetStickerAttributeBySlotIndexInt {
    static int __fastcall hooked(void* thisptr, void*, const int slot,
        const StickerAttribute attribute, const int unknown)
    {
        auto item = reinterpret_cast<Entity*>(std::uintptr_t(thisptr) - s_econ_item_interface_wrapper_offset);

        if (attribute == StickerAttribute::Index)
            if (auto config = get_by_definition_index(item->itemDefinitionIndex()))
                return config->stickers[slot].kit;
        return m_original(thisptr, nullptr, slot, attribute, unknown);
    }

    inline static decltype(&hooked) m_original;
};

void apply_sticker_changer(Entity* item) noexcept
{
    if (constexpr auto hash{ fnv::hash("CBaseAttributableItem->m_Item") }; !s_econ_item_interface_wrapper_offset)
        s_econ_item_interface_wrapper_offset = netvars->operator[](hash) + 0xC;

    static vmt_multi_hook hook;

    const auto econ_item_interface_wrapper = std::uintptr_t(item) + s_econ_item_interface_wrapper_offset;

    if (hook.initialize_and_hook_instance(reinterpret_cast<void*>(econ_item_interface_wrapper))) {
        hook.apply_hook<GetStickerAttributeBySlotIndexFloat>(4);
        hook.apply_hook<GetStickerAttributeBySlotIndexInt>(5);
    }
}

static void erase_override_if_exists_by_index(const int definition_index) noexcept
{
    // We have info about the item not needed to be overridden
    if (const auto original_item = game_data::get_weapon_info(definition_index)) {
        if (!original_item->icon)
            return;

        // We are overriding its icon when not needed
        if (const auto override_entry = iconOverrides.find(original_item->icon); override_entry != end(iconOverrides))
            iconOverrides.erase(override_entry); // Remove the leftover override
    }
}

static void apply_config_on_attributable_item(Entity* item, const item_setting* config,
    const unsigned xuid_low) noexcept
{
    // Force fallback values to be used.
    item->itemIDHigh() = -1;

    // Set the owner of the weapon to our lower XUID. (fixes StatTrak)
    item->accountID() = xuid_low;
    item->entityQuality() = config->quality;

    if (config->stat_trak > -1) {
        item->fallbackStatTrak() = config->stat_trak;
        item->entityQuality() = 9;
    }

    if (is_knife(item->itemDefinitionIndex2()))
        item->entityQuality() = 3; // make a star appear on knife

    if (config->custom_name[0])
        strcpy_s(item->customName(), config->custom_name);

    if (config->paintKit)
        item->fallbackPaintKit() = config->paintKit;

    if (config->seed)
        item->fallbackSeed() = config->seed;

    item->fallbackWear() = config->wear;

    auto& definition_index = item->itemDefinitionIndex();

    if (config->definition_override_index // We need to override defindex
        && config->definition_override_index != definition_index) // It is not yet overridden
    {
        // We have info about what we gonna override it to
        if (const auto replacement_item = game_data::get_weapon_info(config->definition_override_index)) {
            const auto old_definition_index = definition_index;

            definition_index = config->definition_override_index;

            // Set the weapon model index -- required for paint kits to work on replacement items after the 29/11/2016 update.
            //item->GetModelIndex() = g_model_info->GetModelIndex(k_weapon_info.at(config->definition_override_index).model);
            item->setModelIndex(interfaces->modelInfo->getModelIndex(replacement_item->model));
            item->preDataUpdate(0);

            // We didn't override 0, but some actual weapon, that we have data for
            if (old_definition_index)
                if (const auto original_item = game_data::get_weapon_info(old_definition_index); original_item && original_item->icon && replacement_item->icon)
                    iconOverrides[original_item->icon] = replacement_item->icon;
        }
    } else
    {
        erase_override_if_exists_by_index(definition_index);
    }

    apply_sticker_changer(item);
}

static Entity* make_glove(int entry, int serial) noexcept
{
    static std::add_pointer_t<Entity* __cdecl(int, int)> createWearable = nullptr;

    if (!createWearable) {
        createWearable = []() -> decltype(createWearable) {
            for (auto clientClass = interfaces->client->getAllClasses(); clientClass; clientClass = clientClass->next)
                if (clientClass->classId == ClassId::EconWearable)
                    return clientClass->createFunction;
            return nullptr;
        }();
    }

    if (!createWearable)
        return nullptr;

    createWearable(entry, serial);
    return interfaces->entityList->getEntity(entry);
}

static void post_data_update_start(int localHandle) noexcept
{
    const auto local = interfaces->entityList->getEntityFromHandle(localHandle);
    if (!local)
        return;

    const auto local_index = local->index();

    if (!local->isAlive())
        return;

    PlayerInfo player_info;
    if (!interfaces->engine->getPlayerInfo(local_index, player_info))
        return;

    // Handle glove config
    {
        const auto wearables = local->wearables();

        const auto glove_config = get_by_definition_index(GLOVE_T_SIDE);

        static int glove_handle;

        auto glove = interfaces->entityList->getEntityFromHandle(wearables[0]);

        if (!glove) // There is no glove
        {
            // Try to get our last created glove
            const auto our_glove = interfaces->entityList->getEntityFromHandle(glove_handle);

            if (our_glove) // Our glove still exists
            {
                wearables[0] = glove_handle;
                glove = our_glove;
            }
        }

        if (glove_config && glove_config->definition_override_index)
        {
            // We don't have a glove, but we should
            if (!glove)
            {
                auto entry = interfaces->entityList->getHighestEntityIndex() + 1;
#define HIJACK_ENTITY 1
#if HIJACK_ENTITY == 1
                for (int i = 65; i <= interfaces->entityList->getHighestEntityIndex(); i++) {
                    auto entity = interfaces->entityList->getEntity(i);

                    if (entity && entity->getClientClass()->classId == ClassId{ 70 }) {
                        entry = i;
                        break;
                    }
                }
#endif
                const auto serial = rand() % 0x1000;

                glove = make_glove(entry, serial);
                if (glove) {
                    glove->initialized() = true;

                    wearables[0] = entry | serial << 16;

                    // Let's store it in case we somehow lose it.
                    glove_handle = wearables[0];
                }
            }

            if (glove) {
                memory->equipWearable(glove, local);
                local->body() = 1;

                apply_config_on_attributable_item(glove, glove_config, player_info.xuidLow);
            }
        }
    }

    // Handle weapon configs
    {
        auto& weapons = local->weapons();

        for (auto weapon_handle : weapons) {
            if (weapon_handle == -1)
                break;

            auto weapon = interfaces->entityList->getEntityFromHandle(weapon_handle);

            if (!weapon)
                continue;

            auto& definition_index = weapon->itemDefinitionIndex();

            // All knives are terrorist knives.
            if (const auto active_conf = get_by_definition_index(is_knife(weapon->itemDefinitionIndex2()) ? WEAPON_KNIFE : definition_index))
                apply_config_on_attributable_item(weapon, active_conf, player_info.xuidLow);
            else
                erase_override_if_exists_by_index(definition_index);
        }
    }

    const auto view_model = interfaces->entityList->getEntityFromHandle(local->viewModel());

    if (!view_model)
        return;

    const auto view_model_weapon = interfaces->entityList->getEntityFromHandle(view_model->weapon());

    if (!view_model_weapon)
        return;

    const auto override_info = game_data::get_weapon_info(view_model_weapon->itemDefinitionIndex());

    if (!override_info)
        return;

    const auto override_model_index = interfaces->modelInfo->getModelIndex(override_info->model);
    view_model->modelIndex() = override_model_index;

    const auto world_model = interfaces->entityList->getEntityFromHandle(view_model_weapon->weaponWorldModel());

    if (!world_model)
        return;

    world_model->modelIndex() = override_model_index + 1;
}

static bool hudUpdateRequired{ false };

static void updateHud() noexcept
{
    if (auto hud_weapons = memory->findHudElement(memory->hud, "CCSGO_HudWeaponSelection") - 0x28) {
        for (int i = 0; i < *(hud_weapons + 0x20); i++)
            i = memory->clearHudWeapon(hud_weapons, i);
    }
    hudUpdateRequired = false;
}

void SkinChanger::run(FrameStage stage) noexcept
{
    static int localPlayerHandle = -1;

    if (localPlayer)
        localPlayerHandle = localPlayer->handle();

    if (stage == FrameStage::NET_UPDATE_POSTDATAUPDATE_START) {
        post_data_update_start(localPlayerHandle);
        if (hudUpdateRequired && localPlayer && !localPlayer->isDormant())
            updateHud();
    }
}

void SkinChanger::scheduleHudUpdate() noexcept
{
    interfaces->cvar->findVar("cl_fullupdate")->changeCallback();
    hudUpdateRequired = true;
}

void SkinChanger::overrideHudIcon(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (event.getInt("attacker") != localPlayer->getUserId())
        return;

    if (const auto iconOverride = iconOverrides[event.getString("weapon")])
        event.setString("weapon", iconOverride);
}

void SkinChanger::updateStatTrak(GameEvent& event) noexcept
{
    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    const auto weapon = localPlayer->getActiveWeapon();
    if (!weapon)
        return;

    if (const auto conf = get_by_definition_index(is_knife(weapon->itemDefinitionIndex2()) ? WEAPON_KNIFE : weapon->itemDefinitionIndex()); conf && conf->stat_trak > -1) {
        weapon->fallbackStatTrak() = ++conf->stat_trak;
        weapon->postDataUpdate(0);
    }
}

const std::vector<SkinChanger::PaintKit>& SkinChanger::getSkinKits() noexcept
{
    return skinKits;
}

const std::vector<SkinChanger::PaintKit>& SkinChanger::getGloveKits() noexcept
{
    return gloveKits;
}

const std::vector<SkinChanger::PaintKit>& SkinChanger::getStickerKits() noexcept
{
    return stickerKits;
}

class Texture {
    ImTextureID texture = nullptr;
public:
    Texture() = default;
    ~Texture() { clear(); }
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept : texture{ other.texture } { other.texture = nullptr; }
    Texture& operator=(Texture&& other) noexcept { clear(); texture = other.texture; other.texture = nullptr; return *this; }

    void init(int width, int height, const std::uint8_t* data) noexcept;
    void clear() noexcept;
    ImTextureID get() noexcept { return texture; }
};

void Texture::init(int width, int height, const std::uint8_t* data) noexcept
{
    texture = ImGui_CreateTextureRGBA(width, height, data);
}

void Texture::clear() noexcept
{
    if (texture)
        ImGui_DestroyTexture(texture);
    texture = nullptr;
}

static std::unordered_map<std::string, Texture> iconTextures;

ImTextureID SkinChanger::getItemIconTexture(const std::string& iconpath) noexcept
{
    if (iconpath.empty())
        return 0;

    if (iconTextures[iconpath].get())
        return iconTextures[iconpath].get();

    if (iconTextures.size() >= 50)
        iconTextures.erase(iconTextures.begin());

    if (const auto handle = interfaces->baseFileSystem->open(("resource/flash/" + iconpath + "_large.png").c_str(), "r", "GAME")) {
        if (const auto size = interfaces->baseFileSystem->size(handle); size >= 0) {
            const auto buffer = std::make_unique<std::uint8_t[]>(size);
            if (interfaces->baseFileSystem->read(buffer.get(), size, handle) > 0) {
                int width, height;
                stbi_set_flip_vertically_on_load_thread(false);

                if (const auto data = stbi_load_from_memory((const stbi_uc*)buffer.get(), size, &width, &height, nullptr, STBI_rgb_alpha)) {
                    iconTextures[iconpath].init(width, height, data);
                    stbi_image_free(data);
                }
                else {
                    assert(false);
                }
            }
        }
        interfaces->baseFileSystem->close(handle);
    }
    else {
        assert(false);
    }

    return iconTextures[iconpath].get();
}

void SkinChanger::clearItemIconTextures() noexcept
{
    iconTextures.clear();
}