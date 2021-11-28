#pragma once
#include <mutex>
#include <numeric>
#include <sstream>
#include <charconv>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../Netvars.h"

#include "../imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"
#include "../imguiCustom.h"

#include "EnginePrediction.h"
#include "Misc.h"

#include "../SDK/Client.h"
#include "../SDK/ConVar.h"
#include "../SDK/Entity.h"
#include "../SDK/FrameStage.h"
#include "../SDK/GameEvent.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/ItemSchema.h"
#include "../SDK/Localize.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/Panorama.h"
#include "../SDK/Surface.h"
#include "../SDK/UserCmd.h"
#include "../SDK/WeaponData.h"
#include "../SDK/WeaponSystem.h"
#include "../Hacks/Tickbase.h"

#include "../GUI.h"
#include "../Helpers.h"
#include "../GameData.h"
#include "../Xorstr/xorstr.hpp"
#include "Tickbase.h"

#ifndef RAD2DEG
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / 3.14159265358979323846) )
#endif
#define M_PI_F2		((float)(M_PI2))	

#ifndef M_PI2
#define M_PI2		3.14159265358979323846	
#endif

#ifndef DEG2RAD2
#define DEG2RAD2( x  )  ( (float)(x) * (float)(M_PI_F2 / 180.f) )
#endif                                                               // added both idk why


void Misc::edgejump(UserCmd* cmd) noexcept
{
    if (!config->misc.edgejump || !GetAsyncKeyState(config->misc.edgejumpkey))
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto mt = localPlayer->moveType(); mt == MoveType::LADDER || mt == MoveType::NOCLIP)
        return;

    if ((EnginePrediction::getFlags() & 1) && !(localPlayer->flags() & 1))
        cmd->buttons |= UserCmd::IN_JUMP;
}

void Misc::slowwalk(UserCmd* cmd) noexcept
{
    if (!config->misc.slowwalk || !GetAsyncKeyState(config->misc.slowwalkKey))
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon)
        return;

    const auto weaponData = activeWeapon->getWeaponData();
    if (!weaponData)
        return;

    const float maxSpeed = (localPlayer->isScoped() ? weaponData->maxSpeedAlt : weaponData->maxSpeed) / 3;

    if (cmd->forwardmove && cmd->sidemove) {
        const float maxSpeedRoot = maxSpeed * static_cast<float>(M_SQRT1_2);
        cmd->forwardmove = cmd->forwardmove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
        cmd->sidemove = cmd->sidemove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
    } else if (cmd->forwardmove) {
        cmd->forwardmove = cmd->forwardmove < 0.0f ? -maxSpeed : maxSpeed;
    } else if (cmd->sidemove) {
        cmd->sidemove = cmd->sidemove < 0.0f ? -maxSpeed : maxSpeed;
    }
}

void Misc::inverseRagdollGravity() noexcept
{
    static auto ragdollGravity = interfaces->cvar->findVar("cl_ragdoll_gravity");
    ragdollGravity->setValue(config->visuals.inverseRagdollGravity ? -600 : 600);
}

void Misc::ChatSpammer() noexcept
{
    static uint32_t lastspammed = 0;
    static int lastId = 1;

    if (GetTickCount64() - lastspammed > 800)
    {

        if (config->misc.spam.spam_picker == 1) {
            static int lastId = 1;
            lastspammed = GetTickCount64();
            for (int i = lastId; i < interfaces->engine->getMaxClients(); i++)
            {
                const auto Player = interfaces->entityList->getEntity(i);

                lastId++;
                if (lastId == interfaces->engine->getMaxClients())
                    lastId = 1;
                if (!Player || !Player->isAlive())
                    continue;

                if (config->misc.spam.team == 0 && Player->team() != localPlayer->team())
                    continue;

                if (config->misc.spam.team == 1 && Player->team() == localPlayer->team())
                    continue;

                PlayerInfo entityInformation;
                interfaces->engine->getPlayerInfo(i, entityInformation);
                std::string playerName = std::string(entityInformation.name);
                playerName.erase(std::remove(playerName.begin(), playerName.end(), ';'), playerName.end());
                playerName.erase(std::remove(playerName.begin(), playerName.end(), '"'), playerName.end());
                // Remove end line character
                playerName.erase(std::remove(playerName.begin(), playerName.end(), '\n'), playerName.end());
                // Construct a command with our message
                pstring str;
                str = config->misc.spam.say ? "say_team" : "say";
                str << " \"";
                str << playerName;
                str << " | ";
                str << Player->health() << "HP | ";
                str << Player->lastPlaceName() << " | ";
                str << Player->account() << "$ | ";
                str << "\"";

                interfaces->engine->clientCmdUnrestricted(str.c_str());
                break;
            }
        }
        else if (config->misc.spam.spam_picker == 2) {
            {
                lastspammed = GetTickCount64();
                std::string chatText = "say " + config->misc.spam.text;
                interfaces->engine->clientCmdUnrestricted(chatText.c_str());
            }
        }
    }
}

