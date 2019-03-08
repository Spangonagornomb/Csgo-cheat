#include "Esp.h"
#include  "../../GUI/Gui.h"

#define SAME_LINE_1 115
#define SAME_LINE_2 230

#define PUSH_1 200
#define PUSH_2 400

void CEsp::Menu()
{
	auto LMenu = [&]()->void
	{
		static int VisType = 0;

		VectorEx<const char*>VisArrEng = { lolc("Player ESP"), lolc("Other ESP") };
		VectorEx<const char*>VisArrRus = { lolc(u8"�����"),lolc(u8"���") };


		TabsLabels(VisType, VisArrEng, VisArrRus, Vec2(320, 19), false);

		X1Gui().SameLine(325);

		if (X1Gui().Button(ShowPreview ? (MainSettings().Russian ? u8"������������ >>>" : "Preview >>>") :
			(MainSettings().Russian ? u8"������������ <<<" : "Preview <<<"), Vec2(136, 19)))
			ShowPreview = !ShowPreview;

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		if (VisType == 1)
		{
			X1Gui().PushItemWidth(PUSH_1);

			DCheckBox("Grenades", u8"�������", WorldGrenade, u8"���������� �������");
			if (WorldGrenade)
			{
				X1Gui().SameLine(SAME_LINE_1);
				X1Gui().PushItemWidth(PUSH_1);
				VectorEx<const char*>itemsGSEng = { lolc("Text") , lolc("Circle"), lolc("Icon") };
				VectorEx<const char*>itemsGSRus = { lolc(u8"�����") , lolc(u8"����"), lolc(u8"������") };
				DComboBox("Style##grenade", u8"�����##�����", WorldGrenadeStyle, itemsGSEng, itemsGSRus, "");

				if (WorldGrenadeStyle != 1)
				{
					X1Gui().PushItemWidth(PUSH_2);
					SliderInts("Text size##gre", u8"�����##��", TextGrenadeSize, 9, MAX_FONT_SIZE, u8"");
				}
				X1Gui().Spacing();

				DColorEdit("High explosive grenade", u8"���������� �������", HEGrenadeColor);
				X1Gui().SameLine(SAME_LINE_2);
				DColorEdit("Molotov", u8"�������� ��������", MolotovGrenadeColor);

				DColorEdit("Flashbang", u8"�������� �������", FlashGrenadeColor);
				X1Gui().SameLine(SAME_LINE_2);
				DColorEdit("Incendiary grenade", u8"������������� �������", IncGrenadeColor);

				DColorEdit("Smoke grenade", u8"������� �������", SmokeGrenadeColor);
				X1Gui().SameLine(SAME_LINE_2);
				DColorEdit("Decoy grenade", u8"������ �������", DecoyGrenadeColor);
			}
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();
			DCheckBox("Weapons", u8"������", WorldWeapons, u8"���������� ������ �� �����");
			if (WorldWeapons)
			{
				X1Gui().SameLine(SAME_LINE_1);
				X1Gui().PushItemWidth(PUSH_1);
				VectorEx<const char*>itemsWSEng = { lolc("Text"), lolc("Icon") };
				VectorEx<const char*>itemsWSRus = { lolc(u8"�����"), lolc(u8"������") };
				DComboBox("Style##weapon", u8"�����##����", WorldWeaponsStyle, itemsWSEng, itemsWSRus, "");

				X1Gui().PushItemWidth(PUSH_2);
				SliderInts("Text size##weapon", u8"�����##����", TextWorldWeaponSize, 9, MAX_FONT_SIZE, u8"");

				X1Gui().Spacing();
				DColorEdit("Color##weapon", u8"����##����", WorldWeaponsColor);
			}
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();
			DCheckBox("Bomb", u8"�����", WorldBomb, u8"���������� �����");
			if (WorldBomb)
			{
				X1Gui().SameLine(SAME_LINE_1);
				X1Gui().PushItemWidth(PUSH_1);
				VectorEx<const char*>itemsBSEng = { lolc("Text"), lolc("Icon") };
				VectorEx<const char*>itemsBSRus = { lolc(u8"�����"), lolc(u8"������") };
				DComboBox("Style##bomb", u8"�����##����", WorldBombStyle, itemsBSEng, itemsBSRus, "");

				X1Gui().PushItemWidth(PUSH_2);
				SliderInts("Text size##bomb", u8"������ ������##����", TextBombSize, 9, MAX_FONT_SIZE, u8"");

				X1Gui().Spacing();
				DColorEdit("Color##bomb", u8"����##����", WorldBombColor);
			}
			DCheckBox("Bomb info", u8"���������� � �����", WorldBombInfo, u8"���������� ���������� � �����");
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();
			DCheckBox("Grenade trajectory", u8"���������� �������", GranadePrediction, u8"���������� ���������� ������ �������");
			if (GranadePrediction)
			{
				X1Gui().Spacing();
				DColorEdit("Color##trajgr", u8"����##��", GranadePredictionColor);
			}
		}
		else if (VisType == 0)
		{

			DCheckBox("Enemy", u8"�����", Enemy, u8"�������� �� ������");
			X1Gui().SameLine(SAME_LINE_1);
			DCheckBox("Team", u8"�������", Team, u8"�������� �� ���� �������");
			X1Gui().SameLine(SAME_LINE_2);
			DCheckBox("Visible only", u8"������ �������", BoxVisbleOnly, u8"�������� ������ �� �������");
			X1Gui().PushItemWidth(100);
			DCheckBox("Bind", u8"����", BindEnable.Enable, u8"���� ��������� ��������");
			if (BindEnable.Enable)
			{
				X1Gui().SameLine(SAME_LINE_1);
				DCheckBox("Hold", u8"���������", BindEnable.Hold, u8"�������� ������ �������� ������ ���� ������ ������");
				X1Gui().SameLine(SAME_LINE_2);
				HotsKey("Button", u8"������", BindEnable.Button, u8"������ ������ ��������� ��������");
			}

			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			X1Gui().PushItemWidth(PUSH_1);

			DCheckBox("Box", u8"����", BoxEnable, u8"�������� ���� �� ������");
			if (BoxEnable)
			{
				X1Gui().SameLine(SAME_LINE_1);
				X1Gui().PushItemWidth(PUSH_1);
				VectorEx<const char*>itemsBSEng = { lolc("Full") , lolc("Angular"), lolc("3D"), lolc("Circled") };
				VectorEx<const char*>itemsBSRus = { lolc(u8"������") , lolc(u8"�������"), lolc(u8"3D"), lolc(u8"������������") };
				DComboBox("Style##box", u8"�����##����", BoxStyle, itemsBSEng, itemsBSRus, "");
				if (BoxStyle != 2)
				{
					X1Gui().Spacing();
					X1Gui().PushItemWidth(PUSH_2);
					SliderInts("Size##box", u8"������##����", BoxSize, 1, 20, u8"������ �����");
				}
				X1Gui().PushItemWidth(PUSH_1);
				X1Gui().Spacing();
				DCheckBox("Outline##box", u8"�������##����", BoxOutline, u8"�������� ������� �� �����");
				X1Gui().Spacing();

				DColorEdit("CT color", u8"���� ��", BoxCT);
				X1Gui().SameLine(100);
				DColorEdit("T color", u8"���� �", BoxTT);

				X1Gui().SameLine(200);

				DColorEdit("Visible CT color", u8"������� ���� ��", BoxVisbleCT);
				X1Gui().SameLine(340);
				DColorEdit("Visible T color", u8"������� ���� �", BoxVisbleTT);
			}

			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			DCheckBox("Fill box", u8"������� �����", FillBox, u8"�������� ������� �����");
			if (FillBox)
			{
				X1Gui().SameLine(SAME_LINE_2);
				DColorEdit("Color##fillbox", u8"����##�������", FillBoxColor);
			}

			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			DCheckBox("Chams", u8"�����", Chams, u8"�������� ����� �� ������");
			if (Chams)
			{
				X1Gui().SameLine(SAME_LINE_1);
				X1Gui().PushItemWidth(PUSH_1);
				VectorEx<const char*>itemsCSEng = { lolc("Textured"), lolc("Flat"), lolc("Wireframe"), lolc("Chipolino") };
				VectorEx<const char*>itemsCSRus = { lolc(u8"����������") ,lolc(u8"�������"), lolc(u8"��������"), lolc(u8"��������"), lolc(u8"�������������") };
				DComboBox("Style##chams", u8"�����##����", ChamsStyle, itemsCSEng, itemsCSRus, u8"������������� ����� �������� ������ �� ����� ������� ����������");
				X1Gui().SameLine(SAME_LINE_1 + PUSH_1 + 40);
				DCheckBox("Visible only##chams", u8"�������##����", ChamsVisbleOnly, u8"������ ������� �����");

				if (ChamsStyle == 3)
				{
					X1Gui().Spacing();
					X1Gui().PushItemWidth(PUSH_2);
					SliderFloats("Brightness", u8"�������", ChipolinoFixColor, 0.0000000001, 1, u8"");
				}
				X1Gui().Spacing();
				DColorEdit("CT color##chams", u8"���� ��##����", ChamsCT);
				X1Gui().SameLine(100);
				DColorEdit("T color##chams", u8"���� �##����", ChamsTT);

				X1Gui().SameLine(200);

				DColorEdit("Visible CT color##chams", u8"������� ���� ��##����", ChamsVisbleCT);
				X1Gui().SameLine(340);
				DColorEdit("Visible T color##chams", u8"������� ���� �##����", ChamsVisbleTT);
			}

			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			DCheckBox("Health", u8"��������", Health, u8"���������� �������� ������");
			X1Gui().SameLine(SAME_LINE_1);
			DCheckBox("Armor", u8"�����", Armor, u8"���������� ����� ������");
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();
			DCheckBox("Name", u8"���", Name, u8"���������� ��� ������");
			DCheckBox("Rank", u8"����", Rank, u8"���������� ���� ������ � ����");
			DCheckBox("Distance", u8"���������", Distance, u8"���������� ��������� �� ������");
			DCheckBox("Weapon", u8"������", Weapon, u8"���������� ������ ������");
			X1Gui().SameLine(SAME_LINE_1);
			DCheckBox("Ammo", u8"�������", Ammo, u8"���������� ������� ������");
			DCheckBox("Money", u8"������", Money, u8"���������� ������ ������");
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();
			DCheckBox("Flashed", u8"����������", Flashing, u8"�������� ���� ����� ��������");
			X1Gui().SameLine();
			DCheckBox("Defusing", u8"��������", Defusing, u8"�������� ���� ����� �������");
			X1Gui().SameLine();
			DCheckBox("Scoped", u8"������������", Scope, u8"�������� ���� ����� ����������");
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			DCheckBox("Head", u8"������", HeadEsp, u8"�������� ���� ������ ������");
			if (HeadEsp)
			{
				X1Gui().SameLine(SAME_LINE_1);
				DColorEdit("Color##head", u8"����##���", HeadEspColor);
			}

			DCheckBox("Line", u8"�����", Line, u8"�������� ����� �� �������");
			if (Line)
			{
				X1Gui().SameLine(SAME_LINE_1);
				DColorEdit("Color##line", u8"����##���", LineColor);
			}

			DCheckBox("Skeleton", u8"�������", Skeleton, u8"�������� ������� �������");
			if (Skeleton)
			{
				X1Gui().SameLine(SAME_LINE_1);
				DColorEdit("Color##skel", u8"����##����", SkeletonColor);
			}
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();
			DCheckBox("Sounds", u8"�������� ���", SoundEspEnable, u8"�������� ����������� �����");
			if (SoundEspEnable)
			{
				X1Gui().SameLine(SAME_LINE_1);
				VectorEx<const char*>itemsSEEng = { lolc("Circle"), lolc("Dynamic circle(+)"), lolc("Dynamic circle(-)"), lolc("Box"), lolc("Filled box"), lolc("Star") };
				VectorEx<const char*>itemsSERus = { lolc(u8"����"), lolc(u8"������������ ����(+)"), lolc(u8"������������ ����(-)"),lolc(u8"����"), lolc(u8"������� ����"),lolc(u8"������") };
				DComboBox("Style##sound", u8"�����##����", SoundEspStyle, itemsSEEng, itemsSERus, "");

				X1Gui().Spacing();
				X1Gui().PushItemWidth(PUSH_2);
				SliderInts("Size##sound", u8"������##����", SoundEspSize, 1, 100, u8"������ ������");
				X1Gui().PushItemWidth(PUSH_1);
				X1Gui().Spacing();
				DCheckBox("Distance##sound", u8"���������##����", SoundEspDistance, u8"�������� ����������� ��������� �� �����");
				if (SoundEspDistance)
				{
					X1Gui().SameLine(SAME_LINE_1);
					DColorEdit("Color##sounddistance", u8"����##��������", SoundDistanceColor);

					X1Gui().PushItemWidth(PUSH_2);
					SliderInts("Text Size##sound", u8"�����##��������", TextSoundSize, 9, MAX_FONT_SIZE, u8"");
				}
				DColorEdit("Color##sound", u8"����##����", SoundColor);
			}
		}
	};
	LMenu();
}

