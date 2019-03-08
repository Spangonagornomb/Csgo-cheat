#include "LegitAim.h"
#include "../Setup.h"


#define MAXBACKTRACKTICKS (Weapons[SelectedWeapon].BacktrackTicks)
#define TICK_INTERVAL			(I::GlobalVars()->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define GetWeap(a) ((a < 0) ? 0 : (a >= (int)Weapons.size() ? (int)Weapons.size()-1 : a))

int SelectedWeapon = 0;

void CLegitAim::SetSelectedWeapon(bool MenuCheck)
{
	if (MenuCheck ? !CGlobal::IsGuiVisble : true)
	{
		SelectedWeapon = CGlobal::GetWeaponId();

		if (SelectedWeapon < 0)
			SelectedWeapon = 0;

		if (WeaponCustomTypes == 1) // Standard
		{
			switch (CGlobal::GWeaponType)
			{
			case WEAPON_TYPE_RIFEL: SelectedWeapon = 34; StandardSubSelected = 0; CheckAimCust = true; break;
			case WEAPON_TYPE_PISTOL: SelectedWeapon = 35; StandardSubSelected = 1; CheckAimCust = true;  break;
			case WEAPON_TYPE_SHOTGUN: SelectedWeapon = 36; StandardSubSelected = 2; CheckAimCust = true;  break;
			case WEAPON_TYPE_SNIPER: SelectedWeapon = 37; StandardSubSelected = 3; CheckAimCust = true;  break;
			default: SelectedWeapon = -1; CheckAimCust = false;  break;
			}

		}
		else if (WeaponCustomTypes == 2) // Custom
		{
			bool Finded = false;
			if (SelectedWeapon >= 0 && SelectedWeapon < 34)
			{
				for (size_t i(0); i < CustomSubs.size(); i++)
				{
					for (auto &saw : CustomSubs[i].AllocatedWeapon)
					{
						string wepstr = CGlobal::WeaponNames[SelectedWeapon];
						if (wepstr == saw)
						{
							SelectedWeapon = CustomSubs[i].Idx;
							SelectedSub = i;
							Finded = true;
							break;
						}
					}
					if (Finded)
						break;
				}
			}
			if (!Finded)
			{
				SelectedWeapon = 0;
				CheckAimCust = false;
			}
			else
				CheckAimCust = true;

		}
		else if (WeaponCustomTypes == 3) // All
		{
			SelectedWeapon = 38;
			CheckAimCust = true;
		}
		else
			CheckAimCust = true;
	}
}

float TestSF = 10;

void DrawHitBoxLine(Vector* vHitBoxArray, Color color)
{
	Vector vHitBoxOneScreen;
	Vector vHitBoxTwoScreen;

	if (vHitBoxArray[0].IsZero() || !vHitBoxArray[0].IsValid() || vHitBoxArray[1].IsZero() || !vHitBoxArray[1].IsValid())
		return;

	if (CGlobal::WorldToScreen(vHitBoxArray[0], vHitBoxOneScreen) && CGlobal::WorldToScreen(vHitBoxArray[1], vHitBoxTwoScreen))
	{
		GP_Render->DrawLine(vHitBoxOneScreen.x, vHitBoxOneScreen.y,
			vHitBoxTwoScreen.x, vHitBoxTwoScreen.y, color);
	}
}
float TestMouse = 0;
void CLegitAim::Draw()
{
	if (Debug)
	{
		int VTabSz = 15;
		int VTab = 15;

		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("iBestTarget: ") + to_string(iBestTarget)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("iBestHitBox: ") + to_string(iBestHitBox)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("iSilentBestTarget: ") + to_string(iSilentBestTarget)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("iSilentBestHitBox: ") + to_string(iSilentBestHitBox)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("iShotsFired: ") + to_string(iShotsFired)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("iEndBullet: ") + to_string(AimEndBullet)).c_str()); VTab += VTabSz;
		VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("iSilentHitBox: ") + to_string(SilentHitBox)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("iHitBox: ") + to_string(HitBox)).c_str()); VTab += VTabSz;
		VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bNearest: ") + to_string(IsNearest)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bSilentNearest: ") + to_string(IsSilentNearest)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bSilentF: ") + to_string(CanSilent)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("iCurWeapon: ") + to_string(SelectedWeapon)).c_str()); VTab += VTabSz;
		VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("fGFov: ") + to_string(GFov)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("fGSilentFov: ") + to_string(GSilentFov)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("fGSmooth: ") + to_string(GSmooth)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bRCSStandelone: ") + to_string(CanRCSStandelone)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bRCS: ") + to_string(CanRCS)).c_str()); VTab += VTabSz;
		VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(), 
			(("BestPos: X: ") + to_string((int)BestHitBoxPos.x) + (", Y: ") + to_string((int)BestHitBoxPos.y) + (", Z: ") + to_string((int)BestHitBoxPos.z)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(), 
			(("BestSilentPos: X: ") + to_string((int)SilentBestHitBoxPos.x) + (", Y: ") + to_string((int)SilentBestHitBoxPos.y) + (", Z: ") + to_string((int)SilentBestHitBoxPos.z)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(), 
			(("FSA: X: ") + to_string((int)FovStartAng.x) + (", Y: ") + to_string((int)FovStartAng.y) + (", Z: ") + to_string((int)FovStartAng.z)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(), 
			(("PunchAng: X: ") + to_string((int)AimPunchAngle.x) + (", Y: ") + to_string((int)AimPunchAngle.y) + (", Z: ") + to_string((int)AimPunchAngle.z)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(), 
			(("PreVec: X: ") + to_string((int)PreVec.x) + (", Y: ") + to_string((int)PreVec.y) + (", Z: ") + to_string((int)PreVec.z)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(), 
			(("SilentPreVec: X: ") + to_string((int)SilentPreVec.x) + (", Y: ") + to_string((int)SilentPreVec.y) + (", Z: ") + to_string((int)SilentPreVec.z)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(), 
			(("FinalVec: X: ") + to_string((int)FinalVec.x) + (", Y: ") + to_string((int)FinalVec.y) + (", Z: ") + to_string((int)FinalVec.z)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(), 
			(("SilentFinalVec: X: ") + to_string((int)SilentFinalVec.x) + (", Y: ") + to_string((int)SilentFinalVec.y) + (", Z: ") + to_string((int)SilentFinalVec.z)).c_str()); VTab += VTabSz;
		VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("fCurtime: ") + to_string(CurTime)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("fFFDelayEnd: ") + to_string(FFDelay.STimer.EndTime)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bFFDelayEnable: ") + to_string(FFDelay.STimer.Enable)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bFFDelayDisable: ") + to_string(FFDelayDisable)).c_str()); VTab += VTabSz;

		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bFFDelayActive: ") + to_string(FFDelay.STimer.Active)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bFFOneClickDelay: ") + to_string(FFDelayOneClickEnable)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bFFOCDelayActive: ") + to_string(FFDelayOCActive)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bFFOCDelayDisable: ") + to_string(FFDelayOCDisable)).c_str()); VTab += VTabSz;
		VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("fTSDelayEnd: ") + to_string(TSDelay.STimer.EndTime)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bTSDelayEnable: ") + to_string(TSDelay.STimer.Enable)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bTSDelayActive: ") + to_string(TSDelay.STimer.Active)).c_str()); VTab += VTabSz;
		VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bKillStop: ") + to_string(KillStop)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bKillStopEnable: ") + to_string(KillStopEnable)).c_str()); VTab += VTabSz;
		VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bpSilentAutoDelay: ") + to_string(pSilentAutoDelay)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bpSilentAutoDelayEnable: ") + to_string(pSilentAutoDelayEnable)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bpSilentAutoDelayDisable: ") + to_string(pSilentAutoDelayDisable)).c_str()); VTab += VTabSz;
		VTab += VTabSz;

		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bFFAutoDelay: ") + to_string(FFAutoDelay)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bFFAutoDelayEnable: ") + to_string(FFAutoDelayEnable)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bFFAutoDelayDisable: ") + to_string(FFAutoDelayDisable)).c_str()); VTab += VTabSz;
		VTab += VTabSz;

		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bAutoPistol: ") + to_string(AutoPistol)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bIsPistol: ") + to_string(IsPistol)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bIsSniper & Type: ") + to_string(IsSniper) + (" : ") + to_string(IsSniperClass)).c_str()); VTab += VTabSz;
		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bIsShotgun: ") + to_string(IsShotgun)).c_str()); VTab += VTabSz;

		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("bFastZoom & bZoomed: ") + to_string(FastZoom) + (" : ") + to_string(IsZoomed)).c_str()); VTab += VTabSz;

		GP_Render->DrawString(15, Vec2(500, VTab), Color::Yellow(),  (("fTest: ") + to_string(TestMouse)).c_str()); VTab += VTabSz;
	}

	if (ShowEntitysName)
	{
		for (int EntIndex = 0; EntIndex < I::EntityList()->GetHighestEntityIndex(); EntIndex++)
		{
			CBaseEntity* pEntity = (CBaseEntity*)I::EntityList()->GetClientEntity(EntIndex);

			if (!pEntity)
				continue;

			const model_t* pModel = pEntity->GetModel();

			if (!pModel)
				continue;

			const char* pModelName = I::ModelInfo()->GetModelName(pModel);

			if (!pModelName)
				continue;

			Vector EntityScreen;

			if (true)
			{
				if (CGlobal::WorldToScreen(pEntity->GetRenderOrigin(), EntityScreen))
				{
					GP_Render->DrawString(14, Vec2(EntityScreen.x, EntityScreen.y), Color::Yellow(), false, true, pModelName);
				}
			}
		}
	}

	if (ShowBacktrack && Weapons[GetWeap(SelectedWeapon)].Backtrack && Weapons[GetWeap(SelectedWeapon)].BacktrackTicks)
	{
		Color BackColor = Color(255, 255, 255, 60);
		for (int i = 0; i < I::EntityList()->GetHighestEntityIndex(); i++)
		{
			CBaseEntity* local = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());
			CBaseEntity *entity = (CBaseEntity*)I::EntityList()->GetClientEntity(i);
			PlayerInfo pinfo;
			if (entity == nullptr || !local)
				continue;
			if (entity == local)
				continue;
			if (entity->IsDormant())
				continue;

			if (I::Engine()->GetPlayerInfo(i, &pinfo) && !entity->IsDead())
			{
				if (!local->IsDead())
				{
					for (int t = 0; t < Weapons[GetWeap(SelectedWeapon)].BacktrackTicks; ++t)
					{
						Vector screenbacktrack[64][12];

						if (headPositions[i][t].simtime && headPositions[i][t].simtime + 1 > local->GetSimTime())
						{
							for (BYTE IndexArray = 0; IndexArray < 18; IndexArray++)
							{

								DrawHitBoxLine(headPositions[i][t].vHitboxSkeletonArray[IndexArray], BackColor);
							}
						}
					}
				}
				else
				{
					memset(&headPositions[0][0], 0, sizeof(headPositions));
				}
			}
		}

	}

	if (CGlobal::FullUpdateCheck)
		return;

	if (AimWorking && Enable && CheckAimCust)
	{
		if (ShowSpot)
		{
			if (iBestHitBox != -1 && iBestTarget != -1)
			{
				Vector BestHitBoxScreen;

				if (CGlobal::WorldToScreen(BestHitBoxPos, BestHitBoxScreen))
				{
					float LineLen = 12.f;
					GP_Render->DrawLine(BestHitBoxScreen.x - (LineLen / 2.f), BestHitBoxScreen.y, BestHitBoxScreen.x + (LineLen / 2.f), BestHitBoxScreen.y, FovColor);
					GP_Render->DrawLine(BestHitBoxScreen.x, BestHitBoxScreen.y - (LineLen / 2.f), BestHitBoxScreen.x, BestHitBoxScreen.y + (LineLen / 2.f), FovColor);
				}
			}

			if (iSilentBestHitBox != -1 && iSilentBestTarget != -1)
			{
				Vector SilentBestHitBoxScreen;

				if (CGlobal::WorldToScreen(SilentBestHitBoxPos, SilentBestHitBoxScreen))
				{
					float LineLen = 12.f;
					GP_Render->DrawLine(SilentBestHitBoxScreen.x - (LineLen / 2.f), SilentBestHitBoxScreen.y,
						SilentBestHitBoxScreen.x + (LineLen / 2.f), SilentBestHitBoxScreen.y, SilentFovColor);
					GP_Render->DrawLine(SilentBestHitBoxScreen.x, SilentBestHitBoxScreen.y - (LineLen / 2.f),
						SilentBestHitBoxScreen.x, SilentBestHitBoxScreen.y + (LineLen / 2.f), SilentFovColor);
				}
			}
		}

#define GetWeap(a) ((a < 0) ? 0 : (a >= (int)Weapons.size() ? (int)Weapons.size()-1 : a))

		if ((DrawFov || DrawSilentFov) && pLocalPlayer && AimWorking && SelectedWeapon > -1)
		{
			CBaseEntity* pPlayer = pLocalPlayer;

			if (!pPlayer)
				return;

			float dy = CGlobal::iScreenHeight / CGlobal::GFovView;
			float dx = CGlobal::iScreenWidth / CGlobal::GFovView;

			bool _CanSilent = true;

			if (Weapons[GetWeap(SelectedWeapon)].SilentEndBullet > 0)
				if (iShotsFired >= Weapons[GetWeap(SelectedWeapon)].SilentEndBullet)
					_CanSilent = false;

			if (_CanSilent)
				if (Weapons[GetWeap(SelectedWeapon)].SilentStartBullet > 0)
					if (Weapons[GetWeap(SelectedWeapon)].SilentStartBullet - 1 > iShotsFired)
						_CanSilent = false;

			float x = CGlobal::iScreenWidth / 2.f;
			float y = CGlobal::iScreenHeight / 2.f;

			if (FovPos != 2)
			{
				if (pPlayer && pLocalWeapon && !pPlayer->IsDead())
				{
					if (FovPos == 0)
					{
						if (DrawFov)
							GP_Render->DrawRing(x, y, (dy * GFov / 3.f), 32, FovColor);
						if (DrawSilentFov && Weapons[GetWeap(SelectedWeapon)].Silent)
							if (_CanSilent)
								GP_Render->DrawRing(x, y, (dy * GSilentFov), 32, SilentFovColor);
					}
					else if (FovPos == 1)
					{
						Vector punchAngle = (pLocalPlayer->GetPunchAngles() * (Vector(100, 100, 0) / 100.f));

						Vector2D pos = Vector2D(
							x - (dx*(punchAngle.y)),
							y + (dy*(punchAngle.x)));

						if (DrawFov)
							GP_Render->DrawRing(pos.x, pos.y, (dy * GFov / 3.f), 32, FovColor);
						if (DrawSilentFov && Weapons[GetWeap(SelectedWeapon)].Silent)
							if (_CanSilent)
								GP_Render->DrawRing(pos.x, pos.y, (dy * GSilentFov), 32, SilentFovColor);
					}
				}
			}
			if (FovPos == 2 && pOldBestTarget)
			{
				if (DrawFov && iLastBestHitBox != -1 && pOldBestTarget->GetHealth() > 0)
				{
					float base_fov = pow((dx * GFov / 3.f) + 30, 2) * CGlobal::GFovView;
					float iFov = (base_fov / (pLocalPlayer->GetHitboxPosition(0).DistTo(BestHitBoxPos) * CGlobal::GFovView));

					Vector m_vAimBestHitboxScreen;

					if (CGlobal::WorldToScreen(BestHitBoxPos, m_vAimBestHitboxScreen))
						GP_Render->DrawRing(m_vAimBestHitboxScreen.x, m_vAimBestHitboxScreen.y, iFov / int(1920 / CGlobal::iScreenWidth), 32, FovColor);
				}
				if (DrawSilentFov && iLastSilentBestHitBox != -1 && pOldBestTarget->GetHealth() > 0 && Weapons[GetWeap(SelectedWeapon)].Silent)
				{
					float base_fov = pow((dx * GSilentFov) + 30, 2) * CGlobal::GFovView;
					float iFov = (base_fov / (pLocalPlayer->GetHitboxPosition(0).DistTo(SilentBestHitBoxPos) * CGlobal::GFovView));

					Vector m_vAimBestHitboxScreen;

					if (CGlobal::WorldToScreen(SilentBestHitBoxPos, m_vAimBestHitboxScreen) && _CanSilent)
						GP_Render->DrawRing(m_vAimBestHitboxScreen.x, m_vAimBestHitboxScreen.y, iFov / int(1920 / CGlobal::iScreenWidth), 32, SilentFovColor);
				}
			}
		}
	}
}

