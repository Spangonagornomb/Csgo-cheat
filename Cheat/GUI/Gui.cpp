#include "Gui.h"

#define CHECKBOX_SPEED_DELTA  590.f
#define CHECKBOX_SPEED  0.002009f

#define COMBOBOX_SPEED_DELTA  530.f
#define COMBOBOX_SPEED  0.004109f

CGui& X1Gui() {
	return CGui::Singleton();
}

bool CGui::Begin(string Name, GuiWindowFlags Flags, Vec2 Size, Vec2 Pos, CGui* old, color_t* back)
{
	if (X1Gui().G_Enable3D)
		Name += "##3D";

	map<string, CGui>* cur_map = &X1Gui().G_WindowList;

	if (Flags & GWF_ChildWindow)
		cur_map = &pThis->ChildList;

	if (Flags & GWF_Popup)
		cur_map = &pThis->PopupList;

	if (cur_map->count(Name) == 0)
	{
		cur_map->insert(pair<string, CGui>(Name, CGui()));
		pThis = &cur_map->find(Name)->second;
		pThis->Info.Pos = Pos;
	}
	else
		pThis = &cur_map->find(Name)->second;

	pThis->oldThis = old;
	pThis->Info.Name = Name;
	pThis->Info.Size = Size;
	pThis->Info.Flags = Flags;
	pThis->childThis = nullptr;

	if (X1Gui().G_NextWindowPos.x != 0 && X1Gui().G_NextWindowPos.y != 0)
	{
		pThis->Info.Pos = G_NextWindowPos;
	}

	if (pThis->Info.Flags & GWF_ChildWindow || pThis->Info.Flags & GWF_Popup)
		pThis->Info.Pos = Pos;

	Rect WindowRect(Vec2(pThis->Info.Pos.x, pThis->Info.Pos.y),
		Vec2(pThis->Info.Pos.x + pThis->Info.Size.x, pThis->Info.Pos.y + pThis->Info.Size.y));

	if ((pThis->Info.Flags & GWF_ChildWindow) && pThis->oldThis)
		WindowRect = CompareClipRect(pThis->oldThis->Info.ItemsClipRect, WindowRect);
	static Vec2 MouseOffset;

	GuiIO &IO = X1Gui().G_IO;

	bool IsHover = IsMouseHover(WindowRect.min, WindowRect.max) || (IO.ActivWindowName == Name && IO.LMPress);

	if(IsHover)
		IO.ActivWindowName = Name;
	else if(!IsHover && IO.ActivWindowName == Name)
		IO.ActivWindowName = "";

	if ((X1Gui().G_NextWindowPos.x == 0 &&
		X1Gui().G_NextWindowPos.y == 0) &&
		!(pThis->Info.Flags & GWF_ChildWindow) &&
		!(pThis->Info.Flags & GWF_Popup) &&
		pThis->ActivItemId == -1 &&
		IO.EnableMovWindow && IO.LMPress && IsHover)
	{
		if (!pThis->MovThisWindow)
			MouseOffset = IO.MousePos - pThis->Info.Pos;
		pThis->MovThisWindow = true;
	}
	else
	{
		if (pThis->MovThisWindow)
			MouseOffset = Vec2(-1, -1);
		pThis->MovThisWindow = false;		
	}

	if (pThis->MovThisWindow)
		pThis->Info.Pos = IO.MousePos - MouseOffset;

	WindowRect = Rect(Vec2(pThis->Info.Pos.x, pThis->Info.Pos.y),
		Vec2(pThis->Info.Pos.x + pThis->Info.Size.x, pThis->Info.Pos.y + pThis->Info.Size.y));

	if ((pThis->Info.Flags & GWF_ChildWindow) && pThis->oldThis)
		WindowRect = CompareClipRect(pThis->oldThis->Info.ItemsClipRect, WindowRect);

	if (pThis->oldThis && CheckSkipEx(pThis->oldThis->Info.ItemsClipRect, WindowRect))
		return false;

	pThis->Overlay.ResetCount();

	GP_Render->SetFont(X1Gui().GP_Font);

	pThis->Info.ClipRect = WindowRect;

	pThis->Overlay.SetClipRect(Vec4(WindowRect.min, WindowRect.max));

	color_t back_color = X1Gui().G_Style.clrBackground;

	if (back)
		back_color = *back;

	DrawFilledBox(WindowRect, back_color, X1Gui().G_Style.wndRounding);

	pThis->Title_y = 0;
	Vec2 Padding = X1Gui().G_Style.wndPadding;
	if (pThis->Info.Flags & GWF_Popup)
		Padding = X1Gui().G_Style.itmInPadding;

	if (!(pThis->Info.Flags & GWF_NoTitleBar) && !(pThis->Info.Flags & GWF_ChildWindow) && !(pThis->Info.Flags & GWF_Popup))
	{
		Vec2 name_size = CalcTextSize(Name);
		Vec2 name_pos = GetCursorPos();
		name_pos.x += 5;

		Rect title_rect = Rect(pThis->Info.Pos, Vec2(0,0));
		title_rect.max = title_rect.min + Vec2(pThis->Info.Size.x, name_size.y + 2);

		DrawFilledBox(title_rect, X1Gui().G_Style.clrTitle, X1Gui().G_Style.wndRounding);

		pThis->Title_y = name_size.y + 2;

		Text(name_pos, FormatLabel(Name).c_str());

		SetCursorPos(Vec2(Padding.x, Padding.y + name_size.y + 3 - (pThis->ScrollUpOfst * pThis->ScrollUpKoeff)), Padding.x);
	
		pThis->Info.ItemsClipRect = Rect(Vec2(WindowRect.min.x + Padding.x, WindowRect.min.y + name_size.y + 2 + Padding.y),
			Vec2(WindowRect.max.x - Padding.x, WindowRect.max.y - Padding.y));

		pThis->Overlay.SetNewClipRect(Vec4(pThis->Info.ItemsClipRect.min, pThis->Info.ItemsClipRect.max));		
	}
	else
	{
		SetCursorPos(Vec2(Padding.x, Padding.y - (pThis->ScrollUpOfst * pThis->ScrollUpKoeff)), Padding.x);

		pThis->Info.ItemsClipRect = Rect(Vec2(WindowRect.min.x + Padding.x, WindowRect.min.y + Padding.y),
			Vec2(WindowRect.max.x - Padding.x, WindowRect.max.y - Padding.y));

		if (!(pThis->Info.Flags & GWF_Popup))
			pThis->Overlay.SetNewClipRect(Vec4(pThis->Info.ItemsClipRect.min, pThis->Info.ItemsClipRect.max));
	}

	X1Gui().G_NextWindowPos = Vec2();
	return true;
}

void CGui::End()
{
	EndColumns();

	Vec2 CursorPos = GetCursorPos();

	if ((CursorPos.y + (pThis->ScrollUpOfst * pThis->ScrollUpKoeff)) > pThis->Info.ClipRect.max.y &&
		pThis->VScrollEnable && GetStyle().ScrollEnable)
		VerticalScroll();
	else
		pThis->ScrollUpOfst = 0;

	if(!pThis->ItemIdStack.empty() && !pThis->ItemIdStack.find(pThis->ActivItemId))
		pThis->ActivItemId = -1;

	pThis->ItemIdStack.clear();

	pThis->ItemWidth = 0;
	pThis->ItemsId = 0;
	pThis->PrevItemId = -1;
	pThis->ItemNumber = -1;

	X1Gui().G_PrevWindowPos = pThis->Info.Pos;

	SetCursorPos(Vec2(0,0),0);
}

void CGui::VerticalScroll()
{
	int item_id = GetItemId(("SCROLLY"));
	GuiIO &IO = X1Gui().G_IO;
	GuiStyle &style = X1Gui().G_Style;
	Vec2 CursorPos = GetCursorPosEx();

	Rect new_clip_scroll = pThis->Info.ClipRect;
	new_clip_scroll.min.y += pThis->Title_y;

	pThis->Overlay.SetNewClipRect(Vec4(new_clip_scroll.min, new_clip_scroll.max));

	float title_fix = pThis->Title_y;

	float cursor_pos = CursorPos.y + (pThis->ScrollUpOfst * pThis->ScrollUpKoeff) - title_fix;
	float work_size = pThis->Info.Size.y - pThis->Title_y;

	float view_zone_prc = (work_size / cursor_pos) * 100;
	float max_scroll_ofst = cursor_pos - work_size + style.wndPadding.y;
	float scroll_size_y = (work_size * (view_zone_prc / 100));
	Vec2 scroll_size = Vec2(style.scroll_x, scroll_size_y);

	float max_scroll_ofst_screen = work_size - scroll_size_y;

	float scroll_ofst_y = pThis->ScrollUpOfst;
	float scroll_size_screen = max_scroll_ofst_screen * (((scroll_ofst_y / max_scroll_ofst) * 100) / 100);

	pThis->ScrollUpKoeff = 1;

	//if (pThis->ScrollUpOfst > max_scroll_ofst)
	//	pThis->ScrollUpOfst -= 2;

	Rect scroll_rect = Rect(Vec2(pThis->Info.Pos.x + pThis->Info.Size.x - scroll_size.x - 1, pThis->Info.Pos.y + title_fix + scroll_size_screen),
		Vec2(pThis->Info.Pos.x + pThis->Info.Size.x - 1, pThis->Info.Pos.y + scroll_size.y + title_fix + scroll_size_screen));

	color_t scroll_color = style.clrScrollHover;

	/*bool hovered = false;
	bool press = IO.LMPress;
	ButtonBehavior(scroll_rect, hovered);

	if (hovered)
		scroll_color = style.clrScrollHover;

	static bool set_mouse_ofst = false;
	static Vec2 mouse_ofst;*/

	//if (hovered && press)
	//{
	//	scroll_color = style.clrScrollHold;
	//	if (!set_mouse_ofst)
	//	{
	//		mouse_ofst = IO.MousePos - scroll_rect.min;
	//		set_mouse_ofst = true;
	//	}
	//	pThis->ScrollUpOfst = (IO.MousePos.y - mouse_ofst.y) - (pThis->Info.Pos.y + title_fix);
	//}

	//if ((pThis->ActivItemId == item_id || pThis->ActivItemId == -1) && !press)
	//	set_mouse_ofst = false;
	
	if (X1Gui().G_IO.ActivWindowName == pThis->Info.Name /*&& !press*/)
		pThis->ScrollUpOfst -= (IO.MouseWheel * (1 / IO.G_DeltaTime));

	if (pThis->ScrollUpOfst < 0)
		pThis->ScrollUpOfst = 0;
	if (pThis->ScrollUpOfst > max_scroll_ofst)
		pThis->ScrollUpOfst = max_scroll_ofst-1;

	DrawFilledBox(scroll_rect, scroll_color, style.wndRounding);
}

Rect CGui::CompareClipRect(Rect Owner, Rect Child)
{
	if (Owner.min.x > Child.min.x)
		Child.min.x = Owner.min.x;

	if (Owner.min.y > Child.min.y)
		Child.min.y = Owner.min.y;

	if (Owner.max.x < Child.max.x)
		Child.max.x = Owner.max.x;

	if (Owner.max.y < Child.max.y)
		Child.max.y = Owner.max.y;
	return Child;
}

void CGui::ResetOverlayGui(CGui* pGui)
{
	pGui->Overlay.ResetCount();

	for (auto child_it = pGui->ChildList.begin(); child_it != pGui->ChildList.end(); ++child_it)
		ResetOverlayGui(&child_it->second);

	for (auto popup_it = pGui->PopupList.begin(); popup_it != pGui->PopupList.end(); ++popup_it)
		ResetOverlayGui(&popup_it->second);
}

