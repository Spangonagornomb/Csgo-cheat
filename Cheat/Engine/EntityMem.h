#pragma once
#include "../Main.h"
#include "Entity.h"
#include "Weapon.h"

namespace Engine
{
	enum PLAYER_TEAM;
}

using namespace Engine;
using namespace SDK;

class CEntityPlayer
{
public:

	Vector HitBoxes[21];

	Vector OldRenderOrigin;
	Vector OldHead;

	Vector RenderOrigin;
	Vector EyeAngle;
	string Name;
	string WeaponName;

	bool IsPlayer;
	bool IsDormant;
	bool IsDead;
	bool IsVisble;
	bool IsLocal;
	bool IsUpdated;
	bool IsFlashed;

	int Health;
	int Armor;
	int Money;
	int ZoomLevel;

	int Ammo1;
	int Ammo2;

	int WeaponIdx;

	float Distance;

	float Alpha;
	float HideEndTime;

	int Idx = 0;

	PLAYER_TEAM Team;

	CBaseEntity* BaseEntity;
};

class IEntityPlayers
{
protected:
	virtual void Update() = 0;
	virtual void Clear() = 0;
	virtual CEntityPlayer* FindByIdx(int Idx) = 0;
	virtual CEntityPlayer* GetByIdx(int Idx) = 0;
};

class CEntityPlayers : public IEntityPlayers
{
public:

	CEntityPlayer* EntityPlayer;
	CEntityPlayer* EntityLocal;

	CEntityPlayers()
	{
		EntityLocal = new CEntityPlayer();
		EntityPlayer = new CEntityPlayer[MAX_ENTITY_PLAYERS];
	}

	~CEntityPlayers()
	{
		delete[] EntityPlayer;

		EntityLocal = nullptr;
		EntityPlayer = nullptr;
	}

	virtual void Update();
	virtual void Clear();
	virtual CEntityPlayer* FindByIdx(int Idx);
	virtual CEntityPlayer* GetByIdx(int Idx);
};