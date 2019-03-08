#include "Inventory.h"
#include "../Setup.h"
#include "../../GUI/Gui.h"

#define lol(e) (e)
#define lolc(e) (e)

string IGlovesModels[49] =
{
	lol("default"),
	lol("Sport Superconductor"),
	lol("Sport Pandora's Box"),
	lol("Sport Hedge Maze"),
	lol("Sport Arid"),
	lol("Sport Vice"),
	lol("Sport Omega"),
	lol("Sport Bronze Morph"),
	lol("Sport Amphibious"),
	lol("Moto Eclipse"),
	lol("Moto Spearmint"),
	lol("Moto Boom!"),
	lol("Moto Cool Mint"),
	lol("Moto Polygon"),
	lol("Moto Transport"),
	lol("Moto Turtle"),
	lol("Moto Pow"),
	lol("Specialist Crimson Kimono"),
	lol("Specialist Emerald Web"),
	lol("Specialist Foundation"),
	lol("Specialist Forest DDPAT"),
	lol("Specialist Mogul"),
	lol("Specialist Fade"),
	lol("Specialist Buckshot"),
	lol("Specialist Crimson Web"),
	lol("Driver Lunar Weave"),
	lol("Driver Convoy"),
	lol("Driver Crimson Weave"),
	lol("Driver Diamondback"),
	lol("Driver Racing Green"),
	lol("Driver Overtake"),
	lol("Driver Imperial Plad"),
	lol("Driver King Snake"),
	lol("Hand Wraps Leather"),
	lol("Hand Wraps Spruce DDPAT"),
	lol("Hand Wraps Badlands"),
	lol("Hand Wraps Slaughter"),
	lol("Hand Wraps Aboreal"),
	lol("Hand Wraps Duct Tape"),
	lol("Hand Wraps Overprint"),
	lol("Hand Wraps Cobalt Skulls"),
	lol("Bloodhound Charred"),
	lol("Bloodhound Snakebite"),
	lol("Bloodhound Bronzed"),
	lol("Bloodhound Guerrilla"),
	lol("Hydra Case Hardened"),
	lol("Hydra Rattler"),
	lol("Hydra Mangrove"),
	lol("Hydra Emerald")
};

int IPage = 0;