void CGui::Update()
{
	GuiIO &IO = X1Gui().G_IO;

	if (IO.BCKBPress && IO.BCKBDur == 0)
		IO.BCKBClick = true;
	else
		IO.BCKBClick = false;

	if (IO.BCKBPress)
		IO.BCKBDur++;
	else
		IO.BCKBDur = 0;

	if (/*FastCall::G().t_*/GetAsyncKeyState(VK_BACK))
		IO.BCKBPress = true;
	else
		IO.BCKBPress = false;

	if (IO.LMPress && IO.LMPressDur == 0)
		IO.LMClicked = true;
	else
		IO.LMClicked = false;

	if (IO.LMPress)
		IO.LMPressDur += 0.2f;
	else
		IO.LMPressDur = 0;

	IO.KeyCtrl = (FastCall::G().t_GetKeyState(VK_CONTROL) & 0x8000) != 0;

	for (int i = 0; i < 512; i++)
	{
		if (IO.KeysDown[i])
		{
			IO.KeysClick[i] = IO.KeysDownDuration[i] == 0;
			IO.KeysDownDuration[i] += 0.5f;
		}
		else
		{
			IO.KeysDownDuration[i] = 0;
			IO.KeysClick[i] = false;
		}
	}

	INT64 current_time;
	QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
	IO.G_DeltaTime = (float)(current_time - IO.G_Time) / IO.G_TicksInSec;
	IO.G_Time = current_time;

	IO.G_FramerateSecPerFrameAccum += IO.G_DeltaTime - IO.G_FramerateSecPerFrame[IO.G_FramerateSecPerFrameIdx];
	IO.G_FramerateSecPerFrame[IO.G_FramerateSecPerFrameIdx] = IO.G_DeltaTime;
	IO.G_FramerateSecPerFrameIdx = (IO.G_FramerateSecPerFrameIdx + 1) % 120;
	IO.G_Framerate = 1.0f / (IO.G_FramerateSecPerFrameAccum / (float)120);


	for (auto it = X1Gui().G_WindowList.begin(); it != X1Gui().G_WindowList.end(); ++it)
		ResetOverlayGui(&it->second);

	X1Gui().G_PopupDrawStack.clear();

	X1Gui().ClosePopup = false;
}

bool CGui::WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam, bool DefProc)
{
	GuiIO &IO = X1Gui().G_IO;

	//LMPress
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		IO.MouseDown[0] = true;
		IO.LMPress = true;
		return !DefProc ? true : false;
	case WM_LBUTTONUP:
		IO.MouseDown[0] = false;
		IO.LMPress = false;
		return !DefProc ? true : false;
	case WM_RBUTTONDOWN:
		IO.MouseDown[1] = true;
		return !DefProc ? true : false;
	case WM_RBUTTONUP:
		IO.MouseDown[1] = false;
		return !DefProc ? true : false;
	case WM_MBUTTONDOWN:
		IO.MouseDown[2] = true;
		return !DefProc ? true : false;
	case WM_MBUTTONUP:
		IO.MouseDown[2] = false;
	case WM_MOUSEMOVE:
		IO.MousePos.x = (signed short)(lParam);
		IO.MousePos.y = (signed short)(lParam >> 16);
		return !DefProc ? true : false;
	case WM_MOUSEWHEEL:
		IO.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return !DefProc ? true : false;
	case WM_KEYDOWN:
		if (wParam < 256)
			IO.KeysDown[wParam] = 1;
		return !DefProc ? true : false;
	case WM_KEYUP:
		if (wParam < 256)
			IO.KeysDown[wParam] = 0;
		return !DefProc ? true : false;
	case WM_CHAR:
		if (wParam > 0 && wParam < 0x10000)
		{
			wchar_t wch;
			FastCall::G().t_MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char*)&wParam, 1, &wch, 1);
			IO.AddInputCharacter(wch);
			
		}
		return !DefProc ? true : false;
	}
	return false;
}

//void ConvertPosWindow(ImGuiWindow* window)
//{
//	ImDrawVert* vtx_src = window->DrawList->VtxBuffer.Data;
//	for (int cmd_i = 0; cmd_i < window->DrawList->CmdBuffer.Size; cmd_i++)
//		window->DrawList->CmdBuffer[cmd_i].ClipRect = ImVec4(0, 0, iScreenWidth, iScreenHeight);
//	for (int i = 0; i < window->DrawList->VtxBuffer.Size; i++)
//	{
//		Vector pos_screen;
//		if (ImGui3DMenu.W2S(ImGui3DMenu.CalcPoint(Vector(vtx_src->pos.x / 10, vtx_src->pos.y / 10, 0)), pos_screen))
//		{
//			vtx_src->pos.x = pos_screen.x;
//			vtx_src->pos.y = pos_screen.y;
//		}
//		else
//			vtx_src->col = 0x00000000;
//		vtx_src++;
//	}
//}
bool any_3d = false;

void ConvertOverlay(COverManager* cur_over)
{
	for (int i = 0; i < cur_over->CmdBuffer.size(); i++)
		cur_over->CmdBuffer[i].ClipRect = Vec4(0, 0, CGlobal::iScreenWidth, CGlobal::iScreenHeight);

	COverManager::DrawVert* vtx_src = cur_over->VtxBuffer.data();

	for (int i = 0; i < cur_over->VtxBuffer.size(); i++)
	{
		Vector pos_screen;

		if (CGlobal::WorldToScreen(
			GP_3DMenu->CalcPoint(Vector(vtx_src->pos.x / 10, vtx_src->pos.y / 10, 0)),
			pos_screen))
		{
			vtx_src->pos.x = pos_screen.x;
			vtx_src->pos.y = pos_screen.y;
		}
		else
			vtx_src->col = 0x00000000;

		vtx_src++;
		any_3d = true;
	}
}

void CGui::ReleaseGui(CGui* pGui)
{
	if (pGui->Info.Name.find("##3D") != string::npos)
		ConvertOverlay(&pGui->Overlay);

	if (GetIO().ActivWindowName == pGui->Info.Name && pGui->Overlay.VtxBuffer.empty())
		GetIO().ActivWindowName = "";

	GP_Render->SetOverlay(&pGui->Overlay);
	GP_Render->ReleaseOverlay();

	for (auto child_it = pGui->ChildList.begin(); child_it != pGui->ChildList.end(); ++child_it)
		ReleaseGui(&child_it->second);

	for (auto popup_it = pGui->PopupList.begin(); popup_it != pGui->PopupList.end(); ++popup_it)
		X1Gui().G_PopupDrawStack.push_back(&popup_it->second);
	//	ReleaseGui(&popup_it->second);
}

COverManager MouseOverlay;

void CGui::Cleanup() // ����������
{
	any_3d = false;

	for (auto it = X1Gui().G_WindowList.begin(); it != X1Gui().G_WindowList.end(); ++it)
		ReleaseGui(&it->second);

	if (X1Gui().OpenedPopup != GetIO().ActivWindowName && X1Gui().ClosePopup)
		GetIO().ActivWindowName = "";

	for (size_t i = 0; i < X1Gui().G_PopupDrawStack.size(); i++)
		ReleaseGui(X1Gui().G_PopupDrawStack[i]);

	if (any_3d)
	{
		MouseOverlay.ResetCount();

		GP_Render->SetOverlay(&MouseOverlay);
		{
			Vec2 mouse_pos = X1Gui().GetIO().MousePos;
			GP_Render->DrawFilledBox(mouse_pos.x, mouse_pos.y, 4, 4, color_t(0, 255, 0, 255));
			ConvertOverlay(&MouseOverlay);
		}
		GP_Render->ReleaseOverlay();
	}

	//pThis->OpenedPopup = popup;
	//GetIO().ActivWindowName = popup;

	GetIO().MouseWheel = 0;
}

void CGui::Start()
{
	FastCall::G().t_QueryPerformanceFrequency((LARGE_INTEGER *)&GetIO().G_TicksInSec);
	FastCall::G().t_QueryPerformanceCounter((LARGE_INTEGER *)&GetIO().G_Time);
}

Vec2 CGui::GetPrevWindowPos()
{
	return X1Gui().G_PrevWindowPos;
}

void CGui::SetNextWindowPos(Vec2 Pos)
{
	X1Gui().G_NextWindowPos = Pos;
}

GuiIO& CGui::GetIO()
{
	return X1Gui().G_IO;
}

GuiStyle& CGui::GetStyle()
{
	return X1Gui().G_Style;
}

Vec2 CGui::GetCurWindowSize()
{
	return pThis->Info.Size;
}

Vec2 CGui::GetCurWindowPos()
{
	return pThis->Info.Pos;
}

CGui* CGui::GetThis()
{
	return pThis;
}

COverManager* CGui::GetOverlay()
{
	return &pThis->Overlay;
}

string CGui::FormatLabel(string str)
{
	string entry = "";
	size_t len = str.length();

	for (size_t i = 0; i < len; i++)
	{
		if (str[i] == '#' && i + 1 < len && str[i + 1] == '#')
			return entry;
		entry += str[i];
	}

	return entry;
}

ColU32 CGui::IdHash(const void* data, int data_size, ColU32 seed)
{
	static ColU32 crc32_lut[256] = { 0 };
	if (!crc32_lut[1])
	{
		const ColU32 polynomial = 0xEDB88320;
		for (ColU32 i = 0; i < 256; i++)
		{
			ColU32 crc = i;
			for (ColU32 j = 0; j < 8; j++)
				crc = (crc >> 1) ^ (ColU32(-int(crc & 1)) & polynomial);
			crc32_lut[i] = crc;
		}
	}

	seed = ~seed;
	ColU32 crc = seed;
	const unsigned char* current = (const unsigned char*)data;

	if (data_size > 0)
	{
		// Known size
		while (data_size--)
			crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ *current++];
	}
	else
	{
		// Zero-terminated string
		while (unsigned char c = *current++)
		{

			if (c == '#' && current[0] == '#' && current[1] == '#')
				crc = seed;
			crc = (crc >> 8) ^ crc32_lut[(crc & 0xFF) ^ c];
		}
	}
	return ~crc;
}

DWORD CGui::GetItemId(string data)
{
	//pThis->ItemNumber++;

	string str_data = pThis->Info.Name + data;

	pThis->ItemsId = IdHash(str_data.c_str(), str_data.length(), pThis->Info.Name.length() /*+ pThis->ItemNumber*/);
	pThis->PrevItemId = pThis->ItemsId;

	pThis->ItemIdStack.push_back(pThis->ItemsId);

	return pThis->ItemsId;
}

void CGui::SetFont(CFont* pFont)
{
	X1Gui().GP_Font = pFont;
}

CFont* CGui::GetFont()
{
	return X1Gui().GP_Font;
}

bool CGui::IsMouseHoverEx(Vec2 Min, Vec2 Max)
{
	Vec2 mouse = X1Gui().G_IO.MousePos;
	if (Min.x <= mouse.x && mouse.x <= Max.x &&
		Min.y <= mouse.y && mouse.y <= Max.y)
		return true;

	return false;
}
bool CGui::IsMouseHover(Vec2 Min, Vec2 Max)
{
	string chc = X1Gui().G_IO.ActivWindowName;
	
	if(!(pThis->Info.Flags & GWF_ChildWindow) && !(pThis->Info.Flags & GWF_Popup) 
		&& (X1Gui().G_IO.ActivWindowName != "" &&
		X1Gui().G_IO.ActivWindowName != pThis->Info.Name))
		return false;

	Rect rect_window = pThis->Info.ClipRect;

	if(!(pThis->Info.Flags & GWF_ChildWindow) && !(pThis->Info.Flags & GWF_Popup) &&
		X1Gui().G_IO.ActivWindowName == pThis->Info.Name &&
		!IsMouseHoverEx(rect_window.min, rect_window.max))
		return false;

	if (IsMouseHoverEx(Min, Max) && IsMouseHoverEx(rect_window.min, rect_window.max))
		return true;

	return false;
}

