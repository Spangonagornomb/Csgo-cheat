#include "GreandeHelper.h"
#include "../Setup.h"
#include "../../Main.h"
#include "../../License/License.h"
#include "../../License/base64.h"
#include "../../GUI/Gui.h"

CGHelper::Map* CGHelper::GetMapByGName(string _game_name)
{
	for (size_t i = 0; i < maps.size(); i++)
		if (maps[i].game_name == _game_name)
			return &maps[i];

	return nullptr;
}

CGHelper::Map* CGHelper::GetMapByHelp(GHInfo* help)
{
	if (!help)
		return nullptr;

	for (size_t i = 0; i < maps.size(); i++)
		for (size_t j = 0; j < maps[i].helpers.size(); j++)
			if (maps[i].helpers[j].start_pos == help->start_pos)
				return &maps[i];

	return nullptr;
}

void CGHelper::DeleteMap(string _game_name)
{
	int idx = -1;
	for (size_t i = 0; i < maps.size(); i++)
		if (maps[i].game_name == _game_name)
			idx = (int)i;

	if (idx != -1)
		maps.erase(maps.begin() + idx);
}
void CGHelper::DeleteHelp(GHInfo* help)
{
	if (!help)
		return;
	for (size_t i = 0; i < maps.size(); i++)
		for (size_t j = 0; j < maps[i].helpers.size(); j++)
			if (maps[i].helpers[j].start_pos == help->start_pos) {
				maps[i].helpers.erase(maps[i].helpers.begin() + j);
				break;
			}
}

void CGHelper::Add(string _game_name, GHInfo info)
{
	Map* cur_map = GetMapByGName(_game_name);
	if (cur_map)
		cur_map->helpers.push_back(info);
	else
	{
		Map entry;

		entry.game_name = _game_name;
		entry.helpers.push_back(info);

		maps.push_back(entry);
	}
}
bool CGHelper::CheckOldDistance(Map* map, Vector pos)
{
	if (!map)
		return true;

	for (size_t i = 0; i < map->helpers.size(); i++)
		if (map->helpers[i].start_pos.DistTo(pos) < 5.f)
			return false;

	return true;
}

string CGHelper::GetGName(WEAPON_ID id)
{
	if (id == WEAPON_ID::WEAPON_INCGRENADE)
		return !MainSettings().Russian ? "Molotov & Incendiary grenade" : u8"������������� ������� ��� �������";
	else if (id == WEAPON_ID::WEAPON_MOLOTOV)
		return !MainSettings().Russian ? "Molotov & Incendiary grenade" : u8"������������� ������� ��� �������";
	else if (id == WEAPON_ID::WEAPON_HEGRENADE)
		return !MainSettings().Russian ? "High explosive grenade" : u8"���������� �������";
	else if (id == WEAPON_ID::WEAPON_SMOKEGRENADE)
		return !MainSettings().Russian ? "Smoke grenade" : u8"������� �������";
	else if (id == WEAPON_ID::WEAPON_DECOY)
		return !MainSettings().Russian ? "Decoy grenade" : u8"������ �������";
	else if (id == WEAPON_ID::WEAPON_FLASHBANG)
		return !MainSettings().Russian ? "Flashbang" : u8"�������� �������";
	else if (id == WEAPON_ID::WEAPON_NONE)
		return !MainSettings().Russian ? "Unknown" : u8"Unknown";
	else
		return "Unknown";
}

