#pragma once
#include "../Render/DXRender.h"
#include "../Hacks/Setup.h"

//#define GUI_KEY_DOWN( KeyNum ) ( FastCall::G().t_GetAsyncKeyState(KeyNum) & 0x1 )
//#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

//X1Gui().CheckBox("checkbb", &cb_test);

//bool Rus = false;

#define DCheckBox(eng, rus, var, ops)  X1Gui().CheckBox(!MainSettings().Russian ? (eng) : (rus), &var); if (X1Gui().IsItemHovered()) { MainSettings().HintMsg = (ops); }
/*ImGui::Checkbox(!MainCheatConfig.RusLanguage ? (eng) : (rus) , &var); \
if (ImGui::IsItemHovered()) { GHintMsg = (ops); }*/

#define DComboBox(eng, rus, var, items_eng, items_rus, ops) X1Gui().ComboBox(!MainSettings().Russian ? (eng) : (rus), &var, true ? items_eng : items_rus); \
if (X1Gui().IsItemHovered()) { MainSettings().HintMsg = (ops); }
/* ImGui::Combo(!MainCheatConfig.RusLanguage ? (eng) : (rus), &var, !MainCheatConfig.RusLanguage ? items_eng : items_rus, IM_ARRAYSIZE(items_eng));\
if (ImGui::IsItemHovered()) { GHintMsg = (ops); }	*/

#define SliderInts(eng, rus, var,  min_, max_, ops)  X1Gui().SliderInt(!MainSettings().Russian ? (eng) : (rus), &var, min_, max_);\
if (X1Gui().IsItemHovered()) { MainSettings().HintMsg = (ops); }
/*ImGui::SliderInt(!MainCheatConfig.RusLanguage ? (eng) : (rus), &var, min, max);\
if (ImGui::IsItemHovered()) { GHintMsg = (ops); }*/	

#define SliderIntsProc(eng, rus, var,  min_, max_, ops) X1Gui().SliderIntProc(!MainSettings().Russian ? (eng) : (rus), &var, min_, max_); \
if (X1Gui().IsItemHovered()) { MainSettings().HintMsg = (ops); }
/* ImGui::SliderIntProc(!MainCheatConfig.RusLanguage ? (eng) : (rus), &var, min, max);\
if (ImGui::IsItemHovered()) { GHintMsg = (ops); }	*/


#define SliderFloats(eng, rus, var,  min_, max_, ops) X1Gui().SliderFloat(!MainSettings().Russian ? (eng) : (rus), &var, min_, max_); \
if (X1Gui().IsItemHovered()) { MainSettings().HintMsg = (ops); }
/*ImGui::SliderFloat(!MainCheatConfig.RusLanguage ? (eng) : (rus), &var, min, max);\
if (ImGui::IsItemHovered()) { GHintMsg = (ops); }	*/

#define HotsKey(eng, rus, var, ops) X1Gui().HotKey(!MainSettings().Russian ? (eng) : (rus), &var); \
if (X1Gui().IsItemHovered()) { MainSettings().HintMsg = (ops); }
/* ImGui::Hotkey(!MainCheatConfig.RusLanguage ? (eng) : (rus), &var); \
if (ImGui::IsItemHovered()) { GHintMsg = (ops); }	*/

#define DColorEdit(eng, rus, var) X1Gui().ColorEdit2(!MainSettings().Russian ? (eng) : (rus), &var); /*ImGui::MyColorEdit5(!MainCheatConfig.RusLanguage ? (eng) : (rus), var##._color, var##.brainbow, 1 << 7 )*/
#define TextEdit(eng, rus, var, buf, sim) strcpy(buf, var.c_str()); X1Gui().InputText(!MainSettings().Russian ? (eng) : (rus), buf, sim); var = buf; /*strcpy(buf, var.c_str()); ImGui::InputText(!MainCheatConfig.RusLanguage ? (eng) : (rus), buf, sim); var = buf;*/
#define TabsLabels(var, eng, rus, size, vertic) X1Gui().TabLabels(&var, size, !MainSettings().Russian ? eng : rus, vertic);  /*ImGui::SubTabLabels(!MainCheatConfig.RusLanguage ? eng : rus, sizeof(eng) / sizeof(eng[0]), var, order);*/

