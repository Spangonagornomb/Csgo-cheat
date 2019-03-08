#include "SDK.h"
#include <time.h>
#include  "../../Main.h"
//#include "../Settings/Settings.h"


namespace SDK
{
	IVEngineClient*     I::g_pEngine = nullptr;
	IBaseClientDLL*     I::g_pClient = nullptr;
	IClientEntityList*  I::g_pEntityList = nullptr;
	CClientEntityList*	I::g_pClientEntityList = nullptr;
	IGameEventManager2* I::g_pGameEvents = nullptr;
	CGlobalVarsBase*    I::g_pGlobals = nullptr;
	CInput*             I::g_pInput = nullptr;
//	CClientState*       I::g_pClientState = nullptr;
	IEngineTrace*       I::g_pEngineTrace = nullptr;
	IClientMode*        I::g_pClientMode = nullptr;
	CPanel*   I::g_pPanel = nullptr;
	IVModelInfoClient*	I::g_pModelInfo = nullptr;
	IEngineSound*		I::g_pSound = nullptr;
	IVModelRender*		I::g_pModelRender = nullptr;
	IVRenderView*		I::g_pRenderView = nullptr;
	IMaterialSystem*	I::g_pMaterialSystem = nullptr;
	ISurface*			I::g_pSurface = nullptr;
	IGameEventManager2*	I::g_pGameEventMgr = nullptr;
	IInputSystem*		I::g_pInputSystem = nullptr;
	ConVar*             I::g_pConVar = nullptr;
	ILocalize*          I::g_pLocalize = nullptr;
	ISteamGameCoordinator* I::g_pSteamGameCoordinator = nullptr;
	ISteamUser* I::g_pSteamUser = nullptr;

	CreateInterfaceFn CaptureFactory( char* FactoryModule )
	{
		CreateInterfaceFn Interface = 0;

		while ( !Interface )
		{
			HMODULE hFactoryModule = FastCall::G().t_GetModuleHandleA( FactoryModule );

			if ( hFactoryModule )
			{
				Interface = (CreateInterfaceFn)(FastCall::G().t_GetProcAddress( hFactoryModule , ("CreateInterface") ) );
			}

			FastCall::G().t_Sleep( 100 );
		}

		return Interface;
	}

	template<typename T>
	T* CaptureInterface( CreateInterfaceFn Interface , char* InterfaceName )
	{
		T* dwPointer = nullptr;

		while ( !dwPointer )
		{
			dwPointer = (T*)( Interface( InterfaceName , 0 ) );

			FastCall::G().t_Sleep( 100 );
		}

		return dwPointer;
	}

	IVEngineClient* I::Engine()
	{
	
		if ( !g_pEngine )
		{
			CreateInterfaceFn pfnFactory = CaptureFactory( ENGINE_DLL );
			g_pEngine = CaptureInterface<IVEngineClient>( pfnFactory , VENGINE_CLIENT_INTERFACE_VERSION );
			ADD_LOG("->Engine -> %X\n", (DWORD)g_pEngine);
		}
		
		return g_pEngine;
	}

	IBaseClientDLL* I::Client()
	{
		if ( !g_pClient )
		{

			CreateInterfaceFn pfnFactory = CaptureFactory(GetClientDll());

			g_pClient = CaptureInterface<IBaseClientDLL>( pfnFactory , CLIENT_DLL_INTERFACE_VERSION );

			while (!((DWORD)**(IClientMode***)( (*(uint32_t**)Client())[10] + 0x5 )) || !((DWORD)**(CGlobalVarsBase***)( (*(uint32_t**)Client())[0] + 0x1B )))
			{
				FastCall::G().t_Sleep(1000);
			}
			ADD_LOG("->Client -> %X\n", (DWORD)g_pClient);
		}
		
		if (!IsBadReadPtr(g_pClient, sizeof(IBaseClientDLL*)))
			return g_pClient;
		else
			return nullptr;
	}
	
	IClientEntityList* I::EntityList()
	{
		if ( !g_pEntityList )
		{
			CreateInterfaceFn pfnFactory = CaptureFactory(GetClientDll());

			g_pEntityList = CaptureInterface<IClientEntityList>( pfnFactory , VCLIENTENTITYLIST_INTERFACE_VERSION );
			ADD_LOG("->EntityList -> %X\n", (DWORD)g_pEntityList);
		}
		
		if (!IsBadReadPtr(g_pEntityList, sizeof(IClientEntityList*)))
			return g_pEntityList;
		else
			return nullptr;
	}