void Misc::updateClanTag(bool tagChanged) noexcept
{
    static std::string clanTag;

    if (tagChanged) {
        clanTag = config->misc.clanTag;
        if (!clanTag.empty() && clanTag.front() != ' ' && clanTag.back() != ' ')
            clanTag.push_back(' ');
        return;
    }
    
    static auto lastTime = 0.0f;

    if (config->misc.clocktag) {
        if (memory->globalVars->realtime - lastTime < 1.0f)
            return;

        const auto time = std::time(nullptr);
        const auto localTime = std::localtime(&time);
        char s[11];
        s[0] = '\0';
        sprintf_s(s, "[%02d:%02d:%02d]", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
        lastTime = memory->globalVars->realtime;
        memory->setClanTag(s, s);
    } else if (config->misc.customClanTag) {
        if (memory->globalVars->realtime - lastTime < 0.6f)
            return;

        if (config->misc.animatedClanTag && !clanTag.empty()) {
            const auto offset = Helpers::utf8SeqLen(clanTag[0]);
            if (offset != -1)
                std::rotate(clanTag.begin(), clanTag.begin() + offset, clanTag.end());
        }
        lastTime = memory->globalVars->realtime;
        memory->setClanTag(clanTag.c_str(), clanTag.c_str());
    }
    else if (config->misc.cheatSpam) {
        if (memory->globalVars->realtime - lastTime < 0.6f)
            return;

        clanTag = config->misc.cheatName;
        if (config->misc.animatedClanTag && !clanTag.empty()) {
            const auto offset = Helpers::utf8SeqLen(clanTag[0]);

            if (offset != -1)
                std::rotate(clanTag.begin(), clanTag.begin() + offset, clanTag.end());
        }
        lastTime = memory->globalVars->realtime;
        memory->setClanTag(clanTag.c_str(), clanTag.c_str());
    }
}

void Misc::spectatorList() noexcept
{
    if (!config->misc.spectatorList.enabled)
        return;

    GameData::Lock lock;

    const auto& observers = GameData::observers();

    if (std::ranges::none_of(observers, [](const auto& obs) { return obs.targetIsLocalPlayer; }) && !gui->isOpen())
        return;

    if (config->misc.spectatorList.pos != ImVec2{}) {
        ImGui::SetNextWindowPos(config->misc.spectatorList.pos);
        config->misc.spectatorList.pos = {};
    }

    if (config->misc.spectatorList.size != ImVec2{}) {
        ImGui::SetNextWindowSize(ImClamp(config->misc.spectatorList.size, {}, ImGui::GetIO().DisplaySize));
        config->misc.spectatorList.size = {};
    }

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
    if (!gui->isOpen())
        windowFlags |= ImGuiWindowFlags_NoInputs;
    if (config->misc.spectatorList.noTitleBar)
        windowFlags |= ImGuiWindowFlags_NoTitleBar;

    if (!gui->isOpen())
        ImGui::PushStyleColor(ImGuiCol_TitleBg, ImGui::GetColorU32(ImGuiCol_TitleBgActive));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
    ImGui::Begin("Spectator list", nullptr, windowFlags);
    ImGui::PopStyleVar();

    if (!gui->isOpen())
        ImGui::PopStyleColor();

    for (const auto& observer : observers) {
        if (!observer.targetIsLocalPlayer)
            continue;

        if (const auto it = std::ranges::find(GameData::players(), observer.playerHandle, &PlayerData::handle); it != GameData::players().cend()) {
            ImGui::TextWrapped("%s", it->name);
        }
    }

    ImGui::End();
}

static void drawCrosshair(ImDrawList* drawList, const ImVec2& pos, ImU32 color, float thickness) noexcept
{
    drawList->Flags &= ~ImDrawListFlags_AntiAliasedLines;

    // dot
    drawList->AddRectFilled(pos - ImVec2{ 1, 1 }, pos + ImVec2{ 2, 2 }, color & IM_COL32_A_MASK);
    drawList->AddRectFilled(pos, pos + ImVec2{ 1, 1 }, color);

    // left
    drawList->AddRectFilled(ImVec2{ pos.x - 11, pos.y - 1 }, ImVec2{ pos.x - 3, pos.y + 2 }, color & IM_COL32_A_MASK);
    drawList->AddRectFilled(ImVec2{ pos.x - 10, pos.y }, ImVec2{ pos.x - 4, pos.y + 1 }, color);

    // right
    drawList->AddRectFilled(ImVec2{ pos.x + 4, pos.y - 1 }, ImVec2{ pos.x + 12, pos.y + 2 }, color & IM_COL32_A_MASK);
    drawList->AddRectFilled(ImVec2{ pos.x + 5, pos.y }, ImVec2{ pos.x + 11, pos.y + 1 }, color);

    // top (left with swapped x/y offsets)
    drawList->AddRectFilled(ImVec2{ pos.x - 1, pos.y - 11 }, ImVec2{ pos.x + 2, pos.y - 3 }, color & IM_COL32_A_MASK);
    drawList->AddRectFilled(ImVec2{ pos.x, pos.y - 10 }, ImVec2{ pos.x + 1, pos.y - 4 }, color);

    // bottom (right with swapped x/y offsets)
    drawList->AddRectFilled(ImVec2{ pos.x - 1, pos.y + 4 }, ImVec2{ pos.x + 2, pos.y + 12 }, color & IM_COL32_A_MASK);
    drawList->AddRectFilled(ImVec2{ pos.x, pos.y + 5 }, ImVec2{ pos.x + 1, pos.y + 11 }, color);

    drawList->Flags |= ImDrawListFlags_AntiAliasedLines;
}


void Misc::noscopeCrosshair(ImDrawList* drawList) noexcept
{
    if (!config->misc.noscopeCrosshair.enabled)
        return;

    GameData::Lock lock;
    const auto& local = GameData::local();

    if (!local.exists || !local.alive || !local.noScope)
        return;

    drawCrosshair(drawList, ImGui::GetIO().DisplaySize / 2, Helpers::calculateColor(config->misc.noscopeCrosshair), config->misc.noscopeCrosshair.thickness);
}

void Misc::hiddenCvar() noexcept
{
    if (!config->misc.cvarEnabled)
        return;

    static auto cvar = interfaces->cvar->findVar(config->misc.cvar.c_str());
    cvar->setValue(config->misc.cvarValue);

}

static bool worldToScreen(const Vector& in, ImVec2& out) noexcept
{
    const auto& matrix = GameData::toScreenMatrix();

    const auto w = matrix._41 * in.x + matrix._42 * in.y + matrix._43 * in.z + matrix._44;
    if (w < 0.001f)
        return false;

    out = ImGui::GetIO().DisplaySize / 2.0f;
    out.x *= 1.0f + (matrix._11 * in.x + matrix._12 * in.y + matrix._13 * in.z + matrix._14) / w;
    out.y *= 1.0f - (matrix._21 * in.x + matrix._22 * in.y + matrix._23 * in.z + matrix._24) / w;
    out = ImFloor(out);
    return true;
}

void Misc::drawWallbangVector(ImDrawList* dl, Vector &wallbangVector, int wallDmg, int wallChance) noexcept
{
	 if (wallbangVector != Vector{ 0, 0, 0 }) {
        ImVec2 startpos;
        if (worldToScreen(wallbangVector, startpos))
        {
		   dl->AddText(ImGui::GetFont(), 32.0f, startpos, ImColor(1.f, 0.f, 0.f, 1.0f), (std::to_string(wallDmg) + " / " + std::to_string(wallChance) + "%").c_str());
	       dl->AddCircleFilled(startpos, 4, ImColor(1.f, 1.f, 1.f, 0.4f), 32);  
        }
          
    }
}

void Misc::drawStartPos(ImDrawList* dl, Vector &quickpeekstartpos) noexcept {
    if (quickpeekstartpos != Vector{ 0, 0, 0 }) {
        ImVec2 startpos;
        if (worldToScreen(quickpeekstartpos, startpos))
            dl->AddCircleFilled(startpos, 10, ImColor(1.f, 1.f, 1.f, 1.f), 32);
    }
}

void gotoStart(UserCmd* cmd, Vector &quickpeekstartpos) noexcept {

    if (!localPlayer || localPlayer->isDormant() || !localPlayer->isAlive()) return;
    Vector playerLoc = localPlayer->getAbsOrigin();
 
    float yaw = cmd->viewangles.y;
    Vector VecForward = playerLoc - quickpeekstartpos;
 
    Vector translatedVelocity = Vector{
        (float)(VecForward.x * cos(yaw / 180 * (float)M_PI) + VecForward.y * sin(yaw / 180 * (float)M_PI)),
        (float)(VecForward.y * cos(yaw / 180 * (float)M_PI) - VecForward.x * sin(yaw / 180 * (float)M_PI)),
        VecForward.z
    };
	
    cmd->forwardmove = -translatedVelocity.x * 20.f;
    cmd->sidemove = translatedVelocity.y * 20.f;
}




void Misc::recoilCrosshair(ImDrawList* drawList) noexcept
{
    if (!config->misc.recoilCrosshair.enabled)
        return;

    GameData::Lock lock;
    const auto& localPlayerData = GameData::local();

    if (!localPlayerData.exists || !localPlayerData.alive)
        return;

    if (!localPlayerData.shooting)
        return;

    if (ImVec2 pos; worldToScreen(localPlayerData.aimPunch, pos))
        drawCrosshair(drawList, pos, Helpers::calculateColor(config->misc.recoilCrosshair), config->misc.recoilCrosshair.thickness);
}

void Misc::watermark(ImDrawList* drawList) noexcept
{
    if (!&config->misc.watermark)
        return;


    float latency = 0.0f;
    if (auto networkChannel = interfaces->engine->getNetworkChannel(); networkChannel && networkChannel->getLatency(0) > 0.0f)
        latency = networkChannel->getLatency(0);
    
    std::string ping{ std::to_wstring(static_cast<int>(latency * 1000)) + L" ms" };
    const auto tick = 1.f / memory->globalVars->intervalPerTick;
    //FPS
    static auto fps = 1.0f;
    fps = 0.9f * fps + 0.1f * memory->globalVars->absoluteFrameTime;

    //TIME
    std::time_t t = std::time(nullptr);
    std::ostringstream time;
    time << std::put_time(std::localtime(&t), ("%H:%M:%S"));

    std::ostringstream format;
    format << "CS Cheat"
        << " | " << (fps != 0.0f ? static_cast<int>(1 / fps) : 0) << " fps";

    if (interfaces->engine->isConnected()) {
        format << " | local " << tick << " tick";
    }
    else if (interfaces->engine->isInGame()) {
        auto* pInfo = interfaces->engine->getNetworkChannel();
        if (pInfo) 
            format << " | " << ping << " ms " << tick << " tick";
    }
    else if (interfaces->engine->isConnected())
        format << " | loading";

    format << " | " << time.str().c_str();

    const auto textSize = ImGui::CalcTextSize(format.str().c_str());
    const auto displaySize = ImGui::GetIO().DisplaySize;
    
    ImRect window{
        displaySize.x - textSize.x - 9.f,
        1.f,
        displaySize.x - 1.f,
        textSize.y + 9.f
    };
    
    drawList->AddRectFilled(window.Min, window.Max, ImGui::GetColorU32(ImGuiCol_WindowBg), 4);
    const int vertStartIdx = drawList->VtxBuffer.Size;
    drawList->AddRect(window.Min, window.Max, ImGui::GetColorU32(ImGuiCol_TitleBgActive), 4);
    const int vertEndIdx = drawList->VtxBuffer.Size;

    float r, g, b;
    std::tie(r, g, b) = rainbowColor(3.f);
    shadeVertsHSVColorGradientKeepAlpha(drawList, vertStartIdx, vertEndIdx, window.GetTL(), window.GetBR(), ImColor(r, g, b, 1.f), ImGui::GetColorU32(ImGuiCol_TitleBgActive));

    ImVec2 textPos{
        window.GetCenter().x - (textSize.x / 2),
        window.GetCenter().y - (textSize.y / 2)
    };
    drawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), format.str().c_str());
}

