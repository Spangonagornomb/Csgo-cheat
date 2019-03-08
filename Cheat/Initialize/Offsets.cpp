#include "Offsets.h"
#include <map>
#include <string>

std::map<std::string, DWORD> offsets;

namespace Offsets
{
	DWORD m_hMyWeapons = 0;
	DWORD m_hMyWearables = 0;
	DWORD m_hViewModel = 0;
	DWORD m_nTickBase = 0;
	DWORD m_flFlashMaxAlpha = 0;
	DWORD m_iObserverMode = 0;
	DWORD m_hObserverTarget = 0;

	DWORD deadflag = 0;

	DWORD m_lifeState = 0;
	DWORD m_flFlashDuration = 0;

	DWORD m_iAccount = 0;

	DWORD m_bHasHelmet = 0;
	DWORD m_bHasDefuser = 0;

	DWORD m_flDefuseCountDown = 0;
	DWORD m_flC4Blow = 0;
	DWORD m_hBombDefuser = 0;
	DWORD m_bBombDefused = 0;

	DWORD m_bIsScoped = 0;

	DWORD m_iFOVStart = 0;
	DWORD m_fFlags = 0;

	DWORD m_iHealth = 0;
	DWORD m_ArmorValue = 0;
	DWORD m_iTeamNum = 0;
	DWORD m_iShotsFired = 0;

	DWORD m_aimPunchAngle = 0;
	DWORD m_viewPunchAngle = 0;


	DWORD m_flSimulationTime = 0;


	DWORD m_vecVelocity = 0;
	DWORD m_vecViewOffset = 0;
	DWORD m_vecPunchAngles = 0;

	DWORD m_vecOrigin = 0;

	DWORD m_bIsDefusing = 0;

	DWORD m_angEyeAngles = 0;

	DWORD m_hActiveWeapon = 0;

	DWORD m_iClip1 = 0;
	DWORD m_iClip2 = 0;
	DWORD m_flNextPrimaryAttack = 0;
	DWORD m_iWorldModelIndex = 0;
	DWORD m_bCanReload = 0;

	DWORD m_iItemDefinitionIndex = 0;
	DWORD m_iItemIDHigh = 0;
	DWORD m_iAccountID = 0;
	DWORD m_iEntityQuality = 0;
	DWORD m_OriginalOwnerXuidLow = 0;
	DWORD m_OriginalOwnerXuidHigh = 0;
	DWORD m_nFallbackPaintKit = 0;
	DWORD m_flFallbackWear = 0;
	DWORD m_nFallbackSeed = 0;
	DWORD m_nFallbackStatTrak = 0;
	DWORD m_szCustomName = 0;
	DWORD m_hWeaponWorldModel = 0;
	DWORD m_bIsAutoaimTarget = 0;

	DWORD m_nModelIndex = 0;
	DWORD m_iViewModelIndex = 0;
	DWORD m_hOwner = 0;
	DWORD m_hWeapon = 0;
	DWORD m_bSpotted = 0;

	DWORD m_zoomLevel = 0;

	DWORD KeyValues_KeyValues = 0;
	DWORD KeyValues_LoadFromBuffer = 0;

	DWORD m_CollisionGroup = 0;
	DWORD m_rgflCoordinateFrame = 0;

	DWORD m_vecMins = 0;
	DWORD m_vecMaxs = 0;
}