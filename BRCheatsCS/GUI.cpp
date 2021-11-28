#include <fstream>
#include <functional>
#include <string>
#include <ShlObj.h>
#include <Windows.h>
#include <cstring>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_stdlib.h"
#include "imgui/imgui_internal.h"

#include "imguiCustom.h"

#include "GUI.h"
#include "XorStr/xorstr.hpp"
#include "Lang.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/SkinChanger.h"
#include "Helpers.h"
#include "Hooks.h"
#include "Interfaces.h"
#include "SDK/InputSystem.h"
#include "Changer/Changer.h"
#include "Memory.h"
#include "Fonts/IconsFontAwesome5.h"
#include "Fonts/icons.cpp"
#include "Fonts/font.cpp"

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
extern std::map<std::string, std::string> phrases;
IDirect3DTexture9 *skinImage = nullptr;
const char* ezConfigPage = "";

void RightText()
{
    std::string text = "xxxxxxxxx";
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(text.c_str()).x - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x / 2);
}

namespace ImGui
{
    void ImGuiStructItem(ImGuiStruct i) noexcept
    {
        Checkbox(phrases[XorString("global_enabled")].c_str(), &i.enabled);
        SameLine();
        if (Button(phrases[XorString("global_threedots")].c_str()))
            OpenPopup("");

        if (BeginPopup(""))
        {
            Checkbox(phrases[XorString("global_nobackground")].c_str(), &i.noBackGround);
            Checkbox(phrases[XorString("global_notitle")].c_str(), &i.noTittleBar);
            ImGui::EndPopup();
        }
    }
}

GUI::GUI() noexcept
{
    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();

    ImGui::GetStyle().WindowRounding = 4.0f;
    ImGui::GetStyle().ChildRounding = 0.0f;
    ImGui::GetStyle().FrameRounding = 0.0f;
    ImGui::GetStyle().GrabRounding = 0.0f;
    ImGui::GetStyle().PopupRounding = 0.0f;
    ImGui::GetStyle().ScrollbarRounding = 0.0f;
    ImGui::GetStyle().TabRounding = 0.0f;
    ImGui::GetStyle().ScrollbarSize = 9.0f;

    // Style
    style.WindowRounding = 4.f;
    style.FrameRounding = 1.f;
    style.WindowTitleAlign.x = 0.50f;
    style.WindowTitleAlign.y = 0.50f;
    style.GrabRounding = 3.f;
    style.GrabMinSize = 15.f;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.15f, 0.98f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.07f, 0.14f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.15f, 0.20f, 0.69f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.41f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.20f, 0.31f, 0.96f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.15f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.15f, 0.20f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_Button] = ImVec4(0.14f, 0.15f, 0.20f, 0.96f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_Header] = ImVec4(0.24f, 0.20f, 0.31f, 0.54f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_Separator] = ImVec4(0.14f, 0.15f, 0.20f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.14f, 0.15f, 0.20f, 0.96f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.14f, 0.15f, 0.20f, 0.96f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.24f, 0.20f, 0.31f, 0.54f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.24f, 0.20f, 0.31f, 0.54f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.15f, 0.20f, 0.96f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.10f, 0.11f, 0.15f, 0.96f);
    colors[ImGuiCol_TabActive] = ImVec4(0.14f, 0.15f, 0.20f, 0.50f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.15f, 0.20f, 0.69f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.15f, 0.13f, 0.19f, 0.96f);

    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    ImFontConfig icons_config;
    ImFontConfig font_config;
    icons_config.MergeMode = true;
    io.Fonts->AddFontFromMemoryCompressedTTF(font_data, font_size, 16.0f, &font_config, Helpers::getFontGlyphRanges());
    io.Fonts->AddFontFromMemoryCompressedTTF(icons_data, icons_size, 16.0f, &icons_config, icons_ranges);
    io.Fonts->Build();
    if (PWSTR pathToFonts; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &pathToFonts)))
    {
        const std::filesystem::path path{pathToFonts};
        CoTaskMemFree(pathToFonts);

        static constexpr ImWchar ranges[]{0x0020, 0xFFFF, 0};
        ImFontConfig cfg;
        cfg.OversampleV = 3;

        fonts.tahoma = io.Fonts->AddFontFromFileTTF((path / XorString("tahoma.ttf")).string().c_str(), 15.0f, &cfg, Helpers::getFontGlyphRanges());
        fonts.arial = io.Fonts->AddFontFromFileTTF((path / "arial.ttf").string().c_str(), 15.0f, &cfg, ranges);
        fonts.segoeui = io.Fonts->AddFontFromFileTTF((path / XorString("segoeui.ttf")).string().c_str(), 15.0f, &cfg, Helpers::getFontGlyphRanges());
        fonts.segoeuiSized = io.Fonts->AddFontFromFileTTF((path / XorString("segoeui.ttf")).string().c_str(), 25.0f, &cfg, Helpers::getFontGlyphRanges());
        fonts.iconsSized = io.Fonts->AddFontFromMemoryCompressedTTF(icons_data, icons_size, 25.0f, &icons_config, icons_ranges);
    }
}

void GUI::render() noexcept
{
    UpdateLanguage();
    renderGuiStyle2();
    renderAimbotWindow();
    renderRagebotWindow();
    renderTriggerbotWindow();
    renderBacktrackWindow();
    renderGlowWindow();
    renderChamsWindow();
    renderStreamProofESPWindow();
    renderVisualsWindow();
    renderWorldWindow();
    renderSkinChangerWindow();
    renderProfileChangerWindow();
    renderPerformanceWindow();
    renderNickWindow();
    //drawDemo();
    ///*renderMedalChangerWindow();
    //renderInventoryChangerWindow();
    //*/
    renderMiscWindow();
    renderAutoConfigWindow();
    renderEzConfigModal();
    //  renderWarningWindow();
    renderConfigWindow();
}

void GUI::updateColors() const noexcept
{
    switch (config->style.menuColors)
    {
    case 0:
        ImGui::StyleColorsDark();
        break;
    case 1:
        ImGui::StyleColorsLight();
        break;
    case 2:
        ImGui::StyleColorsClassic();
        break;
    }
}

void DisableItems()
{
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
};

void PopDisableItems()
{
    ImGui::PopItemFlag();
    ImGui::PopStyleVar();
};

void GUI::hotkey(int &key) noexcept
{
    key ? ImGui::Text(XorString("[ %s ]"), interfaces->inputSystem->virtualKeyToString(key)) : ImGui::TextUnformatted(phrases[XorString("global_key")].c_str());

    if (!ImGui::IsItemHovered())
        return;

    ImGui::SetTooltip(phrases[XorString("global_keypopup")].c_str());
    ImGuiIO &io = ImGui::GetIO();
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
        if (ImGui::IsKeyPressed(i) && i != config->misc.menuKey)
            key = i != VK_ESCAPE ? i : 0;

    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
        if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != config->misc.menuKey)
            key = i + (i > 1 ? 2 : 1);
}

void GUI::renderAimbotWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.aimbot)
            return;
        ImGui::SetNextWindowSize({600.0f, 0.0f});
        ImGui::Begin(phrases[XorString("window_aimbot")].c_str(), &window.aimbot, windowFlags);
    }
    static int currentCategory{0};
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, XorString("All\0Pistols\0Heavy\0SMG\0Rifles\0"));
    ImGui::PopID();
    ImGui::SameLine();
    static int currentWeapon{0};
    ImGui::PushID(1);

    switch (currentCategory)
    {
    case 0:
        currentWeapon = 0;
        ImGui::NewLine();
        break;
    case 1:
    {
        static int currentPistol{0};
        static constexpr const char *pistols[]{"All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver"};

        ImGui::Combo(
            "", &currentPistol, [](void *data, int idx, const char **out_text)
            {
                if (config->aimbot[idx ? idx : 35].enabled)
                {
                    static std::string name;
                    name = pistols[idx];
                    *out_text = name.append(" *").c_str();
                }
                else
                {
                    *out_text = pistols[idx];
                }
                return true;
            },
            nullptr, IM_ARRAYSIZE(pistols));

        currentWeapon = currentPistol ? currentPistol : 35;
        break;
    }
    case 2:
    {
        static int currentHeavy{0};
        static constexpr const char *heavies[]{"All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev"};

        ImGui::Combo(
            "", &currentHeavy, [](void *data, int idx, const char **out_text)
            {
                if (config->aimbot[idx ? idx + 10 : 36].enabled)
                {
                    static std::string name;
                    name = heavies[idx];
                    *out_text = name.append(" *").c_str();
                }
                else
                {
                    *out_text = heavies[idx];
                }
                return true;
            },
            nullptr, IM_ARRAYSIZE(heavies));

        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
        break;
    }
    case 3:
    {
        static int currentSmg{0};
        static constexpr const char *smgs[]{"All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon"};

        ImGui::Combo(
            "", &currentSmg, [](void *data, int idx, const char **out_text)
            {
                if (config->aimbot[idx ? idx + 16 : 37].enabled)
                {
                    static std::string name;
                    name = smgs[idx];
                    *out_text = name.append(" *").c_str();
                }
                else
                {
                    *out_text = smgs[idx];
                }
                return true;
            },
            nullptr, IM_ARRAYSIZE(smgs));

        currentWeapon = currentSmg ? currentSmg + 16 : 37;
        break;
    }
    case 4:
    {
        static int currentRifle{0};
        static constexpr const char *rifles[]{"All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20"};

        ImGui::Combo(
            "", &currentRifle, [](void *data, int idx, const char **out_text)
            {
                if (config->aimbot[idx ? idx + 23 : 38].enabled)
                {
                    static std::string name;
                    name = rifles[idx];
                    *out_text = name.append(XorString(" *")).c_str();
                }
                else
                {
                    *out_text = rifles[idx];
                }
                return true;
            },
            nullptr, IM_ARRAYSIZE(rifles));

        currentWeapon = currentRifle ? currentRifle + 23 : 38;
        break;
    }
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &config->aimbot[currentWeapon].enabled);
    ImGui::SameLine();
    ImGui::Text(XorString("  |  "));
    ImGui::SameLine();
    ImGui::Checkbox(phrases[XorString("global_onkey")].c_str(), &config->aimbot[currentWeapon].onKey);
    ImGui::SameLine();
    hotkey(config->aimbot[currentWeapon].key);
    ImGui::SameLine();
    ImGui::PushID(2);
    ImGui::PushItemWidth(70.0f);
    ImGui::Combo("", &config->aimbot[currentWeapon].keyMode, XorString("Hold\0Toggle\0"));
    ImGui::PopItemWidth();
    ImGui::PopID();
    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 220.0f);
    ImGui::Checkbox(phrases[XorString("aimhacks_silent")].c_str(), &config->aimbot[currentWeapon].silent);
    ImGui::Checkbox(phrases[XorString("aimhacks_friendlyfire")].c_str(), &config->aimbot[currentWeapon].friendlyFire);
    ImGui::Checkbox(phrases[XorString("aimhacks_visibleonly")].c_str(), &config->aimbot[currentWeapon].visibleOnly);
    ImGui::Checkbox(phrases[XorString("aimhacks_scopedonly")].c_str(), &config->aimbot[currentWeapon].scopedOnly);
    ImGui::Checkbox(phrases[XorString("aimhacks_ignoreflash")].c_str(), &config->aimbot[currentWeapon].ignoreFlash);
    ImGui::Checkbox(phrases[XorString("aimhacks_ignoresmoke")].c_str(), &config->aimbot[currentWeapon].ignoreSmoke);
    ImGui::Checkbox(phrases[XorString("aimhacks_autoshot")].c_str(), &config->aimbot[currentWeapon].autoShot);
    ImGui::Checkbox(phrases[XorString("aimhacks_autoscope")].c_str(), &config->aimbot[currentWeapon].autoScope);
    ImGui::Checkbox(phrases[XorString("aimhacks_killshot")].c_str(), &config->aimbot[currentWeapon].killshot);
    config->aimbot[currentWeapon].shotsFired = std::clamp(config->aimbot[currentWeapon].shotsFired, 0, 150);
    ImGui::Checkbox(phrases[XorString("aimhacks_betweenshots")].c_str(), &config->aimbot[currentWeapon].betweenShots);
    ImGui::NextColumn();
    ImGui::Combo(phrases[XorString("global_bone")].c_str(), &config->aimbot[currentWeapon].bone, XorString("Nearest\0Best damage\0Head\0Neck\0Sternum\0Chest\0Stomach\0Pelvis\0"));
    ImGui::PushItemWidth(200.0f);
    ImGui::SliderFloat(phrases[XorString("aimbot_fov")].c_str(), &config->aimbot[currentWeapon].fov, 0.0f, 255.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
    ImGui::SameLine();
    ImGui::PushID(0);
    ImGui::Checkbox(phrases[XorString("aimbot_drawfov")].c_str(), &config->aimbot[currentWeapon].drawFov);
    ImGui::PopID();
    ImGui::SliderFloat(phrases[XorString("aimbot_smooth")].c_str(), &config->aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
    ImGui::SliderInt(phrases[XorString("aimhacks_hitchance")].c_str(), &config->aimbot[currentWeapon].hitchance, 0, 100, "%d");
    ImGui::InputInt(phrases[XorString("aimhacks_mindamage")].c_str(), &config->aimbot[currentWeapon].minDamage);
    config->aimbot[currentWeapon].minDamage = std::clamp(config->aimbot[currentWeapon].minDamage, 0, 250);
    if (config->aimbot[currentWeapon].silent)
        DisableItems();

    ImGui::Checkbox(phrases[XorString("aimbot_rcs")].c_str(), &config->aimbot[currentWeapon].standaloneRCS);
    if (config->aimbot[currentWeapon].silent)
        PopDisableItems();

    if (config->aimbot[currentWeapon].standaloneRCS && !config->aimbot[currentWeapon].silent)
    {
        ImGui::Checkbox(phrases[XorString("global_onkey2")].c_str(), &config->aimbot[currentWeapon].onKeyRCS);
        ImGui::SameLine();
        hotkey(config->aimbot[currentWeapon].RCSkey);
        ImGui::SameLine();
        ImGui::PushID(5);
        ImGui::PushItemWidth(70.0f);
        ImGui::Combo(" ", &config->aimbot[currentWeapon].RCSkeyMode, XorString("Hold\0Toggle\0"));
        ImGui::PopItemWidth();
        ImGui::PopID();
        ImGui::InputInt(phrases[XorString("aimbot_ignoreshots")].c_str(), &config->aimbot[currentWeapon].shotsFired);
        ImGui::SliderFloat(phrases[XorString("aimbot_rcs_x")].c_str(), &config->aimbot[currentWeapon].recoilControlX, 0.0f, 1.0f, "%.5f");
        ImGui::SliderFloat(phrases[XorString("aimbot_rcs_y")].c_str(), &config->aimbot[currentWeapon].recoilControlY, 0.0f, 1.0f, "%.5f");
    }

    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderRagebotWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.ragebot)
            return;
        ImGui::SetNextWindowSize({600.0f, 0.0f});
        ImGui::Begin(phrases[XorString("window_ragebot")].c_str(), &window.ragebot, windowFlags);
    }
    static int currentCategory{0};
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, XorString("All\0Pistols\0Heavy\0SMG\0Rifles\0"));
    ImGui::PopID();
    ImGui::SameLine();
    static int currentWeapon{0};
    ImGui::PushID(1);

    switch (currentCategory)
    {
    case 0:
        currentWeapon = 0;
        ImGui::NewLine();
        break;
    case 1:
    {
        static int currentPistol{0};
        static constexpr const char *pistols[]{"All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver"};

        ImGui::Combo(
            "", &currentPistol, [](void *data, int idx, const char **out_text)
            {
                if (config->aimbot[idx ? idx : 35].enabled)
                {
                    static std::string name;
                    name = pistols[idx];
                    *out_text = name.append(" *").c_str();
                }
                else
                {
                    *out_text = pistols[idx];
                }
                return true;
            },
            nullptr, IM_ARRAYSIZE(pistols));

        currentWeapon = currentPistol ? currentPistol : 35;
        break;
    }
    case 2:
    {
        static int currentHeavy{0};
        static constexpr const char *heavies[]{"All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev"};

        ImGui::Combo(
            "", &currentHeavy, [](void *data, int idx, const char **out_text)
            {
                if (config->aimbot[idx ? idx + 10 : 36].enabled)
                {
                    static std::string name;
                    name = heavies[idx];
                    *out_text = name.append(" *").c_str();
                }
                else
                {
                    *out_text = heavies[idx];
                }
                return true;
            },
            nullptr, IM_ARRAYSIZE(heavies));

        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
        break;
    }
    case 3:
    {
        static int currentSmg{0};
        static constexpr const char *smgs[]{"All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon"};

        ImGui::Combo(
            "", &currentSmg, [](void *data, int idx, const char **out_text)
            {
                if (config->aimbot[idx ? idx + 16 : 37].enabled)
                {
                    static std::string name;
                    name = smgs[idx];
                    *out_text = name.append(" *").c_str();
                }
                else
                {
                    *out_text = smgs[idx];
                }
                return true;
            },
            nullptr, IM_ARRAYSIZE(smgs));

        currentWeapon = currentSmg ? currentSmg + 16 : 37;
        break;
    }
    case 4:
    {
        static int currentRifle{0};
        static constexpr const char *rifles[]{"All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20"};

        ImGui::Combo(
            "", &currentRifle, [](void *data, int idx, const char **out_text)
            {
                if (config->aimbot[idx ? idx + 23 : 38].enabled)
                {
                    static std::string name;
                    name = rifles[idx];
                    *out_text = name.append(" *").c_str();
                }
                else
                {
                    *out_text = rifles[idx];
                }
                return true;
            },
            nullptr, IM_ARRAYSIZE(rifles));

        currentWeapon = currentRifle ? currentRifle + 23 : 38;
        break;
    }
    }
    ImGui::PopID();

    ImGui::SameLine();
    ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &config->ragebot[currentWeapon].enabled);
    ImGui::SameLine();
    ImGui::Text("  |  ");
    ImGui::SameLine();
    ImGui::Checkbox(phrases[XorString("global_onkey")].c_str(), &config->ragebot[currentWeapon].onKey);
    ImGui::SameLine();
    hotkey(config->ragebot[currentWeapon].key);
    ImGui::SameLine();
    ImGui::PushID(99);
    ImGui::Combo("", &config->ragebot[currentWeapon].keyMode, XorString("Hold\0Toggle\0"));
    ImGui::PopID();
    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    ImGui::Checkbox(phrases[XorString("aimhacks_silent")].c_str(), &config->ragebot[currentWeapon].slient);
    ImGui::Checkbox(phrases[XorString("ragebot_autostop")].c_str(), &config->ragebot[currentWeapon].autoStop);
    ImGui::Checkbox(phrases[XorString("aimhacks_friendlyfire")].c_str(), &config->ragebot[currentWeapon].friendlyFire);
    ImGui::Checkbox(phrases[XorString("aimhacks_betweenshots")].c_str(), &config->ragebot[currentWeapon].betweenShots);
    ImGui::Checkbox(phrases[XorString("aimhacks_autoscope")].c_str(), &config->ragebot[currentWeapon].autoScope);
    ImGui::Checkbox(phrases[XorString("aimhacks_autoshot")].c_str(), &config->ragebot[currentWeapon].autoShot);
    ImGui::Checkbox(phrases[XorString("ragebot_baim")].c_str(), &config->ragebot[currentWeapon].Baim);
    ImGui::Checkbox(phrases[XorString("ragebot_forceshot")].c_str(), &config->ragebot[currentWeapon].keyForceShotEnabled);
    ImGui::SameLine();
    hotkey(config->ragebot[currentWeapon].keyForceShot);
    ImGui::NextColumn();
    ImGuiCustom::MultiCombo(phrases[XorString("ragebot_hitboxes")].c_str(), config->BonesTexts, config->ragebot[currentWeapon].BonesBools, 8);
    ImGui::SliderFloat(phrases[XorString("aimhacks_mindamage")].c_str(), &config->ragebot[currentWeapon].WallDamage, 0, 250);
    ImGui::SliderFloat(phrases[XorString("aimhacks_hitchance")].c_str(), &config->ragebot[currentWeapon].hitChance, 0, 100);
    ImGui::SliderFloat(phrases[XorString("ragebot_headvalue")].c_str(), &config->ragebot[currentWeapon].pointChance, 0, 100);
    ImGui::SliderFloat(phrases[XorString("ragebot_bodyvalue")].c_str(), &config->ragebot[currentWeapon].bodyChance, 0, 100);
    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

