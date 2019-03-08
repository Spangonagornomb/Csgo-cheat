#include "Skins.h"
#include "../Setup.h"
#include "../../GUI/Gui.h"

#define KITS_LIST_X 229
#define KITS_LIST_Y 363

#define ITEM_LIST_X 231
#define ITEM_LIST_Y 409

#define lol(e) (e)

string GlovesModels[49] =
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

int Page = 0;


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

void CSkins::ItemsList(const vector<ItemSettings> &Items, int &var, const Vec2 Size, const string Name)
{
	if (X1Gui().ListBoxHeader(Name.c_str(), Size))
	{
		for (int i = 0; i < (int)Items.size(); i++)
		{
			bool selected = i == var;
			if (X1Gui().SelectLabel((string(Items[i].Name) + "##" + to_string(i)).c_str(), selected))
				var = i;
		}
	}
	X1Gui().ListBoxFooter();
}
void CSkins::SkinsList(const vector<paint_kit> &Skins, int &var, const Vec2 Size, const string Name, const string Find)
{
	if (X1Gui().ListBoxHeader(Name.c_str(), Size))
	{
		for (size_t i = 0; i < Skins.size(); i++)
		{
			if (!FindLower((true ? Skins[i].name_eng : Skins[i].name), Find))
				continue;

			bool selected = i == var;

			if (X1Gui().SelectLabel(((true ? Skins[i].name_eng : Skins[i].name) + "##" + to_string(i)).c_str(), selected, Vec2(151, 0)))
				var = i;
			X1Gui().SameLine(149);
			if (X1Gui().SelectLabel(("| " + to_string(Skins[i].id)).c_str(), selected, Vec2(75, 0)))
				var = i;

		}
	}
	X1Gui().ListBoxFooter();
}
void CSkins::SortSkinsList(const vector<SortedKits_s> &Skins, int &var, const Vec2 Size, const string Name, const string Find)
{
	if (X1Gui().ListBoxHeader(Name.c_str(), Size))
	{
		for (size_t i = 0; i < Skins.size(); i++)
		{
			if (!FindLower((true ? Skins[i].name : Skins[i].name_rus), Find))
				continue;

			bool selected = i == var;

			if (X1Gui().SelectLabel(((true ? Skins[i].name : Skins[i].name_rus) + "##" + to_string(i)).c_str(), selected, Vec2(151, 0)))
				var = i;
			X1Gui().SameLine(149);
			if (X1Gui().SelectLabel(("| " + to_string(Skins[i].kit)).c_str(), selected, Vec2(75, 0)))
				var = i;

		}
	}
	X1Gui().ListBoxFooter();

}


