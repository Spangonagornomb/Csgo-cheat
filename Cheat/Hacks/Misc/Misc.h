#pragma once
#include "../../Engine/Engine.h"
#include "../../Render/DXOverlayManager.h"
#include "../Settings.h"
#include "../../X1API/MinHook/hook.h"

namespace HookTables
{
	using DrawModelExecuteFn = bool(__thiscall*)(void*, IMatRenderContext* ctx, const DrawModelState_t &state,
		const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld);
	extern cDetour<DrawModelExecuteFn>* pDrawModelExecute;
}

namespace Engine
{
	class CBaseEntity;
	class CPlayer;
}

using namespace Engine;

class CHitListener : public IGameEventListener2
{
public:

	struct DamgeInfo_t
	{
		Vector Pos;
		int Number;
		float EndTime;
		float Alpha;
		float OffsetZ;
	};

	vector<DamgeInfo_t> DamageVector;

	bool DrawHit = false;
	bool PlaySound = false;
	float HitMarkerEndTime = 0;

	void RegListener();
	void UnRegListener();

	void FireGameEvent(IGameEvent *pEvent) override;
	void Draw();

	int GetEventDebugID(void) override;
};

class IMisc
{
protected:

	virtual void GetViewModelFOV(float &Fov) = 0;
	virtual void PlaySound(const char* pszSoundName) = 0;
	virtual void AutoAcceptEmit() = 0;
	virtual void DrawModelExecute(void* thisptr, IMatRenderContext* ctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld) = 0;
	virtual void ShowSpectatorList() = 0;
	virtual vector<int> GetObservervators(int playerId) = 0;
	virtual void Night() = 0;
	virtual void SetNewClan(string New, string Name) = 0;
	virtual void Reset() = 0;
	virtual void UpdateSoundList() = 0;
	virtual void CustomWalls() = 0;
	virtual void FrameStageNotify(ClientFrameStage_t Stage) = 0;
	virtual void Menu() = 0;
	virtual void Draw() = 0;
	virtual void CreateMove(bool &bSendPacket, float flInputSampleTime, CUserCmd* pCmd) = 0;
	virtual void OverrideView(CViewSetup* pSetup) = 0;
};

class CMisc : public IMisc
{
public:
	CConfig Config = CConfig(("Misc"));

	struct HitImpact_t
	{
		Vector Pos;
		Vector MyHeadPos;
		float EndTime;
		float Alpha;
	};

	virtual void Menu();
	virtual void Draw();
	virtual void CreateMove(bool &bSendPacket, float flInputSampleTime, CUserCmd* pCmd);
	virtual void OverrideView(CViewSetup* pSetup);
	virtual void GetViewModelFOV(float &Fov);
	virtual void PlaySound(const char* pszSoundName);
	virtual void AutoAcceptEmit();
	virtual void DrawModelExecute(void* thisptr, IMatRenderContext* ctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld);
	virtual void ShowSpectatorList();
	virtual vector<int> GetObservervators(int playerId);
	virtual void Night();
	virtual void SetNewClan(string New, string Name);
	virtual void Reset();
	virtual void UpdateSoundList();
	virtual void CustomWalls();
	virtual void FrameStageNotify(ClientFrameStage_t Stage);

	bool Enable = true;
	bool BHop = false;
	bool AutoStrafe = false;
	bool InfiniteDuck = false;
	bool FovChanger = false;
	int FovView = 100;
	bool FovModelChanger = false;
	int FovModel = 80;
	bool NoVisualRecoil = false;
	bool FreeCam = false;
	int FreeCamSpeed = 5;

	CBind FreeCamBind = CBind(0, true);

	bool Crosshair = false;
	int CrosshairStyle = 1;
	int CrosshairSize = 5;
	bool CrosshairRecoil = true;
	bool CrosshairOutline = true;
	bool SnipCrosshair = false;
	int SnipCrosshairStyle = 1;
	int SnipCrosshairSize = 5;
	bool SnipCrosshairRecoil = false;
	bool SnipCrosshairOutline = true;

	bool SpreadCircel = false;

	vector<HitImpact_t> HitImpacts;

	bool HitMarker = false;
	int HitMarkerTickness = 1;
	int HitMarkerSize = 15;
	bool HitSound = false;
	int HitSoundStyle = 0;

	bool AntiFlash = false;
	int AntiFlashAlpha = 255;

	CHitListener HitWorker;

	bool ClanTagChanger = false;
	int ClanTagChangerStyle = 2;
	string ClanTagChangerText = ("obnoxious");

	bool ChatSpam = false;
	bool ChatSpamStart = false;
	int ChatSpamStyle = 0;
	string ChatSpamText = ("obnoxious");

	bool RadioSpam = false;
	bool RadioSpamStart = false;

	bool AutoAccept = false;
	bool SpectatorList = false;
	int SpectatorListAlpha = 255;
	int SpectatorListPosX = 500;
	int SpectatorListPosY = 500;
	bool SpectatorListAutoHide = true;

	bool LegitAA = false;
	bool NightMode = false;

	bool HandChams = false;
	int HandChamsStyle = 2;

	bool DamageInfo = false;
	int DamageInfoPosition = 1;
	int DamageInfoStyle = 0;
	float DamageInfoShowTime = 2;
	int DamageInfoPosX = 960;
	int DamageInfoPosY = 300;
	int DamageInfoSize = 300;
	int DamageInfoHeight = 90;
	int DamageInfoSpeed = 30;