Vector OldStandRCS = Vector(0, 0, 0);
bool NextSimpleDisable = false;
bool DisableNewWeapn = false;
bool NextAutoShot = false;
float start = 0;

void CLegitAim::CreateMove(bool &bSendPacket, float flInputSampleTime, CUserCmd* pCmd)
{
	iBestTarget = -1;
	iBestHitBox = -1;
	iSilentBestTarget = -1;
	iSilentBestHitBox = -1;
	CanSilent = false;

	AimEndBullet = Weapons[GetWeap(SelectedWeapon)].EndBullet;

	if (!CheckOpportWork(pCmd))
	{
		FFDelayOCActive = false;
		return;
	}

	if (SelectedWeapon < 0)
	{
		return;
	}

	CurTime = I::GlobalVars()->curtime;

	FFDelay.Update(CurTime);
	TSDelay.Update(CurTime);

	if (!pLocalPlayer)
	{
		return;
	}

	bool IsRevolver = CGlobal::GWeaponID == WEAPON_REVOLVER && (pCmd->buttons & IN_ATTACK2);

	IsPistol = CGlobal::GWeaponType == WEAPON_TYPE_PISTOL && CGlobal::GWeaponID != WEAPON_REVOLVER;
	IsSniper = (CGlobal::GWeaponID == WEAPON_AWP || CGlobal::GWeaponID == WEAPON_SSG08);
	IsShotgun = CGlobal::GWeaponType == WEAPON_TYPE_SHOTGUN;

	if (IsSniper || IsShotgun || IsPistol)
		Weapons[GetWeap(SelectedWeapon)].EndBullet = 1;
	else
		Weapons[GetWeap(SelectedWeapon)].EndBullet = -1;

	SetMainParams();
	LpCmd = pCmd;

	StandeloneRCS(pCmd);

	Vector StartAng = pCmd->viewangles;

	AimPunchAngle = pLocalPlayer->GetPunchAngles();
	FovStartAng = pCmd->viewangles + ((AimPunchAngle * (Vector(RCS_X, RCS_Y, 0) / 100.f)) * 2.f);

	if (CanSilent)
	{
		if (!GetBestTarget(true))
			CanSilent = false;

		if (CanSilent)
			if (!GetBestHitBox(true))
				CanSilent = false;

		if (!CanSilent && pSilentAutoDelayEnable && FFDelayEnable)
			pSilentAutoDelay = true;
		else
			pSilentAutoDelay = false;
	}
	else
		pSilentAutoDelay = false;

	GetBestTarget(false);
	if (iBestTarget != -1)
		GetBestHitBox(false);

	static WEAPON_ID OldWeapon = (WEAPON_ID)0;

	if (OldWeapon != CGlobal::GWeaponID)
	{
		DisableNewWeapn = true;
		start = I::GlobalVars()->curtime;
	}

	if (DisableNewWeapn)
	{
		float end = I::GlobalVars()->curtime;
		float diff = end - start;
		if (diff > 0.8f)
			DisableNewWeapn = false;
	}

	OldWeapon = CGlobal::GWeaponID;

	bool FindedTarg = (iBestTarget != -1 && iBestHitBox != -1) || (iSilentBestTarget != -1 && iSilentBestHitBox != -1);

	if (AutoFire && FindedTarg)
		IsRevolver ? pCmd->buttons |= IN_ATTACK2 : pCmd->buttons |= IN_ATTACK;

	if (DisableNewWeapn)
		IsRevolver ? pCmd->buttons &= ~IN_ATTACK2 : pCmd->buttons &= ~IN_ATTACK;

	if (IsSniperClass && FastZoom && !IsZoomed)
	{
		if (pCmd->buttons & IN_ATTACK)
		{
			pCmd->buttons &= ~IN_ATTACK;
			pCmd->buttons |= IN_ZOOM;

			NextAutoShot = true;
		}
		else
			NextAutoShot = false;
	}
	else
		NextAutoShot = false;



	bool PistolPreFire = (IsRevolver ? (pCmd->buttons & IN_ATTACK2) : (pCmd->buttons & IN_ATTACK));

	if ((IsRevolver ? (pCmd->buttons & IN_ATTACK2) : (pCmd->buttons & IN_ATTACK)))
		if ((AutoPistol || AutoFire) && IsPistol)
			CalcAutoPistol(pCmd, pLocalPlayer);

	bool AnyDelay = (FFDelay.STimer.Active || FFAutoDelay || pSilentAutoDelay);

	static bool FixAttack = false;

	if (FixAttack)
	{
		IsRevolver ? pCmd->buttons |= IN_ATTACK2 : pCmd->buttons |= IN_ATTACK;
		FixAttack = false;
	}

	if (FFDelayOCActive && !AnyDelay && FindedTarg && !FFDelayOCDisable)
	{
		IsRevolver ? pCmd->buttons |= IN_ATTACK2 : pCmd->buttons |= IN_ATTACK;
		FixAttack = true;
		FFDelayOCDisable = true;
	}

	bool IsAttack = (IsRevolver ? (pCmd->buttons & IN_ATTACK2) : (pCmd->buttons & IN_ATTACK)) || (NextAutoShot && IsSniperClass && FastZoom);

	FFDelayOCActive = FindedTarg && AnyDelay && (IsAttack || FFDelayOCActive) && FFDelayOneClickEnable && !FFDelayOCDisable;

	static bool SetRevolvTime = false;
	static float RevolvStartFire = 0;
	static float RevolvNextFire = 0;

	if ((pCmd->buttons & IN_ATTACK) && !SetRevolvTime)
	{
		RevolvNextFire = I::GlobalVars()->curtime + 0.20f;
		SetRevolvTime = true;
	}

	if (!(pCmd->buttons & IN_ATTACK))
		SetRevolvTime = false;

	if ((((IsAttack || (AutoPistol && IsPistol && PistolPreFire)) && FindedTarg) || FFDelayOCActive) && true)
	{
		if (FFAutoDelayEnable && FFDelayEnable && !FFDelayDisable)
		{
			if (!CanFire(FovStartAng, pCmd, pBestTarget, 1, pLocalPlayer, true))
				FFAutoDelay = true;
			else
			{
				FFAutoDelay = false;
				FFAutoDelayDisable = true;
			}
		}
		else
			FFAutoDelay = false;

		if (!KillStopEnable)
		{
			if (TSDelayEnable && TSDelayTime > 0 && pOldBestTarget)
				if (pOldBestTarget->GetHealth() == 0)
					TSDelay = CAimTimer(CurTime + TSDelayTime, true);
		}
		else
			if (pOldBestTarget && pOldBestTarget->GetHealth() == 0)
				KillStop = true;

		if (FFAutoDelay && !FFAutoDelayDisable)
			IsRevolver ? pCmd->buttons &= ~IN_ATTACK2 : pCmd->buttons &= ~IN_ATTACK;

		if (FFDelayEnable && FFDelayTime > 0 && !FFDelayDisable)
			if (!FFDelay.STimer.Enable)
				FFDelay = CAimTimer(CurTime + FFDelayTime, true);

		if (FFDelay.STimer.Active)
			IsRevolver ? pCmd->buttons |= IN_ATTACK2 : pCmd->buttons |= IN_ATTACK;

		CalcFinalVecRcs(false);
		if (CanSilent)
			CalcFinalVecRcs(true);

		if (FFDelay.STimer.Active)
			IsRevolver ? pCmd->buttons &= ~IN_ATTACK2 : pCmd->buttons &= ~IN_ATTACK;

		if (pSilentAutoDelay && !pSilentAutoDelayDisable && !CanFire(FovStartAng, pCmd, pBestTarget, 1, pLocalPlayer, true))
			IsRevolver ? pCmd->buttons &= ~IN_ATTACK2 : pCmd->buttons &= ~IN_ATTACK;
		else
			pSilentAutoDelayDisable = true;

		if ((IsRevolver ? (pCmd->buttons & IN_ATTACK2) : (pCmd->buttons & IN_ATTACK)))
			FFDelayOCDisable = true;

		if (!TSDelay.STimer.Active && !KillStop)
		{
			Vector FinalVec_Temp = Smooth(StartAng, FinalVec, GSmooth, false);
			Vector SilentFinalVec_Temp = Smooth(StartAng, SilentFinalVec, 0, true);

			if (CanSilent)
				CanSilent = (Weapons[GetWeap(SelectedWeapon)].SilentHitchance == 100 ?
					true : ((1 + (rand() % 101)) <= Weapons[GetWeap(SelectedWeapon)].SilentHitchance));

			if (!CanSilent)
				if (iBestTarget != -1 && iBestHitBox != -1)
					CreateAssistMove(FinalVec_Temp);

			if (CGlobal::GWeaponID == WEAPON_REVOLVER)
			{
				bool FinalEnableSilent = CanSilent;

				if ((CGlobal::GWeaponID == WEAPON_REVOLVER) && (pCmd->buttons & IN_ATTACK))
				{
					ADD_LOG("%f\n", RevolvNextFire - I::GlobalVars()->curtime);

					bool TimeDDD = RevolvNextFire - I::GlobalVars()->curtime > ((iShotsFired == 0) ? 0.16f : 0.18f);

					if (TimeDDD)
						FinalEnableSilent = false;
					else
						SetRevolvTime = false;
				}

				if (FinalEnableSilent)
				{
					pCmd->viewangles = SilentFinalVec_Temp;
					if (iBestTarget != -1 && iBestHitBox != -1)
						CreateAssistMove(FinalVec_Temp, true);

					bSendPacket = false;
				}
				else
				{
					if (CanSilent)
						if (iBestTarget != -1 && iBestHitBox != -1)
							CreateAssistMove(FinalVec_Temp);
				}
			}
			else
			{
				if (CanSilent)
				{
					pCmd->viewangles = SilentFinalVec_Temp;
					if (iBestTarget != -1 && iBestHitBox != -1)
						CreateAssistMove(FinalVec_Temp, true);

					bSendPacket = false;
				}
			}
		}
	}
	else
	{
		FFDelay.STimer.Enable = false;
		TSDelay.STimer.Enable = false;
		KillStop = false;
		pSilentAutoDelayDisable = false;
		FFAutoDelay = false;
		FFAutoDelayDisable = false;
		SetSmoothStart = false;
		FFDelayOCDisable = false;
	}


	if (!(IsRevolver ? (pCmd->buttons & IN_ATTACK2) : (pCmd->buttons & IN_ATTACK)))
		FFDelayDisable = false;
	else
		FFDelayDisable = true;


	if (pBestTarget)
		pOldBestTarget = pBestTarget;
}