inline bool FindLower(string data, string find)
{
	if (find.empty())
		return true;
	transform(data.begin(), data.end(), data.begin(), ::tolower);
	transform(find.begin(), find.end(), find.begin(), ::tolower);

	if (data.find(find) != string::npos)
		return true;

	return false;
}
void CInventory::Menu()
{
	float long_item_w = X1Gui().GetThis()->Info.Size.x - (X1Gui().GetStyle().wndPadding.x * 2);
	auto SkinParams = [&](SkinSettings &Item) -> void
	{
		X1Gui().PushItemWidth(360);
		SliderFloats("Seed", u8"����", Item.seed, 0.f, 2.f, "");
		X1Gui().SliderFloat(!MainSettings().Russian ? ("Wear") : (u8"������������"), &Item.wear, 0.f, 2.f, "%.9f");
		X1Gui().PushItemWidth(230);
		SliderInts("StatTrak", u8"StatTrak", Item.stat_track, 0, 5000, "");
		X1Gui().SameLine();
		DCheckBox("Auto StatTrak", u8"���� StatTrak##2432134", Item.auto_stat_track, u8"");

		X1Gui().PushItemWidth(360);

		vector<string> Kostil_eng = { lolc("Default"), lolc("Genuine"), lolc("Vintage"), lolc("Unusual"), lolc("Community"), lolc("Developer") ,
			lolc("Self-Made"), lolc("Customized"), lolc("Strange"), lolc("Completed"), lolc("Tournament") };

		vector<string> Kostil_rus = { lolc(u8"�� ���������"), lolc(u8"������������"), lolc(u8"��������"), lolc(u8"��������������"),lolc(u8"����������"), lolc(u8"�����������"),
			lolc(u8"������ ������"), lolc(u8"��������������"), lolc(u8"��������"),lolc(u8"�����������"), lolc(u8"��������") };

		VectorEx<const char* > itemsQQEng = { Kostil_eng[0].c_str() , Kostil_eng[1].c_str(), Kostil_eng[2].c_str(), Kostil_eng[3].c_str(), Kostil_eng[4].c_str() ,Kostil_eng[5].c_str() ,
			Kostil_eng[6].c_str() ,Kostil_eng[6].c_str() ,Kostil_eng[7].c_str() , Kostil_eng[8].c_str() , Kostil_eng[9].c_str() };

		VectorEx<const char* > itemsQQRus = { Kostil_rus[0].c_str() , Kostil_rus[1].c_str(), Kostil_rus[2].c_str(), Kostil_rus[3].c_str(), Kostil_rus[4].c_str() ,Kostil_rus[5].c_str() ,
			Kostil_rus[6].c_str() ,Kostil_rus[6].c_str() ,Kostil_rus[7].c_str() , Kostil_rus[8].c_str() , Kostil_rus[9].c_str() };

		DComboBox("Quality", u8"��������", Item.quality, itemsQQEng, itemsQQRus, "");
		static string lll = "";
		TextEdit("Name##34343", u8"���##34343", lll, Item.custom_name, 128);
	};

	if (WeaponNames.size() > 0)
	{
		VectorEx<const char* > InvArrEng = { lolc("Weapon"), lolc("Knife"),lolc("Gloves"), lolc("Medals") };
		VectorEx<const char* > InvArrRus = { lolc(u8"������"),lolc(u8"����"),lolc(u8"��������"), lolc(u8"������") };
		static int InvSettingsMode = 0;

		TabsLabels(InvSettingsMode, InvArrEng, InvArrRus, Vec2(long_item_w - 4, 20), false);
		X1Gui().Spacing();

		if (InvSettingsMode == 0)
		{
			VectorEx<const char* > WeapSkinArrEng = { lolc("Skin##545"),lolc("Stickers##545") };
			VectorEx<const char* > WeapSkinArrRus = { lolc(u8"�����##545"),lolc(u8"�������##545") };
			static int WeapSkinSettingsMode = 0;

			TabsLabels(WeapSkinSettingsMode, WeapSkinArrEng, WeapSkinArrRus, Vec2(long_item_w, 0), false);

			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			ItemSettings* WItem = &WeaponNames[InvSelectedWeapon];
			if (GP_Skins)
			{
				if (WeapSkinSettingsMode == 0)
				{
					ItemsList(WeaponNames, InvSelectedWeapon, Vec2(225, 389), ("##AllWeapons"));

					WeaponPreview = WItem->CdnName;

					X1Gui().SameLine();

					X1Gui().BeginGroup();
					{
						X1Gui().PushItemWidth(185);
						VectorEx<const char* > itemssmEng = { lolc("All") , lolc("Distributed") };
						VectorEx<const char* > itemssmRus = { lolc(u8"���") , lolc(u8"��������������") };
						DComboBox("Skins##vod", u8"�����##���", WItem->Skin.skins_mode, itemssmEng, itemssmRus, "");
						static char FindBuf[128] = { 0 };
						static string FindSkin = "";
						TextEdit("Find", u8"�����", FindSkin, FindBuf, 128);
						X1Gui().Spacing();
						if (WItem->Skin.skins_mode == 0)
						{
							SkinsList(GP_Skins->skin_kits, WItem->Skin.paint_kit_menu, Vec2(221, 340), ("##AllSkins"), FindSkin);
							WItem->Skin.paint_kit_id = GP_Skins->skin_kits[WItem->Skin.paint_kit_menu].id;
							GP_Skins->SkinPreview = GP_Skins->skin_kits[WItem->Skin.paint_kit_menu].cdn_name;
						}
						else if (WItem->Skin.skins_mode == 1)
						{
							if (GP_Skins->AllSkinsLoaded)
							{
								SortSkinsList(GP_Skins->SortedWeapons[InvSelectedWeapon].kits, WItem->Skin.paint_kit_menu, Vec2(221, 340), ("##AllSkins"), FindSkin);
								if (WItem->Skin.paint_kit_menu < (int)GP_Skins->SortedWeapons[InvSelectedWeapon].kits.size())
								{
									WItem->Skin.paint_kit_id = GP_Skins->SortedWeapons[InvSelectedWeapon].kits[WItem->Skin.paint_kit_menu].kit;
									GP_Skins->SkinPreview = GP_Skins->SortedWeapons[InvSelectedWeapon].kits[WItem->Skin.paint_kit_menu].cdn_name;
								}
							}
							else
							{
								X1Gui().Text(!MainSettings().Russian ? ("Loading skins...") : (u8"�������� ������..."));
							}

						}
					}
					X1Gui().EndGroup();


					X1Gui().Spacing();

					SkinParams(WItem->Skin);


				}
				else if (WeapSkinSettingsMode == 1)
				{
					DCheckBox("Stickers", u8"�������##2432134", WItem->Skin.striker_enable, u8"");
					if (WItem->Skin.striker_enable)
					{
						X1Gui().Spacing();
						X1Gui().PushItemWidth(192);
						X1Gui().Spacing();

						X1Gui().PushItemWidth(320);

						static int iSlot = 0;

						VectorEx<const char* > Slots = { lolc("1"),lolc("2"), lolc("3"), lolc("4") };
						DComboBox("Slot##ss", u8"����", iSlot, Slots, Slots, "");

						static int StickersMode = 1;
						static int SortSelectedS = 0;

						VectorEx<const char* > itemssmEng = { lolc("All") ,lolc("Distributed") };
						VectorEx<const char* > itemssmRus = { lolc(u8"���") ,lolc(u8"��������������") };
						DComboBox("Stickers##vod65657", u8"�������##���765657", StickersMode, itemssmEng, itemssmRus, "");
						static char FindBuf[128] = { 0 };
						static string FindSticker = "";

						X1Gui().Spacing();

						StickerSettings *SItem = &WItem->Skin.Stickers[iSlot];

						if (StickersMode == 0)
						{
							TextEdit("Find", u8"�����", FindSticker, FindBuf, 128);
							X1Gui().ListBoxHeader(("##StickerSerials"), Vec2(453, 326));
							for (size_t i = 0; i < GP_Skins->sticker_kits.size(); i++)
							{
								if (!FindLower(GP_Skins->sticker_kits[i].name, FindSticker))
									continue;

								bool selected = i == SItem->kit_menu_index;
								if (X1Gui().SelectLabel(GP_Skins->sticker_kits[i].name.c_str(), selected))
									SItem->kit_menu_index = i;
							}
							X1Gui().ListBoxFooter();
							SItem->kit = GP_Skins->sticker_kits[SItem->kit_menu_index].id;
						}
						else if (StickersMode == 1)
						{
							if (GP_Skins->AllSkinsLoaded)
							{
								X1Gui().ListBoxHeader(("##SortStickerSerials"), Vec2(71, 363));
								for (size_t i = 0; i < GP_Skins->SortedStickers.size(); i++)
								{
									bool selected = i == SortSelectedS;
									if (X1Gui().SelectLabel(GP_Skins->SortedStickers[i].sub_name.c_str(), selected))
										SortSelectedS = i;
								}
								X1Gui().ListBoxFooter();
								X1Gui().SameLine();
								X1Gui().BeginGroup();
								{
									TextEdit("Find", u8"�����", FindSticker, FindBuf, 128);
									X1Gui().ListBoxHeader(("##StickerSerials"), Vec2(376, 340));
									for (size_t i = 0; i < GP_Skins->SortedStickers[SortSelectedS].Stckers.size(); i++)
									{
										if (!FindLower(GP_Skins->SortedStickers[SortSelectedS].Stckers[i].name, FindSticker))
											continue;

										bool selected = i == SItem->kit_menu_index;

										if (X1Gui().SelectLabel(GP_Skins->SortedStickers[SortSelectedS].Stckers[i].name.c_str(), selected))
											SItem->kit_menu_index = i;
									}
									X1Gui().ListBoxFooter();
								}
								X1Gui().EndGroup();

								if (SItem->kit_menu_index > 0 && SItem->kit_menu_index < (int)GP_Skins->SortedStickers[SortSelectedS].Stckers.size())
									SItem->kit = GP_Skins->SortedStickers[SortSelectedS].Stckers[SItem->kit_menu_index].id;
							}
							else
							{
								X1Gui().Text(!MainSettings().Russian ? ("Loading stickers...") : (u8"�������� ��������..."));
							}
						}

						SliderFloats("Quality", u8"��������", SItem->wear, 0.f, 1.f, "");
						SliderFloats("Scale", u8"������", SItem->scale, 0.f, 1.f, "");
						SliderFloats("Rotation", u8"��������", SItem->rotation, 0.f, 360, "");
					}
				}

				if (X1Gui().Button(("ADD"), Vec2(long_item_w, 22)))
				{
					Inventory invBuffer;
					invBuffer.ItemType = IT_WEAPON;

					invBuffer.Index = InventoryList.size() == 0 ? Inventory::LastIndex + 1 : InventoryList.back().Index + 1;
					if (invBuffer.Index < Inventory::LastIndex)
						invBuffer.Index += Inventory::LastIndex - invBuffer.Index;

					if (invBuffer.Index == Inventory::LastIndex)
						invBuffer.Index = Inventory::LastIndex + 1;

					invBuffer.Weapon = WItem->ID;
					invBuffer.WeaponSkinId = WItem->Skin.paint_kit_id;
					invBuffer.Wear = WItem->Skin.wear;
					invBuffer.Seed = WItem->Skin.seed;
					invBuffer.StatTrack = WItem->Skin.stat_track;
					invBuffer.AutoStatTrack = WItem->Skin.auto_stat_track;
					invBuffer.Rarity = WItem->Skin.quality;

					for (int si(0); si < 5; si++)
					{
						invBuffer.Stickers[si] = WItem->Skin.Stickers[si];
					}

					invBuffer.WeaponName = WeaponNames[InvSelectedWeapon].Name;
					if (WItem->Skin.skins_mode == 0)
						invBuffer.SkinName = GP_Skins->skin_kits[WeaponNames[InvSelectedWeapon].Skin.paint_kit_menu].name_eng;
					else if (WItem->Skin.skins_mode == 1)
						invBuffer.SkinName = GP_Skins->SortedWeapons[InvSelectedWeapon].kits[WItem->Skin.paint_kit_menu].name;

					InventoryList.push_back(invBuffer);

					SendClientHello();
				}
			}
		}
		else if (InvSettingsMode == 1)
		{
			ItemsList(GP_Skins->KnifeNames, InvSelectedKnife, Vec2(225, 419), ("##AllKnifes"));
			X1Gui().SameLine();

			if (GP_Skins)
			{
				ItemSettings* WItem = &GP_Skins->KnifeNames[InvSelectedKnife];

				WeaponPreview = WItem->CdnName;

				X1Gui().BeginGroup();
				{
					X1Gui().PushItemWidth(185);
					VectorEx<const char* > itemssmEng = { "All" , "Distributed" };
					VectorEx<const char* > itemssmRus = { u8"���" , u8"��������������" };
					DComboBox("Skins##vod", u8"�����##���", WItem->Skin.skins_mode, itemssmEng, itemssmRus, "");
					static char FindBuf[128] = { 0 };
					static string FindSkin = "";
					TextEdit("Find", u8"�����", FindSkin, FindBuf, 128);
					X1Gui().Spacing();
					if (WItem->Skin.skins_mode == 0)
					{
						SkinsList(GP_Skins->skin_kits, WItem->Skin.paint_kit_menu, Vec2(221, 370), ("##AllKnifSkins"), FindSkin);
						WItem->Skin.paint_kit_id = GP_Skins->skin_kits[WItem->Skin.paint_kit_menu].id;
						SkinPreview = GP_Skins->skin_kits[WItem->Skin.paint_kit_menu].cdn_name;
					}
					else if (WItem->Skin.skins_mode == 1)
					{
						if (GP_Skins->AllSkinsLoaded)
						{
							SortSkinsList(GP_Skins->SortedKnives[InvSelectedKnife].kits, WItem->Skin.paint_kit_menu, Vec2(221, 370), ("##AllKnifSkins"), FindSkin);
							if (WItem->Skin.paint_kit_menu < (int)GP_Skins->SortedKnives[InvSelectedKnife].kits.size())
							{
								WItem->Skin.paint_kit_id = GP_Skins->SortedKnives[InvSelectedKnife].kits[WItem->Skin.paint_kit_menu].kit;
								SkinPreview = GP_Skins->SortedKnives[InvSelectedKnife].kits[WItem->Skin.paint_kit_menu].cdn_name;
							}
						}
						else
						{
							X1Gui().Text(!MainSettings().Russian ? ("Loading skins...") : (u8"�������� ������..."));
						}
					}
				}
				X1Gui().EndGroup();

				X1Gui().Spacing();

				SkinParams(WItem->Skin);

				if (X1Gui().Button(("ADD"), Vec2(long_item_w, 22)))
				{
					Inventory invBuffer;
					invBuffer.ItemType = IT_KNIFE;

					invBuffer.Index = InventoryList.size() == 0 ? Inventory::LastIndex + 1 : InventoryList.back().Index + 1;
					if (invBuffer.Index < Inventory::LastIndex)
						invBuffer.Index += Inventory::LastIndex - invBuffer.Index;

					if (invBuffer.Index == Inventory::LastIndex)
						invBuffer.Index = Inventory::LastIndex + 1;

					invBuffer.Weapon = WItem->ID;
					invBuffer.WeaponSkinId = WItem->Skin.paint_kit_id;
					invBuffer.Wear = WItem->Skin.wear;
					invBuffer.Seed = WItem->Skin.seed;
					invBuffer.StatTrack = WItem->Skin.stat_track;
					invBuffer.AutoStatTrack = WItem->Skin.auto_stat_track;
					invBuffer.Rarity = WItem->Skin.quality;

					invBuffer.WeaponName = KnifeNames[InvSelectedKnife].Name;
					if (WItem->Skin.skins_mode == 0)
						invBuffer.SkinName = GP_Skins->skin_kits[KnifeNames[InvSelectedKnife].Skin.paint_kit_menu].name_eng;
					else if (WItem->Skin.skins_mode == 1)
						invBuffer.SkinName = GP_Skins->SortedKnives[InvSelectedKnife].kits[WItem->Skin.paint_kit_menu].name;

					InventoryList.push_back(invBuffer);

					SendClientHello();
				}
			}
		}
		else if (InvSettingsMode == 2)
		{
			X1Gui().ListBoxHeader(("##Gloves"), Vec2(long_item_w, 535));
			for (int i = 0; i < sizeof(IGlovesModels) / sizeof(IGlovesModels[0]); i++)
			{
				bool selected = i == InvSelectedGlove;
				if (X1Gui().SelectLabel(IGlovesModels[i].c_str(), selected))
					InvSelectedGlove = i;
			}
			X1Gui().ListBoxFooter();

			if (X1Gui().Button(("ADD"), Vec2(453, 22)) && InvSelectedGlove != 0)
			{
				Inventory invBuffer;
				invBuffer.ItemType = IT_GLOVE;

				invBuffer.Index = InventoryList.size() == 0 ? Inventory::LastIndex + 1 : InventoryList.back().Index + 1;
				if (invBuffer.Index < Inventory::LastIndex)
					invBuffer.Index += Inventory::LastIndex - invBuffer.Index;

				if (invBuffer.Index == Inventory::LastIndex)
					invBuffer.Index = Inventory::LastIndex + 1;

				invBuffer.Weapon = GlovesSkin_Array[InvSelectedGlove - 1].ItemIndex;
				invBuffer.WeaponSkinId = GlovesSkin_Array[InvSelectedGlove - 1].PaintKit;

				invBuffer.WeaponName = IGlovesModels[InvSelectedGlove];

				InventoryList.push_back(invBuffer);

				SendClientHello();
			}
		}
		else if (InvSettingsMode == 3)
		{
			static int SelectedMedal = 0;

			X1Gui().ListBoxHeader(("##Medals"), Vec2(long_item_w, 535));
			for (size_t i = 0; i < Medals.size(); i++)
			{
				bool selected = i == SelectedMedal;
				if (X1Gui().SelectLabel(Medals[i].Name.c_str(), selected))
					SelectedMedal = i;
			}
			X1Gui().ListBoxFooter();
			if (X1Gui().Button(("ADD"), Vec2(long_item_w, 22)))
			{
				Inventory invBuffer;
				invBuffer.ItemType = IT_MEDAL;

				invBuffer.Index = InventoryList.size() == 0 ? Inventory::LastIndex + 1 : InventoryList.back().Index + 1;
				if (invBuffer.Index < Inventory::LastIndex)
					invBuffer.Index += Inventory::LastIndex - invBuffer.Index;

				if (invBuffer.Index == Inventory::LastIndex)
					invBuffer.Index = Inventory::LastIndex + 1;

				invBuffer.WeaponName = Medals[SelectedMedal].Name;
				invBuffer.WeaponSkinId = Medals[SelectedMedal].Id;

				InventoryList.push_back(invBuffer);

				SendClientHello();
			}
		}
		else if (InvSettingsMode == 4)
		{

		}
	}
}

