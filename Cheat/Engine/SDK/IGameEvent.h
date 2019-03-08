#pragma once

#include "SDK.h"

namespace SDK
{
	namespace TABLE
	{
		namespace IGameEvent
		{
			enum
			{
				GetName = 1 ,
				GetInt = 6 ,
				GetFloat = 8 ,
				GetString = 9 ,
				SetString = 16
			};
		}

		namespace IGameEventManager2
		{
			enum
			{
				FireEventClientSide = 9
			};
		}
	}

	class bf_write;
	class bf_read;
	class IGameEvent
	{
	public:
		virtual ~IGameEvent() = 0;
		virtual const char*     GetName() const = 0;

		virtual bool            IsReliable() const = 0;
		virtual bool            IsLocal() const = 0;
		virtual bool            IsEmpty(const char *keyName = nullptr) = 0;

		virtual bool            GetBool(const char *keyName = nullptr, bool defaultValue = false) = 0;
		virtual int             GetInt(const char *keyName = nullptr, int defaultValue = 0) = 0;
		virtual uint64_t        GetUint64(const char *keyName = nullptr, unsigned long defaultValue = 0) = 0;
		virtual float           GetFloat(const char *keyName = nullptr, float defaultValue = 0.0f) = 0;
		virtual const char*     GetString(const char *keyName = nullptr, const char *defaultValue = "") = 0;
		virtual const wchar_t*  GetWString(const char *keyName, const wchar_t *defaultValue = L"") = 0;

		virtual void            SetBool(const char *keyName, bool value) = 0;
		virtual void            SetInt(const char *keyName, int value) = 0;
		virtual void            SetUint64(const char *keyName, unsigned long value) = 0;
		virtual void            SetFloat(const char *keyName, float value) = 0;
		//virtual void            SetString(const char *keyName, const char *value) = 0;
		virtual void            SetWString(const char *keyName, const wchar_t *value) = 0;

		void SetString(const char* szKeyName, const char* szValue)
		{
			VirtualFn(void)(PVOID, const char*, const char*);
			GetMethod< OriginalFn >(this, TABLE::IGameEvent::SetString)(this, szKeyName, szValue);
		}
	};

	class IGameEventListener2
	{
	public:
		virtual ~IGameEventListener2(void) {}

		virtual void FireGameEvent(IGameEvent *event) = 0;
		virtual int  GetEventDebugID(void) = 0;

	public:
		int m_iDebugId;
	};

	class IGameEventManager2
	{
	public:
		virtual             ~IGameEventManager2() = 0;
		virtual int         LoadEventsFromFile(const char *filename) = 0;
		virtual void        Reset() = 0;
		virtual bool        AddListener(IGameEventListener2 *listener, const char *name, bool bServerSide) = 0;
		virtual bool        FindListener(IGameEventListener2 *listener, const char *name) = 0;
		virtual int         RemoveListener(IGameEventListener2 *listener) = 0;
		virtual IGameEvent* CreateEvent_(const char *name, bool bForce, unsigned int dwUnknown) = 0;
		virtual bool        FireEvent(IGameEvent *event, bool bDontBroadcast = false) = 0;
		//virtual bool        FireEventClientSide(IGameEvent *event) = 0;

		bool FireEventClientSide(IGameEvent* pEvent)
		{
			if (sizeof(*pEvent) == NULL || sizeof(this) == NULL )
				return false;

			if(IsBadReadPtr(this, sizeof(IGameEventManager2)))
				return false;

			if (IsBadReadPtr(pEvent, sizeof(IGameEvent*)))
				return false;

			VirtualFn(bool)(PVOID, IGameEvent*);
			return GetMethod< OriginalFn >(this, TABLE::IGameEventManager2::FireEventClientSide)(this, pEvent);
		}
		virtual IGameEvent* DuplicateEvent(IGameEvent *event) = 0;
		virtual void        FreeEvent(IGameEvent *event) = 0;
		virtual bool        SerializeEvent(IGameEvent *event, bf_write *buf) = 0;
		virtual IGameEvent* UnserializeEvent(bf_read *buf) = 0;
	};

	//class IGameEvent
	//{
	//public:
	//	const char* GetName()
	//	{

	//		if (sizeof(this) == NULL)
	//			return " ";

	//		if (IsBadReadPtr(this, sizeof(IGameEvent)))
	//			return " ";

	//		VirtualFn( const char* )( PVOID );
	//		return GetMethod< OriginalFn >( this , TABLE::IGameEvent::GetName )( this );
	//	}

	//	int GetInt( const char* szKeyName , int nDefault = 0 )
	//	{
	//		VirtualFn( int )( PVOID , const char* , int );
	//		return GetMethod< OriginalFn >( this , TABLE::IGameEvent::GetInt )( this , szKeyName , nDefault );
	//	}

	//	float GetFloat( const char *szkeyName = NULL , float defaultValue = 0.0f )
	//	{
	//		VirtualFn( float )( PVOID , const char* , float );
	//		return GetMethod< OriginalFn >( this , TABLE::IGameEvent::GetFloat )( this , szkeyName , defaultValue );
	//	}

	//	const char* GetString( const char* szKeyName )
	//	{
	//		VirtualFn( const char* )( PVOID , const char* , int );
	//		return GetMethod< OriginalFn >( this , TABLE::IGameEvent::GetString )( this , szKeyName , 0 );
	//	}

	//	void SetString( const char* szKeyName , const char* szValue )
	//	{
	//		VirtualFn( void )( PVOID , const char* , const char* );
	//		GetMethod< OriginalFn >( this , TABLE::IGameEvent::SetString )( this , szKeyName , szValue );
	//	}
	//};


	//class IGameEventListener2
	//{
	//public:
	//	virtual	~IGameEventListener2(void) {};

	//	virtual void FireGameEvent(IGameEvent *event) = 0;

	//	virtual int	 GetEventDebugID(void) = 0;

	//};
	//class IGameEventManager2
	//{
	//public:
	//	bool FireEventClientSide(IGameEvent* pEvent)
	//	{
	//		if (sizeof(*pEvent) == NULL || sizeof(this) == NULL )
	//			return false;

	//		if(IsBadReadPtr(this, sizeof(IGameEventManager2)))
	//			return false;

	//		if (IsBadReadPtr(pEvent, sizeof(IGameEvent*)))
	//			return false;

	//		VirtualFn(bool)(PVOID, IGameEvent*);
	//		return GetMethod< OriginalFn >(this, TABLE::IGameEventManager2::FireEventClientSide)(this, pEvent);
	//	}

	//	/*bool AddListener(IGameEventListener2* listener, const char *name, bool bServerSide) {
	//		return CallVFunc<bool(__thiscall *)(void*, IGameEventListener2* listener, const char *name, bool bServerSide)>(this, 3)(this, listener, name, bServerSide);
	//	}*/
	//};

}                                                                                                        