void CLegitAim::SetMainParams()
{
	SmoothMethod = Weapons[GetWeap(SelectedWeapon)].SmoothMethod;
	StartAcceleration = Weapons[GetWeap(SelectedWeapon)].StartAcceleration;
	EndAcceleration = Weapons[GetWeap(SelectedWeapon)].EndAcceleration;

	KillStopEnable = Weapons[GetWeap(SelectedWeapon)].KillStop;

	FFDelayOneClickEnable = Weapons[GetWeap(SelectedWeapon)].FirstFireDelayOneClick;

	FFDelayEnable = Weapons[GetWeap(SelectedWeapon)].FirstFireDelayEnable;
	FFDelayTime = Weapons[GetWeap(SelectedWeapon)].FirstFireDelay;

	TSDelayEnable = Weapons[GetWeap(SelectedWeapon)].TargetSwitchDelayEnable;
	TSDelayTime = Weapons[GetWeap(SelectedWeapon)].TargetSwitchDelay;

	pSilentAutoDelayEnable = Weapons[GetWeap(SelectedWeapon)].FirstFireDelaySilentAuto;
	FFAutoDelayEnable = Weapons[GetWeap(SelectedWeapon)].FirstFireDelayAuto && !EntityAim;

	AutoPistol = Weapons[GetWeap(SelectedWeapon)].AutoPistol;
	FastZoom = Weapons[GetWeap(SelectedWeapon)].FastZoom;

	/*========================================== All =============================================================*/
	CanSilent = IsEnableSilent();
	CanRCS = IsEnableRCS();
	FovPos = Weapons[GetWeap(SelectedWeapon)].FovPos;
	SmoothMF = Weapons[GetWeap(SelectedWeapon)].SmoothMoveFactor;

	/*========================================== Silent ==========================================================*/
	IsSilentNearest = Weapons[GetWeap(SelectedWeapon)].SilentNearest;
	SilentHitBox = Weapons[GetWeap(SelectedWeapon)].SilentHitBox;
	GSilentFov = Weapons[GetWeap(SelectedWeapon)].SilentFov;

	/*========================================== Standard ========================================================*/
	IsNearest = Weapons[GetWeap(SelectedWeapon)].HitBoxAfter1B && iShotsFired >= 1 ?
		Weapons[GetWeap(SelectedWeapon)].NearestRcs : Weapons[GetWeap(SelectedWeapon)].Nearest;

	HitBox = Weapons[GetWeap(SelectedWeapon)].HitBoxAfter1B && iShotsFired >= 1 ?
		Weapons[GetWeap(SelectedWeapon)].HitBoxRcs : Weapons[GetWeap(SelectedWeapon)].HitBox;

	GFov = Weapons[GetWeap(SelectedWeapon)].FovRcs != 0 && iShotsFired >= 1 ?
		Weapons[GetWeap(SelectedWeapon)].FovRcs : Weapons[GetWeap(SelectedWeapon)].Fov;

	if (GDrawRcsFov)
	{
		GFov = Weapons[GetWeap(SelectedWeapon)].FovRcs;
		GDrawRcsFov = false;
	}

	GSmooth = Weapons[GetWeap(SelectedWeapon)].SmoothRcs != 0 && iShotsFired >= 1 ?
		Weapons[GetWeap(SelectedWeapon)].SmoothRcs : Weapons[GetWeap(SelectedWeapon)].Smooth;
	/*==========================================================================================================*/
	CanRCSStandelone = Weapons[GetWeap(SelectedWeapon)].RcsStandelone;

	FaceItMode = FaceIt;
}

