#pragma once

#include "CSX.h"

namespace CSX
{
	namespace Memory
	{
//[swap_lines]
		/* Find Push String ( 0x68, dword ptr [str] ) */
		DWORD FindPushString( DWORD dwStart , DWORD dwEnd , DWORD dwAddress );
		DWORD FindPushString( PCHAR szModule , DWORD dwAddress );

		DWORD FindPatternV2(std::string moduleName, std::string Mask);

		DWORD FindSignature(const char* szModuleName, char* szPattern);

		/* Code Style Use Mask \x8B\xFF\xFF\xFF\xFF x???? */
		DWORD FindPattern( PCHAR pPattern , PCHAR pszMask , DWORD dwStart , DWORD dwEnd , DWORD dwOffset );
		DWORD FindPattern( PCHAR szModule , PCHAR pPattern , PCHAR pszMask , DWORD dwOffset );
		
		/* Code Style No Use Mask \x55\x56\xFF\x00 */
		DWORD FindPattern( PCHAR pPattern , DWORD dwPtLen , DWORD dwStart , DWORD dwEnd , DWORD dwOffset );
		DWORD FindPattern( PCHAR szModule , PCHAR pPattern , DWORD dwPtLen , DWORD dwOffset );

		/* Find String */
		DWORD FindString( PCHAR szModule , PCHAR pszStr );

		/* IDA Style 00 FF ?? */
		DWORD FindPattern( PCHAR pPattern , DWORD dwStart , DWORD dwEnd , DWORD dwOffset );
		DWORD FindPattern( PCHAR szModule , PCHAR pPattern , DWORD dwOffset );

		/* Native memory Func */
		void nt_memset( PVOID pBuffer , DWORD dwLen , DWORD dwSym );
		void nt_memcpy( PVOID pDst , PVOID pSrc , size_t Count );
//[/swap_lines]
	}
}