void CGHelper::AutoAddHelp(string _game_name)
{
	GHInfo entry;
	CBaseEntity* pLocal = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());
	if (pLocal)
	{
		Vector MyEyePos = pLocal->GetEyePosition();
		float BestFovCrosshair = FLT_MAX;
		float BestDistStart = FLT_MAX;

		Vector BestStart;
		Vector BestCrosshair;

		for (int EntIndex = 0; EntIndex < I::EntityList()->GetHighestEntityIndex(); EntIndex++)
		{
			CBaseEntity* pEntity = (CBaseEntity*)I::EntityList()->GetClientEntity(EntIndex);

			if (!pEntity)
				continue;

			if (pEntity == pLocal)
				continue;

			const model_t* pModel = pEntity->GetModel();

			if (!pModel)
				continue;

			const char* pModelName = I::ModelInfo()->GetModelName(pModel);

			if (!pModelName)
				continue;

			if (string(pModelName).find("crosshair") != string::npos)
			{
				float Fov = GP_LegitAim->CalcFOV(CGlobal::GViewAngle, MyEyePos, pEntity->GetRenderOrigin());

				if (Fov < BestFovCrosshair)
				{
					BestFovCrosshair = Fov;
					BestCrosshair = pEntity->GetRenderOrigin();
				}
			}
			if (string(pModelName).find("*") != string::npos)
			{
				float dist = pEntity->GetRenderOrigin().DistTo(MyEyePos);
				if (dist < BestDistStart)
				{
					BestDistStart = dist;
					BestStart = pEntity->GetRenderOrigin();
				}
			}
		}

		BestStart.z = pLocal->GetRenderOrigin().z;

		entry.start_pos = BestStart;

		entry.act = "throw";
		entry.descn = "test_desc";
		entry.grenade = CGlobal::GWeaponID;

		if (!CheckOldDistance(GetMapByGName(_game_name), entry.start_pos))
			return;

		Add(_game_name, entry);
	}
}

void CGHelper::AddHelp(string _game_name, GHInfo Info)
{
	GHInfo entry;
	CBaseEntity* pLocal = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());
	if (pLocal)
	{
		Vector Start = pLocal->GetRenderOrigin();

		if (!CheckOldDistance(GetMapByGName(_game_name), Start))
		{
			Message::Get().Start(!MainSettings().Russian ? ("Already added!") : (u8"��� ���������!"));
			return;
		}

		if (Info.directn.x != 0 && Info.directn.y != 0)
			entry.directn = Vector(Info.directn.x, Info.directn.y, 0);
		else
			I::Engine()->GetViewAngles(entry.directn);

		entry.head_pos = pLocal->GetEyePosition();

		entry.start_pos = Start;
		entry.act = Info.act;
		entry.descn = Info.descn;

		if (Info.grenade == WEAPON_NONE)
			entry.grenade = CGlobal::GWeaponID;
		else
			entry.grenade = Info.grenade;

		entry.name = Info.name;

		Add(_game_name, entry);
	}
}

