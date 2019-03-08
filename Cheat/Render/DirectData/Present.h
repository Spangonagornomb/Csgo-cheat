#pragma once
#include "../../GUI/Gui.h"
#include "../../GUI/Menu.h"
#include "../DXRender.h"
#include "../../Hacks/Setup.h"
#include "../TestTexture.h"
#include "../Fonts/Arial.h"
#include "../Fonts/Tahoma.h"
#include "../Fonts/WeaponIcons.h"

BOOL CreateRender = FALSE;

COverManager VisualsOverlay;
COverManager TestOverlay;
CMenu Menu;

CFont* VisualsFont = nullptr;
CFont* MenuFont = nullptr;

LPDIRECT3DTEXTURE9 UpdateImage(void *pArr, UINT szArr, float w, float h)
{
	LPDIRECT3DTEXTURE9 entry;
	D3DXCreateTextureFromFileInMemoryEx(GP_Render->m_pDevice, pArr, szArr, w, h,
		D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &entry);
	return entry;
}

HRESULT STDMETHODCALLTYPE myPresent(IDirect3DDevice9* pDevice, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{

#ifndef ONLY_DRAW_HOOK
	CGlobal::IsGameReady = I::Engine()->IsInGame();
	//CGlobal::IsGameReady = I::Engine()->IsConnected();
#else
	CGlobal::IsGameReady = false;
#endif 

#ifdef ENABLE_CLEAR_CONSOLE
	system("cls");
#endif 

	if (pDevice)
	{
		if (!CreateRender)
		{
			g_pDevice = pDevice;
			GP_Render->m_pDevice = pDevice;
			VisualsFont = new CFont();
			MenuFont = new CFont();
			CFont::FontConfig font_config;
			font_config.OversampleH = 3;
			font_config.OversampleV = 3;
			font_config.PixelSnapH = 1;
			static const Wchar ranges[] =
			{
				0x0020, 0x00FF,
				0x0400, 0x044F,
				0,
			};
			ADD_LOG("2-1-11-7-3-7-4\n");
			VisualsFont = VisualsFont->CreateFontFromCompressMemory(Arial_compressed_data, Arial_compressed_size, 16.f, &font_config, ranges);
			MenuFont = MenuFont->CreateFontFromCompressMemory(Tahoma_compressed_data, Tahoma_compressed_size, 16.f, &font_config, ranges);
			ADD_LOG("2-1-11-7-3-7-5\n");
			GP_Render->UpdateSzFonts();
			ADD_LOG("2-1-11-7-3-7-6\n");
			CreateFontsTexture();
			ADD_LOG("2-1-11-7-3-7-7\n");
			X1Gui().Start();
			ADD_LOG("2-1-11-7-3-7-8\n");

			Message::Get().Start("Injected");
			ADD_LOG("2-1-11-7-3-7-9\n");
			CreateRender = true;
		}
		else
		{
#ifndef ONLY_DRAW_HOOK
			I::Engine()->GetScreenSize(CGlobal::iScreenWidth, CGlobal::iScreenHeight);
#endif

			CGSettings::G().UpdateColors();

			GP_Render->BeginDraw();
			
			VisualsOverlay.ResetCount();
			GP_Render->SetOverlay(&VisualsOverlay);
			{
				GP_Render->SetFont(VisualsFont);
				if (CGlobal::IsGameReady)
				{
					if (GP_Esp)
						GP_Esp->Draw();

					if (GP_Misc)
						GP_Misc->Draw();

					if (GP_GHelper)
						if (GP_GHelper->Enable)
							GP_GHelper->Draw();

					if (GP_LegitAim)
						GP_LegitAim->Draw();

					if (GP_Esp->GranadePrediction)
						grenade_prediction::Get().Paint();
				}
				else
				{
					if (GP_Esp)
						GP_Esp->Reset();

					if (GP_Misc)
					{
						GP_Misc->Reset();
						GP_Misc->AutoAcceptEmit();
					}
				}

				Message::Get().Show();
			}
			GP_Render->ReleaseOverlay();
			
			X1Gui().Update();

			if (GP_Radar)
				GP_Radar->Draw();

			if (GP_Misc->SpectatorList && !CGlobal::FullUpdateCheck)
				GP_Misc->ShowSpectatorList();

			if(!GP_Enable3DMenu->Check() || !GP_Enable3DMenu->Enable)
				Menu.Draw(MenuFont);

			X1Gui().Cleanup();

			GP_Render->EndDraw();

		}
	}

	return pPresent->GetTrampoline()(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}