static void shadeVertsHSVColorGradientKeepAlpha(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1)
{
    ImVec2 gradient_extent = gradient_p1 - gradient_p0;
    float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
    ImDrawVert* vert_start = draw_list->VtxBuffer.Data + vert_start_idx;
    ImDrawVert* vert_end = draw_list->VtxBuffer.Data + vert_end_idx;

    ImVec4 col0HSV = ImGui::ColorConvertU32ToFloat4(col0);
    ImVec4 col1HSV = ImGui::ColorConvertU32ToFloat4(col1);
    ImGui::ColorConvertRGBtoHSV(col0HSV.x, col0HSV.y, col0HSV.z, col0HSV.x, col0HSV.y, col0HSV.z);
    ImGui::ColorConvertRGBtoHSV(col1HSV.x, col1HSV.y, col1HSV.z, col1HSV.x, col1HSV.y, col1HSV.z);
    ImVec4 colDelta = col1HSV - col0HSV;

    for (ImDrawVert* vert = vert_start; vert < vert_end; vert++)
    {
        float d = ImDot(vert->pos - gradient_p0, gradient_extent);
        float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);

        float h = col0HSV.x + colDelta.x * t;
        float s = col0HSV.y + colDelta.y * t;
        float v = col0HSV.z + colDelta.z * t;

        ImVec4 rgb;
        ImGui::ColorConvertHSVtoRGB(h, s, v, rgb.x, rgb.y, rgb.z);
        vert->col = (ImGui::ColorConvertFloat4ToU32(rgb) & ~IM_COL32_A_MASK) | (vert->col & IM_COL32_A_MASK);
    }
}

void Misc::prepareRevolver(UserCmd* cmd) noexcept
{
    constexpr auto timeToTicks = [](float time) {  return static_cast<int>(0.5f + time / memory->globalVars->intervalPerTick); };
    constexpr float revolverPrepareTime{ 0.234375f };

    static float readyTime;
    if (config->misc.prepareRevolver && localPlayer && (!config->misc.prepareRevolverKey || GetAsyncKeyState(config->misc.prepareRevolverKey))) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver) {
            if (!readyTime) readyTime = memory->globalVars->serverTime() + revolverPrepareTime;
            auto ticksToReady = timeToTicks(readyTime - memory->globalVars->serverTime() - interfaces->engine->getNetworkChannel()->getLatency(0));
            if (ticksToReady > 0 && ticksToReady <= timeToTicks(revolverPrepareTime))
                cmd->buttons |= UserCmd::IN_ATTACK;
            else
                readyTime = 0.0f;
        }
    }
}

void Misc::fastPlant(UserCmd* cmd) noexcept
{
    if (!config->misc.fastPlant)
        return;

    static auto plantAnywhere = interfaces->cvar->findVar("mp_plant_c4_anywhere");

    if (plantAnywhere->getInt())
        return;

    if (!localPlayer || !localPlayer->isAlive() || localPlayer->inBombZone() && localPlayer->flags() & 1)
        return;

    const auto activeWeapon = localPlayer->getActiveWeapon();
    if (!activeWeapon || activeWeapon->getClientClass()->classId != ClassId::C4)
        return;

    cmd->buttons &= ~UserCmd::IN_ATTACK;

    constexpr auto doorRange = 200.0f;

    Trace trace;
    const auto startPos = localPlayer->getEyePosition();
    const auto endPos = startPos + Vector::fromAngle(cmd->viewangles) * doorRange;
    interfaces->engineTrace->traceRay({ startPos, endPos }, 0x46004009, localPlayer.get(), trace);

    if (!trace.entity || trace.entity->getClientClass()->classId != ClassId::PropDoorRotating)
        cmd->buttons &= ~UserCmd::IN_USE;
}

void Misc::fastStop(UserCmd* cmd) noexcept
{
    if (!config->misc.fastStop)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (localPlayer->moveType() == MoveType::NOCLIP || localPlayer->moveType() == MoveType::LADDER || !(localPlayer->flags() & 1) || cmd->buttons & UserCmd::IN_JUMP)
        return;

    if (cmd->buttons & (UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT | UserCmd::IN_FORWARD | UserCmd::IN_BACK))
        return;
    
    const auto velocity = localPlayer->velocity();
    const auto speed = velocity.length2D();
    if (speed < 15.0f)
        return;
    
    Vector direction = velocity.toAngle();
    direction.y = cmd->viewangles.y - direction.y;

    const auto negatedDirection = Vector::fromAngle(direction) * -speed;
    cmd->forwardmove = negatedDirection.x;
    cmd->sidemove = negatedDirection.y;
}

