#pragma once
#include "Definitions.hpp"

#include"IClientNetworkable.hpp"
#include"IClientRenderable.hpp"
#include"IClientUnknown.hpp"
#include"IClientThinkable.hpp"

namespace SDK
{
	struct SpatializationInfo_t;

	class IClientEntity : public IClientUnknown , public IClientRenderable , public IClientNetworkable , public IClientThinkable
	{
	public:

		virtual void             Release( void ) = 0;
		virtual const Vector     GetAbsOrigin( void ) const = 0;
		virtual const QAngle     GetAbsAngles( void ) const = 0;
		virtual void*            GetMouth( void ) = 0;
		virtual bool             GetSoundSpatialization( SpatializationInfo_t info ) = 0;
		virtual bool             IsBlurred( void ) = 0;

		template <typename Fn = void*>
		Fn get_vfunc(void* class_base, const std::size_t index)
		{
			return (*reinterpret_cast<Fn**>(class_base))[index];
		}
		void SetModelIndex(const int index)
		{
			get_vfunc<void(__thiscall*)(IClientEntity*, int)>(this, 75)(this, index);
		}

		int& GetModelIndex();
	};
}