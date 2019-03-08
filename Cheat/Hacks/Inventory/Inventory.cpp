#include "Inventory.h"
#include "../Setup.h"
#include "EconItem.h"

int CInventory::Inventory::LastIndex = 100;
vector<IInventory::Inventory> InventoryList;

class CSharedObjectTypeCache
{
public:
	void AddObject(void* obj)
	{
		typedef void(__thiscall* tOriginal)(void*, void*);
		call_vfunc<tOriginal>(this, 1)(this, obj);
	}

	void RemoveObject(void* obj)
	{
		typedef void(__thiscall* tOriginal)(void*, void*);
		call_vfunc<tOriginal>(this, 3)(this, obj);
	}

	std::vector<CEconItem*> CSharedObjectTypeCache::GetEconItems()
	{
		std::vector<CEconItem*> ret;

		auto size = *reinterpret_cast<size_t*>(this + 0x18);

		auto data = *reinterpret_cast<uintptr_t**>(this + 0x4);

		for (size_t i = 0; i < size; i++)
			ret.push_back(reinterpret_cast<CEconItem*>(data[i]));

		return ret;
	}
};


class C_EconItemView
{
public:
	char pad_0x0000[0x194]; //0x0000
	__int32 m_iItemDefinitionIndex; //0x0194 
	__int32 m_iEntityQuality; //0x0198 
	__int32 m_iEntityLevel; //0x019C 
	char pad_0x01A0[0x8]; //0x01A0
	__int32 m_iItemIDHigh; //0x01A8 
	__int32 m_iItemIDLow; //0x01AC 
	__int32 m_iAccountID; //0x01B0 
	char pad_0x01B4[0x8]; //0x01B4
	unsigned char m_bInitialized; //0x01BC 
	char pad_0x01BD[0x67]; //0x01BD
	char m_szCustomName[32]; //0x0224 

};//Size=0x0244

class CPlayerInventory
{
public:
	void RemoveItem(uint64_t ID)
	{
		static auto fnRemoveItem
			= reinterpret_cast<int(__thiscall*)(void*, int64_t)>(
				CSX::Memory::FindSignature("client_panorama.dll", "55 8B EC 83 E4 F8 56 57 FF 75 0C 8B F1")
				);

		fnRemoveItem(this, ID);
	}

	void RemoveItem(CEconItem* item)
	{
		RemoveItem(*item->GetItemID());
		GetBaseTypeCache()->RemoveObject(item);
	}

	void ClearInventory()
	{
		auto BaseTypeCache = this->GetBaseTypeCache();
		auto items = BaseTypeCache->GetEconItems();
		for (auto item : items)
		{
			RemoveItem(*item->GetItemID());
			BaseTypeCache->RemoveObject(item);
		}
	}

	CSharedObjectTypeCache* CPlayerInventory::GetBaseTypeCache()
	{
		static auto fnGCSDK_CGCClient_FindSOCache
			= reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t, uint64_t, uint64_t, bool)>(
				CSX::Memory::FindSignature("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 1C 0F 10 45 08")
				);

		static auto fnGCSDK_CSharedObjectCache_CreateBaseTypeCache
			= reinterpret_cast<CSharedObjectTypeCache*(__thiscall*)(uintptr_t, int)>(
				CSX::Memory::FindSignature("client_panorama.dll", "55 8B EC 51 53 56 8B D9 8D 45 08")
				);
		DWORD g_GCClientSystem = **reinterpret_cast<uintptr_t**>(CSX::Memory::FindSignature("client_panorama.dll", "8B 0D ? ? ? ? 6A 00 83 EC 10") + 0x2);
		auto SOCahce = fnGCSDK_CGCClient_FindSOCache(g_GCClientSystem + 0x70, *reinterpret_cast<uint64_t*>(this + 0x8), *reinterpret_cast<uint64_t*>(this + 0x10), 0);

		return fnGCSDK_CSharedObjectCache_CreateBaseTypeCache(SOCahce, 1);
	}

	uint32_t GetSteamID()
	{
		return *reinterpret_cast<uint32_t*>(this + 0x8);
	}

	CUtlVector< C_EconItemView* >* CPlayerInventory::GetInventoryItems()
	{
		return reinterpret_cast<CUtlVector<C_EconItemView*>*>(this + 0x2C);
	}

	C_EconItemView* GetInventoryItemByItemID(int id)
	{
		auto items = GetInventoryItems();

		for (int i = 0; i < items->Count(); i++)
		{
			auto curElemt = items->Element(i);

			if (!curElemt)
				continue;

			//auto socData = curElemt->();

			//if (!socData)
			//	continue;

			//if (*socData->GetItemID() == id)
				return curElemt;
		}
	}

	bool AddEconItem(CEconItem* item, int a3, int a4, char a5)
	{
		static auto fnAddEconItem
			= reinterpret_cast<C_EconItemView*(__thiscall*)(void*, CEconItem*, int, int, char)>(
				CSX::Memory::FindSignature("client_panorama.dll", "55 8B EC 83 E4 F8 A1 ? ? ? ? 83 EC 14 53 56 57 8B F9 8B 08")
				);

		GetBaseTypeCache()->AddObject(item);


		auto ret = fnAddEconItem(this, item, a3, a4, a5);

		if (ret)
		{
			auto i = GetInventoryItemByItemID(*item->GetItemID());

			*reinterpret_cast<bool*>((uintptr_t)i + 0xA1) = 1;
		}


		return ret;
	}
};

