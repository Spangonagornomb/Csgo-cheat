#include "Main.h"
#include "X1API/ThreadInMemory.h"
#include "Initialize/Initialize.h"
#include "License/License.h"
#include "GUI/Gui.h"

HANDLE hThread = 0;

class IIStart
{
public:
	virtual class IStart
	{
	public:
		virtual void Start() = 0; 
	};
};

DWORD adress = 0;
CHAR ByteCodeOld[] = "\xFF\x15\x00\x00\x00\x00\xC3";

DWORD WINAPI SetupThread(LPVOID lpThreadParameter)
{
	auto LSetupThread = [&]() -> DWORD
	{
		ADD_LOG("2-1-0\n");
		ADD_LOG("======================Setup: Started\n");
		ADD_LOG("2-1-1\n");
		CInit::IInit *pInit = new CInit::IInit();
		pInit->Init();
		DELETE_PTR(pInit);
		ADD_LOG("2-1-2\n");
		ADD_LOG("======================Setup: Successful\n");
		ADD_LOG("2-1-3\n");
		return 0;
	};

	return LSetupThread();
}

HANDLE CreateThreadMemory(void * adress_func)
{
	__try
	{
		if (adress_func)
		{
			adress = ThreadMemory::ScanAddressTable();

			if (adress)
			{		
				CHAR ByteCode[] = "\xFF\x15\x00\x00\x00\x00\xC3";

				DWORD JumpAdress = (DWORD)(adress_func);

				ThreadMemory::ReadMemory((void*)adress, (void*)ByteCodeOld, 7);

				*(DWORD*)(&ByteCode[2]) = (DWORD)(&JumpAdress);

				int wt = 0;

				while (IsBadWritePtr((void*)adress, 7) != 0 && wt < 10) { Sleep(100); ADD_LOG("Waiting memory...\n"); wt++; }
				ADD_LOG("WriteMemory...\n");
				ThreadMemory::WriteMemory((void*)adress, &ByteCode[0], 7);

				HANDLE ret = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)adress, 0, 0, 0);

				return ret;
			}
			else
			{
				FastCall::G().t_MessageBoxA(0, "Thread error: 1", "Error!", 0);
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return NULL;
	}
	return NULL;
}

void Start()
{
	ADD_LOG("2-0\n");
	auto LStatr = [&]() -> void
	{
		ADD_LOG("2-1\n");
		CreateThread(NULL, 0, &SetupThread, NULL, 0, NULL);
	};
	ADD_LOG("2-2\n");
	LStatr();

	CreateDirectory("C:\\obnoxious framework", NULL);
	CreateDirectory("C:\\obnoxious framework\\Configurations", NULL);
	CreateDirectory("C:\\obnoxious framework\\Resources", NULL);
	CreateDirectory("C:\\obnoxious framework\\Resources\\Images", NULL);
	CreateDirectory("C:\\obnoxious framework\\Resources\\Sounds", NULL);
}

bool Panorama = true;

BOOL WINAPI DllMain( HINSTANCE hinstDLL , DWORD fdwReason , LPVOID lpReserved )
{
	if ( fdwReason == DLL_PROCESS_ATTACH )
	{	

#ifndef ENABLE_LICENSE
		MainSettings().UserName = "User name";
		MainSettings().key = "_KEY";
		MainSettings().id = "2137";
#endif

		auto LDllMain = [&]() -> void
		{
#ifdef ENABLE_CONSOLE_LOG
			AllocConsole();
			AttachConsole(FastCall::G().t_GetCurrentProcessId());
			freopen("CONOUT$", "w", stdout);

			ADD_LOG("DLL ATTACH\n");
			ADD_LOG("DLL BUILD: %s | %s\n", __TIME__, __DATE__);
#endif

			ADD_LOG("1\n");

			CLicense::pDllArgs = (CLicense::DllArgs*)lpReserved;

#ifdef ENABLE_LICENSE
			if (!CLicense::pDllArgs)
			{
				ADD_LOG("Args: Error\n");
				return;
			}
			else
			{
				ADD_LOG("Args: Sucessful\n");
			}
#endif
			ADD_LOG("2\n");
			Start();
			ADD_LOG("3\n");
		};
		LDllMain();	
	}
	else if ( fdwReason == DLL_PROCESS_DETACH )
	{
		GP_Setup->Shutdown();
		DELETE_PTR(GP_Setup);

		ADD_LOG("DLL DETACH\n");
	}

	return TRUE;
}            

char* GetClientDll()
{
	return "client_panorama.dll";
}

CMain& MainSettings() {
	return CMain::Singleton();
}

void CMain::SetMenuColors()
{
	GuiStyle& style = X1Gui().GetStyle();

	auto AutoChagngeColor = [&](Color col, float ch) -> Color
	{
		Color entry;

		entry.SetR(( (col.r() + ch < 0) ? 0 : ( (col.r() + ch > 255) ? 255 : (col.r() + ch) )));
		entry.SetG(( (col.g() + ch < 0) ? 0 : ( (col.g() + ch > 255) ? 255 : (col.g() + ch) )));
		entry.SetB(( (col.b() + ch < 0) ? 0 : ( (col.b() + ch > 255) ? 255 : (col.b() + ch) )));
		entry.SetA(col.a());

		return entry;
	};

	style.clrLine = LineColor;
	style.clrBackground = BackgroundColor;
	style.clrText = TextColor;

	style.clrFrame = FrameColor;
	style.clrFrameHover = AutoChagngeColor(FrameColor, -5);
	style.clrFrameHold = AutoChagngeColor(FrameColor, -9);

	style.clrTabLabel = color_t(ButtonColor.r(), ButtonColor.g(), ButtonColor.b(), 120);
	style.clrTabLabelText = color_t(TextColor.r(), TextColor.g(), TextColor.b(), 160);

	style.clrButton = ButtonColor;
	style.clrButtonHover = AutoChagngeColor(ButtonColor, -10);
	style.clrButtonHold = AutoChagngeColor(ButtonColor, -20);

	style.clrDisButton = DisableButtonColor;
	style.clrDisButtonHover = AutoChagngeColor(DisableButtonColor, -10);
	style.clrDisButtonHold = AutoChagngeColor(DisableButtonColor, -20);

	style.clrScroll = AutoChagngeColor(ButtonColor, -20);
	style.clrScrollHover = AutoChagngeColor(ButtonColor, -20);
	style.clrScrollHold = AutoChagngeColor(ButtonColor, -20);
	style.clrTitle = TitleColor;
}