#define iText(eng,rus)

void CEsp::VisualPreview()
{
	static MoveStruct *MV_Hp = &MVItemList[MI_HEALTH_BAR];
	static MoveStruct *MV_Armor = &MVItemList[MI_ARMOR_BAR];
	static MoveStruct *MV_Hp_Text = &MVItemList[MI_HEALTH_TEXT];
	static MoveStruct *MV_Armor_Text = &MVItemList[MI_ARMOR_TEXT];
	static MoveStruct *MV_Name = &MVItemList[MI_NAME_TEXT];
	static MoveStruct *MV_Weapon = &MVItemList[MI_WEAPON_TEXT];
	static MoveStruct *MV_Ammo = &MVItemList[MI_AMMO_TEXT];
	static MoveStruct *MV_Distance = &MVItemList[MI_DISTANCE_TEXT];
	static MoveStruct *MV_Money = &MVItemList[MI_MONEY_TEXT];
	static MoveStruct *MV_Scope = &MVItemList[MI_SCOPE_TEXT];
	static MoveStruct *MV_Flashed = &MVItemList[MI_FLASHED_TEXT];
	static MoveStruct *MV_Defusing = &MVItemList[MI_DEFUSING_TEXT];

	Vec2 ZoneStartPos = X1Gui().GetCursorPosEx();

	CustomVisuals(ZoneStartPos);

	if ((MITypes)MoveStruct::LastActive == MI_AMMO_TEXT)
	{
		iText("Selected: Ammo", u8"�������: �������");

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		VectorEx<const char*>itemsGSEng = { lolc("Type 1") ,lolc("Type 2"), lolc("Type 3") };
		VectorEx<const char*>itemsGSRus = { lolc(u8"���1") , lolc(u8"���2"),lolc(u8"���3") };
		DComboBox("Style", u8"�����", AmmoStyle, itemsGSEng, itemsGSRus, "");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		SliderInts("Text size", u8"������ ������", TextAmmoSize, 9, MAX_FONT_SIZE, u8"");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		DColorEdit("Color", u8"����", AmmoColor);
	}
	else if ((MITypes)MoveStruct::LastActive == MI_ARMOR_TEXT)
	{
		iText("Selected: Armor", u8"�������: �����");

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		int OldStyle = ArmorStyle;

		VectorEx<const char*>NumStyle = { lolc("Line") , lolc("Text") };
		VectorEx<const char*>NumStyleRus = { lolc(u8"�����") , lolc(u8"�����") };

		DComboBox("Style", "�����", ArmorStyle, NumStyle, NumStyleRus, "");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		SliderInts("Text size", u8"������ ������", TextArmorSize, 9, MAX_FONT_SIZE, u8"");

		if (OldStyle != ArmorStyle)
			MoveStruct::LastActive = MI_ARMOR_BAR;

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		DColorEdit("Color", u8"����", ArmorColor);
	}
	else if ((MITypes)MoveStruct::LastActive == MI_ARMOR_BAR)
	{
		iText("Selected: Armor", u8"�������: �����");

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		int OldStyle = ArmorStyle;

		VectorEx<const char*>NumStyle = { lolc("Line") , lolc("Text") };
		VectorEx<const char*>NumStyleRus = { lolc(u8"�����") , lolc(u8"�����") };

		DComboBox("Style", u8"�����", ArmorStyle, NumStyle, NumStyleRus, "");


		if (OldStyle != ArmorStyle)
			MoveStruct::LastActive = MI_ARMOR_TEXT;

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		DColorEdit("Main color", u8"������� ����", ArmorColor);
		DColorEdit("Background color", u8"���� ����", ArmorColor2);
		DColorEdit("Outline color", u8"���� �������", ArmorColor3);

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		SliderInts("Separators", u8"�����������", MV_Armor->NumParam1, 0, 10, u8"���������� ������������");
		SliderInts("Tickness", u8"�������", MV_Armor->NumParam2, 0, 20, u8"������� �������");

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		DCheckBox("Background", u8"���", MV_Armor->boolParam1, u8"");
		X1Gui().SameLine();
		DCheckBox("Outline", u8"�������", MV_Armor->boolParam2, u8"");
		X1Gui().SameLine();
		DCheckBox("Inverse", u8"��������", MV_Armor->boolParam3, u8"");
	}
	else if ((MITypes)MoveStruct::LastActive == MI_DISTANCE_TEXT)
	{
		iText("Selected: Selected", u8"�������: ���������");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		SliderInts("Text size", u8"������ ������", TextDistanceSize, 9, MAX_FONT_SIZE, u8"");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		DColorEdit("Color", u8"����", DistanceColor);
	}
	else if ((MITypes)MoveStruct::LastActive == MI_HEALTH_TEXT)
	{
		iText("Selected: Health", u8"�������: ��������");

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		VectorEx<const char*>NumStyle = { lolc("Line"), lolc("Text") };
		VectorEx<const char*>NumStyleRus = { lolc(u8"�����"), lolc(u8"�����") };


		int OldStyle = HealthStyle;

		DComboBox("Style", u8"�����", HealthStyle, NumStyle, NumStyleRus, "");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		SliderInts("Text size", u8"������ ������", TextHealthSize, 9, MAX_FONT_SIZE, u8"");

		if (OldStyle != HealthStyle)
			MoveStruct::LastActive = MI_HEALTH_BAR;

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		DColorEdit("Color", u8"����", HealthColor);
	}
	else if ((MITypes)MoveStruct::LastActive == MI_HEALTH_BAR)
	{
		iText("Selected: Health", u8"�������: ��������");

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		VectorEx<const char*>NumStyle = { lolc("Line"), lolc("Text") };
		VectorEx<const char*>NumStyleRus = { lolc(u8"�����"), lolc(u8"�����") };

		int OldStyle = HealthStyle;

		DComboBox("Style", u8"�����", HealthStyle, NumStyle, NumStyleRus, "");

		if (OldStyle != HealthStyle)
			MoveStruct::LastActive = MI_HEALTH_TEXT;

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		DColorEdit("Main Color", u8"������� ����", HealthColor);
		DColorEdit("Background Color", u8"���� ����", HealthColor2);
		DColorEdit("Outline Color", u8"���� �������", HealthColor3);
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		SliderInts("Separators", u8"�����������", MV_Hp->NumParam1, 0, 10, u8"���������� ������������");
		SliderInts("Tickness", u8"�������", MV_Hp->NumParam2, 0, 20, u8"������� �������");

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		DCheckBox("Background", u8"���", MV_Hp->boolParam1, u8"");
		X1Gui().SameLine();
		DCheckBox("Outline", u8"�������", MV_Hp->boolParam2, u8"");
		X1Gui().SameLine();
		DCheckBox("Inverse", u8"��������", MV_Hp->boolParam3, u8"");
	}
	else if ((MITypes)MoveStruct::LastActive == MI_NAME_TEXT)
	{
		iText("Selected: Name", u8"�������: ���");

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		SliderInts("Text size", u8"������ ������", TextNameSize, 9, MAX_FONT_SIZE, u8"");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		DColorEdit("Color", u8"����", NameColor);

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		DCheckBox("Background", u8"���", NameBackground, u8"��� ��� �����");

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		if (NameBackground)
		{
			X1Gui().Spacing();
			DColorEdit("Background color", u8"����", NameColorBack);
		}
	}
	else if ((MITypes)MoveStruct::LastActive == MI_WEAPON_TEXT)
	{
		iText("Selected: Weapon", u8"�������: ������");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		VectorEx<const char*>itemsGSEng = { lolc("Text"), lolc("Icon") };
		VectorEx<const char*>itemsGSRus = { lolc(u8"�����"), lolc(u8"������") };
		DComboBox("Style", u8"�����", WeaponStyle, itemsGSEng, itemsGSRus, "");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		SliderInts("Text size", u8"������ ������", TextWeaponSize, 9, MAX_FONT_SIZE, u8"");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		DColorEdit("Color", u8"����", WeaponColor);
	}
	else if ((MITypes)MoveStruct::LastActive == MI_MONEY_TEXT)
	{
		iText("Selected: Money", u8"�������: ������");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		SliderInts("Text size", "������ ������", TextMoneySize, 9, MAX_FONT_SIZE, u8"");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		DColorEdit("Color", u8"����", MoneyColor);
	}
	else if ((MITypes)MoveStruct::LastActive == MI_SCOPE_TEXT)
	{
		iText("Selected: Scoped", u8"�������: ������������");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		SliderInts("Text size", u8"������ ������", TextScopedSize, 9, MAX_FONT_SIZE, u8"");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		DColorEdit("Color", u8"����", ScopeColor);
	}
	else if ((MITypes)MoveStruct::LastActive == MI_FLASHED_TEXT)
	{
		iText("Selected: Flashed", u8"�������: ����������");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		SliderInts("Text size", u8"������ ������", TextFlashedSize, 9, MAX_FONT_SIZE, u8"");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		DColorEdit("Color", u8"����", FlashingColor);

	}
	else if ((MITypes)MoveStruct::LastActive == MI_DEFUSING_TEXT)
	{
		iText("Selected: Defusing", u8"�������: ��������");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		SliderInts("Text size", u8"������ ������", TextDefusingSize, 9, MAX_FONT_SIZE, u8"");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		DColorEdit("Color", u8"����", DefusingColor);
	}

	float long_item_w = X1Gui().GetThis()->Info.Size.x - (X1Gui().GetStyle().wndPadding.x * 2);
	X1Gui().SetCursorPos(Vec2(X1Gui().GetStyle().wndPadding.x, 638), 0);
	if (X1Gui().Button(("Reset"), Vec2(long_item_w, 22)))
		MoveStruct::InitMItems = false;
}
