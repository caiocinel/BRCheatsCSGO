#pragma once
#include <string>
#include <sstream>
#include "../SDK/Vector.h"

enum class FrameStage;
class GameEvent;
struct ImDrawList;
struct UserCmd;

namespace Misc
{
    void edgejump(UserCmd* cmd) noexcept;
    void slowwalk(UserCmd* cmd) noexcept;
    void inverseRagdollGravity() noexcept;
    void updateClanTag(bool = false) noexcept;
    void spectatorList() noexcept;
    void noscopeCrosshair(ImDrawList* drawlist) noexcept;
    void recoilCrosshair(ImDrawList* drawList) noexcept;
    void watermark(ImDrawList* drawList) noexcept;
    void prepareRevolver(UserCmd*) noexcept;
    void fastPlant(UserCmd*) noexcept;
    void fastStop(UserCmd*) noexcept;
    void drawBombTimer() noexcept;
    void drawFov(ImDrawList* drawList) noexcept;
    void stealNames() noexcept;
    void disablePanoramablur() noexcept;
    void quickReload(UserCmd*) noexcept;
    bool changeName(bool, const char*, float) noexcept;
    void bunnyHop(UserCmd*) noexcept;
    void fakeBan(bool = false) noexcept;
    void setName(bool set) noexcept;
    void fakeItem(bool set) noexcept;
	void nadePredict() noexcept;
    void quickHealthshot(UserCmd*) noexcept;
    void fixTabletSignal() noexcept;
    void drawBombDamage() noexcept;
    void killMessage(GameEvent& event) noexcept;
    void fixMovement(UserCmd* cmd, float yaw) noexcept;
    void antiAfkKick(UserCmd* cmd) noexcept;
    void fixAnimationLOD(FrameStage stage) noexcept;
    void ragdollForce() noexcept;
    void autoPistol(UserCmd* cmd) noexcept;
    void autoReload(UserCmd* cmd) noexcept;
    void revealRanks(UserCmd* cmd) noexcept;
    void autoStrafe(UserCmd* cmd) noexcept;
    void removeCrouchCooldown(UserCmd* cmd) noexcept;
    void moonwalk(UserCmd* cmd) noexcept;
    void playHitSound(GameEvent& event) noexcept;
    void killSound(GameEvent& event) noexcept;
    void hiddenCvar() noexcept;
    void purchaseList(GameEvent* event = nullptr) noexcept;
	void fakeDuck(UserCmd* cmd, bool& sendPacket) noexcept;
    float RandomFloat(float min, float max) noexcept;
    void chokePackets(bool& sendPacket, UserCmd* cmd) noexcept;
	void drawStartPos(ImDrawList* dl, Vector &quickpeekstartpos) noexcept;
	void drawWallbangVector(ImDrawList* dl, Vector &wallbangVector, int wallDmg, int wallChance) noexcept;
    void doorSpam(UserCmd* cmd) noexcept;
    void cheatSpam() noexcept;
    void runReportbot() noexcept;
    void resetReportbot() noexcept;
    void preserveKillfeed(bool roundStart = false) noexcept;
    void ChatSpammer() noexcept;
}


class pstring : public std::string
{
public:
    pstring() : std::string() {}

    template<typename T>
    pstring(const T v) : std::string(v) {}

    template<typename T>
    pstring& operator<<(const T s)
    {
        std::stringstream stream;
        stream << *this;
        stream << s;
        *this = stream.str();
        return *this;
    }

    pstring& operator+(const unsigned int i)
    {
        std::stringstream stream;
        stream << *this;
        stream << i;
        *this = stream.str();
        return *this;
    }
};