void Misc::drawBombTimer() noexcept
{
    if (config->misc.bombTimer.enabled) {
        for (int i = interfaces->engine->getMaxClients(); i <= interfaces->entityList->getHighestEntityIndex(); i++) {
            Entity* entity = interfaces->entityList->getEntity(i);
            if (!entity || entity->isDormant() || entity->getClientClass()->classId != ClassId::PlantedC4 || !entity->c4Ticking())
                continue;

            constexpr unsigned font{ 0xc1 };
            interfaces->surface->setTextFont(font);
            interfaces->surface->setTextColor(255, 255, 255);
            auto drawPositionY{ interfaces->surface->getScreenSize().second / 8 };
            auto bombText{ (std::wstringstream{ } << L"Bomb on " << (!entity->c4BombSite() ? 'A' : 'B') << L" : " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(entity->c4BlowTime() - memory->globalVars->currenttime, 0.0f) << L" s").str() };
            const auto bombTextX{ interfaces->surface->getScreenSize().first / 2 - static_cast<int>((interfaces->surface->getTextSize(font, bombText.c_str())).first / 2) };
            interfaces->surface->setTextPosition(bombTextX, drawPositionY);
            drawPositionY += interfaces->surface->getTextSize(font, bombText.c_str()).second;
            interfaces->surface->printText(bombText.c_str());

            const auto progressBarX{ interfaces->surface->getScreenSize().first / 3 };
            const auto progressBarLength{ interfaces->surface->getScreenSize().first / 3 };
            constexpr auto progressBarHeight{ 5 };

            interfaces->surface->setDrawColor(50, 50, 50);
            interfaces->surface->drawFilledRect(progressBarX - 3, drawPositionY + 2, progressBarX + progressBarLength + 3, drawPositionY + progressBarHeight + 8);
            if (config->misc.bombTimer.rainbow)
                interfaces->surface->setDrawColor(rainbowColor(config->misc.bombTimer.rainbowSpeed));
            else
                interfaces->surface->setDrawColor(config->misc.bombTimer.color);

            static auto c4Timer = interfaces->cvar->findVar("mp_c4timer");

            interfaces->surface->drawFilledRect(progressBarX, drawPositionY + 5, static_cast<int>(progressBarX + progressBarLength * std::clamp(entity->c4BlowTime() - memory->globalVars->currenttime, 0.0f, c4Timer->getFloat()) / c4Timer->getFloat()), drawPositionY + progressBarHeight + 5);

            if (entity->c4Defuser() != -1) {
                if (PlayerInfo playerInfo; interfaces->engine->getPlayerInfo(interfaces->entityList->getEntityFromHandle(entity->c4Defuser())->index(), playerInfo)) {
                    if (wchar_t name[128];  MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                        drawPositionY += interfaces->surface->getTextSize(font, L" ").second;
                        const auto defusingText{ (std::wstringstream{ } << name << L" is defusing: " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(entity->c4DefuseCountDown() - memory->globalVars->currenttime, 0.0f) << L" s").str() };

                        interfaces->surface->setTextPosition((interfaces->surface->getScreenSize().first - interfaces->surface->getTextSize(font, defusingText.c_str()).first) / 2, drawPositionY);
                        interfaces->surface->printText(defusingText.c_str());
                        drawPositionY += interfaces->surface->getTextSize(font, L" ").second;

                        interfaces->surface->setDrawColor(50, 50, 50);
                        interfaces->surface->drawFilledRect(progressBarX - 3, drawPositionY + 2, progressBarX + progressBarLength + 3, drawPositionY + progressBarHeight + 8);
                        interfaces->surface->setDrawColor(0, 255, 0);
                        interfaces->surface->drawFilledRect(progressBarX, drawPositionY + 5, progressBarX + static_cast<int>(progressBarLength * (std::max)(entity->c4DefuseCountDown() - memory->globalVars->currenttime, 0.0f) / (interfaces->entityList->getEntityFromHandle(entity->c4Defuser())->hasDefuser() ? 5.0f : 10.0f)), drawPositionY + progressBarHeight + 5);

                        drawPositionY += interfaces->surface->getTextSize(font, L" ").second;
                        const wchar_t* canDefuseText;

                        if (entity->c4BlowTime() >= entity->c4DefuseCountDown()) {
                            canDefuseText = L"Can Defuse";
                            interfaces->surface->setTextColor(0, 255, 0);
                        } else {
                            canDefuseText = L"Cannot Defuse";
                            interfaces->surface->setTextColor(255, 0, 0);
                        }

                        interfaces->surface->setTextPosition((interfaces->surface->getScreenSize().first - interfaces->surface->getTextSize(font, canDefuseText).first) / 2, drawPositionY);
                        interfaces->surface->printText(canDefuseText);
                    }
                }
            }
            break;
        }
    }
}

void Misc::drawFov(ImDrawList* drawList) noexcept
{
    if (!localPlayer || !localPlayer->isAlive())
        return;

    auto local = localPlayer.get();
    if (!local)
        return;
    auto active = localPlayer->getActiveWeapon();

    if (!active)
        return;

    auto itemIndex = active->itemDefinitionIndex2();

    if ((int)itemIndex < 0 && (int)itemIndex > 10000)
        return;

    int weaponId = getWeaponIndex(itemIndex);

    if (!config->aimbot[weaponId].enabled) weaponId = 0;
    if (config->aimbot[weaponId].drawFov)
    {
        if (!config->aimbot[weaponId].enabled) return;
        auto [width, heigth] = interfaces->surface->getScreenSize();
        if (config->aimbot[weaponId].silent)
            interfaces->surface->setDrawColor(255, 10, 10, 255);
        else interfaces->surface->setDrawColor(10, 255, 10, 255);
        float radius = std::tan(degreesToRadians(config->aimbot[weaponId].fov / 2.f)) / std::tan(degreesToRadians(config->globals.currentFOV / 2.f)) * width;
        drawList->AddCircle(ImGui::GetIO().DisplaySize / 2, radius, ImColor(1.f, 1.f, 1.f, 1.f), 32);
    }

}

void Misc::stealNames() noexcept
{
    if (!config->misc.nameStealer)
        return;

    if (!localPlayer)
        return;

    static std::vector<int> stolenIds;

    for (int i = 1; i <= memory->globalVars->maxClients; ++i) {
        const auto entity = interfaces->entityList->getEntity(i);

        if (!entity || entity == localPlayer.get())
            continue;

        PlayerInfo playerInfo;
        if (!interfaces->engine->getPlayerInfo(entity->index(), playerInfo))
            continue;

        if (playerInfo.fakeplayer || std::find(stolenIds.cbegin(), stolenIds.cend(), playerInfo.userId) != stolenIds.cend())
            continue;

        if (changeName(false, (std::string{ playerInfo.name } +'\x1').c_str(), 1.0f))
            stolenIds.push_back(playerInfo.userId);

        return;
    }
    stolenIds.clear();
}

void Misc::disablePanoramablur() noexcept
{
    static auto blur = interfaces->cvar->findVar("@panorama_disable_blur");
    blur->setValue(config->misc.disablePanoramablur);
}

void Misc::quickReload(UserCmd* cmd) noexcept
{
    if (config->misc.quickReload) {
        static Entity* reloadedWeapon{ nullptr };

        if (reloadedWeapon) {
            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (interfaces->entityList->getEntityFromHandle(weaponHandle) == reloadedWeapon) {
                    cmd->weaponselect = reloadedWeapon->index();
                    cmd->weaponsubtype = reloadedWeapon->getWeaponSubType();
                    break;
                }
            }
            reloadedWeapon = nullptr;
        }

        if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && activeWeapon->isInReload() && activeWeapon->clip() == activeWeapon->getWeaponData()->maxClip) {
            reloadedWeapon = activeWeapon;

            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (auto weapon{ interfaces->entityList->getEntityFromHandle(weaponHandle) }; weapon && weapon != reloadedWeapon) {
                    cmd->weaponselect = weapon->index();
                    cmd->weaponsubtype = weapon->getWeaponSubType();
                    break;
                }
            }
        }
    }
}

bool Misc::changeName(bool reconnect, const char* newName, float delay) noexcept
{
    static auto exploitInitialized{ false };

    static auto name{ interfaces->cvar->findVar("name") };

    if (reconnect) {
        exploitInitialized = false;
        return false;
    }

    if (!exploitInitialized && interfaces->engine->isInGame()) {
        if (PlayerInfo playerInfo; localPlayer && interfaces->engine->getPlayerInfo(localPlayer->index(), playerInfo) && (!strcmp(playerInfo.name, "?empty") || !strcmp(playerInfo.name, "\n\xAD\xAD\xAD"))) {
            exploitInitialized = true;
        } else {
            name->onChangeCallbacks.size = 0;
            name->setValue("\n\xAD\xAD\xAD");
            return false;
        }
    }

    static auto nextChangeTime{ 0.0f };
    if (nextChangeTime <= memory->globalVars->realtime) {
        name->setValue(newName);
        nextChangeTime = memory->globalVars->realtime + delay;
        return true;
    }
    return false;
}