	void* FindInterface(const char* Module, const char* InterfaceName)
	{
		void* Interface = nullptr;
		auto CreateInterface = reinterpret_cast<CreateInterfaceFn>(FastCall::G().t_GetProcAddress(
			FastCall::G().t_GetModuleHandleA(Module), "CreateInterface"));

		char PossibleInterfaceName[1024];
		for (int i = 1; i < 100; i++)
		{
			sprintf(PossibleInterfaceName, "%s0%i", InterfaceName, i);
			Interface = CreateInterface(PossibleInterfaceName, 0);
			if (Interface)
				break;

			sprintf(PossibleInterfaceName, "%s00%i", InterfaceName, i);
			Interface = CreateInterface(PossibleInterfaceName, 0);
			if (Interface)
				break;
		}

		if (!Interface)
			std::cout << InterfaceName << " not found!" << std::endl;
		else
			std::cout << InterfaceName << " 0x" << Interface << std::endl;

		return Interface;
	}
	CClientEntityList* I::ClientEntityList()
	{
		if (!g_pClientEntityList)
		{
			g_pClientEntityList = reinterpret_cast<CClientEntityList*>(FindInterface(GetClientDll(), "VClientEntityList"));
		}

		if (!IsBadReadPtr(g_pClientEntityList, sizeof(CClientEntityList*)))
			return g_pClientEntityList;
		else
			return nullptr;
	}

	CGlobalVarsBase* I::GlobalVars()
	{
		if ( !g_pGlobals )
		{
			auto pClientVFTable = *(uint32_t**)Client();
			g_pGlobals = **(CGlobalVarsBase***)( pClientVFTable[0] + 0x1B );
			ADD_LOG("->Globals -> %X\n", (DWORD)g_pGlobals);
		}

		return g_pGlobals;
	}
	//CClientState* I::ClientState()
	//{
	//	if ( !g_pClientState )
	//	{
	//		
	//		g_pClientState = **(CClientState***)(CSX::Memory::FindPatternV2("engine.dll", "A1 ? ? ? ? 8B 80 ? ? ? ? C3") + 1);
	//		//auto pClientVFTable = *(uint32_t**)Client();
	//		//g_pClientState = **(CInput***)( pClientVFTable[15] + 0x1 );
	//	}
	//	
	//	return g_pClientState;
	//}
	CInput* I::Input()
	{
		if ( !g_pInput )
		{
			auto pClientVFTable = *(uint32_t**)Client();
			g_pInput = **(CInput***)( pClientVFTable[15] + 0x1 );
			ADD_LOG("->Input -> %X\n", (DWORD)g_pInput);
		}
		
		return g_pInput;
	}

	CPanel* I::Panel()
	{
		if (!g_pPanel)
		{
			CreateInterfaceFn pfnFactory = CaptureFactory(("vgui2.dll"));
			g_pPanel = CaptureInterface<CPanel>(pfnFactory, ("VGUI_Panel"));
			ADD_LOG("->Panel -> %X\n", (DWORD)g_pPanel);
		}

		return g_pPanel;
	}

	IEngineTrace* I::EngineTrace()
	{
		if ( !g_pEngineTrace )
		{
			CreateInterfaceFn pfnFactory = CaptureFactory( ENGINE_DLL );
			g_pEngineTrace = CaptureInterface<IEngineTrace>( pfnFactory , INTERFACEVERSION_ENGINETRACE_CLIENT );
			ADD_LOG("->EngineTrace -> %X\n", (DWORD)g_pEngineTrace);
		}
		
		return g_pEngineTrace;
	}

	IClientMode* I::ClientMode()
	{
		if ( !g_pClientMode )
		{
			auto pClientVFTable = *(uint32_t**)Client();
			g_pClientMode = **(IClientMode***)( pClientVFTable[10] + 0x5 );
			ADD_LOG("->ClientMode -> %X\n", (DWORD)g_pClientMode);
		}
		
		return g_pClientMode;
	}

	IVModelInfoClient* I::ModelInfo()
	{
		if ( !g_pModelInfo )
		{
			CreateInterfaceFn pfnFactory = CaptureFactory( ENGINE_DLL );
			g_pModelInfo = CaptureInterface<IVModelInfoClient>( pfnFactory , VMODELINFO_CLIENT_INTERACE_VERSION );
			ADD_LOG("->ModelInfo -> %X\n", (DWORD)g_pModelInfo);
		}
		
		return g_pModelInfo;
	}

	IEngineSound* I::Sound()
	{
		if ( !g_pSound )
		{
			CreateInterfaceFn pfnFactory = CaptureFactory( ENGINE_DLL );
			g_pSound = CaptureInterface<IEngineSound>( pfnFactory , IENGINESOUND_CLIENT_INTERFACE_VERSION );
			ADD_LOG("->Sound -> %X\n", (DWORD)g_pSound);
		}
		
		return g_pSound;
	}

	IVModelRender* I::ModelRender()
	{
		if ( !g_pModelRender )
		{
			CreateInterfaceFn pfnFactory = CaptureFactory( ENGINE_DLL );
			g_pModelRender = CaptureInterface<IVModelRender>( pfnFactory , VENGINE_HUDMODEL_INTERFACE_VERSION );
			ADD_LOG("->ModelRender -> %X\n", (DWORD)g_pModelRender);
		}
		
		return g_pModelRender;
	}

	IVRenderView* I::RenderView()
	{
		if ( !g_pRenderView )
		{
			CreateInterfaceFn pfnFactory = CaptureFactory( ENGINE_DLL );
			g_pRenderView = CaptureInterface<IVRenderView>( pfnFactory , VENGINE_RENDERVIEW_INTERFACE_VERSION );
			ADD_LOG("->RenderView -> %X\n", (DWORD)g_pRenderView);
		}
		
		return g_pRenderView;
	}

