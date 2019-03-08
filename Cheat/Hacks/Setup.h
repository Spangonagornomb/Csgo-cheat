#pragma once
#include "../Engine/EntityMem.h"
#include "Esp/Esp.h"
#include "Misc/Misc.h"
#include "LegitAim/LegitAim.h"
#include "Radar/Radar.h"
#include "Skins/Skins.h"
#include "Inventory/Inventory.h"
#include "GrenadeHelper/GreandeHelper.h"
#include "../Render/DXRender.h"
#include "../Engine/GrenadePrediction.h"
#include "../Engine/SDK/SDK.h"
#include "../3DMenu/3DMenu.h"
#include "../Main.h"


#define DELETE_PTR(name) delete name; name = nullptr


class IISetup
{
public:
	virtual class ISetup
	{
	public:
		virtual void Setup() = 0;
		virtual void Shutdown() = 0;
	};
};

extern CRender::IRender*	GP_Render;
extern CEntityPlayers*		GP_EntPlayers;
extern CEsp*				GP_Esp;
extern CMisc*				GP_Misc;
extern CLegitAim*			GP_LegitAim;
extern CRadar*				GP_Radar;
extern CSkins*				GP_Skins;
extern CInventory*			GP_Inventory;
extern CGHelper*			GP_GHelper;
extern CBind*				GP_Enable3DMenu;
extern C3DMenu*				GP_3DMenu;

class CSetup : public IISetup
{
public:
	virtual class ISetup
	{
	public:
	
		virtual void Setup()
		{
			ADD_LOG("2-1-11-7-0\n");
			GP_Render =		new CRender::IRender();
			GP_EntPlayers = new CEntityPlayers();
			GP_Esp =		new CEsp();
			GP_Misc =		new CMisc();
			GP_LegitAim =	new CLegitAim();
			GP_Radar =		new CRadar();
			GP_Skins =		new CSkins();
			GP_Inventory =	new CInventory();
			GP_GHelper =	new CGHelper();
			GP_Enable3DMenu = new CBind(VK_DELETE);
			GP_3DMenu =		new C3DMenu;
			ADD_LOG("2-1-11-7-1\n");
			//MainSettings().Initialize();
			ADD_LOG("2-1-11-7-2\n");
			CGSettings::G().UpdateList();
			GP_Misc->UpdateSoundList();
			ADD_LOG("2-1-11-7-3\n");
			GP_Esp->InitVisuals();
			GP_Render->Initialize();
			GP_GHelper->Initialize();
			ADD_LOG("2-1-11-7-4\n");
#ifndef ONLY_DRAW_HOOK
			GP_Misc->HitWorker.RegListener();
			GP_Skins->FireEvent.RegListener();
			GP_Skins->initialize_kits();
			GP_Skins->ParseSortedKits();
			GP_Skins->PrepareSortedSkins();
#endif
			ADD_LOG("2-1-11-7-5\n");
			GP_Skins->AllSkinsLoaded = true;
			ADD_LOG("2-1-11-7-6\n");
		}
		virtual void Shutdown()
		{
			GP_Misc->HitWorker.UnRegListener();
			GP_Skins->FireEvent.UnRegListener();

			DELETE_PTR(GP_Render);
			DELETE_PTR(GP_EntPlayers);
			DELETE_PTR(GP_Esp);
			DELETE_PTR(GP_Misc);
			DELETE_PTR(GP_LegitAim);
			DELETE_PTR(GP_Radar);
			DELETE_PTR(GP_Inventory);
			DELETE_PTR(GP_GHelper);
			DELETE_PTR(GP_Enable3DMenu);
			DELETE_PTR(GP_3DMenu);
		}
	};
};

extern CSetup::ISetup *GP_Setup;