void CGui::ShowMetrics()
{
	Begin("Metrics");
	{
		int idx = 0;

		//IO.G_DeltaTime
		Text("DeltaTime: %f", GetIO().G_DeltaTime);
		for (auto it = X1Gui().G_WindowList.begin(); it != X1Gui().G_WindowList.end(); ++it)
		{
			Text("%i: Window name: %s Pos(%i, %i) Size(%i, %i)",
				idx, it->second.Info.Name.c_str(),
				(int)it->second.Info.Pos.x, (int)it->second.Info.Pos.y,
				(int)it->second.Info.Size.x, (int)it->second.Info.Size.y);
			
			Text("\t VTX: %i", it->second.Overlay.VtxBuffer.size());
			Text("\t IDX: %i", it->second.Overlay.IdxBuffer.size());
			Text("\t CMD: %i", it->second.Overlay.CmdBuffer.size());
			for (int i(0); i < it->second.Overlay.CmdBuffer.size(); i++)
			{
				Vec4 rec = it->second.Overlay.CmdBuffer[i].ClipRect;
				Text("\t\t Size: %f, %f, %f, %f", rec.x, rec.y, rec.z, rec.w);
			}
			Text("\t ActivID: %X", it->second.ActivItemId);

			idx++;
		}
		Text("Total VTX: %i", GP_Render->TotalVtxCount);
		Text("Total IDX: %i", GP_Render->TotalIdxCount);


		Text(" ");
		Text("MousePos X: %i, Y: %i", (int)X1Gui().G_IO.MousePos.x, (int)X1Gui().G_IO.MousePos.y);
		Text("LeftMouse: %i", (int)X1Gui().G_IO.LMPress);
		Text("X1Gui().G_IO.ActivWindowName: %s", X1Gui().G_IO.ActivWindowName.c_str());

		//X1Gui().G_IO.ActivWindowName

		static int ct = 0;
		if (X1Gui().G_IO.LMClicked)
			ct++;

		Text("LeftMouseClick: %i (%i)", (int)X1Gui().G_IO.LMClicked, ct);
	}
	End();
}

Vec2 CGui::GetCursorPosEx()
{
	return pThis->CursorPos;
}
Vec2 CGui::GetCursorPos()
{
	if (pThis->GroupStack.empty())
		return pThis->Info.Pos + GetCursorPosEx();
	else
		return pThis->Info.Pos + pThis->GroupStack.back().newPosEx + GetCursorPosEx();

}
void CGui::SetCursorPos(Vec2 Pos, float width)
{
	Vec2 minus = pThis->GroupStack.empty() ? Vec2(0,0) : GetStyle().wndPadding;
	pThis->PrevCursorPos = Vec2(pThis->CursorPos.x + width, pThis->CursorPos.y) - Vec2(minus.x, 0);
	pThis->CursorPos = Pos - Vec2(minus.x, 0);
}

bool CGui::CheckSkipEx(Rect owner, Rect rect)
{
	if (rect.max.x < owner.min.x || rect.max.y < owner.min.y)
		return true;

	if (rect.min.x > owner.max.x || rect.min.y > owner.max.y)
		return true;

	return false;
}

bool CGui::CheckSkip(Rect rect)
{
	Rect window_rect = pThis->Info.ItemsClipRect;

	return CheckSkipEx(window_rect, rect);
}

void CGui::TextFmt(Vec2 Pos, const char* label, ...)
{
	va_list args;
	char buf[256];
	va_start(args, label);
	vsprintf_s(buf, label, args);
	va_end(args);

	DrawString(Pos.x, Pos.y, X1Gui().G_Style.clrText, false, false, buf);
}
void CGui::Text(Vec2 Pos, const char* label)
{
	DrawString(Pos.x, Pos.y, X1Gui().G_Style.clrText, false, false, label);
}
void CGui::TextColored(color_t color, Vec2 Pos, const char* label)
{
	DrawString(Pos.x, Pos.y, color, false, false, label);
}

void CGui::Text(const char* label, ...)
{
	Vec2 pos = GetCursorPos();

	va_list args;
	char buf[256];
	va_start(args, label);
	vsprintf_s(buf, label, args);
	va_end(args);

	Vec2 size = CalcTextSize(buf);

	SetCursorPos(Vec2(X1Gui().G_Style.wndPadding.x, GetCursorPosEx().y + size.y + X1Gui().G_Style.itmPadding.y), 
		size.x + X1Gui().G_Style.itmPadding.x);

	if (CheckSkip(Rect(pos, pos + size)))
		return;

	DrawString(pos.x, pos.y, X1Gui().G_Style.clrText, false, false, buf);
}

bool CGui::ButtonBehavior(Rect Rect, bool &Hovered)
{
	pThis->LastItemHovered = false;
	pThis->LastItemClicked = false;

	Hovered = (IsMouseHover(Rect.min, Rect.max) && pThis->ActivItemId == -1) ||
		(pThis->ActivItemId == pThis->ItemsId && X1Gui().G_IO.LMPress);

	if (Hovered)
		pThis->ActivItemId = pThis->ItemsId;
	if ((!Hovered && pThis->ActivItemId == pThis->ItemsId) ||
		((pThis->ActivItemId == pThis->ItemsId || pThis->ActivItemId == -1) && !X1Gui().G_IO.LMPress))
		pThis->ActivItemId = -1;

	if (Hovered)
		pThis->LastItemHovered = true;

	if (pThis->ItemsId == pThis->ActivItemId && Hovered && X1Gui().G_IO.LMClicked)
	{
		pThis->LastItemClicked = true;
		return true;
	}

	return false;
}

bool CGui::Button(string Label, Vec2 Size)
{
	DWORD id = GetItemId(Label);
	Vec2 pos = GetCursorPos();
	GuiStyle &style = X1Gui().G_Style;

	string formated_label = FormatLabel(Label);

	Vec2 label_size = CalcTextSize(formated_label);
	Vec2 button_size;

	button_size.x = Size.x == 0 ? label_size.x + (style.itmInPadding.x * 2) : Size.x;
	button_size.y = Size.y == 0 ? label_size.y + (style.itmInPadding.y * 2) : Size.y;

	Rect button_rect(pos, pos + button_size);

	SetCursorPos(Vec2(style.wndPadding.x, GetCursorPosEx().y + button_size.y + style.itmPadding.y),
		button_size.x + X1Gui().G_Style.itmPadding.x);

	if (CheckSkip(button_rect))
		return false;

	color_t button_color = style.clrButton;

	bool hovered = false;
	bool clicked = ButtonBehavior(button_rect, hovered);

	if (hovered)
		button_color = style.clrButtonHover;
	if (hovered && X1Gui().G_IO.LMPress)
		button_color = style.clrButtonHold;

	DrawFilledBox(button_rect, button_color, style.wndRounding);

	Text(Vec2(button_rect.min.x + (button_size.x / 2) - (label_size.x / 2),
		button_rect.min.y + (button_size.y / 2) - (label_size.y / 2)), formated_label.c_str());

	return clicked;
}

bool CGui::CheckBox(string Label, bool *var)
{
	DWORD id = GetItemId(Label);
	Vec2 pos = GetCursorPos();
	GuiStyle &style = X1Gui().G_Style;

	Label = FormatLabel(Label);

	Vec2 label_size = CalcTextSize(Label);
	Vec2 cb_size = Vec2((label_size.y * 2) * 1.25f, (label_size.y) * 1.2f);

	Rect cb_rect = Rect(pos, pos + cb_size);
	Rect check_rect = Rect(cb_rect.min, Vec2(cb_rect.max.x + label_size.x + style.itmInPadding.x, cb_rect.max.y));

	SetCursorPos(Vec2(style.wndPadding.x, GetCursorPosEx().y + cb_size.y + style.itmPadding.y),
		cb_size.x + label_size.x + style.itmInPadding.x + 3 + X1Gui().G_Style.itmPadding.x);

	if (CheckSkip(check_rect))
		return false;

	bool hovered = false;
	bool clicked = ButtonBehavior(check_rect, hovered);

	if (clicked)
		*var = !(*var);

	color_t frame_color = style.clrFrame;
	color_t but_color = style.clrButton;

	if (hovered)
		frame_color = style.clrFrameHover;
	if (hovered && X1Gui().G_IO.LMPress)
		frame_color = style.clrFrameHold;

	if (*var)
	{
		if (hovered)
			but_color = style.clrButtonHover;
		if (hovered && X1Gui().G_IO.LMPress)
			but_color = style.clrButtonHold;
	}
	else
	{
		but_color = style.clrDisButton;
		if (hovered)
			but_color = style.clrDisButtonHover;
		if (hovered && X1Gui().G_IO.LMPress)
			but_color = style.clrDisButtonHold;
	}

	Rect but_rect;

	if (style.AnimationEnable)
	{
		CAnimController* cur_anim = GetAnim(id,!(*var));
		cur_anim->Update(*var, CHECKBOX_SPEED, CHECKBOX_SPEED_DELTA);

		float cur_len = cur_anim->Get(cb_size.x / 2);

		but_rect.min = cb_rect.min;
		but_rect.max = Vec2(cb_rect.min.x + (cb_size.x / 2), cb_rect.max.y);

		but_rect.min.x += cur_len;
		but_rect.max.x += cur_len;
	}
	else
	{
		if (*var)
		{
			but_rect.min = Vec2(cb_rect.min.x + (cb_size.x / 2), cb_rect.min.y);
			but_rect.max = cb_rect.max;
		}
		else
		{
			but_rect.min = cb_rect.min;
			but_rect.max = Vec2(cb_rect.min.x + (cb_size.x / 2), cb_rect.max.y);
		}
	}


	DrawFilledBox(cb_rect, frame_color, style.wndRounding);
	DrawFilledBox(but_rect, but_color, style.wndRounding);

	Text(Vec2(cb_rect.max.x + style.itmInPadding.x, cb_rect.min.y + (cb_size.y / 2) - (label_size.y / 2)),
		Label.c_str());

	return clicked;
}

int CGui::ParseFormatPrecision(const char* fmt, int default_precision)
{
	int precision = default_precision;
	while ((fmt = strchr(fmt, '%')) != NULL)
	{
		fmt++;
		if (fmt[0] == '%') { fmt++; continue; } // Ignore "%%"
		while (*fmt >= '0' && *fmt <= '9')
			fmt++;
		if (*fmt == '.')
		{
			precision = atoi(fmt + 1);
			if (precision < 0 || precision > 10)
				precision = default_precision;
		}
		break;
	}
	return precision;
}

float CGui::RoundScalar(float value, int decimal_precision)
{
	static const float min_steps[10] = { 1.0f, 0.1f, 0.01f, 0.001f, 0.0001f, 0.00001f, 0.000001f, 0.0000001f, 0.00000001f, 0.000000001f };
	float min_step = (decimal_precision >= 0 && decimal_precision < 10) ? min_steps[decimal_precision] : powf(10.0f, (float)-decimal_precision);
	bool negative = value < 0.0f;
	value = fabsf(value);
	float remainder = fmodf(value, min_step);
	if (remainder <= min_step * 0.5f)
		value -= remainder;
	else
		value += (min_step - remainder);
	return negative ? -value : value;
}