bool CInventory::SendClientHello()
{

	Inventory* last_item = &InventoryList.back();

	//CSX::Memory::FindPattern(GetClientDll(), ("55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"), 0));

	auto LocalInventory = **reinterpret_cast<CPlayerInventory***>(CSX::Memory::FindSignature("client_panorama.dll", "8B 3D ? ? ? ? 85 FF 74 1A") + 0x2);

	auto Item = CreateEconItem();

	*Item->GetAccountID() = LocalInventory->GetSteamID();
	*Item->GetDefIndex() = last_item->Weapon;
	*Item->GetItemID() = CSX::Utils::RandomIntRange(1, 50000);
	*Item->GetInventory() = 1;
	*Item->GetFlags() = 0;
	*Item->GetOriginalID() = 0;

	for (int j = 0; j < 5; j++)
	{
		Item->AddSticker(j, last_item->Stickers[j].kit, last_item->Stickers[j].wear, last_item->Stickers[j].scale, last_item->Stickers[j].rotation);
	}

	Item->SetStatTrak(last_item->StatTrack);
	Item->SetPaintKit(last_item->WeaponSkinId);
	Item->SetPaintSeed(last_item->Seed);
	Item->SetPaintWear(last_item->Wear);
	Item->SetOrigin(8);
	Item->SetRarity(ITEM_RARITY_MYTHICAL);
	Item->SetLevel(1);
	Item->SetInUse(false);

	LocalInventory->AddEconItem(Item, 1, 0, 1);

	/*if(!CGlobal::IsGameReady)
		I::Engine()->ExecuteClientCmd("econ_clear_inventory_images");

	CMsgClientHello Message;

	Message.set_client_session_need(1);
	Message.clear_socache_have_versions();

	void* ptr = malloc(Message.ByteSize() + 8);

	if (!ptr)
		return false;

	((uint32_t*)ptr)[0] = k_EMsgGCClientHello | ((DWORD)1 << 31);
	((uint32_t*)ptr)[1] = 0;

	Message.SerializeToArray((void*)((DWORD)ptr + 8), Message.ByteSize());

	bool result = I::SteamGameCoordinator()->SendMessage(k_EMsgGCClientHello | ((DWORD)1 << 31), ptr, Message.ByteSize() + 8) == k_EGCResultOK;

	free(ptr);

	return result;*/
	return 0;
}

bool CInventory::SendMMHello()
{
	/*CMsgGCCStrike15_v2_MatchmakingClient2GCHello Message;
	void* ptr = malloc(Message.ByteSize() + 8);
	if (!ptr)
		return false;

	auto unMsgType = k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31);
	((uint32_t*)ptr)[0] = unMsgType;
	((uint32_t*)ptr)[1] = 0;

	Message.SerializeToArray((void*)((DWORD)ptr + 8), Message.ByteSize());

	bool result = I::SteamGameCoordinator()->SendMessage(k_EMsgGCCStrike15_v2_MatchmakingClient2GCHello | ((DWORD)1 << 31), ptr, Message.ByteSize() + 8) == k_EGCResultOK;

	free(ptr);
	return result;*/
	return 0;
}

void CInventory::RetrieveMessage(void* ecx, void* edx, uint32_t *punMsgType, void *pubDest, uint32_t cubDest, uint32_t *pcubMsgSize)
{
	PostRetrieveMessage(punMsgType, pubDest, cubDest, pcubMsgSize);
	PostRetrieveMessageMisc(punMsgType, pubDest, cubDest, pcubMsgSize);
}

void CInventory::SSendMessage(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData)
{
	void* pubDataMutable = const_cast<void*>(pubData);
	PreSendMessage(unMsgType, pubDataMutable, cubData);
}

int CInventory::GetInventoryByGame(int IndGame, int Eqp, TeamID Team)
{
	if (IndGame < 0) {
		for (size_t i(0); i < InventoryList.size(); i++)
			if (InventoryList[i].GetEquippedState == Eqp && InventoryList[i].Team == Team)
				return i;
	}
	else {
		for (size_t i(0); i < InventoryList.size(); i++)
			if (InventoryList[i].Index == IndGame)
				return i;
	}
	return -1;
};

void CInventory::SetKnife(Inventory *Inv, bool IsCT)
{
	if (IsCT)
		for (int i(0); i < (int)GP_Skins->KnifeNames.size(); i++)
			if (GP_Skins->KnifeNames[i].ID == (WEAPON_ID)Inv->Weapon)
			{
				GP_Skins->KnifeNames[i].IsInventory = true;
				GP_Skins->SelectedKnifeModelCT = i;
				GP_Skins->KnifeNames[i].Skin.paint_kit_id = Inv->WeaponSkinId;
				GP_Skins->KnifeNames[i].Skin.seed = Inv->Seed;
				GP_Skins->KnifeNames[i].Skin.stat_track = Inv->StatTrack;
				GP_Skins->KnifeNames[i].Skin.auto_stat_track = Inv->AutoStatTrack;
				GP_Skins->KnifeNames[i].Skin.quality = Inv->Rarity;
			}

	if (!IsCT)
		for (int i(0); i < (int)GP_Skins->KnifeNamesTT.size(); i++)
			if (GP_Skins->KnifeNamesTT[i].ID == (WEAPON_ID)Inv->Weapon)
			{
				GP_Skins->KnifeNamesTT[i].IsInventory = true;
				GP_Skins->SelectedKnifeModelTT = i;
				GP_Skins->KnifeNamesTT[i].Skin.paint_kit_id = Inv->WeaponSkinId;
				GP_Skins->KnifeNamesTT[i].Skin.seed = Inv->Seed;
				GP_Skins->KnifeNamesTT[i].Skin.stat_track = Inv->StatTrack;
				GP_Skins->KnifeNamesTT[i].Skin.auto_stat_track = Inv->AutoStatTrack;
				GP_Skins->KnifeNamesTT[i].Skin.quality = Inv->Rarity;
			}
}

