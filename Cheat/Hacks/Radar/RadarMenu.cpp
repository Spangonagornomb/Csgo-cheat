#include "Radar.h"
#include"../../GUI/Gui.h"

void CRadar::Menu()
{

	VectorEx<const char*> RadTraceStyleEng = { lolc("Line"), lolc("FOV"),lolc("FOV + Line") };
	VectorEx<const char*> RadTraceStyleRus = { lolc(u8"�����"),lolc(u8"���"), lolc(u8"��� + �����") };

	VectorEx<const char*> PointStyleEng = { lolc("Filled box"), lolc("Box"), lolc("Filled circle"), lolc("Circle"), lolc("None") };
	VectorEx<const char*> PointStyleRus = { lolc(u8"������� ����"), lolc(u8"����"), lolc(u8"������� ����"), lolc(u8"����"), lolc("None") };

	X1Gui().PushItemWidth(360);

	DCheckBox("Radar", u8"������������� �����", RadarInGame, u8"���������� ������� �� ������������� ������");
	X1Gui().Spacing();
	X1Gui().Separator();
	X1Gui().Spacing();
	DCheckBox("Drawn radar", u8"�����", RadarActive, u8"���������� ����� ����");
	if (RadarActive)
	{
		X1Gui().SameLine();
		DCheckBox("With map", u8"� ������", TexturedRadar, u8"���������� ����� �� ������ ����");
	}
	X1Gui().Spacing();
	X1Gui().Separator();
	X1Gui().Spacing();
	if (RadarActive)
	{
		DCheckBox("Enemy", u8"�����", Enemy, u8"���������� ������ �� ������ ����");

		X1Gui().SameLine();
		DCheckBox("Team", u8"�������", Team, u8"���������� ������� �� ������ ����");

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		DColorEdit("CT color", u8"���� ��", ColorCT);
		X1Gui().SameLine();
		DColorEdit("Visible CT color", u8"������� ���� ��", ColorVisbleCT);


		DColorEdit("T color", u8"���� �", ColorTT);
		X1Gui().SameLine();
		DColorEdit("Visible T color", u8"������� ���� �", ColorVisbleTT);

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();
		SliderInts("Zoom", u8"�����������", Range, 1, 4000, u8"");
		X1Gui().Spacing();

		SliderInts("Position X", u8"������� �� X", RadarPosX, 1, CGlobal::iScreenWidth, u8"");
		SliderInts("Position Y", u8"������� �� Y", RadarPosY, 1, CGlobal::iScreenHeight, u8"");


		X1Gui().Spacing();

		SliderInts("Size X", u8"������ �� X", RadarSizeX, 1, 900, u8"");
		SliderInts("Size Y", u8"������ �� Y", RadarSizeY, 1, 900, u8"");


		X1Gui().Spacing();

		SliderInts("Alpha", u8"������������", Alpha, 1, 255, u8"");

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		DCheckBox("Fov", u8"���", Fov, u8"�������� ��� �� ������ ����");

		X1Gui().SameLine();
		DCheckBox("Line", u8"�����", Line, u8"�������� ����� �� ������ ����");
		X1Gui().SameLine();
		DCheckBox("Outline", u8"�������", OutLine, u8"�������� ������� ������ ����");
		X1Gui().SameLine();
		DCheckBox("Only in radar", u8"������  �� ������", OnlyInRadar, u8"���������� ������� ������ � ���� ����");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		DColorEdit("Background color", u8"���� ����", ColorBackground);
		DColorEdit("Line color", u8"���� �����", ColorLine);

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		SliderInts("Size##point", u8"������##���", PointSize, 2, 50, u8"������ ����� �������");
		DComboBox("Style##point", u8"�����##���", PointStyle, PointStyleEng, PointStyleRus, "����� ����� �������");
		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		DCheckBox("Sound", u8"����", Sound, u8"���������� ���� �� ������ ����");
		if (Sound)
		{
			SliderInts("Size##soud", u8"������##��", SoundPointSize, 2, 50, u8"������ ����� �����");
			DComboBox("Style##soud", u8"�����##��", SoundPointStyle, PointStyleEng, PointStyleRus, "����� ����� �����");
			DColorEdit("Color##soud", u8"����##��", SoundColor);
		}

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		DCheckBox("Trace", u8"����������� �������", Trace, u8"���������� ����������� �������� ������� �� ������ ����");

		if (Trace)
		{
			SliderInts("Lenght##trace", u8"�����##��", TraceLenght, 2, 300, u8"����� ����������� �������");
			DComboBox("Style##trace", u8"�����##��", TraceStyle, RadTraceStyleEng, RadTraceStyleRus, "����� ����������� �������");
		}

		X1Gui().Spacing();
		X1Gui().Separator();
		X1Gui().Spacing();

		DCheckBox("Name", u8"�����", Name, u8"���������� ����� ������� �� ������ ����");
		if (Name)
		{
			SliderInts("Text size##name", u8"������ ������##��", TextNameSize, 9, MAX_FONT_SIZE, u8"");

			DColorEdit("Color##name", u8"����##��", NameColor);
		}

	}
}