void Misc::bunnyHop(UserCmd* cmd) noexcept
{
    if (!localPlayer)
        return;

    static auto wasLastTimeOnGround{ localPlayer->flags() & 1 };

    if (config->misc.bunnyHop && !(localPlayer->flags() & 1) && localPlayer->moveType() != MoveType::LADDER && !wasLastTimeOnGround)
        cmd->buttons &= ~UserCmd::IN_JUMP;

    wasLastTimeOnGround = localPlayer->flags() & 1;
}

void Misc::fakeBan(bool set) noexcept
{
    static bool shouldSet = false;

    if (set)
        shouldSet = set;

    if (shouldSet && interfaces->engine->isInGame() && changeName(false, std::string{ " \x1\xB" }.append(std::string{ static_cast<char>(config->misc.banColor + 1) }).append(config->misc.banText).append("\n\x1").c_str(), 5.0f))
        shouldSet = false;
}

void Misc::setName(bool set) noexcept
{
    static bool shouldSet = false;
    static bool shouldResetOrigName = false;

    if (set)
        shouldSet = set;

    if (shouldSet && changeName(false, std::string{ "" }.append(config->misc.customName).c_str(), 5.0f) && !(config->misc.customName.c_str() == NULL))
    {
        shouldSet = false;
        shouldResetOrigName = true;
    }
}

void Misc::fakeItem(bool set) noexcept
{
    static auto name{ interfaces->cvar->findVar("name") };
    static auto disconnect{ interfaces->cvar->findVar("disconnect") };

    static int shouldSet = 0;

    std::string playercolor;
    std::string color;
    std::string team;
    std::string star;
    std::string stattrak;
    std::string skinName;
    std::string item;

    if (set)
        if (shouldSet == 0)
            shouldSet = 1;

    if (shouldSet == 1)
    {
        if (config->misc.fakeItemRarity == 0)
            color = "\x08"; // Consumer Grade(White)
        else if (config->misc.fakeItemRarity == 1)
            color = "\x0D"; // Industrial Grade(Light blue)
        else if (config->misc.fakeItemRarity == 2)
            color = "\x0B"; // Mil-Spec(Blue)
        else if (config->misc.fakeItemRarity == 3)
            color = "\x03"; // Restricted(Purple)
        else if (config->misc.fakeItemRarity == 4)
            color = "\x0E"; // Classified(Pink)
        else if (config->misc.fakeItemRarity == 5)
            color = "\x02"; // Covert(Red)
        else if (config->misc.fakeItemRarity == 6)
            color = "\x10"; // Contrabanned(Orange / Gold)

        if (config->misc.fakeItemTeam == 1)
            team = "\x09";
        else
            team = "\x0B";

        if (config->misc.selectedFakeItemFlags[3])
            star = "★ ";
        else
            star = "";

        if (config->misc.selectedFakeItemFlags[2])
            stattrak = "StatTrak™ ";
        else
            stattrak = "";

        if (!config->misc.fakeItemName.empty())
            skinName.append(" | ").append(config->misc.fakeItemName);
        else
            skinName = "";

        if (config->misc.fakeItemType == 0)
            item = "AK-47";
        else if (config->misc.fakeItemType == 1)
            item = "AUG";
        else if (config->misc.fakeItemType == 2)
            item = "AWP";
        else if (config->misc.fakeItemType == 3)
            item = "Bayonet";
        else if (config->misc.fakeItemType == 4)
            item = "Bowie Knife";
        else if (config->misc.fakeItemType == 5)
            item = "Butterfly Knife";
        else if (config->misc.fakeItemType == 6)
            item = "CZ75";
        else if (config->misc.fakeItemType == 7)
            item = "Classic Knife";
        else if (config->misc.fakeItemType == 8)
            item = "Desert Eagle";
        else if (config->misc.fakeItemType == 9)
            item = "Dual Berettas";
        else if (config->misc.fakeItemType == 10)
            item = "FAMAS";
        else if (config->misc.fakeItemType == 11)
            item = "Falchion Knife";
        else if (config->misc.fakeItemType == 12)
            item = "FiveSeveN";
        else if (config->misc.fakeItemType == 13)
            item = "Flip Knife";
        else if (config->misc.fakeItemType == 14)
            item = "G3SG1";
        else if (config->misc.fakeItemType == 15)
            item = "Galil AR";
        else if (config->misc.fakeItemType == 16)
            item = "Glock-18";
        else if (config->misc.fakeItemType == 17)
            item = "Gut Knife";
        else if (config->misc.fakeItemType == 18)
            item = "Huntsman Knife";
        else if (config->misc.fakeItemType == 19)
            item = "Karambit";
        else if (config->misc.fakeItemType == 20)
            item = "M249";
        else if (config->misc.fakeItemType == 21)
            item = "M4A1-S";
        else if (config->misc.fakeItemType == 22)
            item = "M4A4";
        else if (config->misc.fakeItemType == 23)
            item = "M9 Bayonet";
        else if (config->misc.fakeItemType == 24)
            item = "MAC-10";
        else if (config->misc.fakeItemType == 25)
            item = "MAG-7";
        else if (config->misc.fakeItemType == 26)
            item = "MP5-SD";
        else if (config->misc.fakeItemType == 27)
            item = "MP7";
        else if (config->misc.fakeItemType == 28)
            item = "MP9";
        else if (config->misc.fakeItemType == 29)
            item = "Navaja Knife";
        else if (config->misc.fakeItemType == 30)
            item = "Negev";
        else if (config->misc.fakeItemType == 31)
            item = "Nomad Knife";
        else if (config->misc.fakeItemType == 32)
            item = "P2000";
        else if (config->misc.fakeItemType == 33)
            item = "P250";
        else if (config->misc.fakeItemType == 34)
            item = "P90";
        else if (config->misc.fakeItemType == 35)
            item = "PP-Bizon";
        else if (config->misc.fakeItemType == 36)
            item = "Paracord Knife";
        else if (config->misc.fakeItemType == 37)
            item = "SCAR-20";
        else if (config->misc.fakeItemType == 38)
            item = "SG 553";
        else if (config->misc.fakeItemType == 39)
            item = "Sawed-Off";
        else if (config->misc.fakeItemType == 40)
            item = "Shadow Daggers";
        else if (config->misc.fakeItemType == 41)
            item = "Skeleton Knife";
        else if (config->misc.fakeItemType == 42)
            item = "Stiletto Knife";
        else if (config->misc.fakeItemType == 43)
            item = "Survival Knife";
        else if (config->misc.fakeItemType == 44)
            item = "Talon Knife";
        else if (config->misc.fakeItemType == 45)
            item = "Tec-9";
        else if (config->misc.fakeItemType == 46)
            item = "UMP-45";
        else if (config->misc.fakeItemType == 47)
            item = "USP-S";
        else if (config->misc.fakeItemType == 48)
            item = "Ursus Knife";
        else if (config->misc.fakeItemType == 49)
            item = "XM1014";
        else if (config->misc.fakeItemType == 50)
            item = "Hand Wraps";
        else if (config->misc.fakeItemType == 51)
            item = "Moto Gloves";
        else if (config->misc.fakeItemType == 52)
            item = "Specialist Gloves";
        else if (config->misc.fakeItemType == 53)
            item = "Sport Gloves";
        else if (config->misc.fakeItemType == 54)
            item = "Bloodhound Gloves";
        else if (config->misc.fakeItemType == 55)
            item = "Hydra Gloves";
        else if (config->misc.fakeItemType == 56)
            item = "Driver Gloves";

        if (config->misc.fakeItemPlayerColor == 0)
            playercolor = "\x09"; // Yellow
        else if (config->misc.fakeItemPlayerColor == 1)
            playercolor = "\x04"; // Green
        else if (config->misc.fakeItemPlayerColor == 2)
            playercolor = "\x0D"; // Blue
        else if (config->misc.fakeItemPlayerColor == 3)
            playercolor = "\x03"; // Purple
        else if (config->misc.fakeItemPlayerColor == 4)
            playercolor = "\x10"; // Orange

        if (config->misc.fakeItemMessageType == 0)
        { 
            if (interfaces->engine->isInGame() && changeName(false, std::string{ "\n \x1\xB" }.append(playercolor).append("• • ").append(team).append(config->misc.fakeItemPlayerName).append("\x01 has opened a container and found: \x1\xB").append(color).append(star).append(stattrak).append(item).append(skinName).append("\n ").append("\x1").c_str(), 5.0f))
                shouldSet = 2;
        }
        else
        {
            if (interfaces->engine->isInGame() && changeName(false, std::string{ "\n \x1\xB" }.append(playercolor).append("• • ").append(team).append(config->misc.fakeItemPlayerName).append("\x01 has recieved in trade: \x1\xB").append(color).append(star).append(stattrak).append(item).append(skinName).append("\n ").append("\x1").c_str(), 5.0f))
                shouldSet = 2;
        }
    }
    if (shouldSet == 2)
    {
        if (config->misc.selectedFakeItemFlags[1])
            disconnect->setValue(1);
        shouldSet = 0;
    }

}

