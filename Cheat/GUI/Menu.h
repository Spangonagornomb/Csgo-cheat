#include  "Gui.h"
#include "../Hacks/Setup.h"
#include "../Hacks/Settings.h"

#define WINDOW1_SIZE_X 160
#define WINDOW1_SIZE_Y 325

#define WINDOW2_SIZE_X 470
#define WINDOW2_SIZE_Y 665

#define LABEL_WIN_SPEED_DELTA  310.f
#define LABEL_WIN_SPEED  0.006079f

#define MAIN_WIN_SPEED_DELTA  330.f
#define MAIN_WIN_SPEED  0.006239f

#define PREW_WIN_SPEED_DELTA  360.f
#define PREW_WIN_SPEED  0.006279f

class CMenu
{
private:
	int SelectedTab = -1;

	void SettingsMenu()
	{
		static int SelectedSettings = 0;

		VectorEx<const char*>tabs_eng = { ("Configurations"), ("Settings") };
		VectorEx<const char*>tabs_rus = { (u8"Конфиг"),(u8"Меню") };

		TabsLabels(SelectedSettings, tabs_eng, tabs_rus, Vec2(X1Gui().GetCurWindowSize().x - (X1Gui().GetStyle().wndPadding.x * 2), 0), false);

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		if (SelectedSettings == 0)
		{
			CGSettings::G().Menu();
			MainSettings().SetMenuColors();
		}
		else if (SelectedSettings == 1)
		{
			HotsKey("Menu key", u8"Кнопка открытия меню", MainSettings().MenuButton, u8"Задает кнопку открытия меню");
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();
			DCheckBox("Menu animation", u8"Анимация меню", X1Gui().GetStyle().AnimationEnable, "");
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();
			X1Gui().CheckBox(u8"Русский язык", &MainSettings().Russian);
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();
			DCheckBox(u8"3D menu", u8"3Д Меню", GP_Enable3DMenu->Enable, u8"");
			if (GP_Enable3DMenu->Enable)
			{
				X1Gui().SameLine();
				X1Gui().PushItemWidth(200);
				HotsKey("Button##3dm", u8"Кнопка##3дм", GP_Enable3DMenu->Button, u8"");
			}
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();
			DColorEdit("Background color", u8"Цвет фона", MainSettings().BackgroundColor);
			DColorEdit("Title color", u8"Цвет фона заголовка", MainSettings().TitleColor);
			DColorEdit("Text color", u8"Цвет текста", MainSettings().TextColor);
			DColorEdit("Frame color", u8"Цвет фрейма", MainSettings().FrameColor);
			DColorEdit("Button color", u8"Цвет кнопок", MainSettings().ButtonColor);
			X1Gui().SameLine();
			DColorEdit("Disabled button color", u8"Цвет выключенной кнопки", MainSettings().DisableButtonColor);
			DColorEdit("Line color", u8"Цвет линий", MainSettings().LineColor);
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();
			if (X1Gui().Button(!MainSettings().Russian ? ("Reset colors") : (u8"Сбросить цвета")))
			{
				MainSettings().BackgroundColor = Color(18, 18, 22, 255);
				MainSettings().TitleColor = Color(112, 112, 116, 255);
				MainSettings().TextColor = Color(255, 255, 255, 255);
				MainSettings().FrameColor = Color(34, 35, 37, 255);
				MainSettings().ButtonColor = Color(128, 135, 140, 255);
				MainSettings().DisableButtonColor = Color(225, 0, 0, 255);
				MainSettings().LineColor = Color(120, 121, 123, 255);
			}
			MainSettings().SetMenuColors();
		}
	}

public:

	int ChangerMode = 0;

	void Changers()
	{
		vector<string> Kostil_eng = { lolc("Skins"), lolc("Inventory") };
		vector<string> Kostil_rus = { lolc(u8"Скины"),lolc(u8"Инвентарь") };
		VectorEx<const char*> ChaArrEng = { Kostil_eng[0].c_str(),Kostil_eng[1].c_str() };
		VectorEx<const char*> ChaArrRus = { Kostil_rus[0].c_str(),Kostil_rus[1].c_str() };

		TabsLabels(ChangerMode, ChaArrEng, ChaArrRus, Vec2(328, 20), false);

		X1Gui().SameLine(331);

		if (ChangerMode == 0)
		{
			if (X1Gui().Button(GP_Skins->ShowSkinPreview ? ("Preview <<<") : ("Preview >>>"), Vec2(129, 20)))
				GP_Skins->ShowSkinPreview = !GP_Skins->ShowSkinPreview;
		}
		else if (ChangerMode == 1)
		{
			if (X1Gui().Button(GP_Inventory->ShowInventoryList ? ("Inv. list <<<") : ("Inv. list >>>"), Vec2(129, 20)))
				GP_Inventory->ShowInventoryList = !GP_Inventory->ShowInventoryList;
		}

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		if (ChangerMode == 0)
		{
			DCheckBox("Enabled", u8"Включение скинов", GP_Skins->SkinsEnable, u8"");
		}
		else if (ChangerMode == 1)
		{
			DCheckBox("Inventory synchronization", u8"Синхронизация инвентаря", GP_Inventory->SkinsSyncEnable, u8"");
		}

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		if (ChangerMode == 0)
		{
			GP_Skins->Menu();
		}
		else if (ChangerMode == 1)
		{
			GP_Inventory->Menu();
		}
	}

