#include "LegitAim.h"
#include "../../GUI/Gui.h"


#define GetWeap(a) ((a < 0) ? 0 : (a >= Weapons.size() ? Weapons.size()-1 : a))
void CLegitAim::Menu()
{
	if (SelectedWeapon == -1)
		SelectedWeapon = 0;

	if (SelectedWeapon >= (int)Weapons.size())
		SelectedWeapon = 0;

	DCheckBox("Enabled", u8"���������", Enable, u8"�������� ���");

	VectorEx<const char*> CustomTypesEng = { lolc("One weapon"),lolc("Standard subsections") , lolc("Custom subsections"), lolc("All weapons") };
	VectorEx<const char*> CustomTypesRus = { lolc(u8"���� ������"), lolc(u8"����������� ����������"), lolc(u8"��������� ����������"),lolc(u8"��� ������") };

	static int old_type = WeaponCustomTypes;
	X1Gui().SameLine(126);
	X1Gui().PushItemWidth(194);
	DComboBox("##pods", "##pods", WeaponCustomTypes, CustomTypesEng, CustomTypesRus, u8"��������� ��");

	if (old_type != WeaponCustomTypes)
		SetSelectedWeapon(false);

	old_type = WeaponCustomTypes;

	string A_WeaponList_L = ("Weapon list");
	string A_Subsections_L = ("Subsections");
	if (false)
	{
		A_WeaponList_L = (u8"������ ������");
		A_Subsections_L = (u8"����������");
	}

	if (WeaponCustomTypes == 0)
	{
		if (false)
			X1Gui().SameLine(331);
		else
			X1Gui().SameLine(351);

		if (X1Gui().Button(ShowWeaponList ? (A_WeaponList_L + (" <<<")).c_str() : (A_WeaponList_L + (" >>>")).c_str()))
			ShowWeaponList = !ShowWeaponList;
	}
	else if (WeaponCustomTypes == 1 || WeaponCustomTypes == 2)
	{
		if (false)
			X1Gui().SameLine(349);
		else
			X1Gui().SameLine(355);

		if (X1Gui().Button(ShowWeaponList ? (A_Subsections_L + (" <<<")).c_str() : (A_Subsections_L + (" >>>")).c_str()))
			ShowWeaponList = !ShowWeaponList;
	}
	else if (WeaponCustomTypes == 3)
	{
		ShowWeaponList = false;
	}

	X1Gui().Spacing();

	if (X1Gui().BeginChild(("MainChild"), Vec2(0, 173), true))
	{
		VectorEx<const char*> AimPEng = { lolc("Page 1"), lolc("Page 2") };
		VectorEx<const char*> AimPRus = { lolc(u8"��������1"),lolc(u8"��������2") };
		static int SubtabAimMiscSelected = 0;

		TabsLabels(SubtabAimMiscSelected, AimPEng, AimPRus, Vec2(X1Gui().GetCurWindowSize().x - (X1Gui().GetStyle().wndPadding.x * 2), 0), false);
		if (SubtabAimMiscSelected == 0)
		{
			X1Gui().Spacing();
			DCheckBox("Aim at teammates", u8"�� ����", Deathmatch, u8"�������� ��� �� ����");

			X1Gui().SameLine(246);
			DCheckBox("Automatic fire", u8"�����������", AutoFire, u8"");
			DCheckBox("Flash check", u8"�������� ����������", FlashCheck, u8"��������� ��� ���� �� ���������");

			X1Gui().SameLine(246);
			DCheckBox("Jump check##5677", u8"�������� ������##5677", JumpCheck, u8"��������� ��� ���� �� ��������");
			DCheckBox("Smoke check##343434", u8"�������� ����##343434", SmokeCheck, u8"��������� �������� ����������� �� �����");
			X1Gui().SameLine(246);
			DCheckBox("Enemy jump check", u8"�������� ������ �����", JumpEnemyCheck, u8"��������� �������� ������� � ������");

			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			DCheckBox("Draw FOV", u8"���", DrawFov, u8"�������� ����������� ���� ����");

			X1Gui().SameLine(185.f);
			DColorEdit("Color##Fov1", u8"����##��", FovColor);
			X1Gui().SameLine(246);

			DCheckBox("Show target", u8"���������� ����", ShowSpot, u8"���������� ������� �����");

			DCheckBox("Draw silent FOV", u8"��� ��������", DrawSilentFov, u8"�������� ����������� ���� ������� ����");

			X1Gui().SameLine(185.f);
			DColorEdit("Color##SFov1", u8"����##���", SilentFovColor);

			X1Gui().SameLine(246);

			DCheckBox("Wall check", u8"�������� ����", WallCheck, u8"�������� �������� ����");

			if (SelectedWeapon <= 9 || WeaponCustomTypes == 3 || WeaponCustomTypes == 2 || WeaponCustomTypes == 1)
			{
				DCheckBox("Automatic pistol", u8"�����������", Weapons[GetWeap(SelectedWeapon)].AutoPistol, u8"");
				X1Gui().SameLine(246);
			}
			if ((SelectedWeapon > 28 && SelectedWeapon <= 33) || SelectedWeapon == 35 || WeaponCustomTypes == 3 || WeaponCustomTypes == 2 || WeaponCustomTypes)
			{
				DCheckBox("Fast zoom", u8"������� ������", Weapons[GetWeap(SelectedWeapon)].FastZoom, u8"");
			}

		}
		else if (SubtabAimMiscSelected == 1)
		{
			X1Gui().PushItemWidth(250.f);
			X1Gui().Spacing();

			DCheckBox("FACEIT", u8"������", FaceIt, u8"�������� FaceIt �����");

			if (FaceIt)
			{
				X1Gui().SameLine();
				SliderFloats("RCS correction", u8"�������� Rcs", MouseSense, 0.0001, 8, u8"�������� �������� ������");
			}
			X1Gui().PushItemWidth(379);
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			DCheckBox("Entity aim", u8"��� �� �������", EntityAim, u8"����������� ��� �� ������� �������");
			if (EntityAim)
			{
				static char ModelN[64] = { 0 };
				TextEdit("Target entity name", u8"��� �������� �������", EntityTargetName, ModelN, 64);
				DCheckBox("Show entity name", u8"���������� ����� ��������", ShowEntitysName, u8"");
			}

			X1Gui().PushItemWidth(379);
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();
			DCheckBox("Aim bind", u8"���� ����", AimBind.Enable, u8"�������� ���� ���� �� ������");
			if (AimBind.Enable)
			{
				X1Gui().SameLine();
				DCheckBox("Hold", u8"���������", AimBind.Hold, u8"�������� ������ ���� ������ ���� ������ ������");
				X1Gui().Spacing();
				HotsKey("Button", u8"������", AimBind.Button, u8"������ ������ ��������� ����");

			}
			X1Gui().Spacing();
			DCheckBox("Debug", u8"�����", Debug, u8"");
		}
	}
	X1Gui().EndChild();

	VectorEx<const char*> AimPSettingsEng = { lolc("Basic"), lolc("RCS"), lolc("pSilent"), lolc("Delays") };
	VectorEx<const char*> AimPSettingsRus = { lolc(u8"���"), lolc(u8"�����"), lolc(u8"�������"), lolc(u8"��������") };

	TabsLabels(SubTabAimSelected, AimPSettingsEng, AimPSettingsRus, Vec2(X1Gui().GetCurWindowSize().x - (X1Gui().GetStyle().wndPadding.x * 2), 0), false);

	if (X1Gui().BeginChild(("Main2Child4"), Vec2(0, 216), true))
	{
		X1Gui().PushItemWidth(333.f);

		if (SubTabAimSelected == 0)
		{
			VectorEx<const char*> itemsSMFEng = { lolc("Deceleration of speed") , lolc("Constant speed") };
			VectorEx<const char*> itemsSMFRus = { lolc(u8"���������� ��������") , lolc(u8"���������� ��������") };
			DComboBox("Smooth factor", u8"������ ���������", Weapons[GetWeap(SelectedWeapon)].SmoothMoveFactor, itemsSMFEng, itemsSMFRus, "");

			if (Weapons[GetWeap(SelectedWeapon)].SmoothMethod == 0)
			{
				SliderFloats("Smooth", u8"���������", Weapons[GetWeap(SelectedWeapon)].Smooth, 0, 100, u8"������->�������");
			}
			else if (Weapons[GetWeap(SelectedWeapon)].SmoothMethod == 1)
			{

			}

			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			SliderFloats("Fov", u8"���", Weapons[GetWeap(SelectedWeapon)].Fov, 0, 300, "");

			VectorEx<const char*> itemsFPEng = { lolc("Screen center") , lolc("Recoil"), lolc("On player") };
			VectorEx<const char*> itemsFPRus = { lolc(u8"����� ������") , lolc(u8"������"), lolc(u8"�� ������") };
			DComboBox("FOV Pos", u8"������� ����", Weapons[GetWeap(SelectedWeapon)].FovPos, itemsFPEng, itemsFPRus, "");

			if (!EntityAim)
			{
				X1Gui().Spacing();
				X1Gui().Separator();
				X1Gui().Spacing();

				DCheckBox("Nearest", u8"�������", Weapons[GetWeap(SelectedWeapon)].Nearest, u8"�������������� ����� ��������");

				if (!Weapons[GetWeap(SelectedWeapon)].Nearest)
				{
					VectorEx<const char*> itemsHBEng = { lolc("Head"), lolc("Neck"), lolc("Low neck"), lolc("Body"), lolc("Thorax"), lolc("Chest"), lolc("Upper chest") };
					VectorEx<const char*> itemsHBRus = { lolc(u8"������"), lolc(u8"���"), lolc(u8"��� ���"), lolc(u8"����"), lolc(u8"����"), lolc(u8"�����"), lolc(u8"���� �����") };
					DComboBox("Hitbox", u8"�������", Weapons[GetWeap(SelectedWeapon)].HitBox, itemsHBEng, itemsHBRus, "");
				}

				if (!FaceIt)
				{
					X1Gui().Spacing();
					X1Gui().Separator();
					X1Gui().Spacing();

					DCheckBox("Backtrack", u8"����� �����", Weapons[GetWeap(SelectedWeapon)].Backtrack, u8"");
					if (Weapons[GetWeap(SelectedWeapon)].Backtrack)
					{
						SliderInts("Ticks", u8"����", Weapons[GetWeap(SelectedWeapon)].BacktrackTicks, 0, 16, u8"������->�������");

						DCheckBox("Show backtrack", u8"�������� �������", ShowBacktrack, u8"");
					}
				}
			}
		}
		else if (SubTabAimSelected == 1)
		{

			SliderFloats("RCS X", u8"RCS �� X", Weapons[GetWeap(SelectedWeapon)].RcsY, 0, 150, u8"���� �������� ������ �� ��� X");
			SliderFloats("RCS Y", u8"RCS �� Y", Weapons[GetWeap(SelectedWeapon)].RcsX, 0, 150, u8"���� �������� ������ �� ��� Y");

			X1Gui().Spacing();
			X1Gui().Spacing();
			if (false)
				X1Gui().PushItemWidth(260.f);

			SliderFloats("RCS Smooth", u8"��������� ����� ������ ����", Weapons[GetWeap(SelectedWeapon)].SmoothRcs, 0, 100, u8"");
			SliderFloats("RCS FOV", u8"��� ����� ������ ����", Weapons[GetWeap(SelectedWeapon)].FovRcs, 0, 300, u8"");
			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			DCheckBox("Standalone RCS", u8"Rcs ���������", Weapons[GetWeap(SelectedWeapon)].RcsStandelone, u8"�������� ���������� �������� ������");

			if (!EntityAim)
			{
				X1Gui().Spacing();
				X1Gui().Separator();
				X1Gui().Spacing();

				DCheckBox("Hitbox after first bullet", u8"������� ����� ������ ����", Weapons[GetWeap(SelectedWeapon)].HitBoxAfter1B, u8"");

				if (Weapons[GetWeap(SelectedWeapon)].HitBoxAfter1B)
				{
					DCheckBox("Nearest RCS", u8"������� RCS", Weapons[GetWeap(SelectedWeapon)].NearestRcs, u8"���� ����� �������� ����� ������ ����");

					if (!Weapons[GetWeap(SelectedWeapon)].NearestRcs)
					{
						VectorEx<const char*> itemsHBEng = { lolc("Head"), lolc("Neck"), lolc("Low neck"), lolc("Body"), lolc("Thorax"), lolc("Chest"), lolc("Upper chest") };
						VectorEx<const char*> itemsHBRus = { lolc(u8"������"), lolc(u8"���"), lolc(u8"��� ���"), lolc(u8"����"), lolc(u8"����"), lolc(u8"�����"),lolc(u8"���� �����") };
						DComboBox("Hitbox", u8"�������", Weapons[GetWeap(SelectedWeapon)].HitBoxRcs, itemsHBEng, itemsHBRus, "");
					}
				}
			}
			X1Gui().PushItemWidth(333.f);
		}
		else if (SubTabAimSelected == 2)
		{
			if (!FaceIt)
			{
				DCheckBox("Silent", u8"�������� �������", Weapons[GetWeap(SelectedWeapon)].Silent, u8"�������� ������� �������");
				SliderInts("% chance", u8"%���������", Weapons[GetWeap(SelectedWeapon)].SilentHitchance, 0, 100, u8"");

				SliderFloats("FOV", u8"���", Weapons[GetWeap(SelectedWeapon)].SilentFov, 0, 30, "");

				if (!EntityAim)
				{
					X1Gui().Spacing();
					X1Gui().Separator();
					X1Gui().Spacing();

					DCheckBox("Nearest", u8"Nearest", Weapons[GetWeap(SelectedWeapon)].SilentNearest, u8"�������������� ����� ��������");

					if (!Weapons[GetWeap(SelectedWeapon)].SilentNearest)
					{
						VectorEx<const char*> itemsHBEng = { lolc("Head"), lolc("Neck"), lolc("Low neck"), lolc("Body"), lolc("Thorax"), lolc("Chest"), lolc("Upper chest") };
						VectorEx<const char*> itemsHBRus = { lolc(u8"������"), lolc(u8"���"), lolc(u8"��� ���"), lolc(u8"����"), lolc(u8"����"), lolc(u8"�����"),lolc(u8"���� �����") };
						DComboBox("Hitbox", u8"�������", Weapons[GetWeap(SelectedWeapon)].SilentHitBox, itemsHBEng, itemsHBRus, "");
					}

					X1Gui().Spacing();
					X1Gui().Spacing();
				}
				X1Gui().Spacing();
				X1Gui().Separator();
				X1Gui().Spacing();

				SliderInts("Start bullet", u8"��������� ����", Weapons[GetWeap(SelectedWeapon)].SilentStartBullet, 0, 100, "");
				SliderInts("End bullet", u8"�������� ����", Weapons[GetWeap(SelectedWeapon)].SilentEndBullet, 0, 100, u8"0 ��� ���������� �������� ����");
			}
		}
		else if (SubTabAimSelected == 3)
		{
			DCheckBox("Target switch delay", u8"�������� ������", Weapons[GetWeap(SelectedWeapon)].TargetSwitchDelayEnable, u8"�������� �������� ������ ����� ���� ����� ��������");

			if (Weapons[GetWeap(SelectedWeapon)].TargetSwitchDelayEnable)
				SliderFloats("Delay##tsw", u8"��������##���", Weapons[GetWeap(SelectedWeapon)].TargetSwitchDelay, 0, 1.f, "");

			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			DCheckBox("First bullet delay", u8"�������� ����� ������ ���������", Weapons[GetWeap(SelectedWeapon)].FirstFireDelayEnable, u8"�������� �������� ��� ������� ��������");

			if (Weapons[GetWeap(SelectedWeapon)].FirstFireDelayEnable)
			{
				DCheckBox("One click delay", u8"�������� �� 1 �������", Weapons[GetWeap(SelectedWeapon)].FirstFireDelayOneClick, u8"");

				if (!EntityAim)
				{
					X1Gui().SameLine();
					DCheckBox("Auto", u8"����", Weapons[GetWeap(SelectedWeapon)].FirstFireDelayAuto, u8"�������� �������������� ��������");
				}

				if (!FaceIt)
				{
					X1Gui().SameLine();
					DCheckBox("Silent auto delay", u8"���� �������� ��� ��������", Weapons[GetWeap(SelectedWeapon)].FirstFireDelaySilentAuto, u8"������� ���������� ������ ���� ���� ������ � ��� ��������");
				}

				SliderFloats("Delay##ffdef", u8"��������##���", Weapons[GetWeap(SelectedWeapon)].FirstFireDelay, 0, 1.f, "");
			}

			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			DCheckBox("Kill stop", u8"���������� ����� ��������", Weapons[GetWeap(SelectedWeapon)].KillStop, u8"��������� ��� ����� ������� ��������");
		}
	}
	X1Gui().EndChild();
	X1Gui().Spacing();

	if (X1Gui().BeginChild(("TriggerChild"), Vec2(0, 206), true))
	{
		static int SubtabTrigMiscSelected = 0;

		static VectorEx<const char*> AimTrigSettingsEng = { "Basic##trig","Bind##trig" };

		TabsLabels(SubtabTrigMiscSelected, AimTrigSettingsEng, AimTrigSettingsEng, Vec2(X1Gui().GetCurWindowSize().x - (X1Gui().GetStyle().wndPadding.x * 2), 0), false);

		X1Gui().Spacing();
		X1Gui().PushItemWidth(240.f);

		if (SubtabTrigMiscSelected == 0)
		{
			DCheckBox("Trigger", u8"�������", TriggerEnable, u8"�������� �������");
			X1Gui().SameLine(163);
			DCheckBox("Aim at teammates", u8"������� �� ����", TriggerDeathmatch, u8"�������� ������� ��  ���� �������");

			X1Gui().SameLine(360);
			X1Gui().BeginGroup();
			{
				vector<string> TrigHitboxes =
				{
					false ? lols(u8"������") : lols("Head"), //0
					false ? lols(u8"�����") : lols("Chest"), //1
					false ? lols(u8"�����") : lols("Stomach"), //2
					false ? lols(u8"����� ����") : lols("Left arm"), //3
					false ? lols(u8"������ ����") : lols("Right arm"), //4
					false ? lols(u8"����� ����") : lols("Left leg"), //5
					false ? lols(u8"������ ����") : lols("Right leg") //6
				};
				if (X1Gui().ListBoxHeader("##TrigHitboxes", Vec2(90, 142)))
				{
					for (int i = 0; i < 7; i++)
					{
						bool selected = ((i == 0) && Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_HEAD) ||
							((i == 1) && Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_CHEST) ||
							((i == 2) && Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_STOMACH) ||
							((i == 3) && Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_LEFTARM) ||
							((i == 4) && Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_RIGHTARM) ||
							((i == 5) && Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_LEFTLEG) ||
							((i == 6) && Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_RIGHTLEG);

						if (X1Gui().SelectLabel(TrigHitboxes.at(i).c_str(), selected))
						{
							switch (i)
							{
							case 0: Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_HEAD = !Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_HEAD; break;
							case 1: Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_CHEST = !Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_CHEST; break;
							case 2: Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_STOMACH = !Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_STOMACH; break;
							case 3: Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_LEFTARM = !Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_LEFTARM; break;
							case 4: Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_RIGHTARM = !Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_RIGHTARM; break;
							case 5: Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_LEFTLEG = !Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_LEFTLEG; break;
							case 6: Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_RIGHTLEG = !Weapons[GetWeap(SelectedWeapon)].TRIG_HITGROUP_RIGHTLEG; break;
							default: break;
							}
						}
					}
				}
				X1Gui().ListBoxFooter();

				DCheckBox("All", u8"���", Weapons[GetWeap(SelectedWeapon)].TriggerAllHitboxes, u8"�������� �������� ���� ���������");
			}
			X1Gui().EndGroup(true);

			DCheckBox("Flash check", u8"�������� ����������", TriggerFlashCheck, u8"��������� ������� ���� �� �������");
			X1Gui().SameLine(163);
			DCheckBox("Smoke check", u8"�������� ����", TriggerSmokeCheck, u8"��������� ������� ���� ����� �� �����");
			DCheckBox("Jump check", u8"�������� ������", TriggerJumpCheck, u8"��������� ������� ���� �� ��������");
			X1Gui().SameLine(163);
			DCheckBox("Enemy jump check", u8"������ �����", TriggerJumpEnemyCheck, u8"��������� ������� ���� ����� ��������");

			X1Gui().Spacing();

			SliderInts("RCS X##Trigger", u8"Rcs �� X##���", Weapons[GetWeap(SelectedWeapon)].TriggerRcsX, 0, 150, u8"���� �������� ������ �� ��� X");
			SliderInts("RCS Y##Trigger", u8"Rcs �� Y##���", Weapons[GetWeap(SelectedWeapon)].TriggerRcsY, 0, 150, u8"���� �������� ������ �� ��� Y");

			X1Gui().Spacing();

			SliderIntsProc("Hit chance##Trigger", u8"�������##���", Weapons[GetWeap(SelectedWeapon)].TriggerChanse, 0, 100, u8"�");

			SliderFloats("Delay##trig", u8"��������##���", Weapons[GetWeap(SelectedWeapon)].TriggerDelay, 0, 1.f, u8"�������� ����� ������ ��������� �������");
		}
		else if (SubtabTrigMiscSelected == 1)
		{
			DCheckBox("Bind##triggg", u8"����##�����", TriggerBind.Enable, u8"");
			if (TriggerBind.Enable)
			{
				X1Gui().SameLine();
				DCheckBox("Hold##triggg", u8"���������##�����", TriggerBind.Hold, u8"");
				HotsKey("Button##triggg", u8"������##�����", TriggerBind.Button, u8"������ ������ ��������� �������");
			}
		}
	}
	X1Gui().EndChild();
}

