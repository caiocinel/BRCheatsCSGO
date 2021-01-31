#include <algorithm>
#include <cstring>
#include <cwctype>
#include <fstream>
#include <unordered_set>

#define STBI_ONLY_PNG
#define STBI_NO_FAILURE_STRINGS
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include "../imgui/imgui.h"

#include "../imgui/imgui_impl_dx9.h"


#include "../Interfaces.h"

#include "SkinChanger.h"
#include "../Config.h"

#include "../SDK/Client.h"
#include "../SDK/ClientClass.h"
#include "../SDK/ConVar.h"
#include "../SDK/Cvar.h"
#include "../SDK/Engine.h"
#include "../SDK/Entity.h"
#include "../SDK/EntityList.h"
#include "../SDK/FileSystem.h"
#include "../SDK/FrameStage.h"
#include "../SDK/GameEvent.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/Localize.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/WeaponId.h"

#include "../Helpers.h"



item_setting* get_by_definition_index(WeaponId weaponId)
{
    const auto it = std::find_if(config->skinChanger.begin(), config->skinChanger.end(), [weaponId](const item_setting& e) { return e.enabled && e.itemId == weaponId; });
    return it == config->skinChanger.end() ? nullptr : &*it;
}

static constexpr auto is_knife(WeaponId id)
{
    return (id >= WeaponId::Bayonet && id < WeaponId::GloveStuddedBloodhound) || id == WeaponId::KnifeT || id == WeaponId::Knife;
}

static std::wstring toUpperWide(const std::string& s) noexcept
{
    std::wstring upperCase(s.length(), L'\0');
    const auto newLen = mbstowcs(upperCase.data(), s.c_str(), s.length());
    if (newLen != static_cast<std::size_t>(-1))
        upperCase.resize(newLen);
    std::transform(upperCase.begin(), upperCase.end(), upperCase.begin(), [](wchar_t w) { return std::towupper(w); });
    return upperCase;
}

static std::vector<SkinChanger::PaintKit> skinKits{ { 0, "-" } };
static std::vector<SkinChanger::PaintKit> gloveKits;

