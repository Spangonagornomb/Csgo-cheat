#pragma once
#include "../DXRender.h"
#include "../../Hacks/Setup.h"

HRESULT WINAPI myReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* param)
{
	if (GP_Render)
		GP_Render->InvalidateDeviceObjects();

	HRESULT result = pReset->GetTrampoline()(pDevice, param);

	if (GP_Render)
		GP_Render->CreateDeviceObjects();

	return result;
}