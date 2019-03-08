#include "Skins.h"
#include "../Setup.h"
#include "DynSkin.h"

string pszDefaultCtModel = string(("models/weapons/v_knife_default_ct.mdl"));
string pszDefaultTtModel = string(("models/weapons/v_knife_default_t.mdl"));

string pszKnifeBayonet = string(("models/weapons/v_knife_bayonet.mdl"));
string pszKnifeFlip = string(("models/weapons/v_knife_flip.mdl"));
string pszKnifeGut = string(("models/weapons/v_knife_gut.mdl"));
string pszKnifeKarambit = string(("models/weapons/v_knife_karam.mdl"));
string pszKnifeM9Bay = string(("models/weapons/v_knife_m9_bay.mdl"));
string pszKnifeHuntsman = string(("models/weapons/v_knife_tactical.mdl"));
string pszKnifeFalchion = string(("models/weapons/v_knife_falchion_advanced.mdl"));
string pszKnifeBowie = string(("models/weapons/v_knife_survival_bowie.mdl"));
string pszKnifeButterfly = string(("models/weapons/v_knife_butterfly.mdl"));
string pszKnifeShadow = string(("models/weapons/v_knife_push.mdl"));

string pszKnifeNavaja = string(("models/weapons/v_knife_gypsy_jackknife.mdl"));
string pszKnifeStiletto = string(("models/weapons/v_knife_stiletto.mdl"));
string pszKnifeUrsus = string(("models/weapons/v_knife_ursus.mdl"));
string pszKnifeTalon = string(("models/weapons/v_knife_widowmaker.mdl"));

string pszKnifeModels[14] =
{
	pszKnifeBayonet,pszKnifeFlip,pszKnifeGut,pszKnifeKarambit,
	pszKnifeM9Bay,pszKnifeHuntsman,pszKnifeFalchion,pszKnifeBowie,
	pszKnifeButterfly,pszKnifeShadow, pszKnifeNavaja, pszKnifeStiletto,
	pszKnifeUrsus, pszKnifeTalon
};

template <typename T>
T* get_entity_from_handle(UINT h)
{
	if (h == INVALID_EHANDLE_INDEX)
		return nullptr;

	return static_cast<T*>(I::EntityList()->GetClientEntityFromHandle((PVOID)h));
}
static auto get_wearable_create_fn() -> CreateClientClassFn
{
	ClientClass* pClass = I::Client()->GetAllClasses();
	for (pClass = I::Client()->GetAllClasses(); pClass; pClass = pClass->m_pNext)
	{
		if (pClass->m_ClassID == (int)CLIENT_CLASS_ID::CEconWearable)
		{
			return pClass->m_pCreateFn;
		}
	}

	return pClass->m_pCreateFn;
}

static auto make_glove(int entry, int serial) -> CBaseAttributableItem*
{
	static auto create_wearable_fn = get_wearable_create_fn();

	create_wearable_fn(entry, serial);

	const auto glove = static_cast<CBaseAttributableItem*>(I::EntityList()->GetClientEntity(entry));
	assert(glove);
	{
		CSX::Memory::FindPattern(GetClientDll(), ("\x53\x8D\x48\x04\xE8\x00\x00\x00\x00\x8B\x4D\x10"), ("xxxxx????xxx"), 4);

		static auto set_abs_origin_addr = CSX::Memory::FindPattern(GetClientDll(), ("\x55\x8B\xEC\x83\xE4\xF8\x51\x53\x56\x57\x8B\xF1"), ("xxxxxxxxxxxx"), 0);
		const auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const std::array<float, 3>&)>(set_abs_origin_addr);

		static constexpr std::array<float, 3> new_pos = { 10000.f, 10000.f, 10000.f };

		set_abs_origin_fn(glove, new_pos);
	}

	return glove;
}

int CSkins::GetKnifeModelIdx(int ci, bool IsTT)
{
	if (ci == 0)
		return IsTT ? I::ModelInfo()->GetModelIndex(pszDefaultTtModel.c_str()) : I::ModelInfo()->GetModelIndex(pszDefaultCtModel.c_str());

	return I::ModelInfo()->GetModelIndex(pszKnifeModels[ci - 1].c_str());
}

bool ForceUpdated = false;
struct weapon_info
{
	constexpr weapon_info(const char* model, const char* icon = nullptr) :
		model(model),
		icon(icon)
	{}

	const char* model;
	const char* icon;
};