bool CLegitAim::CheckOpportWork(CUserCmd* pCmd)
{
	AimWorking = false;
	static bool DisableIfPK = false;

	if (IsNonAimWeapon())
	{
		if ((pCmd->buttons & IN_ATTACK))
			DisableIfPK = true;

		start = I::GlobalVars()->curtime;
		DisableNewWeapn = true;
		return false;
	}

	if (DisableIfPK && (pCmd->buttons & IN_ATTACK) && AimEndBullet == 1)
		return false;
	else
		DisableIfPK = false;

	if (!AimBind.Check())
		return false;

	pLocalPlayer = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());

	if (!pLocalPlayer)
		return false;

	if (pLocalPlayer->IsDead())
		return false;

	pLocalWeapon = pLocalPlayer->GetBaseWeapon();
	if (!pLocalWeapon)
		return false;

	if (!(pLocalPlayer->GetFlags() & FL_ONGROUND) && JumpCheck)
		return false;

	if (pLocalPlayer->IsFlashed() && FlashCheck)
		return false;

	pLocalWeapon = pLocalPlayer->GetBaseWeapon();

	if (!pLocalWeapon)
		return false;

	if (pLocalWeapon->GetWeaponAmmo() == 0)
		return false;

	if (pLocalWeapon->GetWeaponReload())
		return false;

	iShotsFired = pLocalPlayer->GetShotsFired();

	if (NextAutoShot && iShotsFired != 0)
	{
		NextAutoShot = false;
	}

	if (AimEndBullet != -1 && iShotsFired >= AimEndBullet && !AutoPistol)
		return false;

	if (CGlobal::GWeaponType == WEAPON_TYPE_SNIPER)
		IsSniperClass = true;
	else
		IsSniperClass = false;

	if (IsSniperClass)
		if (pLocalWeapon->GetZoomLevel() == 1 || pLocalWeapon->GetZoomLevel() == 2)
			IsZoomed = true;
		else
			IsZoomed = false;
	else
		IsZoomed = false;


	AimWorking = true;

	POINT pt;
	FastCall::G().t_GetCursorPos(&pt);
	MousePos = Vector(pt.x, pt.y, 0);

	ScreenCenterX = CGlobal::iScreenWidth / 2;
	ScreenCenterY = CGlobal::iScreenHeight / 2;

	return true;
}

float To360(float in)
{
	return (in > 0 ? in : 180 + (180 + in));
}

void CLegitAim::CreateAssistMove(Vector TargetAng, bool IsSilent)
{
	if (!FaceItMode)
	{
		if (IsSilent)
			I::Engine()->SetViewAngles(TargetAng);
		else
		{
			if (LpCmd)
				LpCmd->viewangles = TargetAng;
			I::Engine()->SetViewAngles(TargetAng);
		}
	}
	else if (FaceItMode && !CGlobal::IsGuiVisble)
	{
		if (IsSilent)
			I::Engine()->SetViewAngles(TargetAng);
		else
		{
			Vector ScreenTargPoint;
			if (LpCmd)
			{

				float diff_x = TargetAng.y - LpCmd->viewangles.y;

				if (LpCmd->viewangles.y > 90 && TargetAng.y < -90)
					diff_x = To360(TargetAng.y) - To360(LpCmd->viewangles.y);

				ScreenTargPoint.x = (ScreenCenterX + ((((ScreenCenterX / CGlobal::GFovView) * diff_x)) * -1.f));
				ScreenTargPoint.y = (ScreenCenterY + (((ScreenCenterX / CGlobal::GFovView) * (TargetAng.x - LpCmd->viewangles.x))));

				AssistScreenPoint = ScreenTargPoint;
				NewMouseOfstPos = Vector(ScreenTargPoint.x - MousePos.x, ScreenTargPoint.y - MousePos.y, 0);

				if (fabs(NewMouseOfstPos.x) < 1.f)
				{
					if (fabs(NewMouseOfstPos.x) > 0.01f)
					{
						if (NewMouseOfstPos.x > 0.f)
							NewMouseOfstPos.x = 1.f;
						else
							NewMouseOfstPos.x = -1.f;
					}
					else
						NewMouseOfstPos.x = 0.f;
				}

				if (fabs(NewMouseOfstPos.y) < 1.f)
				{
					if (fabs(NewMouseOfstPos.y) > 0.01f)
					{
						if (NewMouseOfstPos.y > 0.f)
							NewMouseOfstPos.y = 1.f;
						else
							NewMouseOfstPos.y = -1.f;
					}
					else
						NewMouseOfstPos.y = 0.f;
				}
				FastCall::G().t_mouse_event(MOUSEEVENTF_MOVE, (DWORD)NewMouseOfstPos.x, (DWORD)NewMouseOfstPos.y, NULL, NULL);
			}
		}
	}
}

void CLegitAim::CalcAutoPistol(CUserCmd* cmd, CBaseEntity * pLocal)
{
	float NextAttack = pLocal->GetBaseWeapon()->GetNextPrimaryAttack();
	float Tick = pLocal->GetTickBase() * I::GlobalVars()->interval_per_tick;

	if (NextAttack < Tick)
		return;

	if ((WEAPON_ID)*pLocal->GetBaseWeapon()->GeteAttributableItem()->GetItemDefinitionIndex() == WEAPON_REVOLVER)
		cmd->buttons &= ~IN_ATTACK2;
	else
		cmd->buttons &= ~IN_ATTACK;
}

static void Normalize(Vector& angle) {
	while (angle.x > 89.0f) {
		angle.x -= 180.f;
	}
	while (angle.x < -89.0f) {
		angle.x += 180.f;
	}
	while (angle.y > 180.f) {
		angle.y -= 360.f;
	}
	while (angle.y < -180.f) {
		angle.y += 360.f;
	}
}

Vector CLegitAim::Smooth(Vector& viewangles, Vector target, float factor, bool silent)
{
	factor /= 100.f;
	Vector delta = target - viewangles;

	if (CCSmooth && factor > 0)
	{
		Vector OldViewAng = viewangles;
		Vector OldTargAng = target;

		TempVec3 = target;

		if (target.x >= 180)
			target.x = (180.f - (180.f - (360.f - target.x))) * -1.f;

		if (target.y >= 180)
			target.y = (180.f - (180.f - (360.f - target.y))) * -1.f;

		TempVec1 = viewangles;
		TempVec2 = target;

		SmoothCurLen = CCSmooth->GetCurLen(viewangles, target);

		if (!SetSmoothStart)
		{
			CCSmooth->StartLen = GFov / 3.f;
			SetSmoothStart = true;
		}

		SmoothCurAngle = CCSmooth->GetCurAngle(SmoothCurLen, viewangles, target);
		SmoothMaxDelta = CCSmooth->GetMaxDelta(CCSmooth->StartLen, SmoothCurAngle, viewangles, target);

		SmoothProgress = (SmoothCurLen / (CCSmooth->StartLen / 100.f));

		viewangles = OldViewAng;
		target = OldTargAng;
	}

	CGlobal::ClampAngles(delta);

	bool isCurve = SmoothMethod == 1 && !CanSilent;

	if (!isCurve)
	{
		Vector toChange = Vector(0, 0, 0);
		if (silent)
		{
			CGlobal::ClampAngles(target);
			return target;
		}

		if (SmoothMF == 0)
		{
			float smooth = powf(factor, 0.27f);

			smooth = min(0.9999999f, smooth);

			toChange = delta - delta * smooth;
		}
		else if (SmoothMF == 1)
		{
			if (factor <= 0)
			{
				CGlobal::ClampAngles(target);
				return target;
			}

			float smooth = powf(factor, 0.4f);
			smooth = min(0.9999999f, smooth);

			float coeff = fabsf(smooth - 1.f) / delta.Length() * 4.f;
			coeff = min(1.f, coeff);
			toChange = (delta * coeff);
		}

		Vector End = viewangles + toChange;

		CGlobal::ClampAngles(End);

		return End;
	}

	if (isCurve)
	{
		float AccelDelta = StartAcceleration - EndAcceleration;
		float DeltaAngle = atanf(AccelDelta / 100.f) * _180_PI;
		CurAcceleration = SmoothProgress * tan(DeltaAngle * PI / 180);

		if (CurAcceleration < 0)
			CurAcceleration = (AccelDelta * -1.f) - (CurAcceleration * -1.f);

		CurAcceleration = min(StartAcceleration, EndAcceleration) + CurAcceleration;

		factor = CurAcceleration / 100.f;

		Vector toChange = Vector(0, 0, 0);

		if (SmoothMF == 0)
		{
			float smooth = powf(factor, 0.4f);

			smooth = min(0.9999999f, smooth);

			toChange = delta - delta * smooth;

		}
		else if (SmoothMF == 1)
		{
			float smooth = powf(factor, 0.4f);
			smooth = min(0.9999999f, smooth);

			float coeff = fabsf(smooth - 1.f) / delta.Length() * 4.f;
			coeff = min(1.f, coeff);
			toChange = (delta * coeff);
		}

		float Factor_10 = 100.f;
		float Factor_50 = -100.f;
		float Factor_75 = 100.f;

		float CurFactor = 0;

		if (SmoothProgress <= 10.f)
			CurFactor = Factor_10;

		if (SmoothProgress > 10.f && SmoothProgress < 50.f)
			CurFactor = Factor_50;

		if (SmoothProgress >= 50.f)
			CurFactor = Factor_75;


		toChange.x += (CurFactor / 200.f) * toChange.y;
		toChange.y += (CurFactor / 200.f) * toChange.x;


		return viewangles + toChange;
	}
	return viewangles;
}

