#pragma once
#include "../Include/Def.h"
#include "../Hooks/Render.h"
#include "../Hooks/Tables.h"
#include "../Engine/CSX/CSX_Memory.h"
#include "../Engine/CSX/CSX_Utils.h"
#include "../Engine/SDK/SDK.h"
#include "../Engine/NetVar.h"
#include "../Hooks/CreateMove.h"
#include "../Hooks/Views.h"
#include  "../Hooks/Sound.h"
#include "../Hooks/FrameStageNotify.h"
#include "../Hooks/DrawModelExecute.h"
#include "../Hacks/Setup.h"
#include "../License/License.h"
#include "Offsets.h"

using namespace SDK;
using namespace Engine;

class IIInit
{
public:
	virtual class IInit
	{
	public:
		virtual void InitHooks() = 0;
		virtual void InitOffsets() = 0;
		virtual bool Init() = 0;
		virtual void SendSteamId32() = 0;
		int ii = 0;
	};
};

inline int decod(const char *s)
{
	int z = 0, n = 0;
	while (isspace(*s)) s++;
	if (*s == '-') { z = 1; s++; }
	else if (*s == '+') s++;
	while (isdigit(*s)) n = 10 * n + *s++ - '0';
	return (z ? -n : n);
}

#define ResetIdx ("16")
#define CreateMoveIdx ("24")
#define OverrideViewIdx ("18")
#define GetViewModelFOVIdx ("35")
#define SSendMessageIdx ("0")
#define RetrieveMessageIdx ("2")
#define FireEventClientSideThinkIdx ("9")
#define FrameStageNotifyIdx ("37")
#define EmitSound1Idx ("5")
#define EmitSound2Idx ("6")
#define DrawModelExecuteIdx ("21")
#define PlaySoundIdx ("82")
#define LockCursorIdx ("67")
#define ResetIdx ("16")

using namespace HookTables;

