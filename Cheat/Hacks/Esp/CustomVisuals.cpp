#include "Esp.h"
#include  "../../GUI/Gui.h"

#define IMAGE_X 110
#define IMAGE_Y 245

#define IMAGE_OUTLINE 50

#define LINES_X 70
#define LINES_Y 80

#define BOX_POS_X 45
#define BOX_POS_Y 20

#define BOX_SIZE_X 160
#define BOX_SIZE_Y 310

#define MV_HEALTH_TEXT ("90HP")
#define MV_ARMOR_TEXT ("90A")

#define MV_NAME_TEXT ("Name")
#define MV_WEAPON_TEXT ("M4A4")
#define MV_WEAPON_TEXT_ICO ("S")
#define MV_DISTANCE_TEXT ("5m")
#define MV_MONEY_TEXT ("1000$")
#define MV_SCOPE_TEXT ("SCOPED")
#define MV_FLASHED_TEXT ("FLASHED")
#define MV_DEFUSING_TEXT ("DEFUSING")

#define FULL_BACK_SIZE_X 100
#define FULL_BACK_SIZE_Y 100

#define SELECTED_BORDER_SIZE 4

#define VIS_PREW_ZONE_X 350 
#define VIS_PREW_ZONE_Y 400 

bool IsMousPress = false;
bool IsMousClick = false;

int BoxSize_Y = 0;
int BoxSize_X = 0;

bool IsBlockMove = false;

int CEsp::MoveStruct::ActiveIdx = -1;
int CEsp::MoveStruct::LastActive = -1;
bool CEsp::MoveStruct::InitMItems = false;

LPDIRECT3DTEXTURE9 VisualBodyImage;

void CEsp::Move(MoveStruct &Cont, bool IsPressed, Vec2 MouseP, Vec2 StartP, Vec2 CLines, bool IsText)
{
	GuiIO& g =  X1Gui().GetIO();
	bool ChangeSide = false;
	if (MoveStruct::ActiveIdx == Cont.Id || MoveStruct::ActiveIdx == -1)
	{
		if (IsPressed && Cont.IsHover && Cont.Set)
		{
			MoveStruct::ActiveIdx = Cont.Id;
			MoveStruct::LastActive = Cont.Id;

			Cont.Offset.x = StartP.x - (MouseP.x - Cont.Correct.x);
			Cont.Offset.y = StartP.y - (MouseP.y - Cont.Correct.y);

			if (!IsText)
			{
				Sides OSide = Cont.Side;

				if (Cont.Center.x > CLines.x) { Cont.Side = RIGHT_S; }
				if (Cont.Center.x < CLines.x) { Cont.Side = LEFT_S; }

				if (g.MousePos.y < CLines.y + (BoxSize_Y / 8)) { Cont.Side = TOP_S; }
				if (g.MousePos.y > CLines.y + BoxSize_Y - (BoxSize_Y / 8)) { Cont.Side = DOWN_S; }

				if (OSide != Cont.Side && ((OSide != LEFT_S || Cont.Side != RIGHT_S) && (OSide != RIGHT_S || Cont.Side != LEFT_S)))
					ChangeSide = true;
			}
			else
			{
				if (Cont.Center.x > CLines.x) { Cont.Side = RIGHT_S; }
				if (Cont.Center.x < CLines.x) { Cont.Side = LEFT_S; }

				if (Cont.Center.x > CLines.x && Cont.Center.y > CLines.y + (BoxSize_Y / 3)) { Cont.Side = CENTER_RIGHT_S; }
				if (Cont.Center.x < CLines.x && Cont.Center.y > CLines.y + (BoxSize_Y / 3)) { Cont.Side = CENTER_LEFT_S; }

				if (Cont.Center.x > CLines.x && Cont.Center.y > CLines.y + BoxSize_Y - (BoxSize_Y / 3)) { Cont.Side = DOWN_RIGHT_DOWN_S; }
				if (Cont.Center.x < CLines.x && Cont.Center.y > CLines.y + BoxSize_Y - (BoxSize_Y / 3)) { Cont.Side = DOWN_LEFT_DOWN_S; }

				if (Cont.Center.y < CLines.y) { Cont.Side = TOP_S; }
				if (Cont.Center.y < CLines.y && Cont.Center.x < CLines.x - (BoxSize_X / 4)) { Cont.Side = LEFT_TOP_S; }
				if (Cont.Center.y < CLines.y && Cont.Center.x > CLines.x + (BoxSize_X / 4)) { Cont.Side = RIGHT_TOP_S; }

				if (Cont.Center.y > CLines.y + BoxSize_Y) { Cont.Side = DOWN_S; }
				if (Cont.Center.y > CLines.y + BoxSize_Y && Cont.Center.x < CLines.x - (BoxSize_X / 4)) { Cont.Side = LEFT_DOWN_S; }
				if (Cont.Center.y > CLines.y + BoxSize_Y && Cont.Center.x > CLines.x + (BoxSize_X / 4)) { Cont.Side = RIGHT_DOWN_S; }
			}
		}
		else
			MoveStruct::ActiveIdx = -1;

		Cont.Position = Vec2(StartP.x - Cont.Offset.x, StartP.y - Cont.Offset.y);

		if ((MouseP.x < (Cont.Position.x + Cont.Size.x) && MouseP.x > Cont.Position.x) && (MouseP.y < (Cont.Position.y + Cont.Size.y) && MouseP.y > Cont.Position.y))
			Cont.IsHover = true;
		else
			Cont.IsHover = false;

		if (IsPressed && Cont.IsHover)
		{
			if (!Cont.Set && !IsBlockMove)
			{
				Cont.Set = true;
				Cont.Correct.x = MouseP.x - Cont.Position.x;
				Cont.Correct.y = MouseP.y - Cont.Position.y;
			}
		}
		else
			Cont.Set = false;

		Cont.Center = Vec2(Cont.Position.x + Cont.Size.x / 2, Cont.Position.y + Cont.Size.y / 2);
		if (!IsText)
		{
			if (Cont.Side == DOWN_S || Cont.Side == TOP_S)
				Cont.Size = Cont.SizeH;

			if (Cont.Side == LEFT_S || Cont.Side == RIGHT_S)
				Cont.Size = Cont.SizeV;

			if (ChangeSide)
			{
				Cont.Center = MouseP;

				Cont.Offset.x = StartP.x - (MouseP.x - Cont.Size.x / 2);
				Cont.Offset.y = StartP.y - (MouseP.y - Cont.Size.y / 2);

				Cont.Position = Vec2(StartP.x - Cont.Offset.x, StartP.y - Cont.Offset.y);

				Cont.Correct.x = MouseP.x - Cont.Position.x;
				Cont.Correct.y = MouseP.y - Cont.Position.y;
			}
		}
	}
}

