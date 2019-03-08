#pragma once
#include "SDK/SDK.h"
#include "../X1API/X1FastCall.h"
#include <vector> 

using namespace SDK;
using namespace std;

namespace Engine
{
	class NetVarManager
	{
	public:
		bool Init( ClientClass* pClientClass );
		DWORD GetOffset( const char* tableName , const char* propName );
		bool HookProp( const char* tableName , const char* propName , RecvVarProxyFn function );

	private:

		DWORD dwGetProp( const char* tableName , const char* propName , RecvProp** prop = 0 );
		DWORD dwGetProp( RecvTable* recvTable , const char* propName , RecvProp** prop = 0 );
		RecvTable* GetTable( const char* tableName );
		vector<RecvTable*> m_tables;
	};

	extern NetVarManager g_NetVar;
}                         