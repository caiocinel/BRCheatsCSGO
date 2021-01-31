#pragma once

#include <array>
#include <string>
#include <vector>
#include "../imgui/imgui.h"
#include "../nSkinz/item_definitions.hpp"
#include "../SDK/WeaponId.h"

enum class FrameStage;
class GameEvent;
class Entity;

namespace SkinChanger
{
    void run(FrameStage) noexcept;
    void scheduleHudUpdate() noexcept;
    void overrideHudIcon(GameEvent& event) noexcept;
    void updateStatTrak(GameEvent& event) noexcept;

    struct PaintKit {
        PaintKit(int id, const std::string& name, int rarity = 0) noexcept;
        PaintKit(int id, std::string&& name, int rarity = 0) noexcept;
        PaintKit(int id, std::wstring&& name, int rarity = 0) noexcept;

        int id;
        int rarity;
        std::string name;
        std::wstring nameUpperCase;
        std::string iconPath;

        auto operator<(const PaintKit& other) const noexcept
        {
            return nameUpperCase < other.nameUpperCase;
        }
    };

    struct Quality {
        Quality(int index, const char* name) : index{ index }, name{ name } {}
        int index = 0;
        std::string name;
    };

    struct Item {
        Item(WeaponId id, const char* name) : id(id), name(name) {}

        WeaponId id;
        std::string name;
    };

    const std::vector<PaintKit>& getSkinKits() noexcept;
    const std::vector<PaintKit>& getGloveKits() noexcept;
    const std::vector<PaintKit>& getStickerKits() noexcept;
    const std::vector<Quality >& getQualities() noexcept;
    const std::vector<Item>& getGloveTypes() noexcept;
    const std::vector<Item>& getKnifeTypes() noexcept;
    void fixKnifeAnimation(Entity* viewModelWeapon, long& sequence) noexcept;



}
struct sticker_setting
{

    void onLoad()
    {
        const auto& kits = SkinChanger::getStickerKits();
        const auto it = std::find_if(kits.begin(), kits.end(), [this](const auto& k) { return k.id == kit; });
        kit_vector_index = it != SkinChanger::getStickerKits().end() ? std::distance(kits.begin(), it) : 0;
        kit = SkinChanger::getStickerKits()[kit_vector_index].id;
    }

    void update()
    {
        kit = SkinChanger::getStickerKits()[kit_vector_index].id;
    }

    auto operator==(const sticker_setting& o) const
    {
        return kit == o.kit
            && kit_vector_index == o.kit_vector_index
            && wear == o.wear
            && scale == o.scale
            && rotation == o.rotation;
    }

    int kit = 0;
    int kit_vector_index = 0;
    float wear = (std::numeric_limits<float>::min)();
    float scale = 1.f;
    float rotation = 0.f;
};


struct item_setting {
    void update()
    {
        itemId = static_cast<int>(game_data::weapon_names[itemIdIndex].definition_index);
        quality = SkinChanger::getQualities()[entity_quality_vector_index].index;

        if (itemId == GLOVE_T_SIDE) {
            paintKit = SkinChanger::getGloveKits()[paint_kit_vector_index].id;
            definition_override_index = (int)SkinChanger::getGloveTypes()[definition_override_vector_index].id;
        }
        else {
            paintKit = SkinChanger::getSkinKits()[paint_kit_vector_index].id;
            definition_override_index = (int)SkinChanger::getKnifeTypes()[definition_override_vector_index].id;
        }

        for (auto& sticker : stickers)
            sticker.update();
    }

    void onLoad()
    {
        {
            const auto it = std::find_if(std::begin(game_data::weapon_names), std::end(game_data::weapon_names), [this](const auto& k) { return static_cast<int>(k.definition_index) == itemId; });
            itemIdIndex = it != std::end(game_data::weapon_names) ? std::distance(std::begin(game_data::weapon_names), it) : 0;
        }

        {
            const auto& qualities = SkinChanger::getQualities();
            const auto it = std::find_if(qualities.begin(), qualities.end(), [this](const auto& k) { return k.index == quality; });
            entity_quality_vector_index = it != qualities.end() ? std::distance(qualities.begin(), it) : 0;
        }

        if (itemId == GLOVE_T_SIDE) {
            {
                const auto it = std::find_if(SkinChanger::getGloveKits().begin(), SkinChanger::getGloveKits().end(), [this](const auto& k) { return k.id == paintKit; });
                paint_kit_vector_index = it != SkinChanger::getGloveKits().end() ? std::distance(SkinChanger::getGloveKits().begin(), it) : 0;
            }

            {
                const auto it = std::find_if(SkinChanger::getGloveTypes().begin(), SkinChanger::getGloveTypes().end(), [this](const auto& k) { return (int)k.id == definition_override_index; });
                definition_override_vector_index = it != SkinChanger::getGloveTypes().end() ? std::distance(SkinChanger::getGloveTypes().begin(), it) : 0;
            }
        }
        else {
            {
                const auto it = std::find_if(SkinChanger::getSkinKits().begin(), SkinChanger::getSkinKits().end(), [this](const auto& k) { return k.id == paintKit; });
                paint_kit_vector_index = it != SkinChanger::getSkinKits().end() ? std::distance(SkinChanger::getSkinKits().begin(), it) : 0;
            }

            {
                const auto it = std::find_if(SkinChanger::getKnifeTypes().begin(), SkinChanger::getKnifeTypes().end(), [this](const auto& k) { return (int)k.id == definition_override_index; });
                definition_override_vector_index = it != SkinChanger::getKnifeTypes().end() ? std::distance(SkinChanger::getKnifeTypes().begin(), it) : 0;
            }
        }

        for (auto& sticker : stickers)
            sticker.onLoad();
    }

    bool enabled = false;
    int itemIdIndex = 0;
    int itemId = 1;
    int entity_quality_vector_index = 0;
    int quality = 0;
    int paint_kit_vector_index = 0;
    int paintKit = 0;
    int definition_override_vector_index = 0;
    int definition_override_index = 0;
    int seed = 0;
    int stat_trak = -1;
    float wear = (std::numeric_limits<float>::min)();
    char custom_name[32] = "";
    std::array<sticker_setting, 5> stickers;
};

item_setting* get_by_definition_index(WeaponId weaponId);