#define lols(e) string((e))
#define lolc(e) (e)
#define lolsc(e) string((e)).c_str()

class CGui;

CGui& X1Gui();
struct GuiStyle
{
	float scroll_x = 5;
	float wndRounding;
	float wndOutlineTick = 2.f;
	float Alpha = 255;
	Vec2  wndPadding;
	Vec2  itmPadding;
	Vec2  itmInPadding;

	bool AnimationEnable;
	bool ScrollEnable;

	color_t clrTmp;
	color_t clrBackground;
	color_t clrTitle;
	color_t clrText;
	color_t clrLine;
	color_t clrTabLabel;
	color_t clrTabLabelText;
	//color_t clrChildBackground;
	color_t clrFrame, clrFrameHover, clrFrameHold;
	color_t clrButton, clrButtonHover, clrButtonHold;
	color_t clrDisButton, clrDisButtonHover, clrDisButtonHold;
	color_t clrScroll, clrScrollHover, clrScrollHold;

	GuiStyle()
	{
		AnimationEnable = true;
		ScrollEnable = true;
		wndRounding = 2.f;
		wndPadding = Vec2(4, 4);
		itmPadding = Vec2(6, 3);
		itmInPadding = Vec2(3, 2);

		clrLine = color_t(120, 121, 123, 180);
		clrTmp = color_t(0, 0, 0, 0);
		//	clrChildBackground = color_t(18, 18, 22, 255);
		clrBackground = color_t(18, 18, 22, 255);
		clrTitle = color_t(112, 112, 116, 255);
		clrText = color_t(255, 255, 255, 255);

		clrFrame = color_t(34, 35, 37, 255);
		clrFrameHover = color_t(34 - 5, 35 - 5, 37 - 5, 255);
		clrFrameHold = color_t(34 - 9, 35 - 9, 37 - 9, 255);

		clrTabLabel = color_t(128, 135, 140, 120);
		clrTabLabelText = color_t(255, 255, 255, 160);

		clrButton = color_t(128, 135, 140, 255);
		clrButtonHover = color_t(128 - 10, 135 - 10, 140 - 10, 255);
		clrButtonHold = color_t(128 - 20, 135 - 20, 140 - 20, 255);

		clrDisButton = color_t(225, 0, 0, 255);
		clrDisButtonHover = color_t(225 - 10, 0, 0, 255);
		clrDisButtonHold = color_t(225 - 20, 0, 0, 255);

		clrScroll = color_t(128 - 20, 135 - 20, 140 - 20, 255);
		clrScrollHover = color_t(128 - 20, 135 - 20, 140 - 20, 255);
		clrScrollHold = color_t(128 - 20, 135 - 20, 140 - 20, 255);

	}
};

struct GuiIO
{
	Vec2 MousePos;
	bool LMPress = false;
	bool RMPress = false;

	bool LMClicked = false;
	float LMPressDur = 0;
	float MouseWheel = 0;

	bool BCKBPress = false;
	float BCKBDur = 0;
	bool BCKBClick = false;

	bool KeyCtrl = false;

	bool EnableMovWindow = true;
	string ActivWindowName = "";

	INT64 G_Time = 0;
	INT64 G_TicksInSec = 0;
	float G_Framerate = 0;
	float G_DeltaTime = 0;

	float	G_FramerateSecPerFrame[120];
	int		G_FramerateSecPerFrameIdx;
	float	G_FramerateSecPerFrameAccum;

	bool MouseDown[5];
	bool KeysClick[512];
	bool KeysDown[512];
	float KeysDownDuration[512];

	int StrlenW(const Wchar* str)
	{
		int n = 0;
		while (*str++) n++;
		return n;
	}

	Wchar InputCharacters[16 + 1];
	void AddInputCharacter(Wchar c)
	{
		const int n = StrlenW(InputCharacters);
		if (n + 1 < ARRAYSIZE(InputCharacters))
		{
			InputCharacters[n] = c;
			InputCharacters[n + 1] = '\0';
		}
	}

	GuiIO()
	{
		memset(G_FramerateSecPerFrame, 0, sizeof(G_FramerateSecPerFrame));
		memset(KeysDownDuration, 0, sizeof(KeysDownDuration));
	}
};

