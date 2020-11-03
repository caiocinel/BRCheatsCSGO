#include <fstream>
#include <functional>
#include <string>
#include <ShlObj.h>
#include <Windows.h>

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

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
extern std::map<std::string, std::string> phrases;

namespace ImGui {

    void ImGuiStructItem(ImGuiStruct i)noexcept
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
    ImGuiStyle& style = ImGui::GetStyle();

    style.ScrollbarSize = 9.0f;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    if (PWSTR pathToFonts; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &pathToFonts))) {
        const std::filesystem::path path{ pathToFonts };
        CoTaskMemFree(pathToFonts);

        ImFontConfig cfg;
        cfg.OversampleV = 3;

        fonts.tahoma = io.Fonts->AddFontFromFileTTF((path / XorString("tahoma.ttf")).string().c_str(), 15.0f, &cfg, Helpers::getFontGlyphRanges());
        fonts.segoeui = io.Fonts->AddFontFromFileTTF((path / XorString("segoeui.ttf")).string().c_str(), 15.0f, &cfg, Helpers::getFontGlyphRanges());
    }
}

void GUI::render() noexcept
{
    UpdateLanguage();

    renderGuiStyle3();
    renderAimbotWindow();
    renderRagebotWindow();
    renderAntiAimWindow();
    renderTriggerbotWindow();
    renderBacktrackWindow();
    renderGlowWindow();
    renderChamsWindow();
    renderStreamProofESPWindow();
    renderVisualsWindow();
    renderSkinChangerWindow();
    renderMiscWindow();
    renderConfigWindow();

}

void GUI::updateColors() const noexcept
{
    switch (config->style.menuColors) {
    case 0: ImGui::StyleColorsDark(); break;
    case 1: ImGui::StyleColorsLight(); break;
    case 2: ImGui::StyleColorsClassic(); break;
    }
}

void DisableItems() {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
};

void PopDisableItems() {
    ImGui::PopItemFlag();
    ImGui::PopStyleVar();
};

void GUI::hotkey(int& key) noexcept
{
    key ? ImGui::Text(XorString("[ %s ]"), interfaces->inputSystem->virtualKeyToString(key)) : ImGui::TextUnformatted(phrases[XorString("global_key")].c_str());

    if (!ImGui::IsItemHovered())
        return;

    ImGui::SetTooltip(phrases[XorString("global_keypopup")].c_str());
    ImGuiIO& io = ImGui::GetIO();
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
        if (ImGui::IsKeyPressed(i) && i != config->misc.menuKey)
            key = i != VK_ESCAPE ? i : 0;

    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
        if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != config->misc.menuKey)
            key = i + (i > 1 ? 2 : 1);
}