/*void GUI::renderAntiAimWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.antiAim)
            return;
        ImGui::SetNextWindowSize({ 600.0f, 0.0f });
        ImGui::Begin(phrases[XorString("window_antiaim")].c_str(), &window.antiAim, windowFlags);
    }

    ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &config->antiAim.general.enabled);
    ImGui::SameLine();
    ImGui::Text("    |    ");
    ImGui::SameLine();
    ImGui::Text(phrases[XorString("antiaim_invertkey")].c_str());
    ImGui::SameLine();
    hotkey(config->antiAim.general.yawInverseAngleKey);
    ImGui::SameLine();
    ImGui::PushID(1);
    ImGui::SetNextItemWidth(75.0f);
    ImGui::Combo("", &config->antiAim.general.yawInverseKeyMode, XorString("Hold\0Toggle\0"));
    ImGui::PopID();
    ImGui::Checkbox(phrases[XorString("antiaim_fakewalk")].c_str(), &config->antiAim.general.fakeWalk.enabled);
    if (!config->antiAim.general.fakeWalk.enabled)DisableItems();
    ImGui::SameLine();
    hotkey(config->antiAim.general.fakeWalk.key);
    ImGui::PushID(2);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(75.0f);
    ImGui::Combo("", &config->antiAim.general.fakeWalk.keyMode, XorString("Hold\0Toggle\0"));
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::SetNextItemWidth(140.0f);
    ImGui::SliderInt(phrases[XorString("antiaim_fakewalk_speed")].c_str(), &config->antiAim.general.fakeWalk.maxChoke, 3, 15);
    if (!config->antiAim.general.fakeWalk.enabled) PopDisableItems();

    ImGui::Separator();
    ImGui::Columns(3, nullptr, true);
    ImGui::Text(XorString("Standing"));
    ImGui::Checkbox(XorString("Standing Enabled"), &config->antiAim.standing.enabled);
    if (config->antiAim.standing.enabled)
    {

        ImGui::Checkbox(XorString("Standing Pitch"), &config->antiAim.standing.pitch.enabled);
        if (config->antiAim.standing.pitch.enabled) {
            ImGui::PushID(0);
            ImGui::SetNextItemWidth(140.0f);
            ImGui::SliderFloat("", &config->antiAim.standing.pitch.angle, -89.0f, 89.0f, XorString("Pitch Angle: %.2f"), 1);
            ImGui::PopID();
        }
        ImGui::Checkbox(XorString("Standing Yaw"), &config->antiAim.standing.yaw.enabled);
        if (config->antiAim.standing.yaw.enabled) {
            ImGui::PushID(1);
            ImGui::SetNextItemWidth(140.0f);
            ImGui::SliderFloat("", &config->antiAim.standing.yaw.angle, -180.0f, 180.0f, XorString("Yaw Angle: %.2f"), 1);
            ImGui::PopID();
            ImGui::SetNextItemWidth(85.0f);
            ImGui::Combo(XorString("Yaw Mode"), &config->antiAim.standing.yaw.fake.mode, XorString("Static\0Jitter\0"));
            if (config->antiAim.standing.yaw.fake.mode == 1)
            {
                ImGui::PushID(2);
                ImGui::SetNextItemWidth(140.0f);
                ImGui::SliderFloat("", &config->antiAim.standing.yaw.fake.step, 0.0f, 180.0f, XorString("Step: %.2f"), 1);
                ImGui::PopID();
                ImGui::PushID(3);
                ImGui::SetNextItemWidth(140.0f);
                ImGui::SliderFloat("", &config->antiAim.standing.yaw.fake.jitterMax, -180.0f, 180.0f, XorString("Jitter Max: %.2f"), 1);
                ImGui::PopID();
                ImGui::PushID(4);
                ImGui::SetNextItemWidth(140.0f);
                ImGui::SliderFloat("", &config->antiAim.standing.yaw.fake.jitterMin, -180.0f, 180.0f, XorString("Jitter Min: %.2f"), 1);
                ImGui::PopID();
            }
        }
        ImGui::Checkbox("Standing Yaw Desync", &config->antiAim.standing.yaw.desync.enabled);
        if (config->antiAim.standing.yaw.desync.enabled == true)
        {
            ImGui::SetNextItemWidth(85.0f);
            ImGui::PushID(0);
            ImGui::Combo("Desync Mode", &config->antiAim.standing.yaw.desync.mode, XorString("Static\0Jitter\0"));
            ImGui::PopID();
            if (!config->antiAim.standing.yaw.desync.mode)
            {
                ImGui::PushID(5);
                ImGui::SetNextItemWidth(140.0f);
                ImGui::SliderFloat("", &config->antiAim.standing.yaw.desync.bodyLean, -100.0f, 100.0f, XorString("Body Lean: %.2f"), 1);
                ImGui::PopID();
            }
            else if (config->antiAim.standing.yaw.desync.mode == 1)
            {
                ImGui::PushID(6);
                ImGui::SetNextItemWidth(140.0f);
                ImGui::SliderFloat("", &config->antiAim.standing.yaw.desync.step, 0.0f, 100.0f, XorString("Desync Step: %.2f�"), 1);
                ImGui::PopID();
                ImGui::PushID(7);
                ImGui::SetNextItemWidth(140.0f);
                ImGui::SliderFloat("", &config->antiAim.standing.yaw.desync.jitterMax, -100.0f, 100.0f, XorString("Jitter Max: %.2f�"), 1);
                ImGui::PopID();
                ImGui::PushID(8);
                ImGui::SetNextItemWidth(140.0f);
                ImGui::SliderFloat("", &config->antiAim.standing.yaw.desync.jitterMin, -100.0f, 100.0f, XorString("Jitter Min: %.2f�"), 1);
                ImGui::PopID();
            }
            ImGui::Checkbox("LBY Breaker", &config->antiAim.standing.yaw.desync.LBYBreaker.enabled);
            if (config->antiAim.standing.yaw.desync.LBYBreaker.enabled)
            {
                ImGui::PushID(9);
                ImGui::SliderFloat("", &config->antiAim.standing.yaw.desync.LBYBreaker.angle, -180.0f, 180.0f, XorString("LBY Angle: %.2f�"), 1);
                ImGui::PopID();
            }
        }

    };

        ImGui::NextColumn();
        ImGui::Text(XorString("Moving"));
        ImGui::Checkbox(XorString("Moving Enabled"), &config->antiAim.moving.enabled);
        if (config->antiAim.moving.enabled)
        {

            ImGui::Checkbox(XorString("Moving Pitch"), &config->antiAim.moving.pitch.enabled);
            if (config->antiAim.moving.pitch.enabled) {
                ImGui::PushID(10);
                ImGui::SetNextItemWidth(140.0f);
                ImGui::SliderFloat("", &config->antiAim.moving.pitch.angle, -89.0f, 89.0f, XorString("Pitch Angle: %.2f�"), 1);
                ImGui::PopID();
            }
            ImGui::Checkbox(XorString("Moving Yaw"), &config->antiAim.moving.yaw.enabled);
            if (config->antiAim.moving.yaw.enabled) {
                ImGui::PushID(11);
                ImGui::SetNextItemWidth(140.0f);
                ImGui::SliderFloat("", &config->antiAim.moving.yaw.angle, -180.0f, 180.0f, XorString("Yaw Angle: %.2f�"), 1);
                ImGui::PopID();
                ImGui::PushID(12);
                ImGui::SetNextItemWidth(85.0f);
                ImGui::Combo(XorString("Yaw Mode"), &config->antiAim.moving.yaw.fake.mode, XorString("Static\0Jitter\0"));
                ImGui::PopID();
                if (config->antiAim.moving.yaw.fake.mode == 1)
                {
                    ImGui::PushID(13);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat("", &config->antiAim.moving.yaw.fake.step, 0.0f, 180.0f, XorString("Step: %.2f�"), 1);
                    ImGui::PopID();
                    ImGui::PushID(14);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat("", &config->antiAim.moving.yaw.fake.jitterMax, -180.0f, 180.0f, XorString("Jitter Max: %.2f�"), 1);
                    ImGui::PopID();
                    ImGui::PushID(15);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat("", &config->antiAim.moving.yaw.fake.jitterMin, -180.0f, 180.0f, XorString("Jitter Min:%.2f�"), 1);
                    ImGui::PopID();
                }
            }
            ImGui::Checkbox(XorString("Moving Yaw Desync"), &config->antiAim.moving.yaw.desync.enabled);
            if (config->antiAim.moving.yaw.desync.enabled == true)
            {
                ImGui::PushID(1);
                ImGui::SetNextItemWidth(85.0f);
                ImGui::Combo(XorString("Desync Mode"), &config->antiAim.moving.yaw.desync.mode, XorString("Static\0Jitter\0"));
                ImGui::PopID();
                if (!config->antiAim.moving.yaw.desync.mode)
                {
                    ImGui::PushID(16);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat("", &config->antiAim.moving.yaw.desync.bodyLean, -100.0f, 100.0f, XorString("Body Lean: %.2f"), 1);
                    ImGui::PopID();
                }
                else if (config->antiAim.moving.yaw.desync.mode == 1)
                {
                    ImGui::PushID(17);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat(XorString("Moving Desync Step"), &config->antiAim.moving.yaw.desync.step, 0.0f, 100.0f, XorString("Desync Step:%.2f�"), 1);
                    ImGui::PopID();
                    ImGui::PushID(18);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat("", &config->antiAim.moving.yaw.desync.jitterMax, -100.0f, 100.0f, XorString("Jitter Max: %.2f�"), 1);
                    ImGui::PopID();
                    ImGui::PushID(19);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat("", &config->antiAim.moving.yaw.desync.jitterMin, -100.0f, 100.0f, XorString("Jitter Min: %.2f�"), 1);
                    ImGui::PopID();
                }
                ImGui::Checkbox(XorString("Moving LBY Breaker"), &config->antiAim.moving.yaw.desync.LBYBreaker.enabled);
                if (config->antiAim.moving.yaw.desync.LBYBreaker.enabled)
                {
                    ImGui::PushID(20);
                    ImGui::SliderFloat("", &config->antiAim.moving.yaw.desync.LBYBreaker.angle, -180.0f, 180.0f, XorString("LBY Angle: %.2f"), 1);
                    ImGui::PopID();
                }
            }
        };
        ImGui::NextColumn();
        ImGui::Text(XorString("In Air"));
        ImGui::Checkbox(XorString("In Air Enabled"), &config->antiAim.inAir.enabled);
        if (config->antiAim.inAir.enabled)
        {

            ImGui::Checkbox(XorString("inAir Pitch"), &config->antiAim.inAir.pitch.enabled);
            if (config->antiAim.inAir.pitch.enabled) {
                ImGui::PushID(21);
                ImGui::SetNextItemWidth(140.0f);
                ImGui::SliderFloat("", &config->antiAim.inAir.pitch.angle, -89.0f, 89.0f, XorString("Pitch Angle: %.2f"), 1);
                ImGui::PopID();
            }
            ImGui::Checkbox(XorString("inAir Yaw"), &config->antiAim.inAir.yaw.enabled);
            if (config->antiAim.inAir.yaw.enabled) {
                ImGui::PushID(22);
                ImGui::SetNextItemWidth(140.0f);
                ImGui::SliderFloat("", &config->antiAim.inAir.yaw.angle, -180.0f, 180.0f, XorString("Yaw Angle: %.2f"), 1);
                ImGui::PopID();
                ImGui::PushID(23);
                ImGui::SetNextItemWidth(85.0f);
                ImGui::Combo(XorString("Yaw Mode"), &config->antiAim.inAir.yaw.fake.mode, XorString("Static\0Jitter\0"));
                ImGui::PopID();
                if (config->antiAim.inAir.yaw.fake.mode == 1)
                {
                    ImGui::PushID(24);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat("", &config->antiAim.inAir.yaw.fake.step, 0.0f, 180.0f, XorString("Step: %.2f"), 1);
                    ImGui::PopID();
                    ImGui::PushID(14);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat("", &config->antiAim.inAir.yaw.fake.jitterMax, -180.0f, 180.0f, XorString("Jitter Max: %.2f"), 1);
                    ImGui::PopID();
                    ImGui::PushID(25);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat("", &config->antiAim.inAir.yaw.fake.jitterMin, -180.0f, 180.0f, XorString("Jitter Min:%.2f"), 1);
                    ImGui::PopID();
                }
            }
            ImGui::Checkbox(XorString("inAir Yaw Desync"), &config->antiAim.inAir.yaw.desync.enabled);
            if (config->antiAim.inAir.yaw.desync.enabled == true)
            {
                ImGui::PushID(26);
                ImGui::SetNextItemWidth(85.0f);
                ImGui::Combo(XorString("Desync Mode"), &config->antiAim.inAir.yaw.desync.mode, XorString("Static\0Jitter\0"));
                ImGui::PopID();
                if (!config->antiAim.inAir.yaw.desync.mode)
                {
                    ImGui::PushID(27);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat("", &config->antiAim.inAir.yaw.desync.bodyLean, -100.0f, 100.0f, XorString("Body Lean: %.2f"), 1);
                    ImGui::PopID();
                }
                else if (config->antiAim.inAir.yaw.desync.mode == 1)
                {
                    ImGui::PushID(28);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat(XorString("inAir Desync Step"), &config->antiAim.inAir.yaw.desync.step, 0.0f, 100.0f, XorString("Desync Step:%.2f"), 1);
                    ImGui::PopID();
                    ImGui::PushID(29);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat("", &config->antiAim.inAir.yaw.desync.jitterMax, -100.0f, 100.0f, XorString("Jitter Max: %.2f"), 1);
                    ImGui::PopID();
                    ImGui::PushID(30);
                    ImGui::SetNextItemWidth(140.0f);
                    ImGui::SliderFloat("", &config->antiAim.inAir.yaw.desync.jitterMin, -100.0f, 100.0f, XorString("Jitter Min: %.2f"), 1);
                    ImGui::PopID();
                }
                ImGui::Checkbox(XorString("inAir LBY Breaker"), &config->antiAim.inAir.yaw.desync.LBYBreaker.enabled);
                if (config->antiAim.inAir.yaw.desync.LBYBreaker.enabled)
                {
                    ImGui::PushID(31);
                    ImGui::SliderFloat("", &config->antiAim.inAir.yaw.desync.LBYBreaker.angle, -180.0f, 180.0f, XorString("LBY Angle: %.2f"), 1);
                    ImGui::PopID();
                }
            }
        }
        ImGui::Columns(1);

    if (!contentOnly)
        ImGui::End();
}
*/
void GUI::renderTriggerbotWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.triggerbot)
            return;
        ImGui::SetNextWindowSize({0.0f, 0.0f});
        ImGui::Begin(phrases[XorString("window_triggerbot")].c_str(), &window.triggerbot, windowFlags);
    }
    static int currentCategory{0};
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, XorString("All\0Pistols\0Heavy\0SMG\0Rifles\0Zeus x27\0"));
    ImGui::PopID();
    ImGui::SameLine();
    static int currentWeapon{0};
    ImGui::PushID(1);
    switch (currentCategory)
    {
    case 0:
        currentWeapon = 0;
        ImGui::NewLine();
        break;
    case 5:
        currentWeapon = 39;
        ImGui::NewLine();
        break;

    case 1:
    {
        static int currentPistol{0};
        static constexpr const char *pistols[]{"All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver"};

        ImGui::Combo(
            "", &currentPistol, [](void *data, int idx, const char **out_text)
            {
                if (config->triggerbot[idx ? idx : 35].enabled)
                {
                    static std::string name;
                    name = pistols[idx];
                    *out_text = name.append(" *").c_str();
                }
                else
                {
                    *out_text = pistols[idx];
                }
                return true;
            },
            nullptr, IM_ARRAYSIZE(pistols));

        currentWeapon = currentPistol ? currentPistol : 35;
        break;
    }
    case 2:
    {
        static int currentHeavy{0};
        static constexpr const char *heavies[]{"All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev"};

        ImGui::Combo(
            "", &currentHeavy, [](void *data, int idx, const char **out_text)
            {
                if (config->triggerbot[idx ? idx + 10 : 36].enabled)
                {
                    static std::string name;
                    name = heavies[idx];
                    *out_text = name.append(" *").c_str();
                }
                else
                {
                    *out_text = heavies[idx];
                }
                return true;
            },
            nullptr, IM_ARRAYSIZE(heavies));

        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
        break;
    }
    case 3:
    {
        static int currentSmg{0};
        static constexpr const char *smgs[]{"All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon"};

        ImGui::Combo(
            "", &currentSmg, [](void *data, int idx, const char **out_text)
            {
                if (config->triggerbot[idx ? idx + 16 : 37].enabled)
                {
                    static std::string name;
                    name = smgs[idx];
                    *out_text = name.append(" *").c_str();
                }
                else
                {
                    *out_text = smgs[idx];
                }
                return true;
            },
            nullptr, IM_ARRAYSIZE(smgs));

        currentWeapon = currentSmg ? currentSmg + 16 : 37;
        break;
    }
    case 4:
    {
        static int currentRifle{0};
        static constexpr const char *rifles[]{"All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20"};

        ImGui::Combo(
            "", &currentRifle, [](void *data, int idx, const char **out_text)
            {
                if (config->triggerbot[idx ? idx + 23 : 38].enabled)
                {
                    static std::string name;
                    name = rifles[idx];
                    *out_text = name.append(" *").c_str();
                }
                else
                {
                    *out_text = rifles[idx];
                }
                return true;
            },
            nullptr, IM_ARRAYSIZE(rifles));

        currentWeapon = currentRifle ? currentRifle + 23 : 38;
        break;
    }
    }
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &config->triggerbot[currentWeapon].enabled);
    ImGui::SameLine();
    ImGui::Text("  |  ");
    ImGui::SameLine();
    ImGui::Checkbox(phrases[XorString("global_onkey")].c_str(), &config->triggerbot[currentWeapon].onKey);
    ImGui::SameLine();
    hotkey(config->triggerbot[currentWeapon].key);
    ImGui::Separator();
    ImGui::Columns(2, nullptr, true);
    ImGui::Checkbox(phrases[XorString("aimhacks_friendlyfire")].c_str(), &config->triggerbot[currentWeapon].friendlyFire);
    ImGui::Checkbox(phrases[XorString("aimhacks_scopedonly")].c_str(), &config->triggerbot[currentWeapon].scopedOnly);
    ImGui::Checkbox(phrases[XorString("aimhacks_ignoreflash")].c_str(), &config->triggerbot[currentWeapon].ignoreFlash);
    ImGui::Checkbox(phrases[XorString("aimhacks_ignoresmoke")].c_str(), &config->triggerbot[currentWeapon].ignoreSmoke);
    ImGui::Checkbox(phrases[XorString("aimhacks_killshot")].c_str(), &config->triggerbot[currentWeapon].killshot);
    ImGui::NextColumn();
    ImGui::SetNextItemWidth(85.0f);
    ImGui::Combo(phrases[XorString("global_hitgroup")].c_str(), &config->triggerbot[currentWeapon].hitgroup, XorString("All\0Head\0Chest\0Stomach\0Left arm\0Right arm\0Left leg\0Right leg\0"));
    ImGui::SetNextItemWidth(85.0f);
    ImGui::InputInt(phrases[XorString("aimhacks_mindamage")].c_str(), &config->triggerbot[currentWeapon].minDamage);
    config->triggerbot[currentWeapon].minDamage = std::clamp(config->triggerbot[currentWeapon].minDamage, 0, 250);
    ImGui::PushID(1);
    ImGui::SetNextItemWidth(100.0f);
    ImGui::SliderInt("", &config->triggerbot[currentWeapon].shotDelay, 0, 250, phrases[XorString("triggerbot_shotdelay")].c_str());
    ImGui::PopID();
    ImGui::SetNextItemWidth(100.0f);
    ImGui::SliderFloat(phrases[XorString("triggerbot_bursttime")].c_str(), &config->triggerbot[currentWeapon].burstTime, 0.0f, 0.5f, XorString("%.3f s"));
    ImGui::Columns(1);

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderBacktrackWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.backtrack)
            return;
        ImGui::SetNextWindowSize({350.0f, 0.0f});
        ImGui::Begin(phrases[XorString("window_backtrack")].c_str(), &window.backtrack, windowFlags);
    }
    ImGui::Columns(2, nullptr, true);
    ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &config->backtrack.enabled);
    ImGui::Checkbox(phrases[XorString("aimhacks_ignoresmoke")].c_str(), &config->backtrack.ignoreSmoke);
    ImGui::NextColumn();
    ImGui::Checkbox(phrases[XorString("backtrack_extend")].c_str(), &config->backtrack.fakeLatency);
    ImGui::Columns(1);
    if (!config->backtrack.fakeLatency)
    {
        if (config->backtrack.timeLimit >= 201)
        {
            config->backtrack.timeLimit = 200;
        }
    }
    ImGui::PushItemWidth(220.0f);
    ImGui::PushID(0);
    ImGui::SliderInt("", &config->backtrack.timeLimit, 1, config->backtrack.fakeLatency ? 400 : 200, phrases[XorString("backtrack_timelimit")].c_str());
    ImGui::PopID();
    ImGui::PopItemWidth();
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderGlowWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.glow)
            return;
        ImGui::SetNextWindowSize({350.0f, 0.0f});
        ImGui::Begin(phrases[XorString("window_glow")].c_str(), &window.glow, windowFlags);
    }
    static int currentCategory{0};
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, XorString("Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0C4\0Planted C4\0Chickens\0Defuse kits\0Projectiles\0Hostages\0Ragdolls\0"));
    ImGui::PopID();
    static int currentItem{0};
    if (currentCategory <= 3)
    {
        ImGui::SameLine();
        static int currentType{0};
        ImGui::PushID(1);
        ImGui::Combo("", &currentType, XorString("All\0Visible\0Occluded\0"));
        ImGui::PopID();
        currentItem = currentCategory * 3 + currentType;
    }
    else
    {
        currentItem = currentCategory + 8;
    }

    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 150.0f);
    ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &config->glow[currentItem].enabled);
    ImGui::Checkbox(phrases[XorString("wallhacks_healthbased")].c_str(), &config->glow[currentItem].healthBased);
    ImGui::NextColumn();
    ImGui::SetNextItemWidth(100.0f);
    ImGui::Combo(phrases[XorString("global_style")].c_str(), &config->glow[currentItem].style, XorString("Default\0Rim3d\0Edge\0Edge Pulse\0"));
    ImGuiCustom::colorPopup(phrases[XorString("global_color")].c_str(), config->glow[currentItem].color, &config->glow[currentItem].rainbow, &config->glow[currentItem].rainbowSpeed);
    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderChamsWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.chams)
            return;
        ImGui::SetNextWindowSize({0.0f, 0.0f});
        ImGui::Begin(phrases[XorString("window_chams")].c_str(), &window.chams, windowFlags);
    }
    static int currentCategory{0};
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);

    static int material = 1;

    if (ImGui::Combo("", &currentCategory, XorString("Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0Hands\0Backtrack\0Sleeves\0Desync\0")))
        material = 1;

    ImGui::PopID();

    ImGui::SameLine();

    if (material <= 1)
        ImGuiCustom::arrowButtonDisabled("##left", ImGuiDir_Left);
    else if (ImGui::ArrowButton("##left", ImGuiDir_Left))
        --material;

    ImGui::SameLine();
    ImGui::Text("%d", material);

    constexpr std::array categories{"Allies", "Enemies", "Planting", "Defusing", "Local player", "Weapons", "Hands", "Backtrack", "Sleeves", "Desync"};

    ImGui::SameLine();

    if (material >= int(config->chams[categories[currentCategory]].materials.size()))
        ImGuiCustom::arrowButtonDisabled("##right", ImGuiDir_Right);
    else if (ImGui::ArrowButton("##right", ImGuiDir_Right))
        ++material;

    ImGui::SameLine();

    auto &chams{config->chams[categories[currentCategory]].materials[material - 1]};

    ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &chams.enabled);
    ImGui::Separator();
    ImGui::Columns(2, nullptr, true);
    ImGui::Checkbox(phrases[XorString("wallhacks_healthbased")].c_str(), &chams.healthBased);
    ImGui::Checkbox(phrases[XorString("chams_blinking")].c_str(), &chams.blinking);
    ImGui::Checkbox(phrases[XorString("chams_wireframe")].c_str(), &chams.wireframe);
    ImGui::SetNextItemWidth(85.0f);
    ImGui::Combo(phrases[XorString("chams_material")].c_str(), &chams.material, XorString("Normal\0Flat\0Animated\0Platinum\0Glass\0Chrome\0Crystal\0Silver\0Gold\0Plastic\0Glow\0Pearlescent\0Metallic\0"));
    ImGui::NextColumn();
    ImGui::Checkbox(phrases[XorString("chams_cover")].c_str(), &chams.cover);
    ImGui::Checkbox(phrases[XorString("chams_ignorez")].c_str(), &chams.ignorez);
    ImGuiCustom::colorPopup(phrases[XorString("global_color")].c_str(), chams.color, &chams.rainbow, &chams.rainbowSpeed);
    ImGui::Columns(1);
    if (!contentOnly)
    {
        ImGui::End();
    }
}