void CInventory::SetGlove(Inventory *Inv, bool IsCT)
{
	if (IsCT)
		for (int i(0); i < 49; i++)
			if (GP_Skins->GlovesSkin_Array[i].ItemIndex == Inv->Weapon && GP_Skins->GlovesSkin_Array[i].PaintKit == Inv->WeaponSkinId)
				GP_Skins->SelectedGloveCT = i + 1;

	if (!IsCT)
		for (int i(0); i < 49; i++)
			if (GP_Skins->GlovesSkin_Array[i].ItemIndex == Inv->Weapon && GP_Skins->GlovesSkin_Array[i].PaintKit == Inv->WeaponSkinId)
				GP_Skins->SelectedGloveTT = i + 1;
}

void CInventory::PreSendMessage(uint32_t &unMsgType, void* pubData, uint32_t &cubData)
{
	if (InventoryList.empty())
		return;

	uint32_t MessageType = unMsgType & 0x7FFFFFFF;

	//if ((EGCItemMsg)MessageType == k_EMsgGCAdjustItemEquippedState)
	//{
	//	CMsgAdjustItemEquippedState Message;

	//	try
	//	{
	//		if (!Message.ParsePartialFromArray((void*)((DWORD)pubData + 8), cubData - 8))
	//			return;
	//	}
	//	catch (...)
	//	{
	//		return;
	//	}

	//	if (!Message.has_item_id() || !Message.has_new_class() || !Message.has_new_slot())
	//		return;


	//	int GameIdx = (int)((uint32_t)Message.item_id() - 20100);
	//	int EquippedState = Message.new_slot();
	//	TeamID Team = (TeamID)Message.new_class();

	//	bool Reset = EquippedState == 65535 || GameIdx < 0;

	//	int InventoryIndx = GetInventoryByGame(GameIdx, EquippedState, Team);

	//	static int LastChangeWeapon = -1;
	//	static int LastChangeKnife = -1;
	//	static int LastChangeGlove = -1;

	//	if (InventoryIndx < (int)InventoryList.size() && InventoryIndx != -1)
	//	{
	//		auto SetTeam = [](Inventory &Inv, TeamID Tm, bool Rest, bool &IsRemoveCT) -> void
	//		{
	//			if (Rest)
	//			{
	//				if ((CyrTeamID)Inv.iTeam == CYRT_ALL)
	//				{
	//					if (Tm == TEAM_COUNTER_TERRORIST) { Inv.Team = TEAM_TERRORIST; Inv.iTeam = CYRT_TT; }
	//					else if (Tm == TEAM_TERRORIST) { Inv.Team = TEAM_COUNTER_TERRORIST; Inv.iTeam = CYRT_CT; }
	//				}
	//				else
	//				{
	//					if ((CyrTeamID)Inv.iTeam == CYRT_CT && Tm == TEAM_COUNTER_TERRORIST) { Inv.iTeam = CYRT_DISBLE; IsRemoveCT = true; }
	//					if ((CyrTeamID)Inv.iTeam == CYRT_TT && Tm == TEAM_TERRORIST) { Inv.iTeam = CYRT_DISBLE; IsRemoveCT = false; }
	//				}
	//			}
	//			else
	//			{
	//				if ((TeamID)Inv.Team == TEAM_UNASSIGNED || (CyrTeamID)Inv.iTeam == CYRT_DISBLE || (CyrTeamID)Inv.iTeam == CYRT_ALL || (CyrTeamID)Inv.iTeam == CYRT_AUTO)
	//				{
	//					Inv.Team = Tm;
	//					if (Tm == TEAM_COUNTER_TERRORIST) { Inv.iTeam = CYRT_CT; }
	//					else if (Tm == TEAM_TERRORIST) { Inv.iTeam = CYRT_TT; }
	//				}
	//				else
	//				{
	//					if (Inv.Team == TEAM_COUNTER_TERRORIST && Tm == TEAM_TERRORIST) { Inv.Team = TEAM_ALL; Inv.iTeam = CYRT_ALL; }
	//					if (Inv.Team == TEAM_TERRORIST && Tm == TEAM_COUNTER_TERRORIST) { Inv.Team = TEAM_ALL; Inv.iTeam = CYRT_ALL; }
	//				}
	//			}
	//		};

	//		auto GetWeaponFromInv = [&](WEAPON_ID id) -> int
	//		{
	//			for (int i(0); i < (int)GP_Skins->WeaponNames.size(); i++)
	//			{
	//				if (GP_Skins->WeaponNames[i].ID == id)
	//					return i;
	//			}
	//			return 0;
	//		};

	//		Inventory *IBuffer = &InventoryList.at(InventoryIndx);

	//		bool IsRemCt = false;
	//		//pWeapon->PostDataUpdate(0);
	//		//pWeapon->OnDataChanged(0);
	//		if (IBuffer->ItemType == IT_WEAPON)
	//		{

	//			SetTeam(*IBuffer, Team, Reset, IsRemCt);

	//			ItemSettings* WBuffer = &GP_Skins->WeaponNames[GetWeaponFromInv((WEAPON_ID)IBuffer->Weapon)];

	//			WBuffer->IsInventory = true;
	//			if ((CyrTeamID)IBuffer->iTeam == CYRT_DISBLE)
	//			{
	//				if (IsRemCt)
	//					WBuffer->Skin.paint_kit_id = 0;
	//				else
	//					WBuffer->SkinTT.paint_kit_id = 0;
	//			}
	//			else if ((CyrTeamID)IBuffer->iTeam == CYRT_CT)
	//			{
	//				WBuffer->IsInventory = true;
	//				WBuffer->Skin.paint_kit_id = IBuffer->WeaponSkinId;
	//				WBuffer->Skin.wear = IBuffer->Wear;
	//				WBuffer->Skin.seed = IBuffer->Seed;
	//				WBuffer->Skin.stat_track = IBuffer->StatTrack;
	//				WBuffer->Skin.auto_stat_track = IBuffer->AutoStatTrack;
	//				WBuffer->Skin.quality = IBuffer->Rarity;
	//				for (int si(0); si < 5; si++)
	//				{
	//					if (IBuffer->Stickers[si].kit != 0)
	//						WBuffer->Skin.striker_enable = true;
	//					WBuffer->Skin.Stickers[si] = IBuffer->Stickers[si];
	//				}
	//			}
	//			else if ((CyrTeamID)IBuffer->iTeam == CYRT_TT)
	//			{
	//				WBuffer->IsInventory = true;
	//				WBuffer->SkinTT.paint_kit_id = IBuffer->WeaponSkinId;
	//				WBuffer->SkinTT.wear = IBuffer->Wear;
	//				WBuffer->SkinTT.seed = IBuffer->Seed;
	//				WBuffer->SkinTT.stat_track = IBuffer->StatTrack;
	//				WBuffer->SkinTT.auto_stat_track = IBuffer->AutoStatTrack;
	//				WBuffer->SkinTT.quality = IBuffer->Rarity;

	//				for (int si(0); si < 5; si++)
	//				{
	//					if (IBuffer->Stickers[si].kit != 0)
	//						WBuffer->Skin.striker_enable = true;
	//					WBuffer->SkinTT.Stickers[si] = IBuffer->Stickers[si];
	//				}
	//			}
	//			else if ((CyrTeamID)IBuffer->iTeam == CYRT_ALL)
	//			{
	//				WBuffer->IsInventory = true;
	//				WBuffer->SkinTT.paint_kit_id = IBuffer->WeaponSkinId;
	//				WBuffer->SkinTT.wear = IBuffer->Wear;
	//				WBuffer->SkinTT.seed = IBuffer->Seed;
	//				WBuffer->SkinTT.stat_track = IBuffer->StatTrack;
	//				WBuffer->SkinTT.auto_stat_track = IBuffer->AutoStatTrack;
	//				WBuffer->SkinTT.quality = IBuffer->Rarity;
	//				for (int si(0); si < 5; si++)
	//				{
	//					if (IBuffer->Stickers[si].kit != 0)
	//						WBuffer->Skin.striker_enable = true;
	//					WBuffer->SkinTT.Stickers[si] = IBuffer->Stickers[si];
	//				}

	//				WBuffer->Skin.paint_kit_id = IBuffer->WeaponSkinId;
	//				WBuffer->Skin.wear = IBuffer->Wear;
	//				WBuffer->Skin.seed = IBuffer->Seed;
	//				WBuffer->Skin.stat_track = IBuffer->StatTrack;
	//				WBuffer->Skin.auto_stat_track = IBuffer->AutoStatTrack;
	//				WBuffer->Skin.quality = IBuffer->Rarity;
	//				for (int si(0); si < 5; si++)
	//				{
	//					if (IBuffer->Stickers[si].kit != 0)
	//						WBuffer->Skin.striker_enable = true;
	//					WBuffer->Skin.Stickers[si] = IBuffer->Stickers[si];
	//				}
	//			}
	//		}
	//		else if (IBuffer->ItemType == IT_KNIFE)
	//		{
	//			SetTeam(*IBuffer, Team, Reset, IsRemCt);

	//			if ((CyrTeamID)IBuffer->iTeam == CYRT_DISBLE)
	//			{
	//				GP_Skins->SelectedKnifeModelCT = 0;
	//				GP_Skins->SelectedKnifeModelTT = 0;
	//			}
	//			else if ((CyrTeamID)IBuffer->iTeam == CYRT_CT)
	//			{

	//				SetKnife(IBuffer, true);
	//			}
	//			else if ((CyrTeamID)IBuffer->iTeam == CYRT_TT)
	//			{
	//				SetKnife(IBuffer, false);
	//			}
	//			else if ((CyrTeamID)IBuffer->iTeam == CYRT_ALL)
	//			{
	//				SetKnife(IBuffer, true);
	//				SetKnife(IBuffer, false);
	//			}
	//		}
	//		else if (IBuffer->ItemType == IT_GLOVE)
	//		{
	//			SetTeam(*IBuffer, Team, Reset, IsRemCt);

	//			if ((CyrTeamID)IBuffer->iTeam == CYRT_DISBLE)
	//			{
	//				if (IsRemCt)
	//					GP_Skins->SelectedGloveCT = 0;
	//				else
	//					GP_Skins->SelectedGloveTT = 0;
	//			}
	//			else if ((CyrTeamID)IBuffer->iTeam == CYRT_CT)
	//			{
	//				SetGlove(IBuffer, true);
	//			}
	//			else if ((CyrTeamID)IBuffer->iTeam == CYRT_TT)
	//			{
	//				SetGlove(IBuffer, false);
	//			}
	//			else if ((CyrTeamID)IBuffer->iTeam == CYRT_ALL)
	//			{
	//				SetGlove(IBuffer, true);
	//				SetGlove(IBuffer, false);
	//			}
	//		}

	//		IBuffer->GetEquippedState = EquippedState;
	//	}

	//	/*ofstream file("ssp.txt", ios_base::app);
	//	file << InventoryIndx << " () " << GameIdx << " | " << EquippedState << " : " << (int)Message.new_class() << "\n";
	//	file.close();*/
	//	return;
	//}

	return;
}


