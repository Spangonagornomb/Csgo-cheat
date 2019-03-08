#pragma once

#include "Definitions.hpp"

namespace Engine
{
	class CBaseEntity;
}

namespace SDK
{
	class IClientNetworkable;
	class IClientEntity;
	

	class IClientEntityList {
	public:
		virtual IClientNetworkable*   GetClientNetworkable( int entnum ) = 0;
		virtual IClientNetworkable*   GetClientNetworkableFromHandle( CBaseHandle hEnt ) = 0;
		virtual IClientUnknown*		  GetClientUnknownFromHandle( CBaseHandle hEnt ) = 0;
		virtual IClientEntity*        GetClientEntity( int entNum ) = 0;
		virtual IClientEntity*        GetClientEntityFromHandle(PVOID hEnt) = 0;
	//	virtual IClientEntity*        GetClientEntityFromHandle(CBaseHandle hEnt ) = 0;
		virtual int                   NumberOfEntities( bool bIncludeNonNetworkable ) = 0;
		virtual int                   GetHighestEntityIndex( void ) = 0;
		virtual void                  SetMaxEntities( int maxEnts ) = 0;
		virtual int                   GetMaxEntities() = 0;

		template <class T>
		static T GetFunction(void* instance, int index)
		{
			const auto vtable = *static_cast<void***>(instance);
			return reinterpret_cast<T>(vtable[index]);
		}
		Engine::CBaseEntity *GetClientEntityFromHandleknife(HANDLE hEnt);
		Engine::CBaseEntity *GetClientEntityFromHandle(CBaseHandle hEnt)
		{
			typedef Engine::CBaseEntity* (__thiscall* OriginalFn)(PVOID, CBaseHandle);
			return GetFunction<OriginalFn>(this, 4)(this, hEnt);
		}
	};

	class CClientEntityList
	{
	public:
		template <class T>
		static T GetFunction(void* instance, int index)
		{
			const auto vtable = *static_cast<void***>(instance);
			return reinterpret_cast<T>(vtable[index]);
		}
		Engine::CBaseEntity* GetClientEntity(int index)
		{
			typedef Engine::CBaseEntity*(__thiscall* Fn)(void*, int);
			return GetFunction<Fn>(this, 3)(this, index);
		}

		int GetHighestEntityIndex()
		{
			typedef int(__thiscall* Fn)(void*);
			return GetFunction<Fn>(this, 8)(this);
		}
		Engine::CBaseEntity *GetClientEntityFromHandle(CBaseHandle hEnt)
		{
			typedef Engine::CBaseEntity* (__thiscall* OriginalFn)(PVOID, CBaseHandle);
			return GetFunction<OriginalFn>(this, 4)(this, hEnt);
		}
		Engine::CBaseEntity *GetClientEntityFromHandleknife(HANDLE hEnt)
		{
			typedef Engine::CBaseEntity* (__thiscall* OriginalFn)(PVOID, HANDLE);
			return GetFunction<OriginalFn>(this, 4)(this, hEnt);
		}

		Engine::CBaseEntity* GetClientEntityFromHandle(void* handle)
		{
			typedef Engine::CBaseEntity*(__thiscall* Fn)(void*, void*);
			return GetFunction<Fn>(this, 4)(this, handle);
		}
		Engine::CBaseEntity* GetClientEntityFromHandleCheck(void* handle)
		{
			typedef Engine::CBaseEntity*(__thiscall* Fn)(void*, void*);
			return GetFunction<Fn>(this, 4)(this, handle);
		}
	};
}