void CGHelper::Draw()
{
	Map* cur_map = GetMapByGName(I::Engine()->GetLevelName());
	CBaseEntity* pLocal = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());

	if (cur_map && pLocal)
	{
		GHInfo BestInfo;
		float BestDistance = FLT_MAX;

		for (size_t i = 0; i < cur_map->helpers.size(); i++)
		{
			if (!cur_map->helpers[i].enable)
				continue;

			bool selected = CGlobal::IsGuiVisble && SelectedGHInf && SelectedGHInf == &cur_map->helpers[i];


			if (EnableFor1Grenade && !selected)
			{
				bool force_show = (cur_map->helpers[i].grenade == WEAPON_INCGRENADE || cur_map->helpers[i].grenade == WEAPON_MOLOTOV) &&
					(CGlobal::GWeaponID == WEAPON_INCGRENADE || CGlobal::GWeaponID == WEAPON_MOLOTOV);

				if ((cur_map->helpers[i].grenade != CGlobal::GWeaponID) && !force_show)
					continue;
			}
			float dist = cur_map->helpers[i].start_pos.DistTo(pLocal->GetRenderOrigin());

			if (dist > DistShowHelp && !selected)
				continue;

			Color cicle_color = Color::White();
			string GranadeName = GetGName(cur_map->helpers[i].grenade);

			if (cur_map->helpers[i].grenade == WEAPON_ID::WEAPON_INCGRENADE)
				cicle_color = Color::Red();
			else if (cur_map->helpers[i].grenade == WEAPON_ID::WEAPON_MOLOTOV)
				cicle_color = Color::Red();
			else if (cur_map->helpers[i].grenade == WEAPON_ID::WEAPON_HEGRENADE)
				cicle_color = Color::Red();
			else if (cur_map->helpers[i].grenade == WEAPON_ID::WEAPON_SMOKEGRENADE)
				cicle_color = Color::Green();
			else if (cur_map->helpers[i].grenade == WEAPON_ID::WEAPON_DECOY)
				cicle_color = Color::White();
			else if (cur_map->helpers[i].grenade == WEAPON_ID::WEAPON_FLASHBANG)
				cicle_color = Color::Yellow();

			Vector ppos = cur_map->helpers[i].start_pos;

			GP_Render->DrawRing3D(ppos.x, ppos.y, ppos.z, 15.f, 24, cicle_color);

			Vector InfoPosScreen;
			Vector InfoPos = cur_map->helpers[i].start_pos;
			InfoPos.z += 35;

			if (CGlobal::WorldToScreen(InfoPos, InfoPosScreen))
			{
				GP_Render->DrawString(17, Vec2(InfoPosScreen.x, InfoPosScreen.y + 20), cicle_color, false, true, GranadeName.c_str());
				if (ShowName)
					GP_Render->DrawString(16, Vec2(InfoPosScreen.x, InfoPosScreen.y), Color::White(), false, true, cur_map->helpers[i].name.c_str());

				if (selected)
					GP_Render->DrawString(16, Vec2(InfoPosScreen.x, InfoPosScreen.y - 40), Color::White(), false, true, MainSettings().Russian ? u8"�������" : "Selected");
			}


			if (dist < BestDistance)
			{
				BestDistance = dist;
				BestInfo = cur_map->helpers[i];
			}
		}

		if (BestDistance < DistShowInfo)
		{
			string BestGName = GetGName(BestInfo.grenade);

			if (CrosshairStyle == 0)
			{
				Vector CrosshairScreen;

				Vector Crosshair = BestInfo.head_pos + (CGlobal::CalcDir(BestInfo.directn) * 250.f);

				if (CGlobal::WorldToScreen(Crosshair, CrosshairScreen))
				{
					GP_Render->DrawRing(CrosshairScreen.x, CrosshairScreen.y, 10.f, 32, Color::Red());

					Vector TCircleOfst = CalcHelpPos(BestInfo.start_pos);

					GP_Render->DrawRing(CrosshairScreen.x - TCircleOfst.x, CrosshairScreen.y - TCircleOfst.y, 8.f, 32, Color::Green());
					GP_Render->DrawRing(CrosshairScreen.x - TCircleOfst.x, CrosshairScreen.y - TCircleOfst.y, 1.f, 32, Color::Green());

					float line_dist = CrosshairScreen.DistTo(Vector(CGlobal::iScreenWidth / 2, CGlobal::iScreenHeight / 2, 0));
					Color LineColor = Color(255, 255, 255, 180.f - (180.f - ((line_dist > 180.f) ? 180.f : line_dist)));

					GP_Render->DrawLine(CrosshairScreen.x, CrosshairScreen.y, CGlobal::iScreenWidth / 2, CGlobal::iScreenHeight / 2, LineColor);
				}
			}
			else if (CrosshairStyle == 1)
			{
				struct line_s
				{
					Vector start;
					Vector end;

					line_s(Vector s, Vector e)
					{
						start = s;
						end = e;
					}

					void Draw(Color clr)
					{
						GP_Render->DrawLine(start.x, start.y, end.x, end.y, clr);
					}
				};

				Vector CrosshairScreen_1;
				Vector CrosshairScreen_2;

				Vector Crosshair_1 = BestInfo.head_pos + (CGlobal::CalcDir(BestInfo.directn) * 250.f);
				Vector Crosshair_2 = BestInfo.head_pos + (CGlobal::CalcDir(BestInfo.directn) * 200.f);

				if (CGlobal::WorldToScreen(Crosshair_1, CrosshairScreen_1) &&
					CGlobal::WorldToScreen(Crosshair_2, CrosshairScreen_2))
				{
					auto draw_cross = [&](Vector start, Color clr_1, Color clr_2) ->void
					{
						float len_1 = 8;
						float len_2 = 19;
						float len_3 = 53;

						line_s line_left_1(Vector(start.x - len_2, start.y, 0), Vector(start.x - len_3, start.y, 0));
						line_s line_right_1(Vector(start.x + len_2, start.y, 0), Vector(start.x + len_3, start.y, 0));
						line_s line_top_1(Vector(start.x, start.y - len_2, 0), Vector(start.x, start.y - len_3, 0));
						line_s line_down_1(Vector(start.x, start.y + len_2, 0), Vector(start.x, start.y + len_3, 0));

						line_s line_left_2(Vector(start.x - len_1, start.y, 0), Vector(start.x - len_2, start.y, 0));
						line_s line_right_2(Vector(start.x + len_1, start.y, 0), Vector(start.x + len_2, start.y, 0));
						line_s line_top_2(Vector(start.x, start.y - len_1, 0), Vector(start.x, start.y - len_2, 0));
						line_s line_down_2(Vector(start.x, start.y + len_1, 0), Vector(start.x, start.y + len_2, 0));

						line_left_1.Draw(clr_1);
						line_right_1.Draw(clr_1);
						line_top_1.Draw(clr_1);
						line_down_1.Draw(clr_1);

						line_left_2.Draw(clr_2);
						line_right_2.Draw(clr_2);
						line_top_2.Draw(clr_2);
						line_down_2.Draw(clr_2);
					};

					draw_cross(CrosshairScreen_1, Color::White(), Color::Red());
					draw_cross(CrosshairScreen_2, Color::Red(), Color::White());
				}
			}

			if (ShowDiscp)
			{
				string name = BestInfo.name + " (" + BestGName + ")";


				Vec2 g_name_size = GP_Render->CalcTextSize(name.c_str(), GP_Render->SzFonts[13]);
				g_name_size.x += 10;

				Vec2 desc_size = GP_Render->CalcTextSize(BestInfo.descn.c_str(), GP_Render->SzFonts[14]);
				Vec2 desc_pos = Vec2((CGlobal::iScreenWidth / 2) + 100, (CGlobal::iScreenHeight / 2) - 50);

				desc_size.x = (g_name_size.x > desc_size.x ? g_name_size.x + 6.f : desc_size.x);

				GP_Render->DrawFilledBox(desc_pos.x, desc_pos.y - g_name_size.y - 3.f, desc_size.x + 6, g_name_size.y + 3.f, Color(MainSettings().BackgroundColor.r(),
					MainSettings().BackgroundColor.g(),
					MainSettings().BackgroundColor.b(),
					200));

				GP_Render->DrawString(14, Vec2(desc_pos.x + 3.f, desc_pos.y + 3.f), MainSettings().TextColor, BestInfo.descn.c_str());

				GP_Render->DrawString(14, Vec2(desc_pos.x + 3.f, desc_pos.y - 3.f - g_name_size.y), MainSettings().TextColor, name.c_str());
			}
		}
	}
}