const weapon_info* get_weapon_info(int defindex)
{
	const static std::map<int, weapon_info> info =
	{
		{WEAPON_KNIFE,{"models/weapons/v_knife_default_ct.mdl", "knife_default_ct"}},
		{WEAPON_KNIFE_T,{"models/weapons/v_knife_default_t.mdl", "knife_t"}},
		{WEAPON_KNIFE_BAYONET, {"models/weapons/v_knife_bayonet.mdl", "bayonet"}},
		{WEAPON_KNIFE_FLIP, {"models/weapons/v_knife_flip.mdl", "knife_flip"}},
		{WEAPON_KNIFE_GUT, {"models/weapons/v_knife_gut.mdl", "knife_gut"}},
		{WEAPON_KNIFE_KARAMBIT, {"models/weapons/v_knife_karam.mdl", "knife_karambit"}},
		{WEAPON_KNIFE_M9_BAYONET, {"models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet"}},
		{WEAPON_KNIFE_TACTICAL, {"models/weapons/v_knife_tactical.mdl", "knife_tactical"}},
		{WEAPON_KNIFE_FALCHION, {"models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion"}},
		{WEAPON_KNIFE_SURVIVAL_BOWIE, {"models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie"}},
		{WEAPON_KNIFE_BUTTERFLY, {"models/weapons/v_knife_butterfly.mdl", "knife_butterfly"}},
		{WEAPON_KNIFE_PUSH, {"models/weapons/v_knife_push.mdl", "knife_push"}},
		{WEAPON_KNIFE_URSUS,{"models/weapons/v_knife_ursus.mdl", "knife_ursus"}},
		{WEAPON_KNIFE_JACK,{"models/weapons/v_knife_gypsy_jackknife.mdl", "knife_gypsy_jackknife"}},
		{WEAPON_KNIFE_STILETTO,{"models/weapons/v_knife_stiletto.mdl", "knife_stiletto"}},
		{WEAPON_KNIFE_WIDOWMAKER,{"models/weapons/v_knife_widowmaker.mdl", "knife_widowmaker"}},
		{GLOVE_STUDDED_BLOODHOUND,{"models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"}},
		{GLOVE_T_SIDE,{"models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl"}},
		{GLOVE_CT_SIDE,{"models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl"}},
		{GLOVE_SPORTY,{"models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"}},
		{GLOVE_SLICK,{"models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"}},
		{GLOVE_LEATHER_WRAP,{"models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"}},
		{GLOVE_MOTORCYCLE,{"models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"}},
		{GLOVE_SPECIALIST,{"models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"}},
		{GLOVE_HYDRA,{"models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl"}}
	};

	const auto entry = info.find(defindex);
	return entry == end(info) ? nullptr : &entry->second;
}

void CSkins::RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	if (CGlobal::IsGameReady)
	{
		// Get the knife view model id's
		int default_t = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_default_t.mdl");
		int default_ct = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
		int iBayonet = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
		int iButterfly = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
		int iFlip = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_flip.mdl");
		int iGut = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_gut.mdl");
		int iKarambit = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_karam.mdl");
		int iM9Bayonet = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
		int iHuntsman = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_tactical.mdl");
		int iFalchion = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
		int iDagger = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_push.mdl");
		int iBowie = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
		int iGunGame = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_gg.mdl");

		int Navaja = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_gypsy_jackknife.mdl");
		int Stiletto = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_stiletto.mdl");
		int Ursus = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_ursus.mdl");
		int Talon = I::ModelInfo()->GetModelIndex("models/weapons/v_knife_widowmaker.mdl");

		CBaseEntity* pLocal = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());
		if (pLocal)
		{
			bool IsTT = pLocal->GetTeam() == 2;
			int sel_mod = IsTT ? SelectedKnifeModelTT : SelectedKnifeModelCT;

			if (!pLocal->IsDead() && sel_mod && (
				pData->m_Value.m_Int == default_t ||
				pData->m_Value.m_Int == default_ct ||
				pData->m_Value.m_Int == iBayonet ||
				pData->m_Value.m_Int == iFlip ||
				pData->m_Value.m_Int == iGunGame ||
				pData->m_Value.m_Int == iGut ||
				pData->m_Value.m_Int == iKarambit ||
				pData->m_Value.m_Int == iM9Bayonet ||
				pData->m_Value.m_Int == iHuntsman ||
				pData->m_Value.m_Int == iBowie ||
				pData->m_Value.m_Int == iButterfly ||
				pData->m_Value.m_Int == iFalchion ||
				pData->m_Value.m_Int == iDagger ||
				pData->m_Value.m_Int == Navaja ||
				pData->m_Value.m_Int == Stiletto ||
				pData->m_Value.m_Int == Ursus ||
				pData->m_Value.m_Int == Talon))
			{
				int sel_mod = IsTT ? SelectedKnifeModelTT : SelectedKnifeModelCT;
				int mdl_idx = GetKnifeModelIdx(sel_mod, IsTT);

				pData->m_Value.m_Int = mdl_idx;
			}
		}
	}
}

