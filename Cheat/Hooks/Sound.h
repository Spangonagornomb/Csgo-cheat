#pragma once
#include "Tables.h"

//int WINAPI EmitSound1(IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample,
//	float flVolume, soundlevel_t iSoundlevel, int nSeed, int iFlags, int iPitch,
//	const Vector *pOrigin, const Vector *pDirection, CUtlVector< Vector >* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int i)
//{
//
//
//	/*if (GP_Esp && GP_Radar && IsReadyGame)
//		if ((GP_Esp->SoundEspEnable || GP_Radar->Sound) && pSample)
//			if (strstr(pSample, ("footsteps")) && pOrigin)
//				GP_Esp->PlaySound(*pOrigin, iEntIndex);
//*/
//
//	return HookTables::oEmitSound1(filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample,
//		flVolume, iSoundlevel, nSeed, iFlags, iPitch,
//		pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, i);
//}
//
//int _fastcall EmitSound2(void* thisptr, int edx, IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample,
//	float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch,
//	const Vector *pOrigin, const Vector *pDirection, CUtlVector< Vector >* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int i)
//{
//
//	/*if (GP_Esp && GP_Radar && IsReadyGame)
//		if ((GP_Esp->SoundEspEnable || GP_Radar->Sound) && pSample)
//			if (strstr(pSample, ("footsteps")) && pOrigin)
//				GP_Esp->PlaySound(*pOrigin, iEntIndex);*/
//
//
//	return HookTables::oEmitSound2(thisptr, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample,
//		flVolume, flAttenuation, nSeed, iFlags, iPitch,
//		pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, i);
//}

void __fastcall PlaySound(void* thisptr, int edx, const char* pszSoundName)
{
	//if (GP_Misc)
	//	GP_Misc->PlaySound(pszSoundName);
	HookTables::pPlaySound->GetTrampoline()(thisptr, pszSoundName);
}