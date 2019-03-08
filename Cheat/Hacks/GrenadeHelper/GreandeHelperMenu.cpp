#include "GreandeHelper.h"
#include "../../GUI/Gui.h"

void CGHelper::ChangeGHI(GHInfo* info, char NameBuf[], char DescpBuf[], char MapName[], bool AdvMap)
{
	TextEdit("Name", u8"���", info->name, NameBuf, 64);
	X1Gui().Spacing();
	X1Gui().Separator();
	X1Gui().Spacing();

	if (AdvMap)
	{
		static int SelecteMapMode = 0;
		/*const char** itemsGHSS = new const char*[2 + maps.size()];

		itemsGHSS[0] = new const char[sizeof((!MainSettings().Russian ? "Auto" : u8"����"))];
		itemsGHSS[0] = (!MainSettings().Russian ? "Auto" : u8"����");

		itemsGHSS[1] = new const char[sizeof((!MainSettings().Russian ? "Custom" : u8"������"))];
		itemsGHSS[1] = (!MainSettings().Russian ? "Custom" : u8"������");


		for (size_t i(2); i < maps.size() + 2; i++)
		{
			itemsGHSS[i] = new const char[sizeof(maps[i - 2].game_name.c_str())];
			itemsGHSS[i] = maps[i - 2].game_name.c_str();
		}*/

		VectorEx<const char*> itemsGHSS;
		itemsGHSS.push_back(!MainSettings().Russian ? "Auto" : u8"����");
		itemsGHSS.push_back(!MainSettings().Russian ? "Custom" : u8"������");

		for (size_t i(2); i < maps.size() + 2; i++)
		{
			itemsGHSS.push_back(maps[i - 2].game_name.c_str());
		}

		X1Gui().ComboBox(!MainSettings().Russian ? ("Map") : (u8"�����"), &SelecteMapMode, itemsGHSS);

		if (SelecteMapMode == 1)
		{
			X1Gui().InputText(!MainSettings().Russian ? ("Map Name") : (u8"��� �����"), MapName, 300);
		}
		else if (SelecteMapMode == 0)
		{
			strcpy(MapName, string(I::Engine()->GetLevelName()).c_str());
		}
		else
		{
			strcpy(MapName, itemsGHSS[SelecteMapMode]);
		}


		//delete[] itemsGHSS;
	}
	else
	{
		X1Gui().InputText(!MainSettings().Russian ? ("Map Name") : (u8"��� �����"), MapName, 300);
	}
	X1Gui().Spacing();
	X1Gui().Separator();
	X1Gui().Spacing();

#define chu(e)  e

	if (!AdvMap)
	{

		VectorEx<const char*> itemsCSEng = { chu("Incendiary grenade"), chu("Molotov"), chu("High explosive grenade"), chu("Smoke grenade"), chu("Decoy grenade"), chu("Flashbang") };
		VectorEx<const char*> itemsCSRus = { chu(u8"�������������"), chu(u8"�������"), chu(u8"����������"), chu(u8"���"), chu(u8"������"), chu(u8"��������") };

		int bg = 0;

		switch (info->grenade)
		{
		case WEAPON_INCGRENADE: bg = 0; break;
		case WEAPON_MOLOTOV: bg = 1; break;
		case WEAPON_HEGRENADE: bg = 2; break;
		case WEAPON_SMOKEGRENADE: bg = 3; break;
		case WEAPON_DECOY: bg = 4; break;
		case WEAPON_FLASHBANG: bg = 5; break;
		default: break;
		}

		DComboBox("Grenade", u8"�������", bg, itemsCSEng, itemsCSRus, u8"");

		switch (bg)
		{
		case 0: info->grenade = WEAPON_INCGRENADE; break;
		case 1: info->grenade = WEAPON_MOLOTOV; break;
		case 2: info->grenade = WEAPON_HEGRENADE; break;
		case 3: info->grenade = WEAPON_SMOKEGRENADE; break;
		case 4: info->grenade = WEAPON_DECOY; break;
		case 5: info->grenade = WEAPON_FLASHBANG; break;
		default: break;
		}
	}
	else
	{
		VectorEx<const char*> itemsCSEng_ = { chu("Auto"), chu("Incendiary grenade"), chu("Molotov"), chu("High explosive grenade"), chu("Smoke grenade"), chu("Decoy grenade"), chu("Flashbang") };
		VectorEx<const char*> itemsCSRus_ = { chu(u8"����"), chu(u8"�������������"), chu(u8"�������"), chu(u8"����������"), chu(u8"���"), chu(u8"������"), chu(u8"��������") };

		int bg = 0;

		switch (info->grenade)
		{
		case WEAPON_NONE: bg = 0; break;
		case WEAPON_INCGRENADE: bg = 1; break;
		case WEAPON_MOLOTOV: bg = 2; break;
		case WEAPON_HEGRENADE: bg = 3; break;
		case WEAPON_SMOKEGRENADE: bg = 4; break;
		case WEAPON_DECOY: bg = 5; break;
		case WEAPON_FLASHBANG: bg = 6; break;
		default: break;
		}

		DComboBox("Grenade", u8"�������", bg, itemsCSEng_, itemsCSRus_, u8"");

		switch (bg)
		{
		case 0: info->grenade = WEAPON_NONE; break;
		case 1: info->grenade = WEAPON_INCGRENADE; break;
		case 2: info->grenade = WEAPON_MOLOTOV; break;
		case 3: info->grenade = WEAPON_HEGRENADE; break;
		case 4: info->grenade = WEAPON_SMOKEGRENADE; break;
		case 5: info->grenade = WEAPON_DECOY; break;
		case 6: info->grenade = WEAPON_FLASHBANG; break;
		default: break;
		}
	}
	X1Gui().Spacing();
	X1Gui().Separator();
	X1Gui().Spacing();
	SliderFloats("Direction X", u8"����������� X", info->directn.y, -179.9, 179.9, u8"");
	SliderFloats("Direction Y", u8"����������� Y", info->directn.x, -179.9, 179.9, u8"");
};