void CInventory::PostRetrieveMessageMisc(uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize)
{
	uint32_t MessageType = *punMsgType & 0x7FFFFFFF;

	/*if ((ECsgoGCMsg)MessageType == k_EMsgGCCStrike15_v2_MatchmakingGC2ClientHello)
	{
		CMsgGCCStrike15_v2_MatchmakingGC2ClientHello Message;
		try
		{
			if (!Message.ParsePartialFromArray((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8))
				return;
		}
		catch (...)
		{
			return;
		}

		if (ProfileLevel)
			Message.set_player_level(ProfileLevel);

		if (ProfileXP)
			Message.set_player_cur_xp(ProfileXP);

		PlayerRankingInfo* Ranking = Message.mutable_ranking();
		Ranking->set_account_id(I::SteamUser()->GetSteamID().GetAccountID());

		if (ProfileRank)
			Ranking->set_rank_id(ProfileRank);

		if (ProfileWins)
			Ranking->set_wins(ProfileWins);

		PlayerCommendationInfo* Commendation = Message.mutable_commendation();

		if (ProfileFriendly)
			Commendation->set_cmd_friendly(ProfileFriendly);

		if (ProfileLeader)
			Commendation->set_cmd_leader(ProfileLeader);

		if (ProfileTeacher)
			Commendation->set_cmd_teaching(ProfileTeacher);

		if ((uint32_t)Message.ByteSize() <= cubDest - 8)
		{
			Message.SerializeToArray((void*)((DWORD)pubDest + 8), Message.ByteSize());

			*pcubMsgSize = Message.ByteSize() + 8;
		}
	}*/
}
void CInventory::PostRetrieveMessage(uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize)
{



	//uint32_t MessageType = *punMsgType & 0x7FFFFFFF;


	////		*pcubMsgSize = Message.ByteSize() + 8;
	////	}
	////}

	////k_EMsgGCClientWelcome


	//if (InventoryList.empty())
	//	return;

	//if ((EGCBaseClientMsg)MessageType != k_EMsgGCClientWelcome)
	//	return;

	//CMsgClientWelcome Message;

	//try
	//{
	//	if (!Message.ParsePartialFromArray((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8))
	//		return;
	//}
	//catch (...)
	//{
	//	return;
	//}

	//if (Message.outofdate_subscribed_caches_size() <= 0)
	//	return;

	//CMsgSOCacheSubscribed* Cache = Message.mutable_outofdate_subscribed_caches(0);

	//for (int i = 0; i < Cache->objects_size(); i++)
	//{
	//	CMsgSOCacheSubscribed::SubscribedType* Object = Cache->mutable_objects(i);

	//	if (!Object->has_type_id())
	//		continue;

	//	if (Object->type_id() == 1)
	//	{
	//		for (int j = 0; j < Object->object_data_size(); j++)
	//		{
	//			std::string* ObjectData = Object->mutable_object_data(j);

	//			CSOEconItem Item;

	//			if (!Item.ParseFromArray((void*)const_cast<char*>(ObjectData->data()), ObjectData->size()))
	//				continue;

	//			if (Item.equipped_state_size() <= 0)
	//				continue;

	//			for (int k = 0; k < Item.equipped_state_size(); k++)
	//			{
	//				auto EquippedState = Item.mutable_equipped_state(k);

	//				EquippedState->set_new_class(0);
	//				EquippedState->set_new_slot(0);
	//			}

	//		}

	//		if (InventoryList.size() > 0)
	//		{
	//			for (size_t i(0); i < InventoryList.size(); i++)
	//			{
	//				if (InventoryList[i].ItemType != IT_MEDAL)
	//					AddItem(Object, InventoryList[i].Index, InventoryList[i].Weapon, InventoryList[i].Rarity + 1, InventoryList[i].WeaponSkinId, 38, InventoryList[i].Wear, "", i);
	//				else
	//					AddMedals(Object, InventoryList[i].Index, InventoryList[i].WeaponSkinId);
	//			}
	//		}
	//	}
	//}

	//if ((uint32_t)Message.ByteSize() <= cubDest - 8)
	//{
	//	Message.SerializeToArray((void*)((DWORD)pubDest + 8), Message.ByteSize());

	//	*pcubMsgSize = Message.ByteSize() + 8;
	//}
}

