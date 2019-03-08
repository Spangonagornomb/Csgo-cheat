#pragma once
#include <Windows.h>
#include <string>
#include <map>


extern std::map<std::string, DWORD> offsets;


namespace Offsets
{
	extern DWORD m_hMyWeapons;
	extern DWORD m_hMyWearables;
	extern DWORD m_hViewModel;
	extern DWORD m_nTickBase;
	extern DWORD m_flFlashMaxAlpha;
	extern DWORD m_iObserverMode;
	extern DWORD m_hObserverTarget;
	extern DWORD m_vecOrigin;
	extern DWORD deadflag;

	extern DWORD m_bIsDefusing;

	extern DWORD m_lifeState;
	extern DWORD m_flFlashDuration;

	extern DWORD m_iAccount;

	extern DWORD m_bHasHelmet;
	extern DWORD m_bHasDefuser;

	extern DWORD m_flDefuseCountDown;
	extern DWORD m_flC4Blow;
	extern DWORD m_hBombDefuser;
	extern DWORD m_bBombDefused;

	extern DWORD m_bIsScoped;

	extern DWORD m_iFOVStart;
	extern DWORD m_fFlags;
	extern DWORD m_iHealth;
	extern DWORD m_ArmorValue;
	extern DWORD m_iTeamNum;
	extern DWORD m_iShotsFired;

	extern DWORD m_aimPunchAngle;
	extern DWORD m_viewPunchAngle;

	extern DWORD m_flSimulationTime;

	extern DWORD m_vecVelocity;
	extern DWORD m_vecViewOffset;
	extern DWORD m_vecPunchAngles;

	extern DWORD m_angEyeAngles;

	extern DWORD m_hActiveWeapon;

	extern DWORD m_iClip1;
	extern DWORD m_iClip2;
	extern DWORD m_flNextPrimaryAttack;
	extern DWORD m_iWorldModelIndex;
	extern DWORD m_bCanReload;

	extern DWORD m_iItemDefinitionIndex;
	extern DWORD m_iItemIDHigh;
	extern DWORD m_iAccountID;
	extern DWORD m_iEntityQuality;
	extern DWORD m_OriginalOwnerXuidLow;
	extern DWORD m_OriginalOwnerXuidHigh;
	extern DWORD m_nFallbackPaintKit;
	extern DWORD m_flFallbackWear;
	extern DWORD m_nFallbackSeed;
	extern DWORD m_nFallbackStatTrak;
	extern DWORD m_bIsAutoaimTarget;

	extern DWORD m_szCustomName;
	extern DWORD m_hWeaponWorldModel;

	extern DWORD m_nModelIndex;
	extern DWORD  m_iViewModelIndex;
	extern DWORD m_hOwner;
	extern DWORD m_hWeapon;
	extern DWORD m_bSpotted;

	extern DWORD m_zoomLevel;

	extern DWORD KeyValues_KeyValues;
	extern DWORD KeyValues_LoadFromBuffer;

	extern DWORD m_CollisionGroup;
	extern DWORD m_rgflCoordinateFrame;

	extern DWORD m_vecMins;
	extern DWORD m_vecMaxs;


	extern float* FindW2Matrix();
}

