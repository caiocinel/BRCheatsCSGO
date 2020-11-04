#include <iostream>
#include <string>
#include <map>
#include <cstring>

#include "Lang.h"
#include "Xorstr/xorstr.hpp"
#include "Config.h"

std::map<std::string, std::string> phrases;

void UpdateLanguage()
{
	if (config->misc.lang == 0)
	{
		phrases[XorString("main_windowTitle")] = XorString("BRCheats");
		phrases[XorString("global_enabled")] = XorString("Enabled");
		phrases[XorString("global_threedots")] = XorString("...");
		phrases[XorString("global_nobackground")] = XorString("No Background");
		phrases[XorString("global_notitle")] = XorString("No title");
		phrases[XorString("global_key")] = XorString("[ key ]");
		phrases[XorString("global_keypopup")] = XorString("Press any key to change keybind");
		phrases[XorString("global_weapontype")] = XorString("All\0Pistols\0Heavy\0SMG\0Rifles\0");
		phrases[XorString("global_weapontype_zeus")] = XorString("All\0Pistols\0Heavy\0SMG\0Rifles\0Zeus x27\0");
		phrases[XorString("global_onkey")] = XorString("On key");
		phrases[XorString("global_keymode")] = XorString("Hold\0Toggle\0");
		phrases[XorString("global_bone")] = XorString("Bone");
		phrases[XorString("global_bonelist")] = XorString("Nearest\0Best damage\0Head\0Neck\0Sternum\0Chest\0Stomach\0Pelvis\0");
		phrases[XorString("global_all")] = XorString("All");
		phrases[XorString("global_type")] = XorString("Type");
		phrases[XorString("global_scale")] = XorString("Scale");
		phrases[XorString("global_fill")] = XorString("Fill");
		phrases[XorString("global_time")] = XorString("Time");
		phrases[XorString("global_hitgroup")] = XorString("Hitgroup");
		phrases[XorString("global_style")] = XorString("Style");
		phrases[XorString("global_color")] = XorString("Color");
		phrases[XorString("window_aimbot")] = XorString("Aimbot | BRCheats");
		phrases[XorString("window_ragebot")] = XorString("Ragebot | BRCheats");
		phrases[XorString("window_antiaim")] = XorString("Anti Aim | BRCheats");
		phrases[XorString("window_triggerbot")] = XorString("Triggerbot | BRCheats");
		phrases[XorString("window_backtrack")] = XorString("Backtrack | BRCheats");
		phrases[XorString("window_glow")] = XorString("Glow | BRCheats");
		phrases[XorString("window_chams")] = XorString("Chams | BRCheats");
		phrases[XorString("window_esp")] = XorString("ESP BRCheats | Use Game Capture from OBS, this ESP and the Menu will not be visible in your stream");
		phrases[XorString("aimhacks_friendlyfire")] = XorString("Friendly fire");
		phrases[XorString("aimhacks_visibleonly")] = XorString("Visible only");
		phrases[XorString("aimhacks_scopedonly")] = XorString("Scoped Only");
		phrases[XorString("aimhacks_ignoreflash")] = XorString("Ignore Flash");
		phrases[XorString("aimhacks_ignoresmoke")] = XorString("Ignore Smoke");
		phrases[XorString("aimhacks_autoshot")] = XorString("Auto Shot");
		phrases[XorString("aimhacks_autoscope")] = XorString("Auto Scope");
		phrases[XorString("aimhacks_killshot")] = XorString("Killshot");
		phrases[XorString("aimhacks_betweenshots")] = XorString("Between shots");
		phrases[XorString("aimhacks_hitchance")] = XorString("Hit Chance");
		phrases[XorString("aimhacks_mindamage")] = XorString("Min. Damage");
		phrases[XorString("aimhacks_silent")] = XorString("Silent");
		phrases[XorString("wallhacks_healthbased")] = XorString("Health Based");
		phrases[XorString("wallhacks_visibletype")] = XorString("All\0Visible\0Occluded\0");
		phrases[XorString("aimbot_fov")] = XorString("FOV");
		phrases[XorString("aimbot_smooth")] = XorString("Smooth");
		phrases[XorString("aimbot_drawfov")] = XorString("FOV");
		phrases[XorString("aimbot_rcs")] = XorString("Recoil Control System");
		phrases[XorString("aimbot_ignoreshots")] = XorString("Ignore Shots");
		phrases[XorString("aimbot_rcs_y")] = XorString("Recoil Control Y");
		phrases[XorString("aimbot_rcs_x")] = XorString("Recoil Control X");
		phrases[XorString("ragebot_autostop")] = XorString("Auto Stop");
		phrases[XorString("ragebot_baim")] = XorString("Baim");
		phrases[XorString("ragebot_forceshot")] = XorString("Force shot");
		phrases[XorString("ragebot_quickpeek")] = XorString("Quickpeek");
		phrases[XorString("ragebot_hitboxes")] = XorString("Hitboxes");
		phrases[XorString("ragebot_headvalue")] = XorString("Head Value");
		phrases[XorString("ragebot_bodyvalue")] = XorString("Body Value");
		phrases[XorString("ragebot_doubletap")] = XorString("Doubletap");
		phrases[XorString("ragebot_doubletap_speed")] = XorString("Doubletap Speed");
		phrases[XorString("ragebot_doubletap_keymode")] = XorString("Doubletap Key Mode");
		phrases[XorString("antiaim_invertkey")] = XorString("Invert Key");
		phrases[XorString("antiaim_fakewalk")] = XorString("Fakewalk");
		phrases[XorString("antiaim_fakewalk_speed")] = XorString("Speed");
		phrases[XorString("triggerbot_hitgroup")] = XorString("All\0Head\0Chest\0Stomach\0Left arm\0Right arm\0Left leg\0Right leg\0");
		phrases[XorString("triggerbot_shotdelay")] = XorString("Shot delay : % d ms");
		phrases[XorString("triggerbot_bursttime")] = XorString("Burst Time");
		phrases[XorString("backtrack_extend")] = XorString("Extend with fake ping");
		phrases[XorString("backtrack_timelimit")] = XorString("Time limit %d ms");
		phrases[XorString("glow_styles")] = XorString("Default\0Rim3d\0Edge\0Edge Pulse\0");
		phrases[XorString("glow_category")] = XorString("Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0C4\0Planted C4\0Chickens\0Defuse kits\0Projectiles\0Hostages\0Ragdolls\0");
		phrases[XorString("chams_category")] = XorString("Allies\0Enemies\0Planting\0Defusing\0Local player\0Weapons\0Hands\0Backtrack\0Sleeves\0Desync\0");
		phrases[XorString("chams_blinking")] = XorString("Blinking");
		phrases[XorString("chams_wireframe")] = XorString("Wireframe");
		phrases[XorString("chams_material")] = XorString("Material");
		phrases[XorString("chams_cover")] = XorString("Cover");
		phrases[XorString("chams_ignorez")] = XorString("Ignore-Z");
		phrases[XorString("esp_font")] = XorString("Font");
		phrases[XorString("esp_snapline")] = XorString("Snapline");
		phrases[XorString("esp_box")] = XorString("Box");
		phrases[XorString("esp_name")] = XorString("Name");
		phrases[XorString("esp_weapon")] = XorString("Weapon");
		phrases[XorString("esp_flashduration")] = XorString("Flash Duration");
		phrases[XorString("esp_skeleton")] = XorString("Skeleton");
		phrases[XorString("esp_audibleonly")] = XorString("Audible Only");
		phrases[XorString("esp_spottedonly")] = XorString("Spotted Only");
		phrases[XorString("esp_headbox")] = XorString("Head Box");
		phrases[XorString("esp_healthbar")] = XorString("Health Bar");
		phrases[XorString("esp_ammo")] = XorString("Ammo");
		phrases[XorString("esp_trails")] = XorString("Trails");
		phrases[XorString("esp_textculldistance")] = XorString("Text Cull Distance");
		
		
	}
	else if (config->misc.lang == 1)
	{



	}
}
//
//const char* Lang::get(const char* text)
//{
//	if (config->misc.lang == 0)
//		return Lang::en(text);
//
//	if(config->misc.lang == 1)
//		return Lang::pt(text);
//
//	else 
//	{
//		const char* value = Lang::en(text);
//		return value;
//	}
//
//};
//
//const char* Lang::en(const char* text) noexcept
//{
//	phrases[XorString("main_windowTitle")] = XorString("BRCheats EN");
//	phrases["404"] = "404 EN";
//
//	iten = phrases.find(XorString(text));
//	return iten->second;
//};

//
//const char* Lang::pt(const char* text) noexcept
//{
//
//	phrases[XorString("main_windowTitle")] = XorString("BRCheats PT");
//	phrases["404"] = "404 PT";
//
//	itpt = phrases.find(XorString(text));
//	return itpt->second;
//};