void GUI::renderStreamProofESPWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.streamProofESP)
            return;
        ImGui::SetNextWindowSize({0.0f, 400.0f});
        ImGui::Begin(phrases[XorString("window_esp")].c_str(), &window.streamProofESP, windowFlags);
    }

    static std::size_t currentCategory;
    static auto currentItem = "All";

    constexpr auto getConfigShared = [](std::size_t category, const char *item) noexcept -> Shared &
    {
        switch (category)
        {
        case 0:
        default:
            return config->streamProofESP.enemies[item];
        case 1:
            return config->streamProofESP.allies[item];
        case 2:
            return config->streamProofESP.weapons[item];
        case 3:
            return config->streamProofESP.projectiles[item];
        case 4:
            return config->streamProofESP.lootCrates[item];
        case 5:
            return config->streamProofESP.otherEntities[item];
        }
    };

    constexpr auto getConfigPlayer = [](std::size_t category, const char *item) noexcept -> Player &
    {
        switch (category)
        {
        case 0:
        default:
            return config->streamProofESP.enemies[item];
        case 1:
            return config->streamProofESP.allies[item];
        }
    };

    if (ImGui::ListBoxHeader("##list", {170.0f, 300.0f}))
    {
        constexpr std::array categories{"Enemies", "Allies", "Weapons", "Projectiles", "Loot Crates", "Other Entities"};

        for (std::size_t i = 0; i < categories.size(); ++i)
        {
            if (ImGui::Selectable(categories[i], currentCategory == i && std::string_view{currentItem} == "All"))
            {
                currentCategory = i;
                currentItem = "All";
            }

            if (ImGui::BeginDragDropSource())
            {
                switch (i)
                {
                case 0:
                case 1:
                    ImGui::SetDragDropPayload("Player", &getConfigPlayer(i, "All"), sizeof(Player), ImGuiCond_Once);
                    break;
                case 2:
                    ImGui::SetDragDropPayload("Weapon", &config->streamProofESP.weapons["All"], sizeof(Weapon), ImGuiCond_Once);
                    break;
                case 3:
                    ImGui::SetDragDropPayload("Projectile", &config->streamProofESP.projectiles["All"], sizeof(Projectile), ImGuiCond_Once);
                    break;
                default:
                    ImGui::SetDragDropPayload("Entity", &getConfigShared(i, "All"), sizeof(Shared), ImGuiCond_Once);
                    break;
                }
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Player"))
                {
                    const auto &data = *(Player *)payload->Data;

                    switch (i)
                    {
                    case 0:
                    case 1:
                        getConfigPlayer(i, "All") = data;
                        break;
                    case 2:
                        config->streamProofESP.weapons["All"] = data;
                        break;
                    case 3:
                        config->streamProofESP.projectiles["All"] = data;
                        break;
                    default:
                        getConfigShared(i, "All") = data;
                        break;
                    }
                }

                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Weapon"))
                {
                    const auto &data = *(Weapon *)payload->Data;

                    switch (i)
                    {
                    case 0:
                    case 1:
                        getConfigPlayer(i, "All") = data;
                        break;
                    case 2:
                        config->streamProofESP.weapons["All"] = data;
                        break;
                    case 3:
                        config->streamProofESP.projectiles["All"] = data;
                        break;
                    default:
                        getConfigShared(i, "All") = data;
                        break;
                    }
                }

                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Projectile"))
                {
                    const auto &data = *(Projectile *)payload->Data;

                    switch (i)
                    {
                    case 0:
                    case 1:
                        getConfigPlayer(i, "All") = data;
                        break;
                    case 2:
                        config->streamProofESP.weapons["All"] = data;
                        break;
                    case 3:
                        config->streamProofESP.projectiles["All"] = data;
                        break;
                    default:
                        getConfigShared(i, "All") = data;
                        break;
                    }
                }

                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Entity"))
                {
                    const auto &data = *(Shared *)payload->Data;

                    switch (i)
                    {
                    case 0:
                    case 1:
                        getConfigPlayer(i, "All") = data;
                        break;
                    case 2:
                        config->streamProofESP.weapons["All"] = data;
                        break;
                    case 3:
                        config->streamProofESP.projectiles["All"] = data;
                        break;
                    default:
                        getConfigShared(i, "All") = data;
                        break;
                    }
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::PushID(i);
            ImGui::Indent();

            const auto items = [](std::size_t category) noexcept -> std::vector<const char *>
            {
                switch (category)
                {
                case 0:
                case 1:
                    return {"Visible", "Occluded"};
                case 2:
                    return {"Pistols", "SMGs", "Rifles", "Sniper Rifles", "Shotguns", "Machineguns", "Grenades", "Melee", "Other"};
                case 3:
                    return {"Flashbang", "HE Grenade", "Breach Charge", "Bump Mine", "Decoy Grenade", "Molotov", "TA Grenade", "Smoke Grenade", "Snowball"};
                case 4:
                    return {"Pistol Case", "Light Case", "Heavy Case", "Explosive Case", "Tools Case", "Cash Dufflebag"};
                case 5:
                    return {"Defuse Kit", "Chicken", "Planted C4", "Hostage", "Sentry", "Cash", "Ammo Box", "Radar Jammer", "Snowball Pile"};
                default:
                    return {};
                }
            }(i);

            const auto categoryEnabled = getConfigShared(i, "All").enabled;

            for (std::size_t j = 0; j < items.size(); ++j)
            {
                static bool selectedSubItem;
                if (!categoryEnabled || getConfigShared(i, items[j]).enabled)
                {
                    if (ImGui::Selectable(items[j], currentCategory == i && !selectedSubItem && std::string_view{currentItem} == items[j]))
                    {
                        currentCategory = i;
                        currentItem = items[j];
                        selectedSubItem = false;
                    }

                    if (ImGui::BeginDragDropSource())
                    {
                        switch (i)
                        {
                        case 0:
                        case 1:
                            ImGui::SetDragDropPayload("Player", &getConfigPlayer(i, items[j]), sizeof(Player), ImGuiCond_Once);
                            break;
                        case 2:
                            ImGui::SetDragDropPayload("Weapon", &config->streamProofESP.weapons[items[j]], sizeof(Weapon), ImGuiCond_Once);
                            break;
                        case 3:
                            ImGui::SetDragDropPayload("Projectile", &config->streamProofESP.projectiles[items[j]], sizeof(Projectile), ImGuiCond_Once);
                            break;
                        default:
                            ImGui::SetDragDropPayload("Entity", &getConfigShared(i, items[j]), sizeof(Shared), ImGuiCond_Once);
                            break;
                        }
                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Player"))
                        {
                            const auto &data = *(Player *)payload->Data;

                            switch (i)
                            {
                            case 0:
                            case 1:
                                getConfigPlayer(i, items[j]) = data;
                                break;
                            case 2:
                                config->streamProofESP.weapons[items[j]] = data;
                                break;
                            case 3:
                                config->streamProofESP.projectiles[items[j]] = data;
                                break;
                            default:
                                getConfigShared(i, items[j]) = data;
                                break;
                            }
                        }

                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Weapon"))
                        {
                            const auto &data = *(Weapon *)payload->Data;

                            switch (i)
                            {
                            case 0:
                            case 1:
                                getConfigPlayer(i, items[j]) = data;
                                break;
                            case 2:
                                config->streamProofESP.weapons[items[j]] = data;
                                break;
                            case 3:
                                config->streamProofESP.projectiles[items[j]] = data;
                                break;
                            default:
                                getConfigShared(i, items[j]) = data;
                                break;
                            }
                        }

                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Projectile"))
                        {
                            const auto &data = *(Projectile *)payload->Data;

                            switch (i)
                            {
                            case 0:
                            case 1:
                                getConfigPlayer(i, items[j]) = data;
                                break;
                            case 2:
                                config->streamProofESP.weapons[items[j]] = data;
                                break;
                            case 3:
                                config->streamProofESP.projectiles[items[j]] = data;
                                break;
                            default:
                                getConfigShared(i, items[j]) = data;
                                break;
                            }
                        }

                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Entity"))
                        {
                            const auto &data = *(Shared *)payload->Data;

                            switch (i)
                            {
                            case 0:
                            case 1:
                                getConfigPlayer(i, items[j]) = data;
                                break;
                            case 2:
                                config->streamProofESP.weapons[items[j]] = data;
                                break;
                            case 3:
                                config->streamProofESP.projectiles[items[j]] = data;
                                break;
                            default:
                                getConfigShared(i, items[j]) = data;
                                break;
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                }

                if (i != 2)
                    continue;

                ImGui::Indent();

                const auto subItems = [](std::size_t item) noexcept -> std::vector<const char *>
                {
                    switch (item)
                    {
                    case 0:
                        return {"Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-SeveN", "CZ75-Auto", "Desert Eagle", "R8 Revolver"};
                    case 1:
                        return {"MAC-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon"};
                    case 2:
                        return {"Galil AR", "FAMAS", "AK-47", "M4A4", "M4A1-S", "SG 553", "AUG"};
                    case 3:
                        return {"SSG 08", "AWP", "G3SG1", "SCAR-20"};
                    case 4:
                        return {"Nova", "XM1014", "Sawed-Off", "MAG-7"};
                    case 5:
                        return {"M249", "Negev"};
                    case 6:
                        return {"Flashbang", "HE Grenade", "Smoke Grenade", "Molotov", "Decoy Grenade", "Incendiary", "TA Grenade", "Fire Bomb", "Diversion", "Frag Grenade", "Snowball"};
                    case 7:
                        return {"Axe", "Hammer", "Wrench"};
                    case 8:
                        return {"C4", "Healthshot", "Bump Mine", "Zone Repulsor", "Shield"};
                    default:
                        return {};
                    }
                }(j);

                const auto itemEnabled = getConfigShared(i, items[j]).enabled;

                for (const auto subItem : subItems)
                {
                    auto &subItemConfig = config->streamProofESP.weapons[subItem];
                    if ((categoryEnabled || itemEnabled) && !subItemConfig.enabled)
                        continue;

                    if (ImGui::Selectable(subItem, currentCategory == i && selectedSubItem && std::string_view{currentItem} == subItem))
                    {
                        currentCategory = i;
                        currentItem = subItem;
                        selectedSubItem = true;
                    }

                    if (ImGui::BeginDragDropSource())
                    {
                        ImGui::SetDragDropPayload("Weapon", &subItemConfig, sizeof(Weapon), ImGuiCond_Once);
                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Player"))
                        {
                            const auto &data = *(Player *)payload->Data;
                            subItemConfig = data;
                        }

                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Weapon"))
                        {
                            const auto &data = *(Weapon *)payload->Data;
                            subItemConfig = data;
                        }

                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Projectile"))
                        {
                            const auto &data = *(Projectile *)payload->Data;
                            subItemConfig = data;
                        }

                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Entity"))
                        {
                            const auto &data = *(Shared *)payload->Data;
                            subItemConfig = data;
                        }
                        ImGui::EndDragDropTarget();
                    }
                }

                ImGui::Unindent();
            }
            ImGui::Unindent();
            ImGui::PopID();
        }
        ImGui::ListBoxFooter();
    }

    ImGui::SameLine();

    if (ImGui::BeginChild("##child", {400.0f, 300.0f}))
    {
        auto &sharedConfig = getConfigShared(currentCategory, currentItem);

        ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &sharedConfig.enabled);
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 260.0f);
        ImGui::SetNextItemWidth(220.0f);
        if (ImGui::BeginCombo(phrases[XorString("esp_font")].c_str(), config->getSystemFonts()[sharedConfig.font.index].c_str()))
        {
            for (size_t i = 0; i < config->getSystemFonts().size(); i++)
            {
                bool isSelected = config->getSystemFonts()[i] == sharedConfig.font.name;
                if (ImGui::Selectable(config->getSystemFonts()[i].c_str(), isSelected, 0, {250.0f, 0.0f}))
                {
                    sharedConfig.font.index = i;
                    sharedConfig.font.name = config->getSystemFonts()[i];
                    config->scheduleFontLoad(sharedConfig.font.name);
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();

        constexpr auto spacing = 250.0f;
        ImGuiCustom::colorPicker(phrases[XorString("esp_snapline")].c_str(), sharedConfig.snapline);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90.0f);
        ImGui::Combo("##1", &sharedConfig.snapline.type, XorString("Bottom\0Top\0Crosshair\0"));
        ImGui::SameLine(spacing);
        ImGuiCustom::colorPicker(phrases[XorString("esp_box")].c_str(), sharedConfig.box);
        ImGui::SameLine();

        ImGui::PushID(phrases[XorString("esp_box")].c_str());

        if (ImGui::Button(phrases[XorString("global_threedots")].c_str()))
            ImGui::OpenPopup("");

        if (ImGui::BeginPopup(""))
        {
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo(phrases[XorString("global_type")].c_str(), &sharedConfig.box.type, XorString("2D\0"
                                                                                                      "2D corners\0"
                                                                                                      "3D\0"
                                                                                                      "3D corners\0"));
            ImGui::SetNextItemWidth(275.0f);
            ImGui::SliderFloat3(phrases[XorString("global_scale")].c_str(), sharedConfig.box.scale.data(), 0.0f, 0.50f, "%.2f");
            ImGuiCustom::colorPicker(phrases[XorString("global_fill")].c_str(), sharedConfig.box.fill);
            ImGui::EndPopup();
        }

        ImGui::PopID();

        ImGuiCustom::colorPicker(phrases[XorString("esp_name")].c_str(), sharedConfig.name);
        ImGui::SameLine(spacing);

        if (currentCategory < 2)
        {
            auto &playerConfig = getConfigPlayer(currentCategory, currentItem);

            ImGuiCustom::colorPicker(phrases[XorString("esp_weapon")].c_str(), playerConfig.weapon);
            ImGuiCustom::colorPicker(phrases[XorString("esp_flashduration")].c_str(), playerConfig.flashDuration);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker(phrases[XorString("esp_skeleton")].c_str(), playerConfig.skeleton);
            ImGui::Checkbox(phrases[XorString("esp_audibleonly")].c_str(), &playerConfig.audibleOnly);
            ImGui::SameLine(spacing);
            ImGui::Checkbox(phrases[XorString("esp_spottedonly")].c_str(), &playerConfig.spottedOnly);

            ImGuiCustom::colorPicker(phrases[XorString("esp_headbox")].c_str(), playerConfig.headBox);
            ImGui::SameLine();

            ImGui::PushID(XorString("Head Box"));

            if (ImGui::Button(phrases[XorString("global_threedots")].c_str()))
                ImGui::OpenPopup("");

            if (ImGui::BeginPopup(""))
            {
                ImGui::SetNextItemWidth(95.0f);
                ImGui::Combo(phrases[XorString("global_type")].c_str(), &playerConfig.headBox.type, XorString("2D\0"
                                                                                                              "2D corners\0"
                                                                                                              "3D\0"
                                                                                                              "3D corners\0"));
                ImGui::SetNextItemWidth(275.0f);
                ImGui::SliderFloat3(phrases[XorString("global_scale")].c_str(), playerConfig.headBox.scale.data(), 0.0f, 0.50f, "%.2f");
                ImGuiCustom::colorPicker(phrases[XorString("global_fill")].c_str(), playerConfig.headBox.fill);
                ImGui::EndPopup();
            }

            ImGui::PopID();

            ImGui::SameLine(spacing);

            ImGui::Checkbox(phrases[XorString("esp_healthbar")].c_str(), &playerConfig.healthBar);
        }
        else if (currentCategory == 2)
        {
            auto &weaponConfig = config->streamProofESP.weapons[currentItem];
            ImGuiCustom::colorPicker(phrases[XorString("esp_ammo")].c_str(), weaponConfig.ammo);
        }
        else if (currentCategory == 3)
        {
            auto &trails = config->streamProofESP.projectiles[currentItem].trails;

            ImGui::Checkbox(phrases[XorString("esp_trails")].c_str(), &trails.enabled);
            ImGui::SameLine(spacing + 77.0f);
            ImGui::PushID(XorString("Trails"));

            if (ImGui::Button(phrases[XorString("global_threedots")].c_str()))
                ImGui::OpenPopup("");

            if (ImGui::BeginPopup(""))
            {
                constexpr auto trailPicker = [](const char *name, Trail &trail) noexcept
                {
                    ImGui::PushID(name);
                    ImGuiCustom::colorPicker(name, trail);
                    ImGui::SameLine(150.0f);
                    ImGui::SetNextItemWidth(95.0f);
                    ImGui::Combo("", &trail.type, XorString("Line\0Circles\0Filled Circles\0"));
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(95.0f);
                    ImGui::InputFloat(phrases[XorString("global_time")].c_str(), &trail.time, 0.1f, 0.5f, "%.1fs");
                    trail.time = std::clamp(trail.time, 1.0f, 60.0f);
                    ImGui::PopID();
                };

                trailPicker(XorString("Local Player"), trails.localPlayer);
                trailPicker(XorString("Allies"), trails.allies);
                trailPicker(XorString("Enemies"), trails.enemies);
                ImGui::EndPopup();
            }

            ImGui::PopID();
        }

        ImGui::SetNextItemWidth(95.0f);
        ImGui::InputFloat(phrases[XorString("esp_textculldistance")].c_str(), &sharedConfig.textCullDistance, 0.4f, 0.8f, "%.1fm");
        sharedConfig.textCullDistance = std::clamp(sharedConfig.textCullDistance, 0.0f, 999.9f);
    }

    ImGui::EndChild();

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderVisualsWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.visuals)
            return;
        ImGui::SetNextWindowSize({680.0f, 0.0f});
        ImGui::Begin(phrases[XorString("window_visuals")].c_str(), &window.visuals, windowFlags);
    }
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 280.0f);
    constexpr auto playerModels = "Default\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0Pirate\0Pirate Variant A\0Pirate Variant B\0Pirate Variant C\0Pirate Variant D\0Anarchist\0Anarchist Variant A\0Anarchist Variant B\0Anarchist Variant C\0Anarchist Variant D\0Balkan Variant A\0Balkan Variant B\0Balkan Variant C\0Balkan Variant D\0Balkan Variant E\0Jumpsuit Variant A\0Jumpsuit Variant B\0Jumpsuit Variant C\0";
    ImGui::Checkbox(phrases[XorString("visuals_noweapons")].c_str(), &config->visuals.noWeapons);
    ImGui::Checkbox(phrases[XorString("visuals_nosmoke")].c_str(), &config->visuals.noSmoke);
    ImGui::Checkbox(phrases[XorString("visuals_noscopeoverlay")].c_str(), &config->visuals.noScopeOverlay);
    ImGui::Checkbox(phrases[XorString("visuals_inverseragdoll")].c_str(), &config->visuals.inverseRagdollGravity);

    ImGui::Checkbox(phrases[XorString("visuals_noaimpunch")].c_str(), &config->visuals.noAimPunch);
    ImGui::Checkbox(phrases[XorString("visuals_noviewpunch")].c_str(), &config->visuals.noViewPunch);
    ImGui::Checkbox(phrases[XorString("visuals_nohands")].c_str(), &config->visuals.noHands);
    ImGui::Checkbox(phrases[XorString("visuals_nosleeves")].c_str(), &config->visuals.noSleeves);
    ImGui::Checkbox(phrases[XorString("visuals_wireframesmoke")].c_str(), &config->visuals.wireframeSmoke);
    ImGui::Checkbox(phrases[XorString("visuals_deaglespinner")].c_str(), &config->visuals.deagleSpinner);
    ImGuiCustom::colorPicker(phrases[XorString("visuals_bullettracers")].c_str(), config->visuals.bulletTracers);
    ImGui::Checkbox(phrases[XorString("visuals_zoom")].c_str(), &config->visuals.zoom);
    ImGui::SameLine();
    hotkey(config->visuals.zoomKey);
    ImGui::Combo(phrases[XorString("visuals_t_model")].c_str(), &config->visuals.playerModelT, playerModels);
    ImGui::Combo(phrases[XorString("visuals_ct_model")].c_str(), &config->visuals.playerModelCT, playerModels);
    ImGuiCustom::colorPicker(phrases[XorString("visuals_watermark")].c_str(), config->misc.watermark);
    ImGui::SameLine();
    ImGui::InputText("", &config->misc.waterMarkString);

    ImGui::Checkbox(phrases[XorString("visuals_ragdollforce")].c_str(), &config->misc.ragdollForce);
    if (config->misc.ragdollForce)
    {
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputInt(phrases[XorString("visuals_ragdollforce_strenght")].c_str(), &config->misc.ragdollForceStrength, 1, 3);
        config->misc.ragdollForceStrength = std::clamp(config->misc.ragdollForceStrength, 1, 100);
    };
    ImGui::NextColumn();
    ImGui::Checkbox(phrases[XorString("visuals_thirdperson")].c_str(), &config->visuals.thirdperson);
    ImGui::SameLine();
    hotkey(config->visuals.thirdpersonKey);
    ImGui::PushItemWidth(290.0f);
    ImGui::PushID(0);
    if (config->visuals.thirdperson) {
        ImGui::SliderInt("", &config->visuals.thirdpersonDistance, 0, 1000, phrases[XorString("visuals_thirdperson_distance")].c_str());
    }
    ImGui::PopID();
    ImGui::PushID(1);
    ImGui::SliderInt("", &config->visuals.viewmodelFov, -60, 60, phrases[XorString("visuals_viewmodelfov")].c_str());
    ImGui::PopID();
    ImGui::PushID(2);
    ImGui::SliderInt("", &config->visuals.fov, -60, 60, phrases[XorString("visuals_fov")].c_str());
    ImGui::PopID();
    ImGui::PushID(3);
    ImGui::SliderInt("", &config->visuals.farZ, 0, 2000, phrases[XorString("visuals_farz")].c_str());
    ImGui::PopID();
    ImGui::PushID(4);
    ImGui::SliderInt("", &config->visuals.flashReduction, 0, 100, phrases[XorString("visuals_flashreduction")].c_str());
    ImGui::PopID();
    ImGui::PushID(5);
    ImGui::SliderFloat("", &config->visuals.brightness, 0.0f, 1.0f, phrases[XorString("visuals_brightness")].c_str());
    ImGui::PopID();
    ImGui::PopItemWidth();
    ImGui::Combo(phrases[XorString("visuals_hitmarker")].c_str(), &config->visuals.hitMarker, XorString("None\0Default (Cross)\0"));
    ImGui::SliderFloat(phrases[XorString("visuals_hitmarker_time")].c_str(), &config->visuals.hitMarkerTime, 0.1f, 1.5f, XorString("%.2fs"));
    ImGui::Checkbox(phrases[XorString("visuals_indicators")].c_str(), &config->visuals.indicatorsEnabled);
    ImGui::SameLine();
    ImGui::PushID(6);
    ImGui::SetNextItemWidth(175.0f);
    ImGuiCustom::MultiCombo("", config->visuals.indicators, config->visuals.selectedIndicators, 5);
    ImGui::PopID();
    ImGui::Checkbox(phrases[XorString("visuals_rainbowcrosshair")].c_str(), &config->visuals.rainbowCrosshair);
    ImGui::SameLine();
    ImGui::PushItemWidth(100.0f);
    config->visuals.rainbowCrosshairSpeed = std::clamp(config->visuals.rainbowCrosshairSpeed, 0.0f, 25.0f);
    ImGui::InputFloat(phrases[XorString("global_speed")].c_str(), &config->visuals.rainbowCrosshairSpeed, 0.1f, 0.15f, "%.2f");
    ImGui::PopItemWidth();
    ImGuiCustom::colorPicker(phrases[XorString("visuals_noscopecrosshair")].c_str(), config->misc.noscopeCrosshair);
    ImGuiCustom::colorPicker(phrases[XorString("visuals_recoilcrosshair")].c_str(), config->misc.recoilCrosshair);

    ImGui::Columns(1);

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderWorldWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.world)
            return;
        ImGui::SetNextWindowSize({700.0f, 0.0f});
        ImGui::Begin(phrases[XorString("window_world")].c_str(), &window.world, windowFlags);
    }
    ImGui::Columns(2);
    ImGui::Combo(phrases[XorString("visuals_skybox")].c_str(), &config->visuals.skybox, Helpers::skyboxList.data(), Helpers::skyboxList.size());
    ImGuiCustom::colorPicker(phrases[XorString("visuals_worldcolor")].c_str(), config->visuals.world);
    ImGuiCustom::colorPicker(phrases[XorString("visuals_skycolor")].c_str(), config->visuals.sky);

    /*
    ImGui::Checkbox(phrases[XorString("visuals_nightmode")].c_str(), &config->visuals.nightMode);
    ImGui::Checkbox(phrases[XorString("visuals_asuswalls")].c_str(), &config->visuals.asusWalls);
    */
    ImGui::NextColumn();
    ImGui::Checkbox(phrases[XorString("visuals_colorcorrection")].c_str(), &config->visuals.colorCorrection.enabled);
    ImGui::SameLine();
    bool ccPopup = ImGui::Button(phrases[XorString("global_edit")].c_str());
    if (ccPopup)
        ImGui::OpenPopup(XorString("##popup"));

    if (ImGui::BeginPopup(XorString("##popup")))
    {
        ImGui::VSliderFloat(XorString("##1"), {40.0f, 160.0f}, &config->visuals.colorCorrection.blue, 0.0f, 1.0f, XorString("Blue\n%.3f"));
        ImGui::SameLine();
        ImGui::VSliderFloat(XorString("##2"), {40.0f, 160.0f}, &config->visuals.colorCorrection.red, 0.0f, 1.0f, XorString("Red\n%.3f"));
        ImGui::SameLine();
        ImGui::VSliderFloat(XorString("##3"), {40.0f, 160.0f}, &config->visuals.colorCorrection.mono, 0.0f, 1.0f, XorString("Mono\n%.3f"));
        ImGui::SameLine();
        ImGui::VSliderFloat(XorString("##4"), {40.0f, 160.0f}, &config->visuals.colorCorrection.saturation, 0.0f, 1.0f, XorString("Sat\n%.3f"));
        ImGui::SameLine();
        ImGui::VSliderFloat(XorString("##5"), {40.0f, 160.0f}, &config->visuals.colorCorrection.ghost, 0.0f, 1.0f, XorString("Ghost\n%.3f"));
        ImGui::SameLine();
        ImGui::VSliderFloat(XorString("##6"), {40.0f, 160.0f}, &config->visuals.colorCorrection.green, 0.0f, 1.0f, XorString("Green\n%.3f"));
        ImGui::SameLine();
        ImGui::VSliderFloat(XorString("##7"), {40.0f, 160.0f}, &config->visuals.colorCorrection.yellow, 0.0f, 1.0f, XorString("Yellow\n%.3f"));
        ImGui::SameLine();
        ImGui::EndPopup();
    }

    ImGui::Columns(1);

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderSkinChangerWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.skinChanger)
            return;
        ImGui::SetNextWindowSize({800.0f, 0.0f});
        ImGui::Begin(phrases[XorString("window_skinchanger")].c_str(), &window.skinChanger, windowFlags);
    }

    static auto itemIndex = 0;

    ImGui::PushItemWidth(110.0f);
    ImGui::Combo(
        "##1", &itemIndex, [](void *data, int idx, const char **out_text)
        {
            *out_text = SkinChanger::weapon_names[idx].name;
            return true;
        },
        nullptr, SkinChanger::weapon_names.size(), 5);
    ImGui::PopItemWidth();

    auto &selected_entry = config->skinChanger[itemIndex];
    selected_entry.itemIdIndex = itemIndex;

    {
        ImGui::SameLine();
        ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &selected_entry.enabled);
        ImGui::Separator();
        ImGui::Columns(2, nullptr, false);

        static std::string filter;
        ImGui::PushID(XorString("Search"));
        ImGui::InputTextWithHint("", phrases[XorString("global_search")].c_str(), &filter);
        ImGui::PopID();

        if (ImGui::ListBoxHeader(phrases[XorString("skinchanger_paintkit")].c_str()))
        {
            const auto &kits = itemIndex == 1 ? SkinChanger::getGloveKits() : SkinChanger::getSkinKits();

            const std::locale original;
            std::locale::global(std::locale{"en_US.utf8"});

            const auto &facet = std::use_facet<std::ctype<wchar_t>>(std::locale{});
            std::wstring filterWide(filter.length(), L'\0');
            const auto newLen = mbstowcs(filterWide.data(), filter.c_str(), filter.length());
            if (newLen != static_cast<std::size_t>(-1))
                filterWide.resize(newLen);
            std::transform(filterWide.begin(), filterWide.end(), filterWide.begin(), [&facet](wchar_t w)
                           { return facet.toupper(w); });

            std::locale::global(original);

            for (std::size_t i = 0; i < kits.size(); ++i)
            {
                if (filter.empty() || wcsstr(kits[i].nameUpperCase.c_str(), filterWide.c_str()))
                {
                    ImGui::PushID(i);
                    if (ImGui::Selectable(kits[i].name.c_str(), i == selected_entry.paint_kit_vector_index))
                        selected_entry.paint_kit_vector_index = i;

                    if (ImGui::IsItemHovered())
                    {
                        if (const auto icon = SkinChanger::getItemIconTexture(kits[i].iconPath))
                        {
                            ImGui::BeginTooltip();
                            ImGui::Image(icon, {200.0f, 150.0f});
                            ImGui::EndTooltip();
                        }
                    } //
                    ImGui::PopID();
                }
            }
            ImGui::ListBoxFooter();
        }

        ImGui::Combo(
            phrases[XorString("skinchanger_quality")].c_str(), &selected_entry.entity_quality_vector_index, [](void *data, int idx, const char **out_text)
            {
                *out_text = SkinChanger::getQualities()[idx].name.c_str();
                return true;
            },
            nullptr, SkinChanger::getQualities().size(), 5);

        ImGui::InputInt(phrases[XorString("skinchanger_seed")].c_str(), &selected_entry.seed);

        ImGui::InputInt(phrases[XorString("skinchanger_stattrak")].c_str(), &selected_entry.stat_trak);

        selected_entry.stat_trak = (std::max)(selected_entry.stat_trak, -1);

        ImGui::SliderFloat(phrases[XorString("skinchanger_wear")].c_str(), &selected_entry.wear, FLT_MIN, 1.f, XorString("%.10f"), ImGuiSliderFlags_Logarithmic);
        if (itemIndex == 0)
        {
            ImGui::Combo(
                phrases[XorString("skinchanger_knife")].c_str(), &selected_entry.definition_override_vector_index, [](void *data, int idx, const char **out_text)
                {
                    *out_text = SkinChanger::getKnifeTypes()[idx].name.c_str();
                    return true;
                },
                nullptr, SkinChanger::getKnifeTypes().size(), 5);
        }
        else if (itemIndex == 1)
        {
            ImGui::Combo(
                phrases[XorString("skinchanger_glove")].c_str(), &selected_entry.definition_override_vector_index, [](void *data, int idx, const char **out_text)
                {
                    *out_text = SkinChanger::getGloveTypes()[idx].name.c_str();
                    return true;
                },
                nullptr, SkinChanger::getGloveTypes().size(), 5);
        }

        ImGui::InputText(phrases[XorString("skinchanger_nametag")].c_str(), selected_entry.custom_name, 32);
    }

    ImGui::NextColumn();

    {
        ImGui::PushID(XorString("sticker"));

        static std::size_t selectedStickerSlot = 0;

        ImGui::PushItemWidth(-1);

        if (ImGui::ListBoxHeader("", 5))
        {
            for (int i = 0; i < 5; ++i)
            {
                ImGui::PushID(i);

                const auto kit_vector_index = config->skinChanger[itemIndex].stickers[i].kit_vector_index;
                const std::string text = '#' + std::to_string(i + 1) + "  " + SkinChanger::getStickerKits()[kit_vector_index].name;

                if (ImGui::Selectable(text.c_str(), i == selectedStickerSlot))
                    selectedStickerSlot = i;

                ImGui::PopID();
            }
            ImGui::ListBoxFooter();
        }

        ImGui::PopItemWidth();

        auto &selected_sticker = selected_entry.stickers[selectedStickerSlot];

        static std::string filter;
        ImGui::PushID(XorString("Search"));
        ImGui::InputTextWithHint("", phrases[XorString("global_search")].c_str(), &filter);
        ImGui::PopID();

        if (ImGui::ListBoxHeader(phrases[XorString("skinchanger_sticker")].c_str()))
        {
            const auto &kits = SkinChanger::getStickerKits();

            const std::locale original;
            std::locale::global(std::locale{"en_US.utf8"});

            const auto &facet = std::use_facet<std::ctype<wchar_t>>(std::locale{});
            std::wstring filterWide(filter.length(), L'\0');
            const auto newLen = mbstowcs(filterWide.data(), filter.c_str(), filter.length());
            if (newLen != static_cast<std::size_t>(-1))
                filterWide.resize(newLen);
            std::transform(filterWide.begin(), filterWide.end(), filterWide.begin(), [&facet](wchar_t w)
                           { return facet.toupper(w); });

            std::locale::global(original);

            for (std::size_t i = 0; i < kits.size(); ++i)
            {
                if (filter.empty() || wcsstr(kits[i].nameUpperCase.c_str(), filterWide.c_str()))
                {
                    ImGui::PushID(i);
                    if (ImGui::Selectable(kits[i].name.c_str(), i == selected_sticker.kit_vector_index))
                        selected_sticker.kit_vector_index = i;

                    if (ImGui::IsItemHovered())
                    {
                        if (const auto icon = SkinChanger::getItemIconTexture(kits[i].iconPath))
                        {
                            ImGui::BeginTooltip();
                            ImGui::Image(icon, {200.0f, 150.0f});
                            ImGui::EndTooltip();
                        }
                    }

                    ImGui::PopID();
                }
            }
            ImGui::ListBoxFooter();
        }

        ImGui::SliderFloat(phrases[XorString("skinchanger_wear")].c_str(), &selected_sticker.wear, FLT_MIN, 1.0f, "%.10f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat(phrases[XorString("skinchanger_scale")].c_str(), &selected_sticker.scale, 0.1f, 5.0f);
        ImGui::SliderFloat(phrases[XorString("skinchanger_rotation")].c_str(), &selected_sticker.rotation, 0.0f, 360.0f);

        ImGui::PopID();
    }
    selected_entry.update();

    ImGui::Columns(1);

    ImGui::Separator();

    if (ImGui::Button(phrases[XorString("global_update")].c_str(), {130.0f, 30.0f}))
        SkinChanger::scheduleHudUpdate();

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderMiscWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.misc)
            return;
        ImGui::SetNextWindowSize({580.0f, 0.0f});
        ImGui::Begin(phrases[XorString("window_misc")].c_str(), &window.misc, windowFlags);
    }
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 230.0f);

    ImGui::Checkbox(phrases[XorString("misc_radarhack")].c_str(), &config->misc.radarHack);

    ImGui::Checkbox(phrases[XorString("misc_spectatorlist")].c_str(), &config->misc.spectatorList.enabled);
    ImGui::SameLine();

    ImGui::PushID(phrases[XorString("misc_spectatorlist")].c_str());
    if (ImGui::Button(phrases[XorString("global_threedots")].c_str()))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup(""))
    {
        ImGui::Checkbox(phrases[XorString("global_notitle")].c_str(), &config->misc.spectatorList.noTitleBar);
        ImGui::EndPopup();
    }
    ImGui::PopID();

    ImGui::Checkbox(phrases[XorString("misc_revealranks")].c_str(), &config->misc.revealRanks);
    ImGui::Checkbox(phrases[XorString("misc_revealmoney")].c_str(), &config->misc.revealMoney);
    ImGui::Checkbox(phrases[XorString("misc_revealsuspect")].c_str(), &config->misc.revealSuspect);
    ImGui::Checkbox(phrases[XorString("misc_disablemodelocclusion")].c_str(), &config->misc.disableModelOcclusion);
    ImGui::Checkbox(phrases[XorString("misc_antiafkkick")].c_str(), &config->misc.antiAfkKick);
    ImGui::Checkbox(phrases[XorString("misc_autostrafe")].c_str(), &config->misc.autoStrafe);
    ImGui::Checkbox(phrases[XorString("misc_bunnyhop")].c_str(), &config->misc.bunnyHop);
    ImGui::Checkbox(phrases[XorString("misc_fastduck")].c_str(), &config->misc.fastDuck);
    ImGui::Checkbox(phrases[XorString("misc_slowwalk")].c_str(), &config->misc.slowwalk);
    ImGui::SameLine();
    hotkey(config->misc.slowwalkKey);
    ImGui::Checkbox(phrases[XorString("misc_autopistol")].c_str(), &config->misc.autoPistol);
    ImGui::Checkbox(phrases[XorString("misc_autoreload")].c_str(), &config->misc.autoReload);
    ImGui::Checkbox(phrases[XorString("misc_autoaccept")].c_str(), &config->misc.autoAccept);
    ImGui::Checkbox(phrases[XorString("misc_disablehudblur")].c_str(), &config->misc.disablePanoramablur);
    ImGui::Checkbox(phrases[XorString("misc_fastplant")].c_str(), &config->misc.fastPlant);
    ImGui::Checkbox(phrases[XorString("misc_faststop")].c_str(), &config->misc.fastStop);
    ImGuiCustom::colorPicker(phrases[XorString("misc_bombtimer")].c_str(), config->misc.bombTimer);
    ImGui::Checkbox(phrases[XorString("misc_bombdamage")].c_str(), &config->misc.bombDamage);
    ImGui::Checkbox(phrases[XorString("misc_grenadeprediction")].c_str(), &config->misc.nadePredict);
    /*ImGui::PushID("Cvar");
    ImGui::Checkbox("Cvar Force", &config->misc.cvarEnabled);
    ImGui::SameLine();
    if (ImGui::Button("..."))
        ImGui::OpenPopup("");
    if (ImGui::BeginPopup("")) {
        ImGui::InputText("Cvar", &config->misc.cvar); ImGui::SameLine(); ImGui::InputInt("##Valor", &config->misc.cvarValue);
        ImGui::EndPopup();
    }
    ImGui::PopID();*/

    ImGui::NextColumn(); //////////////////////////////////////////////////////////////////////

    ImGui::Checkbox(phrases[XorString("misc_customclantag")].c_str(), &config->misc.customClanTag);
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(0);
    if (ImGui::InputText("", config->misc.clanTag, sizeof(config->misc.clanTag)))
        Misc::updateClanTag(true);
    ImGui::PopID();

    if (config->misc.customClanTag) {
        ImGui::Checkbox(phrases[XorString("misc_animatedclantag")].c_str(), &config->misc.animatedClanTag);
    }

    ImGui::Checkbox(phrases[XorString("misc_clocktag")].c_str(), &config->misc.clocktag);
    ImGui::Checkbox(phrases[XorString("misc_killmessage")].c_str(), &config->misc.killMessage);
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(1);
    ImGui::InputText("", &config->misc.killMessageString);
    ImGui::PopID();

    ImGui::Combo(phrases[XorString("misc_hitsound")].c_str(), &config->misc.hitSound, XorString("None\0Metal\0Gamesense\0Bell\0Glass\0Custom\0"));
    if (config->misc.hitSound == 5)
    {
        ImGui::InputText(phrases[XorString("misc_hitsound_filename")].c_str(), &config->misc.customHitSound);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(XorString("audio file must be put in csgo/sound/ directory"));
    }
    ImGui::PushID(5);
    ImGui::Combo(phrases[XorString("misc_killsound")].c_str(), &config->misc.killSound, XorString("None\0Metal\0Gamesense\0Bell\0Glass\0Custom\0"));
    if (config->misc.killSound == 5)
    {
        ImGui::InputText(phrases[XorString("misc_killsound_filename")].c_str(), &config->misc.customKillSound);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(XorString("audio file must be put in csgo/sound/ directory"));
    }
    ImGui::PopID();

    ImGui::Combo(phrases[XorString("misc_fakelag")].c_str(), &config->misc.fakeLagMode, XorString("Off\0Normal\0Adaptive\0Random\0Switch\0"));
    ImGui::SameLine();
    hotkey(config->misc.fakeLagKey);
    if (!(config->misc.fakeLagMode == 0))
    {
        ImGuiCustom::MultiCombo(phrases[XorString("global_flags")].c_str(), config->misc.fakeLagFlags, config->misc.fakeLagSelectedFlags, 4);
        if (config->misc.fakeLagMode == 3)
        {
            ImGui::SetNextItemWidth(120.0f);
            ImGui::SliderInt(phrases[XorString("misc_min_fakelag_amount")].c_str(), &config->misc.fakeLagTicks, 1, 16);
        }
        else if (!(config->misc.fakeLagMode == 4))
        {
            ImGui::SetNextItemWidth(120.0f);
            ImGui::SliderInt(phrases[XorString("misc_fakelag_amount")].c_str(), &config->misc.fakeLagTicks, 1, 16);
        }
    }

    ImGui::SetNextItemWidth(120.0f);
    ImGui::SliderFloat(phrases[XorString("misc_maxangledelta")].c_str(), &config->misc.maxAngleDelta, 0.0f, 255.0f, "%.2f");
    ImGui::SliderFloat(phrases[XorString("misc_aspectratio")].c_str(), &config->misc.aspectratio, 0.0f, 5.0f, "%.2f");
    ImGui::Checkbox(phrases[XorString("misc_quickreload")].c_str(), &config->misc.quickReload);
    ImGui::Checkbox(phrases[XorString("misc_preparerevolver")].c_str(), &config->misc.prepareRevolver);
    ImGui::SameLine();
    hotkey(config->misc.prepareRevolverKey);

    ImGui::Checkbox(phrases[XorString("misc_fakeduck")].c_str(), &config->misc.fakeDuck);
    ImGui::SameLine();
    hotkey(config->misc.fakeDuckKey);

    ImGui::Checkbox(phrases[XorString("misc_doorspam")].c_str(), &config->misc.doorSpam);
    ImGui::Checkbox(phrases[XorString("misc_preservekillfeed")].c_str(), &config->misc.preserveKillfeed.enabled);
    ImGui::SameLine();

    ImGui::PushID(XorString("Preserve Killfeed"));
    if (ImGui::Button(phrases[XorString("global_threedots")].c_str()))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup(""))
    {
        ImGui::Checkbox(phrases[XorString("misc_onlyheadshots")].c_str(), &config->misc.preserveKillfeed.onlyHeadshots);
        ImGui::EndPopup();
    }
    ImGui::PopID();

    ImGui::Checkbox(phrases[XorString("misc_purchaselist")].c_str(), &config->misc.purchaseList.enabled);
    ImGui::SameLine();

    ImGui::PushID(XorString("Purchase List"));
    if (ImGui::Button(phrases[XorString("global_threedots")].c_str()))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup(""))
    {
        ImGui::SetNextItemWidth(75.0f);
        ImGui::Combo(phrases[XorString("global_mode")].c_str(), &config->misc.purchaseList.mode, XorString("Details\0Summary\0"));
        ImGui::Checkbox(phrases[XorString("misc_onlyduringfreezetime")].c_str(), &config->misc.purchaseList.onlyDuringFreezeTime);
        ImGui::Checkbox(phrases[XorString("misc_showprices")].c_str(), &config->misc.purchaseList.showPrices);
        ImGui::Checkbox(phrases[XorString("global_notitle")].c_str(), &config->misc.purchaseList.noTitleBar);
        ImGui::EndPopup();
    }
    ImGui::PopID();

    ImGui::Checkbox(phrases[XorString("misc_reportbot")].c_str(), &config->misc.reportbot.enabled);
    ImGui::SameLine();
    ImGui::PushID(XorString("Reportbot"));

    if (ImGui::Button(phrases[XorString("global_threedots")].c_str()))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup(""))
    {
        ImGui::PushItemWidth(80.0f);
        ImGui::Combo(phrases[XorString("misc_reportbot_target")].c_str(), &config->misc.reportbot.target, XorString("Enemies\0Allies\0All\0"));
        ImGui::InputInt(phrases[XorString("misc_reportbot_delay")].c_str(), &config->misc.reportbot.delay);
        config->misc.reportbot.delay = (std::max)(config->misc.reportbot.delay, 1);
        ImGui::InputInt(phrases[XorString("misc_reportbot_rounds")].c_str(), &config->misc.reportbot.rounds);
        config->misc.reportbot.rounds = (std::max)(config->misc.reportbot.rounds, 1);
        ImGui::PopItemWidth();
        ImGui::Checkbox(phrases[XorString("misc_reportbot_abusivecomms")].c_str(), &config->misc.reportbot.textAbuse);
        ImGui::Checkbox(phrases[XorString("misc_reportbot_griefing")].c_str(), &config->misc.reportbot.griefing);
        ImGui::Checkbox(phrases[XorString("misc_reportbot_wallhacking")].c_str(), &config->misc.reportbot.wallhack);
        ImGui::Checkbox(phrases[XorString("misc_reportbot_aimhacking")].c_str(), &config->misc.reportbot.aimbot);
        ImGui::Checkbox(phrases[XorString("misc_reportbot_otherhacking")].c_str(), &config->misc.reportbot.other);
        if (ImGui::Button(phrases[XorString("misc_reportbot_reset")].c_str()))
            Misc::resetReportbot();
        ImGui::EndPopup();
    }
    ImGui::PopID();

    ImGui::PushID(phrases[XorString("misc_chatspammer")].c_str());
    ImGui::Combo(phrases[XorString("misc_chatspammer")].c_str(), &config->misc.spam.spam_picker, "Off\0Local\0Custom\0", IM_ARRAYSIZE("Off\0Local\0Custom\0"));
    ImGui::SameLine();
    if (ImGui::Button("..."))
        ImGui::OpenPopup("");
    if (ImGui::BeginPopup(""))
    {
        if (config->misc.spam.spam_picker == 1)
        {

            ImGui::Checkbox("Team Chat (?)###SAY_TEAM2", &config->misc.spam.say);

            ImGui::PushItemWidth(185);
            ImGui::Combo("###POSITIONSTEAM", &config->misc.spam.team, "Allies\0Enemies\0Both\0", IM_ARRAYSIZE("Allies\0Enemies\0Both\0"));
            ImGui::PopItemWidth();
        }
        if (config->misc.spam.spam_picker == 2)
        {
            ImGui::InputText("Text:##", &config->misc.spam.text);
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
    ImGui::TextUnformatted(phrases[XorString("misc_menukey")].c_str());
    ImGui::SameLine();
    hotkey(config->misc.menuKey);

#ifdef _DEBUG
    //if (ImGui::Button("Unload"))
    //    hooks->uninstall();
#endif

    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderConfigWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.config)
            return;
        ImGui::SetNextWindowSize({290.0f, 0.0f});
        ImGui::Begin(phrases[XorString("window_config")].c_str(), &window.config, windowFlags);
    }
    const auto configItems = config->getConfigs();
    static int currentConfig = -1;

    static std::string buffer;
    static std::string cfgName;
    static std::string filter;

    timeToNextConfigRefresh -= ImGui::GetIO().DeltaTime;
    if (timeToNextConfigRefresh <= 0.0f)
    {
        config->listConfigs();
        if (const auto it = std::find(configItems.begin(), configItems.end(), buffer); it != configItems.end())
            currentConfig = std::distance(configItems.begin(), it);
        timeToNextConfigRefresh = 0.1f;
    }

    if (static_cast<std::size_t>(currentConfig) >= configItems.size())
        currentConfig = -1;

    ImGui::SetNextItemWidth(-1.0f);
    if (ImGui::ListBoxHeader("##ConfigList"))
    {
        const std::locale original;
        std::locale::global(std::locale{"en_US.utf8"});

        const auto &facet = std::use_facet<std::ctype<wchar_t>>(std::locale{});
        std::wstring filterWide(filter.length(), L'\0');
        std::locale::global(original);
        for (std::size_t i = 0; i < configItems.size(); ++i)
        {
            if (filter.empty() || strstr(configItems[i].c_str(), filter.c_str()))
            {
                ImGui::PushID(i);
                ImGui::PushFont(fonts.segoeuiSized);
                ImGui::PushFont(fonts.iconsSized);
                if (ImGui::Selectable(configItems[i].c_str(), i == currentConfig, ImGuiSelectableFlags_SpanAvailWidth, ImVec2{470.0f, 30.0f}))
                    currentConfig = i;
                if (ImGui::BeginPopupContextItem(configItems[i].c_str()))
                {
                    ImGui::Text(phrases[XorString("config_using")].c_str(), configItems[i].c_str());
                    ImGui::BeginGroup();
                    ImGui::Text(ICON_FA_CHECK_SQUARE);
                    ImGui::SameLine();
                    ImGui::Text(phrases[XorString("config_load")].c_str());
                    ImGui::EndGroup();
                    if (ImGui::IsItemClicked())
                    {
                        config->load(i);
                        updateColors();
                        SkinChanger::scheduleHudUpdate();
                        Misc::updateClanTag(true);
                    }
                    ImGui::Separator();
                    ImGui::BeginGroup();
                    ImGui::Text(ICON_FA_SAVE);
                    ImGui::SameLine();
                    ImGui::Text(phrases[XorString("config_save")].c_str());
                    ImGui::EndGroup();
                    if (ImGui::IsItemClicked())
                        config->save(i);
                    ImGui::Separator();
                    ImGui::BeginGroup();
                    ImGui::Text(ICON_FA_TRASH_ALT);
                    ImGui::SameLine();
                    ImGui::Text(phrases[XorString("config_delete")].c_str());
                    ImGui::EndGroup();
                    if (ImGui::IsItemClicked())
                    {
                        config->remove(i);
                        if (static_cast<std::size_t>(currentConfig) < configItems.size())
                            buffer = configItems[currentConfig];
                        else
                            buffer.clear();
                    }

                    ImGui::EndPopup();
                }
                ImGui::PopFont();
                ImGui::PopFont();
                ImGui::Separator();
                ImGui::PopID();
            }
        }
        ImGui::ListBoxFooter();
    }

    ImGui::PushID(0);
    ImGui::InputTextWithHint("##Search", phrases[XorString("global_search")].c_str(), &filter);
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_PLUS, {50.0f, 22.0f}))
        ImGui::OpenPopup("NewCfg");

    if (ImGui::BeginPopup("NewCfg", ImGuiPopupFlags_MouseButtonLeft))
    {
        ImGui::InputTextWithHint("##ConfigName", phrases[XorString("config_placeholder_configname")].c_str(), &cfgName);
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_PLUS_CIRCLE))
        {
            config->add(cfgName.c_str());
            cfgName.clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::SameLine();

    if (ImGui::Button(ICON_FA_BACKSPACE, {50.0f, 22.0f}))
        ImGui::OpenPopup(phrases[XorString("config_toreset")].c_str());

    ImGui::SameLine();

    if (ImGui::Button(ICON_FA_FOLDER_OPEN, {50.0f, 22.0f}))
        config->openConfigDir();

    if (ImGui::BeginPopup(phrases[XorString("config_toreset")].c_str()))
    {
        static constexpr const char *names[]{"Whole", "Aimbot", "Ragebot", "Triggerbot", "Backtrack", "Anti aim", "Glow", "Chams", "ESP", "Visuals", "Skin changer", "Sound", "Style", "Misc"};
        for (int i = 0; i < IM_ARRAYSIZE(names); i++)
        {
            if (i == 1)
                ImGui::Separator();

            if (ImGui::Selectable(names[i]))
            {
                switch (i)
                {
                case 0:
                    config->reset();
                    updateColors();
                    Misc::updateClanTag(true);
                    SkinChanger::scheduleHudUpdate();
                    break;
                case 1:
                    config->aimbot = {};
                    break;
                case 2:
                    config->ragebot = {};
                    break;
                case 3:
                    config->triggerbot = {};
                    break;
                case 4:
                    config->backtrack = {};
                    break;
                case 5:
                    config->antiAim = {};
                    break;
                case 6:
                    config->glow = {};
                    break;
                case 7:
                    config->chams = {};
                    break;
                case 8:
                    config->streamProofESP = {};
                    break;
                case 9:
                    config->visuals = {};
                    break;
                case 10:
                    config->skinChanger = {};
                    SkinChanger::scheduleHudUpdate();
                    break;
                case 11:
                    config->sound = {};
                    break;
                case 12:
                    config->style = {};
                    updateColors();
                    break;
                case 13:
                    config->misc = {};
                    Misc::updateClanTag(true);
                    break;
                }
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderProfileChangerWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.profileChanger)
            return;
        ImGui::SetNextWindowSize({290.0f, 0.0f});
        ImGui::Begin("Profile Changer | Cheat", &window.profileChanger, windowFlags);
    }
    if (!config->globals.profileChangerHook)
    {
        ImGui::PushFont(fonts.segoeuiSized);
        ImGui::Text(phrases[XorString("profilechanger_warning")].c_str());
        if (ImGui::Button(phrases[XorString("profilechanger_hookbutton")].c_str()))
            hooks->hookGC();
        ImGui::PopFont();
    }
    else
    {

        const char *ranksGUIEN[] = {
            "Off",
            "Silver 1",
            "Silver 2",
            "Silver 3",
            "Silver 4",
            "Silver elite",
            "Silver elite master",
            "Gold nova 1",
            "Gold nova 2",
            "Gold nova 3",
            "Gold nova master",
            "Master guardian 1",
            "Master guardian 2",
            "Master guardian elite",
            "Distinguished master guardian",
            "Legendary eagle",
            "Legendary eagle master",
            "Supreme master first class",
            "The global elite"};

        const char *ranksDZGUIEN[] = {
            "Nenhum",
            "Lab Rat 1",
            "Lab Rat 2",
            "Sprinting Hare 1",
            "Sprinting Hare 2",
            "Wild Scout 1",
            "Wild Scout 2",
            "Wild Scout Elite",
            "Hunter Fox 1",
            "Hunter Fox 2",
            "Hunter Fox 3",
            "Hunter Fox Elite",
            "Timber Wolf",
            "Ember Wolf",
            "Wildfire Wolf",
            "The Howling Alpha"};

        const char *bansGUIEN[] = {
            "Off",
            "You were kicked from the last match (competitive cooldown)",
            "You killed too many teammates (competitive cooldown)",
            "You killed a teammate at round start (competitive cooldown)",
            "You failed to reconnect to the last match (competitive cooldown)",
            "You abandoned the last match (competitive cooldown)",
            "You did too much damage to your teammates (competitive cooldown)",
            "You did too much damage to your teammates at round start (competitive cooldown)",
            "This account is permanently untrusted (global cooldown)",
            "You were kicked from too many recent matches (competitive cooldown)",
            "Convicted by overwatch - majorly disruptive (global cooldown)",
            "Convicted by overwatch - minorly disruptive (global cooldown)",
            "Resolving matchmaking state for your account (temporary cooldown)",
            "Resolving matchmaking state after the last match (temporary cooldown)",
            "This account is permanently untrusted (global cooldown)",
            "(global cooldown)",
            "You failed to connect by match start. (competitive cooldown)",
            "You have kicked too many teammates in recent matches (competitive cooldown)",
            "Temporary ban from MD10 (temporary cooldown)",
            "A server using your game server login token has been banned. (global cooldown)"};

        static const char *ranksGUIPT[] = {
            "Nenhum",
            "Prata 1",
            "Prata 2",
            "Prata 3",
            "Prata 4",
            "Prata Elite",
            "Prata Elite Mestre",
            "Ouro 1",
            "Ouro 2",
            "Ouro 3",
            "Ouro 4",
            "AK 1",
            "AK 2",
            "AKX",
            "Xerife",
            "Aguia",
            "Aguia 2",
            "Supremo",
            "Global"};

        static const char *bansGUIPT[] = {
            "Nenhum",
            "Voce foi expulso da ultima partida",
            "Voce matou muitos aliados",
            "Voce matou um aliado no inicio da rodada",
            "Voce nao se reconectou a ultima partida",
            "Voce abandonou a ultima partida",
            "Voce causou muito dano aos seus aliados",
            "Voce causou muito dano aos seus aliados no inicio da rodada",
            "Conta permanentemente nao confiavel",
            "Voce foi expulso de muitas partidas recentes",
            "Condenado pela fiscalizacao : infracao grave",
            "Condenado pela fiscalizacao : infracao leve",
            "Resolvendo estado de criacao de partidas da sua conta",
            "Resolvendo estado de criacao de partidas apos a ultima partida",
            "Conta permanentemente nao confiavel",
            "(Suspensao global)",
            "Voce falhou em se conectar no inicio da partida",
            "Voce expulsou muitos aliados em partidas recentes",
            "Suspensao de melhor de dez",
            "Um servidor usando seu token foi banido"};

        const char *ranksDZGUIPT[] = {
            "Nenhum",
            "Rato de Laboratorio 1",
            "Rato de Laboratorio 2",
            "Lebre Veloz 1",
            "Lebre Veloz 2",
            "Patrulheiro 1",
            "Patrulheiro 2",
            "Patrulheiro de Elite",
            "Raposa de Caça 1",
            "Raposa de Caça 2",
            "Raposa de Caça 3",
            "Raposa de Caça de Elite",
            "Lobo-cinzento",
            "Lobo da Brasa",
            "Lobo do Fogo Selvagem",
            "Alfa Uivante"};

        ImGui::Separator();
        ImGui::Columns(3, nullptr);
        ImGui::TextWrapped("                Matchmaking");
        ImGui::SetNextItemWidth(-1.0f);
        if (config->misc.lang == 0)
            ImGui::Combo("##Rank0", &config->profilechanger.rankMatchmaking, ranksGUIEN, ARRAYSIZE(ranksGUIEN));
        else
            ImGui::Combo("##Rank0", &config->profilechanger.rankMatchmaking, ranksGUIPT, ARRAYSIZE(ranksGUIPT));
        ImGui::Text(phrases[XorString("profilechanger_wins")].c_str());
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##Wins0", &config->profilechanger.winsMatchmaking);
        ImGui::NextColumn();
        ImGui::TextWrapped("                   Wingman");
        ImGui::SetNextItemWidth(-1.0f);
        if (config->misc.lang == 0)
            ImGui::Combo("##Rank1", &config->profilechanger.rankWingman, ranksGUIEN, ARRAYSIZE(ranksGUIEN));
        else
            ImGui::Combo("##Rank1", &config->profilechanger.rankWingman, ranksGUIPT, ARRAYSIZE(ranksGUIPT));
        ImGui::Text(phrases[XorString("profilechanger_wins")].c_str());
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##Wins1", &config->profilechanger.winsWingman);
        ImGui::NextColumn();
        ImGui::TextWrapped("                Dangerzone");
        ImGui::SetNextItemWidth(-1.0f);
        if (config->misc.lang == 0)
            ImGui::Combo("##Rank2", &config->profilechanger.rankDangerzone, ranksDZGUIEN, ARRAYSIZE(ranksDZGUIEN));
        else
            ImGui::Combo("##Rank2", &config->profilechanger.rankDangerzone, ranksDZGUIPT, ARRAYSIZE(ranksDZGUIPT));
        ImGui::Text(phrases[XorString("profilechanger_wins")].c_str());
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##Wins2", &config->profilechanger.winsDangerzone);
        ImGui::Columns(1);
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Columns(3, nullptr);
        ImGui::Text(phrases[XorString("profilechanger_level")].c_str());
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##Level", &config->profilechanger.level);
        ImGui::Text(phrases[XorString("profilechanger_exp")].c_str());
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##Xp", &config->profilechanger.exp);
        ImGui::NextColumn();
        ImGui::Text(phrases[XorString("profilechanger_fakebantype")].c_str());
        ImGui::SetNextItemWidth(-1.0f);
        if (config->misc.lang == 0)
            ImGui::Combo("##fake-ban", &config->profilechanger.ban_type, bansGUIEN, IM_ARRAYSIZE(bansGUIEN));
        else
            ImGui::Combo("##fake-ban", &config->profilechanger.ban_type, bansGUIPT, IM_ARRAYSIZE(bansGUIPT));
        ImGui::Text(phrases[XorString("profilechanger_fakebantime")].c_str());
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::SliderInt("##fake-ban-time", &config->profilechanger.ban_time, 0, 1000, "%d h");
        ImGui::NextColumn();
        ImGui::Text(phrases[XorString("profilechanger_friendly")].c_str());
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##Friend", &config->profilechanger.friendly);
        ImGui::Text(phrases[XorString("profilechanger_teach")].c_str());
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##Teach", &config->profilechanger.teach);
        ImGui::Text(phrases[XorString("profilechanger_leader")].c_str());
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputInt("##Leader", &config->profilechanger.leader);
        ImGui::Columns(1);
        ImGui::Spacing();

        if (ImGui::Button("Apply", ImVec2(-1.0f, 0.0f)))
        {
            profile_changer::send_update_messages();
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(phrases[XorString("profilechanger_applyTooltip")].c_str());
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderAutoConfigWindow(bool contentOnly) noexcept
{

    if (!contentOnly)
    {
        if (!window.autoconfig)
            return;
        ImGui::SetNextWindowSize({0.0f, 0.0f});
        ImGui::Begin(phrases[XorString("window_ezconfig")].c_str(), &window.autoconfig, windowFlags);
    }
    static int currentCategory{0};
    static int currentWeapon{0};

    if (ImGui::Button("Legit Aimbot"))
    {
        ezConfigPage = "aimbot";
        /*
        //Desativando conflitos
        config->aimbot[currentWeapon].onKey = false;
        config->aimbot[currentWeapon].enabled = false;
        config->aimbot[currentWeapon].fov = 0.0f;
        config->aimbot[currentWeapon].smooth = 1.0f;
        config->aimbot[currentWeapon].aimlock = false;
        config->aimbot[currentWeapon].scopedOnly = false;
        config->aimbot[currentWeapon].autoShot = false;
        config->aimbot[currentWeapon].silent = false;
        config->aimbot[currentWeapon].visibleOnly = false;
        config->aimbot[currentWeapon].ignoreFlash = false;
        config->aimbot[currentWeapon].ignoreSmoke = false;
        config->aimbot[currentWeapon].minDamage = 0;
        config->aimbot[currentWeapon].killshot = false;
        config->aimbot[currentWeapon].bone = 0;
        config->aimbot[currentWeapon].killshot = false;
        config->aimbot[currentWeapon].scopedOnly = false;
        config->aimbot[currentWeapon].autoScope = false;
        config->aimbot[currentWeapon].maxShotInaccuracy = 1;
        config->aimbot[currentWeapon].maxAimInaccuracy = 1;

        //Ativando Legit
        config->aimbot[currentWeapon].onKey = true;
        config->aimbot[currentWeapon].key = 1;
        config->aimbot[currentWeapon].enabled = true;
        config->aimbot[currentWeapon].fov = 6.8f;
        config->aimbot[currentWeapon].smooth = 28.0f;
        config->aimbot[currentWeapon].aimlock = true;
        config->aimbot[currentWeapon].scopedOnly = false;
        config->aimbot[currentWeapon].visibleOnly = true;
        config->aimbot[currentWeapon].standaloneRCS = true;
        config->aimbot[currentWeapon].recoilControlX = 0.25f;
        config->aimbot[currentWeapon].recoilControlY = 0.25f;
        */
    };
    ImGui::SameLine();
    ImGui::Text(phrases[XorString("ezconfig_legitaimbot")].c_str());

    ImGui::Separator();

    if (ImGui::Button("Simple Wall"))
    {
        static int currentCategory = 1;
        static int currentType = 0;

        static int currentItem = currentCategory * 3 + currentType;
        config->glow[currentItem].enabled = true;
        config->glow[currentItem].healthBased = true;
    };
    ImGui::SameLine();
    ImGui::Text(phrases[XorString("ezconfig_simplewall")].c_str());

    ImGui::Separator();
    if (ImGui::Button("Advanced Visual"))
    {
        config->glow[3].enabled = true;
        config->glow[3].healthBased = true;
        config->glow[6].enabled = true;
        config->glow[6].healthBased = true;
        config->glow[13].enabled = true;
        config->glow[14].enabled = true;
        config->glow[15].enabled = true;
        config->glow[16].enabled = true;
        config->glow[17].enabled = true;
        config->glow[18].enabled = true;
        config->glow[19].enabled = true;
        config->misc.bombTimer.enabled = true;
        config->misc.disableModelOcclusion = true;
        config->visuals.disablePostProcessing = true;
        config->visuals.noFog = true;
        config->visuals.noBlur = true;
        config->visuals.noGrass = true;
        config->visuals.noShadows = true;
        config->visuals.flashReduction = 30;
    };
    ImGui::SameLine();
    ImGui::Text(phrases[XorString("ezconfig_fullwall")].c_str());

    ImGui::Separator();
    if (ImGui::Button("Utilities"))
    {

        config->misc.revealRanks = true;
        config->misc.antiAfkKick = true;
        config->misc.fastDuck = true;
        config->misc.noscopeCrosshair = true;
        config->misc.autoReload = true;
        config->misc.autoAccept = true;
        config->misc.revealRanks = true;
        config->misc.revealMoney = true;
        config->misc.revealSuspect = true;
        config->misc.quickReload = true;
        config->misc.fastPlant = true;
        config->misc.quickHealthshotKey = true;
        config->misc.nadePredict = true;
        config->misc.fixTabletSignal = true;
    };
    ImGui::SameLine();
    ImGui::Text(phrases[XorString("ezconfig_utilities")].c_str());

    ImGui::Separator();
    if (ImGui::Button("BunnyHopping"))
    {

        config->misc.bunnyHop = true;
        config->misc.autoStrafe = true;
    };
    ImGui::SameLine();
    ImGui::Text(phrases[XorString("ezconfig_bunnyhop")].c_str());

    ImGui::Separator();
    if (ImGui::Button("FPS Boost"))
    {

        config->misc.disableModelOcclusion = true;
        config->visuals.disablePostProcessing = true;
        config->visuals.noFog = true;
        config->visuals.noBlur = true;
        config->visuals.noGrass = true;
        config->visuals.noShadows = true;
        config->visuals.noBloom = true;
        config->visuals.no3dSky = true;
    };
    ImGui::SameLine();
    ImGui::Text(phrases[XorString("ezconfig_fpsboost")].c_str());

    ImGui::Separator();
    if (ImGui::Button("Reset"))
    {

        config->reset();
        Misc::updateClanTag(true);
        SkinChanger::scheduleHudUpdate();
    };
    ImGui::SameLine();
    ImGui::Text(phrases[XorString("ezconfig_reset")].c_str());

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderAimHacksWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.aimhacks)
            return;
        ImGui::SetNextWindowSize({0.0f, 0.0f});
        ImGui::Begin("AimHacks", &window.aimhacks, windowFlags);
    }

    if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_NoTooltip))
    {
        if (ImGui::BeginTabItem("Aimbot " ICON_FA_LEAF))
        {
            renderAimbotWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("TriggerBot " ICON_FA_ROBOT))
        {
            renderTriggerbotWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Backtrack " ICON_FA_BACKWARD))
        {
            renderBacktrackWindow(true);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderWallhacksWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.wallhacks)
            return;
        ImGui::SetNextWindowSize({0.0f, 0.0f});
        ImGui::Begin("Wallhacks", &window.wallhacks, windowFlags);
    }

    if (ImGui::BeginTabBar("Wallhacks", ImGuiTabBarFlags_NoTooltip))
    {
        if (ImGui::BeginTabItem("ESP " ICON_FA_CODE_BRANCH))
        {
            renderStreamProofESPWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Glow " ICON_FA_BLIND))
        {
            renderGlowWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Chams " ICON_FA_CHILD))
        {
            renderChamsWindow(true);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderChangersWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.changers)
            return;
        ImGui::SetNextWindowSize({0.0f, 0.0f});
        ImGui::Begin("Changers", &window.changers, windowFlags | ImGuiWindowFlags_AlwaysAutoResize);
    }

    if (ImGui::BeginTabBar("Changers", ImGuiTabBarFlags_NoTooltip))
    {
        if (ImGui::BeginTabItem("Skins " ICON_FA_BRIEFCASE))
        {
            renderSkinChangerWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Profile " ICON_FA_USER))
        {
            renderProfileChangerWindow(true);
            ImGui::EndTabItem();
        }
        /*
        if (ImGui::BeginTabItem("Inventory")) {
            renderInventoryChangerWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Medal")) {
            renderMedalChangerWindow(true);
            ImGui::EndTabItem();
        }
        */
        ImGui::EndTabBar();
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderConfigsWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.configs)
            return;
        ImGui::SetNextWindowSize({0.0f, 0.0f});
        ImGui::Begin("Configs", &window.configs, windowFlags);
    }

    if (ImGui::BeginTabBar("Configs", ImGuiTabBarFlags_NoTooltip))
    {
        if (ImGui::BeginTabItem("Config " ICON_FA_COG))
        {
            renderConfigWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("EzConfig " ICON_FA_USERS_COG))
        {
            renderAutoConfigWindow(true);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderVisualhacksWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.visualhacks)
            return;
        ImGui::SetNextWindowSize({0.0f, 0.0f});
        ImGui::Begin("Visualhacks", &window.visualhacks, windowFlags);
    }

    if (ImGui::BeginTabBar("Visualhacks", ImGuiTabBarFlags_NoTooltip))
    {
        if (ImGui::BeginTabItem("Visuals " ICON_FA_MAGIC))
        {
            renderVisualsWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("World " ICON_FA_GLOBE_AMERICAS))
        {
            renderWorldWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Performance " ICON_FA_BURN))
        {
            renderPerformanceWindow(true);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void GUI::renderNickWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.nick)
            return;
        ImGui::SetNextWindowSize({0.0f, 0.0f});
        ImGui::Begin("nickexploit", &window.nick, windowFlags);
    }
    ImGui::Combo(phrases[XorString("misc_nameexploit")].c_str(), &config->misc.nameChangeSelection, XorString("Off\0Fake Ban\0Fake Item\0Custom Name\0"));
    if (config->misc.nameChangeSelection == 1)
    {
        ImGui::PushID(3);
        ImGui::SetNextItemWidth(100.0f);
        ImGui::Combo("", &config->misc.banColor, XorString("White\0Red\0Purple\0Green\0Light green\0Turquoise\0Light red\0Gray\0Yellow\0Gray 2\0Light blue\0Gray/Purple\0Blue\0Pink\0Dark orange\0Orange\0"));
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::PushID(4);
        ImGui::InputText("", &config->misc.banText);
        ImGui::PopID();
        ImGui::SameLine();
        if (ImGui::Button(phrases[XorString("misc_setupfakeban")].c_str()))
            Misc::fakeBan(true);
    }
    else if (config->misc.nameChangeSelection == 2)
    {
        ImGui::SetNextItemWidth(200.0f);
        ImGuiCustom::MultiCombo(phrases[XorString("misc_fakeitemflags")].c_str(), config->misc.fakeItemFlags, config->misc.selectedFakeItemFlags, 4);
        ImGui::SetNextItemWidth(200.0f);
        ImGui::Combo(phrases[XorString("global_team")].c_str(), &config->misc.fakeItemTeam, XorString("Counter-Terrorist\0Terrorist"));
        ImGui::SetNextItemWidth(200.0f);
        ImGui::Combo(phrases[XorString("misc_messagetype")].c_str(), &config->misc.fakeItemMessageType, XorString("Unbox\0Trade\0"));
        ImGui::SetNextItemWidth(200.0f);
        ImGui::Combo(phrases[XorString("misc_itemtype")].c_str(), &config->misc.fakeItemType, XorString("AK-47\0AUG\0AWP\0Bayonet\0Bowie Knife\0Butterfly Knife\0CZ75-Auto\0Classic Knife\0Desert Eagle\0Dual Berettas\0FAMAS\0Falchion Knife\0Five-SeveN\0Flip Knife\0G3SG1\0Galil AR\0Glock-18\0Gut Knife\0Huntsman Knife\0Karambit\0M249\0M4A1-S\0M4A4\0M9 Bayonet\0MAC-10\0MAG-7\0MP5-SD\0MP7\0MP9\0Navaja Knife\0Negev\0Nomad Knife\0Nova\0P2000\0P250\0P90\0PP-Bizon\0Paracord Knife\0R8 Revolver\0SCAR-20\0SG 553\0SSG 08\0Sawed-Off\0Shadow Daggers\0Skeleton Knife\0Spectral Shiv\0Stiletto Knife\0Survival Knife\0Talon Knife\0Tec-9\0UMP-45\0USP-S\0Ursus Knife\0XM1014\0Hand Wraps\0Moto Gloves\0Specialist Gloves\0Sport Gloves\0Bloodhound Gloves\0Hydra Gloves\0Driver Gloves\0"));
        ImGui::SetNextItemWidth(200.0f);
        ImGui::Combo(phrases[XorString("misc_itemrarity")].c_str(), &config->misc.fakeItemRarity, XorString("Consumer Grade (White)\0Industrial Grade (Light blue)\0Mil-Spec (Blue)\0Restricted (Purple)\0Classified (Pink)\0Covert (Red)\0Contrabanned(Orange/Gold)\0"));
        ImGui::Combo(phrases[XorString("misc_playercolor")].c_str(), &config->misc.fakeItemPlayerColor, XorString("Yellow\0Green\0Blue\0Purple\0Orange"));
        ImGui::InputText(phrases[XorString("misc_playername")].c_str(), &config->misc.fakeItemPlayerName);
        ImGui::InputText(phrases[XorString("misc_skinname")].c_str(), &config->misc.fakeItemName);
        if (ImGui::Button(phrases[XorString("misc_changename")].c_str()))
            Misc::fakeItem(true);
    }
    else if (config->misc.nameChangeSelection == 3)
    {
        ImGui::Checkbox(phrases[XorString("misc_namestealer")].c_str(), &config->misc.nameStealer);
        ImGui::InputText(phrases[XorString("misc_customname")].c_str(), &config->misc.customName);
        if (ImGui::Button(phrases[XorString("misc_changename")].c_str()))
            Misc::setName(true);
    }
}

void GUI::renderPerformanceWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.performance)
            return;
        ImGui::SetNextWindowSize({0.0f, 0.0f});
        ImGui::Begin("Performance", &window.performance, windowFlags);
    }
    ImGui::Columns(2, nullptr, true);
    ImGui::Checkbox(phrases[XorString("visuals_noblur")].c_str(), &config->visuals.noBlur);
    ImGui::Checkbox(phrases[XorString("visuals_disablepostprocessing")].c_str(), &config->visuals.disablePostProcessing);
    ImGui::Checkbox(phrases[XorString("visuals_nograss")].c_str(), &config->visuals.noGrass);
    ImGui::Checkbox(phrases[XorString("visuals_noshadows")].c_str(), &config->visuals.noShadows);
    ImGui::Checkbox(phrases[XorString("visuals_drawFps")].c_str(), &config->visuals.drawFps);
    ImGui::NextColumn();
    ImGui::Checkbox(phrases[XorString("visuals_nobloom")].c_str(), &config->visuals.noBloom);
    ImGui::Checkbox(phrases[XorString("visuals_nofog")].c_str(), &config->visuals.noFog);
    ImGui::Checkbox(phrases[XorString("visuals_no3dsky")].c_str(), &config->visuals.no3dSky);
    ImGui::Checkbox("Gray Textures", &config->visuals.drawgray);
    if (config->visuals.drawFps) {
        ImGui::Checkbox(phrases[XorString("visuals_drawPing")].c_str(), &config->visuals.drawPing);
    }
    ImGui::Columns(1);
}

void GUI::renderMiscsWindow(bool contentOnly) noexcept
{
    if (!contentOnly)
    {
        if (!window.miscs)
            return;
        ImGui::SetNextWindowSize({0.0f, 0.0f});
        ImGui::Begin("Miscs", &window.miscs, windowFlags);
    }
    if (ImGui::BeginTabBar("Miscs", ImGuiTabBarFlags_NoTooltip))
    {
        if (ImGui::BeginTabItem("Misc " ICON_FA_CHESS_PAWN))
        {
            renderMiscWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Nick " ICON_FA_SIGNATURE))
        {
            renderNickWindow(true);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void GUI::renderWarningWindow() noexcept
{

    if (!window.warning)
        return;

    ImGui::Begin("Warning Modal", NULL, windowFlags);
    ImGui::Text("Pra ser usado se precisar de novo");

    ImGui::End();
}

void GUI::renderEzConfigModal(bool contentOnly) noexcept
{
    if (ezConfigPage == "")
        return;

    ImGui::SetNextWindowSize({650.0f, 0.0f});
    ImGui::SetNextWindowPos({300.0f, 150.0f}, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin("##EzConfigModal", &window.ezConfigModal, windowFlags | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    ImGui::Columns(2, nullptr, true);
    ImGui::Text(ezConfigPage);
    ImGui::NextColumn();
    ImGui::Columns(1);
    if (ImGui::Button(XorString("Apply")))
        ezConfigPage = "";

    ImGui::End();
}

void GUI::drawDemo() noexcept
{

    ImGui::ShowDemoWindow();
}

void GUI::renderGuiStyle2() noexcept
{

    ImGui::SetNextWindowSize({600.0f, 0.0f});
    ImGui::Begin("Counter Strike: Global Offensive", nullptr, windowFlags);
    if (ImGui::Button("      " ICON_FA_CROSSHAIRS "\nAimhacks", ImVec2(97.0f, 0.0f)))
    {
        window.tab = 0;
    }
    ImGui::SameLine(0, 0);
    if (ImGui::Button("       " ICON_FA_EYE "\nWallhacks", ImVec2(97.0f, 0.0f)))
    {
        window.tab = 1;
    }
    /* ImGui::SameLine(0, 0);
    if (ImGui::Button("      " ICON_FA_ATOM "\nEzConfig", ImVec2(97.0f, 0.0f)))
    {
        window.tab = 2;
    } */
    ImGui::SameLine(0, 0);
    if (ImGui::Button("    " ICON_FA_PALETTE "\nVisuals", ImVec2(97.0f, 0.0f)))
    {
        window.tab = 3;
    }
    ImGui::SameLine(0, 0);
    if (ImGui::Button("      " ICON_FA_PENCIL_ALT "\nChangers", ImVec2(97.0f, 0.0f)))
    {
        window.tab = 4;
    }
    ImGui::SameLine(0, 0);
    if (ImGui::Button("     " ICON_FA_ASTERISK "\n  Miscs  ", ImVec2(97.0f, 0.0f)))
    {
        window.tab = 5;
    }
    ImGui::SameLine(0, 0);
    if (ImGui::Button("      " ICON_FA_ATOM "\nEzConfig", ImVec2(97.0f, 0.0f)))
    {
        window.tab = 7;
    }

    switch (window.tab)
    {

    case 0:
        renderAimHacksWindow(true);
        break;

    case 1:
        renderWallhacksWindow(true);
        break;

    case 3:
        renderVisualhacksWindow(true);
        break;

    case 4:
        renderChangersWindow(true);
        break;

    case 5:
        renderMiscsWindow(true);
        break;

    case 6:
        renderConfigsWindow(true);
        break;

    case 7:
        renderAutoConfigWindow(true);
        break;

    }

    ImGui::Separator();
    if (ImGui::Button("EN-US " ICON_FA_FLAG_USA))
        config->misc.lang = 0;
    ImGui::SameLine();
    if (ImGui::Button("PT-BR " ICON_FA_GEM))
        config->misc.lang = 1;
    ImGui::SameLine();
    if (ImGui::Button("Unload " ICON_FA_WINDOW_CLOSE))
        hooks->uninstall();
    ImGui::SameLine();

    RightText();
    if (ImGui::Button("Config " ICON_FA_COGS))
        window.tab = 6;
    ImGui::End();
}