void CEsp::CustomVisuals(Vec2 Start)
{
	CGui* Draw = &X1Gui();
	GuiIO& g = X1Gui().GetIO();

	string MV_AMMO_TEXT = "";

	if (AmmoStyle == 0)
	{
		MV_AMMO_TEXT = ("[30]");
	}
	else if (AmmoStyle == 1)
	{
		MV_AMMO_TEXT = ("[30\\90]");
	}
	else if (AmmoStyle == 2)
	{
		MV_AMMO_TEXT = ("[120]");
	}

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

	MV_Hp_Text->Size = GP_Render->CalcTextSize(MV_HEALTH_TEXT, GP_Render->SzFonts[TextHealthSize]);
	MV_Armor_Text->Size = GP_Render->CalcTextSize(MV_ARMOR_TEXT, GP_Render->SzFonts[TextArmorSize]);
	MV_Name->Size = GP_Render->CalcTextSize(MV_NAME_TEXT, GP_Render->SzFonts[TextNameSize]);
	MV_Ammo->Size = GP_Render->CalcTextSize(MV_AMMO_TEXT.c_str(), GP_Render->SzFonts[TextAmmoSize]);
	MV_Distance->Size = GP_Render->CalcTextSize(MV_DISTANCE_TEXT, GP_Render->SzFonts[TextDistanceSize]);
	MV_Money->Size = GP_Render->CalcTextSize(MV_MONEY_TEXT, GP_Render->SzFonts[TextMoneySize]);
	MV_Flashed->Size = GP_Render->CalcTextSize(MV_FLASHED_TEXT, GP_Render->SzFonts[TextFlashedSize]);
	MV_Scope->Size = GP_Render->CalcTextSize(MV_SCOPE_TEXT, GP_Render->SzFonts[TextScopedSize]);
	MV_Defusing->Size = GP_Render->CalcTextSize(MV_DEFUSING_TEXT, GP_Render->SzFonts[TextDefusingSize]);

	if (WeaponStyle == 0)
		MV_Weapon->Size = GP_Render->CalcTextSize(MV_WEAPON_TEXT, GP_Render->SzFonts[TextWeaponSize]);
	else if (WeaponStyle == 1)
		MV_Weapon->Size = GP_Render->CalcTextSize(MV_WEAPON_TEXT_ICO, GP_Render->SzFontsIcon[TextWeaponSize]);

	MV_Hp->IsEnabled = Health && HealthStyle == 0;
	MV_Armor->IsEnabled = Armor && ArmorStyle == 0;

	MV_Hp_Text->IsEnabled = Health && HealthStyle == 1;
	MV_Armor_Text->IsEnabled = Armor && ArmorStyle == 1;

	MV_Name->IsEnabled = Name;
	MV_Weapon->IsEnabled = Weapon;
	MV_Ammo->IsEnabled = Ammo;
	MV_Distance->IsEnabled = Distance;
	MV_Money->IsEnabled = Money;
	MV_Scope->IsEnabled = Scope;
	MV_Flashed->IsEnabled = Flashing;
	MV_Defusing->IsEnabled = Defusing;

	color_t LinesColor = color_t(1.f*255.f, 1.f*255.f, 1.f*255.f, 0.1f*255.f);
	color_t OutLineColor = color_t(1.f*255.f, 1.f*255.f, 1.f*255.f, 0.3f*255.f);
	color_t BackColor = color_t(0.5f*255.f, 0.5f*255.f, 0.5f*255.f, 0.5f*255.f);

	IsMousPress = g.LMPressDur > 0.0f;
	IsMousClick = g.LMClicked;

	Vec2 MousePos = g.MousePos;

	auto FixPos = [](Vec2 Max, Vec2 Min, Vec2& Cur)
	{
		if (Cur.x > Max.x - 5) { Cur.x = Max.x - 5; }
		if (Cur.y > Max.y - 5) { Cur.y = Max.y - 5; }
		if (Cur.x < Min.x + 5) { Cur.x = Min.x + 5; }
		if (Cur.y < Min.y + 5) { Cur.y = Min.y + 5; }
	};

	auto DrawVBox = [Draw](Vec2 Pos, Vec2 Size, color_t Color)
	{
		Pos = Vec2(int(Pos.x), int(Pos.y));
		Draw->DrawLine(Vec2(Pos.x - 0.5f, Pos.y), Vec2(Pos.x + Size.x, Pos.y), Color);
		Draw->DrawLine(Vec2(Pos.x, Pos.y - 0.5f), Vec2(Pos.x, Pos.y + Size.y), Color);
		Draw->DrawLine(Vec2(Pos.x, Pos.y + Size.y), Vec2(Pos.x + Size.x + 0.5f, Pos.y + Size.y), Color);
		Draw->DrawLine(Vec2(Pos.x + Size.x, Pos.y), Vec2(Pos.x + Size.x, Pos.y + Size.y + 0.5f), Color);
	};

	auto DrawVBoxCorner = [Draw](Vec2 Pos, Vec2 Size, color_t Color)
	{
		Pos = Vec2(int(Pos.x), int(Pos.y));
		/*

		01-----------11
		2             2
		|             |
		|             |
		1             1
		22-----------23
		*/

		int LSize_X = Size.x / 4;
		int LSize_Y = Size.y / 4;

		/*01*/
		Draw->DrawLine(
			Vec2(Pos.x, Pos.y),
			Vec2(Pos.x + LSize_X, Pos.y),
			Color);

		/*02*/
		Draw->DrawLine(
			Vec2(Pos.x, Pos.y),
			Vec2(Pos.x, Pos.y + LSize_Y),
			Color);

		/*11*/
		Draw->DrawLine(
			Vec2(Pos.x + Size.x - LSize_X, Pos.y),
			Vec2(Pos.x + Size.x, Pos.y),
			Color);

		/*12*/
		Draw->DrawLine(
			Vec2(Pos.x + Size.x, Pos.y),
			Vec2(Pos.x + Size.x, Pos.y + LSize_Y),
			Color);


		/*21*/
		Draw->DrawLine(
			Vec2(Pos.x, Pos.y + Size.y - LSize_Y),
			Vec2(Pos.x, Pos.y + Size.y),
			Color);

		/*22*/
		Draw->DrawLine(
			Vec2(Pos.x, Pos.y + Size.y),
			Vec2(Pos.x + LSize_X, Pos.y + Size.y),
			Color);

		/*31*/
		Draw->DrawLine(
			Vec2(Pos.x + Size.x, Pos.y + Size.y - LSize_Y),
			Vec2(Pos.x + Size.x, Pos.y + Size.y),
			Color);

		/*32*/
		Draw->DrawLine(
			Vec2(Pos.x + Size.x - LSize_X, Pos.y + Size.y),
			Vec2(Pos.x + Size.x, Pos.y + Size.y),
			Color);
	};

	auto Background = [Draw, DrawVBox, LinesColor, OutLineColor, BackColor](Vec2  Min, Vec2 Max, Vec2 MenuPos)
	{

		Draw->DrawFilledBox(Min, Vec2(VIS_PREW_ZONE_X, VIS_PREW_ZONE_Y), BackColor);

		for (int i(1); i <= LINES_Y; i++)
			Draw->DrawLine(Vec2(Min.x, Min.y + (VIS_PREW_ZONE_Y / LINES_Y)*i), Vec2(Min.x + VIS_PREW_ZONE_X, Min.y + (VIS_PREW_ZONE_Y / LINES_Y)*i), LinesColor);
		for (int i(1); i <= LINES_X; i++)
			Draw->DrawLine(Vec2(Min.x + (VIS_PREW_ZONE_X / LINES_X)*i, Min.y), Vec2(Min.x + (VIS_PREW_ZONE_X / LINES_X)*i, Min.y + VIS_PREW_ZONE_Y), LinesColor);

		DrawVBox(Min, Vec2(VIS_PREW_ZONE_X, VIS_PREW_ZONE_Y), OutLineColor);
	};

	auto DrawBarVertical = [Draw, DrawVBox](Vec2 Pos, Vec2 Size, color_t MainColor, color_t BackColor, color_t OutLineColor, int Lines, MoveStruct* Curt)
	{

		Pos = Vec2(int(Pos.x), int(Pos.y));

		if (Curt->boolParam1)
			Draw->DrawFilledBox(Vec2(Pos.x, Pos.y),
				Vec2(Size.x, Size.y),
				BackColor);

		if (!Curt->boolParam3)
			Draw->DrawFilledBox(Vec2(Pos.x, Pos.y + (Size.y / 10)),
				Vec2(Size.x, Size.y - (Size.y / 10)),
				MainColor);
		else
			Draw->DrawFilledBox(Vec2(Pos.x, Pos.y),
				Vec2(Size.x, Size.y - (Size.y / 10)),
				MainColor);

		if (Lines != 0)
		{
			Lines += 1;
			int DistL = Size.y / Lines;

			if (Lines == 1)
				DistL = Size.y / 2;

			for (int i(1); i < Lines; i++)
				Draw->DrawLine(Vec2(Pos.x, Pos.y + DistL * i), Vec2(Pos.x + Size.x, Pos.y + DistL * i), OutLineColor);
		}

		if (Curt->boolParam2)
			DrawVBox(Pos, Size, OutLineColor);
	};

	auto DrawBarHorizontal = [Draw, DrawVBox](Vec2 Pos, Vec2 Size, color_t MainColor, color_t BackColor, color_t OutLineColor, int Lines, MoveStruct* Curt)
	{
		Pos = Vec2(int(Pos.x), int(Pos.y));
		if (Curt->boolParam1)
			Draw->DrawFilledBox(Vec2(Pos.x, Pos.y),
				Vec2(Size.x, Size.y),
				BackColor);

		if (Curt->boolParam3)
			Draw->DrawFilledBox(Vec2(Pos.x, Pos.y),
				Vec2(Size.x - (Size.x / 10), Size.y),
				MainColor);
		else
			Draw->DrawFilledBox(Vec2(Pos.x + (Size.x / 10), Pos.y),
				Vec2(Size.x - (Size.x / 10), Size.y),
				MainColor);

		if (Lines != 0)
		{
			Lines += 1;
			int DistL = Size.x / Lines;

			if (Lines == 1)
				DistL = Size.x / 2;

			for (int i(1); i < Lines; i++)
				Draw->DrawLine(Vec2(Pos.x + DistL * i, Pos.y), Vec2(Pos.x + DistL * i, Pos.y + Size.y), OutLineColor);
		}

		if (Curt->boolParam2)
			DrawVBox(Pos, Size, OutLineColor);

	};

	Vec2 StartPos = X1Gui().GetCursorPos();

	Vec2 StartPosScreen = X1Gui().GetCursorPos();

	Vec2 TextOffset = Vec2(StartPosScreen.x - StartPos.x, StartPosScreen.y - StartPos.y);
	auto _DrawText = [Draw, TextOffset](string text, MoveStruct* Curt, CFont* font)
	{
		Vec2 Pos = Vec2(Curt->Position.x - TextOffset.x, Curt->Position.y - TextOffset.y);

		if (Curt->boolParam3)
		{
			Vec2 NSize = Curt->Size;
			Vec2 NPos = Curt->Position;

			NSize.x += 30;
			NPos.x -= 15;
			NPos.y += 1.5f;

			float max = NSize.x / 3.f;

			for (int i(0); i < max; i++)
				Draw->DrawFilledBox(Vec2(NPos.x + i, NPos.y),
					Vec2(NPos.x + i + 1, NPos.y + NSize.y),
					color_t(Curt->Color2.G1R(), Curt->Color2.G1G(), Curt->Color2.G1B(), (((float)i / max) * 255) / 255.f));

			Draw->DrawFilledBox(Vec2(NPos.x + max, NPos.y),
				Vec2(NPos.x + (max * 2), NPos.y + NSize.y),
				color_t(Curt->Color2.G1R(), Curt->Color2.G1G(), Curt->Color2.G1B(), 1.f));

			for (int i(0); i < max; i++)
				Draw->DrawFilledBox(Vec2(NPos.x + i + (max * 2), NPos.y),
					Vec2(NPos.x + i + 1 + (max * 2), NPos.y + NSize.y),
					color_t(Curt->Color2.G1R(), Curt->Color2.G1G(), Curt->Color2.G1B(), (255 - (((float)i / max) * 255)) / 255.f));
		}

		if (true)
		{
			Draw->DrawString(font, Pos.x + 1, Pos.y + 1, color_t(0.01f, 0.01f, 0.01f, 1.f), text.c_str());
		}

		Draw->DrawString(font, Pos.x, Pos.y, Curt->Color1, text.c_str());
	};

	Vec2 EndPosScreen = Vec2(StartPosScreen.x + VIS_PREW_ZONE_X, StartPosScreen.y + VIS_PREW_ZONE_Y);

	if ((MousePos.x > StartPosScreen.x && MousePos.x < EndPosScreen.x) &&
		(MousePos.y > StartPosScreen.y && MousePos.y < EndPosScreen.y))
	{

	}

	int FixImgPos_X = ((VIS_PREW_ZONE_X - IMAGE_X) / 2);
	int FixImgPos_Y = ((VIS_PREW_ZONE_Y - IMAGE_Y) / 2);

	Vec2 ImgePos = Vec2(StartPos.x + FixImgPos_X, StartPos.y + FixImgPos_Y);

	Vec2 ScreenImgePos = ImgePos;

	Background(StartPosScreen, EndPosScreen, StartPos);

	if (!VisualBodyImage)
		D3DXCreateTextureFromFileEx(GP_Render->m_pDevice, (CGlobal::SystemDisk + ("obnoxious framework\\Resources\\Images\\Body.png")).c_str(), 222, 579,
			D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &VisualBodyImage);

	if (VisualBodyImage)
		Draw->DrawImage(VisualBodyImage, ImgePos, ImgePos + Vec2(IMAGE_X, IMAGE_Y));

	Draw->DrawLine(Vec2(StartPosScreen.x + (VIS_PREW_ZONE_X / 2), StartPosScreen.y), Vec2(StartPosScreen.x + (VIS_PREW_ZONE_X / 2), StartPosScreen.y + VIS_PREW_ZONE_Y), OutLineColor);
	Draw->DrawLine(Vec2(StartPosScreen.x, StartPosScreen.y + (VIS_PREW_ZONE_Y / 2)), Vec2(StartPosScreen.x + VIS_PREW_ZONE_X, StartPosScreen.y + (VIS_PREW_ZONE_Y / 2)), OutLineColor);

	if (Skeleton)
	{
		Draw->DrawLine(Vec2(StartPosScreen.x + 152 + 20, StartPosScreen.y + 122), Vec2(StartPosScreen.x + 150 + 20, StartPosScreen.y + 103), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 152 + 20, StartPosScreen.y + 122), Vec2(StartPosScreen.x + 157 + 20, StartPosScreen.y + 129), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 157 + 20, StartPosScreen.y + 129), Vec2(StartPosScreen.x + 158 + 20, StartPosScreen.y + 163), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 158 + 20, StartPosScreen.y + 163), Vec2(StartPosScreen.x + 159 + 20, StartPosScreen.y + 194), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 159 + 20, StartPosScreen.y + 194), Vec2(StartPosScreen.x + 180 + 20, StartPosScreen.y + 219), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 180 + 20, StartPosScreen.y + 219), Vec2(StartPosScreen.x + 195 + 20, StartPosScreen.y + 273), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 195 + 20, StartPosScreen.y + 273), Vec2(StartPosScreen.x + 196 + 20, StartPosScreen.y + 306), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 159 + 20, StartPosScreen.y + 194), Vec2(StartPosScreen.x + 139 + 20, StartPosScreen.y + 218), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 139 + 20, StartPosScreen.y + 216), Vec2(StartPosScreen.x + 131 + 20, StartPosScreen.y + 267), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 131 + 20, StartPosScreen.y + 267), Vec2(StartPosScreen.x + 134 + 20, StartPosScreen.y + 286), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 157 + 20, StartPosScreen.y + 132), Vec2(StartPosScreen.x + 121 + 20, StartPosScreen.y + 142), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 121 + 20, StartPosScreen.y + 142), Vec2(StartPosScreen.x + 126 + 20, StartPosScreen.y + 146), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 125 + 20, StartPosScreen.y + 145), Vec2(StartPosScreen.x + 141 + 20, StartPosScreen.y + 134), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 157 + 20, StartPosScreen.y + 132), Vec2(StartPosScreen.x + 183 + 20, StartPosScreen.y + 135), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 181 + 20, StartPosScreen.y + 135), Vec2(StartPosScreen.x + 174 + 20, StartPosScreen.y + 141), SkeletonColor);
		Draw->DrawLine(Vec2(StartPosScreen.x + 161 + 20, StartPosScreen.y + 122), Vec2(StartPosScreen.x + 174 + 20, StartPosScreen.y + 141), SkeletonColor);
	}

	Vec2 StartBoxPos = Vec2(int(ScreenImgePos.x + IMAGE_X / 2), int(ScreenImgePos.y + (IMAGE_Y / 13) - (BoxSize*2.9)));

	Vec2 CLines = Vec2(StartPosScreen.x + (VIS_PREW_ZONE_X / 2), StartBoxPos.y);

	BoxSize_Y = int(IMAGE_Y - (IMAGE_Y / 13) + (BoxSize * 4));
	BoxSize_X = int(BoxSize_Y / 2);
	StartBoxPos.x -= int(BoxSize_X / 2.f);

	int HPTol = MV_Hp->NumParam2;
	int ATol = MV_Armor->NumParam2;

	for (int i(0); (MITypes)i < MI_MAX; i++)
		MVItemList[i].Position = Vec2(int(StartPosScreen.x + MVItemList[i].ZonePos.x), int(StartPosScreen.y + MVItemList[i].ZonePos.y));

	if (!MoveStruct::InitMItems)
	{
		MoveStruct::InitMItems = true;
		MoveStruct::LastActive = MI_MAX;

		/*=====================================================================================*/
		MV_Hp_Text->Position = Vec2(StartBoxPos.x - MV_Hp_Text->Size.x, StartBoxPos.y);
		MV_Hp_Text->StartPos = MV_Hp_Text->Position;
		MV_Hp_Text->Id = MI_HEALTH_TEXT;
		MV_Hp_Text->Side = LEFT_S;
		MV_Hp_Text->TextSize = 17;
		MV_Hp_Text->Offset = Vec2(5, 0);
		MV_Hp_Text->IsText = true;
		MV_Hp_Text->Name = ("MV_HP_Text");
		MV_Hp_Text->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Armor_Text->Position = Vec2(StartBoxPos.x - MV_Armor_Text->Size.x, StartBoxPos.y);
		MV_Armor_Text->StartPos = MV_Armor_Text->Position;
		MV_Armor_Text->Id = MI_ARMOR_TEXT;
		MV_Armor_Text->Side = LEFT_S;
		MV_Armor_Text->TextSize = 17;
		MV_Armor_Text->Offset = Vec2(5, -10);
		MV_Armor_Text->IsText = true;
		MV_Armor_Text->Name = ("MV_Arm_Text");
		MV_Hp_Text->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Hp->Position = Vec2(StartBoxPos.x - HPTol, StartBoxPos.y);
		MV_Hp->StartPos = MV_Hp->Position;
		MV_Hp->Id = MI_HEALTH_BAR;
		MV_Hp->Side = LEFT_S;
		MV_Hp->Size = Vec2(HPTol, BoxSize_Y);
		MV_Hp->Offset = Vec2(5, 0);

		MV_Hp->Color1 = Color(10, 255, 10, 255);
		MV_Hp->Color2 = Color(255, 0, 0, 255);
		MV_Hp->Color3 = Color(0, 0, 0, 255);

		MV_Hp->NumParam1 = 4;
		MV_Hp->NumParam2 = 5;
		MV_Hp->Name = ("MV_HPBar");
		MV_Hp->boolParam1 = true;
		MV_Hp->boolParam2 = true;
		MV_Hp->boolParam3 = false;

		/*=====================================================================================*/
		MV_Armor->Position = Vec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y);
		MV_Armor->StartPos = MV_Armor->Position;
		MV_Armor->Id = MI_ARMOR_BAR;
		MV_Armor->Side = RIGHT_S;
		MV_Armor->Size = Vec2(ATol, BoxSize_Y);
		MV_Armor->Offset = Vec2(-5, 0);

		MV_Armor->Color1 = Color(255, 255, 255, 255);
		MV_Armor->Color2 = Color(255, 0, 0, 255);
		MV_Armor->Color3 = Color(0, 0, 0, 255);

		MV_Armor->NumParam1 = 4;
		MV_Armor->NumParam2 = 5;
		MV_Armor->Name = ("MV_ArmorBar");
		MV_Armor->boolParam1 = true;
		MV_Armor->boolParam2 = true;
		MV_Armor->boolParam3 = false;

		/*=====================================================================================*/
		MV_Name->Position = Vec2(StartBoxPos.x + (BoxSize_X / 2) - (MV_Name->Size.x / 2), StartBoxPos.y - MV_Name->Size.y);
		MV_Name->StartPos = MV_Name->Position;
		MV_Name->Id = MI_NAME_TEXT;
		MV_Name->Side = TOP_S;
		MV_Name->TextSize = 17;
		MV_Name->Offset = Vec2(0, 4);
		MV_Name->IsText = true;
		MV_Name->Name = ("MV_Name");
		MV_Name->boolParam1 = false;

		MV_Name->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Weapon->Position = Vec2(StartBoxPos.x + (BoxSize_X / 2) - (MV_Weapon->Size.x / 2), StartBoxPos.y + BoxSize_Y);
		MV_Weapon->StartPos = MV_Weapon->Position;
		MV_Weapon->Id = MI_WEAPON_TEXT;
		MV_Weapon->Side = DOWN_S;
		MV_Weapon->TextSize = 17;
		MV_Weapon->Offset = Vec2(0, -1);
		MV_Weapon->IsText = true;
		MV_Weapon->Name = ("MV_Weapon");
		MV_Weapon->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Ammo->Position = Vec2(StartBoxPos.x + (BoxSize_X / 2) - (MV_Ammo->Size.x / 2), StartBoxPos.y + BoxSize_Y);
		MV_Ammo->StartPos = MV_Ammo->Position;
		MV_Ammo->Id = MI_AMMO_TEXT;
		MV_Ammo->Side = DOWN_S;
		MV_Ammo->TextSize = 17;
		MV_Ammo->Offset = Vec2(0, -10);
		MV_Ammo->IsText = true;
		MV_Ammo->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Distance->Position = Vec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y + BoxSize_Y - MV_Distance->Size.y);
		MV_Distance->StartPos = MV_Distance->Position;
		MV_Distance->Id = MI_DISTANCE_TEXT;
		MV_Distance->Side = DOWN_RIGHT_DOWN_S;
		MV_Distance->TextSize = 17;
		MV_Distance->Offset = Vec2(-7, 0);
		MV_Distance->IsText = true;
		MV_Distance->Name = ("MV_Distance");
		MV_Distance->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Money->Position = Vec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y);
		MV_Money->StartPos = MV_Money->Position;
		MV_Money->Id = MI_MONEY_TEXT;
		MV_Money->Side = RIGHT_S;
		MV_Money->TextSize = 17;
		MV_Money->Offset = Vec2(-7, 0);
		MV_Money->IsText = true;
		MV_Money->Name = ("MV_Money");
		MV_Money->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Scope->Position = Vec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y);
		MV_Scope->StartPos = MV_Scope->Position;
		MV_Scope->Id = MI_SCOPE_TEXT;
		MV_Scope->Side = RIGHT_S;
		MV_Scope->TextSize = 17;
		MV_Scope->Offset = Vec2(-7, -7);
		MV_Scope->IsText = true;
		MV_Scope->Name = ("MV_Scope");
		MV_Scope->Color1 = Color(255, 255, 255, 255);

		/*=====================================================================================*/
		MV_Flashed->Position = Vec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y + BoxSize_Y / 2);
		MV_Flashed->StartPos = MV_Flashed->Position;
		MV_Flashed->Id = MI_FLASHED_TEXT;
		MV_Flashed->Side = CENTER_RIGHT_S;
		MV_Flashed->TextSize = 17;
		MV_Flashed->Color1 = Color(255, 255, 255, 255);
		MV_Flashed->Offset = Vec2(-7, 0);
		MV_Flashed->IsText = true;
		MV_Flashed->Name = ("MV_Flashed");

		/*=====================================================================================*/
		MV_Defusing->Position = Vec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y + BoxSize_Y / 2);
		MV_Defusing->StartPos = MV_Defusing->Position;
		MV_Defusing->Id = MI_DEFUSING_TEXT;
		MV_Defusing->Side = CENTER_RIGHT_S;
		MV_Defusing->TextSize = 17;
		MV_Defusing->Color1 = Color(255, 255, 255, 255);
		MV_Defusing->Offset = Vec2(-7, -7);
		MV_Defusing->IsText = true;
		MV_Defusing->Name = ("MV_Defusing");
	}

	if (FillBox)
		Draw->DrawFilledBox(StartBoxPos, Vec2(BoxSize_X, BoxSize_Y), FillBoxColor, BoxStyle == 3 ? BoxSize_X / 5.f : 0.0f);

	if (BoxEnable)
	{
		color_t BoxOutLineColor = color_t(0.f, 0.f, 0.f, 255.f);
		if (BoxStyle == 0)
		{
			DrawVBox(StartBoxPos, Vec2(BoxSize_X, BoxSize_Y), BoxVisbleCT);
			if (BoxOutline)
			{
				DrawVBox(Vec2(StartBoxPos.x + 1, StartBoxPos.y + 1), Vec2(BoxSize_X - 2, BoxSize_Y - 2), BoxOutLineColor);
				DrawVBox(Vec2(StartBoxPos.x - 1, StartBoxPos.y - 1), Vec2(BoxSize_X + 2, BoxSize_Y + 2), BoxOutLineColor);
			}
		}
		else if (BoxStyle == 1)
		{
			DrawVBoxCorner(StartBoxPos, Vec2(BoxSize_X, BoxSize_Y), BoxVisbleCT);
			if (BoxOutline)
			{
				DrawVBoxCorner(Vec2(StartBoxPos.x + 1, StartBoxPos.y + 1), Vec2(BoxSize_X - 2, BoxSize_Y - 2), BoxOutLineColor);
				DrawVBoxCorner(Vec2(StartBoxPos.x - 1, StartBoxPos.y - 1), Vec2(BoxSize_X + 2, BoxSize_Y + 2), BoxOutLineColor);

				int LSize_X = BoxSize_X / 4;
				int LSize_Y = BoxSize_Y / 4;

				Draw->DrawLine(Vec2(StartBoxPos.x + LSize_X + 0.5f, StartBoxPos.y - 1), Vec2(StartBoxPos.x + LSize_X + 0.5f, StartBoxPos.y + 1), BoxOutLineColor);
				Draw->DrawLine(Vec2(StartBoxPos.x - 1, StartBoxPos.y + LSize_Y + 0.5f), Vec2(StartBoxPos.x + 1, StartBoxPos.y + LSize_Y + 0.5f), BoxOutLineColor);

				Draw->DrawLine(Vec2(StartBoxPos.x + BoxSize_X - LSize_X - 0.5f, StartBoxPos.y - 1), Vec2(StartBoxPos.x + BoxSize_X - LSize_X - 0.5f, StartBoxPos.y + 1), BoxOutLineColor);
				Draw->DrawLine(Vec2(StartBoxPos.x + BoxSize_X - 1, StartBoxPos.y + LSize_Y + 0.5f), Vec2(StartBoxPos.x + BoxSize_X + 1, StartBoxPos.y + LSize_Y + 0.5f), BoxOutLineColor);

				Draw->DrawLine(Vec2(StartBoxPos.x + LSize_X + 0.5f, StartBoxPos.y + BoxSize_Y - 1), Vec2(StartBoxPos.x + LSize_X + 0.5f, StartBoxPos.y + BoxSize_Y + 1), BoxOutLineColor);
				Draw->DrawLine(Vec2(StartBoxPos.x - 1, StartBoxPos.y + BoxSize_Y - LSize_Y - 0.5f), Vec2(StartBoxPos.x + 1, StartBoxPos.y + BoxSize_Y - LSize_Y - 0.5f), BoxOutLineColor);

				Draw->DrawLine(Vec2(StartBoxPos.x + BoxSize_X - LSize_X - 0.5f, StartBoxPos.y + BoxSize_Y - 1), Vec2(StartBoxPos.x + BoxSize_X - LSize_X - 0.5f, StartBoxPos.y + BoxSize_Y + 1), BoxOutLineColor);
				Draw->DrawLine(Vec2(StartBoxPos.x + BoxSize_X - 1, StartBoxPos.y + BoxSize_Y - LSize_Y - 0.5f), Vec2(StartBoxPos.x + BoxSize_X + 1, StartBoxPos.y + BoxSize_Y - LSize_Y - 0.5f), BoxOutLineColor);
			}
		}
		else if (BoxStyle == 2)
		{

		}
		else if (BoxStyle == 3)
		{
			Draw->DrawBox(StartBoxPos, Vec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y + BoxSize_Y), BoxVisbleCT, BoxSize_X / 5.f);
			if (BoxOutline)
			{
				Draw->DrawBox(Vec2(StartBoxPos.x + 1, StartBoxPos.y + 1), Vec2(StartBoxPos.x + BoxSize_X - 0.9f, StartBoxPos.y + BoxSize_Y - 0.9f), BoxOutLineColor, BoxSize_X / 5.f);
				Draw->DrawBox(Vec2(StartBoxPos.x - 1, StartBoxPos.y - 1), Vec2(StartBoxPos.x + BoxSize_X + 0.9f, StartBoxPos.y + BoxSize_Y + 0.9f), BoxOutLineColor, BoxSize_X / 5.f);
			}
		}
	}

	if (HeadEsp)
	{
		Vec2 HeadCenterPos = Vec2(ScreenImgePos.x + 51, ScreenImgePos.y + 20);

		Draw->DrawRing(HeadCenterPos.x, HeadCenterPos.y, 20, 64, HeadEspColor, 1);
	}

	auto ChangeSidePos = [StartBoxPos](MoveStruct* Curt, int fix) -> Vec2
	{
		switch (Curt->Side)
		{
		case LEFT_S:  return Vec2(StartBoxPos.x - fix - Curt->CorrectPadding.x, StartBoxPos.y);
		case RIGHT_S: return Vec2(StartBoxPos.x + BoxSize_X + Curt->CorrectPadding.x, StartBoxPos.y);
		case TOP_S:   return Vec2(StartBoxPos.x, StartBoxPos.y - fix - Curt->CorrectPadding.y);
		case DOWN_S:  return Vec2(StartBoxPos.x, StartBoxPos.y + BoxSize_Y + Curt->CorrectPadding.y);
		default: return Vec2(0, 0);
		}
	};

	auto ChangeSidePosText = [StartBoxPos](MoveStruct* Curt, Vec2 Size) -> Vec2
	{
		switch (Curt->Side)
		{
		case LEFT_S: return Vec2(StartBoxPos.x - Size.x - Curt->CorrectPadding.x, StartBoxPos.y + Curt->CorrectPadding.y);
		case RIGHT_S:return Vec2(StartBoxPos.x + BoxSize_X + Curt->CorrectPadding.x, StartBoxPos.y + Curt->CorrectPadding.y);
		case TOP_S:  return Vec2(StartBoxPos.x + (BoxSize_X / 2) - (Size.x / 2), StartBoxPos.y - Size.y - Curt->CorrectPadding.y);
		case DOWN_S: return Vec2(StartBoxPos.x + (BoxSize_X / 2) - (Size.x / 2), StartBoxPos.y + BoxSize_Y + Curt->CorrectPadding.y);
		case LEFT_TOP_S:  return Vec2(StartBoxPos.x, StartBoxPos.y - Size.y - Curt->CorrectPadding.y);
		case RIGHT_TOP_S: return Vec2(StartBoxPos.x + BoxSize_X - Size.x, StartBoxPos.y - Size.y - Curt->CorrectPadding.y);
		case LEFT_DOWN_S: return Vec2(StartBoxPos.x, StartBoxPos.y + BoxSize_Y + Curt->CorrectPadding.y);
		case RIGHT_DOWN_S:return Vec2(StartBoxPos.x + BoxSize_X - Size.x, StartBoxPos.y + BoxSize_Y + Curt->CorrectPadding.y);
		case CENTER_LEFT_S:    return Vec2(StartBoxPos.x - Size.x - Curt->CorrectPadding.x, StartBoxPos.y + (BoxSize_Y / 2) + Curt->CorrectPadding.y);
		case CENTER_RIGHT_S:   return Vec2(StartBoxPos.x + BoxSize_X + Curt->CorrectPadding.x, StartBoxPos.y + (BoxSize_Y / 2) + Curt->CorrectPadding.y);
		case DOWN_LEFT_DOWN_S: return Vec2(StartBoxPos.x - Size.x - Curt->CorrectPadding.x, StartBoxPos.y + BoxSize_Y - Size.y + Curt->CorrectPadding.y);
		case DOWN_RIGHT_DOWN_S:return Vec2(StartBoxPos.x + BoxSize_X + Curt->CorrectPadding.x, StartBoxPos.y + BoxSize_Y - Size.y + Curt->CorrectPadding.y);
		default: return Vec2(0, 0);
		}
	};

	Vec2 HPStartPos = ChangeSidePos(MV_Hp, HPTol);
	Vec2 ArmorStartPos = ChangeSidePos(MV_Armor, ATol);

	Vec2 HPTextStartPos = ChangeSidePosText(MV_Hp_Text, MV_Hp_Text->Size);
	Vec2 ArmorTextStartPos = ChangeSidePosText(MV_Armor_Text, MV_Armor_Text->Size);

	Vec2 NameStartPos = ChangeSidePosText(MV_Name, MV_Name->Size);
	Vec2 WeaponStartPos = ChangeSidePosText(MV_Weapon, MV_Weapon->Size);
	Vec2 AmmoStartPos = ChangeSidePosText(MV_Ammo, MV_Ammo->Size);
	Vec2 DistanceStartPos = ChangeSidePosText(MV_Distance, MV_Distance->Size);
	Vec2 MoneyStartPos = ChangeSidePosText(MV_Money, MV_Money->Size);
	Vec2 DefusingStartPos = ChangeSidePosText(MV_Defusing, MV_Defusing->Size);
	Vec2 ScopeStartPos = ChangeSidePosText(MV_Scope, MV_Scope->Size);
	Vec2 FlashedStartPos = ChangeSidePosText(MV_Flashed, MV_Flashed->Size);

	MV_Hp->SizeV = Vec2(HPTol, BoxSize_Y);
	MV_Armor->SizeV = Vec2(ATol, BoxSize_Y);

	MV_Hp->SizeH = Vec2(BoxSize_X, HPTol);
	MV_Armor->SizeH = Vec2(BoxSize_X, ATol);

	auto IsPosTouch = [](MoveStruct& Curt, Vec2 Pos) -> bool
	{
		if ((Pos.x > Curt.Position.x && Pos.x < Curt.Position.x + Curt.Size.x) &&
			(Pos.y > Curt.Position.y && Pos.y < Curt.Position.y + Curt.Size.y))
		{
			return true;
		}
		return false;
	};

	auto CalcPadding = [&](MoveStruct& Curt) -> void
	{
		bool CheckVert = true;
		bool CheckHor = true;

		Curt.CorrectPadding = Vec2(0, 0);

		int TraceLenX = 25;
		int TraceLenY = 30;

		if (Curt.Side == RIGHT_S || Curt.Side == CENTER_RIGHT_S || Curt.Side == DOWN_RIGHT_DOWN_S)
		{
			for (MoveStruct MV : MVItemList)
			{
				if (MV.IsEnabled && Curt.IsEnabled && MV.Id != Curt.Id && (CheckHor || CheckVert) &&
					(MoveStruct::LastActive == Curt.Id || MoveStruct::LastActive == MV.Id ? !IsPosTouch(MV, Curt.Center) : true) &&
					(MV.Side != TOP_S && MV.Side != LEFT_TOP_S && MV.Side != RIGHT_TOP_S))
				{
					if (CheckVert)
					{
						for (int i(-3); i < TraceLenY; i++)
						{
							if (IsPosTouch(MV, Vec2(Curt.Center.x, Curt.Center.y - (Curt.Size.y / 2) - i)))
							{
								Curt.CorrectPadding.y = MV.CorrectPadding.y + MV.Size.y;
								CheckVert = false;
							}
						}
					}

					if (CheckHor && (!MV.IsText ? true : Curt.Center.y > MV.Center.y))
					{
						for (int i(CheckVert ? -3 : 1); i < TraceLenX; i++)
						{
							if (IsPosTouch(MV, Vec2(Curt.Center.x - (Curt.Size.x / 2) - i, Curt.Center.y)))
							{
								Curt.CorrectPadding.x = MV.CorrectPadding.x + MV.Size.x;
								CheckHor = false;
							}
						}
					}
				}
			}
		}

		if (Curt.Side == LEFT_S || Curt.Side == CENTER_LEFT_S || Curt.Side == DOWN_LEFT_DOWN_S)
		{
			for (MoveStruct MV : MVItemList)
			{
				if (MV.IsEnabled && Curt.IsEnabled && MV.Id != Curt.Id && (CheckHor || CheckVert) &&
					(MoveStruct::LastActive == Curt.Id || MoveStruct::LastActive == MV.Id ? !IsPosTouch(MV, Curt.Center) : true) &&
					(MV.Side != TOP_S && MV.Side != LEFT_TOP_S && MV.Side != RIGHT_TOP_S))
				{
					if (CheckVert)
					{
						for (int i(-3); i < TraceLenY; i++)
						{
							if (IsPosTouch(MV, Vec2(Curt.Center.x, Curt.Center.y - (Curt.Size.y / 2) - i)))
							{
								Curt.CorrectPadding.y = MV.CorrectPadding.y + MV.Size.y;
								CheckVert = false;
							}
						}
					}

					if (CheckHor && (!MV.IsText ? true : Curt.Center.y > MV.Center.y))
					{
						for (int i(CheckVert ? -3 : 1); i < TraceLenX; i++)
						{
							if (IsPosTouch(MV, Vec2(Curt.Center.x + (Curt.Size.x / 2) + i, Curt.Center.y)))
							{
								Curt.CorrectPadding.x = MV.CorrectPadding.x + MV.Size.x;
								CheckHor = false;
							}
						}
					}
				}
			}
		}

		if (Curt.Side == TOP_S || Curt.Side == LEFT_TOP_S || Curt.Side == RIGHT_TOP_S)
		{
			for (MoveStruct MV : MVItemList)
			{
				if (MV.IsEnabled && Curt.IsEnabled && MV.Id != Curt.Id && !IsPosTouch(MV, Curt.Center) &&
					(MV.Side != LEFT_S && MV.Side != CENTER_LEFT_S && MV.Side != DOWN_LEFT_DOWN_S && MV.Side != RIGHT_S && MV.Side != CENTER_RIGHT_S && MV.Side != DOWN_RIGHT_DOWN_S))
				{
					for (int i(-3); i < TraceLenY; i++)
					{
						if (IsPosTouch(MV, Vec2(Curt.Center.x, Curt.Center.y + (Curt.Size.y / 2) + i)))
						{
							Curt.CorrectPadding.y = MV.CorrectPadding.y + MV.Size.y;
							return;
						}
					}
				}
			}
		}

		if (Curt.Side == DOWN_S || Curt.Side == LEFT_DOWN_S || Curt.Side == RIGHT_DOWN_S)
		{
			for (MoveStruct MV : MVItemList)
			{
				if (MV.IsEnabled && Curt.IsEnabled && MV.Id != Curt.Id && !IsPosTouch(MV, Curt.Center) &&
					(MV.Side != LEFT_S && MV.Side != CENTER_LEFT_S && MV.Side != DOWN_LEFT_DOWN_S && MV.Side != RIGHT_S && MV.Side != CENTER_RIGHT_S && MV.Side != DOWN_RIGHT_DOWN_S))
				{
					for (int i(-3); i < TraceLenY; i++)
					{
						if (IsPosTouch(MV, Vec2(Curt.Center.x, Curt.Center.y - (Curt.Size.y / 2) - i)))
						{
							Curt.CorrectPadding.y = MV.Size.y;
							return;
						}
					}
				}
			}
		}
	};

	if (Health)
	{
		MV_Hp->Color1 = HealthColor;
		MV_Hp_Text->Color1 = HealthColor;
		MV_Hp->Color2 = HealthColor2;
		MV_Hp->Color3 = HealthColor3;
		if (HealthStyle == 0)
		{
			Move(*MV_Hp, IsMousPress, MousePos, HPStartPos, CLines);
			if (MV_Hp->Side == LEFT_S || MV_Hp->Side == RIGHT_S)
				DrawBarVertical(MV_Hp->Position, MV_Hp->Size, MV_Hp->Color1, MV_Hp->Color2, MV_Hp->Color3, MV_Hp->NumParam1, MV_Hp);
			else
				DrawBarHorizontal(MV_Hp->Position, MV_Hp->Size, MV_Hp->Color1, MV_Hp->Color2, MV_Hp->Color3, MV_Hp->NumParam1, MV_Hp);
		}
		else if (HealthStyle == 1)
		{
			Move(*MV_Hp_Text, IsMousPress, MousePos, HPTextStartPos, CLines, true);
			_DrawText(MV_HEALTH_TEXT, MV_Hp_Text, GP_Render->SzFonts[TextHealthSize]);
		}
	}

	if (Armor)
	{
		MV_Armor->Color1 = ArmorColor;
		MV_Armor_Text->Color1 = ArmorColor;
		MV_Armor->Color2 = ArmorColor2;
		MV_Armor->Color3 = ArmorColor3;

		if (ArmorStyle == 0)
		{
			Move(*MV_Armor, IsMousPress, MousePos, ArmorStartPos, CLines);
			if (MV_Armor->Side == LEFT_S || MV_Armor->Side == RIGHT_S)
				DrawBarVertical(MV_Armor->Position, MV_Armor->Size, MV_Armor->Color1, MV_Armor->Color2, MV_Armor->Color3, MV_Armor->NumParam1, MV_Armor);
			else
				DrawBarHorizontal(MV_Armor->Position, MV_Armor->Size, MV_Armor->Color1, MV_Armor->Color2, MV_Armor->Color3, MV_Armor->NumParam1, MV_Armor);
		}
		else if (ArmorStyle == 1)
		{
			Move(*MV_Armor_Text, IsMousPress, MousePos, ArmorTextStartPos, CLines, true);
			_DrawText(MV_ARMOR_TEXT, MV_Armor_Text, GP_Render->SzFonts[TextArmorSize]);
		}
	}

	if (Name)
	{
		Move(*MV_Name, IsMousPress, MousePos, NameStartPos, CLines, true);

		if (NameBackground)
		{
			Vec2 NSize = MV_Name->Size;
			Vec2 NPos = MV_Name->Position;

			NSize.x += 30;
			NPos.x -= 15;
			NPos.y += 1.5f;

			float max = NSize.x / 3.f;

			for (int i(0); i < max; i++)
				Draw->DrawFilledBoxEx(Vec2(NPos.x + i, NPos.y),
					Vec2(NPos.x + i + 1, NPos.y + NSize.y),
					color_t(NameColorBack.r(), NameColorBack.g(), NameColorBack.b(), (((float)i / max) * 255)));

			Draw->DrawFilledBoxEx(Vec2(NPos.x + max, NPos.y),
				Vec2(NPos.x + (max * 2), NPos.y + NSize.y),
				color_t(NameColorBack.r(), NameColorBack.g(), NameColorBack.b(), 255));

			for (int i(0); i < max; i++)
				Draw->DrawFilledBoxEx(Vec2(NPos.x + i + (max * 2), NPos.y),
					Vec2(NPos.x + i + 1 + (max * 2), NPos.y + NSize.y),
					color_t(NameColorBack.r(), NameColorBack.g(), NameColorBack.b(), (255 - (((float)i / max) * 255)) ));
		}
		MV_Name->Color1 = NameColor;

		_DrawText(MV_NAME_TEXT, MV_Name, GP_Render->SzFonts[TextNameSize]);
	}

	if (Weapon)
	{
		CFont *wep_fnt = nullptr;

		if (WeaponStyle == 0)
			wep_fnt = GP_Render->SzFonts[TextWeaponSize];
		else if (WeaponStyle == 1)
			wep_fnt = GP_Render->SzFontsIcon[TextWeaponSize];

		Move(*MV_Weapon, IsMousPress, MousePos, WeaponStartPos, CLines, true);

		MV_Weapon->Color1 = WeaponColor;

		if (WeaponStyle == 0)
			_DrawText(MV_WEAPON_TEXT, MV_Weapon, wep_fnt);
		else if (WeaponStyle == 1)
		{
			_DrawText(MV_WEAPON_TEXT_ICO, MV_Weapon, wep_fnt);
		}
	}

	if (Ammo)
	{
		Move(*MV_Ammo, IsMousPress, MousePos, AmmoStartPos, CLines, true);
		MV_Ammo->Color1 = AmmoColor;
		_DrawText(MV_AMMO_TEXT, MV_Ammo, GP_Render->SzFonts[TextAmmoSize]);
	}

	if (Distance)
	{
		Move(*MV_Distance, IsMousPress, MousePos, DistanceStartPos, CLines, true);
		MV_Distance->Color1 = DistanceColor;
		_DrawText(MV_DISTANCE_TEXT, MV_Distance, GP_Render->SzFonts[TextDistanceSize]);
	}

	if (Money)
	{
		Move(*MV_Money, IsMousPress, MousePos, MoneyStartPos, CLines, true);
		MV_Money->Color1 = MoneyColor;
		_DrawText(MV_MONEY_TEXT, MV_Money, GP_Render->SzFonts[TextMoneySize]);
	}

	if (Defusing)
	{
		Move(*MV_Defusing, IsMousPress, MousePos, DefusingStartPos, CLines, true);
		MV_Defusing->Color1 = DefusingColor;
		_DrawText(MV_DEFUSING_TEXT, MV_Defusing, GP_Render->SzFonts[TextDefusingSize]);
	}

	if (Scope)
	{
		Move(*MV_Scope, IsMousPress, MousePos, ScopeStartPos, CLines, true);
		MV_Scope->Color1 = ScopeColor;
		_DrawText(MV_SCOPE_TEXT, MV_Scope, GP_Render->SzFonts[TextScopedSize]);
	}

	if (Flashing)
	{
		Move(*MV_Flashed, IsMousPress, MousePos, FlashedStartPos, CLines, true);
		MV_Flashed->Color1 = FlashingColor;
		_DrawText(MV_FLASHED_TEXT, MV_Flashed, GP_Render->SzFonts[TextFlashedSize]);
	}

	if (MoveStruct::LastActive != -1 && (MITypes)MoveStruct::LastActive <= MI_MAX && MoveStruct::ActiveIdx != -1)
	{
		MoveStruct *Selected = &MVItemList[MoveStruct::LastActive];

		color_t SelColor = color_t(0, 255, 255, 102);
		Vec2 SP = Selected->Position;

		int Correct = 0;

		if (!Selected->IsText)
			Correct = 1;

		Draw->DrawLine(Vec2(int(StartPosScreen.x), int(SP.y - Correct)), Vec2(int(StartPosScreen.x + VIS_PREW_ZONE_X), int(SP.y - Correct)), SelColor);
		Draw->DrawLine(Vec2(int(SP.x - Correct), int(StartPosScreen.y)), Vec2(int(SP.x - Correct), int(StartPosScreen.y + VIS_PREW_ZONE_Y)), SelColor);
		Draw->DrawLine(Vec2(int(StartPosScreen.x), int(SP.y + Selected->Size.y + Correct)), Vec2(int(StartPosScreen.x + VIS_PREW_ZONE_X), SP.y + int(Selected->Size.y + Correct)), SelColor);
		Draw->DrawLine(Vec2(int(SP.x + Selected->Size.x + Correct), int(StartPosScreen.y)), Vec2(int(SP.x + Selected->Size.x + Correct), int(StartPosScreen.y + VIS_PREW_ZONE_Y)), SelColor);
	}

	auto CalcVisOffset = [StartBoxPos](MoveStruct& Curt, bool IsText = false)
	{
		float DisttX = 0.000000001;
		float DisttY = 0.000000001;

		if (!IsText)
		{
			if (Curt.Side == LEFT_S)
			{
				DisttX = ((StartBoxPos.x - Curt.CorrectPadding.x) - (Curt.Position.x + Curt.Size.x));
				DisttY = (StartBoxPos.y - Curt.Position.y);
			}
			else if (Curt.Side == RIGHT_S)
			{
				DisttX = ((StartBoxPos.x + Curt.CorrectPadding.x) - Curt.Position.x);
				DisttY = (StartBoxPos.y - Curt.Position.y);
			}
			else if (Curt.Side == TOP_S)
			{
				DisttX = (StartBoxPos.x - Curt.Position.x);
				DisttY = ((StartBoxPos.y - Curt.CorrectPadding.y) - Curt.Position.y - Curt.Size.y);
			}
			else if (Curt.Side == DOWN_S)
			{
				DisttX = (StartBoxPos.x - Curt.Position.x);
				DisttY = ((StartBoxPos.y + BoxSize_Y + Curt.CorrectPadding.y) - Curt.Position.y);
			}
		}
		else
		{
			if (Curt.Side == LEFT_S)
			{
				DisttX = ((StartBoxPos.x - Curt.CorrectPadding.x - Curt.Size.x) - (Curt.Position.x));
				DisttY = ((StartBoxPos.y + Curt.CorrectPadding.y) - Curt.Position.y);
			}
			else if (Curt.Side == RIGHT_S)
			{
				DisttX = ((StartBoxPos.x + BoxSize_X + Curt.CorrectPadding.x) - Curt.Position.x);
				DisttY = ((StartBoxPos.y + Curt.CorrectPadding.y) - Curt.Position.y);
			}
			else if (Curt.Side == TOP_S)
			{
				DisttX = (StartBoxPos.x - Curt.Position.x + (BoxSize_X / 2) - (Curt.Size.x / 2));
				DisttY = ((StartBoxPos.y - Curt.CorrectPadding.y) - Curt.Position.y - Curt.Size.y);
			}
			else if (Curt.Side == DOWN_S)
			{
				DisttX = (StartBoxPos.x - Curt.Position.x + (BoxSize_X / 2) - (Curt.Size.x / 2));
				DisttY = ((StartBoxPos.y + BoxSize_Y + Curt.CorrectPadding.y) - Curt.Position.y);
			}
		}

		if (Curt.Side == LEFT_TOP_S)
		{
			DisttX = (StartBoxPos.x - Curt.Position.x);
			DisttY = ((StartBoxPos.y - Curt.Size.y - Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == RIGHT_TOP_S)
		{
			DisttX = ((StartBoxPos.x + BoxSize_X) - (Curt.Position.x + Curt.Size.x));
			DisttY = ((StartBoxPos.y - Curt.Size.y - Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == LEFT_DOWN_S)
		{
			DisttX = (StartBoxPos.x - Curt.Position.x);
			DisttY = ((StartBoxPos.y + BoxSize_Y + Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == RIGHT_DOWN_S)
		{
			DisttX = ((StartBoxPos.x + BoxSize_X - Curt.Size.x) - Curt.Position.x);
			DisttY = ((StartBoxPos.y + BoxSize_Y + Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == CENTER_LEFT_S)
		{
			DisttX = ((StartBoxPos.x - Curt.CorrectPadding.x - Curt.Size.x) - (Curt.Position.x));
			DisttY = ((StartBoxPos.y + (BoxSize_Y / 2) + Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == CENTER_RIGHT_S)
		{
			DisttX = ((StartBoxPos.x + BoxSize_X + Curt.CorrectPadding.y) - Curt.Position.x);
			DisttY = ((StartBoxPos.y + (BoxSize_Y / 2) + Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == DOWN_LEFT_DOWN_S)
		{
			DisttX = ((StartBoxPos.x - Curt.CorrectPadding.x - Curt.Size.x) - (Curt.Position.x));
			DisttY = ((StartBoxPos.y + BoxSize_Y - Curt.Size.y + Curt.CorrectPadding.y) - (Curt.Position.y));
		}
		else if (Curt.Side == DOWN_RIGHT_DOWN_S)
		{
			DisttX = ((StartBoxPos.x + BoxSize_X + Curt.CorrectPadding.x) - Curt.Position.x);
			DisttY = ((StartBoxPos.y + BoxSize_Y - Curt.Size.y + Curt.CorrectPadding.y) - (Curt.Position.y));
		}

		if (DisttX == 0)
			DisttX = 0.000000001;

		if (DisttY == 0)
			DisttY = 0.000000001;

		Curt.CorrectOfst.x = (BoxSize_X / DisttX);
		Curt.CorrectOfst.y = (BoxSize_Y / DisttY);
	};

	CalcPadding(*MV_Hp);
	CalcPadding(*MV_Armor);

	CalcPadding(*MV_Hp_Text);
	CalcPadding(*MV_Armor_Text);

	CalcPadding(*MV_Name);
	CalcPadding(*MV_Weapon);
	CalcPadding(*MV_Ammo);
	CalcPadding(*MV_Distance);
	CalcPadding(*MV_Money);
	CalcPadding(*MV_Scope);
	CalcPadding(*MV_Flashed);
	CalcPadding(*MV_Defusing);

	CalcVisOffset(*MV_Hp);
	CalcVisOffset(*MV_Armor);

	CalcVisOffset(*MV_Hp_Text, true);
	CalcVisOffset(*MV_Armor_Text, true);

	CalcVisOffset(*MV_Name, true);
	CalcVisOffset(*MV_Weapon, true);
	CalcVisOffset(*MV_Ammo, true);
	CalcVisOffset(*MV_Distance, true);
	CalcVisOffset(*MV_Money, true);
	CalcVisOffset(*MV_Scope, true);
	CalcVisOffset(*MV_Flashed, true);
	CalcVisOffset(*MV_Defusing, true);

	for (int i(0); (MITypes)i < MI_MAX; i++)
		MVItemList[i].ZonePos = Vec2(MVItemList[i].Position.x - StartPosScreen.x, MVItemList[i].Position.y - StartPosScreen.y);

	X1Gui().SetCursorPos(Vec2(StartPos.x, StartPos.y + VIS_PREW_ZONE_Y + 3) - X1Gui().GetThis()->Info.Pos,0);
}


void CEsp::InitVisuals()
{
	auto LInitVisuals = [&]()->void
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

		if (!MoveStruct::InitMItems)
		{
			MoveStruct::InitMItems = true;
			MoveStruct::LastActive = MI_MAX;

			/*=====================================================================================*/
			MV_Hp_Text->StartPos = MV_Hp_Text->Position;
			MV_Hp_Text->Id = MI_HEALTH_TEXT;
			MV_Hp_Text->Side = LEFT_S;
			MV_Hp_Text->TextSize = 17;
			MV_Hp_Text->Offset = Vec2(5, 0);
			MV_Hp_Text->IsText = true;
			MV_Hp_Text->Name = ("MV_HP_Text");
			MV_Hp_Text->Color1[0] = 1.0f;
			MV_Hp_Text->Color1[1] = 1.0f;
			MV_Hp_Text->Color1[2] = 1.0f;

			/*=====================================================================================*/
			MV_Armor_Text->StartPos = MV_Armor_Text->Position;
			MV_Armor_Text->Id = MI_ARMOR_TEXT;
			MV_Armor_Text->Side = LEFT_S;
			MV_Armor_Text->TextSize = 17;
			MV_Armor_Text->Offset = Vec2(5, -10);
			MV_Armor_Text->IsText = true;
			MV_Armor_Text->Name = ("MV_Arm_Text");
			MV_Armor_Text->Color1[0] = 1.0f;
			MV_Armor_Text->Color1[1] = 1.0f;
			MV_Armor_Text->Color1[2] = 1.0f;

			/*=====================================================================================*/
			MV_Hp->StartPos = MV_Hp->Position;
			MV_Hp->Id = MI_HEALTH_BAR;
			MV_Hp->Side = LEFT_S;
			MV_Hp->Offset = Vec2(5, 0);

			MV_Hp->Color1[0] = 0.1f;
			MV_Hp->Color1[1] = 1.0f;
			MV_Hp->Color1[2] = 0.1f;

			MV_Hp->Color2[0] = 1.0f;
			MV_Hp->Color2[1] = 0.0f;
			MV_Hp->Color2[2] = 0.0f;

			MV_Hp->Color3[0] = 0.0f;
			MV_Hp->Color3[1] = 0.0f;
			MV_Hp->Color3[2] = 0.0f;

			MV_Hp->NumParam1 = 4;
			MV_Hp->NumParam2 = 5;
			MV_Hp->Name = ("MV_HPBar");
			MV_Hp->boolParam1 = true;
			MV_Hp->boolParam2 = true;
			MV_Hp->boolParam3 = false;

			/*=====================================================================================*/
			MV_Armor->StartPos = MV_Armor->Position;
			MV_Armor->Id = MI_ARMOR_BAR;
			MV_Armor->Side = RIGHT_S;
			MV_Armor->Offset = Vec2(-5, 0);

			MV_Armor->Color1[0] = 1.0f;
			MV_Armor->Color1[1] = 1.0f;
			MV_Armor->Color1[2] = 1.0f;

			MV_Armor->Color2[0] = 1.0f;
			MV_Armor->Color2[1] = 0.0f;
			MV_Armor->Color2[2] = 0.0f;

			MV_Armor->Color3[0] = 0.0f;
			MV_Armor->Color3[1] = 0.0f;
			MV_Armor->Color3[2] = 0.0f;

			MV_Armor->NumParam1 = 4;
			MV_Armor->NumParam2 = 5;
			MV_Armor->Name = ("MV_ArmorBar");
			MV_Armor->boolParam1 = true;
			MV_Armor->boolParam2 = true;
			MV_Armor->boolParam3 = false;

			/*=====================================================================================*/
			MV_Name->StartPos = MV_Name->Position;
			MV_Name->Id = MI_NAME_TEXT;
			MV_Name->Side = TOP_S;
			MV_Name->TextSize = 17;
			MV_Name->Offset = Vec2(0, 4);
			MV_Name->IsText = true;
			MV_Name->Name = ("MV_Name");
			MV_Name->boolParam1 = false;
			MV_Name->Color2[0] = 0.09f;
			MV_Name->Color2[1] = 0.09f;
			MV_Name->Color2[2] = 0.09f;

			/*=====================================================================================*/
			MV_Weapon->StartPos = MV_Weapon->Position;
			MV_Weapon->Id = MI_WEAPON_TEXT;
			MV_Weapon->Side = DOWN_S;
			MV_Weapon->TextSize = 17;
			MV_Weapon->Offset = Vec2(0, -1);
			MV_Weapon->IsText = true;
			MV_Weapon->Name = ("MV_Weapon");

			/*=====================================================================================*/
			MV_Ammo->StartPos = MV_Ammo->Position;
			MV_Ammo->Id = MI_AMMO_TEXT;
			MV_Ammo->Side = DOWN_S;
			MV_Ammo->TextSize = 17;
			MV_Ammo->Offset = Vec2(0, -10);
			MV_Ammo->IsText = true;
			MV_Ammo->Name = ("MV_Ammo");
			MV_Ammo->Color1[0] = 1.0f;
			MV_Ammo->Color1[1] = 1.0f;
			MV_Ammo->Color1[2] = 1.0f;

			/*=====================================================================================*/
			MV_Distance->StartPos = MV_Distance->Position;
			MV_Distance->Id = MI_DISTANCE_TEXT;
			MV_Distance->Side = DOWN_RIGHT_DOWN_S;
			MV_Distance->TextSize = 17;
			MV_Distance->Offset = Vec2(-7, 0);
			MV_Distance->IsText = true;
			MV_Distance->Name = ("MV_Distance");

			/*=====================================================================================*/
			MV_Money->StartPos = MV_Money->Position;
			MV_Money->Id = MI_MONEY_TEXT;
			MV_Money->Side = RIGHT_S;
			MV_Money->TextSize = 17;
			MV_Money->Color1[0] = 1.0f;
			MV_Money->Color1[1] = 1.0f;
			MV_Money->Color1[2] = 1.0f;
			MV_Money->Offset = Vec2(-7, 0);
			MV_Money->IsText = true;
			MV_Money->Name = ("MV_Money");

			/*=====================================================================================*/
			MV_Scope->StartPos = MV_Scope->Position;
			MV_Scope->Id = MI_SCOPE_TEXT;
			MV_Scope->Side = RIGHT_S;
			MV_Scope->TextSize = 17;
			MV_Scope->Color1[0] = 1.0f;
			MV_Scope->Color1[1] = 1.0f;
			MV_Scope->Color1[2] = 1.0f;
			MV_Scope->Offset = Vec2(-7, -7);
			MV_Scope->IsText = true;
			MV_Scope->Name = ("MV_Scope");

			/*=====================================================================================*/
			MV_Flashed->StartPos = MV_Flashed->Position;
			MV_Flashed->Id = MI_FLASHED_TEXT;
			MV_Flashed->Side = CENTER_RIGHT_S;
			MV_Flashed->TextSize = 17;
			MV_Flashed->Color1[0] = 1.0f;
			MV_Flashed->Color1[1] = 1.0f;
			MV_Flashed->Color1[2] = 1.0f;
			MV_Flashed->Offset = Vec2(-7, 0);
			MV_Flashed->IsText = true;
			MV_Flashed->Name = ("MV_Flashed");

			/*=====================================================================================*/
			MV_Defusing->StartPos = MV_Defusing->Position;
			MV_Defusing->Id = MI_DEFUSING_TEXT;
			MV_Defusing->Side = CENTER_RIGHT_S;
			MV_Defusing->TextSize = 17;
			MV_Defusing->Color1[0] = 1.0f;
			MV_Defusing->Color1[1] = 1.0f;
			MV_Defusing->Color1[2] = 1.0f;
			MV_Defusing->Offset = Vec2(-7, -7);
			MV_Defusing->IsText = true;
			MV_Defusing->Name = ("MV_Defusing");
		}
	};
	LInitVisuals();
}
