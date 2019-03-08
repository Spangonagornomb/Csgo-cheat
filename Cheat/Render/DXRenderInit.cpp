#pragma comment(lib, "../X1API/detours.lib")
#include "../X1API/detours.h"

#include "DXRender.h"
#include "DirectData/Reset.h"
#include "DirectData/Present.h"
#include "DirectData/EndScene.h"

oEndScene pEndScene = nullptr;

cDetour<oPresent>* pPresent;
cDetour<oReset>* pReset;

#define HOOK(func,addy) p##func = (o##func)DetourFunction((PBYTE)addy,(PBYTE)my##func) 
#define UNHOOK(func,addy) p##func = (o##func)DetourFunction((PBYTE)addy,(PBYTE)p##func)

LPDIRECT3DDEVICE9 nm_pD3Ddev = nullptr;

LRESULT CALLBACK MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CRender::IRender::DX_Init(DWORD* table)
{
	
	LPDIRECT3DDEVICE9 pd3dDevice = NULL;
	do
	{
		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, "SEGUARD_Window", NULL };
		RegisterClassEx(&wc);
		HWND hWnd = CreateWindowA("SEGUARD_Window", NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, GetDesktopWindow(), NULL, wc.hInstance, NULL);
		LPDIRECT3D9 pD3D = Direct3DCreate9(D3D_SDK_VERSION);
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

		d3dpp.BackBufferCount = 3;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

		HRESULT res = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pd3dDevice);
	
		if(res == D3D_OK)
			int tt = GetLastError();
		else if (res == D3DERR_DEVICELOST)
			int tt = GetLastError();
		else if (res == D3DERR_INVALIDCALL)
			int tt = GetLastError();
		else if (res == D3DERR_NOTAVAILABLE)
			int tt = GetLastError();
		else if (res == D3DERR_OUTOFVIDEOMEMORY)
			int tt = GetLastError();

		DestroyWindow(hWnd);

		Sleep(1000);
		ADD_LOG("device!!!\n");
	} while (!pd3dDevice);

	DWORD* pVTable = (DWORD*)pd3dDevice;
	pVTable = (DWORD*)pVTable[0];
	table[0] = pVTable[16];
#ifdef PRESENT_ENABLE
	table[1] = pVTable[17];
#else
	table[1] = pVTable[42];
#endif
	
}

PBYTE HookVTable(PDWORD* dwVTable, PBYTE dwHook, INT Index)
{
	DWORD dwOld = 0;
	VirtualProtect((void*)((*dwVTable) + (Index * 4)), 4, PAGE_EXECUTE_READWRITE, &dwOld);
	PBYTE pOrig = ((PBYTE)(*dwVTable)[Index]);
	(*dwVTable)[Index] = (DWORD)dwHook;
	VirtualProtect((void*)((*dwVTable) + (Index * 4)), 4, dwOld, &dwOld);
	return pOrig;
}

HANDLE hThreadVMT = 0;

DWORD WINAPI VMT_Patching(LPVOID  Param)
{
	while (1)
	{
		Sleep(100);
		HookVTable((PDWORD*)nm_pD3Ddev, (PBYTE)myReset, 16);
#ifdef PRESENT_ENABLE
		HookVTable((PDWORD*)nm_pD3Ddev, (PBYTE)myPresent, 17);
#else
		HookVTable((PDWORD*)nm_pD3Ddev, (PBYTE)myEndScene, 42);
#endif

	}
	TerminateThread(hThreadVMT, 0);
	return 1;
}

