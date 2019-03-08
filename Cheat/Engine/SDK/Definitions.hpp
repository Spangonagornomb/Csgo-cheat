#pragma once

#include <Windows.h>

#ifndef CONCAT_IMPL
#define CONCAT_IMPL(x, y) x##y
#ifndef MACRO_CONCAT
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)
#ifndef PAD
#define PAD(SIZE) BYTE MACRO_CONCAT(_pad, __COUNTER__)[SIZE];
#endif
#endif
#endif

#define VirtualFn( cast ) typedef cast( __thiscall* OriginalFn )

template< typename Function > Function call_vfunc(PVOID Base, DWORD Index)
{
	PDWORD* VTablePointer = (PDWORD*)Base;
	PDWORD VTableFunctionBase = *VTablePointer;
	DWORD dwAddress = VTableFunctionBase[Index];
	return (Function)(dwAddress);
}

template <typename T>
T GetVFunc(void *vTable, int iIndex) {
	return (*(T**)vTable)[iIndex];
}


template< typename Function > Function GetMethod( PVOID Base , DWORD Index )
{
	PDWORD* VTablePointer = (PDWORD*)Base;
	PDWORD VTableFunctionBase = *VTablePointer;
	DWORD dwAddress = VTableFunctionBase[Index];

	if (IsBadReadPtr((Function)(dwAddress), sizeof(Function)))
		return nullptr;

	return (Function)( dwAddress );
}


typedef struct con_nprint_s
{
	int		index;			// Row #
	float	time_to_live;	// # of seconds before it disappears. -1 means to display for 1 frame then go away.
	float	color[3];		// RGB colors ( 0.0 -> 1.0 scale )
	bool	fixed_width_font;
} con_nprint_t;

struct ScreenFade_t
{
	unsigned short 	duration;		// FIXED 16 bit, with SCREENFADE_FRACBITS fractional, seconds duration
	unsigned short 	holdTime;		// FIXED 16 bit, with SCREENFADE_FRACBITS fractional, seconds duration until reset (fade & hold)
	short			fadeFlags;		// flags
	byte			r , g , b , a;		// fade to color ( max alpha )
};

struct CHudTexture
{
	char	szShortName[64];	//0x0000
	char	szTextureFile[64];	//0x0040
	bool	bRenderUsingFont;	//0x0080
	bool	bPrecached;			//0x0081
	char	cCharacterInFont;	//0x0082
	BYTE	pad_0x0083;			//0x0083
	int		hFont;				//0x0084
	int		iTextureId;			//0x0088
	float	afTexCoords[4];		//0x008C
	int		iPosX[4];			//0x009C
}; //Size=0x00AC

class CWeaponInfo
{
public:
	char _0x0000[4];
	BYTE m_bParsedScript; //0x0004 
	BYTE m_bLoadedHudElements; //0x0005 
	BYTE m_szClassName; //0x0006 
	char _0x0007[1961];
	__int32 m_iAmmoType; //0x07B0 
	char _0x07B4[12];
	CHudTexture* m_pHudTexture_WeaponSilenced; //0x07C0 
	CHudTexture* m_pHudTexture_Weapon; //0x07C4 
	CHudTexture* m_pHudTexture_Ammo; //0x07C8 
	char _0x07CC[4];
	CHudTexture* m_pHudTexture_Crosshair; //0x07D0 
	char _0x07D4[20];
	__int32 m_iWeaponType; //0x07E8 
	__int32 m_iTeam; //0x07EC 
	__int32 m_iWeaponId; //0x07F0 
	BYTE m_bFullAuto; //0x07F4 
	char _0x07F5[3];
	float m_flHeatPerShot; //0x07F8 
	__int32 m_iWeaponPrice; //0x07FC 
	float m_flArmorRatio; //0x0800 
	float m_flMaxPlayerSpeed; //0x0804 
	float m_flMaxPlayerSpeedAlt; //0x0808 
	__int32 m_iCrosshairMinDistance; //0x080C 
	__int32 m_iCrosshairDeltaDistance; //0x0810 
	float m_flPenetration; //0x0814 
	__int32 m_iDamage; //0x0818 
	float m_flRange; //0x081C 
	float m_flRangeModifier; //0x0820 
	__int32 m_iBullets; //0x0824 
	float m_flCycleTime; //0x0828 
	float m_flCycleTimeAlt; //0x082C 
	char _0x0830[416];
	float m_flSpread; //0x09D0 
	float m_flSpreadAlt; //0x09D4 
	float m_flInaccuracyCrouch; //0x09D8 
	float m_flInaccuracyCrouchAlt; //0x09DC 
	float m_flInaccuracyStand; //0x09E0 
	float m_flInaccuracyStandAlt; //0x09E4 
	float m_flInaccuracyJump; //0x09E8 
	float m_flInaccuracyJumpAlt; //0x09EC 
	float m_flInaccuracyLand; //0x09F0 
	float m_flInaccuracyLandAlt; //0x09F4 
	float m_flInaccuracyLadder; //0x09F8 
	float m_flInaccuracyLadderAlt; //0x09FC 
	float m_flInaccuracyFire; //0x0A00 
	float m_flInaccuracyFireAlt; //0x0A04 
	float m_flInaccuracyMove; //0x0A08 
	float m_flInaccuracyMoveAlt; //0x0A0C 
	float m_flRecoveryTimeStand; //0x0A10 
	float m_flRecoveryTimeCrouch; //0x0A14 
	float m_flInaccuracyReload; //0x0A18 
	float m_flInaccuracyAltSwitch; //0x0A1C 
	float m_flRecoilAngle; //0x0A20 
	float m_flRecoilAngleAlt; //0x0A24 
	float m_flRecoilAngleVariance; //0x0A28 
	float m_flRecoilAngleVarianceAlt; //0x0A2C 
	float m_flRecoilMagnitude; //0x0A30 
	float m_flRecoilMagnitudeAlt; //0x0A34 
	float m_flRecoilMagnitudeVariance; //0x0A38 
	float m_flRecoilMagnitudeVarianceAlt; //0x0A3C 
	__int32 m_iRecoilSeed; //0x0A40 
	float m_flFlinchVelocityModifierLarge; //0x0A44 
	float m_flFlinchVelocityModifierSmall; //0x0A48 
	float m_flTimeToIdle; //0x0A4C 
	float m_flIdleInterval; //0x0A50 
};