	IMaterialSystem* I::MaterialSystem()
	{
		if ( !g_pMaterialSystem )
		{
			CreateInterfaceFn pfnFactory = CaptureFactory( MATERIAL_DLL );
			g_pMaterialSystem = CaptureInterface<IMaterialSystem>( pfnFactory , MATERIAL_SYSTEM_INTERFACE_VERSION );

			ADD_LOG("->MaterialSystem -> %X\n", (DWORD)g_pMaterialSystem);

		}
		
		return g_pMaterialSystem;
	}
	
	ISurface* I::Surface()
	{
		if ( !g_pSurface )
		{
			CreateInterfaceFn pfnFactory = CaptureFactory( VGUIMT_DLL );
			g_pSurface = CaptureInterface<ISurface>( pfnFactory , VGUI_SURFACE_INTERFACE_VERSION );

			ADD_LOG("->Surface -> %X\n", (DWORD)g_pSurface);
		}
		
		return g_pSurface;
	}

	IGameEventManager2*	I::GameEvent()
	{
		if ( !g_pGameEventMgr )
		{
			CreateInterfaceFn pfnFactory = CaptureFactory( ENGINE_DLL );
			g_pGameEventMgr = CaptureInterface<IGameEventManager2>( pfnFactory , GAMEEVENTMANAGER_INTERFACE_VERSION );

			ADD_LOG("->GameEventMgr -> %X\n", (DWORD)g_pGameEventMgr);
		}
		
		return g_pGameEventMgr;
	}

	IInputSystem* I::InputSystem()
	{
		if ( !g_pInputSystem )
		{
			CreateInterfaceFn pfnFactory = CaptureFactory( INPUTSYSTEM_DLL );
			g_pInputSystem = CaptureInterface<IInputSystem>( pfnFactory , INPUTSYSTEM_INTERFACE_VERSION );

			ADD_LOG("->InputSystem -> %X\n", (DWORD)g_pInputSystem);
		}
		
		return g_pInputSystem;
	}

	ConVar* I::GetConVar()
	{
		if (!g_pConVar)
		{
			CreateInterfaceFn pfnFactory = CaptureFactory(VSTDLIB_DLL);
			g_pConVar = CaptureInterface<ConVar>(pfnFactory, VENGINEVCAR_INTERFACE_VERSION);
			ADD_LOG("->ConVar -> %X\n", (DWORD)g_pConVar);
		}

		return g_pConVar;
	}

	ILocalize* I::Localize()
	{
		if (!g_pLocalize)
		{
			CreateInterfaceFn pfnFactory = CaptureFactory(("localize.dll"));
			g_pLocalize = CaptureInterface<ILocalize>(pfnFactory, ("Localize_001"));
			ADD_LOG("->Localize -> %X\n", (DWORD)g_pLocalize);
		}

		return g_pLocalize;
	}

	ISteamUser* I::SteamUser()
	{
		if (!g_pSteamUser) {
			SteamGameCoordinator();
		}

		return g_pSteamUser;
	}

	ISteamGameCoordinator* I::SteamGameCoordinator()
	{
		if (!g_pSteamGameCoordinator) {
			typedef uint32_t SteamPipeHandle;
			typedef uint32_t SteamUserHandle;


			SteamUserHandle hSteamUser = ((SteamUserHandle(__cdecl*)(void))FastCall::G().t_GetProcAddress(FastCall::G().t_GetModuleHandleA(("steam_api.dll")), ("SteamAPI_GetHSteamUser")))();
			SteamPipeHandle hSteamPipe = ((SteamPipeHandle(__cdecl*)(void))FastCall::G().t_GetProcAddress(FastCall::G().t_GetModuleHandleA(("steam_api.dll")), ("SteamAPI_GetHSteamPipe")))();

			auto SteamClient = ((ISteamClient*(__cdecl*)(void))FastCall::G().t_GetProcAddress(FastCall::G().t_GetModuleHandleA(("steam_api.dll")), ("SteamClient")))();

			auto SteamHTTP = SteamClient->GetISteamHTTP(hSteamUser, hSteamPipe, ("STEAMHTTP_INTERFACE_VERSION002"));
			g_pSteamUser = SteamClient->GetISteamUser(hSteamUser, hSteamPipe, ("SteamUser019"));
			auto SteamFriends = SteamClient->GetISteamFriends(hSteamUser, hSteamPipe, ("SteamFriends015"));
			auto SteamInventory = SteamClient->GetISteamInventory(hSteamUser, hSteamPipe, ("STEAMINVENTORY_INTERFACE_V002"));
			g_pSteamGameCoordinator = (ISteamGameCoordinator*)SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, ("SteamGameCoordinator001"));
			ADD_LOG("->SteamUser -> %X\n", (DWORD)g_pSteamUser);
			ADD_LOG("->SteamGameCoordinator -> %X\n", (DWORD)g_pSteamGameCoordinator);
		}

		return g_pSteamGameCoordinator;
	}
}  