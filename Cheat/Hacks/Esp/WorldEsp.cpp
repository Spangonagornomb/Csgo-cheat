#include "Esp.h"
#include "../Setup.h"

void CEsp::WorldEsp()
{
	auto LWorldEsp = [&]()->void
	{
		if (WorldGrenade || WorldWeapons || WorldBomb || WorldBombInfo)
		{
			CBaseEntity* plocal = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());
			int max_e = I::EntityList()->GetHighestEntityIndex();
			if (plocal)
			{
				for (int EntIndex = 0; EntIndex < max_e; EntIndex++)
				{
					CBaseEntity* pEntity = (CBaseEntity*)I::EntityList()->GetClientEntity(EntIndex);

					if (!pEntity || pEntity->IsPlayer())
						continue;

					if (pEntity == (CBaseEntity*)0xE || pEntity == (CBaseEntity*)0xB)
						continue;

					const model_t* pModel = pEntity->GetModel();

					if (!pModel)
						continue;

					const char* pModelName = I::ModelInfo()->GetModelName(pModel);

					if (!pModelName)
						continue;

					Vector EntityScreen;

					if (WorldGrenade)
					{
						if ((strstr(pModelName, ("_dropped.mdl")) && !strstr(pModelName, ("models/weapons/w_ied"))) || strstr(pModelName, ("smokegrenade_thrown.mdl")))
						{
							if (CGlobal::WorldToScreen(pEntity->GetRenderOrigin(), EntityScreen))
							{

								string  GrenadeName = "";
								Color GrenadeColor = Color::White();
								bool Finded = false;

								if (strstr(pModelName, ("fraggrenade")))
								{
									GrenadeColor = HEGrenadeColor;
									if (WorldGrenadeStyle == 2) GrenadeName = ("j");
									else GrenadeName = ("HE GRENADE");
									Finded = true;
								}
								else if (strstr(pModelName, ("molotov")))
								{
									GrenadeColor = MolotovGrenadeColor;
									if (WorldGrenadeStyle == 2) GrenadeName = ("l");
									else GrenadeName = ("MOLOTOV");
									Finded = true;
								}
								else if (strstr(pModelName, ("flashbang")))
								{
									GrenadeColor = FlashGrenadeColor;
									if (WorldGrenadeStyle == 2) GrenadeName = ("i");
									else GrenadeName = ("FLASHBANG");
									Finded = true;
								}
								else if (strstr(pModelName, ("incendiarygrenade")))
								{
									GrenadeColor = IncGrenadeColor;
									if (WorldGrenadeStyle == 2) GrenadeName = ("n");
									else GrenadeName = ("INCENDIARY");
									Finded = true;
								}
								else if (strstr(pModelName, ("smokegrenade_thrown.mdl")))
								{
									GrenadeColor = SmokeGrenadeColor;
									if (WorldGrenadeStyle == 2) GrenadeName = ("k");
									else GrenadeName = ("SMOKE");
									Finded = true;
								}
								else if (strstr(pModelName, ("decoy")))
								{
									GrenadeColor = DecoyGrenadeColor;
									if (WorldGrenadeStyle == 2) GrenadeName = ("m");
									else GrenadeName = ("DECOY");
									Finded = true;
								}

								if (Finded)
								{
									switch (WorldGrenadeStyle)
									{
									case 0: GP_Render->DrawString(TextGrenadeSize, Vec2((int)EntityScreen.x, (int)EntityScreen.y), GrenadeColor, true, true, GrenadeName.c_str()); break;
									case 1: GP_Render->DrawRing((int)EntityScreen.x, (int)EntityScreen.y, 10, 32, GrenadeColor, 2.f); break;
									case 2: GP_Render->DrawStringIcon(TextGrenadeSize, Vec2((int)EntityScreen.x, (int)EntityScreen.y), GrenadeColor, true, true, GrenadeName.c_str()); break;
									default:
										break;
									}
								}
							}
						}
					}

					if (WorldWeapons)
					{
						if (strstr(pModelName, ("models/weapons/w_")) && strstr(pModelName, ("_dropped.mdl")) &&
							!strstr(pModelName, ("models/weapons/w_eq_")) && !strstr(pModelName, ("models/weapons/w_ied")))
						{
							if (CGlobal::WorldToScreen(pEntity->GetRenderOrigin(), EntityScreen))
							{
#define Lol(s) s
								string WeaponName = pModelName + 17;
								WeaponName[WeaponName.size() - 12] = '\0';
								if (WorldWeaponsStyle == 0)
								{
									if (strstr(WeaponName.c_str(), ("rif_ak47"))) WeaponName = Lol(M_WEAPON_AK47_STR);
									else if (strstr(WeaponName.c_str(), ("rif_aug"))) WeaponName = Lol(M_WEAPON_AUG_STR);
									else if (strstr(WeaponName.c_str(), ("rif_famas")))WeaponName = Lol(M_WEAPON_FAMAS_STR);
									else if (strstr(WeaponName.c_str(), ("rif_galilar"))) WeaponName = Lol(M_WEAPON_GALILAR_STR);
									else if (strstr(WeaponName.c_str(), ("rif_m4a1_s"))) WeaponName = Lol(M_WEAPON_M4A1_S_STR);
									else if (strstr(WeaponName.c_str(), ("rif_m4a1"))) WeaponName = Lol(M_WEAPON_M4A1_STR);
									else if (strstr(WeaponName.c_str(), ("rif_sg556"))) WeaponName = Lol(M_WEAPON_SG553_STR);
									else if (strstr(WeaponName.c_str(), ("pist_223"))) WeaponName = Lol(M_WEAPON_USP_S_STR);
									else if (strstr(WeaponName.c_str(), ("pist_revolver"))) WeaponName = Lol(M_WEAPON_REVOLVER_STR);
									else if (strstr(WeaponName.c_str(), ("pist_cz_75"))) WeaponName = Lol(M_WEAPON_CZ75A_STR);
									else if (strstr(WeaponName.c_str(), ("pist_deagle"))) WeaponName = Lol(M_WEAPON_DEAGLE_STR);
									else if (strstr(WeaponName.c_str(), ("pist_elite"))) WeaponName = Lol(M_WEAPON_ELITE_STR);
									else if (strstr(WeaponName.c_str(), ("pist_fiveseven"))) WeaponName = Lol(M_WEAPON_FIVESEVEN_STR);
									else if (strstr(WeaponName.c_str(), ("pist_glock18"))) WeaponName = Lol(M_WEAPON_GLOCK_STR);
									else if (strstr(WeaponName.c_str(), ("pist_hkp2000"))) WeaponName = Lol(M_WEAPON_HKP2000_STR);
									else if (strstr(WeaponName.c_str(), ("pist_p250"))) WeaponName = Lol(M_WEAPON_P250_STR);
									else if (strstr(WeaponName.c_str(), ("pist_tec9"))) WeaponName = Lol(M_WEAPON_TEC9_STR);
									else if (strstr(WeaponName.c_str(), ("snip_awp"))) WeaponName = Lol(M_WEAPON_AWP_STR);
									else if (strstr(WeaponName.c_str(), ("snip_g3sg1"))) WeaponName = Lol(M_WEAPON_G3SG1_STR);
									else if (strstr(WeaponName.c_str(), ("snip_scar20"))) WeaponName = Lol(M_WEAPON_SCAR20_STR);
									else if (strstr(WeaponName.c_str(), ("snip_ssg08"))) WeaponName = Lol(M_WEAPON_SSG08_STR);
									else if (strstr(WeaponName.c_str(), ("smg_mac10"))) WeaponName = Lol(M_WEAPON_MAC10_STR);
									else if (strstr(WeaponName.c_str(), ("smg_mp7"))) WeaponName = Lol(M_WEAPON_MP7_STR);
									else if (strstr(WeaponName.c_str(), ("smg_mp9"))) WeaponName = Lol(M_WEAPON_MP9_STR);
									else if (strstr(WeaponName.c_str(), ("smg_ump45"))) WeaponName = Lol(M_WEAPON_UMP45_STR);
									else if (strstr(WeaponName.c_str(), ("smg_mp5sd"))) WeaponName = Lol(M_WEAPON_MP5_STR);
									else if (strstr(WeaponName.c_str(), ("smg_p90"))) WeaponName = Lol(M_WEAPON_P90_STR);
									else if (strstr(WeaponName.c_str(), ("shot_mag7"))) WeaponName = Lol(M_WEAPON_MAG7_STR);
									else if (strstr(WeaponName.c_str(), ("smg_bizon"))) WeaponName = Lol(M_WEAPON_BIZON_STR);
									else if (strstr(WeaponName.c_str(), ("shot_nova"))) WeaponName = Lol(M_WEAPON_NOVA_STR);
									else if (strstr(WeaponName.c_str(), ("shot_sawedoff"))) WeaponName = Lol(M_WEAPON_SAWEDOFF_STR);
									else if (strstr(WeaponName.c_str(), ("shot_xm1014"))) WeaponName = Lol(M_WEAPON_XM1014_STR);
									else if (strstr(WeaponName.c_str(), ("mach_m249"))) WeaponName = Lol(M_WEAPON_M249_STR);
									else if (strstr(WeaponName.c_str(), ("mach_negev"))) WeaponName = Lol(M_WEAPON_NEGEV_STR);
									else WeaponName = WeaponName;

									GP_Render->DrawString(TextWorldWeaponSize, Vec2((int)EntityScreen.x, (int)EntityScreen.y), WorldWeaponsColor, true, true, WeaponName.c_str());
								}
								else if (WorldWeaponsStyle == 1)
								{
									if (strstr(WeaponName.c_str(), ("rif_ak47"))) WeaponName = ("W");
									else if (strstr(WeaponName.c_str(), ("rif_aug"))) WeaponName = ("U");
									else if (strstr(WeaponName.c_str(), ("rif_famas"))) WeaponName = ("R");
									else if (strstr(WeaponName.c_str(), ("rif_galilar"))) WeaponName = ("Q");
									else if (strstr(WeaponName.c_str(), ("rif_m4a1_s"))) WeaponName = ("T");
									else if (strstr(WeaponName.c_str(), ("rif_m4a1"))) WeaponName = ("S");
									else if (strstr(WeaponName.c_str(), ("rif_sg556"))) WeaponName = ("V");
									else if (strstr(WeaponName.c_str(), ("pist_223"))) WeaponName = ("G");
									else if (strstr(WeaponName.c_str(), ("pist_revolver"))) WeaponName = ("J");
									else if (strstr(WeaponName.c_str(), ("pist_cz_75"))) WeaponName = ("I");
									else if (strstr(WeaponName.c_str(), ("pist_deagle"))) WeaponName = ("A");
									else if (strstr(WeaponName.c_str(), ("pist_elite"))) WeaponName = ("B");
									else if (strstr(WeaponName.c_str(), ("pist_fiveseven"))) WeaponName = ("C");
									else if (strstr(WeaponName.c_str(), ("pist_glock18"))) WeaponName = ("D");
									else if (strstr(WeaponName.c_str(), ("pist_hkp2000"))) WeaponName = ("F");
									else if (strstr(WeaponName.c_str(), ("pist_p250"))) WeaponName = ("F");
									else if (strstr(WeaponName.c_str(), ("pist_tec9"))) WeaponName = ("H");
									else if (strstr(WeaponName.c_str(), ("snip_awp"))) WeaponName = ("Z");
									else if (strstr(WeaponName.c_str(), ("snip_g3sg1"))) WeaponName = ("X");
									else if (strstr(WeaponName.c_str(), ("snip_scar20"))) WeaponName = ("Y");
									else if (strstr(WeaponName.c_str(), ("snip_ssg08"))) WeaponName = ("a");
									else if (strstr(WeaponName.c_str(), ("smg_mac10"))) WeaponName = ("K");
									else if (strstr(WeaponName.c_str(), ("smg_mp7"))) WeaponName = ("N");
									else if (strstr(WeaponName.c_str(), ("smg_mp9"))) WeaponName = ("O");
									else if (strstr(WeaponName.c_str(), ("smg_ump45"))) WeaponName = ("L");
									else if (strstr(WeaponName.c_str(), ("smg_p90"))) WeaponName = ("P");
									else if (strstr(WeaponName.c_str(), ("shot_mag7"))) WeaponName = ("d");
									else if (strstr(WeaponName.c_str(), ("smg_bizon"))) WeaponName = ("M");
									else if (strstr(WeaponName.c_str(), ("shot_nova"))) WeaponName = ("e");
									else if (strstr(WeaponName.c_str(), ("shot_sawedoff"))) WeaponName = ("c");
									else if (strstr(WeaponName.c_str(), ("shot_xm1014"))) WeaponName = ("b");
									else if (strstr(WeaponName.c_str(), ("mach_m249"))) WeaponName = ("g");
									else if (strstr(WeaponName.c_str(), ("mach_negev"))) WeaponName = ("f");
									else WeaponName = WeaponName;

									if (WeaponName.size() == 1)
										GP_Render->DrawStringIcon(TextWorldWeaponSize, Vec2((int)EntityScreen.x, (int)EntityScreen.y), WorldWeaponsColor, true, true, WeaponName.c_str());
									else
										GP_Render->DrawString(TextWorldWeaponSize, Vec2((int)EntityScreen.x, (int)EntityScreen.y), WorldWeaponsColor, true, true, WeaponName.c_str());
								}
							}
						}
					}

					if (WorldBomb)
					{
						if (pEntity->GetClientClass()->m_ClassID == (int)CLIENT_CLASS_ID::CC4 || pEntity->GetClientClass()->m_ClassID == (int)CLIENT_CLASS_ID::CPlantedC4)
						{
							if (pEntity != (CBaseEntity*)0xB && CGlobal::WorldToScreen(pEntity->GetRenderOrigin(), EntityScreen))
							{
								if (WorldBombStyle == 0)
									GP_Render->DrawString(TextBombSize, Vec2((int)EntityScreen.x, (int)EntityScreen.y), WorldBombColor, true, true, ("BOMB"));
								else if (WorldBombStyle == 1)
									GP_Render->DrawStringIcon(TextBombSize, Vec2((int)EntityScreen.x, (int)EntityScreen.y), WorldBombColor, true, true, ("o"));
							}
						}
					}

					if (WorldBombInfo)
					{
						if (pEntity != (CBaseEntity*)0xB)
						{
							if (pEntity->GetClientClass()->m_ClassID == (int)CLIENT_CLASS_ID::CPlantedC4)
							{
								DrawBombInfo(pEntity, plocal);
							}
						}
					}
				}
			}
		}
	};
	LWorldEsp();
}

