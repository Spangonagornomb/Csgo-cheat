#pragma once
//#include "../Engine/CSX/CSX_Hook.h"
#include "../Hacks/Setup.h"
#include "../Hooks/Hook.h"
#include "../Engine/SDK/SDK.h"
#include "../X1API/MinHook/hook.h"
using namespace SDK;

namespace HookTables
{
	using CreateMoveFn = bool(__stdcall*)(float, CUserCmd*);
	using OverrideViewFn = bool(__stdcall*)(CViewSetup*);
	using GetViewModelFOVFn = float(__stdcall*)();
	using FrameStageNotifyFn = void(__thiscall*)(void*, int);
	using FireEventClientSideThinkFn = bool(__thiscall*)(void*, IGameEvent*);
	using DrawModelExecuteFn = bool(__thiscall*)(void*, IMatRenderContext*, const DrawModelState_t&,
		const ModelRenderInfo_t&, matrix3x4_t*);
	using PlaySoundFn = bool(__thiscall*)(void*, const char*);
	using LockCursorFn = bool(__thiscall*)(void*);
	using RetrieveMessageFn = EGCResults(__thiscall*)(void*, uint32_t*, void*, uint32_t, uint32_t*);
	using SendMessageFn = EGCResults(__thiscall*)(void*, uint32_t, const void*, uint32_t);
	using PostDataUpdateFn = void(__stdcall*)(void*, int);


	cDetour<CreateMoveFn>* pCreateMove;
	cDetour<OverrideViewFn>* pOverrideView;
	cDetour<GetViewModelFOVFn>* pGetViewModelFOV;
	cDetour<FrameStageNotifyFn>* pFrameStageNotify;
	cDetour<FireEventClientSideThinkFn>* pFireEventClientSideThink;
	cDetour<DrawModelExecuteFn>* pDrawModelExecute;
	cDetour<PlaySoundFn>* pPlaySound;
	cDetour<LockCursorFn>* pLockCursor;
	cDetour<RetrieveMessageFn>* pRetrieveMessage;
	cDetour<SendMessageFn>* pSendMessage;
	cDetour<PostDataUpdateFn>* pPostDataUpdate;
}