void CSkins::PostDataUpdate()
{
	HANDLE worldmodel_handle = 0;

	CBaseEntity* worldmodel = nullptr;

	int nLocalPlayerID = I::Engine()->GetLocalPlayer();

	CBaseEntity* pLocal = (CBaseEntity*)I::ClientEntityList()->GetClientEntity(nLocalPlayerID);

	if (!pLocal)
		return;

	PlayerInfo LocalPlayerInfo;
	if (!I::Engine()->GetPlayerInfo(nLocalPlayerID, &LocalPlayerInfo))
		return;

	bool IsTT = pLocal->GetTeam() == 2;

	auto weapons = pLocal->m_hMyWeapons();
	for (size_t i = 0; weapons[i] != INVALID_EHANDLE_INDEX; i++)
	{
		CBaseEntity *pEntity = (CBaseEntity*)I::ClientEntityList()->GetClientEntityFromHandle(weapons[i]);
		if (pEntity)
		{
			CBaseWeapon* pWeapon = (CBaseWeapon*)pEntity;
			CBaseAttributableItem* pAttrib = pWeapon->GeteAttributableItem();

			if (!pAttrib)
				continue;

			if (LocalPlayerInfo.m_nXuidLow != *pAttrib->GetOriginalOwnerXuidLow())
				continue;

			if (LocalPlayerInfo.m_nXuidHigh != *pAttrib->GetOriginalOwnerXuidHigh())
				continue;

			if (pEntity->GetClientClass()->m_ClassID == 93)
			{
				int sel_mod = IsTT ? SelectedKnifeModelTT : SelectedKnifeModelCT;
				if (sel_mod)
				{
					int mdl_idx = GetKnifeModelIdx(sel_mod, IsTT);
					worldmodel_handle = pWeapon->GetWeaponWorldModel();
					if (worldmodel_handle)
						worldmodel = (CBaseEntity*)I::ClientEntityList()->GetClientEntityFromHandleknife(worldmodel_handle);

					*pWeapon->ModelIndex() = mdl_idx;
					*pWeapon->ViewModelIndex() = mdl_idx;

					if (worldmodel)
						*pWeapon->ModelIndex() = mdl_idx + 1;

					*pWeapon->fixskins() = KnifeNames[sel_mod].ID;
					*pAttrib->GetEntityQuality() = 3;
				}
			}

			ApplyCustomSkin(pAttrib, *pAttrib->GetItemDefinitionIndex(), IsTT);

			*pWeapon->OwnerXuidLow() = 0;
			*pWeapon->OwnerXuidHigh() = 0;
			*pWeapon->fixItemIDHigh() = 1;
		}
	}

	int CurGlove = 0;

	if (pLocal->GetTeam() == 2)
		CurGlove = SelectedGloveTT;
	else
		CurGlove = SelectedGloveCT;

	if (CurGlove)
	{
		UINT* wearables = pLocal->GetWearables();

		if (!wearables)
			return;
		static auto glove_handle = UINT(0);

		auto glove = (CBaseAttributableItem*)I::EntityList()->GetClientEntityFromHandle((PVOID)wearables[0]);
		if (!glove)
		{
			const auto our_glove = (CBaseAttributableItem*)I::EntityList()->GetClientEntityFromHandle((PVOID)glove_handle);

			if (our_glove)
			{
				wearables[0] = glove_handle;
				glove = our_glove;
			}
		}
		if (pLocal->IsDead() || pLocal->GetHealth() <= 0)
		{
			if (glove)
			{
				glove->GetClientNetworkable()->SetDestroyedOnRecreateEntities();
				glove->GetClientNetworkable()->Release();
			}
			return;
		}

		if (!glove)
		{
			const auto entry = I::EntityList()->GetHighestEntityIndex() + 1;
			const auto serial = rand() % 0x1000;
			glove = make_glove(entry, serial);
			wearables[0] = entry | serial << 16;
			glove_handle = wearables[0];
		}

		if (glove)
		{
			*glove->GetIndex() = -1;

			*glove->GetItemIDHigh() = -1;
			*glove->GetAccountID() = LocalPlayerInfo.m_nXuidLow;

			*glove->GetEntityQuality() = 4;
			*glove->GetFallbackWear() = 0.00000001f;
			*glove->GetFallbackSeed() = 0;
			*glove->GetFallbackStatTrak() = -1;
			*glove->GetFallbackPaintKit() = GlovesSkin_Array[CurGlove - 1].PaintKit;

			auto& definition_index = *glove->GetItemDefinitionIndex();

			if (GlovesSkin_Array[CurGlove - 1].ItemIndex != definition_index)
			{
				int ModelIndex = I::ModelInfo()->GetModelIndex(GlovesSkin_Array[CurGlove - 1].szModel);

				*glove->GetItemDefinitionIndex() = GlovesSkin_Array[CurGlove - 1].ItemIndex;

				glove->SetModelIndex(ModelIndex);
				glove->GetClientNetworkable()->PreDataUpdate(DATA_UPDATE_CREATED);
			}
		}
	}
}

void CSkins::SetKillIconCfg()
{
	char* pszKnifeModelsIcon[14] =
	{
		"bayonet","knife_flip","knife_gut","knife_karambit",
		"knife_m9_bayonet","knife_tactical","knife_falchion",
		"knife_survival_bowie","knife_butterfly","knife_push",
		"knife_gypsy_jackknife", "knife_stiletto", "knife_ursus", "knife_widowmaker"
	};

	if (SelectedKnifeModelCT)
	{
		g_KillIconCfg["knife"] = pszKnifeModelsIcon[SelectedKnifeModelCT - 1];
	}
	else
	{
		g_KillIconCfg["knife"] = "knife";
	}

	if (SelectedKnifeModelTT)
	{
		g_KillIconCfg["knife_t"] = pszKnifeModelsIcon[SelectedKnifeModelTT - 1];
	}
	else
	{
		g_KillIconCfg["knife_t"] = "knife_t";
	}
}

