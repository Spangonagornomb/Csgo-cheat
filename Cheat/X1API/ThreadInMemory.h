#pragma once
#include  "../Main.h"

class ThreadMemory
{
public:
	static void WriteMemory(void *adr, void *ptr, int size)
	{
		DWORD OldProtection;
		VirtualProtect(adr, size, PAGE_EXECUTE_READWRITE, &OldProtection);
		memcpy(adr, ptr, size);
		VirtualProtect(adr, size, OldProtection, &OldProtection);
	}

	static void ReadMemory(void *adr, void *ptr, int size)
	{
		DWORD OldProtection;
		VirtualProtect(adr, size, PAGE_EXECUTE_READWRITE, &OldProtection);
		memcpy(ptr, adr, size);
		VirtualProtect(adr, size, OldProtection, &OldProtection);
	}

	//struct ThreadData
	//{
	//	DWORD  Adress;
	//	void * func;
	//	DWORD  JumpAdress;
	//};
	//static std::vector<ThreadData*> adress_list;


	static BOOL CompareMem(const BYTE* pData, const BYTE* bMask, const char* szMask)

	{
		for (; *szMask; ++szMask, ++pData, ++bMask)
			if (*szMask == 'x' && *pData != *bMask)
				return false;
		return (*szMask) == NULL;
	}
	static DWORD FindPatternMem(DWORD dwAddress, DWORD dwLen, BYTE *bMask, char * szMask)
	{
		for (DWORD i = 0; i < dwLen; i++)
		{
			__try
			{
				if (CompareMem((BYTE*)(dwAddress + i), bMask, szMask))
				{
					return (DWORD)(dwAddress + i);
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return 0;
			}
		}
		return 0;
	}

	static DWORD ScanAddressTableEX(const wchar_t* dll_name)
	{
		DWORD dwDLL = reinterpret_cast<DWORD>(GetModuleHandleW(dll_name)); //Ќаш модуль, фактически может быть любым.
		if (dwDLL)
		{
			DWORD dwAddress = FindPatternMem(dwDLL, 0x700000, (PBYTE)"\xCC\xCC\xCC\xCC\xCC\xCC\xCC", "xxxxxxx");
			if (dwAddress)
			{
				ADD_LOG("1: %S (%X)\n", dll_name, dwAddress);
				return dwAddress;
			}
			DWORD dwAddress2 = FindPatternMem(dwDLL, 0x700000, (PBYTE)"\x90\x90\x90\x90\x90\x90\x90", "xxxxxxx");
			if (dwAddress2)
			{
				ADD_LOG("2: %S (%X)\n", dll_name, dwAddress2);
				return dwAddress2;
			}
		}
		return 0;
	}

	static DWORD ScanAddressTable()
	{
		DWORD dwAddress = ScanAddressTableEX(L"ntdll.dll");

		if (!dwAddress)
			dwAddress = ScanAddressTableEX(L"KERNEL32.dll");
		if (!dwAddress)
			dwAddress = ScanAddressTableEX(L"KERNELBASE.dll");
		if (!dwAddress)
			dwAddress = ScanAddressTableEX(L"msvcrt.dll");
		if (!dwAddress)
			dwAddress = ScanAddressTableEX(L"ole32.dll");
		if (!dwAddress)
			dwAddress = ScanAddressTableEX(L"USER32.dll");
		if (!dwAddress)
			dwAddress = ScanAddressTableEX(L"SHELL32.dll");
		if (!dwAddress)
			dwAddress = ScanAddressTableEX(L"WINMM.dll");
		if (!dwAddress)
			dwAddress = ScanAddressTableEX(L"PSAPI.dll");

		return dwAddress;
	}


	static HANDLE CreateThreadMemory(void * adress_func)
	{
		__try
		{
			if (adress_func)
			{
				DWORD adress = ScanAddressTable();

				if (adress)
				{
					//с
					CHAR ByteCode[] = "\xFF\x15\x00\x00\x00\x00\xC3";

					//ThreadData * threadData = new ThreadData();

					//threadData->Adress = adress;
					//threadData->func = adress_func;
					DWORD JumpAdress = (DWORD)(adress_func);

					//ReadMemory((void*)adress, (void*)ByteCodeOld, 7);

					*(DWORD*)(&ByteCode[2]) = (DWORD)(&JumpAdress);

					WriteMemory((void*)adress, &ByteCode[0], 7);

					//adress_list.push_back(threadData);
					return CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)adress, 0, 0, 0);
				}
				else
				{
					FastCall::G().t_MessageBoxA(0, "Thred error: 1", "Error!", 0);
				}
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return NULL;
		}
		return NULL;
	}
};