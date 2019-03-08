#pragma once

#include "../../Engine/Engine.h"
#include "../../Render/DXOverlayManager.h"
#include "../Settings.h"

class IRadar
{
protected:
	virtual void DrawMapImage(CBaseEntity* plocal) = 0;
	virtual void RenderPlayer(CBaseEntity* plocal) = 0;
	virtual void RenderSounds(CBaseEntity* plocal) = 0;
	virtual void RadarRange(float* x, float* y, float range) = 0;
	virtual void CalcRadarPoint(Vector vOrigin, float& screenx, float& screeny, bool ChechOut = true) = 0;



	virtual void Menu() = 0;
	virtual void Draw() = 0;
};

class CRadar : public IRadar
{
private:
	virtual void RadarRange(float* x, float* y, float range);
	virtual void CalcRadarPoint(Vector vOrigin, float& screenx, float& screeny, bool ChechOut = true);

	virtual void DrawMapImage(CBaseEntity* plocal);
	virtual void RenderPlayer(CBaseEntity* plocal);
	virtual void RenderSounds(CBaseEntity* plocal);

	Vector LocalRendOrig;
	Vector LocalViewOfst;

	IDirect3DTexture9 *RadarMapImage;
public:

	CConfig Config = CConfig(("Radar"));

	bool Enable = true;
	bool RadarActive = false;
	bool TexturedRadar = false;
	bool Team = false;
	bool Enemy = true;
	bool Sound = false;
	bool RadarInGame = false;
	int Range = 1890;
	int Alpha = 40;
	int RadarSizeX = 260;
	int RadarSizeY = 260;
	int RadarPosX = 306;
	int RadarPosY = 76;
	int PointSize = 5;
	int PointStyle = 2;
	int SoundPointSize = 4;
	int SoundPointStyle = 3;
	bool Trace = false;
	int TraceLenght = 20;
	int TraceStyle = 1;
	bool Fov = true;
	bool Line = true;
	bool OutLine = true;
	bool OnlyInRadar = false;
	bool Name = false;

	int TextNameSize = 12;

	Color ColorCT = Color(255, 0, 0, 255);
	Color ColorTT = Color(0, 255, 0, 255);
	Color ColorVisbleCT = Color(255, 255, 0, 255);
	Color ColorVisbleTT = Color(0, 255, 255, 255);
	Color ColorBackground = Color(20, 20, 20, 255);
	Color ColorLine = Color(255, 255, 255, 255);
	Color SoundColor = Color(255, 0, 100, 255);
	Color NameColor = Color(255, 255, 100, 255);

	virtual void Menu();
	virtual void Draw();

	CRadar()
	{
		RadarMapImage = nullptr;

		RV(ColorCT, "ColorCT");
		RV(ColorTT, "ColorTT");
		RV(ColorVisbleCT, "ColorVisbleCT");
		RV(ColorVisbleTT, "ColorCTColorVisbleTT")
		RV(ColorBackground, "ColorBackground");
		RV(ColorLine, "ColorLine");
		RV(SoundColor, "SoundColor");
		RV(NameColor, "NameColor");

		RV(RadarActive, "RadarActive");
		RV(TexturedRadar, "TexturedRadar");
		RV(Team, "Team");
		RV(Enemy, "Enemy");
		RV(Sound, "Sound");
		RV(RadarInGame, "RadarInGame");
		RV(Range, "Range");
		RV(Alpha, "Alpha");
		RV(RadarSizeX, "RadarSizeX");
		RV(RadarSizeY, "RadarSizeY");
		RV(RadarPosX, "RadarPosX");
		RV(RadarPosY, "RadarPosY");
		RV(PointSize, "PointSize");
		RV(PointStyle, "PointStyle");
		RV(Trace, "Trace");
		RV(TraceLenght, "TraceLenght");
		RV(TraceStyle, "TraceStyle");
		RV(Fov, "Fov");
		RV(Line, "Line");
		RV(OutLine, "OutLine");
		RV(OnlyInRadar, "OnlyInRadar");
		RV(Name, "Name");

		ADD_LOG("Setup: CRadar sucessful\n");
	}
};