class CInit : public IIInit
{
public:
	virtual class IInit
	{
	public:
		virtual void InitHooks()
		{
			auto LInitHooks = [&]() -> void
			{
				ADD_LOG("======================Init hooks:\n");
				ADD_LOG("2-1-11-0\n");
				auto& pContext = cContext::GetInstance();
				ADD_LOG("2-1-11-1\n");
				PVOID* SurfaceTable = *reinterpret_cast<PVOID**>(I::Surface());
				PVOID* ClientModeTable = *reinterpret_cast<PVOID**>(I::ClientMode());
				PVOID* ClientTable = *reinterpret_cast<PVOID**>(I::Client());
				PVOID* ModelRenderTable = *reinterpret_cast<PVOID**>(I::ModelRender());
				ADD_LOG("2-1-11-2\n");
#ifndef ONLY_DRAW_HOOK
				ADD_LOG("2-1-11-3\n");
				if (SurfaceTable)
				{
					pContext.ApplyDetour<PlaySoundFn>(static_cast<PlaySoundFn>(SurfaceTable[82]),
						reinterpret_cast<PlaySoundFn>
						(PlaySound),
						&pPlaySound);
					ADD_LOG("Hook: Sound\n");

					pContext.ApplyDetour<LockCursorFn>(static_cast<LockCursorFn>(SurfaceTable[67]),
						reinterpret_cast<LockCursorFn>
						(LockCursor),
						&pLockCursor);
					ADD_LOG("Hook: Cursor\n");
				}
				ADD_LOG("2-1-11-4\n");

				ADD_LOG("2-1-11-5\n");
				if (ClientModeTable)
				{
					pContext.ApplyDetour<CreateMoveFn>(static_cast<CreateMoveFn>(ClientModeTable[24]),
						reinterpret_cast<CreateMoveFn>
						(CreateMove),
						&pCreateMove);
					ADD_LOG("Hook: CreateMove\n");

					pContext.ApplyDetour<OverrideViewFn>(static_cast<OverrideViewFn>(ClientModeTable[18]),
						reinterpret_cast<OverrideViewFn>
						(OverrideView),
						&pOverrideView);
					ADD_LOG("Hook: OverrideView\n");

					pContext.ApplyDetour<GetViewModelFOVFn>(static_cast<GetViewModelFOVFn>(ClientModeTable[35]),
						reinterpret_cast<GetViewModelFOVFn>
						(GetViewModelFOV),
						&pGetViewModelFOV);
					ADD_LOG("Hook: OverrideView\n");
				}
				ADD_LOG("2-1-11-6\n");
				if (ClientTable)
				{
					pContext.ApplyDetour<FrameStageNotifyFn>(static_cast<FrameStageNotifyFn>(ClientTable[37]),
						reinterpret_cast<FrameStageNotifyFn>
						(FrameStageNotify),
						&pFrameStageNotify);
					ADD_LOG("Hook: StageNotify\n");
				}

				if (ModelRenderTable)
				{
					pContext.ApplyDetour<DrawModelExecuteFn>(static_cast<DrawModelExecuteFn>(ModelRenderTable[21]),
						reinterpret_cast<DrawModelExecuteFn>
						(DrawModelExecute),
						&pDrawModelExecute);
					ADD_LOG("Hook: DME\n");
				}

#endif
				ADD_LOG("2-1-11-7\n");
				GP_Setup = new CSetup::ISetup();
				GP_Setup->Setup();
				ADD_LOG("2-1-11-8\n");
				ADD_LOG("All Hooks sucessful\n");
			};
			LInitHooks();
			ADD_LOG("2-1-11-9\n");
		}
		virtual void InitOffsets()
		{
#define mGetOffset(table, prop) g_NetVar.GetOffset((table), (prop))
			auto LInitOffsets = [&]() -> void
			{
				ADD_LOG("2-1-9-0\n");
				ADD_LOG("======================Init Offsets:\n");

				offsets["m_hMyWeapons"] = mGetOffset("DT_BasePlayer", "m_hActiveWeapon") / 2;
				offsets["m_hMyWearables"] = mGetOffset("DT_BasePlayer", "m_hMyWearables");
				offsets["m_vecOrigin"] = mGetOffset("DT_BasePlayer", "m_vecOrigin");
				offsets["m_hViewModel"] = mGetOffset("DT_BasePlayer", "m_hViewModel[0]");
				offsets["m_nTickBase"] = mGetOffset("DT_BasePlayer", "m_nTickBase");
				offsets["m_iObserverMode"] = mGetOffset("DT_BasePlayer", "m_iObserverMode");
				offsets["m_hObserverTarget"] = mGetOffset("DT_BasePlayer", "m_hObserverTarget");
				offsets["deadflag"] = mGetOffset("DT_BasePlayer", "deadflag");
				offsets["m_bIsDefusing"] = mGetOffset("DT_CSPlayer", "m_bIsDefusing");
				offsets["m_iAccount"] = mGetOffset("DT_CSPlayer", "m_iAccount");
				offsets["m_lifeState"] = mGetOffset("DT_CSPlayer", "m_lifeState");
				offsets["m_flFlashDuration"] = mGetOffset("DT_CSPlayer", "m_flFlashDuration");
				offsets["m_flC4Blow"] = mGetOffset("DT_PlantedC4", "m_flC4Blow");
				offsets["m_flDefuseCountDown"] = mGetOffset("DT_PlantedC4", "m_flDefuseCountDown");
				offsets["m_hBombDefuser"] = mGetOffset("DT_PlantedC4", "m_hBombDefuser");
				offsets["m_bBombDefused"] = mGetOffset("DT_PlantedC4", "m_bBombDefused");
				offsets["m_flFlashMaxAlpha"] = mGetOffset("DT_CSPlayer", "m_flFlashMaxAlpha");
				offsets["m_bHasHelmet"] = mGetOffset("DT_CSPlayer", "m_bHasHelmet");
				offsets["m_bHasDefuser"] = mGetOffset("DT_CSPlayer", "m_bHasDefuser");
				offsets["m_bIsScoped"] = mGetOffset("DT_CSPlayer", "m_bIsScoped");
				offsets["m_iFOVStart"] = mGetOffset("DT_CSPlayer", "m_iFOVStart");
				offsets["m_fFlags"] = mGetOffset("DT_CSPlayer", "m_fFlags");
				offsets["m_iHealth"] = mGetOffset("DT_BasePlayer", "m_iHealth");
				offsets["m_ArmorValue"] = mGetOffset("DT_CSPlayer", "m_ArmorValue");
				offsets["m_iTeamNum"] = mGetOffset("DT_BasePlayer", "m_iTeamNum");
				offsets["m_iShotsFired"] = mGetOffset("DT_CSPlayer", "m_iShotsFired");
				offsets["m_aimPunchAngle"] = mGetOffset("DT_BasePlayer", "m_aimPunchAngle");
				offsets["m_viewPunchAngle"] = mGetOffset("DT_BasePlayer", "m_viewPunchAngle");
				offsets["m_vecVelocity"] = mGetOffset("DT_CSPlayer", "m_vecVelocity[0]");
				offsets["m_vecPunchAngles"] = mGetOffset("DT_BasePlayer", "m_aimPunchAngle");
				offsets["m_vecViewOffset"] = mGetOffset("DT_CSPlayer", "m_vecViewOffset[0]");
				offsets["m_angEyeAngles"] = mGetOffset("DT_CSPlayer", "m_angEyeAngles");
				offsets["m_hActiveWeapon"] = mGetOffset("DT_BasePlayer", "m_hActiveWeapon");
				offsets["m_iClip1"] = mGetOffset("DT_BaseCombatWeapon", "m_iClip1");
				offsets["m_iClip2"] = mGetOffset("DT_BaseCombatWeapon", "m_iPrimaryReserveAmmoCount");
				offsets["m_flNextPrimaryAttack"] = mGetOffset("DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
				offsets["m_bCanReload"] = offsets["m_flNextPrimaryAttack"] + 0x6D;
				offsets["m_iItemDefinitionIndex"] = mGetOffset("DT_BaseCombatWeapon", "m_iItemDefinitionIndex");
				offsets["m_bIsAutoaimTarget"] = mGetOffset("DT_BaseEntity", "m_bIsAutoaimTarget");
				offsets["m_iItemIDHigh"] = mGetOffset("DT_BaseAttributableItem", "m_iItemIDHigh");
				offsets["m_iAccountID"] = mGetOffset("DT_BaseAttributableItem", "m_iAccountID");
				offsets["m_iEntityQuality"] = mGetOffset("DT_BaseAttributableItem", "m_iEntityQuality");
				offsets["m_OriginalOwnerXuidLow"] = mGetOffset("DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
				offsets["m_OriginalOwnerXuidHigh"] = mGetOffset("DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
				offsets["m_nFallbackPaintKit"] = mGetOffset("DT_BaseAttributableItem", "m_nFallbackPaintKit");
				offsets["m_flFallbackWear"] = mGetOffset("DT_BaseAttributableItem", "m_flFallbackWear");
				offsets["m_nFallbackSeed"] = mGetOffset("DT_BaseAttributableItem", "m_nFallbackSeed");
				offsets["m_szCustomName"] = mGetOffset("DT_BaseAttributableItem", "m_szCustomName");
				offsets["m_nFallbackStatTrak"] = mGetOffset("DT_BaseAttributableItem", "m_nFallbackStatTrak");
				offsets["m_nModelIndex"] = 0x258;
				offsets["m_iViewModelIndex"] = 0x3220;
				offsets["m_hOwner"] = mGetOffset("DT_PredictedViewModel", "m_hOwner");
				offsets["m_hWeapon"] = mGetOffset("DT_BaseViewModel", "m_hWeapon");
				offsets["m_bSpotted"] = mGetOffset("DT_BaseEntity", "m_bSpotted");
				offsets["m_zoomLevel"] = mGetOffset("DT_WeaponAWP", "m_zoomLevel");
				offsets["m_flSimulationTime"] = mGetOffset("DT_BaseEntity", "m_flSimulationTime");
				offsets["m_hWeaponWorldModel"] = mGetOffset("DT_BaseCombatWeapon", "m_hWeaponWorldModel");
				offsets["m_iWorldModelIndex"] = mGetOffset("DT_BaseCombatWeapon", "m_iWorldModelIndex");
				ADD_LOG("2-1-9-1\n");
				offsets["KeyValues_KeyValues"] = CSX::Memory::FindPatternV2(GetClientDll(), KEY_VALUES_MASK);
				offsets["KeyValues_LoadFromBuffer"] = CSX::Memory::FindPatternV2(GetClientDll(), KEY_VALUES_LOAD_FROM_BUFFER_MASK);
				ADD_LOG("All Offsets sucessful\n");
				ADD_LOG("2-1-9-2\n");
			};
			LInitOffsets();
			ADD_LOG("2-1-9-3\n");
		}

		vector<void*> InterfaceList;
		vector<void*> ModList;
		using getValue = DWORD(__stdcall*)(void);
		using getValueModule = bool(_cdecl*)(PCHAR, DWORD);

		
		virtual bool Init()
		{
			auto LInit = [&]() -> bool
			{
#ifdef ENABLE_LICENSE
				ADD_LOG("2-1-0\n");
				try
				{
					CLicense::GetCheatUser();
					CLic1::ILic2* dab = new CLic1::ILic2();
					ADD_LOG("2-1-1\n");
					if (!dab)
					{
						ADD_LOG("2-1-1-1\n");
						FastCall::G().t_Beep(100, 100);
						return false;
					}

					ADD_LOG("2-1-2\n");
					if (!dab->Check())
					{
						ADD_LOG("2-1-2-1\n");
						FastCall::G().t_Beep(100, 100);
						FastCall::G().t_Beep(100, 100);
						FastCall::G().t_Beep(100, 100);

						return false;
					}
					ADD_LOG("2-1-3\n");
					if (!CLicense::CheckMemes())
					{
						ADD_LOG("2-1-3-1\n");
						FastCall::G().t_Beep(100, 100);
						FastCall::G().t_Beep(100, 100);
						return false;
					}

				}
				catch (const exception& e)
				{
				}
#endif

#ifndef ONLY_DRAW_HOOK
				ADD_LOG("2-1-4\n");
				
				ADD_LOG("2-1-5\n");
				if (!CSX::Utils::IsModuleLoad(ENGINE_DLL, 5001))
					return false;
				if (!CSX::Utils::IsModuleLoad(VGUI2_DLL, 5001))
					return false;
				if (!CSX::Utils::IsModuleLoad(VGUIMAT_DLL, 5001))
					return false;
				if (!CSX::Utils::IsModuleLoad(VSTDLIB_DLL, 5001))
					return false;
				if (!CSX::Utils::IsModuleLoad(STEAMAPI_DLL, 5001))
					return false;
				if (!CSX::Utils::IsModuleLoad(SERVERBROWSER_DLL, 40000))
					return false;

				FastCall::G().t_Sleep(1500);

				if (!CSX::Utils::IsModuleLoad("client_panorama.dll", 5001))
					return false;

				ADD_LOG("2-1-8\n");
				if (!g_NetVar.Init(I::Client()->GetAllClasses()))
				{
					ADD_LOG("2-1-9-8-1\n");
					FastCall::G().t_MessageBoxA(0, "AllClasses error", "Error!", 0);
					return false;
				}
				ADD_LOG("2-1-9\n");
				InitOffsets();
				ADD_LOG("2-1-10\n");
				//SendSteamId32();
#endif 
				ADD_LOG("2-1-11\n");
				InitHooks();
				return true;
			};
			return LInit();
		}
	};
};