static void initializeKits() noexcept
{

    static bool initalized = false;
    if (initalized)
        return;
    initalized = true;

    const std::locale original;
    std::locale::global(std::locale{ "en_US.utf8" });

    const auto itemSchema = memory->itemSystem()->getItemSchema();

    struct KitWeapon {
        KitWeapon(int paintKit, WeaponId weaponId, const char* iconPath) noexcept : paintKit{ paintKit }, weaponId{ weaponId }, iconPath{ iconPath } {}
        int paintKit;
        WeaponId weaponId;
        const char* iconPath;
    };

    std::vector<KitWeapon> kitsWeapons;

    for (int i = 0; i < itemSchema->getLootListCount(); ++i) {
        const auto& contents = itemSchema->getLootList(i)->getLootListContents();

        for (int j = 0; j < contents.size; ++j) {
            if (contents[j].paintKit != 0)
                kitsWeapons.emplace_back(contents[j].paintKit, contents[j].weaponId());
        }
    }

    for (int i = 0; i < itemSchema->getItemSetCount(); ++i) {
        const auto set = itemSchema->getItemSet(i);

        for (int j = 0; j < set->getItemCount(); ++j) {
            const auto paintKit = set->getItemPaintKit(j);
            if (paintKit != 0)
                kitsWeapons.emplace_back(paintKit, set->getItemDef(j));
        }
    }

    for (int i = 0; i <= itemSchema->paintKits.lastAlloc; i++) {
        const auto paintKit = itemSchema->paintKits.memory[i].value;

        if (paintKit->id == 0 || paintKit->id == 9001) // ignore workshop_default
            continue;

        std::string name;

        if (const auto it = std::find_if(kitsWeapons.begin(), kitsWeapons.end(), [&paintKit](const auto& p) { return p.first == paintKit->id; }); it != kitsWeapons.end()) {
            name = interfaces->localize->findAsUTF8(itemSchema->getItemDefinitionInterface(it->second)->getItemBaseName());
            name += " | ";
        }
        else if (paintKit->id >= 10000) {
            const std::string_view gloveName{ paintKit->name.data() };

            if (gloveName.starts_with("bloodhound"))
                name = interfaces->localize->findAsUTF8("CSGO_Wearable_t_studdedgloves");
            else if (gloveName.starts_with("motorcycle"))
                name = interfaces->localize->findAsUTF8("CSGO_Wearable_v_motorcycle_glove");
            else if (gloveName.starts_with("slick"))
                name = interfaces->localize->findAsUTF8("CSGO_Wearable_v_slick_glove");
            else if (gloveName.starts_with("sporty"))
                name = interfaces->localize->findAsUTF8("CSGO_Wearable_v_sporty_glove");
            else if (gloveName.starts_with("specialist"))
                name = interfaces->localize->findAsUTF8("CSGO_Wearable_v_specialist_glove");
            else if (gloveName.starts_with("operation10"))
                name = interfaces->localize->findAsUTF8("CSGO_Wearable_t_studded_brokenfang_gloves");
            else if (gloveName.starts_with("handwrap"))
                name = interfaces->localize->findAsUTF8("CSGO_Wearable_v_leather_handwrap");
            else
                assert(false);

            name += " | ";

        name += interfaces->localize->findAsUTF8(paintKit->itemName.data() + 1);

        (paintKit->id < 10000 ? skinKits : gloveKits).emplace_back(paintKit->id, name, toUpperWide(name));
    }

    std::sort(skinKits.begin() + 1, skinKits.end());
    std::sort(gloveKits.begin(), gloveKits.end());

    for (int i = 0; i <= itemSchema->stickerKits.lastAlloc; i++) {
        const auto stickerKit = itemSchema->stickerKits.memory[i].value;
        std::string name = interfaces->localize->findAsUTF8(stickerKit->id != 242 ? stickerKit->itemName.data() + 1 : "StickerKit_dhw2014_teamdignitas_gold");
        stickerKits.emplace_back(stickerKit->id, name, toUpperWide(name));
    }

    std::sort(std::next(stickerKits.begin()), stickerKits.end());
    std::locale::global(original);
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

const std::vector<SkinChanger::Quality>& SkinChanger::getQualities() noexcept
{
    static std::vector<Quality> qualities;
    if (qualities.empty()) {
        for (const auto& node : memory->itemSystem()->getItemSchema()->qualities) {
            const auto quality = node.value;
            if (const auto localizedName = interfaces->localize->findAsUTF8(quality.name); localizedName != quality.name)
                qualities.emplace_back(quality.value, localizedName);
        }

        if (qualities.empty()) // fallback
            qualities.emplace_back(0, "Default");
    }

    return qualities;
}

const std::vector<SkinChanger::Item>& SkinChanger::getGloveTypes() noexcept
{
    static std::vector<SkinChanger::Item> gloveTypes;
    if (gloveTypes.empty()) {
        gloveTypes.emplace_back(WeaponId{}, "Default");

        const auto itemSchema = memory->itemSystem()->getItemSchema();
        for (int i = 0; i <= itemSchema->itemsSorted.lastAlloc; i++) {
            const auto item = itemSchema->itemsSorted.memory[i].value;
            if (std::strcmp(item->getItemTypeName(), "#Type_Hands") == 0)
                gloveTypes.emplace_back(item->getWeaponId(), interfaces->localize->findAsUTF8(item->getItemBaseName()));
        }
    }

    return gloveTypes;
}

const std::vector<SkinChanger::Item>& SkinChanger::getKnifeTypes() noexcept
{
    static std::vector<SkinChanger::Item> knifeTypes;
    if (knifeTypes.empty()) {
        knifeTypes.emplace_back(WeaponId{}, "Default");

        const auto itemSchema = memory->itemSystem()->getItemSchema();
        for (int i = 0; i <= itemSchema->itemsSorted.lastAlloc; i++) {
            const auto item = itemSchema->itemsSorted.memory[i].value;
            if (std::strcmp(item->getItemTypeName(), "#CSGO_Type_Knife") == 0 && item->getRarity() == 6)
                knifeTypes.emplace_back(item->getWeaponId(), interfaces->localize->findAsUTF8(item->getItemBaseName()));
        }
    }

    return knifeTypes;
}

SkinChanger::PaintKit::PaintKit(int id, std::wstring&& name) noexcept : id(id), nameUpperCase(std::move(name))
{
    this->name = interfaces->localize->convertUnicodeToAnsi(nameUpperCase.c_str());
    nameUpperCase = Helpers::toUpper(nameUpperCase);
}

SkinChanger::PaintKit::PaintKit(int id, const std::string& name) noexcept : id(id), name(name)
{
    nameUpperCase = Helpers::toUpper(Helpers::toWideString(name));
}

static int random(int min, int max) noexcept
{
    return rand() % (max - min + 1) + min;
}

static int get_new_animation(const uint32_t model, const int sequence) noexcept
{
    enum Sequence
    {
        SEQUENCE_DEFAULT_DRAW = 0,
        SEQUENCE_DEFAULT_IDLE1 = 1,
        SEQUENCE_DEFAULT_IDLE2 = 2,
        SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
        SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
        SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
        SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
        SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
        SEQUENCE_DEFAULT_LOOKAT01 = 12,

        SEQUENCE_BUTTERFLY_DRAW = 0,
        SEQUENCE_BUTTERFLY_DRAW2 = 1,
        SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
        SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

        SEQUENCE_FALCHION_IDLE1 = 1,
        SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
        SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
        SEQUENCE_FALCHION_LOOKAT01 = 12,
        SEQUENCE_FALCHION_LOOKAT02 = 13,

        SEQUENCE_DAGGERS_IDLE1 = 1,
        SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
        SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
        SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
        SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

        SEQUENCE_BOWIE_IDLE1 = 1,
    };

    // Hashes for best performance.
    switch (model) {
    case fnv::hash("models/weapons/v_knife_butterfly.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_DRAW:
            return random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
        default:
            return sequence + 1;
        }
    }
    case fnv::hash("models/weapons/v_knife_falchion_advanced.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_FALCHION_IDLE1;
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return random(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        default:
            return sequence - 1;
        }
    }
    case fnv::hash("models/weapons/v_knife_push.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_DAGGERS_IDLE1;
        case SEQUENCE_DEFAULT_LIGHT_MISS1:
        case SEQUENCE_DEFAULT_LIGHT_MISS2:
            return random(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
        case SEQUENCE_DEFAULT_HEAVY_MISS1:
            return random(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
        case SEQUENCE_DEFAULT_HEAVY_HIT1:
        case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
        case SEQUENCE_DEFAULT_LOOKAT01:
            return sequence + 3;
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        default:
            return sequence + 2;
        }
    }
    case fnv::hash("models/weapons/v_knife_survival_bowie.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_DRAW:
        case SEQUENCE_DEFAULT_IDLE1:
            return sequence;
        case SEQUENCE_DEFAULT_IDLE2:
            return SEQUENCE_BOWIE_IDLE1;
        default:
            return sequence - 1;
        }
    }
    case fnv::hash("models/weapons/v_knife_ursus.mdl"):
    case fnv::hash("models/weapons/v_knife_skeleton.mdl"):
    case fnv::hash("models/weapons/v_knife_outdoor.mdl"):
    case fnv::hash("models/weapons/v_knife_cord.mdl"):
    case fnv::hash("models/weapons/v_knife_canis.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_DRAW:
            return random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
        default:
            return sequence + 1;
        }
    }
    case fnv::hash("models/weapons/v_knife_stiletto.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(12, 13);
        }
    }
    case fnv::hash("models/weapons/v_knife_widowmaker.mdl"):
    {
        switch (sequence)
        {
        case SEQUENCE_DEFAULT_LOOKAT01:
            return random(14, 15);
        }
    }
    default:
        return sequence;
    }
}

void SkinChanger::fixKnifeAnimation(Entity* viewModelWeapon, long& sequence) noexcept
{
    if (!localPlayer)
        return;

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon)
        return;

    if (!is_knife(viewModelWeapon->itemDefinitionIndex2())))
        return;

    const auto active_conf = get_by_definition_index(WEAPON_KNIFE);
    if (!active_conf || !active_conf->definition_override_index)
        return;

    const auto def = memory->itemSystem()->getItemSchema()->getItemDefinitionInterface(WeaponId(active_conf->definition_override_index));
    if (!def)
        return;

    if (const auto model = def->getPlayerDisplayModel())
        sequence = get_new_animation(fnv::hashRuntime(model), sequence);
}