void CSkins::ApplyCustomModel(CBaseEntity* pLocal, CBaseAttributableItem* pWeapon)
{
	CBaseViewModel* pViewModel = pLocal->GetViewModel();

	if (!pViewModel)
		return;

	DWORD hViewModelWeapon = pViewModel->GetWeapon();

	if (!hViewModelWeapon)
		return;

	CBaseAttributableItem* pViewModelWeapon = (CBaseAttributableItem*)I::EntityList()->GetClientEntityFromHandle((PVOID)hViewModelWeapon);

	if (pViewModelWeapon != pWeapon)
		return;

	int nViewModelIndex = pViewModel->GetModelIndex();

	if (!nViewModelIndex)
		return;

	if (g_ViewModelCfg.find(nViewModelIndex) == g_ViewModelCfg.end())
		return;

	pViewModel->SetModelIndex(I::ModelInfo()->GetModelIndex(g_ViewModelCfg[nViewModelIndex]));
}

bool CSkins::IsKnife(int idx)
{
	if ((WEAPON_ID)idx == WEAPON_KNIFE || (WEAPON_ID)idx == WEAPON_KNIFE_T ||
		(WEAPON_ID)idx == WEAPON_KNIFE_BAYONET || (WEAPON_ID)idx == WEAPON_KNIFE_FLIP ||
		(WEAPON_ID)idx == WEAPON_KNIFE_GUT || (WEAPON_ID)idx == WEAPON_KNIFE_KARAMBIT ||
		(WEAPON_ID)idx == WEAPON_KNIFE_M9_BAYONET || (WEAPON_ID)idx == WEAPON_KNIFE_TACTICAL ||
		(WEAPON_ID)idx == WEAPON_KNIFE_SURVIVAL_BOWIE || (WEAPON_ID)idx == WEAPON_KNIFE_BUTTERFLY ||
		(WEAPON_ID)idx == WEAPON_KNIFE_PUSH || (WEAPON_ID)idx == WEAPON_KNIFE_FALCHION ||
		(WEAPON_ID)idx == WEAPON_KNIFE_JACK || (WEAPON_ID)idx == WEAPON_KNIFE_STILETTO ||
		(WEAPON_ID)idx == WEAPON_KNIFE_URSUS || (WEAPON_ID)idx == WEAPON_KNIFE_WIDOWMAKER)
	{
		return true;
	}

	return false;
}
void CSkins::ApplyCustomSkin(CBaseAttributableItem* pWeapon, int nWeaponIndex, bool IsTT)
{
	int ListIdx = 0;

	bool bIsKnife = IsKnife(nWeaponIndex);

	if (!GetWeaponByDefIdx(nWeaponIndex, ListIdx, bIsKnife, IsTT))
		return;

	ItemSettings *Item = bIsKnife ? (IsTT ? &KnifeNamesTT[SelectedKnifeModelTT] : &KnifeNames[SelectedKnifeModelCT]) : &WeaponNames[ListIdx];

	if (!Item)
		return;

	if (Item->IsInventory && !GP_Inventory->SkinsSyncEnable)
		return;

	if (Item->IsInventory)
	{
		if (Item->Skin.paint_kit_id != 0 && !IsTT)
			SetSkin(pWeapon, &Item->Skin, Item->ID, false, bIsKnife);
		if (Item->SkinTT.paint_kit_id != 0 && IsTT)
			SetSkin(pWeapon, &Item->SkinTT, Item->ID, false, bIsKnife);
	}
	else
		SetSkin(pWeapon, &Item->Skin, Item->ID, false, bIsKnife);
}

void CSkins::SetSkin(CBaseAttributableItem* pWeapon, SkinSettings *SkinParam, int id, bool IsCT, bool IsKnife)
{
	*pWeapon->GetItemIDHigh() = -1;

	*pWeapon->GetFallbackPaintKit() = SkinParam->paint_kit_id;
	*pWeapon->GetEntityQuality() = SkinParam->quality;
	if (SkinParam->stat_track != 0)
		*pWeapon->GetFallbackStatTrak() = SkinParam->stat_track;

	*pWeapon->GetFallbackWear() = SkinParam->wear;

	if (SkinParam->seed)
		*pWeapon->GetFallbackSeed() = SkinParam->seed;

	if (SkinParam->custom_name)
		snprintf(pWeapon->GetCustomName(), 32, "%s", SkinParam->custom_name);

	if (*pWeapon->GetItemDefinitionIndex() != id)
	{
		*pWeapon->GetItemDefinitionIndex() = id;

		if (IsKnife)
			pWeapon->SetModelIndex(GetKnifeModelIdx(IsCT ? SelectedKnifeModelCT : SelectedKnifeModelTT, !IsCT));

		pWeapon->GetClientNetworkable()->PreDataUpdate(DATA_UPDATE_CREATED);
		pWeapon->OnDataChanged(DATA_UPDATE_CREATED);
	}
};

bool CSkins::GetWeaponByDefIdx(int Idx, int &ListIdx, bool IsKnife, bool IsTT)
{
	if (!IsKnife)
	{
		for (size_t i(0); i < WeaponNames.size(); i++)
		{
			if (WeaponNames[i].ID == (WEAPON_ID)Idx)
			{
				ListIdx = i;
				return true;
			}
		}
	}
	else
	{
		for (size_t i(0); i < KnifeNames.size(); i++)
		{
			if (KnifeNames[i].ID == (WEAPON_ID)Idx)
			{
				ListIdx = i;
				return true;
			}
		}
	}
	return false;
}