class input {
public:
	char pad_0x00[0x0C]; // 0x00
	bool m_trackir_available; //0x0C
	bool m_mouse_initiated; //0x0D
	bool m_mouse_active; //0x0E
	char pad_0x0F[0xFBE];
}; // size = 0xFCC


enum CyrTeamID : int
{
	CYRT_CT,
	CYRT_TT,
	CYRT_ALL,
	CYRT_AUTO,
	CYRT_DISBLE
};

enum TeamID
{
	TEAM_UNASSIGNED,
	TEAM_SPECTATOR,
	TEAM_TERRORIST,
	TEAM_COUNTER_TERRORIST,
	TEAM_ALL
};

namespace SDK
{
	typedef void* ( *CreateInterfaceFn )( const char *pName , int *pReturnCode );
	typedef void* ( *InstantiateInterfaceFn )( );

	typedef void( *pfnDemoCustomDataCallback )( unsigned char *pData , size_t iSize );

	enum class FontFeature {
		FONT_FEATURE_ANTIALIASED_FONTS = 1 ,
		FONT_FEATURE_DROPSHADOW_FONTS = 2 ,
		FONT_FEATURE_OUTLINE_FONTS = 6 ,
	};

	enum class FontDrawType {
		FONT_DRAW_DEFAULT = 0 ,
		FONT_DRAW_NONADDITIVE ,
		FONT_DRAW_ADDITIVE ,
		FONT_DRAW_TYPE_COUNT = 2 ,
	};

	enum class FontFlags {
		FONTFLAG_NONE ,
		FONTFLAG_ITALIC = 0x001 ,
		FONTFLAG_UNDERLINE = 0x002 ,
		FONTFLAG_STRIKEOUT = 0x004 ,
		FONTFLAG_SYMBOL = 0x008 ,
		FONTFLAG_ANTIALIAS = 0x010 ,
		FONTFLAG_GAUSSIANBLUR = 0x020 ,
		FONTFLAG_ROTARY = 0x040 ,
		FONTFLAG_DROPSHADOW = 0x080 ,
		FONTFLAG_ADDITIVE = 0x100 ,
		FONTFLAG_OUTLINE = 0x200 ,
		FONTFLAG_CUSTOM = 0x400 ,
		FONTFLAG_BITMAP = 0x800 ,
	};

	enum class ClientFrameStage_t {
		FRAME_UNDEFINED = -1 ,
		FRAME_START ,
		FRAME_NET_UPDATE_START ,
		FRAME_NET_UPDATE_POSTDATAUPDATE_START ,
		FRAME_NET_UPDATE_POSTDATAUPDATE_END ,
		FRAME_NET_UPDATE_END ,
		FRAME_RENDER_START ,
		FRAME_RENDER_END
	};

	enum class ObserverMode_t : int
	{
		OBS_MODE_NONE = 0 ,
		OBS_MODE_DEATHCAM = 1 ,
		OBS_MODE_FREEZECAM = 2 ,
		OBS_MODE_FIXED = 3 ,
		OBS_MODE_IN_EYE = 4 ,
		OBS_MODE_CHASE = 5 ,
		OBS_MODE_ROAMING = 6
	};
}