void CLegitAim::VixViewAng(Vector &VE)
{
	if (VE.y <= -90 && VE.y >= -179)
		VE.y = (90 - ((90 + VE.y) * -1));

	if (VE.x <= -90 && VE.x >= -179)
		VE.x = (90 - ((90 + VE.x) * -1));
}

Vector CLegitAim::PlayerAnglToScreen(Vector Angl)
{
	if (LpCmd)
	{
		Vector viewangles = LpCmd->viewangles;

		float FovFal = CGlobal::GFovView;
		return Vector(
			(ScreenCenterX + ((((ScreenCenterX / FovFal) * (Angl.y - viewangles.y))) * -1.f)),
			(ScreenCenterY + (((ScreenCenterX / FovFal) * (Angl.x - viewangles.x)))),
			0);
	}
	else
		return Vector(0, 0, 0);
}

bool CLegitAim::IsNonAimWeapon()
{
	if (CGlobal::GWeaponType == WEAPON_TYPE_GRENADE ||
		CGlobal::GWeaponType == WEAPON_TYPE_KNIFE ||
		CGlobal::GWeaponType == WEAPON_TYPE_C4 ||
		CGlobal::GWeaponType == WEAPON_TYPE_UNKNOWN)
		return true;

	return false;
}

bool CLegitAim::CanShoot()
{
	if (pLocalWeapon->GetWeaponAmmo() < 1)
		return false;

	float server_time = pLocalPlayer->GetTickBase() * I::GlobalVars()->interval_per_tick;

	float next_shot = pLocalWeapon->GetNextPrimaryAttack() - server_time;

	if (next_shot > 0)
	{
		return false;
	}

	return true;
}

bool CLegitAim::IsEnableSilent()
{
	if (FaceIt)
		return false;

	bool ret = false;
	if (Weapons[GetWeap(SelectedWeapon)].Silent && Weapons[GetWeap(SelectedWeapon)].SilentFov > 0)
	{
		ret = true;
		if (Weapons[GetWeap(SelectedWeapon)].SilentHitchance == 0)
			ret = false;
		else
		{
			if (Weapons[GetWeap(SelectedWeapon)].SilentStartBullet > 0)
				if (Weapons[GetWeap(SelectedWeapon)].SilentStartBullet - 1 > iShotsFired)
					ret = false;

			if (Weapons[GetWeap(SelectedWeapon)].SilentEndBullet > 0 && ret)
				if (iShotsFired >= Weapons[GetWeap(SelectedWeapon)].SilentEndBullet)
					ret = false;
		}
	}

	if (ret)
		ret = CanShoot();

	return ret;
}

bool CLegitAim::IsEnableRCS()
{
	bool Ret = false;

	RCS_X = Weapons[GetWeap(SelectedWeapon)].RcsX;
	RCS_Y = Weapons[GetWeap(SelectedWeapon)].RcsY;

	bool CheckPistol = CGlobal::GWeaponType == WEAPON_TYPE_PISTOL ? true : iShotsFired > 1;

	if ((RCS_X > 0 || RCS_Y > 0) && CheckPistol)
		Ret = true;
	else
		Ret = false;

	return Ret;
}

bool CLegitAim::GetBestTarget(bool _CheckSilent)
{
	float BestFov = _CheckSilent ? GSilentFov : GFov / 3.f;
	int _iBestTarget = -1;
	if (BestFov > 0.1f)
	{
		if (FovPos == 2)
			BestFov = FLT_MAX;

		MyEyeAng = pLocalPlayer->GetEyePosition();

		if (!EntityAim)
		{
			for (int i = 0; i < I::Engine()->GetMaxClients(); i++)
			{
				CBaseEntity* CheckEntity = (CBaseEntity*)I::EntityList()->GetClientEntity(i);

				if (!CheckEntity)
					continue;

				if (!CheckEntity->IsValid())
					continue;

				if (CheckEntity == pLocalPlayer)
					continue;

				if (!Deathmatch && CheckEntity->GetTeam() == pLocalPlayer->GetTeam())
					continue;

				if (!(CheckEntity->GetFlags() & FL_ONGROUND) && JumpEnemyCheck)
					continue;

				if (_CheckSilent ? IsSilentNearest : IsNearest)
				{
					for (int j(0); j < 17; j++)
					{
						Vector vecHitBox = CheckEntity->GetHitboxPosition(j);

						if (!CheckEntity->IsVisibleHitBox(pLocalPlayer, MyEyeAng, vecHitBox, SmokeCheck, FlashCheck) && WallCheck)
							continue;

						float _Fov = CalcFOV(FovStartAng, MyEyeAng, vecHitBox);

						if (_Fov < BestFov)
						{
							BestFov = _Fov;
							_iBestTarget = i;
							pBestTarget = CheckEntity;
							break;
						}
					}
				}
				else
				{
					int _HitBox = _CheckSilent ? SilentHitBox : HitBox;
					if (_HitBox == 3)
						_HitBox = 4;

					Vector vecHitBox = CheckEntity->GetHitboxPosition(_HitBox);

					if (!CheckEntity->IsVisibleHitBox(pLocalPlayer, MyEyeAng, vecHitBox, SmokeCheck, FlashCheck) && WallCheck)
						continue;

					float _Fov = CalcFOV(FovStartAng, MyEyeAng, vecHitBox);

					if (_Fov < BestFov)
					{
						BestFov = _Fov;
						_iBestTarget = i;
						pBestTarget = CheckEntity;
					}
				}
			}
		}
		else
		{
			for (int EntIndex = 0; EntIndex < I::EntityList()->GetHighestEntityIndex(); EntIndex++)
			{
				CBaseEntity* pEntity = (CBaseEntity*)I::EntityList()->GetClientEntity(EntIndex);

				if (!pEntity || pEntity->IsPlayer())
					continue;

				const model_t* pModel = pEntity->GetModel();

				if (pModel)
				{
					const char* pModelName = I::ModelInfo()->GetModelName(pModel);

					if (pModelName)
					{

						string md = pModelName;

						if (md == EntityTargetName)
						{
							Vector vSrc = pLocalPlayer->GetEyePosition();
							Vector vEnd = pEntity->GetEyePosition();

							float fov = CalcFOV(FovStartAng, MyEyeAng, vEnd);
							if (fov < BestFov)
							{
								BestFov = fov;
								_iBestTarget = EntIndex;
								pBestTarget = pEntity;
							}
						}
					}
				}

			}
		}
	}

	if (_CheckSilent)
	{
		iSilentBestTarget = _iBestTarget;
		if (iSilentBestTarget == -1)
			return false;
		else
			return true;
	}
	else
	{
		iBestTarget = _iBestTarget;
		if (iBestTarget == -1)
			return false;
		else
			return true;
	}
	return true;
}
bool CLegitAim::GetBestHitBox(bool _CheckSilent, bool CheckBackTrack)
{
	if (!CheckBackTrack)
		if (!pBestTarget)
			return false;

	float BestFov = _CheckSilent ? GSilentFov : GFov / 3.f;
	int HiddenHitBoxs = 0;
	int _BestHitBox = -1;
	Vector _BestHitBoxPos = Vector(0, 0, 0);

	if (BestFov > 0.1f)
	{
		if (FovPos == 2)
			BestFov = FLT_MAX;

		MyEyeAng = pLocalPlayer->GetEyePosition();
		Vector vecHitBox = Vector(0, 0, 0);

		if (!EntityAim)
		{
			if (_CheckSilent ? IsSilentNearest : IsNearest)
			{
				for (int i(0); i < 7; i++)
				{
					if (!CheckBackTrack)
					{
						vecHitBox = pBestTarget->GetHitboxPosition(i);

						if (!pBestTarget->IsVisibleHitBox(pLocalPlayer, MyEyeAng, vecHitBox, SmokeCheck, FlashCheck) && WallCheck)
						{
							HiddenHitBoxs++;
							continue;
						}
					}

					float _Fov = CalcFOV(FovStartAng, MyEyeAng, vecHitBox);

					if (_Fov < BestFov)
					{
						BestFov = _Fov;
						_BestHitBox = i;
						_BestHitBoxPos = vecHitBox;
					}
				}
				if (HiddenHitBoxs > 6)
				{
					for (int i(7); i < 17; i++)
					{
						if (!CheckBackTrack)
						{
							vecHitBox = pBestTarget->GetHitboxPosition(i);

							if (!pBestTarget->IsVisibleHitBox(pLocalPlayer, MyEyeAng, vecHitBox, SmokeCheck, FlashCheck) && WallCheck)
								continue;
						}

						float _Fov = CalcFOV(FovStartAng, MyEyeAng, vecHitBox);

						if (_Fov < BestFov)
						{
							BestFov = _Fov;
							_BestHitBox = i;
							_BestHitBoxPos = vecHitBox;
						}
					}
				}
			}
			else
			{
				int _HitBox = _CheckSilent ? SilentHitBox : HitBox;

				if (_HitBox == 3)
					_HitBox = 4;

				_BestHitBox = _HitBox;

				if (!CheckBackTrack)
					_BestHitBoxPos = pBestTarget->GetHitboxPosition(_HitBox);
			}

		}
		else
		{
			for (int EntIndex = 0; EntIndex < I::EntityList()->GetHighestEntityIndex(); EntIndex++)
			{
				CBaseEntity* pEntity = (CBaseEntity*)I::EntityList()->GetClientEntity(EntIndex);

				if (!pEntity || pEntity->IsPlayer())
					continue;

				const model_t* pModel = pEntity->GetModel();

				if (pModel)
				{
					const char* pModelName = I::ModelInfo()->GetModelName(pModel);

					if (pModelName)
					{

						string md = pModelName;

						if (md == EntityTargetName)
						{
							Vector vSrc = pLocalPlayer->GetEyePosition();
							Vector vEnd = pEntity->GetEyePosition();

							float fov = CalcFOV(FovStartAng, MyEyeAng, vEnd);
							if (fov < BestFov)
							{
								BestFov = fov;
								_BestHitBox = 1;
								_BestHitBoxPos = vEnd;
							}
						}
					}
				}

			}
		}

		if (_CheckSilent)
			iLastBestHitBox = _BestHitBox;
		else
			iLastSilentBestHitBox = _BestHitBox;

		if (FovPos == 2)
		{
			float dy = CGlobal::iScreenHeight / CGlobal::GFovView;
			float dx = CGlobal::iScreenWidth / CGlobal::GFovView;

			float iFovVal = 0;
			if (_CheckSilent)
				iFovVal = dx * GSilentFov;
			else
				iFovVal = dx * (GFov / 3.f);

			float base_fov = pow(iFovVal + 30, 2) * CGlobal::GFovView;
			BestFov = (base_fov / (pLocalPlayer->GetHitboxPosition(0).DistTo(_BestHitBoxPos) * CGlobal::GFovView));
			float fov = CalcFOV(FovStartAng, MyEyeAng, _BestHitBoxPos);
			fov += fov / 8.f;

			TestSF = fov;

			if (fov > (BestFov / 10.f))
				_BestHitBox = -1;
		}
	}
	if (_CheckSilent)
	{
		iSilentBestHitBox = _BestHitBox;
		SilentBestHitBoxPos = _BestHitBoxPos;

		SilentPreVec = CalculateAngle(MyEyeAng, SilentBestHitBoxPos);

		if (iSilentBestHitBox == -1)
			return false;
		else
			return true;
	}
	else
	{
		iBestHitBox = _BestHitBox;
		BestHitBoxPos = _BestHitBoxPos;

		PreVec = CalculateAngle(MyEyeAng, BestHitBoxPos);

		if (iBestHitBox == -1)
			return false;
		else
			return true;
	}
}

