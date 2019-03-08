#include "Skins.h"
#include "../Setup.h"
#include <algorithm>
#include "../../X1API/MinHook/hook.h"

class CCStrike15ItemSchema;
class CCStrike15ItemSystem;

template <typename Key, typename Value>
struct Node_t
{
	int previous_id;		//0x0000
	int next_id;			//0x0004
	void* _unknown_ptr;		//0x0008
	int _unknown;			//0x000C
	Key key;				//0x0010
	Value value;			//0x0014
};

template <typename Key, typename Value>
struct Head_t
{
	Node_t<Key, Value>* memory;		//0x0000
	int allocation_count;			//0x0004
	int grow_size;					//0x0008
	int start_element;				//0x000C
	int next_available;				//0x0010
	int _unknown;					//0x0014
	int last_element;				//0x0018
}; //Size=0x001C

   // could use CUtlString but this is just easier and CUtlString isn't needed anywhere else
struct String_t
{
	char* buffer;	//0x0000
	int capacity;	//0x0004
	int grow_size;	//0x0008
	int length;		//0x000C
}; //Size=0x0010

struct CPaintKit
{
	int id;						//0x0000

	String_t name;				//0x0004
	String_t description;		//0x0014
	String_t item_name;			//0x0024
	String_t material_name;		//0x0034
	String_t image_inventory;	//0x0044

	char pad_0x0054[0x8C];		//0x0054
}; //Size=0x00E0

struct CStickerKit
{
	int id;

	int item_rarity;

	String_t name;
	String_t description;
	String_t item_name;
	String_t material_name;
	String_t image_inventory;

	int tournament_event_id;
	int tournament_team_id;
	int tournament_player_id;
	bool is_custom_sticker_material;

	float rotate_end;
	float rotate_start;

	float scale_min;
	float scale_max;

	float wear_min;
	float wear_max;

	String_t image_inventory2;
	String_t image_inventory_large;

	std::uint32_t pad0[4];
};
auto get_export(const char* module_name, const char* export_name) -> void*
{
	HMODULE mod;
	while (!((mod = FastCall::G().t_GetModuleHandleA(module_name))))
		FastCall::G().t_Sleep(100);

	return reinterpret_cast<void*>(FastCall::G().t_GetProcAddress(mod, export_name));
}

//RecvVarProxyFn fnSequenceProxyFn = NULL;
cDetour<RecvVarProxyFn>* fnSequenceProxyFn;
static auto random_sequence(const int low, const int high) -> int
{
	return rand() % (high - low + 1) + low;
}

void Hook_SetViewModelSequence(const CRecvProxyData *pDataConst, void *pStruct, void *pOut)
{
	CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);
	CBaseViewModel* pViewModel = (CBaseViewModel*)pStruct;

	if (pViewModel)
	{
		IClientEntity* pOwner = I::EntityList()->GetClientEntityFromHandle((PVOID)pViewModel->GetOwner());

		if (pOwner && pOwner->EntIndex() == I::Engine()->GetLocalPlayer())
		{
			const model_t* pModel = I::ModelInfo()->GetModel(pViewModel->GetModelIndex());
			const char* szModel = I::ModelInfo()->GetModelName(pModel);

			int m_nSequence = pData->m_Value.m_Int;

			if (!strcmp(szModel, ("models/weapons/v_knife_butterfly.mdl")))
			{
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_DRAW:
					m_nSequence = random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2); break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03); break;
				default:
					m_nSequence++;
				}
			}
			else if (!strcmp(szModel, ("models/weapons/v_knife_falchion_advanced.mdl")))
			{

				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_FALCHION_IDLE1; break;
				case SEQUENCE_DEFAULT_HEAVY_MISS1:
					m_nSequence = random_sequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP); break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = random_sequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02); break;
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				default:
					m_nSequence--;
				}
			}
			else if (!strcmp(szModel, ("models/weapons/v_knife_push.mdl")))
			{
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_DAGGERS_IDLE1; break;
				case SEQUENCE_DEFAULT_LIGHT_MISS1:
				case SEQUENCE_DEFAULT_LIGHT_MISS2:
					m_nSequence = random_sequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5); break;
				case SEQUENCE_DEFAULT_HEAVY_MISS1:
					m_nSequence = random_sequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1); break;
				case SEQUENCE_DEFAULT_HEAVY_HIT1:
				case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence += 3; break;
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				default:
					m_nSequence += 2;
				}
			}
			else if (!strcmp(szModel, ("models/weapons/v_knife_survival_bowie.mdl")))
			{
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_BOWIE_IDLE1; break;
				default:
					m_nSequence--;
				}
			}
			else if (!strcmp(szModel, ("models/weapons/v_knife_ursus.mdl")))
			{
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_DRAW:
					m_nSequence = random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
					break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
					break;
				default:
					m_nSequence++;
					break;
				}
			}
			else if (!strcmp(szModel, ("models/weapons/v_knife_stiletto.mdl")))
			{
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = CSX::Utils::RandomIntRange(12, 13);
					break;
				}
			}
			else if (!strcmp(szModel, ("models/weapons/v_knife_widowmaker.mdl")))
			{
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = CSX::Utils::RandomIntRange(14, 15);
					break;
				}
			}
			pData->m_Value.m_Int = m_nSequence;
		}
	}

	fnSequenceProxyFn->GetTrampoline()(pData, pStruct, pOut);
}
cDetour<RecvVarProxyFn>* oRecvnModelIndex;
void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	if (CGlobal::IsGameReady && GP_Skins)
		if (GP_Skins->SkinsEnable)
			GP_Skins->RecvProxy_Viewmodel(pData, pStruct, pOut);
	oRecvnModelIndex->GetTrampoline()(pData, pStruct, pOut);
}