	bool FakeLag = false;
	int FakeLagFactor = 7;
	CBind FakeLagBind = CBind(VK_LCONTROL);

	int TextDamageInfo = 38;

	bool ColoredWalls = false;
	int ColoredWallsStyle = 0;

	bool KnifeBot = false;
	int KnifeBotDistance = 81;
	int KnifeBotFilter = 0;
	int KnifeBotMode = 0;
	CBind KnifeBotBind = CBind(0x42);
	bool BulletTrace = false;


	Color CrosshairColor = Color(255, 0, 0, 255);
	Color SnipCrosshairColor = Color(255, 0, 0, 255);
	Color SpreadColor = Color(255, 255, 50, 255);
	Color HitMarkerColor = Color(255, 0, 0, 255);
	Color HandChamsColor = Color(255, 198, 0, 255);
	Color DamageInfoColor = Color(255, 100, 100, 255);
	Color ColoredWallsColor = Color(255, 100, 100, 255);

	vector<string> SoundList;

	CMisc()
	{
		RV(CrosshairColor, "CrosshairColor");
		RV(SnipCrosshairColor, "SnipCrosshairColor");
		RV(SpreadColor, "SpreadColor");
		RV(HitMarkerColor, "HitMarkerColor");
		RV(HandChamsColor, "HandChamsColor");
		RV(DamageInfoColor, "DamageInfoColor");
		RV(ColoredWallsColor, "ColoredWallsColor");

		RV(BHop, "BHop");
		RV(AutoStrafe, "AutoStrafe");
		RV(InfiniteDuck, "InfiniteDuck");
		RV(FovChanger, "FovChanger");
		RV(FovView, "FovView");
		RV(FovModelChanger, "FovModelChanger");
		RV(FovModel, "FovModel");
		RV(NoVisualRecoil, "NoVisualRecoil");
		RV(FreeCam, "FreeCam");
		RV(FreeCamSpeed, "FreeCamSpeed");
		RV(FreeCamBind, "FreeCamBind");
		RV(Crosshair, "Crosshair");
		RV(CrosshairStyle, "CrosshairStyle");
		RV(CrosshairSize, "CrosshairSize");
		RV(CrosshairRecoil, "CrosshairRecoil");
		RV(CrosshairOutline, "CrosshairOutline");
		RV(SnipCrosshair, "SnipCrosshair");
		RV(SnipCrosshairStyle, "SnipCrosshairStyle");
		RV(SnipCrosshairSize, "SnipCrosshairSize");
		RV(SnipCrosshairRecoil, "SnipCrosshairRecoil");
		RV(SnipCrosshairOutline, "SnipCrosshairOutline");
		RV(SpreadCircel, "SpreadCircel");
		RV(HitMarker, "HitMarker");
		RV(HitMarkerTickness, "HitMarkerTickness");
		RV(HitMarkerSize, "HitMarkerSize");
		RV(HitSound, "HitSound");
		RV(HitSoundStyle, "HitSoundStyle");
		RV(AntiFlash, "AntiFlash");
		RV(AntiFlashAlpha, "AntiFlashAlpha");
		RV(ClanTagChanger, "ClanTagChanger");
		RV(ClanTagChangerStyle, "ClanTagChangerStyle");
		RV(ClanTagChangerText, "ClanTagChangerText");
		RV(ChatSpam, "ChatSpam");
		RV(ChatSpamStart, "ChatSpamStart");
		RV(ChatSpamStyle, "ChatSpamStyle");
		RV(ChatSpamText, "ChatSpamText");
		RV(RadioSpam, "RadioSpam");
		RV(RadioSpamStart, "RadioSpamStart");
		RV(AutoAccept, "AutoAccept");
		RV(SpectatorList, "SpectatorList");
		RV(SpectatorListAlpha, "SpectatorListAlpha");
		RV(SpectatorListPosX, "SpectatorListPosX");
		RV(SpectatorListPosY, "SpectatorListPosY");
		RV(SpectatorListAutoHide, "SpectatorListAutoHide");
		RV(LegitAA, "LegitAA");
		RV(NightMode, "NightMode");
		RV(HandChams, "HandChams");
		RV(HandChamsStyle, "HandChamsStyle");
		RV(DamageInfo, "DamageInfo");
		RV(DamageInfoPosition, "DamageInfoPosition");
		RV(DamageInfoStyle, "DamageInfoStyle");
		RV(DamageInfoShowTime, "DamageInfoShowTime");
		RV(DamageInfoPosX, "DamageInfoPosX");
		RV(DamageInfoPosY, "DamageInfoPosY");
		RV(DamageInfoSize, "DamageInfoSize");
		RV(DamageInfoHeight, "DamageInfoHeight");
		RV(DamageInfoSpeed, "DamageInfoSpeed");
		RV(FakeLag, "FakeLag");
		RV(FakeLagFactor, "FakeLagFactor");
		RV(FakeLagBind, "FakeLagBind");

		RV(TextDamageInfo, "TextDamageInfo");
		RV(ColoredWalls, "ColoredWalls");
		RV(ColoredWallsStyle, "ColoredWallsStyle");
		RV(KnifeBot, "KnifeBot");
		RV(KnifeBotDistance, "KnifeBotDistance");
		RV(KnifeBotFilter, "KnifeBotFilter");
		RV(KnifeBotBind, "KnifeBotBind");
		RV(KnifeBotMode, "KnifeBotMode");

		ADD_LOG("Setup: CMisc sucessful\n");
	}
};