void Misc::nadePredict() noexcept
{
    static auto nadeVar{ interfaces->cvar->findVar("cl_grenadepreview") };

    nadeVar->onChangeCallbacks.size = 0;
    nadeVar->setValue(config->misc.nadePredict);
}

void Misc::quickHealthshot(UserCmd* cmd) noexcept
{
    if (!localPlayer)
        return;

    static bool inProgress{ false };

    if (GetAsyncKeyState(config->misc.quickHealthshotKey) & 1)
        inProgress = true;

    if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && inProgress) {
        if (activeWeapon->getClientClass()->classId == ClassId::Healthshot && localPlayer->nextAttack() <= memory->globalVars->serverTime() && activeWeapon->nextPrimaryAttack() <= memory->globalVars->serverTime())
            cmd->buttons |= UserCmd::IN_ATTACK;
        else {
            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (const auto weapon{ interfaces->entityList->getEntityFromHandle(weaponHandle) }; weapon && weapon->getClientClass()->classId == ClassId::Healthshot) {
                    cmd->weaponselect = weapon->index();
                    cmd->weaponsubtype = weapon->getWeaponSubType();
                    return;
                }
            }
        }
        inProgress = false;
    }
}

void Misc::fixTabletSignal() noexcept
{
    if (config->misc.fixTabletSignal && localPlayer) {
        if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && activeWeapon->getClientClass()->classId == ClassId::Tablet)
            activeWeapon->tabletReceptionIsBlocked() = false;
    }
}

void Misc::drawBombDamage() noexcept
{
    if (!config->misc.bombDamage) return;

    if (!localPlayer) return;

    for (int i = interfaces->engine->getMaxClients(); i <= interfaces->entityList->getHighestEntityIndex(); i++)
    {
        auto entity = interfaces->entityList->getEntity(i);
        if (!entity || entity->isDormant() || entity->getClientClass()->classId != ClassId::PlantedC4 || !entity->
            c4Ticking())
            continue;

        auto vecBombDistance = entity->origin() - localPlayer->origin();

        const auto d = (vecBombDistance.length() - 75.68f) / 789.2f;
        auto flDamage = 450.7f * exp(-d * d);

        const float ArmorValue = localPlayer->armor();
        if (ArmorValue > 0)
        {
            auto flNew = flDamage * 0.5f;
            auto flArmor = (flDamage - flNew) * 0.5f;

            if (flArmor > ArmorValue)
            {
                flArmor = ArmorValue * 2.f;
                flNew = flDamage - flArmor;
            }

            flDamage = flNew;
        }

        const int bombDamage = max(ceilf(flDamage), 0);

        //Could get the specator target here as well and set the color based on the spaceted player
        //I'm too lazy for that tho, green while you are dead just looks nicer
        if (localPlayer->isAlive() && bombDamage >= localPlayer->health())
            interfaces->surface->setTextColor(255, 0, 0);
        else
            interfaces->surface->setTextColor(0, 255, 0);

        auto bombDmgText{ (std::wstringstream{} << L"Bomb Damage: " << bombDamage).str() };

        constexpr unsigned font{ 0xc1 };
        interfaces->surface->setTextFont(font);

        auto drawPositionY{ interfaces->surface->getScreenSize().second / 8 };
        const auto bombDmgX{
            interfaces->surface->getScreenSize().first / 2 - static_cast<int>((interfaces->surface->getTextSize(font, bombDmgText.c_str())).first / 2)
        };

        drawPositionY -= interfaces->surface->getTextSize(font, bombDmgText.c_str()).second;

        interfaces->surface->setTextPosition(bombDmgX, drawPositionY);
        interfaces->surface->printText(bombDmgText.c_str());
    }
}

void Misc::killMessage(GameEvent& event) noexcept
{
    if (!config->misc.killMessage)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    std::string cmd = "say \"";
    cmd += config->misc.killMessageString;
    cmd += '"';
    interfaces->engine->clientCmdUnrestricted(cmd.c_str());
}

void Misc::fixMovement(UserCmd* cmd, float yaw) noexcept
{
    if (config->misc.fixMovement) {
        float oldYaw = yaw + (yaw < 0.0f ? 360.0f : 0.0f);
        float newYaw = cmd->viewangles.y + (cmd->viewangles.y < 0.0f ? 360.0f : 0.0f);
        float yawDelta = newYaw < oldYaw ? fabsf(newYaw - oldYaw) : 360.0f - fabsf(newYaw - oldYaw);
        yawDelta = 360.0f - yawDelta;

        const float forwardmove = cmd->forwardmove;
        const float sidemove = cmd->sidemove;
        cmd->forwardmove = std::cos(degreesToRadians(yawDelta)) * forwardmove + std::cos(degreesToRadians(yawDelta + 90.0f)) * sidemove;
        cmd->sidemove = std::sin(degreesToRadians(yawDelta)) * forwardmove + std::sin(degreesToRadians(yawDelta + 90.0f)) * sidemove;
    }
}

void Misc::antiAfkKick(UserCmd* cmd) noexcept
{
    if (config->misc.antiAfkKick && cmd->commandNumber % 2)
        cmd->buttons |= 1 << 26;
}

void Misc::fixAnimationLOD(FrameStage stage) noexcept
{
    if (config->misc.fixAnimationLOD && stage == FrameStage::RENDER_START) {
        if (!localPlayer)
            return;

        for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
            Entity* entity = interfaces->entityList->getEntity(i);
            if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive()) continue;
            *reinterpret_cast<int*>(entity + 0xA28) = 0;
            *reinterpret_cast<int*>(entity + 0xA30) = memory->globalVars->framecount;
        }
    }
}