void NetvarHook()
{
	SDK::ClientClass *pClass = I::Client()->GetAllClasses();
	while (pClass)
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;

		if (!strcmp(pszName, "DT_BaseViewModel"))
		{
			for (int i = 0; i < pClass->m_pRecvTable->m_nProps; i++)
			{
				SDK::RecvProp *pProp = &(pClass->m_pRecvTable->m_pProps[i]);
				const char *name = pProp->m_pVarName;
				if (!strcmp(name, "m_nModelIndex"))
				{
					auto& pContext = cContext::GetInstance();
					pContext.ApplyDetour<RecvVarProxyFn>((RecvVarProxyFn)pProp->m_ProxyFn,
						reinterpret_cast<RecvVarProxyFn>(
							Hooked_RecvProxy_Viewmodel),
						&oRecvnModelIndex);
				}
			}
		}
		pClass = pClass->m_pNext;
	}
}
auto CSkins::initialize_kits() -> void
{
	const auto V_UCS2ToUTF8 = static_cast<int(*)(const wchar_t* ucs2, char* utf8, int len)>(get_export(("vstdlib.dll"), ("V_UCS2ToUTF8")));

	// Search the relative calls

	// call    ItemSystem
	// push    dword ptr [esi+0Ch]
	// lea     ecx, [eax+4]
	// call    CEconItemSchema::GetPaintKitDefinition

	const auto sig_address = CSX::Memory::FindPattern(GetClientDll(), ("\xE8\x00\x00\x00\x00\xFF\x76\x0C\x8D\x48\x04\xE8"), ("x????xxxxxxx"), 0);


	// Skip the opcode, read rel32 address
	const auto item_system_offset = *reinterpret_cast<std::int32_t*>(sig_address + 1);

	// Add the offset to the end of the instruction
	const auto item_system_fn = reinterpret_cast<CCStrike15ItemSystem* (*)()>(sig_address + 5 + item_system_offset);

	// Skip VTable, first member variable of ItemSystem is ItemSchema
	const auto item_schema = reinterpret_cast<CCStrike15ItemSchema*>(std::uintptr_t(item_system_fn()) + sizeof(void*));

	// Dump paint kits
	{
		// Skip the instructions between, skip the opcode, read rel32 address
		const auto get_paint_kit_definition_offset = *reinterpret_cast<std::int32_t*>(sig_address + 11 + 1);

		// Add the offset to the end of the instruction
		const auto get_paint_kit_definition_fn = reinterpret_cast<CPaintKit*(__thiscall*)(CCStrike15ItemSchema*, int)>(sig_address + 11 + 5 + get_paint_kit_definition_offset);

		// The last offset is start_element, we need that

		// push    ebp
		// mov     ebp, esp
		// sub     esp, 0Ch
		// mov     eax, [ecx+298h]

		// Skip instructions, skip opcode, read offset
		const auto start_element_offset = *reinterpret_cast<std::intptr_t*>(std::uintptr_t(get_paint_kit_definition_fn) + 8 + 2);

		// Calculate head base from start_element's offset
		const auto head_offset = start_element_offset - 12;

		const auto map_head = reinterpret_cast<Head_t<int, CPaintKit*>*>(std::uintptr_t(item_schema) + head_offset);

		for (auto i = 0; i <= map_head->last_element; ++i)
		{
			const auto paint_kit = map_head->memory[i].value;

			if (paint_kit->id == 9001)
				continue;
			//I::Localize()
			const auto wide_name = I::Localize()->Find(paint_kit->item_name.buffer + 1);

			//const auto wide_name = g_localize->Find(paint_kit->item_name.buffer + 1);
			const auto cdn_name = paint_kit->name.buffer;
			char name[256];
			V_UCS2ToUTF8(wide_name, name, sizeof(name));

			if (paint_kit->id < 10000)
				skin_kits.push_back({ paint_kit->id, name, name, cdn_name });
			else
				glove_kits.push_back({ paint_kit->id, name, name, cdn_name });
		}

		//std::sort(skin_kits.begin(), skin_kits.end());
		//std::sort(glove_kits.begin(), glove_kits.end());
	}

	// Dump sticker kits
	{
		const auto sticker_sig = CSX::Memory::FindPattern(GetClientDll(), ("\x53\x8D\x48\x04\xE8\x00\x00\x00\x00\x8B\x4D\x10"), ("xxxxx????xxx"), 4);

		// Skip the opcode, read rel32 address
		const auto get_sticker_kit_definition_offset = *reinterpret_cast<std::intptr_t*>(sticker_sig + 1);

		// Add the offset to the end of the instruction
		const auto get_sticker_kit_definition_fn = reinterpret_cast<CPaintKit*(__thiscall*)(CCStrike15ItemSchema*, int)>(sticker_sig + 5 + get_sticker_kit_definition_offset);

		// The last offset is head_element, we need that

		//	push    ebp
		//	mov     ebp, esp
		//	push    ebx
		//	push    esi
		//	push    edi
		//	mov     edi, ecx
		//	mov     eax, [edi + 2BCh]

		// Skip instructions, skip opcode, read offset
		const auto start_element_offset = *reinterpret_cast<intptr_t*>(std::uintptr_t(get_sticker_kit_definition_fn) + 8 + 2);

		// Calculate head base from start_element's offset
		const auto head_offset = start_element_offset - 12;

		const auto map_head = reinterpret_cast<Head_t<int, CStickerKit*>*>(std::uintptr_t(item_schema) + head_offset);

		for (auto i = 0; i <= map_head->last_element; ++i)
		{
			const auto sticker_kit = map_head->memory[i].value;

			char sticker_name_if_valve_fucked_up_their_translations[64];

			auto sticker_name_ptr = sticker_kit->item_name.buffer + 1;

			if (strstr(sticker_name_ptr, ("StickerKit_dhw2014_dignitas")))
			{
				strcpy_s(sticker_name_if_valve_fucked_up_their_translations, ("StickerKit_dhw2014_teamdignitas"));
				strcat_s(sticker_name_if_valve_fucked_up_their_translations, sticker_name_ptr + 27);
				sticker_name_ptr = sticker_name_if_valve_fucked_up_their_translations;
			}
			const auto wide_name = I::Localize()->Find(sticker_name_ptr);

			char name[256];
			V_UCS2ToUTF8(wide_name, name, sizeof(name));

			sticker_kits.push_back({ sticker_kit->id, name, name, "" });
		}

		sticker_kits.insert(sticker_kits.begin(), { 0, "None", "", "" });
	}

	SetKillIconCfg();

	for (ClientClass* pClass = I::Client()->GetAllClasses(); pClass; pClass = pClass->m_pNext)
	{
		if (!strcmp(pClass->m_pNetworkName, ("CBaseViewModel")))
		{
			RecvTable* pClassTable = pClass->m_pRecvTable;

			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++)
			{
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];

				if (!pProp || strcmp(pProp->m_pVarName, ("m_nSequence")))
					continue;

				//fnSequenceProxyFn = pProp->m_ProxyFn;
				//pProp->m_ProxyFn = (RecvVarProxyFn)Hook_SetViewModelSequence;
				auto& pContext = cContext::GetInstance();
				pContext.ApplyDetour<RecvVarProxyFn>((RecvVarProxyFn)pProp->m_ProxyFn,
					reinterpret_cast<RecvVarProxyFn>(
						Hook_SetViewModelSequence),
					&fnSequenceProxyFn);
				break;
			}

			break;
		}
	}

	NetvarHook();

	ADD_LOG("Setup: Skins & Stickers sucessful\n");
} 