void CGHelper::Menu()
{
	float long_item_w = X1Gui().GetThis()->Info.Size.x - (X1Gui().GetStyle().wndPadding.x * 2);

	DCheckBox("Enabled", u8"������������", Enable, u8"");

	X1Gui().SameLine();

	DCheckBox("Only for hand grenades", u8"������ ��� ������� � �����", EnableFor1Grenade, u8"");

	X1Gui().Spacing();
	X1Gui().Separator();
	X1Gui().Spacing();
	DCheckBox("Show description", u8"���������� ���������", ShowDiscp, u8"");
	X1Gui().SameLine();
	DCheckBox("Show name", u8"���������� ��������", ShowName, u8"");
	X1Gui().Spacing();
	SliderFloats("Show info distance", u8"��������� ����������� ����������", DistShowInfo, 1, 400, u8"");
	SliderFloats("Show distance", u8"��������� �����������", DistShowHelp, 1, 2000, u8"");

	VectorEx<const char*> itemsCSEng = { chu("Type 1"), chu("Type 2") };
	VectorEx<const char*> itemsCSRus = { chu(u8"��� 1"), chu(u8"��� 2") };

	DComboBox("Crosshair", u8"������", CrosshairStyle, itemsCSEng, itemsCSRus, u8"");

	X1Gui().Spacing();
	X1Gui().Separator();
	X1Gui().Spacing();

	static int Mode = 1;
	static GHInfo InfoProt;

	if (X1Gui().ListBoxHeader("##GHelpList", Vec2(long_item_w, 268)))
	{
		GuiStyle& style = X1Gui().GetStyle();

		Vec2 cur_menu_pos = X1Gui().GetCursorPos();
		Vec2 prev_pos_1 = Vec2(0, 0);
		Vec2 prev_pos_2 = Vec2(0, 0);

		auto CreateNode = [&](float cor_x, float len, bool check, Vec2 &prev) -> void
		{
			cur_menu_pos = X1Gui().GetCursorPosEx();
			Vec2 cur_pos = X1Gui().GetCursorPos();

			Vec2 next_pos = Vec2(cur_pos.x + len + cor_x, cur_pos.y);

			X1Gui().DrawLine(Vec2(cur_pos.x + cor_x, cur_pos.y + 9), Vec2(next_pos.x - 3, next_pos.y + 9), MainSettings().LineColor);
			X1Gui().DrawLine(Vec2(cur_pos.x + cor_x, (prev.y != 0) ? prev.y : (cur_pos.y - (check ? 9 : 3))), Vec2(cur_pos.x + cor_x, next_pos.y + 9), MainSettings().LineColor);

			prev = Vec2(cur_pos.x + cor_x, next_pos.y + 9);

			X1Gui().SetCursorPos(Vec2(cur_menu_pos.x + len + cor_x, cur_menu_pos.y), 0);
		};

		string CurrentMapName = I::Engine()->GetLevelName();

		for (int i = 0; i < maps.size(); i++)
		{
			CreateNode(3, 15, i != 0, prev_pos_1);

			bool selected_map = SelectedMap && SelectedMap == &maps[i];

			if (CurrentMapName == maps[i].game_name)
				style.clrText = Color::Green();

			bool tree_open = X1Gui().BeginTreeNode((maps[i].game_name + "##" + to_string(i)).c_str(), Vec2(long_item_w - 30, 0), false);
			if (X1Gui().IsItemClicked())
			{
				SelectedMap = &maps[i];
				SelectedGHInf = nullptr;
				Mode = 0;
			}

			if (CurrentMapName == maps[i].game_name)
				style.clrText = MainSettings().TextColor;

			if (tree_open)
			{
				for (int j = 0; j < maps[i].helpers.size(); j++)
				{
					X1Gui().Spacing();
					CreateNode(24, 20, j != 0, prev_pos_2);
					bool selected = SelectedGHInf && SelectedGHInf == &maps[i].helpers[j];

					if (X1Gui().SelectLabel((maps[i].helpers[j].name + " (" + GetGName(maps[i].helpers[j].grenade) + ")##" + to_string((i + 1) + (j + 1))).c_str(),
						selected, Vec2(selected ? 305 : 395, 0)))
					{
						SelectedGHInf = &maps[i].helpers[j];
						SelectedMap = nullptr;
						Mode = 0;
					}

					if (selected)
					{
						X1Gui().SameLine();

						style.clrFrame = Color(MainSettings().FrameColor.r() + 18.f,
							MainSettings().FrameColor.g() + 18.f,
							MainSettings().FrameColor.b() + 18.f, 255);
						float old_pad_y = style.itmPadding.y;
						style.itmPadding.y = 0;

						DCheckBox("Enable", u8"�����.", maps[i].helpers[j].enable, u8"�������� �����������");

						style.clrFrame = MainSettings().FrameColor;
						style.itmPadding.y = old_pad_y;
					}
				}
				prev_pos_2 = Vec2(0, 0);
			}

			X1Gui().Spacing();
		}
	}
	X1Gui().ListBoxFooter();

	X1Gui().Spacing();
	X1Gui().Separator();
	X1Gui().Spacing();

	VectorEx<const char*> itemsCSSEng = { lolc("Editing"), lolc("Adding") };
	VectorEx<const char*> itemsCSSRus = { lolc(u8"��������������"), lolc(u8"����������") };

	TabsLabels(Mode, itemsCSSEng, itemsCSSRus, Vec2(long_item_w, 20), false);

	X1Gui().Spacing();
	X1Gui().Separator();
	X1Gui().Spacing();

	if (Mode == 0)
	{
		auto RemvMapHelp = [&](Map* map, GHInfo* info) -> void
		{
			if (!map || !info)
				return;

			if (map->helpers.size() == 1)
			{
				DeleteMap(map->game_name);
				map = nullptr;
				info = nullptr;
			}
			else
			{
				DeleteHelp(info);
				info = nullptr;
			}
		};

		if (SelectedGHInf)
		{
			char NameBuf[64] = { 0 };
			char DescpBuf[300] = { 0 };
			char MapNameBuf[300] = { 0 };

			Map* CurMap = GetMapByHelp(SelectedGHInf);
			if (CurMap)
			{
				string OldName = CurMap->game_name;

				strcpy(MapNameBuf, CurMap->game_name.c_str());
				ChangeGHI(SelectedGHInf, NameBuf, DescpBuf, MapNameBuf, false);

				if (OldName != string(MapNameBuf))
				{
					GHInfo tempInfo = *SelectedGHInf;
					RemvMapHelp(CurMap, SelectedGHInf);
					Add(string(MapNameBuf), tempInfo);

					Map* NewMap = GetMapByGName(string(MapNameBuf));
					if (NewMap)
						SelectedGHInf = &NewMap->helpers.back();
				}

				X1Gui().Spacing();
				X1Gui().Separator();
				X1Gui().Spacing();

				if (X1Gui().Button(!MainSettings().Russian ? ("Remove") : (u8"�������"), Vec2(long_item_w, 22)))
					if (CurMap)
						RemvMapHelp(CurMap, SelectedGHInf);
			}
		}
		if (SelectedMap)
		{
			char MapNameBuf[300] = { 0 };

			TextEdit("Map", u8"�����", SelectedMap->game_name, MapNameBuf, 300);
			if (X1Gui().IsItemHovered())
				MainSettings().HintMsg = (u8"�������� ��� ��������������� ������ �����");

			if (SelectedMap->game_name.empty())
				SelectedMap->game_name = I::Engine()->GetLevelName();


			if (X1Gui().Button(!MainSettings().Russian ? ("Remove") : (u8"�������"), Vec2(long_item_w, 22)))
			{
				while (SelectedMap)
				{
					if (SelectedMap->helpers.empty())
						SelectedMap = nullptr;
					else
						RemvMapHelp(SelectedMap, &SelectedMap->helpers[0]);
				}
			}
		}
	}
	else if (Mode == 1)
	{
		char NameBuf[64] = { 0 };
		char DescpBuf[300] = { 0 };
		char MapNameBuf[300] = { 0 };
		static string tempMapName = "";

		strcpy(MapNameBuf, tempMapName.c_str());
		ChangeGHI(&InfoProt, NameBuf, DescpBuf, MapNameBuf, true);
		tempMapName = MapNameBuf;

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		if (X1Gui().Button(!MainSettings().Russian ? ("Add") : (u8"��������"), Vec2(long_item_w, 22)))
		{
			AddHelp(tempMapName.empty() ? I::Engine()->GetLevelName() : tempMapName, InfoProt);

			InfoProt = GHInfo();
			SelectedGHInf = nullptr;
		}
	}
	X1Gui().Spacing();
	X1Gui().Separator();
	X1Gui().Spacing();
	if (X1Gui().Button(!MainSettings().Russian ? ("Load") : (u8"��������� ��������"), Vec2(228, 22)))
	{
		if (LoadMaps());
	}
	X1Gui().SameLine();
	if (X1Gui().Button(!MainSettings().Russian ? ("Save") : (u8"��������� ��������"), Vec2(228, 22)))
	{
		if (SaveMaps());
	}
	X1Gui().Spacing();
	X1Gui().Separator();
	X1Gui().Spacing();
	if (X1Gui().Button(!MainSettings().Russian ? ("Reset") : (u8"�������� ��������� ��������"), Vec2(long_item_w, 22)))
	{
		maps.clear();
		SelectedGHInf = nullptr;
		SelectedMap = nullptr;
	}
}