float Calc_Armor(float flDamage, int ArmorValue)
{
	float flArmorRatio = 0.5f;
	float flArmorBonus = 0.5f;
	if (ArmorValue > 0) {
		float flNew = flDamage * flArmorRatio;
		float flArmor = (flDamage - flNew) * flArmorBonus;

		if (flArmor > static_cast<float>(ArmorValue)) {
			flArmor = static_cast<float>(ArmorValue) * (1.f / flArmorBonus);
			flNew = flDamage - flArmor;
		}

		flDamage = flNew;
	}
	return flDamage;
}

void CEsp::DrawBombInfo(CBaseEntity* entity, CBaseEntity* Local)
{
	if (!entity)
		return;

	CBaseEntity* local = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());

	static bool IsDefusing = false;

	Color Back1(94, 102, 107, 229);
	Color Back2(15, 12, 17, 220);
	Color Back3(0, 0, 0, 0);
	int w_b = 5;
	int h_b = 37;
	if (IsDefusing)
		h_b += 20;

	GP_Render->DrawFilledBox(0, CGlobal::iScreenHeight / 2 + h_b / 2, w_b, h_b, Back1);
	GP_Render->DrawFilledBox(w_b, CGlobal::iScreenHeight / 2 + h_b / 2, 150, h_b, Back2);
	Vector vOrig; Vector vScreen;

	CBaseEntity* Bomb = (CBaseEntity*)entity;
	float flBlow = Bomb->GetC4BlowTime();
	float lifetime = flBlow - (I::GlobalVars()->interval_per_tick * Local->GetTickBase());

	float flDistance = Local->GetEyePosition().DistTo(entity->GetEyePosition());
	float a = 450.7f;
	float b = 75.68f;
	float c = 789.2f;
	float d = ((flDistance - b) / c);
	float flDamage = a * exp(-d * d);

	float damage = float((std::max)((int)ceilf(Calc_Armor(flDamage, Local->GetArmor())), 0));
	Color EndHp;
	if (lifetime > -2.f)
	{
		if (damage >= Local->GetHealth() && !local->IsDead())
			GP_Render->DrawString(15, Vec2(w_b + 4, CGlobal::iScreenHeight / 2 + h_b / 2 + 20), NameColor, true, false, ("You will die"));
		else if (Local->GetHealth() > damage && !local->IsDead())
		{
			if (Local->GetHealth() - damage > 10)
				EndHp = Color::White();
			else
				EndHp = Color::Red();
			GP_Render->DrawString(15, Vec2(w_b + 4, CGlobal::iScreenHeight / 2 + h_b / 2 + 20), NameColor, true, false, ("Health left: %.0f"), Local->GetHealth() - damage);
		}
	}
	Color EndTime;

	if (lifetime > 11)
		EndTime = Color::White();
	else
		EndTime = Color::Red();

	if (lifetime > 0)
		GP_Render->DrawString(15, Vec2(w_b + 4, CGlobal::iScreenHeight / 2 + h_b / 2 + 2), NameColor, true, false, ("Explosion time: %.1f"), lifetime);
	else
		GP_Render->DrawString(15, Vec2(w_b + 4, CGlobal::iScreenHeight / 2 + h_b / 2 + 2), NameColor, true, false, ("Explosion time: 0"));

	Color EndDefuse;
	if (Bomb->GetBombDefuser() > 0)
	{
		float countdown = Bomb->GetC4DefuseCountDown() - (Local->GetTickBase() * I::GlobalVars()->interval_per_tick);
		if (countdown > 0.01f)
		{
			if (lifetime > countdown)
				EndDefuse = Color::Red();
			else
				EndDefuse = Color::White();

			GP_Render->DrawString(15, Vec2(w_b + 4, CGlobal::iScreenHeight / 2 + h_b / 2 + 40), EndDefuse, true, false, ("Defuse time: %.1f"), countdown);

			IsDefusing = true;
		}
	}
	else
		IsDefusing = false;
}