enum GuiWindowFlags
{
	GWF_Window = 0,
	GWF_NoTitleBar = 1 << 0,
	GWF_NoScrollbar = 1 << 1,
	GWF_ChildWindow = 1 << 2,
	GWF_Popup = 1 << 3,
	GWF_ComboBox = 1 << 4,
};

class CAnimController
{
private:

	void Set(bool back)
	{
		if (back)
		{
			current = 0;
			back_current = 100;
		}
		else
		{
			current = 100;
			back_current = 0;
		}
	}

	void Animator100Ex(float &Current, const float SpeedCor1, const float Delta);
	float current = 0;
	float back_current = 0;
public:

	CAnimController() {};
	CAnimController(bool first)
	{
		Set(first);
	}

	bool IsEnd()
	{
		return current == 100;
	}

	bool IsStart()
	{
		return current == 0;
	}

	void Reset()
	{
		Set(true);
	}

	float Get(float num, bool back = false)
	{
		if (back)
			return (num * ((100.f - current) / 100.f));
		else
			return (num * (current / 100.f));
	}

	void Update(bool check, float speed, float delta_speed)
	{
		if (check)
		{
			Animator100Ex(current, speed, delta_speed);
			back_current = 100.f - current;
		}
		else
		{
			Animator100Ex(back_current, speed, delta_speed);
			current = 100.f - back_current;
		}
	}
};

class IGui
{
private:
	virtual float RoundScalar(float value, int decimal_precision) = 0;
	virtual int ParseFormatPrecision(const char* fmt, int default_precision) = 0;
	virtual ColU32 IdHash(const void* data, int data_size, ColU32 seed) = 0;
	virtual DWORD GetItemId(string data) = 0;
	virtual bool CheckSkipEx(Rect owner, Rect rect) = 0;
	virtual bool CheckSkip(Rect rect) = 0;
	virtual bool BeginIntenrnalWin(string Label, GuiWindowFlags Flags, Vec2 Size, Vec2 Pos, color_t col, bool Scroll = true) = 0;
	virtual void EndIntenrnalWin() = 0;
	virtual void OpenPopup(string popup) = 0;
	virtual bool IsPopupOpen(string popup) = 0;
	virtual bool IsMouseHoverEx(Vec2 Min, Vec2 Max) = 0;
	virtual bool IsMouseHover(Vec2 Min, Vec2 Max) = 0;
	virtual Rect CompareClipRect(Rect Owner, Rect Child) = 0;
	virtual void VerticalScroll() = 0;
	virtual void ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v) = 0;
	virtual string FormatLabel(string str) = 0;
