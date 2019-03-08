#pragma once
#include "../DXRender.h"

HRESULT WINAPI myEndScene(IDirect3DDevice9* pDevice)
{
	if (pDevice)
	{
		
	}
	return pEndScene(pDevice);
}