void GUI::renderAimbotWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.aimbot)
            return;
        ImGui::SetNextWindowSize({ 600.0f, 0.0f });
        ImGui::Begin(phrases[XorString("window_aimbot")].c_str(), &window.aimbot, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, phrases[XorString("global_weapontype")].c_str());
    ImGui::PopID();
    ImGui::SameLine();
    static int currentWeapon{ 0 };
    ImGui::PushID(1);

    switch (currentCategory) {
    case 0:
        currentWeapon = 0;
        ImGui::NewLine();
        break;
    case 1: {
        static int currentPistol{ 0 };
        static constexpr const char* pistols[]{ "All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

        ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx : 35].enabled) {
                static std::string name;
                name = pistols[idx];
                *out_text = name.append(" *").c_str();
            }
            else {
                *out_text = pistols[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(pistols));

        currentWeapon = currentPistol ? currentPistol : 35;
        break;
    }
    case 2: {
        static int currentHeavy{ 0 };
        static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

        ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx + 10 : 36].enabled) {
                static std::string name;
                name = heavies[idx];
                *out_text = name.append(" *").c_str();
            }
            else {
                *out_text = heavies[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(heavies));

        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
        break;
    }
    case 3: {
        static int currentSmg{ 0 };
        static constexpr const char* smgs[]{ "All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

        ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx + 16 : 37].enabled) {
                static std::string name;
                name = smgs[idx];
                *out_text = name.append(" *").c_str();
            }
            else {
                *out_text = smgs[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(smgs));

        currentWeapon = currentSmg ? currentSmg + 16 : 37;
        break;
    }
    case 4: {
        static int currentRifle{ 0 };
        static constexpr const char* rifles[]{ "All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

        ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
            if (config->aimbot[idx ? idx + 23 : 38].enabled) {
                static std::string name;
                name = rifles[idx];
                *out_text = name.append(XorString(" *")).c_str();
            }
            else {
                *out_text = rifles[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(rifles));

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
    ImGui::Combo("", &config->aimbot[currentWeapon].keyMode, phrases[XorString("global_keymode")].c_str());
    ImGui::PopItemWidth();
    ImGui::PopID();

    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 220.0f);
    ImGui::Checkbox(phrases[XorString("aimhacks_silent")].c_str(), &config->aimbot[currentWeapon].silent);
    ImGui::Checkbox(phrases[XorString("global_friendlyfire")].c_str(), &config->aimbot[currentWeapon].friendlyFire);
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
    ImGui::Combo(phrases[XorString("global_bone")].c_str() , & config->aimbot[currentWeapon].bone, phrases[XorString("global_bonelist")].c_str());
    ImGui::PushItemWidth(200.0f);
    ImGui::SliderFloat(phrases[XorString("aimbot_fov")].c_str(), &config->aimbot[currentWeapon].fov, 0.0f, 255.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
    ImGui::PushItemWidth(240.0f);
    ImGui::SameLine();
    ImGui::Checkbox(phrases[XorString("aimbot_drawfov")].c_str(), &config->aimbot[currentWeapon].drawFov);
    ImGui::SliderFloat(phrases[XorString("aimbot_smooth")].c_str() , &config->aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
  //ImGui::SliderInt(phrases[XorString("aimhacks_hitchance")].c_str(), &config->aimbot[currentWeapon].hitchance, 0, 100, "%d");
    ImGui::InputInt(phrases[XorString("aimhacks_mindamage")].c_str(), &config->aimbot[currentWeapon].minDamage);
    config->aimbot[currentWeapon].minDamage = std::clamp(config->aimbot[currentWeapon].minDamage, 0, 250);
    if (config->aimbot[currentWeapon].silent)
        DisableItems();

    ImGui::Checkbox(phrases[XorString("aimbot_rcs")].c_str(), &config->aimbot[currentWeapon].standaloneRCS);
    if (config->aimbot[currentWeapon].silent)
        PopDisableItems();

    if (config->aimbot[currentWeapon].standaloneRCS && !config->aimbot[currentWeapon].silent)
    {

        ImGui::InputInt(phrases[XorString("aimbot_ignoreshots")].c_str(), &config->aimbot[currentWeapon].shotsFired);
        ImGui::SliderFloat(phrases[XorString("aimbot_rcs_x")].c_str(), &config->aimbot[currentWeapon].recoilControlX, 0.0f, 1.0f, "%.5f");
        ImGui::SliderFloat(phrases[XorString("aimbot_rcs_x")].c_str(), &config->aimbot[currentWeapon].recoilControlY, 0.0f, 1.0f, "%.5f");

    }
    

    
    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderRagebotWindow(bool contentOnly) noexcept
{
	if (!contentOnly) {
		if (!window.ragebot)
			return;
		ImGui::SetNextWindowSize({ 600.0f, 0.0f });
		ImGui::Begin(phrases[XorString("window_ragebot")].c_str(), &window.ragebot, windowFlags);
	}
	static int currentCategory{ 0 };
	ImGui::PushItemWidth(110.0f);
	ImGui::PushID(0);
	ImGui::Combo("", &currentCategory, phrases[XorString("global_weapontype")].c_str());
	ImGui::PopID();
	ImGui::SameLine();
	static int currentWeapon{ 0 };
	ImGui::PushID(1);

	switch (currentCategory) {
	case 0:
		currentWeapon = 0;
		ImGui::NewLine();
		break;
	case 1: {
		static int currentPistol{ 0 };
		static constexpr const char* pistols[]{ "All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

		ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
			if (config->aimbot[idx ? idx : 35].enabled) {
				static std::string name;
				name = pistols[idx];
				*out_text = name.append(" *").c_str();
			}
			else {
				*out_text = pistols[idx];
			}
			return true;
			}, nullptr, IM_ARRAYSIZE(pistols));

		currentWeapon = currentPistol ? currentPistol : 35;
		break;
	}
	case 2: {
		static int currentHeavy{ 0 };
		static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

		ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
			if (config->aimbot[idx ? idx + 10 : 36].enabled) {
				static std::string name;
				name = heavies[idx];
				*out_text = name.append(" *").c_str();
			}
			else {
				*out_text = heavies[idx];
			}
			return true;
			}, nullptr, IM_ARRAYSIZE(heavies));

		currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
		break;
	}
	case 3: {
		static int currentSmg{ 0 };
		static constexpr const char* smgs[]{ "All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

		ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
			if (config->aimbot[idx ? idx + 16 : 37].enabled) {
				static std::string name;
				name = smgs[idx];
				*out_text = name.append(" *").c_str();
			}
			else {
				*out_text = smgs[idx];
			}
			return true;
			}, nullptr, IM_ARRAYSIZE(smgs));

		currentWeapon = currentSmg ? currentSmg + 16 : 37;
		break;
	}
	case 4: {
		static int currentRifle{ 0 };
		static constexpr const char* rifles[]{ "All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

		ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
			if (config->aimbot[idx ? idx + 23 : 38].enabled) {
				static std::string name;
				name = rifles[idx];
				*out_text = name.append(" *").c_str();
			}
			else {
				*out_text = rifles[idx];
			}
			return true;
			}, nullptr, IM_ARRAYSIZE(rifles));

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
    ImGui::Combo("", &config->ragebot[currentWeapon].keyMode, phrases[XorString("global_keymode")].c_str());
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
    ImGui::Checkbox(phrases[XorString("ragebot_quickpeek")].c_str(), &config->ragebot[currentWeapon].QuickPeekEnabled);
    ImGui::SameLine();
    hotkey(config->ragebot[currentWeapon].QuickPeekKey);
    ImGui::NextColumn();
    ImGuiCustom::MultiCombo(phrases[XorString("ragebot_hitboxes")].c_str(), config->BonesTexts, config->ragebot[currentWeapon].BonesBools, 8);
    ImGui::SliderFloat(phrases[XorString("aimhacks_mindamage")].c_str(), &config->ragebot[currentWeapon].WallDamage, 0, 250);
    ImGui::SliderFloat(phrases[XorString("aimhacks_hitchance")].c_str(), &config->ragebot[currentWeapon].hitChance, 0, 100);
    ImGui::SliderFloat(phrases[XorString("ragebot_headvalue")].c_str(), &config->ragebot[currentWeapon].pointChance, 0, 100);
    ImGui::SliderFloat(phrases[XorString("ragebot_bodyvalue")].c_str(), &config->ragebot[currentWeapon].bodyChance, 0, 100);
    ImGui::Checkbox(phrases[XorString("ragebot_doubletap")].c_str(), &config->ragebotExtra.doubletap);
    if (config->ragebotExtra.doubletap)
    {
        ImGui::SetNextItemWidth(110.0f);
        ImGui::Combo(phrases[XorString("ragebot_doubletap_speed")].c_str(), &config->ragebotExtra.doubletapSpeed, "Instant\0Fast\0Accurate\0");
        ImGui::SameLine();
        hotkey(config->ragebotExtra.doubleTapKey);
        ImGui::SetNextItemWidth(110.0f);
        ImGui::Combo(phrases[XorString("ragebot_doubletap_keymode")].c_str(), &config->ragebotExtra.doubleTapKeyMode, phrases[XorString("global_keymode")].c_str());
    }
    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderAntiAimWindow(bool contentOnly) noexcept
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
    ImGui::Combo("", &config->antiAim.general.yawInverseKeyMode, phrases[XorString("global_keymode")].c_str());
    ImGui::PopID();
    ImGui::Checkbox(phrases[XorString("antiaim_fakewalk")].c_str(), &config->antiAim.general.fakeWalk.enabled);
    if (!config->antiAim.general.fakeWalk.enabled)DisableItems();
    ImGui::SameLine();
    hotkey(config->antiAim.general.fakeWalk.key);
    ImGui::PushID(2);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(75.0f);
    ImGui::Combo("", &config->antiAim.general.fakeWalk.keyMode, phrases[XorString("global_keymode")].c_str());
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

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderTriggerbotWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.triggerbot)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin(phrases[XorString("window_triggerbot")].c_str(), &window.triggerbot, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, phrases[XorString("global_weapontype_zeus")].c_str());
    ImGui::PopID();
    ImGui::SameLine();
    static int currentWeapon{ 0 };
    ImGui::PushID(1);
    switch (currentCategory) {
    case 0:
        currentWeapon = 0;
        ImGui::NewLine();
        break;
    case 5:
        currentWeapon = 39;
        ImGui::NewLine();
        break;

    case 1: {
        static int currentPistol{ 0 };
        static constexpr const char* pistols[]{ "All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

        ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx : 35].enabled) {
                static std::string name;
                name = pistols[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = pistols[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(pistols));

        currentWeapon = currentPistol ? currentPistol : 35;
        break;
    }
    case 2: {
        static int currentHeavy{ 0 };
        static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

        ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx + 10 : 36].enabled) {
                static std::string name;
                name = heavies[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = heavies[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(heavies));

        currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
        break;
    }
    case 3: {
        static int currentSmg{ 0 };
        static constexpr const char* smgs[]{ "All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

        ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx + 16 : 37].enabled) {
                static std::string name;
                name = smgs[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = smgs[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(smgs));

        currentWeapon = currentSmg ? currentSmg + 16 : 37;
        break;
    }
    case 4: {
        static int currentRifle{ 0 };
        static constexpr const char* rifles[]{ "All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

        ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
            if (config->triggerbot[idx ? idx + 23 : 38].enabled) {
                static std::string name;
                name = rifles[idx];
                *out_text = name.append(" *").c_str();
            } else {
                *out_text = rifles[idx];
            }
            return true;
            }, nullptr, IM_ARRAYSIZE(rifles));

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
    ImGui::Combo(phrases[XorString("global_hitgroup")].c_str(), &config->triggerbot[currentWeapon].hitgroup, phrases[XorString("triggerbot_hitgroup")].c_str());
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
    if (!contentOnly) {
        if (!window.backtrack)
            return;
        ImGui::SetNextWindowSize({ 350.0f, 0.0f });
        ImGui::Begin(phrases[XorString("window_backtrack")].c_str(), &window.backtrack, windowFlags);
    }
    ImGui::Columns(2, nullptr, true);
    ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &config->backtrack.enabled);
    ImGui::Checkbox(phrases[XorString("aimhacks_ignoresmoke")].c_str(), &config->backtrack.ignoreSmoke);
    ImGui::NextColumn();
    ImGui::Checkbox(phrases[XorString("backtrack_extend")].c_str(), &config->backtrack.fakeLatency);
    ImGui::Columns(1);
    if (!config->backtrack.fakeLatency) { if (config->backtrack.timeLimit >= 201) { config->backtrack.timeLimit = 200; } }
    ImGui::PushItemWidth(220.0f); ImGui::PushID(0);
    ImGui::SliderInt("", &config->backtrack.timeLimit, 1, config->backtrack.fakeLatency ? 400 : 200, phrases[XorString("backtrack_timelimit")].c_str());
    ImGui::PopID(); ImGui::PopItemWidth();
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderGlowWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.glow)
            return;
        ImGui::SetNextWindowSize({ 350.0f, 0.0f });
        ImGui::Begin(phrases[XorString("window_glow")].c_str(), &window.glow, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);
    ImGui::Combo("", &currentCategory, phrases[XorString("glow_category")].c_str());
    ImGui::PopID();
    static int currentItem{ 0 };
    if (currentCategory <= 3) {
        ImGui::SameLine();
        static int currentType{ 0 };
        ImGui::PushID(1);
        ImGui::Combo("", &currentType, phrases[XorString("wallhacks_visibletype")].c_str());
        ImGui::PopID();
        currentItem = currentCategory * 3 + currentType;
    } else {
        currentItem = currentCategory + 8;
    }

    ImGui::Separator();
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 150.0f);
    ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &config->glow[currentItem].enabled);
    ImGui::Checkbox(phrases[XorString("wallhacks_healthbased")].c_str(), &config->glow[currentItem].healthBased);
    ImGui::NextColumn();
    ImGui::SetNextItemWidth(100.0f);
    ImGui::Combo(phrases[XorString("global_style")].c_str(), &config->glow[currentItem].style, phrases[XorString("glow_styles")].c_str());
    ImGuiCustom::colorPopup(phrases[XorString("global_color")].c_str(), config->glow[currentItem].color, &config->glow[currentItem].rainbow, &config->glow[currentItem].rainbowSpeed);
    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderChamsWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.chams)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin(phrases[XorString("window_chams")].c_str(), &window.chams, windowFlags);
    }
    static int currentCategory{ 0 };
    ImGui::PushItemWidth(110.0f);
    ImGui::PushID(0);

    static int material = 1;

    if (ImGui::Combo("", &currentCategory, phrases[XorString("chams_category")].c_str()))
        material = 1;

    ImGui::PopID();

    ImGui::SameLine();

    if (material <= 1)
        ImGuiCustom::arrowButtonDisabled("##left", ImGuiDir_Left);
    else if (ImGui::ArrowButton("##left", ImGuiDir_Left))
        --material;

    ImGui::SameLine();
    ImGui::Text("%d", material);

    constexpr std::array categories{ "Allies", "Enemies", "Planting", "Defusing", "Local player", "Weapons", "Hands", "Backtrack", "Sleeves", "Desync" };

    ImGui::SameLine();

    if (material >= int(config->chams[categories[currentCategory]].materials.size()))
        ImGuiCustom::arrowButtonDisabled("##right", ImGuiDir_Right);
    else if (ImGui::ArrowButton("##right", ImGuiDir_Right))
        ++material;

    ImGui::SameLine();

    auto& chams{ config->chams[categories[currentCategory]].materials[material - 1] };

    ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &chams.enabled);
    ImGui::Separator();
    ImGui::Columns(2, nullptr, true);
    ImGui::Checkbox(phrases[XorString("wallhacks_healthbased")].c_str(), &chams.healthBased);
    ImGui::Checkbox("Blinking", &chams.blinking);
    ImGui::Checkbox("Wireframe", &chams.wireframe);
    ImGui::SetNextItemWidth(85.0f);
    ImGui::Combo("Material", &chams.material, "Normal\0Flat\0Animated\0Platinum\0Glass\0Chrome\0Crystal\0Silver\0Gold\0Plastic\0Glow\0Pearlescent\0Metallic\0");
    ImGui::NextColumn();
    ImGui::Checkbox("Cover", &chams.cover);
    ImGui::Checkbox("Ignore-Z", &chams.ignorez);
    ImGuiCustom::colorPopup("Color", chams.color, &chams.rainbow, &chams.rainbowSpeed);

    if (!contentOnly) {
        ImGui::End();
    }
}