void CLegitAim::CalcFinalVecRcs(bool _CheckSilent)
{
	if (_CheckSilent)
		SilentFinalVec = SilentPreVec - ((AimPunchAngle * (Vector(100.f, 100.f, 0) / 100.f)) * 2.f);
	else
		FinalVec = PreVec - (AimPunchAngle * Vector(RCS_X / 50.f, RCS_Y / 50.f, 0));
}

float CLegitAim::CalcFOV(Vector& viewangles, const Vector& vSrc, const Vector& vEnd)
{
	Vector vAng, vAim;

	vAng = CalculateAngle(vSrc, vEnd);
	MakeVector(viewangles, vAim);
	MakeVector(vAng, vAng);

	return Rad2Deg(acos(vAim.Dot(vAng)) / vAim.LengthSqr());
}

void CLegitAim::MakeVector(const Vector& vIn, Vector& vOut)
{
	float pitch = Deg2Rad(vIn.x);
	float yaw = Deg2Rad(vIn.y);
	float temp = cos(pitch);

	vOut.x = -temp * -cos(yaw);
	vOut.y = sin(yaw) * temp;
	vOut.z = -sin(pitch);
}

void CLegitAim::StandeloneRCS(CUserCmd* pCmd)
{
	if ((pCmd->buttons & IN_ATTACK) && CanRCS && CanRCSStandelone)
	{
		Vector AimPunch = (AimPunchAngle * (Vector(RCS_X, RCS_Y, 0) / 100.f)) * 2.f;
		Vector StartPunch = pCmd->viewangles;
		Vector EndPunch = OldStandRCS - AimPunch;
		Vector BufPunch = StartPunch + EndPunch;

		Vector delta = BufPunch - pCmd->viewangles;
		BufPunch = pCmd->viewangles + (delta * (min(1.f, (fabsf(powf(0.6f, 0.4f) - 1.f) / delta.Length() * 4.f))));

		CGlobal::ClampAngles(BufPunch);

		if (FaceItMode)
		{
			Vector ScreenTargPoint;
			if (LpCmd)
			{
				ScreenTargPoint.x = (ScreenCenterX + ((((ScreenCenterX / CGlobal::GFovView) * (BufPunch.y - LpCmd->viewangles.y))) * -1.f));
				ScreenTargPoint.y = (ScreenCenterY + (((ScreenCenterX / CGlobal::GFovView) * (BufPunch.x - LpCmd->viewangles.x))));

				AssistScreenPoint = ScreenTargPoint;
				NewMouseOfstPos = Vector(ScreenTargPoint.x - MousePos.x, ScreenTargPoint.y - MousePos.y, 0);

				NewMouseOfstPos *= MouseSense;

				if (fabs(NewMouseOfstPos.x) < 1.f)
				{
					if (fabs(NewMouseOfstPos.x) > 0.01f)
					{
						if (NewMouseOfstPos.x > 0.f)
							NewMouseOfstPos.x = 1.f;
						else
							NewMouseOfstPos.x = -1.f;
					}
					else
						NewMouseOfstPos.x = 0.f;
				}

				if (fabs(NewMouseOfstPos.y) < 1.f)
				{
					if (fabs(NewMouseOfstPos.y) > 0.01f)
					{
						if (NewMouseOfstPos.y > 0.f)
							NewMouseOfstPos.y = 1.f;
						else
							NewMouseOfstPos.y = -1.f;
					}
					else
						NewMouseOfstPos.y = 0.f;
				}

				if (fabs(NewMouseOfstPos.x) > 110.f)
					NewMouseOfstPos.x = 0;

				if (fabs(NewMouseOfstPos.y) > 110.f)
					NewMouseOfstPos.y = 0;

				FastCall::G().t_mouse_event(MOUSEEVENTF_MOVE, (DWORD)NewMouseOfstPos.x, (DWORD)NewMouseOfstPos.y, NULL, NULL);
			}
		}
		else
			pCmd->viewangles = BufPunch;

		OldStandRCS = AimPunch;
	}
	else
		OldStandRCS = (AimPunchAngle*(Vector(RCS_X, RCS_Y, 0) / 100.f)) * 2.f;
}

Vector CLegitAim::CalculateAngle(const Vector& in, Vector out)
{
	Vector ret = Vector();
	Vector delta = in - out;
	double hyp = delta.Length2D();
	ret.y = (atan(delta.y / delta.x) * 57.295779513082f);
	ret.x = (atan(delta.z / hyp) * 57.295779513082f);
	ret[2] = 0.00;

	if (delta.x >= 0.0)
		ret.y += 180.0f;
	return ret;
}

float CLegitAim::Rad2Deg(float x)
{
	return (x * (180.0f / M_PI));
}

float CLegitAim::Deg2Rad(float x)
{
	return (x * (M_PI / 180.0f));
}

bool LastShoot = false;
float FireDelay = 0;
void CLegitAim::TriggerCreateMove(CUserCmd* pCmd)
{
	if (!TriggerBind.Check())
		return;

	if (IsNonAimWeapon())
		return;

	pLocalPlayer = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());

	if (!pLocalPlayer)
		return;

	if (pLocalPlayer->IsDead())
		return;

	pLocalWeapon = pLocalPlayer->GetBaseWeapon();

	if (!pLocalWeapon)
		return;

	if (pLocalWeapon->GetWeaponAmmo() == 0)
		return;

	if (pLocalWeapon->GetWeaponReload())
		return;

	if (pLocalPlayer->IsFlashed() && TriggerFlashCheck)
		return;

	if (!(pLocalPlayer->GetFlags() & FL_ONGROUND) && TriggerJumpCheck)
		return;

	Vector MyAngle = pCmd->viewangles + pLocalPlayer->GetPunchAngles();

	TriggerGetBestTarget(MyAngle);

	bool EnableRcs = false;
	bool CanFireChanse = false;

	bool CanFireDelay = I::GlobalVars()->curtime <= FireDelay;
	bool delay = false;
	if (CanFire(MyAngle, pCmd, pTriggerBestTarget, TriggerBestIdx, pLocalPlayer, Weapons[GetWeap(SelectedWeapon)].TriggerAllHitboxes))
	{
		CanFireChanse = (1 + (rand() % 101)) <= Weapons[GetWeap(SelectedWeapon)].TriggerChanse;
		if (CanFireChanse)
		{
			if (Weapons[GetWeap(SelectedWeapon)].TriggerDelay > 0 && !CanFireDelay)
			{
				if (!LastShoot)
					FireDelay = I::GlobalVars()->curtime + Weapons[GetWeap(SelectedWeapon)].TriggerDelay;

				LastShoot = true;
				CanFireDelay = I::GlobalVars()->curtime <= FireDelay;
			}

			if (CanFireDelay && LastShoot)
				delay = true;

			if (!delay)
			{
				EnableRcs = true;
				if (!(pCmd->buttons & IN_ATTACK))
				{
					pCmd->buttons |= IN_ATTACK;
				}
			}
		}
	}
	else
	{
		LastShoot = false;
	}

	TriggerRCS(Weapons[GetWeap(SelectedWeapon)].TriggerRcsX, Weapons[GetWeap(SelectedWeapon)].TriggerRcsY, pCmd, EnableRcs);
}
void AngleVectors(const Vector &vAngles, Vector& vForward)
{
	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(vAngles[1]));
	cy = cos(DEG2RAD(vAngles[1]));

	sp = sin(DEG2RAD(vAngles[0]));
	cp = cos(DEG2RAD(vAngles[0]));

	vForward.x = cp * cy;
	vForward.y = cp * sy;
	vForward.z = -sp;
}
bool __declspec(noinline) CLegitAim::CanFire(Vector mAngle, CUserCmd* pCmd, CBaseEntity* BaseEnt, int BestInd, CBaseEntity* Local, bool AllHitGroup)
{
	if (BestInd != -1 && BaseEnt)
	{
		trace_t Trace;
		Ray_t Ray;
		CTraceFilter Filter;

		Vector vecS = Local->GetRenderOrigin() + Local->GetViewOffset();
		Vector vecForvard;
		Vector vAngle = Vector(pCmd->viewangles.x + (Local->GetAimPunchAngle().x * 2),
			pCmd->viewangles.y + (Local->GetAimPunchAngle().y * 2), 0);

		AngleVectors(vAngle, vecForvard);
		vecForvard *= 8000.0f;

		Ray.Init(vecS, vecS + vecForvard);

		Filter.pSkip = Local;
		I::EngineTrace()->TraceRay(Ray, PlayerVisibleMask, &Filter, &Trace);

		auto CheckEnableHBox = [&](int HBox) -> bool
		{
			switch (HBox)
			{
			case 1: return Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_HEAD;
			case 2: return Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_CHEST;
			case 3: return Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_STOMACH;
			case 4: return Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_LEFTARM;
			case 5: return Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_RIGHTARM;
			case 6: return Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_LEFTLEG;
			case 7: return Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_RIGHTLEG;
			default: return false;
			}
		};

		if (string(Trace.surface.name).find("glass") != string::npos ||
			string(Trace.surface.name).find("GLASS") != string::npos ||
			string(Trace.surface.name).find("Glass") != string::npos)
		{

			trace_t New_Trace;
			Ray_t New_Ray;
			CTraceFilter New_Filter;

			New_Ray.Init(Trace.endpos, vecS + vecForvard);

			New_Filter.pSkip = Trace.m_pEnt;
			I::EngineTrace()->TraceRay(New_Ray, PlayerVisibleMask, &New_Filter, &New_Trace);

			if (New_Trace.m_pEnt == BaseEnt && (AllHitGroup ? true && New_Trace.hitgroup != 0 : CheckEnableHBox((New_Trace.hitgroup))))
				return true;
		}
		else
			if (Trace.m_pEnt == BaseEnt && (AllHitGroup ? true && Trace.hitgroup != 0 : CheckEnableHBox((Trace.hitgroup))))
				return true;

		return false;
	}
	return false;

}