bool CGui::SliderFloat(string Label, float *var, float min, float max, const char* display_format)
{
	DWORD id = GetItemId(Label);
	Vec2 pos = GetCursorPos();
	GuiStyle &style = X1Gui().G_Style;
	GuiIO &IO = X1Gui().G_IO;

	Label = FormatLabel(Label);

	Vec2 label_size = CalcTextSize(Label);

	float frame_len = 0;
	if (pThis->ItemWidth == 0)
		frame_len = pThis->Info.Size.x - label_size.x - (style.wndPadding.x*2)-5;
	else
		frame_len = pThis->ItemWidth;

	Vec2 frame_size = Vec2(frame_len, (label_size.y) * 1.2f);
	Rect frame_rect = Rect(pos, pos + frame_size);

	SetCursorPos(Vec2(style.wndPadding.x, GetCursorPosEx().y + frame_size.y + style.itmPadding.y),
		frame_size.x + label_size.x + style.itmInPadding.x + style.itmPadding.x);

	if (CheckSkip(frame_rect))
		return false;

	bool hovered = false;
	bool clicked = ButtonBehavior(frame_rect, hovered);
	bool var_change = hovered && X1Gui().G_IO.LMPress;

	color_t frame_color = style.clrFrame;
	color_t but_color = style.clrButton;

	if (hovered)
	{
		frame_color = style.clrFrameHover;
		but_color = style.clrButtonHover;
	}
	if (var_change)
	{
		frame_color = style.clrFrameHold;
		but_color = style.clrButtonHold;
	}


	float in_pad = 2.3f;

	float line_len = frame_len - (in_pad * 2);

	Rect line_full_rect = Rect(Vec2(frame_rect.min.x + in_pad, frame_rect.min.y + in_pad),
		Vec2(frame_rect.max.x - in_pad, frame_rect.max.y - in_pad));

	float delta = max - min;
	float power = line_len / delta;
	float mouse_to_start = 0;
	int precision = ParseFormatPrecision(display_format, 3);

	if (var_change)
	{
		mouse_to_start = IO.MousePos.x - line_full_rect.min.x;

		float new_var = min + (mouse_to_start / power);

		new_var = RoundScalar(new_var, precision);

		if (new_var != *var)
			*var = new_var;
	}

	mouse_to_start = (*var - min) * power; // ����� �����

	if (*var > max)
		*var = max;

	if (*var < min)
		*var = min;

	if (mouse_to_start < 0)
		mouse_to_start = 0;
	if (mouse_to_start > line_len)
		mouse_to_start = line_len;

	Rect line_rect = Rect(line_full_rect.min, Vec2(line_full_rect.min.x + mouse_to_start, line_full_rect.max.y));

	DrawFilledBox(frame_rect, frame_color, style.wndRounding);
	DrawFilledBox(line_rect, but_color, style.wndRounding);

	char* var_buf = new char[128];
	std::sprintf(var_buf, display_format, (*var));
	Vec2 var_size = CalcTextSize(string(var_buf));
	var_size.x = 7 * strlen(var_buf); /*fix this*/

	Text(Vec2(frame_rect.min.x + (frame_size.x / 2) - (var_size.x / 2), frame_rect.min.y + (frame_size.y / 2) - (var_size.y / 2)),
		var_buf);

	delete[] var_buf;

	Text(Vec2(frame_rect.max.x + style.itmInPadding.x, frame_rect.min.y + (frame_size.y / 2) - (label_size.y / 2)),
		Label.c_str());

	return var_change;
}

bool CGui::SliderInt(string Label, int *var, int min, int max)
{
	float buf = (float)(*var);
	bool change = SliderFloat(Label, &buf, (float)min, (float)max, "%6.0lf");
	*var = (int)buf;

	return change;
}

bool CGui::SliderIntProc(string Label, int *var, int min, int max)
{
	float buf = (float)(*var);
	bool change = SliderFloat(Label, &buf, (float)min, (float)max, "%.0f%%");
	*var = (int)buf;

	return change;
}

bool CGui::BeginIntenrnalWin(string Label, GuiWindowFlags Flags, Vec2 Size, Vec2 Pos, color_t col, bool Scroll)
{
	CGui* m_oldThis = &(*pThis);
	bool ret = Begin(Label, Flags, Size, Pos, m_oldThis, &col);

//	SetCursorPos(GetStyle().wndPadding, 0);

	pThis->oldThis->childThis = pThis;
	pThis->VScrollEnable = Scroll;
	return ret;
}
void CGui::EndIntenrnalWin()
{
	End();
	pThis = pThis->oldThis;
}

void CGui::OpenPopup(string popup)
{
	X1Gui().OpenedPopup = popup;
	GetIO().ActivWindowName = popup;
}
bool CGui::IsPopupOpen(string popup)
{
	return X1Gui().OpenedPopup == popup;
}

bool CGui::ListBoxHeader(string Label, Vec2 Size)
{
	Vec2 pos = GetCursorPos();
	return BeginIntenrnalWin(Label, GWF_ChildWindow, Size, pos, GetStyle().clrFrame);
}

void CGui::ListBoxFooter()
{
	Vec2 ChildSize = pThis->Info.Size;
	EndIntenrnalWin();

	GuiStyle &style = X1Gui().G_Style;

	SetCursorPos(Vec2(style.wndPadding.x, GetCursorPosEx().y + ChildSize.y + style.itmPadding.y),
		ChildSize.x + style.itmPadding.x);
}

bool CGui::SelectLabel(string Label, bool Selected, Vec2 Size)
{
	DWORD id = GetItemId(Label);
	Vec2 pos = GetCursorPos();
	GuiStyle &style = X1Gui().G_Style;
	GuiIO &IO = X1Gui().G_IO;

	Label = FormatLabel(Label);

	Vec2 label_size = CalcTextSize(Label);

	Vec2 button_size;
	button_size.x = Size.x != 0 ? Size.x : pThis->Info.Size.x - (style.wndPadding.x * 2);
	button_size.y = Size.y != 0 ? Size.y : (label_size.y) * 1.2f;

	Rect button_rect = Rect(pos, pos + button_size);

	SetCursorPos(Vec2(style.wndPadding.x, GetCursorPosEx().y + button_size.y),
		button_size.x + style.itmPadding.x);

	if (CheckSkip(button_rect))
		return false;

	bool hovered = false;
	bool clicked = ButtonBehavior(button_rect, hovered);
	bool press = X1Gui().G_IO.LMPress;

	color_t button_color = style.clrButton;

	if (!Selected)
		button_color = style.clrFrame;

	if (hovered)
		button_color = style.clrButtonHover;
	if (hovered && press)
		button_color = style.clrButtonHold;

	DrawFilledBox(button_rect, button_color, style.wndRounding);

	Text(Vec2(button_rect.min.x + style.itmInPadding.x, button_rect.min.y + (button_size.y / 2) - (label_size.y / 2)),
		Label.c_str());

	return clicked;
}

bool CGui::ComboBox(string Label, int *var, VectorEx<const char*> &items)
{
	DWORD id = GetItemId(Label);
	Vec2 pos = GetCursorPos();
	GuiStyle &style = X1Gui().G_Style;
	GuiIO &IO = X1Gui().G_IO;

	string formated_label = FormatLabel(Label);

	Vec2 label_size = CalcTextSize(formated_label);

	float frame_len = 0;
	if (pThis->ItemWidth == 0)
		frame_len = pThis->Info.Size.x - label_size.x - style.wndPadding.x - 10;
	else
		frame_len = pThis->ItemWidth - label_size.x;

	Vec2 frame_size = Vec2(frame_len, (label_size.y) * 1.2f);
	Rect frame_rect = Rect(pos, pos + frame_size);

	SetCursorPos(Vec2(style.wndPadding.x, GetCursorPosEx().y + frame_size.y + style.itmPadding.y),
		frame_size.x + style.itmInPadding.x + label_size.x + style.itmPadding.x);

	if (CheckSkip(frame_rect))
		return false;

	int items_count = items.size();

	bool hovered = false;
	bool clicked = ButtonBehavior(frame_rect, hovered);
	bool press = IO.LMPress;

	color_t frame_color = style.clrFrame;
	color_t but_color = style.clrButton;

	if (hovered)
	{
		frame_color = style.clrFrameHover;
		but_color = style.clrButtonHover;
	}
	if (hovered && press)
	{
		frame_color = style.clrFrameHold;
		but_color = style.clrButtonHold;
	}

	float item_y = CalcTextSize(items[0]).y * 1.2;


	Vec2 old_pad = style.wndPadding;
	style.wndPadding = style.itmInPadding;

	float popup_y = item_y * items_count + (style.wndPadding.y * 2);
	float targ_popup_y = popup_y;


	if (style.AnimationEnable)
	{
		CAnimController* cur_anim = GetAnim(id, !IsPopupOpen(Label));
		cur_anim->Update(IsPopupOpen(Label), COMBOBOX_SPEED, COMBOBOX_SPEED_DELTA);
		popup_y = cur_anim->Get(targ_popup_y);
	}

	Vec2 popup_size = Vec2(frame_len, popup_y);
	Rect popup_rect = Rect(Vec2(frame_rect.min.x, frame_rect.max.y), Vec2());
	popup_rect.max = popup_rect.min + popup_size;

	if (clicked && !IsPopupOpen(Label))
	{
		OpenPopup(Label);
		clicked = false;
		IO.LMClicked = false;
	}

	if ((IsPopupOpen(Label) || style.AnimationEnable && popup_y > 0) && !CheckSkip(popup_rect))
	{
		bool close_popup = false;
		BeginIntenrnalWin(Label, GWF_Popup, popup_size, popup_rect.min, style.clrFrame, targ_popup_y == popup_y);
		{
			SetCursorPos(Vec2(style.wndPadding.x, style.wndPadding.y),0);
			for (int i(0); i < items.size(); i++)
			{
				if (SelectLabel(items[i], i == *var))
				{
					*var = i;
					close_popup = true;
				}
			}
		}
		EndIntenrnalWin();

		if (((!IsMouseHoverEx(popup_rect.min, popup_rect.max) && IO.LMClicked) || close_popup) && IsPopupOpen(Label))
		{
			X1Gui().OpenedPopup = "";
			pThis->childThis->ActivItemId = -1;
			X1Gui().ActivItemId = -1;
			X1Gui().ClosePopup = true;
			IO.ActivWindowName = "";
			IO.LMClicked = false;
			IO.LMPress = false;
		}
	}
	style.wndPadding = old_pad;

	string selected_str = items[*var];
	Vec2 selected_str_size = CalcTextSize(selected_str);

	DrawFilledBox(frame_rect, frame_color, style.wndRounding);

	Rect but_rect = Rect(Vec2(frame_rect.min.x + frame_size.x - (frame_size.y*1.2f), frame_rect.min.y),
		frame_rect.max);

	DrawFilledBox(but_rect, but_color, style.wndRounding);

	{
		float line_y = 2;
		float line_pad = 3;

		Rect lines_full = Rect(Vec2(but_rect.min.x + line_pad, but_rect.min.y + line_pad + 1.5f),
			Vec2(but_rect.max.x - line_pad, but_rect.max.y - line_pad - 1.5f));

		Rect line_1 = Rect(lines_full.min,
			Vec2(lines_full.max.x, lines_full.min.y + line_y));
		Rect line_2 = Rect(Vec2(lines_full.min.x, lines_full.min.y + 2 + line_y),
			Vec2(lines_full.max.x, lines_full.max.y - 2 - line_y));
		Rect line_3 = Rect(Vec2(lines_full.min.x, lines_full.max.y - line_y),
			lines_full.max);

		DrawFilledBox(line_1, style.clrTabLabelText, 0);
		DrawFilledBox(line_2, style.clrTabLabelText, 0);
		DrawFilledBox(line_3, style.clrTabLabelText, 0);
	}

	Text(Vec2(frame_rect.min.x + style.itmInPadding.x, frame_rect.min.y + (frame_size.y / 2) - (selected_str_size.y / 2)),
		items[*var]);
	Text(Vec2(frame_rect.max.x + style.itmInPadding.x, frame_rect.min.y + (frame_size.y / 2) - (label_size.y / 2)),
		formated_label.c_str());

	return clicked;
}

void CGui::ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v)
{
	float K = 0.f;
	if (g < b)
	{
		const float tmp = g; g = b; b = tmp;
		K = -1.f;
	}
	if (r < g)
	{
		const float tmp = r; r = g; g = tmp;
		K = -2.f / 6.f - K;
	}

	const float chroma = r - (g < b ? g : b);
	out_h = fabsf(K + (g - b) / (6.f * chroma + 1e-20f));
	out_s = chroma / (r + 1e-20f);
	out_v = r;
}

void ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b)
{
	if (s == 0.0f)
	{
		// gray
		out_r = out_g = out_b = v;
		return;
	}

	h = fmodf(h, 1.0f) / (60.0f / 360.0f);
	int   i = (int)h;
	float f = h - (float)i;
	float p = v * (1.0f - s);
	float q = v * (1.0f - s * f);
	float t = v * (1.0f - s * (1.0f - f));

	switch (i)
	{
	case 0: out_r = v; out_g = t; out_b = p; break;
	case 1: out_r = q; out_g = v; out_b = p; break;
	case 2: out_r = p; out_g = v; out_b = t; break;
	case 3: out_r = p; out_g = q; out_b = v; break;
	case 4: out_r = t; out_g = p; out_b = v; break;
	case 5: default: out_r = v; out_g = p; out_b = q; break;
	}
}

float *CGui::ColorToArr(SDK::Color &col)
{	
	return new float[4]{ (float)col._color[0], (float)col._color[1],  (float)col._color[2], (float)col._color[3] };
}

