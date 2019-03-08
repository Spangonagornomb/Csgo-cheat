#pragma once
#include "../Include/Winheaders.h"
#include "Tables.h"

namespace RenderHook
{
	//typedef HRESULT(WINAPI* Present_t)(IDirect3DDevice9* pDevice, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
	//typedef HRESULT(WINAPI* Reset_t)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);

	//Present_t Present_o = nullptr;
	//Reset_t Reset_o = nullptr;

	//IDirect3DDevice9* g_pDevice = nullptr;

	//HRESULT WINAPI Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	//{
	////	ADD_LOG("ResetCall\n");
	//	HRESULT hRes = Reset_o(pDevice, pPresentationParameters);

	//	return hRes;
	//}
	//
	//HRESULT WINAPI Present(IDirect3DDevice9* pDevice, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
	//{
	//	//ADD_LOG("PresentCall\n");
	//	return Present_o(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	//}
}