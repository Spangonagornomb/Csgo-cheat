#include "SDK.h"
#include "../../Main.h"
//#include "../Settings/Settings.h"

namespace SDK
{
	/*
	void KeyValues::InitKetValues( KeyValues* pKeyValues , const char* name )
	{
		typedef bool( __thiscall *_InitKetValues )( KeyValues* , const char* );

		static _InitKetValues InitKetValuesFn = 0;
		static bool SearchFunction = false;

		if ( !SearchFunction )
		{
			DWORD dwFunctionAddress = CSX::Memory::FindPattern( CLIENT_DLL , "\xFF\x52\x24\x8B\x0E\x33\x4D\xFC\x81\xE1\x00\x00\x00\x00\x31\x0E\x88\x46\x03\xC1\xF8\x08\x66\x89\x46\x12\x8B\xC6" , ( "xxxxxxxxxx????xxxxxxxxxxxxxx" ) , -0x42 );

			if ( dwFunctionAddress )
			{
				InitKetValuesFn = (_InitKetValues)dwFunctionAddress;
				SearchFunction = true;
			}
		}

		if ( InitKetValuesFn && SearchFunction )
		{
			InitKetValuesFn( pKeyValues, name );
		}
	}
	*/

	bool KeyValues::LoadFromBuffer( KeyValues *pThis , const char *pszFirst , const char *pszSecond , PVOID pSomething , PVOID pAnother , PVOID pLast )
	{
		typedef bool( __thiscall *_LoadFromBuffer )( KeyValues* , const char* , const char* , PVOID , PVOID , PVOID );

		static _LoadFromBuffer LoadFromBufferFn = 0;
		static bool SearchFunction = false;

		if ( !SearchFunction )
		{
			
			DWORD dwFunctionAddress = CSX::Memory::FindPattern(GetClientDll(), ("55 8B EC 83 EC 44 53 56 57 8B F9 89 7D F8 FF 15 ? ? ? ? 8B F0 B9") , 0 );


			if ( dwFunctionAddress )
			{
				LoadFromBufferFn = (_LoadFromBuffer)dwFunctionAddress;
				SearchFunction = true;
			}
		}

		if ( LoadFromBufferFn && SearchFunction )
		{
			return LoadFromBufferFn( pThis , pszFirst , pszSecond , pSomething , pAnother , pLast );
		}

		return false;
	}
}   