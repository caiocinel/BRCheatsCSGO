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
		phrases[XorString("global_speed")] = XorString("Speed");
		phrases[XorString("global_edit")] = XorString("Edit");

		phrases[XorString("window_aimbot")] = XorString("Aimbot | BRCheats");
		phrases[XorString("window_ragebot")] = XorString("Ragebot | BRCheats");
		phrases[XorString("window_antiaim")] = XorString("Anti Aim | BRCheats");
		phrases[XorString("window_triggerbot")] = XorString("Triggerbot | BRCheats");
		phrases[XorString("window_backtrack")] = XorString("Backtrack | BRCheats");
		phrases[XorString("window_glow")] = XorString("Glow | BRCheats");
		phrases[XorString("window_chams")] = XorString("Chams | BRCheats");
		phrases[XorString("window_esp")] = XorString("ESP BRCheats | Use Game Capture from OBS, this ESP and the Menu will not be visible in your stream");
		phrases[XorString("window_visuals")] = XorString("Visuals | BRCheats");

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

		phrases[XorString("visuals_noweapons")] = XorString("No Weapons");
		phrases[XorString("visuals_nosmoke")] = XorString("No Smoke");
		phrases[XorString("visuals_noblur")] = XorString("No Blur");
		phrases[XorString("visuals_noscopeoverlay")] = XorString("No Scope Overlay");
		phrases[XorString("visuals_nograss")] = XorString("No Grass");
		phrases[XorString("visuals_noshadows")] = XorString("No Shadows");
		phrases[XorString("visuals_nightmode")] = XorString("Night Mode");
		phrases[XorString("visuals_asuswalls")] = XorString("Asus Walls");
		phrases[XorString("visuals_nobloom")] = XorString("No Bloom");
		phrases[XorString("visuals_disablepostprocessing")] = XorString("Disable Post-processing");
		phrases[XorString("visuals_inverseragdoll")] = XorString("Inverse Ragdoll Gravity");
		phrases[XorString("visuals_nofog")] = XorString("No Fog");
		phrases[XorString("visuals_no3dsky")] = XorString("No 3D Sky");
		phrases[XorString("visuals_noaimpunch")] = XorString("No Aim Punch");
		phrases[XorString("visuals_noviewpunch")] = XorString("No View Punch");
		phrases[XorString("visuals_nohands")] = XorString("No Hands");
		phrases[XorString("visuals_nosleeves")] = XorString("No Sleeves");
		phrases[XorString("visuals_wireframesmoke")] = XorString("Wireframe Smoke");
		phrases[XorString("visuals_worldcolor")] = XorString("World Color");
		phrases[XorString("visuals_skycolor")] = XorString("Sky Color");
		phrases[XorString("visuals_deaglespinner")] = XorString("Deagle Spinner");
		phrases[XorString("visuals_bullettracers")] = XorString("Bullet Tracers");
		phrases[XorString("visuals_zoom")] = XorString("Zoom");
		phrases[XorString("visuals_t_model")] = XorString("T Player Model");
		phrases[XorString("visuals_ct_model")] = XorString("CT Player Model");

		phrases[XorString("visuals_thirdperson")] = XorString("Thirdperson");
		phrases[XorString("visuals_thirdperson_distance")] = XorString("Thirdperson distance: %d");
		phrases[XorString("visuals_viewmodelfov")] = XorString("Viewmodel FOV: %d");
		phrases[XorString("visuals_fov")] = XorString("FOV: %d");
		phrases[XorString("visuals_farz")] = XorString("Far Z: %d");
		phrases[XorString("visuals_flashreduction")] = XorString("Flash reduction: %d%%");
		phrases[XorString("visuals_brightness")] = XorString("Brightness: %.2f");
		phrases[XorString("visuals_skybox")] = XorString("Skybox");
		phrases[XorString("visuals_screeneffect")] = XorString("Screen Effect");
		phrases[XorString("visuals_hiteffect")] = XorString("Hit Effect");
		phrases[XorString("visuals_hiteffect_time")] = XorString("Screen Effect Time");
		phrases[XorString("visuals_hitmarker")] = XorString("Hit Marker");
		phrases[XorString("visuals_hitmarker_time")] = XorString("Hit Marker Time");
		phrases[XorString("visuals_indicators")] = XorString("Indicators");
		phrases[XorString("visuals_rainbowcrosshair")] = XorString("Rainbow Crosshair");
		phrases[XorString("visuals_noscopecrosshair")] = XorString("Noscope Crosshair");
		phrases[XorString("visuals_recoilcrosshair")] = XorString("Recoil Crosshair");
		phrases[XorString("visuals_watermark")] = XorString("Watermark");
		phrases[XorString("visuals_colorcorrection")] = XorString("Color Correction");
		phrases[XorString("visuals_ragdollforce")] = XorString("Ragdoll Force");
		phrases[XorString("visuals_ragdollforce_strenght")] = XorString("Ragdoll Force Strenght");


		
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