public:
	virtual void SetFont(CFont* pFont) = 0;
	virtual CFont* GetFont() = 0;
	virtual void ShowMetrics() = 0;
	virtual Vec2 GetCursorPos() = 0;
	virtual Vec2 GetCursorPosEx() = 0;
	virtual void SetCursorPos(Vec2 Pos, float width) = 0;
	virtual Vec2 GetPrevWindowPos() = 0;
	virtual void SetNextWindowPos(Vec2 Pos) = 0;
	virtual Vec2 GetCurWindowSize() = 0;
	virtual Vec2 GetCurWindowPos() = 0;
	virtual CGui* GetThis() = 0;
	virtual COverManager* GetOverlay() = 0;
	virtual GuiIO& GetIO() = 0;
	virtual GuiStyle& GetStyle() = 0;
	virtual bool Begin(string Name, GuiWindowFlags Flags = GWF_Window, Vec2 Size = Vec2(400, 1000), Vec2 Pos = Vec2(50, 50), CGui* old = nullptr, color_t* back = nullptr) = 0;
	virtual void End() = 0;
	virtual bool ListBoxHeader(string Label, Vec2 Size = Vec2()) = 0;
	virtual void ListBoxFooter() = 0;
	virtual void TextFmt(Vec2 Pos, const char* label, ...) = 0;
	virtual void Text(Vec2 Pos, const char* label) = 0;
	virtual void Text(const char* label, ...) = 0;
	virtual void TextColored(color_t color, Vec2 Pos, const char* label) = 0;
	virtual bool ButtonBehavior(Rect Rect, bool &Hovered) = 0;
	virtual bool Button(string Label, Vec2 Size = Vec2()) = 0;
	virtual bool CheckBox(string Label, bool *var) = 0;
	virtual bool SliderFloat(string Label, float *var, float min, float max, const char* display_format = "%6.3lf") = 0;
	virtual bool SliderInt(string Label, int *var, int min, int max) = 0;
	virtual bool SliderIntProc(string Label, int *var, int min, int max) = 0;
	virtual bool ComboBox(string Label, int *var, VectorEx<const char*> &items) = 0;
	virtual bool SelectLabel(string Label, bool Selected, Vec2 Size = Vec2()) = 0;
	virtual bool ColorEdit(string Label, float *var, Vec2 Size = Vec2()) = 0;
	virtual bool ColorEdit(string Label, SDK::Color *var, Vec2 Size = Vec2()) = 0;
	virtual bool ColorEdit2(string Label, SDK::Color *var, Vec2 Size = Vec2()) = 0;
	virtual bool ColorPicker(string Label, float *var) = 0;
	virtual bool ColorPicker(string Label, SDK::Color *var) = 0;
	virtual bool InputText(string Label, char *Buf, size_t BufSize) = 0;
	virtual void TabLabels(int *var, Vec2 Size, VectorEx<const char*> &items, bool vertical = false) = 0;
	virtual void BeginGroup() = 0;
	virtual void EndGroup(bool NewLine = false) = 0;
	virtual bool BeginChild(string Label, Vec2 Size = Vec2(0, 0), bool Outline = false) = 0;
	virtual void EndChild() = 0;
	virtual float *ColorToArr(SDK::Color &col) = 0;
	virtual void SameLine(float len = 0) = 0;
	virtual void Separator() = 0;
	virtual void Spacing() = 0;
	virtual void PushItemWidth(float Width) = 0;
	virtual void ResetOverlayGui(CGui* pGui) = 0;
	virtual void ReleaseGui(CGui* pGui) = 0;
	virtual void Update() = 0;
	virtual void Cleanup() = 0;
	virtual void Start() = 0;
	virtual bool WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam, bool DefProc) = 0;
	virtual void DrawFilledBox(Rect rect, color_t color, float rounding) = 0;
	virtual void DrawFilledBox(Vec2 a, Vec2 b, color_t color, float rounding = 0) = 0;
	virtual void DrawBox(Rect rect, int16_t thickness, color_t color, float rounding) = 0;
	virtual void DrawBox(Vec2 a, Vec2 b, color_t color, float rounding = 0) = 0;
	virtual void DrawBox(int16_t x, int16_t y, int16_t width, int16_t height, int16_t thickness, color_t color, float rounding) = 0;
	virtual void DrawLine(Vec2 a, Vec2 b, color_t color, float thickness = 1.f) = 0;
	virtual void DrawGradientBox(int16_t x, int16_t y, int16_t width, int16_t height, color_t color1, color_t color2, color_t color3, color_t color4) = 0;
	virtual void DrawCircle(int16_t x, int16_t y, int16_t radius, uint16_t points, color_t color) = 0;
	virtual void DrawRing(int16_t x, int16_t y, int16_t radius, uint16_t points, color_t color, float thickness) = 0;
	virtual Vec2 CalcTextSize(string text) = 0;
	virtual void DrawString(int16_t x, int16_t y, color_t color, bool outlined, bool centered, const char* text) = 0;
	virtual void DrawString(CFont* font, int16_t x, int16_t y, color_t color, const char* text, ...) = 0;
	virtual void DrawImage(LPDIRECT3DTEXTURE9 _texture, Vec2 _a, Vec2 _b) = 0;
};

class CGui :  public IGui
{
private:
	struct GuiInfo
	{
		string  Name;
		Vec2	Pos;
		Vec2	Size;
		Rect	ClipRect;
		Rect	ItemsClipRect;
		GuiWindowFlags Flags;
		GuiInfo()
		{
			Flags = GWF_Window;
			Name = "";
			ClipRect = Rect();
			Pos = Vec2(200, 200);
			Size = Vec2(100, 100);
		}
	};

	struct Group
	{
		Vec2 oldPos;
		Vec2 newPosEx;
		GuiStyle oldStyle;
	};