void GUI::renderStreamProofESPWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.streamProofESP)
            return;
        ImGui::SetNextWindowSize({ 0.0f, 0.0f });
        ImGui::Begin("ESP BRCheats | Use Game Capture from OBS, this ESP and the Menu will not be visible in your stream", &window.streamProofESP, windowFlags);
    }

    static std::size_t currentCategory;
    static auto currentItem = "All";

    constexpr auto getConfigShared = [](std::size_t category, const char* item) noexcept -> Shared& {
        switch (category) {
        case 0: default: return config->streamProofESP.enemies[item];
        case 1: return config->streamProofESP.allies[item];
        case 2: return config->streamProofESP.weapons[item];
        case 3: return config->streamProofESP.projectiles[item];
        case 4: return config->streamProofESP.lootCrates[item];
        case 5: return config->streamProofESP.otherEntities[item];
        }
    };

    constexpr auto getConfigPlayer = [](std::size_t category, const char* item) noexcept -> Player& {
        switch (category) {
        case 0: default: return config->streamProofESP.enemies[item];
        case 1: return config->streamProofESP.allies[item];
        }
    };

    if (ImGui::ListBoxHeader("##list", { 170.0f, 300.0f })) {
        constexpr std::array categories{ "Enemies", "Allies", "Weapons", "Projectiles", "Loot Crates", "Other Entities" };

        for (std::size_t i = 0; i < categories.size(); ++i) {
            if (ImGui::Selectable(categories[i], currentCategory == i && std::string_view{ currentItem } == "All")) {
                currentCategory = i;
                currentItem = "All";
            }

            if (ImGui::BeginDragDropSource()) {
                switch (i) {
                case 0: case 1: ImGui::SetDragDropPayload("Player", &getConfigPlayer(i, "All"), sizeof(Player), ImGuiCond_Once); break;
                case 2: ImGui::SetDragDropPayload("Weapon", &config->streamProofESP.weapons["All"], sizeof(Weapon), ImGuiCond_Once); break;
                case 3: ImGui::SetDragDropPayload("Projectile", &config->streamProofESP.projectiles["All"], sizeof(Projectile), ImGuiCond_Once); break;
                default: ImGui::SetDragDropPayload("Entity", &getConfigShared(i, "All"), sizeof(Shared), ImGuiCond_Once); break;
                }
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Player")) {
                    const auto& data = *(Player*)payload->Data;

                    switch (i) {
                    case 0: case 1: getConfigPlayer(i, "All") = data; break;
                    case 2: config->streamProofESP.weapons["All"] = data; break;
                    case 3: config->streamProofESP.projectiles["All"] = data; break;
                    default: getConfigShared(i, "All") = data; break;
                    }
                }

                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Weapon")) {
                    const auto& data = *(Weapon*)payload->Data;

                    switch (i) {
                    case 0: case 1: getConfigPlayer(i, "All") = data; break;
                    case 2: config->streamProofESP.weapons["All"] = data; break;
                    case 3: config->streamProofESP.projectiles["All"] = data; break;
                    default: getConfigShared(i, "All") = data; break;
                    }
                }

                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Projectile")) {
                    const auto& data = *(Projectile*)payload->Data;

                    switch (i) {
                    case 0: case 1: getConfigPlayer(i, "All") = data; break;
                    case 2: config->streamProofESP.weapons["All"] = data; break;
                    case 3: config->streamProofESP.projectiles["All"] = data; break;
                    default: getConfigShared(i, "All") = data; break;
                    }
                }

                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity")) {
                    const auto& data = *(Shared*)payload->Data;

                    switch (i) {
                    case 0: case 1: getConfigPlayer(i, "All") = data; break;
                    case 2: config->streamProofESP.weapons["All"] = data; break;
                    case 3: config->streamProofESP.projectiles["All"] = data; break;
                    default: getConfigShared(i, "All") = data; break;
                    }
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::PushID(i);
            ImGui::Indent();

            const auto items = [](std::size_t category) noexcept -> std::vector<const char*> {
                switch (category) {
                case 0:
                case 1: return { "Visible", "Occluded" };
                case 2: return { "Pistols", "SMGs", "Rifles", "Sniper Rifles", "Shotguns", "Machineguns", "Grenades", "Melee", "Other" };
                case 3: return { "Flashbang", "HE Grenade", "Breach Charge", "Bump Mine", "Decoy Grenade", "Molotov", "TA Grenade", "Smoke Grenade", "Snowball" };
                case 4: return { "Pistol Case", "Light Case", "Heavy Case", "Explosive Case", "Tools Case", "Cash Dufflebag" };
                case 5: return { "Defuse Kit", "Chicken", "Planted C4", "Hostage", "Sentry", "Cash", "Ammo Box", "Radar Jammer", "Snowball Pile" };
                default: return { };
                }
            }(i);

            const auto categoryEnabled = getConfigShared(i, "All").enabled;

            for (std::size_t j = 0; j < items.size(); ++j) {
                static bool selectedSubItem;
                if (!categoryEnabled || getConfigShared(i, items[j]).enabled) {
                    if (ImGui::Selectable(items[j], currentCategory == i && !selectedSubItem && std::string_view{ currentItem } == items[j])) {
                        currentCategory = i;
                        currentItem = items[j];
                        selectedSubItem = false;
                    }

                    if (ImGui::BeginDragDropSource()) {
                        switch (i) {
                        case 0: case 1: ImGui::SetDragDropPayload("Player", &getConfigPlayer(i, items[j]), sizeof(Player), ImGuiCond_Once); break;
                        case 2: ImGui::SetDragDropPayload("Weapon", &config->streamProofESP.weapons[items[j]], sizeof(Weapon), ImGuiCond_Once); break;
                        case 3: ImGui::SetDragDropPayload("Projectile", &config->streamProofESP.projectiles[items[j]], sizeof(Projectile), ImGuiCond_Once); break;
                        default: ImGui::SetDragDropPayload("Entity", &getConfigShared(i, items[j]), sizeof(Shared), ImGuiCond_Once); break;
                        }
                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Player")) {
                            const auto& data = *(Player*)payload->Data;

                            switch (i) {
                            case 0: case 1: getConfigPlayer(i, items[j]) = data; break;
                            case 2: config->streamProofESP.weapons[items[j]] = data; break;
                            case 3: config->streamProofESP.projectiles[items[j]] = data; break;
                            default: getConfigShared(i, items[j]) = data; break;
                            }
                        }

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Weapon")) {
                            const auto& data = *(Weapon*)payload->Data;

                            switch (i) {
                            case 0: case 1: getConfigPlayer(i, items[j]) = data; break;
                            case 2: config->streamProofESP.weapons[items[j]] = data; break;
                            case 3: config->streamProofESP.projectiles[items[j]] = data; break;
                            default: getConfigShared(i, items[j]) = data; break;
                            }
                        }

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Projectile")) {
                            const auto& data = *(Projectile*)payload->Data;

                            switch (i) {
                            case 0: case 1: getConfigPlayer(i, items[j]) = data; break;
                            case 2: config->streamProofESP.weapons[items[j]] = data; break;
                            case 3: config->streamProofESP.projectiles[items[j]] = data; break;
                            default: getConfigShared(i, items[j]) = data; break;
                            }
                        }

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity")) {
                            const auto& data = *(Shared*)payload->Data;

                            switch (i) {
                            case 0: case 1: getConfigPlayer(i, items[j]) = data; break;
                            case 2: config->streamProofESP.weapons[items[j]] = data; break;
                            case 3: config->streamProofESP.projectiles[items[j]] = data; break;
                            default: getConfigShared(i, items[j]) = data; break;
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                }

                if (i != 2)
                    continue;

                ImGui::Indent();

                const auto subItems = [](std::size_t item) noexcept -> std::vector<const char*> {
                    switch (item) {
                    case 0: return { "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-SeveN", "CZ75-Auto", "Desert Eagle", "R8 Revolver" };
                    case 1: return { "MAC-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };
                    case 2: return { "Galil AR", "FAMAS", "AK-47", "M4A4", "M4A1-S", "SG 553", "AUG" };
                    case 3: return { "SSG 08", "AWP", "G3SG1", "SCAR-20" };
                    case 4: return { "Nova", "XM1014", "Sawed-Off", "MAG-7" };
                    case 5: return { "M249", "Negev" };
                    case 6: return { "Flashbang", "HE Grenade", "Smoke Grenade", "Molotov", "Decoy Grenade", "Incendiary", "TA Grenade", "Fire Bomb", "Diversion", "Frag Grenade", "Snowball" };
                    case 7: return { "Axe", "Hammer", "Wrench" };
                    case 8: return { "C4", "Healthshot", "Bump Mine", "Zone Repulsor", "Shield" };
                    default: return { };
                    }
                }(j);

                const auto itemEnabled = getConfigShared(i, items[j]).enabled;

                for (const auto subItem : subItems) {
                    auto& subItemConfig = config->streamProofESP.weapons[subItem];
                    if ((categoryEnabled || itemEnabled) && !subItemConfig.enabled)
                        continue;

                    if (ImGui::Selectable(subItem, currentCategory == i && selectedSubItem && std::string_view{ currentItem } == subItem)) {
                        currentCategory = i;
                        currentItem = subItem;
                        selectedSubItem = true;
                    }

                    if (ImGui::BeginDragDropSource()) {
                        ImGui::SetDragDropPayload("Weapon", &subItemConfig, sizeof(Weapon), ImGuiCond_Once);
                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Player")) {
                            const auto& data = *(Player*)payload->Data;
                            subItemConfig = data;
                        }

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Weapon")) {
                            const auto& data = *(Weapon*)payload->Data;
                            subItemConfig = data;
                        }

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Projectile")) {
                            const auto& data = *(Projectile*)payload->Data;
                            subItemConfig = data;
                        }

                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity")) {
                            const auto& data = *(Shared*)payload->Data;
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

    if (ImGui::BeginChild("##child", { 400.0f, 0.0f })) {
        auto& sharedConfig = getConfigShared(currentCategory, currentItem);

        ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &sharedConfig.enabled);
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 260.0f);
        ImGui::SetNextItemWidth(220.0f);
        if (ImGui::BeginCombo("Font", config->getSystemFonts()[sharedConfig.font.index].c_str())) {
            for (size_t i = 0; i < config->getSystemFonts().size(); i++) {
                bool isSelected = config->getSystemFonts()[i] == sharedConfig.font.name;
                if (ImGui::Selectable(config->getSystemFonts()[i].c_str(), isSelected, 0, { 250.0f, 0.0f })) {
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
        ImGuiCustom::colorPicker("Snapline", sharedConfig.snapline);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90.0f);
        ImGui::Combo("##1", &sharedConfig.snapline.type, "Bottom\0Top\0Crosshair\0");
        ImGui::SameLine(spacing);
        ImGuiCustom::colorPicker("Box", sharedConfig.box);
        ImGui::SameLine();

        ImGui::PushID("Box");

        if (ImGui::Button("..."))
            ImGui::OpenPopup("");

        if (ImGui::BeginPopup("")) {
            ImGui::SetNextItemWidth(95.0f);
            ImGui::Combo("Type", &sharedConfig.box.type, "2D\0" "2D corners\0" "3D\0" "3D corners\0");
            ImGui::SetNextItemWidth(275.0f);
            ImGui::SliderFloat3("Scale", sharedConfig.box.scale.data(), 0.0f, 0.50f, "%.2f");
        	ImGuiCustom::colorPicker("Fill", sharedConfig.box.fill);
            ImGui::EndPopup();
        }

        ImGui::PopID();

        ImGuiCustom::colorPicker("Name", sharedConfig.name);
        ImGui::SameLine(spacing);

        if (currentCategory < 2) {
            auto& playerConfig = getConfigPlayer(currentCategory, currentItem);

            ImGuiCustom::colorPicker("Weapon", playerConfig.weapon);
            ImGuiCustom::colorPicker("Flash Duration", playerConfig.flashDuration);
            ImGui::SameLine(spacing);
            ImGuiCustom::colorPicker("Skeleton", playerConfig.skeleton);
            ImGui::Checkbox("Audible Only", &playerConfig.audibleOnly);
            ImGui::SameLine(spacing);
            ImGui::Checkbox("Spotted Only", &playerConfig.spottedOnly);

            ImGuiCustom::colorPicker("Head Box", playerConfig.headBox);
            ImGui::SameLine();

            ImGui::PushID("Head Box");

            if (ImGui::Button("..."))
                ImGui::OpenPopup("");

            if (ImGui::BeginPopup("")) {
                ImGui::SetNextItemWidth(95.0f);
                ImGui::Combo("Type", &playerConfig.headBox.type, "2D\0" "2D corners\0" "3D\0" "3D corners\0");
                ImGui::SetNextItemWidth(275.0f);
                ImGui::SliderFloat3("Scale", playerConfig.headBox.scale.data(), 0.0f, 0.50f, "%.2f");
            	ImGuiCustom::colorPicker("Fill", playerConfig.headBox.fill);
                ImGui::EndPopup();
            }

            ImGui::PopID();

        	ImGui::SameLine(spacing);

            ImGui::Checkbox("Health Bar", &playerConfig.healthBar);
        } else if (currentCategory == 2) {
            auto& weaponConfig = config->streamProofESP.weapons[currentItem];
            ImGuiCustom::colorPicker("Ammo", weaponConfig.ammo);
        } else if (currentCategory == 3) {
            auto& trails = config->streamProofESP.projectiles[currentItem].trails;

            ImGui::Checkbox("Trails", &trails.enabled);
            ImGui::SameLine(spacing + 77.0f);
            ImGui::PushID("Trails");

            if (ImGui::Button("..."))
                ImGui::OpenPopup("");

            if (ImGui::BeginPopup("")) {
                constexpr auto trailPicker = [](const char* name, Trail& trail) noexcept {
                    ImGui::PushID(name);
                    ImGuiCustom::colorPicker(name, trail);
                    ImGui::SameLine(150.0f);
                    ImGui::SetNextItemWidth(95.0f);
                    ImGui::Combo("", &trail.type, "Line\0Circles\0Filled Circles\0");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(95.0f);
                    ImGui::InputFloat("Time", &trail.time, 0.1f, 0.5f, "%.1fs");
                    trail.time = std::clamp(trail.time, 1.0f, 60.0f);
                    ImGui::PopID();
                };

                trailPicker("Local Player", trails.localPlayer);
                trailPicker("Allies", trails.allies);
                trailPicker("Enemies", trails.enemies);
                ImGui::EndPopup();
            }

            ImGui::PopID();
        }

        ImGui::SetNextItemWidth(95.0f);
        ImGui::InputFloat("Text Cull Distance", &sharedConfig.textCullDistance, 0.4f, 0.8f, "%.1fm");
        sharedConfig.textCullDistance = std::clamp(sharedConfig.textCullDistance, 0.0f, 999.9f);
}

    ImGui::EndChild();


    if (!contentOnly)
        ImGui::End();
}

