#include "EntityMem.h"
#include "../Hacks/Setup.h"

void CEntityPlayers::Update()
{
	CBaseEntity* plocal = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());

	if (plocal)
	{
		for (int EntIndex = 0; EntIndex < MAX_ENTITY_PLAYERS; EntIndex++)
		{
			CBaseEntity* pEntity = (CBaseEntity*)I::EntityList()->GetClientEntity(EntIndex);

			if (!pEntity)
			{
				EntityPlayer[EntIndex].IsUpdated = false;
				continue;
			}

			if (!pEntity->IsPlayer())
			{
				EntityPlayer[EntIndex].IsUpdated = false;
				continue;
			}

			EntityPlayer[EntIndex].RenderOrigin = pEntity->GetRenderOrigin();

			if (EntityPlayer[EntIndex].RenderOrigin.IsZero())
			{
				EntityPlayer[EntIndex].IsUpdated = false;
				continue;
			}

			if (GP_Esp->Skeleton)
			{
				for (int i(0); i < 20; i++)
					EntityPlayer[EntIndex].HitBoxes[i] = pEntity->GetHitboxPosition(i);
			}
			else
				EntityPlayer[EntIndex].HitBoxes[HITBOX_HEAD] = pEntity->GetHitboxPosition(HITBOX_HEAD);

			EntityPlayer[EntIndex].EyeAngle = pEntity->GetEyeAngles();
			EntityPlayer[EntIndex].BaseEntity = pEntity;
			EntityPlayer[EntIndex].IsPlayer = true;
			EntityPlayer[EntIndex].IsDead = pEntity->IsDead();
			EntityPlayer[EntIndex].IsDormant = pEntity->IsDormant();
			if (!EntityPlayer[EntIndex].HitBoxes[HITBOX_HEAD].IsZero())
			{
				EntityPlayer[EntIndex].Alpha = 255.f;
				EntityPlayer[EntIndex].HideEndTime = 0.f;
			}

			EntityPlayer[EntIndex].IsVisble = pEntity->IsVisible(plocal);
			EntityPlayer[EntIndex].Name = pEntity->GetPlayerName();
			EntityPlayer[EntIndex].Idx = EntIndex;
			EntityPlayer[EntIndex].IsLocal = pEntity == plocal;
			EntityPlayer[EntIndex].Team = (PLAYER_TEAM)pEntity->GetTeam();
			EntityPlayer[EntIndex].Distance = EntityPlayer[EntIndex].RenderOrigin.DistTo(plocal->GetRenderOrigin()) / 33;
			EntityPlayer[EntIndex].Money = pEntity->GetMoney();
			EntityPlayer[EntIndex].Health = pEntity->GetHealth();
			EntityPlayer[EntIndex].Armor = pEntity->GetArmor();
			EntityPlayer[EntIndex].IsFlashed = pEntity->IsFlashed();

			CBaseWeapon* pWeaponEntity = pEntity->GetBaseWeapon();
			
			EntityPlayer[EntIndex].IsUpdated = true;

			if (EntityPlayer[EntIndex].IsLocal)
				EntityLocal = &EntityPlayer[EntIndex];
		}
	}
}

CEntityPlayer* CEntityPlayers::FindByIdx(int Idx)
{
	for (int i(0); i < MAX_ENTITY_PLAYERS; i++)
		if (Idx == EntityPlayer[i].Idx)
			return &EntityPlayer[i];
	return nullptr;
}

CEntityPlayer* CEntityPlayers::GetByIdx(int Idx)
{
	if (Idx >= 0 && Idx < MAX_ENTITY_PLAYERS)
	{
		return &EntityPlayer[Idx];
	}

	return nullptr;
}

void CEntityPlayers::Clear()
{
	memset(EntityLocal, 0, sizeof(CEntityPlayer));
	memset(EntityPlayer, 0, sizeof(CEntityPlayer) * MAX_ENTITY_PLAYERS);
}