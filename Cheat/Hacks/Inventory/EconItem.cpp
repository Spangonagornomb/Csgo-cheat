#include "EconItem.h"

CEconItem* CreateEconItem()
{
	static auto fnCreateSharedObjectSubclass_EconItem_
		= reinterpret_cast<CEconItem*(__stdcall*)()>(
			*reinterpret_cast<uintptr_t*>(CSX::Memory::FindSignature("client_panorama.dll", "C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4") + 3)
			);
	return fnCreateSharedObjectSubclass_EconItem_();
}
uintptr_t GetItemSchema()
{
	static auto fnGetItemSchema
		= reinterpret_cast<uintptr_t(__stdcall*)()>(
			CSX::Memory::FindSignature("client_panorama.dll", "A1 ? ? ? ? 85 C0 75 53")
			);
	return fnGetItemSchema();
}

uint32_t* CEconItem::GetAccountID()
{
	return reinterpret_cast<uint32_t*>(this + 0x1C);
}
uint64_t* CEconItem::GetItemID()
{
	return reinterpret_cast<uint64_t*>(this + 0x8);
}

uint64_t* CEconItem::GetOriginalID()
{
	return reinterpret_cast<uint64_t*>(this + 0x10);
}

uint16_t* CEconItem::GetDefIndex()
{
	return reinterpret_cast<uint16_t*>(this + 0x24);
}

uint32_t* CEconItem::GetInventory()
{
	return reinterpret_cast<uint32_t*>(this + 0x20);
}

unsigned char* CEconItem::GetFlags()
{
	return reinterpret_cast<unsigned char*>(this + 0x30);
}

unsigned short* CEconItem::GetEconItemData()
{
	return reinterpret_cast<unsigned short*>(this + 0x26);
}

void CEconItem::UpdateEquippedState(unsigned int state)
{
	static auto fnUpdateEquippedState
		= reinterpret_cast<int(__thiscall*)(CEconItem*, unsigned int)>(
			CSX::Memory::FindSignature("client_panorama.dll", "55 8B EC 8B 45 08 8B D0 C1 EA 10")
			);

	fnUpdateEquippedState(this, state);
}

void CEconItem::AddSticker(int index, int kit, float wear, float scale, float rotation)
{
	SetAttributeValue<int>(113 + 4 * index, kit);
	SetAttributeValue<float>(114 + 4 * index, wear);
	SetAttributeValue<float>(115 + 4 * index, scale);
	SetAttributeValue<float>(116 + 4 * index, rotation);
}

void CEconItem::SetStatTrak(int val)
{
	SetAttributeValue<int>(80, val);
	SetAttributeValue<int>(81, 0);
	SetQuality(ITEM_QUALITY_STRANGE);
}

void CEconItem::SetPaintKit(float kit)
{
	SetAttributeValue<float>(6, kit);
}

void CEconItem::SetPaintSeed(float seed)
{
	SetAttributeValue<float>(7, seed);
}

void CEconItem::SetPaintWear(float wear)
{
	SetAttributeValue<float>(8, wear);
}

void CEconItem::SetQuality(ItemQuality quality)
{
	auto data = *GetEconItemData();
	*GetEconItemData() = data ^ (data ^ 32 * quality) & 0x1E0;
}

void CEconItem::SetRarity(ItemRarity rarity)
{
	auto data = *GetEconItemData();
	*GetEconItemData() = (data ^ (rarity << 11)) & 0x7800 ^ data;
}

void CEconItem::SetOrigin(int origin)
{
	auto data = *GetEconItemData();
	*GetEconItemData() = data ^ ((unsigned __int8)data ^ (unsigned __int8)origin) & 0x1F;
}

void CEconItem::SetLevel(int level)
{
	auto data = *GetEconItemData();
	*GetEconItemData() = data ^ (data ^ (level << 9)) & 0x600;
}

void CEconItem::SetInUse(bool in_use)
{
	auto data = *GetEconItemData();
	*GetEconItemData() = data & 0x7FFF | (in_use << 15);
}

void CEconItem::SetCustomName(const char* name)
{
	static auto Address = CSX::Memory::FindSignature("client_panorama.dll", "E8 ? ? ? ? 8B 46 78 C1 E8 0A A8 01 74 13 8B 46 34");
	static auto fnSetCustomName
		= reinterpret_cast<CEconItem*(__thiscall*)(void*, const char*)>(
			*reinterpret_cast<uintptr_t*>(Address + 1) + Address + 5
			);

	fnSetCustomName(this, name);
}

void CEconItem::SetCustomDesc(const char* name)
{
	static auto Address = CSX::Memory::FindSignature("client_panorama.dll", "E8 ? ? ? ? 33 DB 39 5E 3C 7E 5E");
	static auto fnSetCustomDesc
		= reinterpret_cast<CEconItem*(__thiscall*)(void*, const char*)>(
			*reinterpret_cast<uintptr_t*>(Address + 1) + Address + 5
			);

	fnSetCustomDesc(this, name);
}