string GetStickerByIdx(int idx)
{
	string entry = "";
	if (GP_Skins->AllSkinsLoaded)
	{
		for (auto &v : GP_Skins->sticker_kits)
			if (v.id == idx)
				return v.name;
	}
	return entry;
}

void CInventory::InvListMenu()
{
	float long_item_w = X1Gui().GetThis()->Info.Size.x - (X1Gui().GetStyle().wndPadding.x * 2);
	static int InventSelectItem = 0;

	X1Gui().Text((("Items: ") + to_string(InventoryList.size())).c_str());

	X1Gui().ListBoxHeader(("##InventoryList"), Vec2(long_item_w, 390));
	for (size_t i = 0; i < InventoryList.size(); i++)
	{
		bool Select = i == InventSelectItem;

		if (X1Gui().SelectLabel((InventoryList[i].WeaponName + "##" + to_string(i)).c_str(), Select, Vec2(157, 0)))
			InventSelectItem = i;

		X1Gui().SameLine();

		if (X1Gui().SelectLabel(("| " + InventoryList[i].SkinName + " (" + to_string(InventoryList[i].WeaponSkinId) + ")" + "##" + to_string(i)).c_str(), Select))
			InventSelectItem = i;

	}
	X1Gui().ListBoxFooter();
	if (InventSelectItem >= (int)InventoryList.size())
		InventSelectItem = InventoryList.size() - 1;

	if ((int)InventoryList.size() > 0 && InventSelectItem > -1 && InventSelectItem < (int)InventoryList.size())
	{
		X1Gui().PushItemWidth(171);
		
		if (InventoryList[InventSelectItem].ItemType == IT_WEAPON)
		{
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			X1Gui().Text("Stickers:");
			X1Gui().Text("Slot1: ");
			X1Gui().SameLine();
			X1Gui().Text(GetStickerByIdx(InventoryList[InventSelectItem].Stickers[0].kit).c_str());
			X1Gui().Text("Slot2: ");
			X1Gui().SameLine();
			X1Gui().Text(GetStickerByIdx(InventoryList[InventSelectItem].Stickers[1].kit).c_str());
			X1Gui().Text("Slot3: ");
			X1Gui().SameLine();
			X1Gui().Text(GetStickerByIdx(InventoryList[InventSelectItem].Stickers[2].kit).c_str());
			X1Gui().Text("Slot4: ");
			X1Gui().SameLine();
			X1Gui().Text(GetStickerByIdx(InventoryList[InventSelectItem].Stickers[3].kit).c_str());

			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();
		}

		if (X1Gui().Button(!MainSettings().Russian ? ("Update Inventory") : (u8"�������� ���������"), Vec2(long_item_w, 22)))
		{
			SendClientHello();
		}
		X1Gui().Spacing();
		if (X1Gui().Button(!MainSettings().Russian ? ("Remove") : (u8"�������"), Vec2(175, 22)))
		{
			Inventory DeletedItem = InventoryList[InventSelectItem];

			if (InventoryList[InventSelectItem].ItemType == IT_WEAPON)
			{
				bool IsReset = true;

				for (Inventory I : InventoryList)
					if (I.Weapon == DeletedItem.Weapon) { IsReset = false; break; }

				if (IsReset)
				{
					
				}
			}

			InventoryList.erase(InventoryList.begin() + InventSelectItem);

			InventSelectItem--;
			SendClientHello();
		}
		X1Gui().SameLine();
		if (X1Gui().Button(!MainSettings().Russian ? ("Remove all") : (u8"������� ���"), Vec2(176, 22)) && InventoryList.size() > 0)
		{
			SendClientHello();
			InventoryList.clear();

			for (int i(0); i < (int)WeaponNames.size(); i++)
			{
				WeaponNames[i].IsInventory = false;
			}
			for (int i(0); i < (int)KnifeNames.size(); i++)
			{
				KnifeNames[i].IsInventory = false;
			}
			for (int i(0); i < (int)KnifeNamesTT.size(); i++)
			{
				KnifeNamesTT[i].IsInventory = false;
			}
		}
	}
}