//void CInventory::AddItem(CMsgSOCacheSubscribed::SubscribedType* Object, int index, int itemDefIndex, int rarity, int paintKit, int seed, float wear, std::string name, int InventoryLIdx)
//{
//	CSOEconItem Skin;
//
//	Skin.set_id(20100 + index);
//	Skin.set_account_id(I::SteamUser()->GetSteamID().GetAccountID());
//	Skin.set_def_index(itemDefIndex);
//	Skin.set_inventory(20100 + index);
//	Skin.set_origin(24);
//	Skin.set_quantity(1);
//	Skin.set_level(1);
//	Skin.set_style(0);
//	Skin.set_flags(0);
//	Skin.set_in_use(false);
//	Skin.set_original_id(0);
//	Skin.set_rarity(rarity);
//	Skin.set_quality(0);
//
//	if (name.size() > 0)
//		Skin.set_custom_name(name.data());
//
//	if ((CyrTeamID)InventoryList[InventoryLIdx].iTeam != CYRT_AUTO && (CyrTeamID)InventoryList[InventoryLIdx].iTeam != CYRT_ALL)
//	{
//		if (InventoryList[InventoryLIdx].Team == TEAM_COUNTER_TERRORIST)
//		{
//			CSOEconItemEquipped* EquippedState = Skin.add_equipped_state();
//
//			EquippedState->set_new_class(TEAM_COUNTER_TERRORIST);
//			EquippedState->set_new_slot(InventoryList[InventoryLIdx].GetEquippedState);
//		}
//		if (InventoryList[InventoryLIdx].Team == TEAM_TERRORIST)
//		{
//			CSOEconItemEquipped* EquippedState = Skin.add_equipped_state();
//
//			EquippedState->set_new_class(TEAM_TERRORIST);
//			EquippedState->set_new_slot(InventoryList[InventoryLIdx].GetEquippedState);
//		}
//	}
//
//	// Paint Kit
//	auto PaintKitAttribute = Skin.add_attribute();
//	float PaintKitAttributeValue = (float)paintKit;
//
//	PaintKitAttribute->set_def_index(6);
//
//	PaintKitAttribute->set_value_bytes(&PaintKitAttributeValue, 4);
//
//	// Paint Seed
//	auto SeedAttribute = Skin.add_attribute();
//	float SeedAttributeValue = (float)seed;
//
//	SeedAttribute->set_def_index(7);
//
//	SeedAttribute->set_value_bytes(&SeedAttributeValue, 4);
//
//	// Paint Wear
//	auto WearAttribute = Skin.add_attribute();
//	float WearAttributeValue = wear;
//
//	WearAttribute->set_def_index(8);
//
//	WearAttribute->set_value_bytes(&WearAttributeValue, 4);
//
//	if (InventoryList[InventoryLIdx].StatTrack != 0)
//	{
//		CSOEconItemAttribute* StatTrakAttribute = Skin.add_attribute();
//		uint32_t StatTrakAttributeValue = 0;
//
//		StatTrakAttribute->set_def_index(81);
//
//		StatTrakAttribute->set_value_bytes(&StatTrakAttributeValue, 4);
//
//		// Counter Value
//		CSOEconItemAttribute* StatTrakCountAttribute = Skin.add_attribute();
//		uint32_t StatTrakCountAttributeValue = InventoryList[InventoryLIdx].StatTrack;
//
//		StatTrakCountAttribute->set_def_index(80);
//
//		StatTrakCountAttribute->set_value_bytes(&StatTrakCountAttributeValue, 4);
//	}
//
//	// Stickers
//	for (int j = 0; j < 5; j++)
//	{
//		// Sticker Kit
//		CSOEconItemAttribute* StickerKitAttribute = Skin.add_attribute();
//		uint32_t StickerKitAttributeValue = InventoryList[InventoryLIdx].Stickers[j].kit;
//
//		StickerKitAttribute->set_def_index(113 + 4 * j);
//
//		StickerKitAttribute->set_value_bytes(&StickerKitAttributeValue, sizeof(StickerKitAttributeValue));
//
//		// Sticker Wear
//		CSOEconItemAttribute* StickerWearAttribute = Skin.add_attribute();
//		float StickerWearAttributeValue = InventoryList[InventoryLIdx].Stickers[j].wear;
//
//		StickerWearAttribute->set_def_index(114 + 4 * j);
//
//		StickerWearAttribute->set_value_bytes(&StickerWearAttributeValue, sizeof(StickerWearAttributeValue));
//
//		// Sticker Scale
//		CSOEconItemAttribute* StickerScaleAttribute = Skin.add_attribute();
//		float StickerScaleAttributeValue = InventoryList[InventoryLIdx].Stickers[j].scale;
//
//		StickerScaleAttribute->set_def_index(115 + 4 * j);
//
//		StickerScaleAttribute->set_value_bytes(&StickerScaleAttributeValue, sizeof(StickerScaleAttributeValue));
//
//		// Sticker Rotation
//		CSOEconItemAttribute* StickerRotationAttribute = Skin.add_attribute();
//		float StickerRotationAttributeValue = InventoryList[InventoryLIdx].Stickers[j].rotation;
//
//		StickerRotationAttribute->set_def_index(116 + 4 * j);
//
//		StickerRotationAttribute->set_value_bytes(&StickerRotationAttributeValue, sizeof(StickerRotationAttributeValue));
//	}
//
//	Object->add_object_data(Skin.SerializeAsString());
//}
//
//
//void CInventory::AddMedals(CMsgSOCacheSubscribed::SubscribedType* pInventoryCacheObject, int Index, int MedalId)
//{
//	CSOEconItem Medal;
//
//	Medal.set_account_id(I::SteamUser()->GetSteamID().GetAccountID());
//	Medal.set_origin(9);
//	Medal.set_rarity(6);
//	Medal.set_quantity(1);
//	Medal.set_quality(4);
//	Medal.set_level(1);
//
//	CSOEconItemAttribute* TimeAcquiredAttribute = Medal.add_attribute();
//	uint32_t TimeAcquiredAttributeValue = 0;
//
//	TimeAcquiredAttribute->set_def_index(222);
//
//	TimeAcquiredAttribute->set_value_bytes(&TimeAcquiredAttributeValue, 4);
//
//	Medal.set_def_index(MedalId);
//	Medal.set_inventory(20000 + Index);
//	Medal.set_id(20000 + Index);
//
//	pInventoryCacheObject->add_object_data(Medal.SerializeAsString());
//
//}
string __readFile(const string& fileName)
{
	ifstream f(fileName);
	f.seekg(0, std::ios::end);
	size_t size = f.tellg();
	string s(size, ' ');
	f.seekg(0);
	f.read(&s[0], size);
	return s;
}
void CInventory::InitalizeMedals()
{
	string BuffNames = __readFile((".\\csgo\\scripts\\items\\items_game.txt"));

	if (BuffNames.length() < 3)
	{
		Medals.push_back({ 0, "<NULL>" });
		return;
	}

	size_t  pFind = 0;
	size_t  OldpFind = 0;

	vector<size_t> PositionsMedals;
	do
	{
		pFind = BuffNames.find(("status_icons"), OldpFind);
		OldpFind = pFind + 1;
		PositionsMedals.push_back(pFind);

	} while (pFind != string::npos);

	if (PositionsMedals.size() < 3)
	{
		Medals.push_back({ 0, "<NULL>" });
		return;
	}

	for (size_t Pos : PositionsMedals)
	{
		if (Pos > BuffNames.length()) { continue; }

		string InfoBuf = "";
		string sId = "";
		string Name = "";

		int ReadNameIter = 0;

		bool ReadInfo = false;

		for (size_t i(Pos - 330); i < Pos + 150; i++)
		{
			if (BuffNames[i] == '\"' && !ReadInfo)
			{
				for (size_t j(i); j < i + 10; j++)
				{
					if (BuffNames[j] == '{')
					{
						string Fusck = "";
						for (size_t l(j); l < j + 50; l++) { Fusck += BuffNames[l]; }
						if (Fusck.find(("name")) != string::npos) { ReadInfo = true; }
					}
				}
			}
			if (ReadInfo) { InfoBuf += BuffNames[i]; }
		}

		for (size_t i = 1; i < InfoBuf.length(); i++)
		{
			if (InfoBuf[i] == '\"') { break; }
			sId += InfoBuf[i];
		}

		size_t NameStartPos = InfoBuf.find(("name")) + 2;

		for (size_t i(NameStartPos); i < InfoBuf.length(); i++)
		{
			if (ReadNameIter == 2 && InfoBuf[i] == '\"') { break; }
			if (InfoBuf[i] == '\"') { ReadNameIter++; continue; }
			if (ReadNameIter == 2) { Name += InfoBuf[i]; }
		}

		if (Name == ("item 6088")) { Name = ("Commodity Pin - Dust II"); }
		if (sId == ("6088")) { sId = ("6101"); }


		Medals.push_back({ atoi(sId.c_str()), Name });
	}

	ADD_LOG("Setup: Medals sucessful\n");
}