void GUI::renderVisualsWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.visuals)
            return;
        ImGui::SetNextWindowSize({ 680.0f, 0.0f });
        ImGui::Begin("Visuals | BRCheats", &window.visuals, windowFlags);
    }
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 280.0f);
    constexpr auto playerModels = "Default\0Special Agent Ava | FBI\0Operator | FBI SWAT\0Markus Delrow | FBI HRT\0Michael Syfers | FBI Sniper\0B Squadron Officer | SAS\0Seal Team 6 Soldier | NSWC SEAL\0Buckshot | NSWC SEAL\0Lt. Commander Ricksaw | NSWC SEAL\0Third Commando Company | KSK\0'Two Times' McCoy | USAF TACP\0Dragomir | Sabre\0Rezan The Ready | Sabre\0'The Doctor' Romanov | Sabre\0Maximus | Sabre\0Blackwolf | Sabre\0The Elite Mr. Muhlik | Elite Crew\0Ground Rebel | Elite Crew\0Osiris | Elite Crew\0Prof. Shahmat | Elite Crew\0Enforcer | Phoenix\0Slingshot | Phoenix\0Soldier | Phoenix\0Pirate\0Pirate Variant A\0Pirate Variant B\0Pirate Variant C\0Pirate Variant D\0Anarchist\0Anarchist Variant A\0Anarchist Variant B\0Anarchist Variant C\0Anarchist Variant D\0Balkan Variant A\0Balkan Variant B\0Balkan Variant C\0Balkan Variant D\0Balkan Variant E\0Jumpsuit Variant A\0Jumpsuit Variant B\0Jumpsuit Variant C\0";
    ImGui::Checkbox("No weapons", &config->visuals.noWeapons);
    ImGui::Checkbox("No smoke", &config->visuals.noSmoke);
    ImGui::Checkbox("No blur", &config->visuals.noBlur);
    ImGui::Checkbox("No scope overlay", &config->visuals.noScopeOverlay);
    ImGui::Checkbox("No grass", &config->visuals.noGrass);
    ImGui::Checkbox("No shadows", &config->visuals.noShadows);
    ImGui::Checkbox("Night Mode", &config->visuals.nightMode);
    ImGui::Checkbox("Asus walls", &config->visuals.asusWalls);
    ImGui::Checkbox("No bloom", &config->visuals.noBloom);
    ImGui::Checkbox("Disable post-processing", &config->visuals.disablePostProcessing);
    ImGui::Checkbox("Inverse ragdoll gravity", &config->visuals.inverseRagdollGravity);
    ImGui::Checkbox("No fog", &config->visuals.noFog);
    ImGui::Checkbox("No 3d sky", &config->visuals.no3dSky);
    ImGui::Checkbox("No aim punch", &config->visuals.noAimPunch);
    ImGui::Checkbox("No view punch", &config->visuals.noViewPunch);
    ImGui::Checkbox("No hands", &config->visuals.noHands);
    ImGui::Checkbox("No sleeves", &config->visuals.noSleeves);
    ImGui::Checkbox("Wireframe smoke", &config->visuals.wireframeSmoke);
    ImGuiCustom::colorPicker("World color", config->visuals.world);
    ImGuiCustom::colorPicker("Sky color", config->visuals.sky);
    ImGui::Checkbox("Deagle spinner", &config->visuals.deagleSpinner);
    ImGuiCustom::colorPicker("Bullet Tracers", config->visuals.bulletTracers);
    ImGui::Checkbox("Zoom", &config->visuals.zoom);
    ImGui::SameLine();
    hotkey(config->visuals.zoomKey);
    ImGui::Combo("T Player Model", &config->visuals.playerModelT, playerModels);
    ImGui::Combo("CT Player Model", &config->visuals.playerModelCT, playerModels);
    ImGui::NextColumn();
    ImGui::Checkbox("Thirdperson", &config->visuals.thirdperson);
    ImGui::SameLine();
    hotkey(config->visuals.thirdpersonKey);
    ImGui::PushItemWidth(290.0f);
    ImGui::PushID(0);
    ImGui::SliderInt("", &config->visuals.thirdpersonDistance, 0, 1000, "Thirdperson distance: %d");
    ImGui::PopID();
    ImGui::PushID(1);
    ImGui::SliderInt("", &config->visuals.viewmodelFov, -60, 60, "Viewmodel FOV: %d");
    ImGui::PopID();
    ImGui::PushID(2);
    ImGui::SliderInt("", &config->visuals.fov, -60, 60, "FOV: %d");
    ImGui::PopID();
    ImGui::PushID(3);
    ImGui::SliderInt("", &config->visuals.farZ, 0, 2000, "Far Z: %d");
    ImGui::PopID();
    ImGui::PushID(4);
    ImGui::SliderInt("", &config->visuals.flashReduction, 0, 100, "Flash reduction: %d%%");
    ImGui::PopID();
    ImGui::PushID(5);
    ImGui::SliderFloat("", &config->visuals.brightness, 0.0f, 1.0f, "Brightness: %.2f");
    ImGui::PopID();
    ImGui::PopItemWidth();
    ImGui::Combo("Skybox", &config->visuals.skybox, Helpers::skyboxList.data(), Helpers::skyboxList.size());
    ImGui::Combo("Screen effect", &config->visuals.screenEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
    ImGui::Combo("Hit effect", &config->visuals.hitEffect, "None\0Drone cam\0Drone cam with noise\0Underwater\0Healthboost\0Dangerzone\0");
    ImGui::SliderFloat("Hit effect time", &config->visuals.hitEffectTime, 0.1f, 1.5f, "%.2fs");
    ImGui::Combo("Hit marker", &config->visuals.hitMarker, "None\0Default (Cross)\0");
    ImGui::SliderFloat("Hit marker time", &config->visuals.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
	ImGui::Checkbox("Indicators", &config->visuals.indicatorsEnabled);
    ImGui::SameLine();
    ImGui::PushID(6);
    ImGuiCustom::MultiCombo("", config->visuals.indicators, config->visuals.selectedIndicators, 5);
    ImGui::PopID();
    ImGui::Checkbox("Rainbow crosshair", &config->visuals.rainbowCrosshair);
    ImGui::SameLine();
    ImGui::PushItemWidth(100.0f);
    config->visuals.rainbowCrosshairSpeed = std::clamp(config->visuals.rainbowCrosshairSpeed, 0.0f, 25.0f);
    ImGui::InputFloat("Speed", &config->visuals.rainbowCrosshairSpeed, 0.1f, 0.15f, "%.2f");
    ImGui::PopItemWidth();
    ImGuiCustom::colorPicker("Noscope crosshair", config->misc.noscopeCrosshair);
    ImGuiCustom::colorPicker("Recoil crosshair", config->misc.recoilCrosshair);
    ImGuiCustom::colorPicker("Watermark", config->misc.watermark);
    ImGui::Checkbox("Color correction", &config->visuals.colorCorrection.enabled);
    ImGui::SameLine();
    bool ccPopup = ImGui::Button("Edit");
    ImGui::Checkbox("Ragdoll force", &config->misc.ragdollForce);
    if (config->misc.ragdollForce) {
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputInt("Ragdoll force strength", &config->misc.ragdollForceStrength, 1, 3);
        config->misc.ragdollForceStrength = std::clamp(config->misc.ragdollForceStrength, 1, 100);
    };
    if (ccPopup)
        ImGui::OpenPopup("##popup");

    if (ImGui::BeginPopup("##popup")) {
        ImGui::VSliderFloat("##1", { 40.0f, 160.0f }, &config->visuals.colorCorrection.blue, 0.0f, 1.0f, "Blue\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##2", { 40.0f, 160.0f }, &config->visuals.colorCorrection.red, 0.0f, 1.0f, "Red\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##3", { 40.0f, 160.0f }, &config->visuals.colorCorrection.mono, 0.0f, 1.0f, "Mono\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##4", { 40.0f, 160.0f }, &config->visuals.colorCorrection.saturation, 0.0f, 1.0f, "Sat\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##5", { 40.0f, 160.0f }, &config->visuals.colorCorrection.ghost, 0.0f, 1.0f, "Ghost\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##6", { 40.0f, 160.0f }, &config->visuals.colorCorrection.green, 0.0f, 1.0f, "Green\n%.3f"); ImGui::SameLine();
        ImGui::VSliderFloat("##7", { 40.0f, 160.0f }, &config->visuals.colorCorrection.yellow, 0.0f, 1.0f, "Yellow\n%.3f"); ImGui::SameLine();
        ImGui::EndPopup();
    }

    ImGui::Columns(1);

    if (!contentOnly)
        ImGui::End();
}

void GUI::renderSkinChangerWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.skinChanger)
            return;
        ImGui::SetNextWindowSize({ 700.0f, 0.0f });
        ImGui::Begin("Skin Changer | BRCheats", &window.skinChanger, windowFlags);
    }

    static auto itemIndex = 0;

    ImGui::PushItemWidth(110.0f);
    ImGui::Combo("##1", &itemIndex, [](void* data, int idx, const char** out_text) {
        *out_text = game_data::weapon_names[idx].name;
        return true;
        }, nullptr, IM_ARRAYSIZE(game_data::weapon_names), 5);
    ImGui::PopItemWidth();

    auto& selected_entry = config->skinChanger[itemIndex];
    selected_entry.itemIdIndex = itemIndex;

    {
        ImGui::SameLine();
        ImGui::Checkbox(phrases[XorString("global_enabled")].c_str(), &selected_entry.enabled);
        ImGui::Separator();
        ImGui::Columns(2, nullptr, false);

        static std::string filter;
        ImGui::PushID("Search");
        ImGui::InputTextWithHint("", "Search", &filter);
        ImGui::PopID();

        if (ImGui::ListBoxHeader("Paint Kit")) {
            const auto& kits = itemIndex == 1 ? SkinChanger::getGloveKits() : SkinChanger::getSkinKits();

            const std::locale original;
            std::locale::global(std::locale{ "en_US.utf8" });

            const auto& facet = std::use_facet<std::ctype<wchar_t>>(std::locale{});
            std::wstring filterWide(filter.length(), L'\0');
            const auto newLen = mbstowcs(filterWide.data(), filter.c_str(), filter.length());
            if (newLen != static_cast<std::size_t>(-1))
                filterWide.resize(newLen);
            std::transform(filterWide.begin(), filterWide.end(), filterWide.begin(), [&facet](wchar_t w) { return facet.toupper(w); });

            std::locale::global(original);

            for (std::size_t i = 0; i < kits.size(); ++i) {
                if (filter.empty() || wcsstr(kits[i].nameUpperCase.c_str(), filterWide.c_str())) {
                    ImGui::PushID(i);
                    if (ImGui::Selectable(kits[i].name.c_str(), i == selected_entry.paint_kit_vector_index))
                        selected_entry.paint_kit_vector_index = i;
                    ImGui::PopID();
                }
            }
            ImGui::ListBoxFooter();
        }

        ImGui::Combo("Quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text) {
            *out_text = game_data::quality_names[idx].name;
            return true;
            }, nullptr, IM_ARRAYSIZE(game_data::quality_names), 5);

        ImGui::InputInt("Seed", &selected_entry.seed);

        ImGui::InputInt("StatTrak\u2122", &selected_entry.stat_trak);

        selected_entry.stat_trak = (std::max)(selected_entry.stat_trak, -1);

        ImGui::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", ImGuiSliderFlags_Logarithmic);
        if (itemIndex == 0) {
            ImGui::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = game_data::knife_names[idx].name;
                return true;
                }, nullptr, IM_ARRAYSIZE(game_data::knife_names), 5);
        } else if (itemIndex == 1) {
            ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
                *out_text = game_data::glove_names[idx].name;
                return true;
                }, nullptr, IM_ARRAYSIZE(game_data::glove_names), 5);
        }

        ImGui::InputText("Name Tag", selected_entry.custom_name, 32);
    }

    ImGui::NextColumn();

    {
        ImGui::PushID("sticker");

        static auto selectedStickerSlot = 0;

        ImGui::PushItemWidth(-1);

        if (ImGui::ListBoxHeader("", 5)) {
            for (int i = 0; i < 5; ++i) {
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

        auto& selected_sticker = selected_entry.stickers[selectedStickerSlot];

        static std::string filter;
        ImGui::PushID("Search");
        ImGui::InputTextWithHint("", "Search", &filter);
        ImGui::PopID();

        if (ImGui::ListBoxHeader("Sticker")) {
            const auto& kits = SkinChanger::getStickerKits();

            const std::locale original;
            std::locale::global(std::locale{ "en_US.utf8" });

            const auto& facet = std::use_facet<std::ctype<wchar_t>>(std::locale{});
            std::wstring filterWide(filter.length(), L'\0');
            const auto newLen = mbstowcs(filterWide.data(), filter.c_str(), filter.length());
            if (newLen != static_cast<std::size_t>(-1))
                filterWide.resize(newLen);
            std::transform(filterWide.begin(), filterWide.end(), filterWide.begin(), [&facet](wchar_t w) { return facet.toupper(w); });

            std::locale::global(original);

            for (std::size_t i = 0; i < kits.size(); ++i) {
                if (filter.empty() || wcsstr(kits[i].nameUpperCase.c_str(), filterWide.c_str())) {
                    ImGui::PushID(i);
                    if (ImGui::Selectable(kits[i].name.c_str(), i == selected_sticker.kit_vector_index))
                        selected_sticker.kit_vector_index = i;
                    ImGui::PopID();
                }
            }
            ImGui::ListBoxFooter();
        }

        ImGui::SliderFloat("Wear", &selected_sticker.wear, FLT_MIN, 1.0f, "%.10f", ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat("Scale", &selected_sticker.scale, 0.1f, 5.0f);
        ImGui::SliderFloat("Rotation", &selected_sticker.rotation, 0.0f, 360.0f);

        ImGui::PopID();
    }
    selected_entry.update();

    ImGui::Columns(1);

    ImGui::Separator();

    if (ImGui::Button("Update", { 130.0f, 30.0f }))
        SkinChanger::scheduleHudUpdate();


    if (!contentOnly)
        ImGui::End();
}

void GUI::renderMiscWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.misc)
            return;
        ImGui::SetNextWindowSize({ 580.0f, 0.0f });
        ImGui::Begin("Misc | BRCheats", &window.misc, windowFlags);
    }
    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 230.0f);

    ImGui::SetNextItemWidth(120.0f);
    ImGui::SliderInt("Lang", &config->misc.lang, 0, 1);

    ImGui::TextUnformatted("Menu key");
    ImGui::SameLine();
    hotkey(config->misc.menuKey);

    ImGui::Checkbox("Radar hack", &config->misc.radarHack);
    ImGui::Checkbox("Spectator List", &config->misc.spectatorList.enabled);
    ImGui::PushID("Spectator List");
    ImGui::SameLine();
    if (ImGui::Button("..."))
        ImGui::OpenPopup("C");

    if (ImGui::BeginPopup("C")) {
        ImGui::Checkbox("No BackGround", &config->misc.spectatorList.noBackGround);
        ImGui::Checkbox("No TittleBar", &config->misc.spectatorList.noTittleBar);
        ImGui::EndPopup();
    }
    ImGui::PopID();

    ImGui::Checkbox("Reveal ranks", &config->misc.revealRanks);
    ImGui::Checkbox("Reveal money", &config->misc.revealMoney);
    ImGui::Checkbox("Reveal suspect", &config->misc.revealSuspect);
    ImGui::Checkbox("Fix animation LOD", &config->misc.fixAnimationLOD);
    ImGui::Checkbox("Fix bone matrix", &config->misc.fixBoneMatrix);
    ImGui::Checkbox("Fix movement", &config->misc.fixMovement);
    ImGui::Checkbox("Disable model occlusion", &config->misc.disableModelOcclusion);
    ImGui::Checkbox("Anti AFK kick", &config->misc.antiAfkKick);
    ImGui::Checkbox("Auto strafe", &config->misc.autoStrafe);
    ImGui::Checkbox("Bunny hop", &config->misc.bunnyHop);
    ImGui::Checkbox("Fast duck", &config->misc.fastDuck);
    ImGui::Checkbox("Moonwalk", &config->misc.moonwalk);
    ImGui::Checkbox("Edge Jump", &config->misc.edgejump);
    ImGui::SameLine();
    hotkey(config->misc.edgejumpkey);
    ImGui::Checkbox("Slowwalk", &config->misc.slowwalk);
    ImGui::SameLine();
    hotkey(config->misc.slowwalkKey);
    ImGui::Checkbox("Auto pistol", &config->misc.autoPistol);
    ImGui::Checkbox("Auto reload", &config->misc.autoReload);
    ImGui::Checkbox("Auto accept", &config->misc.autoAccept);
    ImGui::Checkbox("Disable HUD blur", &config->misc.disablePanoramablur);
    ImGui::Checkbox("Fast plant", &config->misc.fastPlant);
    ImGui::Checkbox("Fast Stop", &config->misc.fastStop);
    ImGuiCustom::colorPicker("Bomb timer", config->misc.bombTimer);
    ImGui::Checkbox("Quick reload", &config->misc.quickReload);
    ImGui::Checkbox("Prepare revolver", &config->misc.prepareRevolver);
    ImGui::SameLine();
    hotkey(config->misc.prepareRevolverKey);


    ImGui::NextColumn(); ////////////////////////////////////////////////////


    ImGui::Checkbox("Animated clan tag", &config->misc.animatedClanTag);
    ImGui::Checkbox("Clock tag", &config->misc.clocktag);
    ImGui::Checkbox("Custom clantag", &config->misc.customClanTag);
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(0);

    if (ImGui::InputText("", config->misc.clanTag, sizeof(config->misc.clanTag)))
        Misc::updateClanTag(true);
    ImGui::PopID();
    ImGui::Checkbox("Kill message", &config->misc.killMessage);
    ImGui::SameLine();
    ImGui::PushItemWidth(120.0f);
    ImGui::PushID(1);
    ImGui::InputText("", &config->misc.killMessageString);
    ImGui::PopID();

    ImGui::Combo("Name Exploit", &config->misc.nameChangeSelection, "Off\0Fake Ban\0Fake Item\0Custom Name");
    if (config->misc.nameChangeSelection == 1)
    {
        ImGui::PushID(3);
        ImGui::SetNextItemWidth(100.0f);
        ImGui::Combo("", &config->misc.banColor, "White\0Red\0Purple\0Green\0Light green\0Turquoise\0Light red\0Gray\0Yellow\0Gray 2\0Light blue\0Gray/Purple\0Blue\0Pink\0Dark orange\0Orange\0");
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::PushID(4);
        ImGui::InputText("", &config->misc.banText);
        ImGui::PopID();
        ImGui::SameLine();
        if (ImGui::Button("Setup fake ban"))
            Misc::fakeBan(true);
    }
    else if (config->misc.nameChangeSelection == 2)
    {
        ImGui::SetNextItemWidth(200.0f);
        ImGuiCustom::MultiCombo("Fake Item Flags", config->misc.fakeItemFlags, config->misc.selectedFakeItemFlags, 4);
        ImGui::SetNextItemWidth(200.0f);
        ImGui::Combo("Team", &config->misc.fakeItemTeam, "Counter-Terrorist\0Terrorist");
        ImGui::SetNextItemWidth(200.0f);
        ImGui::Combo("Message Type", &config->misc.fakeItemMessageType, "Unbox\0Trade\0");
        ImGui::SetNextItemWidth(200.0f);
        ImGui::Combo("Item Type", &config->misc.fakeItemType, "AK-47\0AUG\0AWP\0Bayonet\0Bowie Knife\0Butterfly Knife\0CZ75-Auto\0Classic Knife\0Desert Eagle\0Dual Berettas\0FAMAS\0Falchion Knife\0Five-SeveN\0Flip Knife\0G3SG1\0Galil AR\0Glock-18\0Gut Knife\0Huntsman Knife\0Karambit\0M249\0M4A1-S\0M4A4\0M9 Bayonet\0MAC-10\0MAG-7\0MP5-SD\0MP7\0MP9\0Navaja Knife\0Negev\0Nomad Knife\0Nova\0P2000\0P250\0P90\0PP-Bizon\0Paracord Knife\0R8 Revolver\0SCAR-20\0SG 553\0SSG 08\0Sawed-Off\0Shadow Daggers\0Skeleton Knife\0Spectral Shiv\0Stiletto Knife\0Survival Knife\0Talon Knife\0Tec-9\0UMP-45\0USP-S\0Ursus Knife\0XM1014\0Hand Wraps\0Moto Gloves\0Specialist Gloves\0Sport Gloves\0Bloodhound Gloves\0Hydra Gloves\0Driver Gloves\0");
        ImGui::SetNextItemWidth(200.0f);
        ImGui::Combo("Item Color/Rarity", &config->misc.fakeItemRarity, "Consumer Grade (White)\0Industrial Grade (Light blue)\0Mil-Spec (Blue)\0Restricted (Purple)\0Classified (Pink)\0Covert (Red)\0Contrabanned(Orange/Gold)\0");
        ImGui::Combo("Player Color", &config->misc.fakeItemPlayerColor, "Yellow\0Green\0Blue\0Purple\0Orange");
        ImGui::InputText("Player Name", &config->misc.fakeItemPlayerName);
        ImGui::InputText("Skin Name", &config->misc.fakeItemName);
        if (ImGui::Button("Change Name"))
            Misc::fakeItem(true);
    }
    else if (config->misc.nameChangeSelection == 3)
    {
        ImGui::Checkbox("Name stealer", &config->misc.nameStealer);
        ImGui::InputText("Custom Name", &config->misc.customName);
        if (ImGui::Button("Change Name"))
            Misc::setName(true);
    }
    
    ImGui::Combo("Hit Sound", &config->misc.hitSound, "None\0Metal\0Gamesense\0Bell\0Glass\0Custom\0");
    if (config->misc.hitSound == 5) {
        ImGui::InputText("Hit Sound filename", &config->misc.customHitSound);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("audio file must be put in csgo/sound/ directory");
    }
    ImGui::PushID(5);
    ImGui::Combo("Kill Sound", &config->misc.killSound, "None\0Metal\0Gamesense\0Bell\0Glass\0Custom\0");
    if (config->misc.killSound == 5) {
        ImGui::InputText("Kill Sound filename", &config->misc.customKillSound);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("audio file must be put in csgo/sound/ directory");
    }
    ImGui::PopID();

    ImGui::Combo("Fake Lag", &config->misc.fakeLagMode, "Off\0Normal\0Adaptive\0Random\0Switch");
    ImGui::SameLine();
	 hotkey(config->misc.fakeLagKey);
    if (!(config->misc.fakeLagMode == 0))
    {
        ImGuiCustom::MultiCombo("Flags", config->misc.fakeLagFlags, config->misc.fakeLagSelectedFlags, 4);
        if (config->misc.fakeLagMode == 3)
        {
            ImGui::SetNextItemWidth(120.0f);
            ImGui::SliderInt("Min Fakelag Amount", &config->misc.fakeLagTicks, 1, 16);
        }
        else if (!(config->misc.fakeLagMode == 4))
        {
            ImGui::SetNextItemWidth(120.0f);
            ImGui::SliderInt("Fakelag Amount", &config->misc.fakeLagTicks, 1, 16);
        }
    }

    ImGui::Text("Quick healthshot");
    ImGui::SameLine();
    hotkey(config->misc.quickHealthshotKey);
    ImGui::Checkbox("Grenade Prediction", &config->misc.nadePredict);
    ImGui::Checkbox("Fix tablet signal", &config->misc.fixTabletSignal);
    ImGui::SetNextItemWidth(120.0f);
    ImGui::SliderFloat("Max angle delta", &config->misc.maxAngleDelta, 0.0f, 255.0f, "%.2f");
    ImGui::SliderFloat("Aspect Ratio", &config->misc.aspectratio, 0.0f, 5.0f, "%.2f");
    ImGui::Checkbox("Fake prime", &config->misc.fakePrime);

    ImGui::Checkbox("Fakeduck", &config->misc.fakeDuck);
    ImGui::SameLine();
    hotkey(config->misc.fakeDuckKey);

    ImGui::Checkbox("Door spam", &config->misc.doorSpam);
    ImGui::Checkbox("Preserve Killfeed", &config->misc.preserveKillfeed.enabled);
    ImGui::SameLine();

    ImGui::PushID("Preserve Killfeed");
    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::Checkbox("Only Headshots", &config->misc.preserveKillfeed.onlyHeadshots);
        ImGui::EndPopup();
    }
    ImGui::PopID();

    ImGui::Checkbox("Purchase List", &config->misc.purchaseList.enabled);
    ImGui::SameLine();

    ImGui::PushID("Purchase List");
    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::SetNextItemWidth(75.0f);
        ImGui::Combo("Mode", &config->misc.purchaseList.mode, "Details\0Summary\0");
        ImGui::Checkbox("Only During Freeze Time", &config->misc.purchaseList.onlyDuringFreezeTime);
        ImGui::Checkbox("Show Prices", &config->misc.purchaseList.showPrices);
        ImGui::Checkbox("No Title Bar", &config->misc.purchaseList.noTitleBar);
        ImGui::EndPopup();
    }
    ImGui::PopID();

	ImGui::Checkbox("Shots Cout", &config->misc.ShotsCout.enabled);
    ImGui::PushID("Shots Cout");
    ImGui::SameLine();
    if (ImGui::Button("..."))
        ImGui::OpenPopup("A");

    if (ImGui::BeginPopup("A")){
        ImGui::Checkbox("No BackGround", &config->misc.ShotsCout.noBackGround);
        ImGui::Checkbox("No TittleBar", &config->misc.ShotsCout.noTittleBar);
        ImGui::EndPopup();
    }
    ImGui::PopID();


	ImGui::Checkbox("Status Bar", &config->misc.Sbar.enabled);
	ImGui::SameLine();

	ImGui::PushID("StatusBar");
	if (ImGui::Button("..."))
		ImGui::OpenPopup("S");

	if (ImGui::BeginPopup("S")) {
		ImGui::Checkbox("NoBackGround", &config->misc.Sbar.noBackGround);
		ImGui::Checkbox("NoTittleBar", &config->misc.Sbar.noTittleBar);
		ImGui::Checkbox("ShowViewAngles", &config->misc.Sbar.ShowPlayerRealViewAngles);
		ImGui::Checkbox("ShowPlayerStatus", &config->misc.Sbar.ShowPlayerStatus);
		ImGui::Checkbox("ShowGameGlobalVars", &config->misc.Sbar.ShowGameGlobalVars);
		ImGui::EndPopup();
	}
	ImGui::PopID();

    ImGui::Checkbox("Reportbot", &config->misc.reportbot.enabled);
    ImGui::SameLine();
    ImGui::PushID("Reportbot");

    if (ImGui::Button("..."))
        ImGui::OpenPopup("");

    if (ImGui::BeginPopup("")) {
        ImGui::PushItemWidth(80.0f);
        ImGui::Combo("Target", &config->misc.reportbot.target, "Enemies\0Allies\0All\0");
        ImGui::InputInt("Delay (s)", &config->misc.reportbot.delay);
        config->misc.reportbot.delay = (std::max)(config->misc.reportbot.delay, 1);
        ImGui::InputInt("Rounds", &config->misc.reportbot.rounds);
        config->misc.reportbot.rounds = (std::max)(config->misc.reportbot.rounds, 1);
        ImGui::PopItemWidth();
        ImGui::Checkbox("Abusive Communications", &config->misc.reportbot.textAbuse);
        ImGui::Checkbox("Griefing", &config->misc.reportbot.griefing);
        ImGui::Checkbox("Wall Hacking", &config->misc.reportbot.wallhack);
        ImGui::Checkbox("Aim Hacking", &config->misc.reportbot.aimbot);
        ImGui::Checkbox("Other Hacking", &config->misc.reportbot.other);
        if (ImGui::Button("Reset"))
            Misc::resetReportbot();
        ImGui::EndPopup();
    }
    ImGui::PopID();
    ImGui::Checkbox("Cheat Spam", &config->misc.cheatSpam);

    ImGui::Columns(1);
    if (!contentOnly)
        ImGui::End();
}

