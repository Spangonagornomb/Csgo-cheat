#include "CSX_Memory.h"
#include "../../X1API/X1FastCall.h"

#include <Psapi.h>
#pragma comment(lib,"psapi")

#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

MODULEINFO GetModuleInfo( PCHAR szModule )
{
	MODULEINFO modinfo = { 0 };
	HMODULE hModule = FastCall::G().t_GetModuleHandleA( szModule );

	if ( hModule == 0 )
		return modinfo;


	GetModuleInformation(FastCall::G().t_GetCurrentProcess() , hModule , &modinfo , sizeof( MODULEINFO ) );
	return modinfo;
}
#define IsInRange(x, a, b) (x >= a && x <= b)
#define GetBits(x) (IsInRange(x, '0', '9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xA))
#define GetByte(x) (GetBits(x[0]) << 4 | GetBits(x[1]))
namespace CSX
{
	namespace Memory
	{

		DWORD FindSig(DWORD dwAddress, DWORD dwLength, const char* szPattern)
		{
			if (!dwAddress || !dwLength || !szPattern)
				return 0;

			const char* pat = szPattern;
			DWORD firstMatch = NULL;

			for (DWORD pCur = dwAddress; pCur < dwLength; pCur++)
			{
				if (!*pat)
					return firstMatch;

				if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == GetByte(pat))
				{
					if (!firstMatch)
						firstMatch = pCur;

					if (!pat[2])
						return firstMatch;

					if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
						pat += 3;

					else pat += 2;
				}
				else
				{
					pat = szPattern;
					firstMatch = 0;
				}
			}

			return 0;
		}
		/* Find Push String ( 0x68, dword ptr [str] ) */

		DWORD FindPushString( DWORD dwStart , DWORD dwEnd , DWORD dwAddress )
		{
			char szPattern[5] = { 0x68 , 0x00 , 0x00 , 0x00 , 0x00 };
			*(PDWORD)&szPattern[1] = dwAddress;
			return FindPattern( szPattern , sizeof( szPattern ) , dwStart , dwEnd , 0 );
		}

		DWORD FindPushString( PCHAR szModule , DWORD dwAddress )
		{
			MODULEINFO mInfo = GetModuleInfo( szModule );

			DWORD dwStart = (DWORD)mInfo.lpBaseOfDll;
			DWORD dwSize = (DWORD)mInfo.SizeOfImage;

			return FindPushString( dwStart , dwStart + dwSize , dwAddress );
		}

		/* Code Style Use Mask \x8B\xFF\xFF\xFF\xFF x???? */

		DWORD FindPattern( PCHAR pPattern , PCHAR pszMask , DWORD dwStart , DWORD dwEnd , DWORD dwOffset )
		{
			bool bFound = false;
	
			DWORD dwPtLen = FastCall::G().t_lstrlenA( pszMask );

			#if ENABLE_DEBUG_FILE == 1
				CSX::Log::Add( "::FindPattern(%s) Init", pszMask);
			#endif

			for ( DWORD dwPtr = dwStart; dwPtr < dwEnd - dwPtLen; dwPtr++ ) {
				bFound = true;

				for ( DWORD idx = 0; idx < dwPtLen; idx++ ) {
					if ( pszMask[idx] == 'x' && pPattern[idx] != *(PCHAR)( dwPtr + idx ) )
					{
						bFound = false;
						break;
					}
				}

				if ( bFound )
				{
					#if ENABLE_DEBUG_FILE == 1
						string pPattern_str = pPattern;
						CSX::Log::Add( "::>>FindPattern(%s) = %X", pszMask, dwPtr + dwOffset - dwStart);
					#endif
					return dwPtr + dwOffset;
				}
			}

			return 0;
		}

		DWORD FindPattern( PCHAR szModule , PCHAR pPattern , PCHAR pszMask , DWORD dwOffset )
		{
			MODULEINFO mInfo = GetModuleInfo( szModule );
			DWORD dwStart = (DWORD)mInfo.lpBaseOfDll;
			DWORD dwSize = (DWORD)mInfo.SizeOfImage;

			return FindPattern( pPattern , pszMask , dwStart , dwStart + dwSize , dwOffset );
		}

		/* Code Style No Use Mask \x55\x56\xFF\x00 */