void CLegitAim::SubsectionsMenu()
{
	if (WeaponCustomTypes == 0)
	{
		if (SelectedWeapon == -1)
			SelectedWeapon = 0;

		static int selectedItem = SelectedWeapon;
		if (X1Gui().ListBoxHeader("##0", Vec2(154, 665 - 7)))
		{
			for (int i = 0; i < (sizeof(CGlobal::WeaponNames) / sizeof(*CGlobal::WeaponNames)); i++)
			{
				bool selected = i == SelectedWeapon;
				if (X1Gui().SelectLabel(CGlobal::WeaponNames[i].c_str(), selected, Vec2(144, 0)))
					SelectedWeapon = i;
			}
		}
		X1Gui().ListBoxFooter();
	}
	else if (WeaponCustomTypes == 1)
	{
		static int tabOrderCs[] = { 0 , 1 , 2 , 3 };

		string sub_name0 = lols("Rifles");
		string sub_name1 = lols("Pistols");
		string sub_name2 = lols("Shotguns");
		string sub_name3 = lols("Snipers");

		VectorEx<const char*>tabNamesCs = {
			sub_name0.c_str(),sub_name1.c_str() ,sub_name2.c_str() ,sub_name3.c_str() };

		Vec2 Pos = Vec2(0, 0);

		TabsLabels(StandardSubSelected, tabNamesCs, tabNamesCs, Vec2(X1Gui().GetCurWindowSize().x - (X1Gui().GetStyle().wndPadding.x * 2),
			X1Gui().GetCurWindowSize().y - (X1Gui().GetStyle().wndPadding.y * 2)), true);


		if (StandardSubSelected == 0)
			SelectedWeapon = 34;
		else if (StandardSubSelected == 1)
			SelectedWeapon = 35;
		else if (StandardSubSelected == 2)
			SelectedWeapon = 36;
		else if (StandardSubSelected == 3)
			SelectedWeapon = 37;
	}
	else if (WeaponCustomTypes == 2)
	{
		static char NewSubName[64] = { 't','e','s','t' };

		X1Gui().PushItemWidth(300);

		X1Gui().InputText(("Name"), NewSubName, 64);

		if (CustomSubs.size() < 33)
		{
			X1Gui().SameLine(365);
			if (X1Gui().Button(("Add"), Vec2(89, 0)) && string(NewSubName).length() > 0)
			{
				CustomSub  CustomEntry;
				CustomEntry.Idx = Weapons.size();
				CustomEntry.Name = NewSubName;
				CustomEntry.Name += "##" + to_string(CustomEntry.Idx);

				WeaponSettings WeaponEntry;
				WeaponEntry.WeaponName = CustomEntry.Name;
				WeaponEntry.IsCustomSub = true;

				CustomSubs.push_back(CustomEntry);
				Weapons.push_back(WeaponEntry);

				SelectedSub = CustomSubs.size() - 1;

				strcpy(NewSubName, "");
			}
		}

		if (SelectedSub < (int)CustomSubs.size() && SelectedSub >= 0)
		{
			if (X1Gui().Button(("Rename")))
			{
				if (string(NewSubName).length() > 0)
				{
					CustomSubs[SelectedSub].Name = NewSubName;
					strcpy(NewSubName, "");
				}
			}

			X1Gui().SameLine();
			if (X1Gui().Button(("Remove")))
			{
				for (size_t i = 0; i < CustomSubs[SelectedSub].AllocatedWeapon.size(); i++)
					UnallocatedWeapons.push_back(CustomSubs[SelectedSub].AllocatedWeapon[i]);

				for (size_t i = 0; i < Weapons.size(); i++)
					if (i == CustomSubs[SelectedSub].Idx)
					{
						for (size_t j(SelectedSub); j < CustomSubs.size(); j++)
							CustomSubs[j].Idx--;
						Weapons.erase(Weapons.begin() + i);
					}

				CustomSubs.erase(CustomSubs.begin() + SelectedSub);
			}

			X1Gui().Spacing();
			X1Gui().Separator();
			X1Gui().Spacing();

			if ((int)CustomSubs.size() <= SelectedSub)
				SelectedSub = CustomSubs.size() - 1;

			if (CustomSubs.size() > 0 && SelectedSub > -1)
			{
				if (SelectedSubAlloc < (int)CustomSubs[SelectedSub].AllocatedWeapon.size())
				{
					X1Gui().SameLine(155);
					if (X1Gui().Button((">>>"), Vec2(148, 0)))
					{
						UnallocatedWeapons.push_back(CustomSubs[SelectedSub].AllocatedWeapon[SelectedSubAlloc]);
						CustomSubs[SelectedSub].AllocatedWeapon.erase(CustomSubs[SelectedSub].AllocatedWeapon.begin() + SelectedSubAlloc);
						if (SelectedSubAlloc > 0)
							SelectedSubAlloc--;
					}
				}

				if ((int)UnallocatedWeapons.size() > 0 && SelectedSubUnAlloc < (int)UnallocatedWeapons.size() && SelectedSubUnAlloc >= 0)
				{
					X1Gui().SameLine(310);
					if (X1Gui().Button(("<<<"), Vec2(144, 0)))
					{
						CustomSubs[SelectedSub].AllocatedWeapon.push_back(UnallocatedWeapons[SelectedSubUnAlloc]);
						UnallocatedWeapons.erase(UnallocatedWeapons.begin() + SelectedSubUnAlloc);

						if (SelectedSubUnAlloc > 0)
							SelectedSubUnAlloc--;
					}
				}
			}
		}

		if (CustomSubs.size() > 0)
		{
			if (X1Gui().ListBoxHeader(("##Subsections"), Vec2(149, 583)))
			{
				for (size_t i = 0; i < CustomSubs.size(); i++)
				{
					bool selected = i == SelectedSub;
					if (X1Gui().SelectLabel(CustomSubs[i].Name.c_str(), selected))
						SelectedSub = i;
				}
			}
			X1Gui().ListBoxFooter();

			X1Gui().SameLine();
			if (X1Gui().ListBoxHeader(("##SubsectionWeapons"), Vec2(149, 583)))
			{
				if (SelectedSub < (int)CustomSubs.size())
				{
					for (size_t i = 0; i < CustomSubs[SelectedSub].AllocatedWeapon.size(); i++)
					{
						bool selected = i == SelectedSubAlloc;
						if (X1Gui().SelectLabel(CustomSubs[SelectedSub].AllocatedWeapon[i].c_str(), selected))
							SelectedSubAlloc = i;
					}
				}
			}
			X1Gui().ListBoxFooter();

			X1Gui().SameLine();

			if (X1Gui().ListBoxHeader(("UnallocatedWeapons"), Vec2(146, 583)))
			{
				if (SelectedSubUnAlloc < (int)UnallocatedWeapons.size())
				{
					for (size_t i = 0; i < UnallocatedWeapons.size(); i++)
					{
						bool selected = i == SelectedSubUnAlloc;
						if (X1Gui().SelectLabel(UnallocatedWeapons[i].c_str(), selected))
							SelectedSubUnAlloc = i;
					}
				}
			}
			X1Gui().ListBoxFooter();

			if (SelectedSub < (int)CustomSubs.size())
				if (CustomSubs[SelectedSub].Idx < (int)Weapons.size())
					SelectedWeapon = CustomSubs[SelectedSub].Idx;
				else
					SelectedWeapon = 0;
			else
				SelectedWeapon = 0;
		}
	}
}
