#pragma once
#include "Tables.h"


void __fastcall FrameStageNotify(void* ecx, void* edx, int Stage)
{
	static auto ofunc = HookTables::pFrameStageNotify->GetTrampoline();

	if ((ClientFrameStage_t)Stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START && CGlobal::IsGameReady && GP_Skins)
		if (GP_Skins->SkinsEnable)
			GP_Skins->PostDataUpdate();

	if (GP_Esp && GP_Esp->SoundEspEnable && CGlobal::IsGameReady && (ClientFrameStage_t)Stage == ClientFrameStage_t::FRAME_NET_UPDATE_END)
	{
		CUtlVector<SndInfo_t> sndList;
		sndList.RemoveAll();

		I::Sound()->GetActiveSounds(sndList);

		for (int i = 0; i < sndList.Count(); i++)
		{
			if (!sndList[i].m_pOrigin)
				continue;

			if (sndList[i].m_nChannel != 4)
				continue;

			if (!sndList[i].m_bUpdatePositions)
				continue;

			GP_Esp->PlaySound(*sndList[i].m_pOrigin, sndList[i].m_nSoundSource);
		}
	}

	ofunc(ecx, Stage);
}

