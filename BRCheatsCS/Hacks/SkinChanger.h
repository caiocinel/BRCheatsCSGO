#pragma once

#include <string>
#include <vector>
#include "../imgui/imgui.h"

enum class FrameStage;
class GameEvent;

namespace SkinChanger
{
    void initializeKits() noexcept;
    void run(FrameStage) noexcept;
    void scheduleHudUpdate() noexcept;
    void overrideHudIcon(GameEvent& event) noexcept;
    void updateStatTrak(GameEvent& event) noexcept;

    struct PaintKit {
        PaintKit(int id, const std::string& name, const std::wstring& nameUpperCase) noexcept : id(id), name(name), nameUpperCase(nameUpperCase) { }
        PaintKit(int id, std::wstring&& name, std::string&& iconPath, int rarity = 0) noexcept;
        int id;
        std::string name;
        std::wstring nameUpperCase;
        std::string iconPath;

        auto operator<(const PaintKit& other) const noexcept
        {
            return nameUpperCase < other.nameUpperCase;
        }
    };

    const std::vector<PaintKit>& getSkinKits() noexcept;
    const std::vector<PaintKit>& getGloveKits() noexcept;
    const std::vector<PaintKit>& getStickerKits() noexcept;

    ImTextureID getItemIconTexture(const std::string& iconpath) noexcept;
    void clearItemIconTextures() noexcept;

}