Vector CGHelper::CalcHelpPos(Vector target)
{
	CBaseEntity* plocal = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());

	if (!plocal)
		return Vector(0, 0, 0);

	Vector vAngle = Vector(0, 0, 0);;

	I::Engine()->GetViewAngles(vAngle);

	float range = 5.f;

	float r_1, r_2;
	float x_1, y_1;

	Vector LocalRendOrig = plocal->GetRenderOrigin();
	Vector LocalViewOfst = plocal->GetViewOffset();

	Vector vEyeOrigin = LocalRendOrig + LocalViewOfst;

	r_1 = -(target.y - vEyeOrigin.y);
	r_2 = target.x - vEyeOrigin.x;
	float Yaw = vAngle.y - 90.0f;

	float yawToRadian = Yaw * (float)(M_PI / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20.f;
	y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20.f;

	x_1 *= range;
	y_1 *= range;

	return Vector(x_1, y_1, 0);
}

bool CGHelper::SaveMaps()
{
	std::ofstream o("C:\\obnoxious framework\\GrenadeHelper");
	nlohmann::json JO;

	if (!maps.empty())
	{
		for (auto &v_map : maps)
		{
			nlohmann::json jmap_buf;

			jmap_buf["GameName"] = base64_encode((const unsigned char *)v_map.game_name.c_str(), v_map.game_name.length());

			for (auto &v_help : v_map.helpers)
			{
				nlohmann::json jhelp_buf;

				jhelp_buf["StartX"] = v_help.start_pos.x;
				jhelp_buf["StartY"] = v_help.start_pos.y;
				jhelp_buf["StartZ"] = v_help.start_pos.z;

				jhelp_buf["HeadX"] = v_help.head_pos.x;
				jhelp_buf["HeadY"] = v_help.head_pos.y;
				jhelp_buf["HeadZ"] = v_help.head_pos.z;

				jhelp_buf["DirX"] = v_help.directn.x;
				jhelp_buf["DirY"] = v_help.directn.y;

				jhelp_buf["Name"] = base64_encode((const unsigned char *)v_help.name.c_str(), v_help.name.length());
				jhelp_buf["Grenade"] = (int)v_help.grenade;
				jhelp_buf["Enable"] = v_help.enable;

				jmap_buf["Helpers"].push_back(jhelp_buf);
			}
			JO["Maps"].push_back(jmap_buf);
		}
	}

	CCrypt cr;

	o << cr.O(JO.dump()) << std::endl;

	JO.clear();
	o.close();

	return true;
}