void CInventory::SaveInventory(nlohmann::json &j)
{
	
	j[("Inventory")][("LastIdx")] = Inventory::LastIndex;

	for (auto &v : InventoryList)
	{
		nlohmann::json jb;
		jb[("ItemType")] = (int)v.ItemType;
		jb[("Index")] = v.Index;
		jb[("Weapon")] = v.Weapon;
		jb[("WeaponSkinId")] = v.WeaponSkinId;
		jb[("Rarity")] = v.Rarity;
		jb[("iTeam")] = v.iTeam;
		jb[("Team")] = (int)v.Team;
		jb[("GetEquippedState")] = v.GetEquippedState;
		jb[("Wear")] = v.Wear;
		jb[("Seed")] = v.Seed;
		jb[("StatTrack")] = v.StatTrack;
		jb[("AutoStatTrack")] = v.AutoStatTrack;
		jb[("SkinName")] = v.SkinName;
		jb[("WeaponName")] = v.WeaponName;
		jb[("Name")] = v.Name;

		if (v.ItemType == _ItemType::IT_WEAPON)
		{
			for (int j(0); j < 5; j++)
			{
				jb[(("StickerId_") + to_string(j))] = v.Stickers[j].kit;
				jb[(("StickerScale_") + to_string(j))] = v.Stickers[j].scale;
				jb[(("StickerWear_") + to_string(j))] = v.Stickers[j].wear;
				jb[(("StickerRotation_") + to_string(j))] = v.Stickers[j].rotation;
			}
		}
		j[("Inventory")][("Items")].push_back(jb);
	}
}
void CInventory::LoadInventory(nlohmann::json &j)
{
	if (!j[("Inventory")].is_null())
	{
		if (!j[("Inventory")][("LastIdx")].is_null())
			Inventory::LastIndex = j[("Inventory")][("LastIdx")];

		if (!InventoryList.empty())
			InventoryList.clear();

		if (!j[("Inventory")][("Items")].is_null())
		{
			for (int i(0); i < (int)j[("Inventory")][("Items")].size(); i++)
			{
				Inventory InvEntry;
				if (!j[("Inventory")][("Items")].at(i)[("ItemType")].is_null())
					InvEntry.ItemType = (_ItemType)(int)j[("Inventory")][("Items")].at(i)[("ItemType")];
				if (!j[("Inventory")][("Items")].at(i)[("Index")].is_null())
					InvEntry.Index = j[("Inventory")][("Items")].at(i)[("Index")];
				if (!j[("Inventory")][("Items")].at(i)[("Weapon")].is_null())
					InvEntry.Weapon = j[("Inventory")][("Items")].at(i)[("Weapon")];
				if (!j[("Inventory")][("Items")].at(i)[("WeaponSkinId")].is_null())
					InvEntry.WeaponSkinId = j[("Inventory")][("Items")].at(i)[("WeaponSkinId")];
				if (!j[("Inventory")][("Items")].at(i)[("Rarity")].is_null())
					InvEntry.Rarity = j[("Inventory")][("Items")].at(i)[("Rarity")];
				if (!j[("Inventory")][("Items")].at(i)[("iTeam")].is_null())
					InvEntry.iTeam = j[("Inventory")][("Items")].at(i)[("iTeam")];
				if (!j[("Inventory")][("Items")].at(i)[("Team")].is_null())
					InvEntry.Team = (TeamID)(int)j[("Inventory")][("Items")].at(i)[("Team")];
				if (!j[("Inventory")][("Items")].at(i)[("GetEquippedState")].is_null())
					InvEntry.GetEquippedState = j[("Inventory")][("Items")].at(i)[("GetEquippedState")];
				if (!j[("Inventory")][("Items")].at(i)[("Wear")].is_null())
					InvEntry.Wear = j[("Inventory")][("Items")].at(i)[("Wear")];

				if (!j[("Inventory")][("Items")].at(i)[("Seed")].is_null())
					InvEntry.Seed = j[("Inventory")][("Items")].at(i)[("Seed")];

				if (!j[("Inventory")][("Items")].at(i)[("StatTrack")].is_null())
					InvEntry.StatTrack = j[("Inventory")][("Items")].at(i)[("StatTrack")];

				if (!j[("Inventory")][("Items")].at(i)[("AutoStatTrack")].is_null())
					InvEntry.AutoStatTrack = j[("Inventory")][("Items")].at(i)[("AutoStatTrack")];

				if (!j[("Inventory")][("Items")].at(i)[("SkinName")].is_null())
					InvEntry.SkinName = j[("Inventory")][("Items")].at(i)[("SkinName")].get<string>();
				if (!j[("Inventory")][("Items")].at(i)[("WeaponName")].is_null())
					InvEntry.WeaponName = j[("Inventory")][("Items")].at(i)[("WeaponName")].get<string>();
				if (!j[("Inventory")][("Items")].at(i)[("Name")].is_null())
					InvEntry.Name = j[("Inventory")][("Items")].at(i)[("Name")].get<string>();

				if (InvEntry.ItemType == _ItemType::IT_WEAPON)
				{
					for (int k(0); k < 5; k++)
					{
						if (!j[("Inventory")][("Items")].at(i)[(("StickerId_") + to_string(k))].is_null())
							InvEntry.Stickers[k].kit = j[("Inventory")][("Items")].at(i)[(("StickerId_") + to_string(k))];

						if (!j[("Inventory")][("Items")].at(i)[(("StickerScale_") + to_string(k))].is_null())
							InvEntry.Stickers[k].scale = j[("Inventory")][("Items")].at(i)[(("StickerScale_") + to_string(k))];

						if (!j[("Inventory")][("Items")].at(i)[(("StickerWear_") + to_string(k))].is_null())
							InvEntry.Stickers[k].wear = j[("Inventory")][("Items")].at(i)[(("StickerWear_") + to_string(k))];

						if (!j[("Inventory")][("Items")].at(i)[(("StickerRotation_") + to_string(k))].is_null())
							InvEntry.Stickers[k].rotation = j[("Inventory")][("Items")].at(i)[(("StickerRotation_") + to_string(k))];
					}
				}
				InventoryList.push_back(InvEntry);
			}
		}
	}
}