		DWORD FindPattern( PCHAR pPattern , DWORD dwPtLen , DWORD dwStart , DWORD dwEnd , DWORD dwOffset )
		{
			bool bFound = false;

			for ( DWORD dwPtr = dwStart; dwPtr < dwEnd - dwPtLen; dwPtr++ ) {
				bFound = true;

				for ( DWORD idx = 0; idx < dwPtLen; idx++ ) {
					if ( pPattern[idx] != *(PCHAR)( dwPtr + idx ) )
					{
						bFound = false;
						break;
					}
				}

				if ( bFound )
					return dwPtr + dwOffset;
			}
			return 0;
		}

		DWORD FindPattern( PCHAR szModule , PCHAR pPattern , DWORD dwPtLen , DWORD dwOffset )
		{
			MODULEINFO mInfo = GetModuleInfo( szModule );
			DWORD dwStart = (DWORD)mInfo.lpBaseOfDll;
			DWORD dwSize = (DWORD)mInfo.SizeOfImage;

			return FindPattern( pPattern , dwPtLen , dwStart , dwStart + dwSize , dwOffset );
		}

		/* Find String */

		DWORD FindString( PCHAR szModule , PCHAR pszStr )
		{
			return FindPattern( szModule , pszStr , FastCall::G().t_lstrlenA( pszStr ) , 0 );
		}

		/* IDA Style 00 FF ?? */

		DWORD FindPatternV2(std::string moduleName, std::string Mask)
		{
			const char* pat = Mask.c_str();
			DWORD firstMatch = 0;
			DWORD rangeStart = (DWORD)FastCall::G().t_GetModuleHandleA(moduleName.c_str());
			MODULEINFO miModInfo; GetModuleInformation(FastCall::G().t_GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
			DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;

			for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
			{
				if (!*pat)
					return firstMatch;

				if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
				{
					if (!firstMatch)
						firstMatch = pCur;

					if (!pat[2])
						return firstMatch;

					if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
						pat += 3;

					else
						pat += 2; //one ?
				}
				else
				{
					pat = Mask.c_str();
					firstMatch = 0;
				}
			}
			return NULL;
		}

		DWORD FindSignature(const char* szModuleName, char* szPattern)
		{
			HMODULE hModule = FastCall::G().t_GetModuleHandleA(szModuleName);
			PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hModule;
			
			if (!pDOSHeader)
				return NULL;
			PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hModule) + pDOSHeader->e_lfanew);

			DWORD ret = FindSig(((DWORD)hModule) + pNTHeaders->OptionalHeader.BaseOfCode, ((DWORD)hModule) + pNTHeaders->OptionalHeader.SizeOfCode, szPattern);;

			return ret;
		}

		DWORD FindPattern( PCHAR pPattern , DWORD dwStart , DWORD dwEnd , DWORD dwOffset )
		{
			const char* pPat = pPattern;
			DWORD dwFind = 0;
			
			for ( DWORD dwPtr = dwStart; dwPtr < dwEnd; dwPtr++ ) {
				if ( !*pPat )
					return dwFind;

				if ( *(PBYTE)pPat == '\?' || *(BYTE*)dwPtr == getByte( pPat ) )
				{
					if ( !dwFind )
						dwFind = dwPtr;

					if ( !pPat[2] )
						return dwFind + dwOffset;

					if ( *(PWORD)pPat == '\?\?' || *(PBYTE)pPat != '\?' )
					{
						pPat += 3;
					}
					else
						pPat += 2;
				}
				else
				{
					pPat = pPattern;
					dwFind = 0;
				}
			}
		

			return 0;
		}

		DWORD FindPattern( PCHAR szModule , PCHAR pPattern , DWORD dwOffset )
		{
			MODULEINFO mInfo = GetModuleInfo( szModule );		

			DWORD dwStart = (DWORD)mInfo.lpBaseOfDll;
			DWORD dwSize = (DWORD)mInfo.SizeOfImage;

			return FindPattern( pPattern , dwStart , dwStart + dwSize , dwOffset );
		}

		/* Native memory Func */

		void nt_memset( PVOID pBuffer , DWORD dwLen , DWORD dwSym )
		{
			_asm
			{
				pushad
				mov edi , [pBuffer]
				mov ecx , [dwLen]
				mov eax , [dwSym]
				rep stosb
				popad
			}
		}

		void nt_memcpy( PVOID pDst , PVOID pSrc , size_t Count )
		{
			_asm
			{
				mov	edi , [pDst]
				mov	esi , [pSrc]
				mov	ecx , [Count]
				rep	movsb
			}
		}
	}
}    