BYTE WhiteTexture[/*145*/] =
{
	0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00,
	0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x1B,
	0x00, 0x00, 0x00, 0x17, 0x08, 0x02, 0x00, 0x00, 0x00, 0x75,
	0x74, 0xA3, 0x79, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47,
	0x42, 0x00, 0xAE, 0xCE, 0x1C, 0xE9, 0x00, 0x00, 0x00, 0x04,
	0x67, 0x41, 0x4D, 0x41, 0x00, 0x00, 0xB1, 0x8F, 0x0B, 0xFC,
	0x61, 0x05, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73,
	0x00, 0x00, 0x0E, 0xC3, 0x00, 0x00, 0x0E, 0xC3, 0x01, 0xC7,
	0x6F, 0xA8, 0x64, 0x00, 0x00, 0x00, 0x26, 0x49, 0x44, 0x41,
	0x54, 0x48, 0x4B, 0x63, 0xF8, 0x4F, 0x6D, 0x30, 0x6A, 0x22,
	0x75, 0xC0, 0xA8, 0x89, 0xD4, 0x01, 0xA3, 0x26, 0x52, 0x07,
	0x8C, 0x9A, 0x48, 0x1D, 0x30, 0x6A, 0x22, 0x75, 0xC0, 0xE0,
	0x37, 0xF1, 0xFF, 0x7F, 0x00, 0x52, 0x70, 0x40, 0x23, 0x76,
	0x21, 0x01, 0xFD, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E,
	0x44, 0xAE, 0x42, 0x60, 0x82,
};
using namespace  CSX::Memory;
void CRender::IRender::Initialize()
{
	ADD_LOG("2-1-11-7-3-0\n");
	m_pDevice = nullptr;
	m_pDrawMngr = nullptr;
	m_pFont = nullptr;
	m_pStateBlockDraw = nullptr;
	ADD_LOG("2-1-11-7-3-1\n");
	Fonts.clear();
	Fonts.resize(0);
	ADD_LOG("2-1-11-7-3-2\n");
	ScreenWidth = 1920;
	ScreenHeight = 1080;
	ADD_LOG("2-1-11-7-3-3\n");

	auto& pContext = cContext::GetInstance();

	DWORD d3d9TablePtrPtr = FindPattern(SHADERPIDX9_DLL, D3D9_PATTERN, D3D9_MASK, 1);
	if (d3d9TablePtrPtr)
	{
		g_pDevice = (IDirect3DDevice9*)(**(PDWORD*)d3d9TablePtrPtr);
		PVOID* IDirect3DDevice9Table = *reinterpret_cast<PVOID**>(g_pDevice);

		if (IDirect3DDevice9Table)
		{
			pContext.ApplyDetour<oPresent>(static_cast<oPresent>(IDirect3DDevice9Table[17]),
				reinterpret_cast<oPresent>
				(myPresent),
				&pPresent);
			ADD_LOG("Hook: Present\n");

			pContext.ApplyDetour<oReset>(static_cast<oReset>(IDirect3DDevice9Table[16]),
				reinterpret_cast<oReset>
				(myReset),
				&pReset);
			ADD_LOG("Hook: Reset\n");
			ADD_LOG("2-1-11-7-3-9\n");
		}
		else
		{
			ADD_LOG("2-1-11-7-3-10\n");
			ADD_LOG("Setup: InitTable Device Error\n");
			FastCall::G().t_MessageBoxA(0, "InitTable error", "Error!", 0);
		}
		ADD_LOG("2-1-11-7-3-11\n");
	}
	else
	{
		ADD_LOG("Setup: Hook Present Error\n");
		FastCall::G().t_MessageBoxA(0, "Present error", "Error!", 0);
	}
	ADD_LOG("2-1-11-7-3-12\n");

	ADD_LOG("Waith device...\n");
	while (!nm_pD3Ddev) { nm_pD3Ddev = m_pDevice; Sleep(50); }
	ADD_LOG("2-1-11-7-3-13\n");
	g_pDevice = m_pDevice;

	ADD_LOG("2-1-11-7-3-14\n");
#ifdef ENABLE_HOOK_CS_WND_PROC
	HWND hWindow = FastCall::G().t_FindWindowA("Valve001", 0);
#else
	HWND hWindow = FastCall::G().t_FindWindowA("winA", 0);
#endif 
	ADD_LOG("->Window -> %X\n", (DWORD)hWindow);
	ADD_LOG("2-1-11-7-3-15\n");
	WndProc_o = (WNDPROC)FastCall::G().t_SetWindowLongA(hWindow, GWL_WNDPROC, (LONG)(LONG_PTR)WndProcHandler);
}