void Misc::autoPistol(UserCmd* cmd) noexcept
{
    if (config->misc.autoPistol && localPlayer) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && activeWeapon->isPistol() && activeWeapon->nextPrimaryAttack() > memory->globalVars->serverTime()) {
            if (activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver)
                cmd->buttons &= ~UserCmd::IN_ATTACK2;
            else
                cmd->buttons &= ~UserCmd::IN_ATTACK;
        }
    }
}

float Misc::RandomFloat(float min, float max) noexcept
{
	return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
}

void Misc::chokePackets(bool& sendPacket, UserCmd* cmd) noexcept
{
    bool doFakeLag{ false };
    auto position = localPlayer->getAbsOrigin();
    auto velocity = localPlayer->velocity();
    auto extrapolatedVelocity = sqrt(sqrt(velocity.x * velocity.y * velocity.z));
    auto& records = config->globals.serverPos;
    float distanceDifToServerSide = sqrt(sqrt(records.origin.x * records.origin.y * records.origin.z));

    if (config->misc.fakeLagMode != 0)
    {
        if ((config->misc.fakeLagSelectedFlags[0] && cmd->buttons & (UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2))
            || (config->misc.fakeLagSelectedFlags[1] && !(cmd->buttons& (UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT)))
            || (config->misc.fakeLagSelectedFlags[2] && cmd->buttons & (UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT))
            || (config->misc.fakeLagSelectedFlags[3] && !(localPlayer->flags() & 1)))
            doFakeLag = true;
        else
            doFakeLag = false;
    }

    if (doFakeLag)
    {
        if (config->misc.fakeLagMode == 1)
            sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= config->misc.fakeLagTicks;
        if (config->misc.fakeLagMode == 2)
        {
            auto requiredPacketsToBreakLagComp = 65 / extrapolatedVelocity;
            if (!(requiredPacketsToBreakLagComp > config->misc.fakeLagTicks) && requiredPacketsToBreakLagComp <= 16)
                sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= requiredPacketsToBreakLagComp;
            else
                sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= 16;
        }
        if (config->misc.fakeLagMode == 3)
        {
            float lagTicks = RandomFloat(config->misc.fakeLagTicks, 16);
            sendPacket = interfaces->engine->getNetworkChannel()->chokedPackets >= lagTicks;
        }
        if (config->misc.fakeLagMode == 4)
        {
            if (distanceDifToServerSide < 64.f && interfaces->engine->getNetworkChannel()->chokedPackets < 16)
                sendPacket = false;
            else
                sendPacket = true;
        }
    }
}

void Misc::fakeDuck(UserCmd* cmd, bool& sendPacket) noexcept
{
    if (config->misc.fakeDuck)
    {
        if (config->misc.fakeDuckKey != 0) {
            if (!GetAsyncKeyState(config->misc.fakeDuckKey))
            {
                config->misc.fakeDucking = false;
                return;
            }
            else
                config->misc.fakeDucking = true;
        }

        if (config->misc.fakeDucking)
        {
            static bool counter = false;
            static int counters = 0;
            if (counters == 9)
            {
                counters = 0;
                counter = !counter;
            }
            counters++;
            if (counter)
            {
                cmd->buttons |= UserCmd::IN_DUCK;
                sendPacket = true;
            }
            else
            {
                sendPacket = false;
                cmd->buttons &= ~UserCmd::IN_DUCK;
            }
        }
    }
}

void Misc::autoReload(UserCmd* cmd) noexcept
{
    if (config->misc.autoReload && localPlayer) {
        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (activeWeapon && getWeaponIndex(activeWeapon->itemDefinitionIndex2()) && !activeWeapon->clip())
            cmd->buttons &= ~(UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2);
    }
}

void Misc::revealRanks(UserCmd* cmd) noexcept
{
    if (config->misc.revealRanks && cmd->buttons & UserCmd::IN_SCORE)
        interfaces->client->dispatchUserMessage(50, 0, 0, nullptr);
}

void Misc::autoStrafe(UserCmd* cmd) noexcept
{
    if (localPlayer
        && config->misc.autoStrafe
        && !(localPlayer->flags() & 1)
        && localPlayer->moveType() != MoveType::NOCLIP) {
        if (cmd->mousedx < 0)
            cmd->sidemove = -450.0f;
        else if (cmd->mousedx > 0)
            cmd->sidemove = 450.0f;
    }
}

void Misc::removeCrouchCooldown(UserCmd* cmd) noexcept
{
    if (config->misc.fastDuck)
        cmd->buttons |= UserCmd::IN_BULLRUSH;
}

void Misc::moonwalk(UserCmd* cmd) noexcept
{
    if (config->misc.moonwalk && localPlayer && localPlayer->moveType() != MoveType::LADDER)
        cmd->buttons ^= UserCmd::IN_FORWARD | UserCmd::IN_BACK | UserCmd::IN_MOVELEFT | UserCmd::IN_MOVERIGHT;
}

void Misc::ragdollForce() noexcept
{
    if (!config->misc.ragdollForce)
        return;

    for (int i = 1; i <= interfaces->entityList->getHighestEntityIndex(); i++) {
        Entity* entity = interfaces->entityList->getEntity(i);

        if (!entity || !(entity->getClientClass()->classId == ClassId::CSRagdoll))
            continue;

        entity->ragdollForce() *= (float)config->misc.ragdollForceStrength;
        entity->ragdollVelocity() *= (float)config->misc.ragdollForceStrength;
    }
}

void Misc::playHitSound(GameEvent& event) noexcept
{
    if (!config->misc.hitSound)
        return;

    if (!localPlayer)
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    constexpr std::array hitSounds{
        "play physics/metal/metal_solid_impact_bullet2",
        "play buttons/arena_switch_press_02",
        "play training/timer_bell",
        "play physics/glass/glass_impact_bullet1"
    };

    if (static_cast<std::size_t>(config->misc.hitSound - 1) < hitSounds.size())
        interfaces->engine->clientCmdUnrestricted(hitSounds[config->misc.hitSound - 1]);
    else if (config->misc.hitSound == 5)
        interfaces->engine->clientCmdUnrestricted(("play " + config->misc.customHitSound).c_str());
}




void Misc::killSound(GameEvent& event) noexcept
{
    if (!config->misc.killSound)
        return;

    if (!localPlayer || !localPlayer->isAlive())
        return;

    if (const auto localUserId = localPlayer->getUserId(); event.getInt("attacker") != localUserId || event.getInt("userid") == localUserId)
        return;

    constexpr std::array killSounds{
        "play physics/metal/metal_solid_impact_bullet2",
        "play buttons/arena_switch_press_02",
        "play training/timer_bell",
        "play physics/glass/glass_impact_bullet1"
    };

    if (static_cast<std::size_t>(config->misc.killSound - 1) < killSounds.size())
        interfaces->engine->clientCmdUnrestricted(killSounds[config->misc.killSound - 1]);
    else if (config->misc.killSound == 5)
        interfaces->engine->clientCmdUnrestricted(("play " + config->misc.customKillSound).c_str());
}