float Saturate(float f) { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; }

bool CGui::ColorEdit2(string Label, SDK::Color *var, Vec2 Size)
{
	DWORD id = GetItemId(Label);
	Vec2 pos = GetCursorPos();
	GuiStyle &style = X1Gui().G_Style;
	GuiIO &IO = X1Gui().G_IO;

	string formated_label = FormatLabel(Label);

	Vec2 label_size = CalcTextSize(formated_label);

	Vec2 frame_size;
	frame_size.x = Size.x != 0 ? Size.x : (label_size.y) * 1.2f;
	frame_size.y = Size.y != 0 ? Size.y : (label_size.y) * 1.2f;

	Rect frame_rect = Rect(pos, pos + frame_size);

	Vec2 popup_size = Vec2(204, 213);
	Rect popup_rect = Rect(frame_rect.max, frame_rect.max + popup_size);

	SetCursorPos(Vec2(style.wndPadding.x, GetCursorPosEx().y + frame_size.y + style.itmPadding.y),
		frame_size.x + style.itmInPadding.x + label_size.x + style.itmPadding.x);

	if (CheckSkip(frame_rect))
		return false;

	bool hovered = false;
	bool clicked = ButtonBehavior(frame_rect, hovered);

	if (clicked)
		OpenPopup(Label);
	if (IsPopupOpen(Label))
	{
		BeginIntenrnalWin(Label, GWF_Popup, popup_size, popup_rect.min, style.clrFrame);
		{
			ColorPicker(Label, var);
			Spacing();
			Separator();
			Spacing();
			ColorEdit(Label + "##pick_2", var, Vec2(popup_size.x - (style.itmInPadding.x*2) , 0));
			Spacing();
			Separator();
			Spacing();
			CheckBox("Rainbow", &var->brainbow);
		}
		EndIntenrnalWin();

		if ((!IsMouseHoverEx(popup_rect.min, popup_rect.max) &&
			!IsMouseHoverEx(frame_rect.min, frame_rect.max) && IO.LMClicked))
		{
			X1Gui().OpenedPopup = "";
			pThis->childThis->ActivItemId = -1;
			X1Gui().ActivItemId = -1;
			IO.ActivWindowName = "";
			IO.LMClicked = false;
		}
	}

	DrawFilledBox(frame_rect, *var, style.wndRounding);

	Text(Vec2(frame_rect.max.x + style.itmInPadding.x, frame_rect.min.y + (frame_size.y / 2) - (label_size.y / 2)),
		formated_label.c_str());
}

bool CGui::ColorEdit(string Label, SDK::Color *var, Vec2 Size)
{
	float* arr = ColorToArr(*var);
	bool change = ColorEdit(Label, arr, Size);

	var->SetR(arr[0]);
	var->SetG(arr[1]);
	var->SetB(arr[2]);
	var->SetA(arr[3]);

	return change;
}

bool CGui::ColorEdit(string Label, float *var, Vec2 Size)
{
	DWORD id = GetItemId(Label);
	Vec2 pos = GetCursorPos();
	GuiStyle &style = X1Gui().G_Style;
	GuiIO &IO = X1Gui().G_IO;

	Label = FormatLabel(Label);

	Vec2 label_size = CalcTextSize(Label);

	Vec2 full_frame_size;
	full_frame_size.x = Size.x != 0 ? Size.x : pThis->Info.Size.x - label_size.x - style.wndPadding.x - 10;
	full_frame_size.y = Size.y != 0 ? Size.y : (label_size.y) * 1.2f;

	Rect full_rect = Rect(pos, pos + full_frame_size);

	SetCursorPos(Vec2(X1Gui().G_Style.wndPadding.x, GetCursorPosEx().y + full_frame_size.y + X1Gui().G_Style.itmPadding.y),
		full_frame_size.x + style.itmInPadding.x + label_size.x + style.itmPadding.x);

	if (CheckSkip(full_rect))
		return false;

	float color_len = ((full_frame_size.x - full_frame_size.y - (style.itmInPadding.x * 3)) / 3);

	Rect r_rect = Rect(full_rect.min,
		Vec2(full_rect.min.x + color_len, full_rect.max.y));

	Rect g_rect = Rect(Vec2(r_rect.max.x + style.itmInPadding.x, full_rect.min.y),
		Vec2(r_rect.max.x + style.itmInPadding.x + color_len, full_rect.max.y));

	Rect b_rect = Rect(Vec2(g_rect.max.x + style.itmInPadding.x, full_rect.min.y),
		Vec2(g_rect.max.x + style.itmInPadding.x + color_len, full_rect.max.y));

	
	bool pressed = IO.LMPress;
	int but_id = 0;
	auto button_check = [&](Rect rect, float &value, string lable_v)->bool
	{
		color_t frame_color = style.clrFrame;

		bool hovered = false;
		DWORD cur_id = GetItemId(Label + "##" + to_string(but_id));
		but_id++;
		ButtonBehavior(rect, hovered);
		bool activ = pressed && hovered;

		if (hovered)
			frame_color = style.clrFrameHover;
		if (activ)
			frame_color = style.clrFrameHold;

		DrawFilledBox(rect, frame_color, style.wndRounding);

		string val_str = lable_v + to_string((int)value);

		Vec2 val_size = CalcTextSize(val_str);
		Vec2 rect_size = Vec2(rect.max.x - rect.min.x, rect.max.y - rect.min.y);

		Text(Vec2(rect.min.x + (rect_size.x / 2) - (val_size.x / 2),
			rect.min.y + (rect_size.y / 2) - (val_size.y / 2)), val_str.c_str());

		static Vec2 start_mouse;
		static bool set_pos = false;
		static float start_value = 0;

		if (activ)
		{
			if (!set_pos)
			{
				start_mouse = IO.MousePos;
				start_value = value;
				set_pos = true;
			}

			if (set_pos)
			{
				value = start_value + Vec2(IO.MousePos - start_mouse).x;

				if (value < 0)
					value = 0;
				if (value > 255)
					value = 255;
			}
		}

		if (hovered && !pressed)
		{
			set_pos = false;
		}

		return false;
	};

	bool changed = button_check(r_rect, var[0], ("R:"));
	changed = button_check(g_rect, var[1], ("G:"));
	changed = button_check(b_rect, var[2], ("B:"));

	Rect color_rect = Rect(Vec2(b_rect.max.x + style.itmInPadding.x, full_rect.min.y),
		Vec2(b_rect.max.x + style.itmInPadding.x + full_frame_size.y, full_rect.max.y));

	color_t col = color_t(var[0], var[1], var[2],255);
	DrawFilledBox(color_rect, col, style.wndRounding);

	Text(Vec2(color_rect.max.x + style.itmInPadding.x,
		color_rect.min.y + (full_frame_size.y / 2) - (label_size.y / 2)), Label.c_str());

	return changed;
}

bool CGui::ColorPicker(string Label, SDK::Color *var)
{
	float* arr = ColorToArr(*var);

	bool change = ColorPicker(Label, arr);

	var->SetR(arr[0]);
	var->SetG(arr[1]);
	var->SetB(arr[2]);
	var->SetA(arr[3]);

	return change;
}


bool CGui::ColorPicker(string Label, float *var)
{
	DWORD id = GetItemId(Label);
	Vec2 pos = GetCursorPos();
	GuiStyle &style = X1Gui().G_Style;
	GuiIO &IO = X1Gui().G_IO;
	float bars_width = 18;

	Vec2 color_matrix_size = Vec2(150, 150);

	Vec2 hue_bar_pos = Vec2(pos.x + style.itmPadding.x + color_matrix_size.x, pos.y);
	Vec2 hue_bar_size = Vec2(bars_width, color_matrix_size.y);

	Vec2 alpha_bar_pos = Vec2(hue_bar_pos.x + style.itmPadding.x + hue_bar_size.x, pos.y);
	Vec2 alpha_size_pos = Vec2(bars_width, color_matrix_size.y);

	Rect matrix_rect = Rect(pos, pos + color_matrix_size);
	Rect hue_rect = Rect(hue_bar_pos, hue_bar_pos + hue_bar_size);
	Rect alpha_rect = Rect(alpha_bar_pos, alpha_bar_pos + alpha_size_pos);

	SetCursorPos(Vec2(X1Gui().G_Style.wndPadding.x, GetCursorPosEx().y + color_matrix_size.y + X1Gui().G_Style.itmPadding.y),
		color_matrix_size.x + style.itmInPadding.x + hue_bar_size.x + alpha_size_pos.x + style.itmPadding.x);

	if (CheckSkip(matrix_rect) && CheckSkip(hue_rect) && CheckSkip(alpha_rect))
		return false;

	bool pressed = IO.LMPress;

	DWORD matrix_id = GetItemId(("matrix"));
	bool matrix_hovered = false;
	ButtonBehavior(matrix_rect, matrix_hovered);
	bool matrix_activ = matrix_hovered && pressed;

	DWORD hue_id = GetItemId(("hue"));
	bool hue_hovered = false;
	ButtonBehavior(hue_rect, hue_hovered);
	bool hue_activ = hue_hovered && pressed;

	DWORD alpha_id = GetItemId(("alpha"));
	bool alpha_hovered = false;
	ButtonBehavior(alpha_rect, alpha_hovered);
	bool alpha_activ = alpha_hovered && pressed;

	float tmp_var[4] = { var[0] / 255.f, var[1] / 255.f, var[2] / 255.f, var[3] / 255.f };

	float H = 0, S = 0, V = 0;
	ColorConvertRGBtoHSV(tmp_var[0], tmp_var[1], tmp_var[2], H, S, V);

	bool value_change = false;
	bool hsv_change = false;

	if (matrix_activ)
	{
		S = Saturate((IO.MousePos.x - pos.x) / (color_matrix_size.x - 1));
		V = 1.0f - Saturate((IO.MousePos.y - pos.y) / (color_matrix_size.y - 1));
		hsv_change = value_change = true;
	}

	if (hue_activ)
	{
		H = Saturate((IO.MousePos.y - pos.y) / (color_matrix_size.y - 1));
		hsv_change = value_change = true;
	}

	if (alpha_activ)
	{
		tmp_var[3] = 1.0f - Saturate((IO.MousePos.y - pos.y) / (color_matrix_size.y - 1));
		value_change = true;
	}

	if(hsv_change)
		ColorConvertHSVtoRGB(H >= 1.0f ? H - 10 * 1e-6f : H, S > 0.0f ? S : 10 * 1e-6f, V > 0.0f ? V : 1e-6f,
			tmp_var[0], tmp_var[1], tmp_var[2]);

	if (value_change) {
		float new_H = 1, new_S = 1, new_V = 1;
		ColorConvertRGBtoHSV(tmp_var[0], tmp_var[1], tmp_var[2], new_H, new_S, new_V);
		if (new_H <= 0 && H > 0) {
			if (new_V <= 0 && V != new_V)
				ColorConvertHSVtoRGB(H, S, new_V <= 0 ? V * 0.5f : new_V, tmp_var[0], tmp_var[1], tmp_var[2]);
			else if (new_S <= 0)
				ColorConvertHSVtoRGB(H, new_S <= 0 ? S * 0.5f : new_S, new_V, tmp_var[0], tmp_var[1], tmp_var[2]);
		}
	}

	var[0] = tmp_var[0] * 255.f;
	var[1] = tmp_var[1] * 255.f;
	var[2] = tmp_var[2] * 255.f;
	var[3] = tmp_var[3] * 255.f;
	Vec2 cursor_pos;

	cursor_pos.x = pos.x + (S * color_matrix_size.x + 0.5f);
	cursor_pos.y = pos.y + ((1 - V) * color_matrix_size.y + 0.5f);

	ColU32 hue_colors[] = { D3DCOLOR_RGBA(255,0,0,255),
		D3DCOLOR_RGBA(255,255,0,255), 
		D3DCOLOR_RGBA(0,255,0,255),
		D3DCOLOR_RGBA(0,255,255,255),
		D3DCOLOR_RGBA(0,0,255,255),
		D3DCOLOR_RGBA(255,0,255,255),
		D3DCOLOR_RGBA(255,0,0,255) 
	};

	//hue_bar
	for (int i = 0; i < 6; i++) 
	{
		DrawGradientBox(hue_bar_pos.x, hue_bar_pos.y + ((hue_bar_size.y / 6) * i),
			hue_bar_size.x,(hue_bar_size.y / 6),
			hue_colors[i], hue_colors[i], hue_colors[i + 1], hue_colors[i + 1]);
	}

	float tmp_current[3] = { 1,1,1 };

	ColorConvertHSVtoRGB(H, 1, 1, tmp_current[0], tmp_current[1], tmp_current[2]);

	tmp_current[0] *= 255.f;
	tmp_current[1] *= 255.f;
	tmp_current[2] *= 255.f;

	DWORD current = D3DCOLOR_RGBA((int)tmp_current[0], (int)tmp_current[1], (int)tmp_current[2], (int)255);

	float hue_cursor_y = (int)(H * color_matrix_size.y + 0.5f);
	if (hue_activ)
	{
		Rect hue_sel = Rect(Vec2(hue_bar_pos.x, hue_bar_pos.y - 3 + hue_cursor_y),Vec2());
		hue_sel.max = hue_sel.min + Vec2(hue_bar_size.x, 7);
		DrawFilledBox(hue_sel, current, 0);
		DrawBox(hue_bar_pos.x, hue_bar_pos.y - 3 + hue_cursor_y, hue_bar_size.x, 7, 1, 1, 0xFFFFFFFF);
	}
	else
		DrawLine(Vec2(hue_bar_pos.x-2, hue_bar_pos.y + hue_cursor_y),
			Vec2(hue_bar_pos.x + hue_bar_size.x + 1, hue_bar_pos.y + hue_cursor_y), 0xFFFFFFFF);

	//alpha_bar
	DrawGradientBox(alpha_bar_pos.x, alpha_bar_pos.y,
		alpha_size_pos.x, alpha_size_pos.y,
		0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0xFF000000);

	float alpha = tmp_var[3];
	float alpha_cursor_y = (int)((1.0f - alpha) * color_matrix_size.y + 0.5f);

	//float alpha_cursor_y = (float)(int)(picker_pos.y + (1.0f - col[3]) * sv_picker_size + 0.5f);
	DrawLine(Vec2(alpha_bar_pos.x - 2, alpha_bar_pos.y + alpha_cursor_y),
		Vec2(alpha_bar_pos.x + alpha_size_pos.x + 1, alpha_bar_pos.y + alpha_cursor_y), 0xFFFFFFFF);


	//color_matrix
	DrawGradientBox(pos.x, pos.y,
		color_matrix_size.x, color_matrix_size.y,
		0xFFFFFFFF, current, current, 0xFFFFFFFF);
	DrawGradientBox(pos.x, pos.y,
		color_matrix_size.x, color_matrix_size.y, 
		0x00000000, 0x00000000, 0xFF000000, 0xFF000000);


	float CROSSHAIR_SIZE = 7.0f;
	Vec2 p = Vec2((int)cursor_pos.x, (int)cursor_pos.y);
	if (matrix_activ)
	{
		current = D3DCOLOR_RGBA((int)var[0], (int)var[1], (int)var[2], (int)255);

		DrawCircle(p.x, p.y, CROSSHAIR_SIZE, 28, current);
		DrawRing(p.x, p.y, CROSSHAIR_SIZE + 0.5f, 28, 0xFFFFFFFF, 0.5f);
	}
	else
	{
		DrawLine(Vec2(p.x - CROSSHAIR_SIZE, p.y), Vec2(p.x - 2, p.y), 0xFFFFFFFF);
		DrawLine(Vec2(p.x + CROSSHAIR_SIZE, p.y), Vec2(p.x + 2, p.y), 0xFFFFFFFF);
		DrawLine(Vec2(p.x, p.y + CROSSHAIR_SIZE), Vec2(p.x, p.y + 2), 0xFFFFFFFF);
		DrawLine(Vec2(p.x, p.y - CROSSHAIR_SIZE), Vec2(p.x, p.y - 2), 0xFFFFFFFF);
	}

	return value_change;
}

