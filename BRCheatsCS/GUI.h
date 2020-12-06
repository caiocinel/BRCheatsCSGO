#pragma once

#include <memory>
#include <string>

struct ImFont;

class GUI {
public:
    GUI() noexcept;
    void render() noexcept;
    bool open = true;
		struct {
		ImFont* tahoma = nullptr;
		ImFont* segoeui = nullptr;
        ImFont* arial = nullptr;
	} fonts;
private:
    static void hotkey(int&) noexcept;
    void updateColors() const noexcept;
	void drawLuaItems(int tab, int column);
    void renderAimbotWindow(bool contentOnly = false) noexcept;
    void renderRagebotWindow(bool contentOnly = false) noexcept;
    void renderAntiAimWindow(bool contentOnly = false) noexcept;
    void renderTriggerbotWindow(bool contentOnly = false) noexcept;
    void renderBacktrackWindow(bool contentOnly = false) noexcept;
    void renderGlowWindow(bool contentOnly = false) noexcept;
    void renderChamsWindow(bool contentOnly = false) noexcept;
    void renderStreamProofESPWindow(bool contentOnly = false) noexcept;
    void renderVisualsWindow(bool contentOnly = false) noexcept;
    void renderWorldWindow(bool contentOnly = false) noexcept;
    void renderSkinChangerWindow(bool contentOnly = false) noexcept;
    void renderProfileChangerWindow(bool contentOnly = false) noexcept;
    void renderMedalChangerWindow(bool contentOnly = false) noexcept;
    void renderInventoryChangerWindow(bool contentOnly = false) noexcept;
    void renderMiscWindow(bool contentOnly = false) noexcept;
    void renderAutoConfigWindow(bool contentOnly = false) noexcept;
    void renderConfigWindow(bool contentOnly = false) noexcept;
    void renderGuiStyle3() noexcept;
    void renderWarningWindow() noexcept;

    struct {
        bool warning = true;
        bool aimbot = false;
        bool ragebot = false;
        bool antiAim = false;
        bool triggerbot = false;
        bool backtrack = false;
        bool glow = false;
        bool chams = false;
        bool streamProofESP = false;
        bool visuals = false;
        bool world = false;
        bool skinChanger = false;
        bool profileChanger = false;
        bool medalChanger = false;
        bool inventoryChanger = false;
        bool sound = false;
        bool style = false;
        bool misc = false;
        bool config = false;
        bool autoconfig = false;
    } window;
};
inline std::unique_ptr<GUI> gui;