void CSkins::Menu()
{
	float long_item_w = X1Gui().GetThis()->Info.Size.x - (X1Gui().GetStyle().wndPadding.x * 2);

	auto SkinParams = [&](SkinSettings &Item) -> void
	{
		X1Gui().PushItemWidth(360);
		SliderFloats("Seed", u8"�������", Item.seed, 0.f, 2.f, "");
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

		VectorEx<const char*> itemsQQEng = { Kostil_eng[0].c_str() , Kostil_eng[1].c_str(), Kostil_eng[2].c_str(), Kostil_eng[3].c_str(), Kostil_eng[4].c_str() ,Kostil_eng[5].c_str() ,
			Kostil_eng[6].c_str() ,Kostil_eng[6].c_str() ,Kostil_eng[7].c_str() , Kostil_eng[8].c_str() , Kostil_eng[9].c_str() };

		VectorEx<const char*> itemsQQRus = { Kostil_rus[0].c_str() , Kostil_rus[1].c_str(), Kostil_rus[2].c_str(), Kostil_rus[3].c_str(), Kostil_rus[4].c_str() ,Kostil_rus[5].c_str() ,
			Kostil_rus[6].c_str() ,Kostil_rus[6].c_str() ,Kostil_rus[7].c_str() , Kostil_rus[8].c_str() , Kostil_rus[9].c_str() };

		DComboBox("Quality", u8"��������", Item.quality, itemsQQEng, itemsQQRus, "");

		string old_name = Item.custom_name;
		X1Gui().InputText(!MainSettings().Russian ? "Name##34343" : u8"���##34343", Item.custom_name, 128);

		if (old_name != (string)Item.custom_name)
			IsNameChange = true;
	};

	VectorEx<const char*> SkinArrEng = { lolc("Weapon"), lolc("Knife"), lolc("Gloves") };
	VectorEx<const char*> SkinArrRus = { lolc(u8"������"), lolc(u8"����"),lolc(u8"��������") };

	static int SkinSettingsMode = 0;

	TabsLabels(SkinSettingsMode, SkinArrEng, SkinArrRus, Vec2(long_item_w - 3, 0), false);
	X1Gui().Spacing();
	if (SkinSettingsMode == 0)
	{
		VectorEx<const char*> WeapSkinArrEng = { lolc("Skin##545"),lolc("Stickers##545") };
		VectorEx<const char*> WeapSkinArrRus = { lolc(u8"�����##545"),lolc(u8"�������##545") };

		static int WeapSkinSettingsMode = 0;

		TabsLabels(WeapSkinSettingsMode, WeapSkinArrEng, WeapSkinArrRus, Vec2(long_item_w, 0), false);

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		if (X1Gui().Button(true ? ("Update") : (u8"��������"), Vec2(long_item_w, 22)))
		{
			UpdateSkins();
			Message::Get().Start(!MainSettings().Russian ? ("Updated!") : (u8"���������!"));
		}

		X1Gui().Spacing();

		if (WeapSkinSettingsMode == 0)
		{
			ItemsList(WeaponNames, SelectedWeapon, Vec2(ITEM_LIST_X, ITEM_LIST_Y), ("##AllWeapons"));

			X1Gui().SameLine();

			ItemSettings* WItem = &WeaponNames[SelectedWeapon];

			WeaponPreview = WItem->CdnName;

			X1Gui().BeginGroup();
			{
				X1Gui().PushItemWidth(185);
				VectorEx<const char*> itemssmEng = { lolc("All") , lolc("Distributed") };
				VectorEx<const char*> itemssmRus = { lolc(u8"���") , lolc(u8"��������������") };
				DComboBox("Skins##vod", u8"�����##���", WItem->Skin.skins_mode, itemssmEng, itemssmRus, "");
				static char FindBuf[128] = { 0 };
				static string FindSkin = "";
				TextEdit("Find", u8"�����", FindSkin, FindBuf, 128);
				X1Gui().Spacing();
				if (WItem->Skin.skins_mode == 0)
				{
					SkinsList(skin_kits, WItem->Skin.paint_kit_menu, Vec2(KITS_LIST_X, KITS_LIST_Y), ("##AllSkins"), FindSkin);
					WItem->Skin.paint_kit_id = skin_kits[WItem->Skin.paint_kit_menu].id;
					SkinPreview = skin_kits[WItem->Skin.paint_kit_menu].cdn_name;
				}
				else if (WItem->Skin.skins_mode == 1)
				{
					if (AllSkinsLoaded)
					{
						SortSkinsList(SortedWeapons[SelectedWeapon].kits, WItem->Skin.paint_kit_menu, Vec2(KITS_LIST_X, KITS_LIST_Y), ("##AllSkins"), FindSkin);
						if (WItem->Skin.paint_kit_menu < (int)SortedWeapons[SelectedWeapon].kits.size())
						{
							WItem->Skin.paint_kit_id = SortedWeapons[SelectedWeapon].kits[WItem->Skin.paint_kit_menu].kit;
							SkinPreview = SortedWeapons[SelectedWeapon].kits[WItem->Skin.paint_kit_menu].cdn_name;
						}
					}
					else
					{
						X1Gui().ListBoxHeader(("##AllSkinsEmpty"), Vec2(KITS_LIST_X, KITS_LIST_Y));
						X1Gui().Text(true ? ("Loading skins...") : (u8"�������� ������..."));
						X1Gui().ListBoxFooter();
					}

				}
			}
			X1Gui().EndGroup();

			X1Gui().Spacing();

			SkinParams(WItem->Skin);
		}
		else if (WeapSkinSettingsMode == 1)
		{
			DCheckBox("Stickers", u8"�������##2432134", WeaponNames[SelectedWeapon].Skin.striker_enable, u8"");
			if (WeaponNames[SelectedWeapon].Skin.striker_enable)
			{
				X1Gui().Spacing();
				X1Gui().PushItemWidth(192);
				X1Gui().Spacing();

				X1Gui().PushItemWidth(320);

				static int iSlot = 0;

				VectorEx<const char*> Slots = { lolc("1"),lolc("2"), lolc("3"), lolc("4"), lolc("5") };
				DComboBox("Slot##ss", u8"����", iSlot, Slots, Slots, "");

				static int StickersMode = 1;
				static int SortSelectedS = 0;

				VectorEx<const char*> itemssmEng = { lolc("All") ,lolc("Distributed") };
				VectorEx<const char*> itemssmRus = { lolc(u8"���") ,lolc(u8"��������������") };
				DComboBox("Stickers##vod65657", u8"�������##���765657", StickersMode, itemssmEng, itemssmRus, "");
				static char FindBuf[128] = { 0 };
				static string FindSticker = "";

				X1Gui().Spacing();

				StickerSettings *SItem = &WeaponNames[SelectedWeapon].Skin.Stickers[iSlot];

				if (StickersMode == 0)
				{
					TextEdit("Find", u8"�����", FindSticker, FindBuf, 128);
					X1Gui().ListBoxHeader(("##StickerSerials"), Vec2(453, 326));
					for (size_t i = 0; i < sticker_kits.size(); i++)
					{
						if (!FindLower(sticker_kits[i].name, FindSticker))
							continue;

						bool selected = i == SItem->kit_menu_index;
						if (X1Gui().SelectLabel(sticker_kits[i].name.c_str(), selected))
							SItem->kit_menu_index = i;
					}
					X1Gui().ListBoxFooter();
					SItem->kit = sticker_kits[SItem->kit_menu_index].id;
				}
				else if (StickersMode == 1)
				{
					if (AllSkinsLoaded)
					{
						X1Gui().ListBoxHeader(("##SortStickerSerials"), Vec2(71, 363));
						for (size_t i = 0; i < SortedStickers.size(); i++)
						{
							bool selected = i == SortSelectedS;
							if (X1Gui().SelectLabel(SortedStickers[i].sub_name.c_str(), selected))
								SortSelectedS = i;
						}
						X1Gui().ListBoxFooter();
						X1Gui().SameLine();
						X1Gui().BeginGroup();
						{
							TextEdit("Find", u8"�����", FindSticker, FindBuf, 128);
							X1Gui().ListBoxHeader(("##StickerSerials"), Vec2(376, 340));
							for (size_t i = 0; i < SortedStickers[SortSelectedS].Stckers.size(); i++)
							{
								if (!FindLower(SortedStickers[SortSelectedS].Stckers[i].name, FindSticker))
									continue;

								bool selected = i == SItem->kit_menu_index;

								if (X1Gui().SelectLabel(SortedStickers[SortSelectedS].Stckers[i].name.c_str(), selected))
									SItem->kit_menu_index = i;
							}
							X1Gui().ListBoxFooter();
						}
						X1Gui().EndGroup();

						if (SItem->kit_menu_index > 0 && SItem->kit_menu_index < (int)SortedStickers[SortSelectedS].Stckers.size())
							SItem->kit = SortedStickers[SortSelectedS].Stckers[SItem->kit_menu_index].id;
					}
					else
					{
						X1Gui().Text(true ? ("Loading stickers...") : (u8"�������� ��������..."));
					}
				}

				SliderFloats("Quality", u8"��������", SItem->wear, 0.f, 1.f, "");
				SliderFloats("Scale", u8"������", SItem->scale, 0.f, 1.f, "");
				SliderFloats("Rotation", u8"��������", SItem->rotation, 0.f, 360, "");
			}
		}
	}
	else if (SkinSettingsMode == 1)
	{
		VectorEx<const char*> KnfTeamArrEng = { lolc("CT"), lolc("T") };
		VectorEx<const char*> KnfTeamArrRus = { lolc(u8"��"),lolc(u8"�") };

		static int KnfTeamSettingsMode = 0;

		TabsLabels(KnfTeamSettingsMode, KnfTeamArrEng, KnfTeamArrRus, Vec2(long_item_w, 0), false);

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		if (X1Gui().Button(true ? ("Update") : (u8"��������"), Vec2(long_item_w, 22)))
		{
			UpdateSkins();
			Message::Get().Start(!MainSettings().Russian ? ("Updated!") : (u8"���������!"));
		}

		X1Gui().Spacing();

		if (KnfTeamSettingsMode == 0)
		{
			ItemsList(KnifeNames, SelectedKnifeModelCT, Vec2(ITEM_LIST_X, ITEM_LIST_Y), ("##AllKnifes"));
			X1Gui().SameLine();
			ItemSettings* WItem = &KnifeNames[SelectedKnifeModelCT];

			WeaponPreview = WItem->CdnName;

			X1Gui().BeginGroup();
			{
				X1Gui().PushItemWidth(185);
				VectorEx<const char*> itemssmEng = { lolc("All"), lolc("Distributed") };
				VectorEx<const char*> itemssmRus = { lolc(u8"���"), lolc(u8"��������������") };
				DComboBox("Skins##vod", u8"�����##���", WItem->Skin.skins_mode, itemssmEng, itemssmRus, "");
				static char FindBuf[128] = { 0 };
				static string FindSkin = "";
				TextEdit("Find", u8"�����", FindSkin, FindBuf, 128);
				X1Gui().Spacing();
				if (WItem->Skin.skins_mode == 0)
				{
					SkinsList(skin_kits, WItem->Skin.paint_kit_menu, Vec2(KITS_LIST_X, KITS_LIST_Y), ("##AllKnifSkins"), FindSkin);
					WItem->Skin.paint_kit_id = skin_kits[WItem->Skin.paint_kit_menu].id;
					SkinPreview = skin_kits[WItem->Skin.paint_kit_menu].cdn_name;
				}
				else if (WItem->Skin.skins_mode == 1)
				{
					if (AllSkinsLoaded)
					{
						SortSkinsList(SortedKnives[SelectedKnifeModelCT].kits, WItem->Skin.paint_kit_menu, Vec2(KITS_LIST_X, KITS_LIST_Y), ("##AllKnifSkins"), FindSkin);
						if (WItem->Skin.paint_kit_menu < (int)SortedKnives[SelectedKnifeModelCT].kits.size())
						{
							WItem->Skin.paint_kit_id = SortedKnives[SelectedKnifeModelCT].kits[WItem->Skin.paint_kit_menu].kit;
							SkinPreview = SortedKnives[SelectedKnifeModelCT].kits[WItem->Skin.paint_kit_menu].cdn_name;
						}
					}
					else
					{
						X1Gui().ListBoxHeader(("##AllSkinsEmpty"), Vec2(KITS_LIST_X, KITS_LIST_Y));
						X1Gui().Text(true ? ("Loading skins...") : (u8"�������� ������..."));
						X1Gui().ListBoxFooter();
					}
				}
			}
			X1Gui().EndGroup();
			X1Gui().Spacing();

			SkinParams(WItem->Skin);

			WeaponPreview = KnifeNames[SelectedKnifeModelCT].CdnName;
			SkinPreview = skin_kits[KnifeNames[SelectedKnifeModelCT].Skin.paint_kit_menu].cdn_name;
		}
		else if (KnfTeamSettingsMode == 1)
		{
			ItemsList(KnifeNamesTT, SelectedKnifeModelTT, Vec2(ITEM_LIST_X, ITEM_LIST_Y), ("##AllKnifes"));
			X1Gui().SameLine();
			ItemSettings* WItem = &KnifeNamesTT[SelectedKnifeModelTT];

			WeaponPreview = WItem->CdnName;

			X1Gui().BeginGroup();
			{
				X1Gui().PushItemWidth(185);
				VectorEx<const char*> itemssmEng = { lolc("All"), lolc("Distributed") };
				VectorEx<const char*> itemssmRus = { lolc(u8"���"), lolc(u8"��������������") };
				DComboBox("Skins##vod", u8"�����##���", WItem->Skin.skins_mode, itemssmEng, itemssmRus, "");
				static char FindBuf[128] = { 0 };
				static string FindSkin = "";
				TextEdit("Find", u8"�����", FindSkin, FindBuf, 128);
				X1Gui().Spacing();
				if (WItem->Skin.skins_mode == 0)
				{
					SkinsList(skin_kits, WItem->Skin.paint_kit_menu, Vec2(KITS_LIST_X, KITS_LIST_Y), ("##AllKnifSkins"), FindSkin);
					WItem->Skin.paint_kit_id = skin_kits[WItem->Skin.paint_kit_menu].id;
					SkinPreview = skin_kits[WItem->Skin.paint_kit_menu].cdn_name;
				}
				else if (WItem->Skin.skins_mode == 1)
				{
					if (AllSkinsLoaded)
					{
						SortSkinsList(SortedKnives[SelectedKnifeModelTT].kits, WItem->Skin.paint_kit_menu, Vec2(KITS_LIST_X, KITS_LIST_Y), ("##AllKnifSkins"), FindSkin);
						if (WItem->Skin.paint_kit_menu < (int)SortedKnives[SelectedKnifeModelTT].kits.size())
						{
							WItem->Skin.paint_kit_id = SortedKnives[SelectedKnifeModelTT].kits[WItem->Skin.paint_kit_menu].kit;
							SkinPreview = SortedKnives[SelectedKnifeModelTT].kits[WItem->Skin.paint_kit_menu].cdn_name;
						}
					}
					else
					{
						X1Gui().ListBoxHeader(("##AllSkinsEmpty"), Vec2(KITS_LIST_X, KITS_LIST_Y));
						X1Gui().Text(true ? ("Loading skins...") : (u8"�������� ������..."));
						X1Gui().ListBoxFooter();
					}
				}
			}
			X1Gui().EndGroup();
			X1Gui().Spacing();

			SkinParams(WItem->Skin);

			WeaponPreview = KnifeNamesTT[SelectedKnifeModelTT].CdnName;
			SkinPreview = skin_kits[KnifeNamesTT[SelectedKnifeModelTT].Skin.paint_kit_menu].cdn_name;
		}
	}
	else if (SkinSettingsMode == 2)
	{

		VectorEx<const char*> glvTeamArrEng = { lolc("CT"), lolc("T") };
		VectorEx<const char*> glvTeamArrRus = { lolc(u8"��"),lolc(u8"�") };
		static int glvTeamSettingsMode = 0;

		TabsLabels(glvTeamSettingsMode, glvTeamArrEng, glvTeamArrRus, Vec2(long_item_w, 0), false);

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		if (X1Gui().Button(true ? ("Update") : (u8"��������"), Vec2(long_item_w, 22)))
		{
			UpdateSkins();
			Message::Get().Start(!MainSettings().Russian ? ("Updated!") : (u8"���������!"));
		}

		X1Gui().Spacing();

		if (glvTeamSettingsMode == 0)
		{
			X1Gui().ListBoxHeader(("##Gloves"), Vec2(long_item_w, 522));
			for (int i = 0; i < sizeof(GlovesModels) / sizeof(GlovesModels[0]); i++)
			{
				bool selected = i == SelectedGloveCT;
				if (X1Gui().SelectLabel(GlovesModels[i].c_str(), selected))
					SelectedGloveCT = i;
			}
			X1Gui().ListBoxFooter();

			if (SelectedGloveCT > 0)
			{
				SkinPreview = GlovesSkin_Array[SelectedGloveCT - 1].Url;
				WeaponPreview = GlovesSkin_Array[SelectedGloveCT - 1].Url;
			}
		}
		else if (glvTeamSettingsMode == 1)
		{
			X1Gui().ListBoxHeader(("##Gloves"), Vec2(long_item_w, 522));
			for (int i = 0; i < sizeof(GlovesModels) / sizeof(GlovesModels[0]); i++)
			{
				bool selected = i == SelectedGloveTT;
				if (X1Gui().SelectLabel(GlovesModels[i].c_str(), selected))
					SelectedGloveTT = i;
			}
			X1Gui().ListBoxFooter();

			if (SelectedGloveTT > 0)
			{
				SkinPreview = GlovesSkin_Array[SelectedGloveTT - 1].Url;
				WeaponPreview = GlovesSkin_Array[SelectedGloveTT - 1].Url;
			}
		}
	}
}

void CSkins::Preview()
{
	int ImageSzX = 304;
	int ImageSzY = 231;

	Vec2 BackMin = X1Gui().GetCursorPos();
	Vec2 BackMax = Vec2(BackMin.x + ImageSzX, BackMin.y + ImageSzY);

	color_t BackColor = color_t(185.f, 183.f, 185.f, 183.f);
	X1Gui().DrawFilledBox(BackMin, BackMax, BackColor);

	static IDirect3DTexture9 *skinImg = nullptr;

	static string OldKit = SkinPreview;
	string link = "";

	if (OldKit != SkinPreview)
	{
		skinImg = nullptr;

		link = CGlobal::FindSkinURl(SkinPreview, WeaponPreview);
		if (skinImg == nullptr)
		{
			std::string imData = CGlobal::DownloadSkinBytes(link.c_str());

			D3DXCreateTextureFromFileInMemoryEx(GP_Render->m_pDevice, imData.data(), imData.length(), 512, 384, D3DX_DEFAULT,
				0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &skinImg);
		}
	}

	if (skinImg)
		X1Gui().DrawImage(skinImg, BackMin, BackMin + Vec2(ImageSzX, ImageSzY));

	OldKit = SkinPreview;
}