void CGui::SameLine(float len)
{
	if (len == 0)
		SetCursorPos(pThis->PrevCursorPos, 0);
	else
		SetCursorPos(Vec2(X1Gui().G_Style.wndPadding.x + len, pThis->PrevCursorPos.y),0);
}
void CGui::AddLineWidth(float len)
{
	pThis->CursorPos.x += len;
}

void CGui::Separator()
{
	Vec2 pos = GetCursorPos();

	Vec2 window_pos = pThis->Info.Pos;
	Vec2 window_size = pThis->Info.Size;
	Vec2 line_start = Vec2(window_pos.x,(int)(pos.y + (X1Gui().G_Style.itmPadding.y / 2)-2));
	Vec2 line_end = Vec2(line_start.x + window_size.x, line_start.y);

	DrawLine(line_start, line_end, X1Gui().G_Style.clrLine);

	SetCursorPos(Vec2(X1Gui().G_Style.wndPadding.x, GetCursorPosEx().y + X1Gui().G_Style.itmPadding.y ),0);
}
void CGui::VerticalSeparator(float ofst)
{
	Vec2 win_pos = pThis->Info.Pos;
	Vec2 win_size = pThis->Info.Size;

	Vec2 line_start = Vec2(win_pos.x + ofst, win_pos.y);
	Vec2 line_end = Vec2(line_start.x, line_start.y + win_size.y);

	DrawLine(line_start, line_end, X1Gui().G_Style.clrLine);
}
void CGui::Spacing()
{
	SetCursorPos(Vec2(pThis->CursorPos.x, pThis->CursorPos.y + (X1Gui().G_Style.itmPadding.y/2)), 0);
}
void CGui::PushItemWidth(float Width)
{
	pThis->ItemWidth = Width;
}

void CGui::TabLabels(int *var, Vec2 Size, VectorEx<const char*> &items, bool vertical)
{
	DWORD id = GetItemId(("##Labels"));
	Vec2 pos = GetCursorPos();
	GuiStyle &style = X1Gui().G_Style;
	GuiIO &IO = X1Gui().G_IO;

	color_t old_button_color = style.clrButton;
	color_t old_text_color = style.clrText;

	Vec2 old_padding = style.itmPadding;
	style.itmPadding = Vec2(old_padding.x / 2, 1);

	size_t item_count = items.size();

	float button_y = Size.y != 0 ? Size.y : CalcTextSize(items[0]).y * 1.2f;
	if (vertical)
		button_y = (Size.y / item_count) - (style.itmPadding.y);

	float button_x = Size.x;
	if(!vertical)
		button_x = (Size.x / item_count) - (style.itmPadding.x/ item_count);

	Vec2 button_size = Vec2(button_x, (int)button_y);

	for (size_t i = 0; i < item_count; i++)
	{
		style.clrButton = style.clrTabLabel;
		style.clrText = style.clrTabLabelText;
		if (i == *var)
		{
			style.clrButton = style.clrButtonHold;
			style.clrText = old_text_color;
		}

		if (Button(items[i], button_size))
			*var = i;

		if (!vertical)
			SameLine();

		style.clrButton = old_button_color;
	}

	style.itmPadding = old_padding;
	style.clrText = old_text_color;

	SetCursorPos(Vec2(style.wndPadding.x, GetCursorPosEx().y + (vertical ? 0 : button_size.y) + style.itmPadding.y),
		button_size.x + style.itmPadding.x);
}

const char* const KeyNames[] = { "Unknown","LBUTTON","RBUTTON","CANCEL","MBUTTON","XBUTTON1","XBUTTON2","Unknown","BACK","TAB","Unknown","Unknown","CLEAR","RETURN","Unknown","Unknown","SHIFT",
	"CONTROL","MENU","PAUSE","CAPITAL","KANA","Unknown","JUNJA","FINAL","KANJI","Unknown","ESCAPE","CONVERT","NONCONVERT","ACCEPT","MODECHANGE","SPACE","PRIOR","NEXT","END","HOME","LEFT","UP",
	"RIGHT","DOWN","SELECT","PRINT","EXECUTE","SNAPSHOT","INSERT","DELETE","HELP","0","1","2","3","4","5","6","7","8","9","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown",
	"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","LWIN","RWIN","APPS","Unknown","SLEEP","NUMPAD0","NUMPAD1","NUMPAD2","NUMPAD3","NUMPAD4",
	"NUMPAD5","NUMPAD6","NUMPAD7","NUMPAD8","NUMPAD9","MULTIPLY","ADD","SEPARATOR","SUBTRACT","DECIMAL","DIVIDE","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12","F13","F14",
	"F15","F16","F17","F18","F19","F20","F21","F22","F23","F24","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","NUMLOCK","SCROLL","OEM_NEC_EQUAL",
	"OEM_FJ_MASSHOU","OEM_FJ_TOUROKU","OEM_FJ_LOYA","OEM_FJ_ROYA","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","Unknown","LSHIFT","RSHIFT",
	"LCONTROL","RCONTROL","LMENU","RMENU"
};

bool CGui::HotKey(string Label, int *var)
{
	DWORD id = GetItemId(Label);
	Vec2 pos = GetCursorPos();
	GuiStyle &style = X1Gui().G_Style;
	GuiIO &IO = X1Gui().G_IO;

	string formated_label = FormatLabel(Label);

	Vec2 label_size = CalcTextSize(formated_label);

	float frame_len = 0;
	if (pThis->ItemWidth == 0)
		frame_len = pThis->Info.Size.x - label_size.x - style.wndPadding.x - 10;
	else
		frame_len = pThis->ItemWidth - label_size.x;

	Vec2 frame_size = Vec2(frame_len, (label_size.y) * 1.2f);
	Rect frame_rect = Rect(pos, pos + frame_size);

	SetCursorPos(Vec2(style.wndPadding.x, GetCursorPosEx().y + frame_size.y + style.itmPadding.y),
		frame_size.x + style.itmInPadding.x + label_size.x + style.itmPadding.x);

	if (CheckSkip(frame_rect))
		return false;

	bool hovered = false;
	bool clicked = ButtonBehavior(frame_rect, hovered);
	bool pressed = X1Gui().G_IO.LMPress;

	color_t frame_color = style.clrFrame;

	if (hovered)
		frame_color = style.clrFrameHover;
	if (hovered && clicked)
		frame_color = style.clrFrameHold;

	bool value_changed = false;
	int key = *var;

	char buf_display[64] = "None";

	if (pThis->UsingItemId == id /*&& !clicked*/ /*&& !clicked && !hovered*/)
	{
		for (auto i = 0; i < 5; i++)
		{
			if (IO.MouseDown[i])
			{
				switch (i) 
				{
				case 0:
					key = VK_LBUTTON;
					break;
				case 1:
					key = VK_RBUTTON;
					break;
				case 2:
					key = VK_MBUTTON;
					break;
				case 3:
					key = VK_XBUTTON1;
					break;
				case 4:
					key = VK_XBUTTON2;
					break;
				}
				value_changed = true;
				pThis->UsingItemId = -1;
			}
		}
		if (!value_changed)
		{
			for (auto i = VK_BACK; i <= VK_RMENU; i++) 
			{
				if (IO.KeysDown[i])
				{
					key = i;
					value_changed = true;
					pThis->UsingItemId = -1;
				}
			}
		}
		*var = key;
		strcpy(buf_display, "<Press a key>");
	}
	else if (pThis->UsingItemId != id && *var != 0)
		strcpy(buf_display, KeyNames[*var]);

	if (clicked && pThis->UsingItemId != id)
	{
		pThis->UsingItemId = id;
		IO.MouseDown[0] = false;
	}
	if (pressed && !hovered && pThis->UsingItemId == id)
		pThis->UsingItemId = -1;

	DrawFilledBox(frame_rect, frame_color, style.wndRounding);

	Text(Vec2(frame_rect.max.x + style.itmInPadding.x,
		frame_rect.min.y + (frame_size.y / 2) - (label_size.y / 2)), formated_label.c_str());

	Vec2 var_size = CalcTextSize(buf_display);

	Text(Vec2(frame_rect.min.x + (frame_size.x / 2) - (var_size.x / 2),
		frame_rect.min.y + (frame_size.y / 2) - (label_size.y / 2)), buf_display);
}

int TextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end)
{
	unsigned int c = (unsigned int)-1;
	const unsigned char* str = (const unsigned char*)in_text;
	if (!(*str & 0x80))
	{
		c = (unsigned int)(*str++);
		*out_char = c;
		return 1;
	}
	if ((*str & 0xe0) == 0xc0)
	{
		*out_char = 0xFFFD; // will be invalid but not end of string
		if (in_text_end && in_text_end - (const char*)str < 2) return 1;
		if (*str < 0xc2) return 2;
		c = (unsigned int)((*str++ & 0x1f) << 6);
		if ((*str & 0xc0) != 0x80) return 2;
		c += (*str++ & 0x3f);
		*out_char = c;
		return 2;
	}
	if ((*str & 0xf0) == 0xe0)
	{
		*out_char = 0xFFFD; // will be invalid but not end of string
		if (in_text_end && in_text_end - (const char*)str < 3) return 1;
		if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf)) return 3;
		if (*str == 0xed && str[1] > 0x9f) return 3; // str[1] < 0x80 is checked below
		c = (unsigned int)((*str++ & 0x0f) << 12);
		if ((*str & 0xc0) != 0x80) return 3;
		c += (unsigned int)((*str++ & 0x3f) << 6);
		if ((*str & 0xc0) != 0x80) return 3;
		c += (*str++ & 0x3f);
		*out_char = c;
		return 3;
	}
	if ((*str & 0xf8) == 0xf0)
	{
		*out_char = 0xFFFD; // will be invalid but not end of string
		if (in_text_end && in_text_end - (const char*)str < 4) return 1;
		if (*str > 0xf4) return 4;
		if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf)) return 4;
		if (*str == 0xf4 && str[1] > 0x8f) return 4; // str[1] < 0x80 is checked below
		c = (unsigned int)((*str++ & 0x07) << 18);
		if ((*str & 0xc0) != 0x80) return 4;
		c += (unsigned int)((*str++ & 0x3f) << 12);
		if ((*str & 0xc0) != 0x80) return 4;
		c += (unsigned int)((*str++ & 0x3f) << 6);
		if ((*str & 0xc0) != 0x80) return 4;
		c += (*str++ & 0x3f);
		// utf-8 encodings of values used in surrogate pairs are invalid
		if ((c & 0xFFFFF800) == 0xD800) return 4;
		*out_char = c;
		return 4;
	}
	*out_char = 0;
	return 0;
}

int TextStrFromUtf8(Wchar* buf, int buf_size, const char* in_text, const char* in_text_end, const char** in_text_remaining)
{
	Wchar* buf_out = buf;
	Wchar* buf_end = buf + buf_size;
	while (buf_out < buf_end - 1 && (!in_text_end || in_text < in_text_end) && *in_text)
	{
		unsigned int c;
		in_text += TextCharFromUtf8(&c, in_text, in_text_end);
		if (c == 0)
			break;
		if (c < 0x10000)    // FIXME: Losing characters that don't fit in 2 bytes
			*buf_out++ = (Wchar)c;
	}
	*buf_out = 0;
	if (in_text_remaining)
		*in_text_remaining = in_text;
	return (int)(buf_out - buf);
}

static inline int ImTextCountUtf8BytesFromChar(unsigned int c)
{
	if (c < 0x80) return 1;
	if (c < 0x800) return 2;
	if (c >= 0xdc00 && c < 0xe000) return 0;
	if (c >= 0xd800 && c < 0xdc00) return 4;
	return 3;
}

int TextCountUtf8BytesFromStr(const Wchar* in_text, const Wchar* in_text_end)
{
	int bytes_count = 0;
	while ((!in_text_end || in_text < in_text_end) && *in_text)
	{
		unsigned int c = (unsigned int)(*in_text++);
		if (c < 0x80)
			bytes_count++;
		else
			bytes_count += ImTextCountUtf8BytesFromChar(c);
	}
	return bytes_count;
}

bool STB_TEXTEDIT_INSERTCHARS(VectorEx<Wchar>* obj, int pos, const Wchar* new_text, int new_text_len, int text_len)
{
	const int new_text_len_utf8 = TextCountUtf8BytesFromStr(new_text, new_text + new_text_len);

	Wchar* text = obj->data();
	
	memmove(text + pos + new_text_len, text + pos, (size_t)(text_len - pos) * sizeof(Wchar));
	memcpy(text + pos, new_text, (size_t)new_text_len * sizeof(Wchar));


	return true;
}

void Strncpy(char* dst, const char* src, int count)
{
	if (count < 1) return;
	strncpy(dst, src, (size_t)count);
	dst[count - 1] = 0;
}

static inline int TextCharToUtf8(char* buf, int buf_size, unsigned int c)
{
	if (c < 0x80)
	{
		buf[0] = (char)c;
		return 1;
	}
	if (c < 0x800)
	{
		if (buf_size < 2) return 0;
		buf[0] = (char)(0xc0 + (c >> 6));
		buf[1] = (char)(0x80 + (c & 0x3f));
		return 2;
	}
	if (c >= 0xdc00 && c < 0xe000)
	{
		return 0;
	}
	if (c >= 0xd800 && c < 0xdc00)
	{
		if (buf_size < 4) return 0;
		buf[0] = (char)(0xf0 + (c >> 18));
		buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
		buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[3] = (char)(0x80 + ((c) & 0x3f));
		return 4;
	}
	//else if (c < 0x10000)
	{
		if (buf_size < 3) return 0;
		buf[0] = (char)(0xe0 + (c >> 12));
		buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[2] = (char)(0x80 + ((c) & 0x3f));
		return 3;
	}
}

int TextStrToUtf8(char* buf, int buf_size, const Wchar* in_text, const Wchar* in_text_end)
{
	char* buf_out = buf;
	const char* buf_end = buf + buf_size;
	while (buf_out < buf_end - 1 && (!in_text_end || in_text < in_text_end) && *in_text)
	{
		unsigned int c = (unsigned int)(*in_text++);
		if (c < 0x80)
			*buf_out++ = (char)c;
		else
			buf_out += TextCharToUtf8(buf_out, (int)(buf_end - buf_out - 1), c);
	}
	*buf_out = 0;
	return (int)(buf_out - buf);
}

char* __ToNormalChar(Wchar chh)
{
	char* buf_out = '\0';
	unsigned int c = (unsigned int)(chh);
	if (c < 0x80)
		*buf_out = (char)c;
	else
		*buf_out = TextCharToUtf8(buf_out, 1, c);
	return buf_out;
}

int str_len(char* str)
{
	int len = 0;
	while (*(str + len) != '\0')
	{
		len++;
	}
	return len;
}

static const char* GetClipboardText()
{
	static VectorEx<char> buf_local;
	buf_local.clear();
	if (!FastCall::G().t_OpenClipboard(NULL))
		return NULL;
	HANDLE wbuf_handle = FastCall::G().t_GetClipboardData(CF_UNICODETEXT);
	if (wbuf_handle == NULL)
		return NULL;
	if (Wchar* wbuf_global = (Wchar*)FastCall::G().t_GlobalLock(wbuf_handle))
	{
		int buf_len = TextCountUtf8BytesFromStr(wbuf_global, NULL) + 1;
		buf_local.resize(buf_len);
		TextStrToUtf8(buf_local.data(), buf_len, wbuf_global, NULL);
	}
	FastCall::G().t_GlobalUnlock(wbuf_handle);
	FastCall::G().t_CloseClipboard();
	return buf_local.data();
}
int TextCountCharsFromUtf8(const char* in_text, const char* in_text_end)
{
	int char_count = 0;
	while ((!in_text_end || in_text < in_text_end) && *in_text)
	{
		unsigned int c;
		in_text += TextCharFromUtf8(&c, in_text, in_text_end);
		if (c == 0)
			break;
		if (c < 0x10000)
			char_count++;
	}
	return char_count;
}
static void SetClipboardText( const char* text)
{
	if (!FastCall::G().t_OpenClipboard(NULL))
		return;
	const int wbuf_length = TextCountCharsFromUtf8(text, NULL) + 1;
	HGLOBAL wbuf_handle = FastCall::G().t_GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)wbuf_length * sizeof(Wchar));
	if (wbuf_handle == NULL)
		return;
	Wchar* wbuf_global = (Wchar*)FastCall::G().t_GlobalLock(wbuf_handle);
	TextStrFromUtf8(wbuf_global, wbuf_length, text, NULL, NULL);
	FastCall::G().t_GlobalUnlock(wbuf_handle);
	FastCall::G().t_EmptyClipboard();
	FastCall::G().t_SetClipboardData(CF_UNICODETEXT, wbuf_handle);
	FastCall::G().t_CloseClipboard();
}

bool CGui::InputText(string Label, char *Buf, size_t BufSize)
{
	DWORD id = GetItemId(Label);
	Vec2 pos = GetCursorPos();
	GuiStyle &style = X1Gui().G_Style;
	GuiIO &IO = X1Gui().G_IO;

	string formated_label = FormatLabel(Label);

	Vec2 label_size = CalcTextSize(formated_label);

	float frame_len = 0;
	if (pThis->ItemWidth == 0)
		frame_len = pThis->Info.Size.x - label_size.x - style.wndPadding.x - 10;
	else
		frame_len = pThis->ItemWidth - label_size.x;

	Vec2 frame_size = Vec2(frame_len, (label_size.y) * 1.2f);
	Rect frame_rect = Rect(pos, pos + frame_size);

	SetCursorPos(Vec2(style.wndPadding.x, GetCursorPosEx().y + frame_size.y + style.itmPadding.y),
		frame_size.x + label_size.x + style.itmInPadding.x + style.itmPadding.x);

	if (CheckSkip(frame_rect))
		return false;

	bool hovered = false;
	bool clicked = ButtonBehavior(frame_rect, hovered);
	bool pressed = X1Gui().G_IO.LMPress;

	color_t frame_color = style.clrFrame;

	if (hovered)
		frame_color = style.clrFrameHover;
	if (hovered && pressed)
		frame_color = style.clrFrameHold;

	DrawFilledBox(frame_rect, frame_color, style.wndRounding);

	if(clicked)
		pThis->UsingItemId = id;
	if (pressed && !hovered && pThis->UsingItemId == id)
		pThis->UsingItemId = -1;

	Vec2 buf_str_size = CalcTextSize(string(Buf));
	
	if (pThis->UsingItemId == id)
	{
		Vec2 cursor_edit_pos = Vec2(frame_rect.min.x +style.itmInPadding.x + buf_str_size.x,
			frame_rect.min.y + (frame_size.y / 2) - (buf_str_size.y / 2));

		DrawLine(cursor_edit_pos, Vec2(cursor_edit_pos.x, cursor_edit_pos.y + buf_str_size.y), Color::White());

		bool com_key_edit = false;

		if (IO.KeyCtrl && IO.KeysClick[int('V')])
		{
			if (const char* clipboard_buf = GetClipboardText())
			{
				int clipboard_len = (int)strlen(clipboard_buf);
				int start_n = strlen(Buf);

				int target_len = (BufSize < clipboard_len ? BufSize : clipboard_len);
				target_len = ((target_len > BufSize - start_n) ? target_len - start_n : target_len);

				for (int i = 0; i < target_len; i++)
				{
					Buf[start_n + i] = clipboard_buf[i];
				}
			}

			com_key_edit = true;
		}

		if (IO.KeyCtrl && IO.KeysClick[int('C')])
		{
			SetClipboardText(Buf);
			com_key_edit = true;
		}

		if (IO.InputCharacters[0] && !IO.BCKBPress && !com_key_edit)
		{		
			size_t start_n = strlen(Buf);

			for (int n = 0; n < ARRAYSIZE(IO.InputCharacters) && IO.InputCharacters[n]; n++)
			{
				if (unsigned int c = (unsigned int)IO.InputCharacters[n])
				{
					if (c != 8)
					{
						Buf[start_n] = (int)c;
						start_n++;
					}
				}
			}

			for (int i = start_n; i < BufSize; i++)
			{
				Buf[i] = '\0';
			}

			memset(IO.InputCharacters, 0, sizeof(IO.InputCharacters));
		}

		if (IO.BCKBClick || IO.KeysDownDuration[VK_BACK] > 20.f)
		{
			size_t start_n = strlen(Buf);
			if(start_n - 1 >= 0)
				Buf[start_n - 1] = '\0';
		}
	}


	string buf_str = Buf;

	buf_str_size = CalcTextSize(buf_str);

	Text(Vec2(frame_rect.min.x + style.itmInPadding.x,
		frame_rect.min.y + (frame_size.y / 2) - (buf_str_size.y / 2)), buf_str.c_str());

	Text(Vec2(frame_rect.max.x + style.itmInPadding.x,
		frame_rect.min.y + (frame_size.y / 2) - (label_size.y / 2)), formated_label.c_str());

	return false;
}

