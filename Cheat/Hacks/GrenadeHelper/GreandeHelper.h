#pragma once
#include "../../Main.h"
#include "../Settings.h"

class IGHelper
{
protected:

	struct GHInfo
	{
		string descn = "";
		string act = "";
		string name = "";
		Vector start_pos = Vector(0, 0, 0);
		Vector head_pos = Vector(0, 0, 0);
		Vector directn = Vector(0, 0, 0);
		WEAPON_ID grenade = WEAPON_NONE;

		bool enable = true;
	};

	struct Map
	{
		string game_name;
		vector<GHInfo> helpers;
	};

	virtual void Add(string _game_name, GHInfo info) = 0;
	virtual bool CheckOldDistance(Map* map, Vector pos) = 0;

	virtual string GetGName(WEAPON_ID id) = 0;

	virtual void Draw() = 0;
	virtual void Menu() = 0;

	virtual void AutoAddHelp(string _game_name) = 0;
	virtual void AddHelp(string _game_name, GHInfo Info) = 0;

	virtual Map* GetMapByGName(string _game_name) = 0;

	virtual Vector CalcHelpPos(Vector target) = 0;
//	virtual Map* GetMapByHelp(GHInfo* help) = 0;

	//virtual void DeleteMap(string _game_name) = 0;
	//virtual void DeleteHelp(GHInfo* help) = 0;
};

class CGHelper : public IGHelper
{
private:
	vector<Map> maps;

	virtual void Add(string _game_name, GHInfo info);
	virtual bool CheckOldDistance(Map* map, Vector pos);

	string GetGName(WEAPON_ID id);
public:

	CConfig Config = CConfig(("GHelper"));

	bool Enable;
	float DistShowInfo;
	float DistShowHelp;
	bool ShowDiscp;
	bool ShowName;
	bool EnableFor1Grenade;
	int CrosshairStyle;

	GHInfo* SelectedGHInf = new GHInfo();
	Map* SelectedMap = new Map();

	void Initialize()
	{
		Enable = true;
		DistShowInfo = 150.f;
		DistShowHelp = 700.f;
		ShowDiscp = true;
		ShowName = true;
		EnableFor1Grenade = true;
		CrosshairStyle = 0;

		SelectedGHInf = new GHInfo();
		SelectedMap = new Map();

		RV(Enable, "Enable");
		RV(DistShowInfo, "DistShowInfo");
		RV(DistShowHelp, "DistShowHelp");
		RV(ShowDiscp, "ShowDiscp");
		RV(ShowName, "ShowName");
		RV(CrosshairStyle, "CrosshairStyle");
		RV(EnableFor1Grenade, "EnableFor1Grenade");
	}
	
	virtual void AutoAddHelp(string _game_name);
	virtual void AddHelp(string _game_name, GHInfo Info);

	virtual void Draw();
	virtual void Menu();

	virtual Map* GetMapByGName(string _game_name);
	/*virtual*/ Map* GetMapByHelp(GHInfo* help);

	/*virtual*/ void DeleteMap(string _game_name);
	/*virtual*/ void DeleteHelp(GHInfo* help);

	virtual Vector CalcHelpPos(Vector target);

	void ChangeGHI(GHInfo* info, char NameBuf[], char DescpBuf[], char MapName[], bool AdvMap);

	bool SaveMaps();
	bool LoadMaps();
};