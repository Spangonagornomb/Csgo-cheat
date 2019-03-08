#pragma once

#define MULTIHACK_VERSION
#define PRESENT_ENABLE

#define MAX_FONT_SIZE 30
#define ENABLE_HOOK_CS_WND_PROC

#ifdef MULTIHACK_VERSION
#define CHEAT_HEAD ("obnoxious framework")
#define CHEAT_TYPE "multihack"
#endif

#define PATH_TO_CHEAT_SETTINGS "\\obnoxious framework\\"

#define MAX_ENTITY_PLAYERS 64

#ifdef ENABLE_VMP

#else

#endif 

#ifdef ENABLE_CONSOLE_LOG
#define ADD_LOG(m, ...) printf(m, __VA_ARGS__)
#else
#define ADD_LOG(m, ...)
#endif

#define VGUI2_DLL ("vgui2.dll")
#define VGUIMAT_DLL	("vguimatsurface.dll")
#define D3D9_DLL ("d3d9.dll")
#define SHADERPIDX9_DLL ("shaderapidx9.dll")
#define GAMEOVERLAYRENDERER_DLL ("GameOverlayRenderer.dll")
#define D3D9_MASK ("x????xxxxxx")
#define GMOR_MASK ("x????x????xxxx????xxxxxxx")

#define D3D9_PATTERN ("\xA1\x00\x00\x00\x00\x50\x8B\x08\xFF\x51\x0C")
#define FORCE_FULL_UPDATE_PATTERN ("A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85")
#define GMOR_PATTERN ("\xA3\x00\x00\x00\x00\x68\x00\x00\x00\x00\xFF\x76\x54\xE8\x00\x00\x00\x00\x83\xC4\x08\x84\xC0\x75\x17")
#define SMOK_PATTERN ("\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0")

#define KEY_VALUES_MASK  ("55 8B EC 51 33 C0 C7 45")
#define KEY_VALUES_LOAD_FROM_BUFFER_MASK  ("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04")

#define DELETE_PTR(name) delete name; name = nullptr

#define EX_ARRAYSIZE(_ARR)      ((int)(sizeof(_ARR)/sizeof(*_ARR)))
#define EX_PI                   3.14159265358979323846f
#define EX_OFFSETOF(_TYPE,_ELM) ((size_t)&(((_TYPE*)0)->_ELM))