string readFile(const string& fileName)
{
	ifstream f(fileName);
	if (!f)
		return ("Read Error");
	stringstream ss;
	ss << f.rdbuf();
	return ss.str();
}

bool CGHelper::LoadMaps()
{
	string JCont = readFile("C:\\obnoxious framework\\GrenadeHelper");

	if (JCont == ("Read Error"))
		return false;

	CCrypt cr;

	JCont = cr.I(JCont);

	nlohmann::json JI = nlohmann::json::parse(JCont);

	maps.clear();

	if (!JI["Maps"].is_null())
	{
		for (size_t i(0); i < JI["Maps"].size(); i++)
		{
			Map map_e;

			map_e.game_name = base64_decode(JI["Maps"].at(i)["GameName"].get<string>());

			for (size_t j(0); j < JI["Maps"].at(i)["Helpers"].size(); j++)
			{
				GHInfo info_e;

				info_e.start_pos.x = JI["Maps"].at(i)["Helpers"].at(j)["StartX"];
				info_e.start_pos.y = JI["Maps"].at(i)["Helpers"].at(j)["StartY"];
				info_e.start_pos.z = JI["Maps"].at(i)["Helpers"].at(j)["StartZ"];

				info_e.head_pos.x = JI["Maps"].at(i)["Helpers"].at(j)["HeadX"];
				info_e.head_pos.y = JI["Maps"].at(i)["Helpers"].at(j)["HeadY"];
				info_e.head_pos.z = JI["Maps"].at(i)["Helpers"].at(j)["HeadZ"];

				info_e.directn.x = JI["Maps"].at(i)["Helpers"].at(j)["DirX"];
				info_e.directn.y = JI["Maps"].at(i)["Helpers"].at(j)["DirY"];

				info_e.name = base64_decode(JI["Maps"].at(i)["Helpers"].at(j)["Name"].get<string>());

				info_e.grenade = (WEAPON_ID)JI["Maps"].at(i)["Helpers"].at(j)["Grenade"];
				info_e.enable = JI["Maps"].at(i)["Helpers"].at(j)["Enable"];

				map_e.helpers.push_back(info_e);
			}

			maps.push_back(map_e);
		}
	}

	JI.clear();

	return true;
}