	map<string, CGui> ChildList;
	map<string, CGui> PopupList;

	map<string, CGui> G_WindowList;
	map<DWORD, CAnimController> G_AnimList;
	CFont*			  GP_Font;
	GuiStyle          G_Style;
	GuiIO             G_IO;
	Vec2              G_NextWindowPos;
	Vec2              G_PrevWindowPos;
	VectorEx<CGui*>   G_PopupDrawStack;

	CGui* pThis;
	CGui* oldThis;
	CGui* childThis;

	COverManager Overlay;

	float ScrollUpOfst;
	float ScrollUpKoeff;
	
	string OpenedPopup;
	
	Vec2 CursorPos;
	Vec2 PrevCursorPos;

	bool MovThisWindow;

	int ItemNumber;
	VectorEx<DWORD> ItemIdStack;
	DWORD ItemsId;
	DWORD PrevItemId;
	DWORD ActivItemId;
	DWORD UsingItemId;

	struct Column
	{
		int idx = 0;
		float offset_x = 0;
		CGui* pColumn = nullptr;
	};

	VectorEx<Column> ColumnStack;

	map<DWORD, bool> TreeList;
	bool& GetTreeState(DWORD id, bool first)
	{
		if (!pThis->TreeList.count(id))
			pThis->TreeList.insert(pair<DWORD, bool>(id, first));
		return pThis->TreeList[id];
	}

	float ItemWidth;
	float Title_y;

	virtual float RoundScalar(float value, int decimal_precision);
	virtual int ParseFormatPrecision(const char* fmt, int default_precision);
	virtual ColU32 IdHash(const void* data, int data_size, ColU32 seed);
	virtual DWORD GetItemId(string data);
	virtual bool CheckSkipEx(Rect owner, Rect rect);
	virtual bool CheckSkip(Rect rect);
	virtual bool BeginIntenrnalWin(string Label, GuiWindowFlags Flags, Vec2 Size, Vec2 Pos, color_t col, bool Scroll = true);
	virtual void EndIntenrnalWin();
	virtual void OpenPopup(string popup);
	virtual bool IsPopupOpen(string popup);
	virtual bool IsMouseHoverEx(Vec2 Min, Vec2 Max);
	virtual bool IsMouseHover(Vec2 Min, Vec2 Max);
	virtual Rect CompareClipRect(Rect Owner, Rect Child);
	virtual void VerticalScroll();
	virtual void ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v);
	virtual string FormatLabel(string str);

	VectorEx<Group> GroupStack;

	bool PrevChildOutlined = false;

	bool ClosePopup = false;

	map<DWORD, CAnimController>& GetAnimList()
	{
		return X1Gui().G_AnimList;
	}

	CAnimController* GetAnim(DWORD id, bool first)
	{
		if (!GetAnimList().count(id))
			GetAnimList().insert(pair<DWORD, CAnimController>(id, CAnimController(first)));

		return &GetAnimList()[id];
	}

	bool VScrollEnable;
	bool LastItemHovered;
	bool LastItemClicked;