void Misc::purchaseList(GameEvent* event) noexcept
{
    static std::mutex mtx;
    std::scoped_lock _{ mtx };

    static std::unordered_map<std::string, std::pair<std::vector<std::string>, int>> purchaseDetails;
    static std::unordered_map<std::string, int> purchaseTotal;
    static int totalCost;

    static auto freezeEnd = 0.0f;

    if (event) {
        switch (fnv::hashRuntime(event->getName())) {
        case fnv::hash("item_purchase"): {
            const auto player = interfaces->entityList->getEntity(interfaces->engine->getPlayerForUserID(event->getInt("userid")));

            if (player && localPlayer && memory->isOtherEnemy(player, localPlayer.get())) {
                if (const auto definition = memory->itemSystem()->getItemSchema()->getItemDefinitionByName(event->getString("weapon"))) {
                    auto& purchase = purchaseDetails[player->getPlayerName()];
                    if (const auto weaponInfo = memory->weaponSystem->getWeaponInfo(definition->getWeaponId())) {
                        purchase.second += weaponInfo->price;
                        totalCost += weaponInfo->price;
                    }
                    const std::string weapon = interfaces->localize->findAsUTF8(definition->getItemBaseName());
                    ++purchaseTotal[weapon];
                    purchase.first.push_back(weapon);
                }
            }
            break;
        }
        case fnv::hash("round_start"):
            freezeEnd = 0.0f;
            purchaseDetails.clear();
            purchaseTotal.clear();
            totalCost = 0;
            break;
        case fnv::hash("round_freeze_end"):
            freezeEnd = memory->globalVars->realtime;
            break;
        }
    } else {
        if (!config->misc.purchaseList.enabled)
            return;

        static const auto mp_buytime = interfaces->cvar->findVar("mp_buytime");

        if ((!interfaces->engine->isInGame() || freezeEnd != 0.0f && memory->globalVars->realtime > freezeEnd + (!config->misc.purchaseList.onlyDuringFreezeTime ? mp_buytime->getFloat() : 0.0f) || purchaseDetails.empty() || purchaseTotal.empty()) && !gui->open)
            return;

        ImGui::SetNextWindowSize({ 200.0f, 200.0f }, ImGuiCond_Once);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
        if (!gui->open)
            windowFlags |= ImGuiWindowFlags_NoInputs;
        if (config->misc.purchaseList.noTitleBar)
            windowFlags |= ImGuiWindowFlags_NoTitleBar;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, { 0.5f, 0.5f });
        ImGui::Begin("Purchases", nullptr, windowFlags);
        ImGui::PopStyleVar();

        if (config->misc.purchaseList.mode == PurchaseList::Details) {
            for (const auto& [playerName, purchases] : purchaseDetails) {
                std::string s;
                s.reserve(std::accumulate(purchases.first.begin(), purchases.first.end(), 0, [](int length, const auto& s) { return length + s.length() + 2; }));
                for (const auto& purchasedItem : purchases.first)
                    s += purchasedItem + ", ";

                if (s.length() >= 2)
                    s.erase(s.length() - 2);

                if (config->misc.purchaseList.showPrices)
                    ImGui::TextWrapped("%s $%d: %s", playerName.c_str(), purchases.second, s.c_str());
                else
                    ImGui::TextWrapped("%s: %s", playerName.c_str(), s.c_str());
            }
        } else if (config->misc.purchaseList.mode == PurchaseList::Summary) {
            for (const auto& purchase : purchaseTotal)
                ImGui::TextWrapped("%d x %s", purchase.second, purchase.first.c_str());

            if (config->misc.purchaseList.showPrices && totalCost > 0) {
                ImGui::Separator();
                ImGui::TextWrapped("Total: $%d", totalCost);
            }
        }
        ImGui::End();
    }
}


void Misc::doorSpam(UserCmd* cmd) noexcept {

    if (!localPlayer || !config->misc.doorSpam || localPlayer->isDefusing())
        return;

    constexpr auto doorRange = 200.0f;

    Trace trace;
    const auto startPos = localPlayer->getEyePosition();
    const auto endPos = startPos + Vector::fromAngle(cmd->viewangles) * doorRange;
    interfaces->engineTrace->traceRay({ startPos, endPos }, 0x46004009, localPlayer.get(), trace);

    if (trace.entity && trace.entity->getClientClass()->classId == ClassId::PropDoorRotating)
        if (cmd->buttons & UserCmd::IN_USE && cmd->tickCount & 1)
            cmd->buttons &= ~UserCmd::IN_USE;
}


static std::vector<std::uint64_t> reportedPlayers;
static int reportbotRound;

void Misc::runReportbot() noexcept
{
    if (!config->misc.reportbot.enabled)
        return;

    if (!localPlayer)
        return;

    static auto lastReportTime = 0.0f;

    if (lastReportTime + config->misc.reportbot.delay > memory->globalVars->realtime)
        return;

    if (reportbotRound >= config->misc.reportbot.rounds)
        return;

    for (int i = 1; i <= interfaces->engine->getMaxClients(); ++i) {
        const auto entity = interfaces->entityList->getEntity(i);

        if (!entity || entity == localPlayer.get())
            continue;

        if (config->misc.reportbot.target != 2 && (entity->isOtherEnemy(localPlayer.get()) ? config->misc.reportbot.target != 0 : config->misc.reportbot.target != 1))
            continue;

        PlayerInfo playerInfo;
        if (!interfaces->engine->getPlayerInfo(i, playerInfo))
            continue;

        if (playerInfo.fakeplayer || std::find(reportedPlayers.cbegin(), reportedPlayers.cend(), playerInfo.xuid) != reportedPlayers.cend())
            continue;

        std::string report;

        if (config->misc.reportbot.textAbuse)
            report += "textabuse,";
        if (config->misc.reportbot.griefing)
            report += "grief,";
        if (config->misc.reportbot.wallhack)
            report += "wallhack,";
        if (config->misc.reportbot.aimbot)
            report += "aimbot,";
        if (config->misc.reportbot.other)
            report += "speedhack,";

        if (!report.empty()) {
            memory->submitReport(std::to_string(playerInfo.xuid).c_str(), report.c_str());
            lastReportTime = memory->globalVars->realtime;
            reportedPlayers.push_back(playerInfo.xuid);
        }
        return;
    }

    reportedPlayers.clear();
    ++reportbotRound;
}

void Misc::cheatSpam() noexcept
{
    if (!localPlayer)
        return;

    static auto lastTime = 2.0f;

    if (config->misc.cheatSpam) {
        if (memory->globalVars->realtime - lastTime < 2.0f)
            return;

        updateClanTag(true);
        std::string cmd = "say \"";
        cmd += XorString("The most COMPLETE and SAFE cheat on the market at the LOWEST PRICE, https://Cheat.net");
        cmd += '"';
        interfaces->engine->clientCmdUnrestricted(cmd.c_str());
        lastTime = memory->globalVars->realtime;
    }
}

void Misc::resetReportbot() noexcept
{
    reportbotRound = 0;
    reportedPlayers.clear();
}

void Misc::preserveKillfeed(bool roundStart) noexcept
{
    if (!config->misc.preserveKillfeed.enabled)
        return;

    static auto nextUpdate = 0.0f;

    if (roundStart) {
        nextUpdate = memory->globalVars->realtime + 10.0f;
        return;
    }

    if (nextUpdate > memory->globalVars->realtime)
        return;

    nextUpdate = memory->globalVars->realtime + 2.0f;

    const auto deathNotice = memory->findHudElement(memory->hud, "CCSGO_HudDeathNotice");
    if (!deathNotice)
        return;

    const auto deathNoticePanel = (*(UIPanel**)(*(deathNotice - 5 + 22) + 4));
    const auto childPanelCount = deathNoticePanel->getChildCount();

    for (int i = 0; i < childPanelCount; ++i) {
        const auto child = deathNoticePanel->getChild(i);
        if (!child)
            continue;

        if (child->hasClass("DeathNotice_Killer") && (!config->misc.preserveKillfeed.onlyHeadshots || child->hasClass("DeathNoticeHeadShot")))
            child->setAttributeFloat("SpawnTime", memory->globalVars->currenttime);
    }
}