void  CLegitAim::TriggerGetBestTarget(Vector mAngle)
{
	float bestFov = FLT_MAX;

	for (int i = 0; i < I::Engine()->GetMaxClients(); i++)
	{
		CBaseEntity* pBaseEntity = (CBaseEntity*)I::EntityList()->GetClientEntity(i);

		if (!pBaseEntity)
			continue;

		if (!pBaseEntity->IsValid())
			continue;

		if (pBaseEntity == pLocalPlayer)
			continue;

		if (!TriggerDeathmatch && pBaseEntity->GetTeam() == pLocalPlayer->GetTeam())
			continue;

		if (!(pBaseEntity->GetFlags() & FL_ONGROUND) && TriggerJumpEnemyCheck)
			continue;

		Vector vSrc = pLocalPlayer->GetEyePosition();

		Vector vEnd = pBaseEntity->GetHitboxPosition(0);

		if (!pBaseEntity->IsVisibleHitBox(pLocalPlayer, vSrc, vEnd, TriggerSmokeCheck, FlashCheck))
			continue;

		float fov = CalcFOV(mAngle, vSrc, vEnd);
		if (fov < bestFov)
		{
			bestFov = fov;
			TriggerBestIdx = i;
			pTriggerBestTarget = pBaseEntity;
		}
	}
}
Vector _OldAimPunch = Vector(0, 0, 0);
void CLegitAim::TriggerRCS(int X, int Y, CUserCmd* pCmd, bool Enable)
{
	if (pLocalPlayer)
	{
		if (Enable)
		{
			Vector AimPunch = (pLocalPlayer->GetPunchAngles()*(Vector(X, Y, 0) / 100.f)) * 2.f;

			Vector StartPunch = pCmd->viewangles;

			Vector EndPunch = _OldAimPunch - AimPunch;

			Vector BufPunch = StartPunch + EndPunch;

			CGlobal::ClampAngles(BufPunch);

			pCmd->viewangles = BufPunch;
			_OldAimPunch = AimPunch;
		}
		else
			_OldAimPunch = (pLocalPlayer->GetPunchAngles()*(Vector(X, Y, 0) / 100.f)) * 2.f;
	}
}
Vector AngleVector(QAngle meme)
{
	auto sy = sin(meme.y / 180.f * static_cast<float>(3.141592654f));
	auto cy = cos(meme.y / 180.f * static_cast<float>(3.141592654f));

	auto sp = sin(meme.x / 180.f * static_cast<float>(3.141592654f));
	auto cp = cos(meme.x / 180.f* static_cast<float>(3.141592654f));

	return Vector(cp*cy, cp*sy, -sp);
}

float DistancePointToLine(Vector Point, Vector LineOrigin, Vector Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.Dot(Dir) / (Dir.x*Dir.x + Dir.y*Dir.y + Dir.z*Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).Length();
}

void GetHitBoxSkeleton(int nHitBoxOne, int nHitBoxTwo, CBaseEntity* pEntity, Vector* vOut)
{
	vOut[0] = pEntity->GetHitboxPosition(nHitBoxOne);
	vOut[1] = pEntity->GetHitboxPosition(nHitBoxTwo);
}

void CLegitAim::BacktrackCreateMove(CUserCmd* pCmd)
{
	iBackTrackbestTarget = -1;

	if (SelectedWeapon < 0)
		return;

	if (Weapons[GetWeap(SelectedWeapon)].Backtrack && !FaceIt && Weapons[GetWeap(SelectedWeapon)].BacktrackTicks)
	{
		float bestFov = FLT_MAX;
		CBaseEntity* local = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());
		PlayerInfo info;
		for (int i = 0; i < SDK::I::Engine()->GetMaxClients(); i++)
		{
			auto entity = (CBaseEntity*)SDK::I::EntityList()->GetClientEntity(i);

			if (!entity || !local)
				continue;

			if (entity == local)
				continue;

			if (!I::Engine()->GetPlayerInfo(i, &info))
				continue;

			if (entity->IsDormant())
				continue;

			if (local->GetTeam() == entity->GetTeam())
				continue;

			if (!entity->IsDead())
			{
				float simtime = entity->GetSimTime();
				Vector hitboxPos = entity->GetHitboxPosition(0 & 3 & 4 & 5 & 6 & 7 & 8 & 9 & 16 & 17 & 18 & 19 & 10 & 11);
				backtrackData bfg = { simtime, hitboxPos };

				if (ShowBacktrack && Weapons[GetWeap(SelectedWeapon)].BacktrackTicks)
				{
					Vector	vHitboxSkeletonArrayBuf[18][2];

					// Body
					GetHitBoxSkeleton(HITBOX_HEAD, HITBOX_NECK, entity, vHitboxSkeletonArrayBuf[0]);
					GetHitBoxSkeleton(HITBOX_NECK, HITBOX_UPPER_CHEST, entity, vHitboxSkeletonArrayBuf[1]);
					GetHitBoxSkeleton(HITBOX_UPPER_CHEST, HITBOX_THORAX, entity, vHitboxSkeletonArrayBuf[2]);
					GetHitBoxSkeleton(HITBOX_THORAX, HITBOX_BELLY, entity, vHitboxSkeletonArrayBuf[3]);

					// Right leg
					GetHitBoxSkeleton(HITBOX_BELLY, HITBOX_LEFT_THIGH, entity, vHitboxSkeletonArrayBuf[4]);
					GetHitBoxSkeleton(HITBOX_LEFT_THIGH, HITBOX_LEFT_CALF, entity, vHitboxSkeletonArrayBuf[5]);
					GetHitBoxSkeleton(HITBOX_LEFT_CALF, HITBOX_LEFT_FOOT, entity, vHitboxSkeletonArrayBuf[6]);

					// Left leg
					GetHitBoxSkeleton(HITBOX_BELLY, HITBOX_RIGHT_THIGH, entity, vHitboxSkeletonArrayBuf[7]);
					GetHitBoxSkeleton(HITBOX_RIGHT_THIGH, HITBOX_RIGHT_CALF, entity, vHitboxSkeletonArrayBuf[8]);
					GetHitBoxSkeleton(HITBOX_RIGHT_CALF, HITBOX_RIGHT_FOOT, entity, vHitboxSkeletonArrayBuf[9]);

					// Right arm
					GetHitBoxSkeleton(HITBOX_UPPER_CHEST, HITBOX_LEFT_UPPER_ARM, entity, vHitboxSkeletonArrayBuf[10]);
					GetHitBoxSkeleton(HITBOX_LEFT_UPPER_ARM, HITBOX_LEFT_FOREARM, entity, vHitboxSkeletonArrayBuf[11]);
					GetHitBoxSkeleton(HITBOX_LEFT_FOREARM, HITBOX_LEFT_HAND, entity, vHitboxSkeletonArrayBuf[12]);

					// Left arm
					GetHitBoxSkeleton(HITBOX_UPPER_CHEST, HITBOX_RIGHT_UPPER_ARM, entity, vHitboxSkeletonArrayBuf[13]);
					GetHitBoxSkeleton(HITBOX_RIGHT_UPPER_ARM, HITBOX_RIGHT_FOREARM, entity, vHitboxSkeletonArrayBuf[14]);
					GetHitBoxSkeleton(HITBOX_RIGHT_FOREARM, HITBOX_RIGHT_HAND, entity, vHitboxSkeletonArrayBuf[15]);

					for (int h(0); h < 18; h++)
					{
						bfg.vHitboxSkeletonArray[h][0] = vHitboxSkeletonArrayBuf[h][0];
						bfg.vHitboxSkeletonArray[h][1] = vHitboxSkeletonArrayBuf[h][1];
					}
				}

				if (Weapons[GetWeap(SelectedWeapon)].BacktrackTicks)
				{
					headPositions[i][pCmd->command_number % MAXBACKTRACKTICKS] = bfg;

					Vector ViewDir = AngleVector(pCmd->viewangles + (local->GetAimPunchAngle() * 2.f));
					float FOVDistance = DistancePointToLine(hitboxPos, local->GetEyePosition(), ViewDir);

					if (bestFov > FOVDistance)
					{
						bestFov = FOVDistance;
						iBackTrackbestTarget = i;
						pBestBacktrTarget = entity;
					}
				}
			}
		}

		float bestTargetSimTime = 0;
		if (iBackTrackbestTarget != -1)
		{
			float tempFloat = FLT_MAX;

			Vector ViewDir = AngleVector(pCmd->viewangles + (local->GetAimPunchAngle() * 2.f));

			for (int t = 0; t <= MAXBACKTRACKTICKS - 1; ++t)
			{
				float tempFOVDistance = DistancePointToLine(headPositions[iBackTrackbestTarget][t].hitboxPos, local->GetEyePosition(), ViewDir);

				if (tempFloat > tempFOVDistance && headPositions[iBackTrackbestTarget][t].simtime > I::GlobalVars()->curtime - 1)
				{
					tempFloat = tempFOVDistance;
					bestTargetSimTime = headPositions[iBackTrackbestTarget][t].simtime;
					iBackTrackBestSimTime = bestTargetSimTime;
				}
			}

			if (pCmd->buttons & IN_ATTACK)
			{
				pCmd->tick_count = TIME_TO_TICKS(bestTargetSimTime);
			}
		}
	}
}

