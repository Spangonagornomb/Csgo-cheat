#pragma once

#include "Engine.h"

namespace SDK
{
	class IClientEntity;
}

using namespace SDK;

namespace Engine
{
	class CBaseAttributableItem;
	class CBaseViewModel;

	class CBaseWeapon : public IClientEntity
	{
	public:

		int				GetWeaponId();
		int				GetWeaponType();
		int				GetWeaponAmmo();
		int             GetWeaponAmmoClp2();
		int				GetZoomLevel();
		bool			GetWeaponReload();
		float			GetNextPrimaryAttack();
		float GetInaccuracy();
		HANDLE* GetWeaponWorldModel();
		int* ModelIndex();
		int* ViewModelIndex();
		int* fixItemIDHigh();
		short* fixskins();
		int* OwnerXuidLow();
		int* OwnerXuidHigh();
		CWeaponInfo*			GetWeaponInfo();
		CBaseAttributableItem*	GeteAttributableItem();
		int* GetEntityQuality();

		const char*		GetName();
		const char*		GetPrintName();

	};

	class CBaseAttributableItem : public IClientEntity
	{
	public:

		short*			GetItemDefinitionIndex();
		int*			GetItemIDHigh();
		int*			fixItemIDHigh();
		int*			GetAccountID();
		int*			GetEntityQuality();
		int*			GetOriginalOwnerXuidLow();
		int*			GetOriginalOwnerXuidHigh();
		int*			GetFallbackPaintKit();
		float*			GetFallbackWear();
		int*			GetFallbackSeed();
		int*			GetFallbackStatTrak();
		char*			GetCustomName();
		CBaseViewModel*	GetViewModel();
		int*			GetIndex();
		

		int* ModelIndex();
		int* ViewModelIndex();
	};

	enum WEAPON_TYPE
	{
		WEAPON_TYPE_RIFEL,
		WEAPON_TYPE_PISTOL ,
		WEAPON_TYPE_SHOTGUN ,
		WEAPON_TYPE_SNIPER ,
		WEAPON_TYPE_GRENADE ,
		WEAPON_TYPE_KNIFE ,
		WEAPON_TYPE_C4 ,
		WEAPON_TYPE_UNKNOWN ,
	};

	
	enum WEAPON_ID
	{
		WEAPON_NONE = 0,
		WEAPON_DEAGLE,
		WEAPON_ELITE,
		WEAPON_FIVESEVEN,
		WEAPON_GLOCK,
		WEAPON_AK47 = 7,
		WEAPON_AUG,
		WEAPON_AWP,
		WEAPON_FAMAS,
		WEAPON_G3SG1,
		WEAPON_GALILAR = 13,
		WEAPON_M249,
		WEAPON_M4A1 = 16,
		WEAPON_MAC10,
		WEAPON_P90 = 19,
		WEAPON_MP5 = 23,
		WEAPON_UMP45,
		WEAPON_XM1014,
		WEAPON_BIZON,
		WEAPON_MAG7,
		WEAPON_NEGEV,
		WEAPON_SAWEDOFF,
		WEAPON_TEC9,
		WEAPON_TASER,
		WEAPON_HKP2000,
		WEAPON_MP7,
		WEAPON_MP9,
		WEAPON_NOVA,
		WEAPON_P250,
		WEAPON_SCAR20 = 38,
		WEAPON_SG553,
		WEAPON_SSG08,
		WEAPON_KNIFEGG,
		WEAPON_KNIFE,
		WEAPON_FLASHBANG,
		WEAPON_HEGRENADE,
		WEAPON_SMOKEGRENADE,
		WEAPON_MOLOTOV,
		WEAPON_DECOY,
		WEAPON_INCGRENADE,
		WEAPON_C4,
		WEAPON_HEALTHSHOT = 57,
		WEAPON_KNIFE_T = 59,
		WEAPON_M4A1_SILENCER,
		WEAPON_USP_SILENCER,
		WEAPON_CZ75A = 63,
		WEAPON_REVOLVER,
		WEAPON_TAGRENADE = 68,
		WEAPON_FISTS,
		WEAPON_BREACHCHARGE,
		WEAPON_TABLET = 72,
		WEAPON_MELEE = 74,
		WEAPON_AXE,
		WEAPON_HAMMER,
		WEAPON_SPANNER = 78,
		WEAPON_KNIFE_GHOST = 80,
		WEAPON_FIREBOMB,
		WEAPON_DIVERSION,
		WEAPON_FRAG_GRENADE,
		WEAPON_KNIFE_BAYONET = 500,
		WEAPON_KNIFE_FLIP = 505,
		WEAPON_KNIFE_GUT,
		WEAPON_KNIFE_KARAMBIT,
		WEAPON_KNIFE_M9_BAYONET,
		WEAPON_KNIFE_TACTICAL,
		WEAPON_KNIFE_FALCHION = 512,
		WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
		WEAPON_KNIFE_BUTTERFLY,
		WEAPON_KNIFE_PUSH,
		WEAPON_KNIFE_URSUS = 519,
		WEAPON_KNIFE_JACK,
		WEAPON_KNIFE_STILETTO = 522,
		WEAPON_KNIFE_WIDOWMAKER,
		GLOVE_STUDDED_BLOODHOUND = 5027,
		GLOVE_CT_SIDE = 5028,
		GLOVE_T_SIDE = 5029,
		GLOVE_SPORTY = 5030,
		GLOVE_SLICK = 5031,
		GLOVE_LEATHER_WRAP = 5032,
		GLOVE_MOTORCYCLE = 5033,
		GLOVE_SPECIALIST = 5034,
		GLOVE_HYDRA = 5035
	};
}                                                                                                                                 