public:

	bool G_Enable3D;

	GuiInfo	Info;

	CGui()
	{
		G_Enable3D = false;
		LastItemHovered = false;
		LastItemClicked = false;
		VScrollEnable = true;
		OpenedPopup = "";
		UsingItemId = -1;
		ScrollUpKoeff = 0;
		ScrollUpOfst = 0;
		pThis = nullptr;
		oldThis = nullptr;
		ItemWidth = 0;
		ActivItemId = -1;
		PrevItemId = -1;
		ItemNumber = -1;
		ItemsId = 0;
		MovThisWindow = false;
		CursorPos = Vec2(0, 0);
		PrevCursorPos = Vec2(0, 0);
		G_NextWindowPos = Vec2(0, 0);
		G_PrevWindowPos = Vec2(0, 0);
		G_Style = GuiStyle();
		Info = GuiInfo();
		G_WindowList.clear();
	}
	~CGui()
	{

	}

	virtual void SetFont(CFont* pFont);
	virtual CFont* GetFont();
	virtual void ShowMetrics();

	virtual Vec2 GetCursorPos();
	virtual Vec2 GetCursorPosEx();
	virtual void SetCursorPos(Vec2 Pos, float width);
	virtual Vec2 GetPrevWindowPos();
	virtual void SetNextWindowPos(Vec2 Pos);
	virtual Vec2 GetCurWindowSize();
	virtual Vec2 GetCurWindowPos();
	virtual CGui* GetThis();
	virtual COverManager* GetOverlay();

	bool IsItemHovered()
	{
		return pThis->LastItemHovered;
	}

	bool IsItemClicked()
	{
		return pThis->LastItemClicked;
	}

	virtual GuiIO& GetIO();
	virtual GuiStyle& GetStyle();

	virtual bool Begin(string Name, GuiWindowFlags Flags = GWF_Window, Vec2 Size = Vec2(400, 1000), Vec2 Pos = Vec2(50, 50), CGui* old = nullptr, color_t* back = nullptr);
	virtual void End();

	virtual bool ListBoxHeader(string Label, Vec2 Size = Vec2());
	virtual void ListBoxFooter();

	virtual void TextFmt(Vec2 Pos, const char* label, ...);
	virtual void Text(Vec2 Pos, const char* label);
	virtual void TextColored(color_t color, Vec2 Pos, const char* label);
	virtual void Text(const char* label, ...);
	virtual bool ButtonBehavior(Rect Rect, bool &Hovered);
	virtual bool Button(string Label, Vec2 Size = Vec2());
	virtual bool CheckBox(string Label, bool *var);
	virtual bool SliderFloat(string Label, float *var, float min, float max, const char* display_format = "%6.3lf");
	virtual bool SliderInt(string Label, int *var, int min, int max);
	virtual bool SliderIntProc(string Label, int *var, int min, int max);
	virtual bool ComboBox(string Label, int *var, VectorEx<const char*> &items);
	virtual bool SelectLabel(string Label, bool Selected, Vec2 Size = Vec2());
	virtual bool ColorEdit(string Label, float *var, Vec2 Size = Vec2());
	virtual bool ColorEdit(string Label, SDK::Color *var, Vec2 Size = Vec2());
	virtual bool ColorEdit2(string Label, SDK::Color *var, Vec2 Size = Vec2());
	virtual bool ColorPicker(string Label, float *var);
	virtual bool ColorPicker(string Label, SDK::Color *var);
	virtual bool InputText(string Label, char *Buf, size_t BufSize);
	virtual void TabLabels(int *var, Vec2 Size, VectorEx<const char*> &items, bool vertical = false);
	bool HotKey(string Label, int *var);

	void BeginColumns(string Label, int count);
	void EndColumns();
	void NextColumn();

	virtual void BeginGroup();
	virtual void EndGroup(bool NewLine = false);

	virtual bool BeginChild(string Label, Vec2 Size = Vec2(0, 0), bool Outline = false);
	virtual void EndChild();

	bool BeginTreeNode(string Label, Vec2 Size, bool first);

	virtual float *ColorToArr(SDK::Color &col);

	virtual void SameLine(float len = 0);
	void AddLineWidth(float len = 0);
	virtual void Separator();
	void VerticalSeparator(float ofst);
	virtual void Spacing();
	virtual void PushItemWidth(float Width);

	virtual void ResetOverlayGui(CGui* pGui);
	virtual void ReleaseGui(CGui* pGui);

	virtual void Update();
	virtual void Cleanup();
	virtual void Start();

	virtual bool WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam, bool DefProc);

	virtual void DrawFilledBox(Rect rect, color_t color, float rounding);
	virtual void DrawFilledBox(Vec2 a, Vec2 b, color_t color, float rounding = 0);
	void DrawFilledBoxEx(Vec2 a, Vec2 b, color_t color, float rounding = 0);
	virtual void DrawBox(Rect rect, int16_t thickness, color_t color, float rounding);
	virtual void DrawBox(Vec2 a, Vec2 b, color_t color, float rounding = 0);
	virtual void DrawBox(int16_t x, int16_t y, int16_t width, int16_t height, int16_t thickness, color_t color, float rounding);
	virtual void DrawLine(Vec2 a, Vec2 b, color_t color, float thickness = 1.f);
	virtual void DrawGradientBox(int16_t x, int16_t y, int16_t width, int16_t height, color_t color1, color_t color2, color_t color3, color_t color4);
	virtual void DrawCircle(int16_t x, int16_t y, int16_t radius, uint16_t points, color_t color);
	virtual void DrawRing(int16_t x, int16_t y, int16_t radius, uint16_t points, color_t color, float thickness);
	virtual Vec2 CalcTextSize(string text);
	virtual void DrawString(int16_t x, int16_t y, color_t color, bool outlined, bool centered, const char* text);
	virtual void DrawString(CFont* font, int16_t x, int16_t y, color_t color, const char* text, ...);
	virtual void DrawImage(LPDIRECT3DTEXTURE9 _texture, Vec2 _a, Vec2 _b);
	void DrawImage(LPDIRECT3DTEXTURE9 _texture, Vec2 _a, Vec2 _b, Vec2 _c, Vec2 _d);

	static CGui& Singleton()
	{
		static CGui pObject;
		return pObject;
	}
};