void CLegitAim::SaveWeapons(nlohmann::json &j)
{
#define SV(a,b) j[("LegitAim")][("AimWeapons")][v.WeaponName][(a)] = b;


	for (int i(0); i < (int)CustomSubs.size(); i++)
	{
		j[("LegitAim")][("Subsections")][("SubsNames")].push_back(CustomSubs[i].Name);
		for (int g(0); g < (int)CustomSubs[i].AllocatedWeapon.size(); g++)
			j[("LegitAim")][("Subsections")][CustomSubs[i].Name].push_back(CustomSubs[i].AllocatedWeapon[g]);
	}

	WeaponSettings emptyEntry;

	for (auto &v : Weapons)
	{
		if (emptyEntry == v)
		{
			SV("Emp", true);
			continue;
		}

		SV("IsCustomSub", v.IsCustomSub);
		SV("Silent", v.Silent);
		SV("SilentFov", v.SilentFov);
		SV("SilentHitBox", v.SilentHitBox);
		SV("SilentNearest", v.SilentNearest);
		SV("SilentHitchance", v.SilentHitchance);
		SV("SilentStartBullet", v.SilentStartBullet);
		SV("SilentEndBullet", v.SilentEndBullet);
		SV("FovPos", v.FovPos);
		SV("Smooth", v.Smooth);
		SV("Fov", v.Fov);
		SV("HitBox", v.HitBox);
		SV("Nearest", v.Nearest);
		SV("FirstFireDelayEnable", v.FirstFireDelayEnable);
		SV("FirstFireDelayAuto", v.FirstFireDelayAuto);
		SV("FirstFireDelaySilentAuto", v.FirstFireDelaySilentAuto);
		SV("FirstFireDelayOneClick", v.FirstFireDelayOneClick);
		SV("FirstFireDelay", v.FirstFireDelay);
		SV("TargetSwitchDelayEnable", v.TargetSwitchDelayEnable);
		SV("TargetSwitchDelay", v.TargetSwitchDelay);
		SV("KillStop", v.KillStop);
		SV("EndBullet", v.EndBullet);
		SV("RcsX", v.RcsX);
		SV("RcsY", v.RcsY);
		SV("SmoothRcs", v.SmoothRcs);
		SV("FovRcs", v.FovRcs);
		SV("HitBoxAfter1B", v.HitBoxAfter1B);
		SV("HitBoxRcs", v.HitBoxRcs);
		SV("NearestRcs", v.NearestRcs);
		SV("RcsStandelone", v.RcsStandelone);
		SV("AutoPistol", v.AutoPistol);
		SV("FastZoom", v.FastZoom);
		SV("TRIG_HITGROUP_HEAD", v.TRIG_HITGROUP_HEAD);
		SV("TRIG_HITGROUP_CHEST", v.TRIG_HITGROUP_CHEST);
		SV("TRIG_HITGROUP_STOMACH", v.TRIG_HITGROUP_STOMACH);
		SV("TRIG_HITGROUP_LEFTARM", v.TRIG_HITGROUP_LEFTARM);
		SV("TRIG_HITGROUP_RIGHTARM", v.TRIG_HITGROUP_RIGHTARM);
		SV("TRIG_HITGROUP_LEFTLEG", v.TRIG_HITGROUP_LEFTLEG);
		SV("TRIG_HITGROUP_RIGHTLEG", v.TRIG_HITGROUP_RIGHTLEG);
		SV("TriggerAllHitboxes", v.TriggerAllHitboxes);
		SV("TriggerChanse", v.TriggerChanse);
		SV("TriggerRcsX", v.TriggerRcsX);
		SV("TriggerRcsY", v.TriggerRcsY);
		SV("TriggerDelay", v.TriggerDelay);
		SV("Backtrack", v.Backtrack);
		SV("BacktrackTicks", v.BacktrackTicks);
		SV("SmoothMoveFactor", v.SmoothMoveFactor);
	}
}

void CLegitAim::LoadWeapons(nlohmann::json &j)
{
#define LV(a,b) if(!j[("LegitAim")][("AimWeapons")][v.WeaponName][(a)].is_null()){ b = j[("LegitAim")][("AimWeapons")][v.WeaponName][(a)];}

	if (!CustomSubs.empty())
		CustomSubs.clear();

	Weapons.clear();

	ReinitWeapons();


	if (!j[("LegitAim")].is_null())
	{
		if (!j[("LegitAim")][("AimWeapons")].is_null())
		{
			if (!j[("LegitAim")][("Subsections")].is_null())
			{
				if (!j[("LegitAim")][("Subsections")][("SubsNames")].is_null())
				{
					for (size_t i(0); i < j[("LegitAim")][("Subsections")][("SubsNames")].size(); i++)
					{
						CustomSub SubEntry;
						if (!j[("LegitAim")][("Subsections")][("SubsNames")].at(i).is_null())
						{
							SubEntry.Name = j[("LegitAim")][("Subsections")][("SubsNames")].at(i).get<string>();
							SubEntry.Idx = Weapons.size();

							for (size_t g(0); g < j[("LegitAim")][("Subsections")][SubEntry.Name].size(); g++)
								if (!j[("LegitAim")][("Subsections")][SubEntry.Name].at(g).is_null())
									SubEntry.AllocatedWeapon.push_back(j[("LegitAim")][("Subsections")][SubEntry.Name].at(g).get<string>());

							WeaponSettings WeaponEntry;
							WeaponEntry.WeaponName = SubEntry.Name;

							CustomSubs.push_back(SubEntry);
							Weapons.push_back(WeaponEntry);
						}
					}

					if (!UnallocatedWeapons.empty())
						UnallocatedWeapons.clear();

					for (int i(0); i < 34; i++)
					{
						// Weapon names
						bool Finded = false;
						for (size_t j(0); j < CustomSubs.size(); j++)
						{
							for (size_t l(0); l < CustomSubs[j].AllocatedWeapon.size(); l++)
							{
								if (CustomSubs[j].AllocatedWeapon[l] == CGlobal::WeaponNames[i])
								{
									Finded = true;
									break;
								}
							}
						}
						if (!Finded)
							UnallocatedWeapons.push_back(CGlobal::WeaponNames[i]);
					}
				}
			}
			WeaponSettings emptyEntry;
			for (auto &v : Weapons)
			{

				bool IsEmpty = false;
				LV("Emp", IsEmpty);

				if (IsEmpty)
				{
					string nn = v.WeaponName;
					v = emptyEntry;
					v.WeaponName = nn;
					continue;
				}

				LV("IsCustomSub", v.IsCustomSub);
				LV("Silent", v.Silent);
				LV("SilentFov", v.SilentFov);
				LV("SilentHitBox", v.SilentHitBox);
				LV("SilentNearest", v.SilentNearest);
				LV("SilentHitchance", v.SilentHitchance);
				LV("SilentStartBullet", v.SilentStartBullet);
				LV("SilentEndBullet", v.SilentEndBullet);
				LV("FovPos", v.FovPos);
				LV("Smooth", v.Smooth);
				LV("Fov", v.Fov);
				LV("HitBox", v.HitBox);
				LV("Nearest", v.Nearest);
				LV("FirstFireDelayEnable", v.FirstFireDelayEnable);
				LV("FirstFireDelayAuto", v.FirstFireDelayAuto);
				LV("FirstFireDelaySilentAuto", v.FirstFireDelaySilentAuto);
				LV("FirstFireDelayOneClick", v.FirstFireDelayOneClick);
				LV("FirstFireDelay", v.FirstFireDelay);
				LV("TargetSwitchDelayEnable", v.TargetSwitchDelayEnable);
				LV("TargetSwitchDelay", v.TargetSwitchDelay);
				LV("KillStop", v.KillStop);
				LV("EndBullet", v.EndBullet);
				LV("RcsX", v.RcsX);
				LV("RcsY", v.RcsY);
				LV("SmoothRcs", v.SmoothRcs);
				LV("FovRcs", v.FovRcs);
				LV("HitBoxAfter1B", v.HitBoxAfter1B);
				LV("HitBoxRcs", v.HitBoxRcs);
				LV("NearestRcs", v.NearestRcs);
				LV("RcsStandelone", v.RcsStandelone);
				LV("AutoPistol", v.AutoPistol);
				LV("FastZoom", v.FastZoom);
				LV("TRIG_HITGROUP_HEAD", v.TRIG_HITGROUP_HEAD);
				LV("TRIG_HITGROUP_CHEST", v.TRIG_HITGROUP_CHEST);
				LV("TRIG_HITGROUP_STOMACH", v.TRIG_HITGROUP_STOMACH);
				LV("TRIG_HITGROUP_LEFTARM", v.TRIG_HITGROUP_LEFTARM);
				LV("TRIG_HITGROUP_RIGHTARM", v.TRIG_HITGROUP_RIGHTARM);
				LV("TRIG_HITGROUP_LEFTLEG", v.TRIG_HITGROUP_LEFTLEG);
				LV("TRIG_HITGROUP_RIGHTLEG", v.TRIG_HITGROUP_RIGHTLEG);
				LV("TriggerAllHitboxes", v.TriggerAllHitboxes);
				LV("TriggerChanse", v.TriggerChanse);
				LV("TriggerRcsX", v.TriggerRcsX);
				LV("TriggerRcsY", v.TriggerRcsY);
				LV("TriggerDelay", v.TriggerDelay);
				LV("Backtrack", v.Backtrack);
				LV("BacktrackTicks", v.BacktrackTicks);
				LV("SmoothMoveFactor", v.SmoothMoveFactor);
			}
		}
	}
}