void GUI::renderConfigWindow(bool contentOnly) noexcept
{
    if (!contentOnly) {
        if (!window.config)
            return;
        ImGui::SetNextWindowSize({ 290.0f, 0.0f });
        ImGui::Begin("Config | BRCheats", &window.config, windowFlags);
    }

    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnOffset(1, 170.0f);

    static bool incrementalLoad = false;
    ImGui::PushItemWidth(160.0f);

    if (ImGui::Button("Reload configs", { 160.0f, 25.0f }))
        config->listConfigs();

    auto& configItems = config->getConfigs();
    static int currentConfig = -1;

    if (static_cast<std::size_t>(currentConfig) >= configItems.size())
        currentConfig = -1;

    static std::string buffer;

    if (ImGui::ListBox("", &currentConfig, [](void* data, int idx, const char** out_text) {
        auto& vector = *static_cast<std::vector<std::string>*>(data);
        *out_text = vector[idx].c_str();
        return true;
        }, &configItems, configItems.size(), 5) && currentConfig != -1)
            buffer = configItems[currentConfig];

        ImGui::PushID(0);
        if (ImGui::InputTextWithHint("", "config name", &buffer, ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (currentConfig != -1)
                config->rename(currentConfig, buffer.c_str());
        }
        ImGui::PopID();
        ImGui::NextColumn();

        ImGui::PushItemWidth(100.0f);

        if (ImGui::Button("Create config", { 100.0f, 25.0f }))
            config->add(buffer.c_str());

        if (ImGui::Button("Reset config", { 100.0f, 25.0f }))
            ImGui::OpenPopup("Config to reset");

        if (ImGui::BeginPopup("Config to reset")) {
            static constexpr const char* names[]{ "Whole", "Aimbot", "Ragebot" ,"Triggerbot", "Backtrack", "Anti aim", "Glow", "Chams", "ESP", "Visuals", "Skin changer", "Sound", "Style", "Misc" };
            for (int i = 0; i < IM_ARRAYSIZE(names); i++) {
                if (i == 1) ImGui::Separator();

                if (ImGui::Selectable(names[i])) {
                    switch (i) {
                    case 0: config->reset(); updateColors(); Misc::updateClanTag(true); SkinChanger::scheduleHudUpdate(); break;
                    case 1: config->aimbot = { }; break;
                    case 2: config->ragebot = { }; break;
                    case 3: config->triggerbot = { }; break;
                    case 4: config->backtrack = { }; break;
                    case 5: config->antiAim = { }; break;
                    case 6: config->glow = { }; break;
                    case 7: config->chams = { }; break;
                    case 8: config->streamProofESP = { }; break;
                    case 9: config->visuals = { }; break;
                    case 10: config->skinChanger = { }; SkinChanger::scheduleHudUpdate(); break;
                    case 11: config->sound = { }; break;
                    case 12: config->style = { }; updateColors(); break;
                    case 13: config->misc = { };  Misc::updateClanTag(true); break;
                    }
                }
            }
            ImGui::EndPopup();
        }
        if (currentConfig != -1) {
            if (ImGui::Button("Load selected", { 100.0f, 25.0f })) {
                config->load(currentConfig, incrementalLoad);
                updateColors();
                SkinChanger::scheduleHudUpdate();
                Misc::updateClanTag(true);
            }
            if (ImGui::Button("Save selected", { 100.0f, 25.0f }))
                config->save(currentConfig);
            if (ImGui::Button("Delete selected", { 100.0f, 25.0f })) {
                config->remove(currentConfig);
                currentConfig = -1;
                buffer.clear();
            }
        }
        ImGui::Columns(1);
        if (!contentOnly)
            ImGui::End();
}

void GUI::renderGuiStyle2() noexcept
{   
    ImGui::Begin("BRCheats", nullptr, windowFlags | ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip)) {
        if (ImGui::BeginTabItem("Aimbot")) {
            renderAimbotWindow(true);
            ImGui::EndTabItem();
		}
        if (ImGui::BeginTabItem("Ragebot")) {
			renderRagebotWindow(true);
			ImGui::EndTabItem();
		}
        if (ImGui::BeginTabItem("Anti aim")) {
            renderAntiAimWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Triggerbot")) {
            renderTriggerbotWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Backtrack")) {
            renderBacktrackWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Glow")) {
            renderGlowWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Chams")) {
            renderChamsWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("ESP")) {
            renderStreamProofESPWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Visuals")) {
            renderVisualsWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Skin changer")) {
            renderSkinChangerWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Misc")) {
            renderMiscWindow(true);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Config")) {
            renderConfigWindow(true);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}


void GUI::renderGuiStyle3() noexcept
{
    ImGui::SetNextWindowSize({ 100.0f, 0.0f });
    if (ImGui::Begin(phrases[XorString("main_windowTitle")].c_str(), &gui->open, windowFlags)) {
        if (ImGui::Button("Aimbot", ImVec2(-1.0f, 0.0f))) {
            window.aimbot = !window.aimbot;
        }
        if (ImGui::Button("Ragebot", ImVec2(-1.0f, 0.0f))) {
            window.ragebot = !window.ragebot;
        }
        if (ImGui::Button("Anti aim", ImVec2(-1.0f, 0.0f))) {
            window.antiAim = !window.antiAim;
        }
        if (ImGui::Button("Triggerbot", ImVec2(-1.0f, 0.0f))) {
            window.triggerbot = !window.triggerbot;
        }
        if (ImGui::Button("Backtrack", ImVec2(-1.0f, 0.0f))) {
            window.backtrack = !window.backtrack;
        }
        if (ImGui::Button("Glow", ImVec2(-1.0f, 0.0f))) {
            window.glow = !window.glow;
        }
        if (ImGui::Button("Chams", ImVec2(-1.0f, 0.0f))) {
            window.chams = !window.chams;
        }
        if (ImGui::Button("ESP", ImVec2(-1.0f, 0.0f))) {
            window.streamProofESP = !window.streamProofESP;
        }
        if (ImGui::Button("Visuals", ImVec2(-1.0f, 0.0f))) {
            window.visuals = !window.visuals;
        }
        if (ImGui::Button("Skin Changer", ImVec2(-1.0f, 0.0f))) {
            window.skinChanger = !window.skinChanger;
        }
        if (ImGui::Button("Misc", ImVec2(-1.0f, 0.0f))) {
            window.misc = !window.misc;
        }
        if (ImGui::Button("Config", ImVec2(-1.0f, 0.0f))) {
            window.config = !window.config;
        }
    }
    ImGui::End();
}