	void SelectWindow()
	{
		string name = MainSettings().UserName;
		string end_data = MainSettings().EndDate;
		string days = MainSettings().Days;
#ifdef MULTIHACK_VERSION
		VectorEx<const char*>tabs_eng = { "Aimbot", "Visualizations", "Miscellaneous", "Radar", "Grenade helper", "Changers", "Configuration" };
		VectorEx<const char*>tabs_rus = { u8"Аимбот", u8"Визуалы",u8"Дополнительно",  u8"Радар", "Grenade Helper", u8"Ченжеры", u8"Настройки" };
#endif 
		TabsLabels(SelectedTab, tabs_eng, tabs_rus, Vec2(X1Gui().GetCurWindowSize().x - (X1Gui().GetStyle().wndPadding.x * 2), 220), true);
	}

	void DrawCurTab(int tab)
	{
		switch (tab)
		{
#ifdef MULTIHACK_VERSION
		case 0: if (GP_LegitAim) GP_LegitAim->Menu(); break;
		case 1: if (GP_Esp) GP_Esp->Menu(); break;
		case 2: if (GP_Misc) GP_Misc->Menu(); break;
		case 3: if (GP_Radar) GP_Radar->Menu(); break;
		case 4: if (GP_GHelper) { GP_GHelper->Menu(); } break;
		case 5: Changers(); break;
		case 6: SettingsMenu(); break;
#endif
		default: break;
		}
	}

	CAnimController AnimLabelsWin;
	CAnimController AnimMainWin;
	CAnimController AnimPrewWin;

	Vec2 LabelWinSize;
	Vec2 MainWinSize;
	Vec2 PrewWinSize;

	bool LabelWinEnd = false;
	bool MainWinEnd = false;
	bool PrewWinEnd = false;

	DWORD next_time_close = 0;