class Message
{
private:
	bool Enable = false;

	float OfstLayWhite = 0;
	float OfstLayBlack = 0;
	int NumTicks = 100000;

	Vec2 MsgTextSize = Vec2(0, 0);

	string MessageText;
public:

	Message()
	{
		MessageText = "";
	}

	void Start(string Msg)
	{
		MessageText = Msg;


		MsgTextSize = GP_Render->CalcTextSize(MessageText.c_str(), GP_Render->SzFonts[20]);

		OfstLayWhite = 0;
		OfstLayBlack = 0;
		NumTicks = 0;

		Enable = true;
	}

#define MSG_SPEED_X 0.0999f

#define MSG_SIZE_WHITE_X 200
#define MSG_SIZE_BLACK_X 193

#define MSG_SIZE_Y 55
	void Show()
	{
		if (Enable)
		{
			int posX = CGlobal::iScreenWidth;
			int posY = 50;

			float DeltaSpeed = 15;

			if (NumTicks < 10000)
				NumTicks += (DeltaSpeed > 1000 ? 50 : DeltaSpeed);
			else
				Enable = false;

			if (NumTicks < 1900)
			{
				if (OfstLayWhite < MSG_SIZE_WHITE_X)
					OfstLayWhite += (MSG_SIZE_WHITE_X - OfstLayWhite) * MSG_SPEED_X;

				if (OfstLayWhite > MSG_SIZE_WHITE_X)
					OfstLayWhite = MSG_SIZE_WHITE_X;

				if (OfstLayWhite > MSG_SIZE_WHITE_X / 1.09f)
					if (OfstLayBlack < MSG_SIZE_BLACK_X)
						OfstLayBlack += (MSG_SIZE_BLACK_X - OfstLayBlack) * MSG_SPEED_X;

				if (OfstLayBlack > MSG_SIZE_BLACK_X)
					OfstLayBlack = MSG_SIZE_BLACK_X;
			}
			else
			{
				if (OfstLayBlack > 0)
					OfstLayBlack -= (((MSG_SIZE_BLACK_X)-OfstLayBlack) * MSG_SPEED_X) + 0.01f;

				if (OfstLayBlack < (MSG_SIZE_BLACK_X - 5.f) && OfstLayWhite >= 0)
					OfstLayWhite -= ((MSG_SIZE_WHITE_X - OfstLayWhite) * MSG_SPEED_X) + 0.01f;
			}

			GP_Render->DrawFilledBox(int(posX - OfstLayWhite), posY, int(OfstLayWhite) + 3, MSG_SIZE_Y, Color::White());
			GP_Render->DrawFilledBox(int(posX - OfstLayBlack), posY, int(OfstLayBlack) + 3, MSG_SIZE_Y, Color::Black());

			Vec2 TPos = Vec2(0, 0);

			TPos.x = (posX - OfstLayBlack) + ((MSG_SIZE_BLACK_X / 2) - (MsgTextSize.x / 2));
			TPos.y = posY + (MSG_SIZE_Y / 2) - (MsgTextSize.y / 2);

			GP_Render->DrawString(20, TPos, Color::White(), MessageText.c_str());
		}
	}

	static Message& Get()
	{
		static Message obj;
		return obj;
	}
};