struct hud_weapons_t {
	std::int32_t* get_weapon_count() {
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};

template<class T>
static T* FindHudElement(const char* name)
{
	static auto pThis = *reinterpret_cast<DWORD**>(CSX::Memory::FindPatternV2(("client_panorama.dll"), ("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08")) + 1);
	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(CSX::Memory::FindPatternV2(("client_panorama.dll"), ("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
	return (T*)find_hud_element(pThis, name);
}

void CSkins::UpdateSkins(bool reset)
{
	CGlobal::FullUpdateCheck = true;

	SetKillIconCfg();

	typedef void(*ForceUpdate) (void);
	ForceUpdate FullUpdate = (ForceUpdate)CSX::Memory::FindSignature(ENGINE_DLL, FORCE_FULL_UPDATE_PATTERN);
	FullUpdate();

	ForceUpdated = reset;
	ADD_LOG("ForceFullUpdate\n");
	CGlobal::FullUpdateCheck = false;
}

void CSkinListener::RegListener()
{
	I::GameEvent()->AddListener(this, ("player_death"), false);
	I::GameEvent()->AddListener(this, ("player_spawn"), false);
}

void CSkinListener::UnRegListener()
{
	I::GameEvent()->RemoveListener(this);
}

void ForceItemUpdate(CBaseWeapon* Weapon)
{
	Weapon->GetClientNetworkable()->PostDataUpdate(DATA_UPDATE_CREATED);
	Weapon->GetClientNetworkable()->OnDataChanged(DATA_UPDATE_CREATED);
}

void CSkinListener::FireGameEvent(IGameEvent *event)
{
	if (GP_Skins)
	{
		if (!GP_Skins->SkinsEnable)
			return;
		if (!strcmp(event->GetName(), ("player_spawn")))
		{
			if (I::Engine()->GetPlayerForUserID(event->GetInt(("userid"))) == I::Engine()->GetLocalPlayer())
			{

			}
		}
		if (!strcmp(event->GetName(), ("player_death")))
		{
			int nUserID = event->GetInt(("attacker"));

			if (!nUserID)
				return;

			if (I::Engine()->GetPlayerForUserID(nUserID) != I::Engine()->GetLocalPlayer())
				return;

			if (GP_Skins->SelectedWeapon < (int)GP_Skins->WeaponNames.size() && CGlobal::GWeaponType != WEAPON_TYPE_UNKNOWN)
			{
				CBaseEntity* local = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());
				if (local)
				{
					bool IsTT = local->GetTeam() == 2;
					if (CGlobal::GWeaponType == WEAPON_TYPE_KNIFE)
					{
						int ListIdx = 0;
						if (GP_Skins->GetWeaponByDefIdx((int)CGlobal::GWeaponID, ListIdx, true, IsTT))
						{
							CSkins::ItemSettings* WItem = (IsTT ? &GP_Skins->KnifeNamesTT[ListIdx] : &GP_Skins->KnifeNames[ListIdx]);
							if (WItem->Skin.auto_stat_track)
							{
								WItem->Skin.stat_track++;
								ForceItemUpdate(local->GetBaseWeapon());
							}
						}
					}
					else
					{
						CSkins::ItemSettings* WItem = &GP_Skins->WeaponNames[GP_Skins->SelectedWeapon];
						if (WItem->IsInventory)
						{
							if (IsTT)
							{
								if (WItem->SkinTT.auto_stat_track)
								{
									WItem->SkinTT.stat_track++;
									ForceItemUpdate(local->GetBaseWeapon());
								}
							}
							else
							{
								if (WItem->Skin.auto_stat_track)
								{
									WItem->Skin.stat_track++;
									ForceItemUpdate(local->GetBaseWeapon());
								}
							}
						}
						else
						{
							if (WItem->Skin.auto_stat_track)
							{
								WItem->Skin.stat_track++;
								ForceItemUpdate(local->GetBaseWeapon());
							}
						}
					}
				}
			}

			const char* szWeapon = event->GetString(("weapon"));

			for (auto ReplacementIcon : GP_Skins->g_KillIconCfg)
			{
				if (!strcmp(szWeapon, ReplacementIcon.first))
				{
					event->SetString(("weapon"), ReplacementIcon.second);
					break;
				}
			}

			szWeapon = event->GetString(("weapon"));
		}
		return;
	}
}

int CSkinListener::GetEventDebugID(void)
{
	return 0x2A;
}

enum class EStickerAttributeType
{
	Index,
	Wear,
	Scale,
	Rotation
};
DWORD dwEconItemInterfaceWrapper = 0;

typedef float(__thiscall* GetStickerAttributeBySlotIndexFloatFn)(void*, int, EStickerAttributeType, float);
GetStickerAttributeBySlotIndexFloatFn oGetStickerAttributeBySlotIndexFloat;

void CSkins::ApplyStickerHooks(CBaseAttributableItem* pItem) {}
int CSkins::StickWeaponByDefIndex(int DefIdx)
{
	for (size_t i(0); i < WeaponNames.size(); i++)
	{
		if (WeaponNames[i].ID == (WEAPON_ID)DefIdx)
			return i;
	}
	return 0;
}

void CSkins::SaveSkins(nlohmann::json &j)
{
#define SV(o,a,b) j[("Skins")][o][v.Name][a] = b;

	StickerSettings emptyEntryStricker;
	SkinSettings emptyEntry;

	auto SaveItem = [&](string sname, vector<ItemSettings> &It, bool SaveSticker = false) -> void
	{
		for (auto &v : It)
		{
			if (emptyEntry == v.Skin)
			{
				SV(sname, ("Emp"), true);
				continue;
			}

			SV(sname, ("custom_name"), v.Skin.custom_name);
			SV(sname, ("paint_kit_id"), v.Skin.paint_kit_id);
			SV(sname, ("paint_kit_menu"), v.Skin.paint_kit_menu);
			SV(sname, ("skins_mode"), v.Skin.skins_mode);
			SV(sname, ("quality"), v.Skin.quality);
			SV(sname, ("seed"), v.Skin.seed);
			SV(sname, ("stat_track"), v.Skin.stat_track);
			SV(sname, ("wear"), v.Skin.wear);
			if (SaveSticker)
			{
				SV(sname, ("striker_enable"), v.Skin.striker_enable);
				for (int i(0); i < 5; i++)
				{
					if (emptyEntryStricker == v.Skin.Stickers[i])
					{
						SV(sname, string(("SRE") + to_string(i)), true);
						continue;
					}

					SV(sname, string(("Sticker") + to_string(i) + ("kit")), v.Skin.Stickers[i].kit);
					SV(sname, string(("Sticker") + to_string(i) + ("kit_menu_index")), v.Skin.Stickers[i].kit_menu_index);
					SV(sname, string(("Sticker") + to_string(i) + ("rotation")), v.Skin.Stickers[i].rotation);
					SV(sname, string(("Sticker") + to_string(i) + ("scale")), v.Skin.Stickers[i].scale);
					SV(sname, string(("Sticker") + to_string(i) + ("wear")), v.Skin.Stickers[i].wear);
				}
			}
		}
	};

	SaveItem(("Weapons"), WeaponNames, true);
	SaveItem(("KnifesCT"), KnifeNames);
	SaveItem(("KnifesTT"), KnifeNamesTT);
}

void CSkins::LoadSkins(nlohmann::json &j)
{
#define LV(o,a,b) if(!j[("Skins")][o][v.Name][a].is_null()) {b = j[("Skins")][o][v.Name][a];}

	StickerSettings emptyEntryStricker;
	SkinSettings emptyEntry;

	auto LoadItem = [&](string sname, vector<ItemSettings> &It, bool LoadSticker = false) -> void
	{
		if (!j[("Skins")][sname].is_null())
		{
			for (auto &v : It)
			{
				if (!j[("Skins")][sname][v.Name].is_null())
				{
					bool IsEmpty = false;
					LV(sname, "Emp", IsEmpty);
					if (IsEmpty)
					{
						v.Skin = emptyEntry;
						continue;
					}

					if (!j[("Skins")][sname][v.Name][("custom_name")].is_null())
					{
						string buf = j[("Skins")][sname][v.Name][("custom_name")].get<string>();
						for (int i(0); i < 32; i++)
							v.Skin.custom_name[i] = '\0';
						for (int i(0); i < (((int)buf.length() >= 32) ? 32 : (int)buf.length()); i++)
							v.Skin.custom_name[i] = buf[i];
					}

					LV(sname, "paint_kit_id", v.Skin.paint_kit_id);
					LV(sname, "paint_kit_menu", v.Skin.paint_kit_menu);
					LV(sname, "skins_mode", v.Skin.skins_mode);
					LV(sname, "quality", v.Skin.quality);
					LV(sname, "seed", v.Skin.seed);
					LV(sname, "stat_track", v.Skin.stat_track);
					LV(sname, "wear", v.Skin.wear);
					if (LoadSticker)
					{
						LV(sname, "striker_enable", v.Skin.striker_enable);
						for (int i(0); i < 5; i++)
						{
							bool IsEmptyStick = false;
							LV(sname, string(("SRE") + to_string(i)), IsEmptyStick);
							if (IsEmptyStick)
							{
								v.Skin.Stickers[i] = emptyEntryStricker;
								continue;
							}

							LV(sname, string("Sticker" + to_string(i) + "kit"), v.Skin.Stickers[i].kit);
							LV(sname, string("Sticker" + to_string(i) + "kit_menu_index"), v.Skin.Stickers[i].kit_menu_index);
							LV(sname, string("Sticker" + to_string(i) + "rotation"), v.Skin.Stickers[i].rotation);
							LV(sname, string("Sticker" + to_string(i) + "scale"), v.Skin.Stickers[i].scale);
							LV(sname, string("Sticker" + to_string(i) + "wear"), v.Skin.Stickers[i].wear);
						}
					}
				}
			}
		}
	};
	if (!j[("Skins")].is_null())
	{
		LoadItem(("Weapons"), WeaponNames, true);
		LoadItem(("KnifesCT"), KnifeNames);
		LoadItem(("KnifesTT"), KnifeNamesTT);
	}
}

#define lol(e) string((e))

#define WEAPON_DEAGLE_STR		lol("deagle")		// 0
#define WEAPON_ELITE_STR		lol("elite")		// 1
#define WEAPON_FIVESEVEN_STR	lol("fiveseven")	// 2
#define WEAPON_GLOCK_STR		lol("glock"	)		// 3
#define WEAPON_TASER_STR		lol("taser")
#define WEAPON_HKP2000_STR		lol("hkp2000")		// 4
#define WEAPON_P250_STR			lol("p250")			// 5
#define WEAPON_USP_S_STR		lol("usp_silencer")	// 6
#define WEAPON_CZ75A_STR		lol("cz75a"	)		// 7
#define WEAPON_REVOLVER_STR		lol("revolver")		// 8
#define WEAPON_TEC9_STR			lol("tec9")			// 9
#define WEAPON_AK47_STR		lol("ak47")				// 10
#define WEAPON_AUG_STR		lol("aug")				// 11
#define WEAPON_FAMAS_STR	lol("famas")			// 12
#define WEAPON_GALILAR_STR	lol("galilar")			// 13
#define WEAPON_M249_STR		lol("m249")				// 14
#define WEAPON_M4A1_STR		lol("m4a1")				// 15
#define WEAPON_M4A1_S_STR	lol("m4a1_silencer")	// 16
#define WEAPON_MAC10_STR	lol("mac10")			// 17
#define WEAPON_P90_STR		lol("p90")				// 18
#define WEAPON_UMP45_STR	lol("ump45")			// 19
#define WEAPON_MP5_STR		lol("mp5sd")			// 20
#define WEAPON_XM1014_STR	lol("xm1014")			// 20+1
#define WEAPON_BIZON_STR	lol("bizon")			// 21
#define WEAPON_MAG7_STR		lol("mag7")				// 22
#define WEAPON_NEGEV_STR	lol("negev")			// 23
#define WEAPON_SAWEDOFF_STR	lol("sawedoff")			// 24
#define WEAPON_MP7_STR		lol("mp7")				// 25
#define WEAPON_MP9_STR		lol("mp9")				// 26
#define WEAPON_NOVA_STR		lol("nova")				// 27
#define WEAPON_SG553_STR	lol("sg556")			// 28
#define WEAPON_G3SG1_STR	lol("g3sg1")			// 29
#define WEAPON_SCAR20_STR	lol("scar20")			// 30
#define WEAPON_AWP_STR		lol("awp")				// 31
#define WEAPON_SSG08_STR	lol("ssg08")			// 32
string pWeaponData[34] =
{
	WEAPON_DEAGLE_STR,WEAPON_ELITE_STR,WEAPON_FIVESEVEN_STR,
	WEAPON_GLOCK_STR,WEAPON_HKP2000_STR,WEAPON_P250_STR,
	WEAPON_USP_S_STR,WEAPON_CZ75A_STR,WEAPON_REVOLVER_STR,
	WEAPON_TEC9_STR,
	WEAPON_AK47_STR,WEAPON_AUG_STR,WEAPON_FAMAS_STR,WEAPON_GALILAR_STR,
	WEAPON_M249_STR,WEAPON_M4A1_STR,WEAPON_M4A1_S_STR,WEAPON_MAC10_STR,
	WEAPON_P90_STR,WEAPON_UMP45_STR,WEAPON_MP5_STR,WEAPON_XM1014_STR,WEAPON_BIZON_STR,
	WEAPON_MAG7_STR,WEAPON_NEGEV_STR,WEAPON_SAWEDOFF_STR,
	WEAPON_MP7_STR,WEAPON_MP9_STR,WEAPON_NOVA_STR,WEAPON_SG553_STR,
	WEAPON_SCAR20_STR,WEAPON_G3SG1_STR,
	WEAPON_AWP_STR,WEAPON_SSG08_STR
};

string pKnifeData[14] =
{
	lol("knife_m9_bayonet"),lol("knife_flip"),lol("knife_gut"),lol("knife_karambit") ,lol("knife_m9_bayonet"),
	lol("knife_tactical"),lol("knife_falchion"),lol("knife_survival_bowie"),lol("knife_butterfly"),lol("knife_push"),
	lol("knife_gypsy_jackknife"), lol("knife_stiletto"), lol("knife_ursus"), lol("knife_widowmaker")
};

void CSkins::ParseSortedKits()
{
	auto pSkins = new DynSkin::Skins::CSkins();

	string KnifeModels[] =
	{
		lol("Bayonet"),lol("Flip"),lol("Gut"),lol("Karambit") ,lol("M9 Bayonet"),
		lol("Huntsman"),lol("Falchion"),lol("Bowie"),lol("Butterfly"),lol("Shadow Daggers"), lol("Navaja Knife"), lol("Stiletto Knife"),
		lol("Ursus Knife"), lol("Talon Knife")
	};

	string CsgoPatch = CSX::Utils::GetModuleBaseDir(0) + ("\\csgo");

	if (pSkins->Load(CsgoPatch, ("csgo")))
	{
		for (int iWeaponIndex = 0; iWeaponIndex < sizeof(pWeaponData) / sizeof(*pWeaponData); iWeaponIndex++)
		{
			string WeaponSearch = ("weapon_") + string(pWeaponData[iWeaponIndex]);
			DynSkin::Skins::vecSkinInfo SkinInfoWeapon = pSkins->GetSkinInfoByWeapon(WeaponSearch);

			SortedWeapons_s WeaponEntry;

			WeaponEntry.name = CGlobal::WeaponNames[iWeaponIndex];
			WeaponEntry.kits.push_back(SortedKits_s());

			for (size_t iSkinWeaponIndex = 0; iSkinWeaponIndex < SkinInfoWeapon.size(); iSkinWeaponIndex++)
			{
				if (SkinInfoWeapon[iSkinWeaponIndex].nFallbackPaintKit >= 10000)
					continue;

				string DragonKing = ("Dragon King");
				string FullName = SkinInfoWeapon[iSkinWeaponIndex]._name;

				if (SkinInfoWeapon[iSkinWeaponIndex].nFallbackPaintKit == 400 && SkinInfoWeapon[iSkinWeaponIndex]._name.find(DragonKing) != string::npos)
					FullName = DragonKing;

				SortedKits_s KitEntry;

				KitEntry.kit = SkinInfoWeapon[iSkinWeaponIndex].nFallbackPaintKit;
				KitEntry.name = FullName;

				WeaponEntry.kits.push_back(KitEntry);
			}

			SortedWeapons.push_back(WeaponEntry);
		}

		SortedWeapons_s DefKnifeEntry;

		DefKnifeEntry.name = ("Default");
		DefKnifeEntry.kits.push_back(SortedKits_s());
		SortedKnives.push_back(DefKnifeEntry);
		for (int iKnifeIndex = 0; iKnifeIndex < sizeof(pKnifeData) / sizeof(*pKnifeData); iKnifeIndex++)
		{
			string KnifeSearch = ("weapon_") + string(pKnifeData[iKnifeIndex]);
			DynSkin::Skins::vecSkinInfo SkinInfoKnife = pSkins->GetSkinInfoByWeapon(KnifeSearch);

			SortedWeapons_s KnifeEntry;
			KnifeEntry.name = KnifeModels[iKnifeIndex];
			KnifeEntry.kits.push_back(SortedKits_s());

			for (size_t iSkinKnifeIndex = 0; iSkinKnifeIndex < SkinInfoKnife.size(); iSkinKnifeIndex++)
			{
				SortedKits_s KitEntry;
				KitEntry.kit = SkinInfoKnife[iSkinKnifeIndex].nFallbackPaintKit;
				KitEntry.name = SkinInfoKnife[iSkinKnifeIndex]._name;
				KitEntry.cdn_name = SkinInfoKnife[iSkinKnifeIndex]._shortname;
				KnifeEntry.kits.push_back(KitEntry);
			}
			SortedKnives.push_back(KnifeEntry);
		}

		pSkins->Release();
	}

	delete pSkins;
	ADD_LOG("Setup: Sorted Skins & Stickers sucessful\n");
}

void CSkins::PrepareSortedSkins()
{
	for (size_t i(0); i < SortedWeapons.size(); i++)
	{
		for (size_t j(0); j < SortedWeapons[i].kits.size(); j++)
		{
			for (size_t k(0); k < skin_kits.size(); k++)
			{
				if (skin_kits[k].id == SortedWeapons[i].kits[j].kit)
				{
					SortedWeapons[i].kits[j].name_rus = skin_kits[k].name;
					skin_kits[k].name_eng = SortedWeapons[i].kits[j].name;

					SortedWeapons[i].kits[j].cdn_name = skin_kits[k].cdn_name;
					break;
				}
			}
		}
	}

	for (size_t i(0); i < SortedKnives.size(); i++)
	{
		for (size_t j(0); j < SortedKnives[i].kits.size(); j++)
		{
			for (size_t k(0); k < skin_kits.size(); k++)
			{
				if (skin_kits[k].id == SortedKnives[i].kits[j].kit)
				{
					SortedKnives[i].kits[j].name_rus = skin_kits[k].name;
					skin_kits[k].name_eng = SortedKnives[i].kits[j].name;

					break;
				}
			}
		}
	}

	SortedStickers_s SSEntry;
	SSEntry.sub_name = ("2018"); // 0
	SortedStickers.push_back(SSEntry);
	SSEntry.sub_name = ("2017"); // 1
	SortedStickers.push_back(SSEntry);
	SSEntry.sub_name = ("2016"); // 2
	SortedStickers.push_back(SSEntry);
	SSEntry.sub_name = ("2015"); // 3
	SortedStickers.push_back(SSEntry);
	SSEntry.sub_name = ("2014"); // 4
	SortedStickers.push_back(SSEntry);
	SSEntry.sub_name = ("Other"); // 5
	SortedStickers.push_back(SSEntry);

	for (size_t i(0); i < SortedStickers.size() - 1; i++)
		for (size_t j(0); j < sticker_kits.size(); j++)
			if (sticker_kits[j].name.find(SortedStickers[i].sub_name) != string::npos)
				SortedStickers[i].Stckers.push_back(sticker_kits[j]);

	for (size_t j(0); j < sticker_kits.size(); j++)
		if (sticker_kits[j].name.find(("2018")) == string::npos &&
			sticker_kits[j].name.find(("2017")) == string::npos &&
			sticker_kits[j].name.find(("2016")) == string::npos &&
			sticker_kits[j].name.find(("2015")) == string::npos &&
			sticker_kits[j].name.find(("2014")) == string::npos)
			SortedStickers.back().Stckers.push_back(sticker_kits[j]);

	ADD_LOG("Setup: All skins sorted sucessful\n");
}