	void Draw(CFont *MenuFont)
	{
		auto LDraw = [&]()->void
		{
			auto ResetAnim = [&](bool reset)->void
			{
				AnimLabelsWin.Reset();
				AnimMainWin.Reset();
				AnimPrewWin.Reset();
				LabelWinEnd = reset;
				MainWinEnd = reset;
				PrewWinEnd = reset;
			};

			X1Gui().SetFont(MenuFont);

			bool AnimEnable = X1Gui().GetStyle().AnimationEnable;
			bool GuiEnable = CGlobal::IsGuiVisble;

			if (AnimEnable)
			{
				if (GuiEnable)
				{
					AnimLabelsWin.Update(true, LABEL_WIN_SPEED, LABEL_WIN_SPEED_DELTA);
					LabelWinSize = Vec2(WINDOW1_SIZE_X, 25 + AnimLabelsWin.Get(WINDOW1_SIZE_Y - 25));
					LabelWinEnd = AnimLabelsWin.IsEnd();
				}
				else
				{
					LabelWinSize = Vec2(WINDOW1_SIZE_X, 25);
					ResetAnim(false);
				}
			}
			else
			{
				LabelWinSize = Vec2(WINDOW1_SIZE_X, WINDOW1_SIZE_Y);
				ResetAnim(true);
			}

			if (GuiEnable)
			{
				X1Gui().GetStyle().ScrollEnable = LabelWinEnd && MainWinEnd;

				MainSettings().HintMsg = "";

				Vec2 oldWinPadding = X1Gui().GetStyle().wndPadding;
				X1Gui().GetStyle().wndPadding = Vec2(2, 1);

				if (X1Gui().Begin(CHEAT_HEAD, GWF_Window, LabelWinSize, Vec2(450, 50)))
				{
					SelectWindow();

					next_time_close = GetTickCount64() + 15000;
				}
				X1Gui().End();

				X1Gui().GetStyle().wndPadding = oldWinPadding;

				if (LabelWinEnd && SelectedTab != -1)
				{
					if (AnimEnable)
					{
						AnimMainWin.Update(true, MAIN_WIN_SPEED, MAIN_WIN_SPEED_DELTA);
						MainWinSize = Vec2(WINDOW2_SIZE_X, AnimMainWin.Get(WINDOW2_SIZE_Y));
						MainWinEnd = AnimMainWin.IsEnd();
					}
					else
					{
						MainWinSize = Vec2(WINDOW2_SIZE_X, WINDOW2_SIZE_Y);
					}

					Vec2 LabelWinPos = X1Gui().GetPrevWindowPos();
					X1Gui().SetNextWindowPos(Vec2(LabelWinPos.x + WINDOW1_SIZE_X + 4, LabelWinPos.y));

					if (X1Gui().Begin(("Main"), GWF_NoTitleBar, MainWinSize, Vec2(600, 400)))
					{
						DrawCurTab(SelectedTab);
					}
					X1Gui().End();

					if (MainWinEnd)
					{
						Vec2 MainWinPos = X1Gui().GetPrevWindowPos();
#ifdef MULTIHACK_VERSION
#define VIS_TAB 1
#define LAIM_TAB 0
#define SKIN_TAB 5
#endif 

						bool IsVisualsTab = SelectedTab == VIS_TAB && GP_Esp->ShowPreview;
						bool IsLegitAimTab = SelectedTab == LAIM_TAB && GP_LegitAim->ShowWeaponList;
						bool IsSkinsTab = SelectedTab == SKIN_TAB && ChangerMode == 0 && GP_Skins->ShowSkinPreview;
						bool IsInventTab = SelectedTab == SKIN_TAB && ChangerMode == 1 && GP_Inventory->ShowInventoryList;

						static Vec2 TargetSize = Vec2(100, 100);
						static Vec2 CurSize = Vec2(100, 100);

						if (IsVisualsTab)
							TargetSize = Vec2(350 + (X1Gui().GetStyle().wndPadding.x * 2) + 1, WINDOW2_SIZE_Y);
						else if (IsSkinsTab)
							TargetSize = Vec2(320, 248);
						else if (IsInventTab)
							TargetSize = Vec2(365, WINDOW2_SIZE_Y);
						else if (IsLegitAimTab)
						{
							if (GP_LegitAim->WeaponCustomTypes == 0)
								TargetSize = Vec2(160, WINDOW2_SIZE_Y);
							else if (GP_LegitAim->WeaponCustomTypes == 1)
								TargetSize = Vec2(160, 135);
							else if (GP_LegitAim->WeaponCustomTypes == 2)
								TargetSize = Vec2(463, WINDOW2_SIZE_Y);
						}


						static bool ShowPrewWin = false;
						bool EnablePrewWin = IsVisualsTab || IsLegitAimTab || IsSkinsTab || IsInventTab;

						if (AnimEnable)
						{
							AnimPrewWin.Update(EnablePrewWin, PREW_WIN_SPEED, PREW_WIN_SPEED_DELTA);
							CurSize.x = AnimPrewWin.Get(TargetSize.x);
							CurSize.y = TargetSize.y;
							ShowPrewWin = !AnimPrewWin.IsStart();
						}
						else
						{
							CurSize = TargetSize;
							ShowPrewWin = EnablePrewWin;
						}

						if (ShowPrewWin)
						{
							if (AnimEnable)
								X1Gui().SetNextWindowPos(Vec2(MainWinPos.x + WINDOW2_SIZE_X + AnimPrewWin.Get(4), MainWinPos.y));
							else
								X1Gui().SetNextWindowPos(Vec2(MainWinPos.x + WINDOW2_SIZE_X + 4, MainWinPos.y));

							if (X1Gui().Begin(("prew"), GWF_NoTitleBar, CurSize, Vec2(1, 1)))
							{
								if (SelectedTab == LAIM_TAB && GP_LegitAim->WeaponCustomTypes != 3 && GP_LegitAim->ShowWeaponList)
									if (GP_LegitAim)
										GP_LegitAim->SubsectionsMenu();

								if (SelectedTab == VIS_TAB)
									if (GP_Esp)
										GP_Esp->VisualPreview();

								if (SelectedTab == SKIN_TAB)
								{
									if (ChangerMode == 0)
										GP_Skins->Preview();
									else if (ChangerMode == 1)
										GP_Inventory->InvListMenu();
								}
							}
							X1Gui().End();
						}

						if (MainSettings().HintMsg != "")
						{
							int HintSizeY = X1Gui().CalcTextSize(MainSettings().HintMsg).y + 9;
							X1Gui().SetNextWindowPos(Vec2(MainWinPos.x, MainWinPos.y - HintSizeY - 4));
							if (X1Gui().Begin(("Hint"), GWF_NoTitleBar, Vec2(WINDOW2_SIZE_X, HintSizeY), Vec2(1, 1)))
							{

							}
							X1Gui().End();
						}
					}
				}
			}
			else
			{
				if (SelectedTab != -1 && next_time_close <= GetTickCount64())
					SelectedTab = -1;
			}
		};
		LDraw();
	}
};