void CGui::BeginGroup()
{
	Group entry;
	entry.oldPos = GetCursorPos();
	entry.newPosEx = GetCursorPosEx();
	entry.oldStyle = GetStyle();
	pThis->GroupStack.push_back(entry);
	pThis->CursorPos = Vec2(0, 0);
}

void CGui::EndGroup(bool NewLine)
{
	GetStyle() = pThis->GroupStack.back().oldStyle;
	Vec2 group_cursor_pos = GetCursorPosEx();
	Vec2 old_pos = pThis->GroupStack.back().newPosEx;
	pThis->GroupStack.pop_back();
	SetCursorPos(Vec2(GetStyle().wndPadding.x, old_pos.y + (NewLine ? 22/*fix this*/ : group_cursor_pos.y)), 0);
}

bool CGui::BeginChild(string Label, Vec2 Size, bool Outline)
{
	DWORD id = GetItemId(("##Labels"));
	Vec2 pos = GetCursorPos();
	GuiStyle &style = X1Gui().G_Style;
	GuiIO &IO = X1Gui().G_IO;

	if (Size.x == 0)
		Size.x = pThis->Info.Size.x - (style.wndPadding.x * 2);

	if (Outline)
	{
		Rect outline_rect = Rect(pos, pos + Size);
		DrawBox(outline_rect, style.wndOutlineTick, style.clrLine, style.wndRounding);
	}

	if (Outline)
	{
		Size.x -= style.wndOutlineTick * 2;
		Size.y -= style.wndOutlineTick * 2;

		pos.x += style.wndOutlineTick;
		pos.y += style.wndOutlineTick;

		pThis->PrevChildOutlined = true;
	}

	return BeginIntenrnalWin(Label, GWF_ChildWindow, Size, pos, style.clrBackground);
}
void CGui::EndChild()
{
	GuiStyle &style = X1Gui().G_Style;
	Vec2 ChildSize = pThis->Info.Size;

	EndIntenrnalWin();

	SetCursorPos(Vec2(style.wndPadding.x, GetCursorPosEx().y + ChildSize.y + style.itmPadding.y + (pThis->PrevChildOutlined ? (style.wndOutlineTick*2) : 0)),
		ChildSize.x + style.itmPadding.x);
}


bool CGui::BeginTreeNode(string Label, Vec2 Size, bool first)
{
	DWORD id = GetItemId(Label);

	bool selected = GetTreeState(id, first);

	if (SelectLabel(Label, false, Size))
		GetTreeState(id, first) = !selected;

	return selected;
}
//void EndTreeNode()
//{
//
//}

void CGui::BeginColumns(string Label, int count)
{
	//DWORD id = GetItemId(("##Labels"));
	//Vec2 pos = GetCursorPos();
	//GuiStyle &style = X1Gui().G_Style;
	//GuiIO &IO = X1Gui().G_IO;

	//float prev_column_ofst = 0;

	//for (int i = 0; i < count; i++)
	//{
	//	Column entry;
	//	entry.idx = i;
	//	entry.offset_x = 100;
	//	pos.x += prev_column_ofst;
	//	prev_column_ofst += entry.offset_x;
	//	//entry.pColumn;
	//	BeginIntenrnalWin(Label, GWF_ChildWindow, Vec2(), pos, style.clrTabLabel);

	//	pThis->ColumnStack.push_back(entry);
	//}
	//ColumnStack
}
void CGui::EndColumns()//�� ����� BeginColumns
{

}
void CGui::NextColumn()
{

}

//####################################################################################################


void CGui::DrawFilledBox(Rect rect, color_t color, float rounding)
{
	rect.min.x = (int)rect.min.x;
	rect.min.y = (int)rect.min.y;
	rect.max.x = (int)rect.max.x;
	rect.max.y = (int)rect.max.y;

	if (rounding > 0.0f)
	{
		pThis->Overlay.PathRect(rect.min, rect.max, rounding, -1);
		pThis->Overlay.PathFillConvex(color.color);
	}
	else
	{
		pThis->Overlay.PrimReserve(6, 4);
		pThis->Overlay.PrimRect(rect.min, rect.max, color.color);
	}
}

void CGui::DrawFilledBox(Vec2 a, Vec2 b, color_t color, float rounding)
{
	X1Gui().DrawFilledBox(Rect(a,  a + b), color, rounding);
}
void CGui::DrawFilledBoxEx(Vec2 a, Vec2 b, color_t color, float rounding)
{
	X1Gui().DrawFilledBox(Rect(a, b), color, rounding);
}

void CGui::DrawBox(Rect rect, int16_t thickness, color_t color, float rounding)
{
	pThis->Overlay.PathRect(rect.min + Vec2(0.5f, 0.5f), rect.max - Vec2(0.5f, 0.5f), rounding, -1);
	pThis->Overlay.PathStroke(color.color, true, thickness);
}

void CGui::DrawBox(Vec2 a, Vec2 b, color_t color, float rounding)
{
	X1Gui().DrawBox(Rect(a, b),  1, color, rounding);
}

void CGui::DrawBox(int16_t x, int16_t y, int16_t width, int16_t height, int16_t thickness, color_t color, float rounding)
{
	X1Gui().DrawBox(Rect(Vec2(x, y), Vec2(x + width, y + height)), thickness, color, rounding);
}

void CGui::DrawLine(Vec2 a, Vec2 b, color_t color, float thickness)
{
	pThis->Overlay.PathLineTo(a + Vec2(0.5f, 0.5f));
	pThis->Overlay.PathLineTo(b + Vec2(0.5f, 0.5f));
	pThis->Overlay.PathStroke(color.color, false, thickness);
}

void CGui::DrawGradientBox(int16_t x, int16_t y, int16_t width, int16_t height, color_t color1, color_t color2, color_t color3, color_t color4)
{
	const Vec2 uv = COverManager::FontTexUvWhitePixel;

	Vec2 a(x, y), c(x + width, y + height);

	pThis->Overlay.PrimReserve(6, 4);

	unsigned int *_VtxCurrentIdx = &pThis->Overlay._VtxCurrentIdx;

	pThis->Overlay.PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx)); pThis->Overlay.PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx + 1)); pThis->Overlay.PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx + 2));
	pThis->Overlay.PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx)); pThis->Overlay.PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx + 2)); pThis->Overlay.PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx + 3));

	pThis->Overlay.PrimWriteVtx(a, uv, color1.color);
	pThis->Overlay.PrimWriteVtx(Vec2(c.x, a.y), uv, color2.color);
	pThis->Overlay.PrimWriteVtx(c, uv, color3.color);
	pThis->Overlay.PrimWriteVtx(Vec2(a.x, c.y), uv, color4.color);
}

void CGui::DrawCircle(int16_t x, int16_t y, int16_t radius, uint16_t points, color_t color)
{
	const float a_max = EX_PI * 2.0f * ((float)points - 1.0f) / (float)points;
	pThis->Overlay.PathArcTo(Vec2(x, y), radius, 0.0f, a_max, points);
	pThis->Overlay.PathFillConvex(color.color);
}

void CGui::DrawRing(int16_t x, int16_t y, int16_t radius, uint16_t points, color_t color, float thickness)
{
	const float a_max = EX_PI * 2.0f * ((float)points - 1.0f) / (float)points;
	pThis->Overlay.PathArcTo(Vec2(x, y), radius - 0.5f, 0.0f, a_max, points);
	pThis->Overlay.PathStroke(color.color, true, thickness);
}

Vec2 CGui::CalcTextSize(string text)
{
	return GP_Render->m_pFont->CalcTextSize(text);
}

void CGui::DrawString(int16_t x, int16_t y, color_t color, bool outlined, bool centered, const char* text)
{
	CFont* font = GP_Render->m_pFont;

	pThis->Overlay.SetTexture(g_FontTexture);

	if (outlined)
	{
		font->RenderText(&pThis->Overlay, font->FontSize, Vec2(x - 1, y), 0xFF000000, text);
		font->RenderText(&pThis->Overlay, font->FontSize, Vec2(x, y - 1), 0xFF000000, text);
		font->RenderText(&pThis->Overlay, font->FontSize, Vec2(x + 1, y), 0xFF000000, text);
		font->RenderText(&pThis->Overlay, font->FontSize, Vec2(x, y + 1), 0xFF000000, text);
	}

	font->RenderText(&pThis->Overlay, font->FontSize, Vec2(x, y), color.color, text);
}

void CGui::DrawString(CFont* font, int16_t x, int16_t y, color_t color, const char* text, ...)
{
	CFont* old_fnt = &(*GP_Render->m_pFont);
	GP_Render->m_pFont = font;

	DrawString(x, y, color, false, false, text);
	GP_Render->m_pFont = old_fnt;
}

void CGui::DrawImage(LPDIRECT3DTEXTURE9 _texture, Vec2 _a, Vec2 _b)
{
	//��������
	//a->b
	//b->c
	//c->d
	//d->a

	Vec2 a = Vec2(_b.x, _a.y);
	Vec2 b = _b;
	Vec2 c = Vec2(_a.x, _b.y);
	Vec2 d = _a;


	Vec2 uv_a(0, 0);
	Vec2 uv_c(1, 1);
	Vec2 uv_b(1, 0);
	Vec2 uv_d(0, 1);

	pThis->Overlay.PushCmdBuffer();

	pThis->Overlay.PrimReserve(6, 4);
	pThis->Overlay.PrimQuadUV(a, b, c, d, uv_b, uv_c, uv_d, uv_a, 0xFFFFFFFF);
	pThis->Overlay.SetTexture(_texture);

	pThis->Overlay.PushCmdBuffer();
}

void CGui::DrawImage(LPDIRECT3DTEXTURE9 _texture, Vec2 a, Vec2 b, Vec2 c, Vec2 d)
{
	Vec2 uv_a(0, 0);
	Vec2 uv_c(1, 1);
	Vec2 uv_b(1, 0);
	Vec2 uv_d(0, 1);

	pThis->Overlay.PushCmdBuffer();

	pThis->Overlay.PrimReserve(6, 4);
	pThis->Overlay.PrimQuadUV(a, b, c, d, uv_b, uv_c, uv_d, uv_a, 0xFFFFFFFF);
	pThis->Overlay.SetTexture(_texture);

	pThis->Overlay.PushCmdBuffer();
}

//####################################################################################################

void CAnimController::Animator100Ex(float &Current, const float SpeedCor1, const float Delta)
{
	//http://yotx.ru/#!1/3_h/ubZwf7Zwf7Rgzhf23/aP9gf2ffiCH8r@1vbmzsre/sbm9sbJ0dbG5cHkD3dnfBWwcHu7vQnf2DfRINu7Fzyng83WI8bl1e7O5v7QMG

	if (Current < 100.f)
	{
		float Progg = (-50.f + Current) * -1.f;
		float Speed = ((Progg * Progg) / ((150.f - (50.f - Progg)) * Delta)) + SpeedCor1 * 2.f;
		float cor =  ((1000 / X1Gui().GetIO().G_Framerate) / 100.f) / (Speed);

		if (cor < 0.7f)
			cor = 0.7f;

		Current += cor;
	}
	if (Current > 100.f)
		Current = 100.f;
}
