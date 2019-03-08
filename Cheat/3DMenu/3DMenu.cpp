#include "3DMenu.h"
#include "../Hacks/Setup.h"


bool C3DMenu::Begin(string label, bool open, Vector size, Vector offset, bool back)
{
	static bool OldOpen = false;

	CBaseEntity* plocal = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());
	if (plocal)
	{
		I::Engine()->GetViewAngles(player_view_ang);
		player_view_pos = plocal->GetRenderOrigin() + plocal->GetViewOffset();
	}

	if (/*OldOpen != */!open)
	{
		menu_size = size;
		MenuBackBox.front = CalcBackSurface(122.f, menu_size, true, offset);
		if(back)
			MenuBackBox.back = CalcBackSurface(122.f + menu_size.y, menu_size, false, offset);

		menu_rotate_ang = player_view_ang;
	}

	OldOpen = open;

	items_num = 0;

	ClearDraw();

	if (!open)
		return false;

	//menu_font = FontMain[17];

	IsClicked = CheckClick();

	CalcMouse();

	disable_zone_min = Vector(0, 0, 0);
	disable_zone_max = Vector(0, 0, 0);

	if (!back)
		return true;

	if (label.size() > 0)
	{
		Box3D_s header_box = MenuBackBox;

		header_box.front.ld.z = header_box.front.lu.z - header_h;
		header_box.front.rd.z = header_box.front.ld.z;

		header_box.back.ld.z = header_box.front.ld.z;
		header_box.back.rd.z = header_box.front.rd.z;

		cursor_pos = Vector(window_padding.x, window_padding.y + header_h, 0);

		DrawList_1.AddBox(MenuBackBox, MenuBackColor);

		DrawList_1.AddBox(header_box, HeaderColor);
		DrawList_1.AddText(label, Vector(1, 0.3f, 0), TextColor, DT_LEFT);
	}
	else
	{
		DrawList_1.AddBox(MenuBackBox, MenuBackColor);
		cursor_pos = Vector(window_padding.x, window_padding.y, 0);
	}
}
void C3DMenu::End()
{
	RenderDraw();
	DrawMouse();
}
C3DMenu::Surface_s C3DMenu::CalcBackSurface(float distance, Vector Size, bool save_center, Vector offset)
{
	Surface_s entry;

	Vector direction;
	Vector center_left;
	Vector center_right;
	Vector view = player_view_ang;
	view.x = 0;

	direction = CGlobal::CalcDir(view);

	Vector center = player_view_pos + (direction * distance);

	center = center + (CGlobal::CalcDir(Vector(0, view.y - 90,0)) * offset.x);
	center = center + (CGlobal::CalcDir(Vector(90, view.y, 0)) * offset.y);


	if (save_center)
		menu_center_pos = center;

	AngleVectors(Vector(view.x, view.y - 90, view.z), direction);
	center_right = center + (direction * (Size.x / 2));

	AngleVectors(Vector(view.x, view.y + 90, view.z), direction);
	center_left = center + (direction * (Size.x / 2));

	entry.lu = Vector(center_left.x, center_left.y, center.z + (Size.z / 2));
	entry.ru = Vector(center_right.x, center_right.y, center.z + (Size.z / 2));

	entry.ld = Vector(center_left.x, center_left.y, center.z - (Size.z / 2));
	entry.rd = Vector(center_right.x, center_right.y, center.z - (Size.z / 2));

	entry.lu_2d = Vector(0, 0, 0);
	entry.rd_2d = Vector(Size.x, Size.z, 0);

	return entry;
}

C3DMenu::Surface_s C3DMenu::CalcSurface(Vector min, Vector max, float h)
{
	Surface_s entry;

	Vector view = menu_rotate_ang;
	Vector start = MenuBackBox.front.lu;

	entry.lu = start + (CGlobal::CalcDir(Vector(0, view.y - 90, 0)) * min.x);

	if (h != 0)
		entry.lu = entry.lu + (CGlobal::CalcDir(Vector(0, view.y - 180, 0)) * h);

	entry.lu = entry.lu + (CGlobal::CalcDir(Vector(90, view.y, 0)) * min.y);
	entry.ru = entry.lu + (CGlobal::CalcDir(Vector(0, view.y - 90, 0)) * (max.x - min.x));

	entry.ld = entry.lu + (CGlobal::CalcDir(Vector(90, view.y, 0)) * (max.y - min.y));
	entry.rd = entry.ld + (CGlobal::CalcDir(Vector(0, view.y - 90, 0)) * (max.x - min.x));

	return entry;
}

Vector C3DMenu::CalcPoint(Vector pnt)
{
	Vector entry = MenuBackBox.front.lu + (CGlobal::CalcDir(Vector(0, menu_rotate_ang.y - 90, 0)) * pnt.x);
	entry = entry + (CGlobal::CalcDir(Vector(90, menu_rotate_ang.y, 0)) * pnt.y);

	return entry;
}

C3DMenu::Box3D_s C3DMenu::CalcBox(Vector min, Vector max, float h)
{
	Box3D_s entry;

	entry.back = CalcSurface(min, max);
	entry.front = CalcSurface(min, max, h);

	return entry;
}

void C3DMenu::DrawSurface(Surface_s surf, Color color, bool outline)
{
	Surface_s surf_screen;

	if (W2S(surf.lu, surf_screen.lu) && W2S(surf.ru, surf_screen.ru) &&
		W2S(surf.ld, surf_screen.ld) && W2S(surf.rd, surf_screen.rd))
	{
		/*GP_IRender->render_Box(
			ImVec2(IV2(surf_screen.lu)), ImVec2(IV2(surf_screen.ru)),
			ImVec2(IV2(surf_screen.rd)), ImVec2(IV2(surf_screen.ld)),
			color.GetImColor());

		if (outline)
		{
			GP_IRender->render_Border(
				ImVec2(IV2(surf_screen.lu)), ImVec2(IV2(surf_screen.ru)),
				ImVec2(IV2(surf_screen.rd)), ImVec2(IV2(surf_screen.ld)),
				Color(255, 255, 255, 40).GetImColor());
		}*/
	}
}

void C3DMenu::DrawBox(Box3D_s surf, Color color)
{
	Surface_s left;
	Surface_s right;
	Surface_s top;
	Surface_s down;

	left.lu = surf.back.lu;
	left.ru = surf.front.lu;

	left.ld = surf.back.ld;
	left.rd = surf.front.ld;

	right.lu = surf.back.ru;
	right.ru = surf.front.ru;

	right.ld = surf.back.rd;
	right.rd = surf.front.rd;

	top.lu = surf.back.lu;
	top.ru = surf.back.ru;

	top.ld = surf.front.lu;
	top.rd = surf.front.ru;

	down.lu = surf.back.ld;
	down.ru = surf.back.rd;

	down.ld = surf.front.ld;
	down.rd = surf.front.rd;

	DrawSurface(surf.back, color);

	DrawSurface(left, color, true);
	DrawSurface(right, color, true);
	DrawSurface(top, color, true);
	DrawSurface(down, color, true);

	DrawSurface(surf.front, color);
}

void C3DMenu::CalcMouse()
{
	Vector direction;

	float dist_c_to_start = Vector(MenuBackBox.front.ru - menu_center_pos).Length2d();
	float dist_to_start = Vector(MenuBackBox.front.ru - player_view_pos).Length2d();
	float dist_to_center = Vector(menu_center_pos - player_view_pos).Length2d();

	float ang_betw_surf = RAD2DEG(acos((pow(dist_to_center, 2) + pow(dist_c_to_start, 2) - pow(dist_to_start, 2)) /
		(2 * dist_to_center * dist_c_to_start))) - 180;

	float dist_on_surf = dist_to_center * sin(DEG2RAD(90 - ang_betw_surf));

	AngleVectors(Vector(0, menu_rotate_ang.y + 90, 0), direction);

	Vector cursor_surf_pos = menu_center_pos + (direction * dist_on_surf);

	float dist_to_surf = Vector(cursor_surf_pos - player_view_pos).Length2d();

	float pos_x = (tan(DEG2RAD(player_view_ang.y - menu_rotate_ang.y)) * dist_to_surf) + dist_on_surf;
	float pos_z = dist_to_surf * tan(DEG2RAD(player_view_ang.x));

	cursor_surf_pos.z = player_view_pos.z - pos_z;

	mouse_pos_2d = Vector((menu_size.x / 2) - pos_x,
		menu_size.z - (cursor_surf_pos.z - MenuBackBox.front.ld.z), 0);

	mouse_pos = menu_center_pos + (direction * pos_x);
	mouse_pos.z = cursor_surf_pos.z;
}

void C3DMenu::DrawMouse()
{
	float line_len = 1.f;

	Vector center = mouse_pos;

	if (!IsHovered(mouse_pos_2d, MenuBackBox.front.lu_2d, MenuBackBox.front.rd_2d))
		return;

	Vector direction;
	Vector left;
	Vector right;
	Vector top;
	Vector down;

	Vector left_screen;
	Vector right_screen;
	Vector top_screen;
	Vector down_screen;

	AngleVectors(Vector(0, menu_rotate_ang.y + 90, 0), direction);
	left = center + (direction * line_len);
	AngleVectors(Vector(0, menu_rotate_ang.y - 90, 0), direction);
	right = center + (direction * line_len);

	AngleVectors(Vector(-90, menu_rotate_ang.y, 0), direction);
	top = center + (direction * line_len);
	AngleVectors(Vector(90, menu_rotate_ang.y, 0), direction);
	down = center + (direction * line_len);

	if (W2S(left, left_screen) && W2S(right, right_screen) &&
		W2S(top, top_screen) && W2S(down, down_screen))
	{
		//GP_IRender->render_Line(ImVec2(left_screen.x, left_screen.y), ImVec2(right_screen.x, right_screen.y), Color::Aqua().GetImColor());
		//GP_IRender->render_Line(ImVec2(top_screen.x, top_screen.y), ImVec2(down_screen.x, down_screen.y), Color::Aqua().GetImColor());
	}
}
#define ITEM_SIZE_Y 5

#define CHECK_BOX_SZ_X  10

void C3DMenu::DCheckBox(string label, bool *var)
{
	int item_id = GetItemId();

	Vector min = GetCursorPos();
	Vector max = Vector(min.x + CHECK_BOX_SZ_X, min.y + ITEM_SIZE_Y, 0);

	Color color = DisableButtonColor;
	Color frame_color = FrameColor;

	Vector main_min = min;
	Vector main_max = Vector(min.x + (CHECK_BOX_SZ_X / 2), min.y + ITEM_SIZE_Y, 0);

	Vector text_size = CalcTextSize(label);
	Vector text_pos = Vector(max.x + text_padding.x, main_min.y + (ITEM_SIZE_Y / 2) - (text_size.y / 2), 0);

	if (*var)
	{
		color = ButtonColor;

		main_min = Vector(min.x + (CHECK_BOX_SZ_X / 2), min.y, 0);
		main_max = Vector(min.x + CHECK_BOX_SZ_X, min.y + ITEM_SIZE_Y, 0);
	}

	bool hovrd = false;
	bool click = ButtonBehavior(min, max, hovrd);

	if (hovrd)
	{
		ChangeColor(color, -30.f);
		ChangeColor(frame_color, -30.f);
	}

	if (click)
	{
		ChangeColor(color, 30.f);
		*var = !*var;
	}

	Box3D_s main_box = CalcBox(main_min, main_max, items_d);
	Box3D_s frame_box = CalcBox(min, max, items_d);

	DrawList_1.AddBox(frame_box, frame_color);
	DrawList_1.AddBox(main_box, color);

	DrawList_1.AddText(label, text_pos, TextColor, DT_LEFT);

	prev_cursor_pos = Vector(max.x + (text_padding.x * 2) + text_size.x, min.y, 0);

	cursor_pos = Vector(window_padding.x, max.y + items_spacing.y, 0);
}

bool C3DMenu::DButton(string label, Vector size)
{
	int item_id = GetItemId();
	Vector min = GetCursorPos();
	Vector max = Vector(min.x + size.x, min.y + size.y, 0);

	Vector text_size = CalcTextSize(label);

	Vector text_pos = Vector(min.x + (size.x / 2) - (text_size.x / 2), min.y + (size.y / 2) - (text_size.y / 2), 0);

	Color color = ButtonColor;

	bool hovrd = false;
	bool click = ButtonBehavior(min, max, hovrd);

	if (hovrd)
		ChangeColor(color, -30.f);

	if (click)
		ChangeColor(color, 30.f);

	DrawList_1.AddBox(CalcBox(min, max, items_d), color);
	DrawList_1.AddText(label, text_pos, TextColor, DT_LEFT | DT_CENTER);

	prev_cursor_pos = Vector(max.x, min.y, 0);
	cursor_pos = Vector(window_padding.x, max.y + items_spacing.y, 0);

	return hovrd && click;
}

void C3DMenu::DSliderInt(string label, int *var, int v_min, int v_max)
{
	float v_f = (float)*var;
	DSliderFloat(label, &v_f, v_min, v_max, true);
	(*var) = (int)v_f;
}
void C3DMenu::DSliderFloat(string label, float *var, float v_min, float v_max, bool is_int)
{
	int item_id = GetItemId();
	Vector frame_min = GetCursorPos();
	Vector frame_max = Vector(frame_min.x + items_width, frame_min.y + ITEM_SIZE_Y, 0);

	Vector text_size = CalcTextSize(label);
	Vector text_pos = Vector(frame_max.x + text_padding.x, frame_min.y + (ITEM_SIZE_Y / 2) - (text_size.y / 2), 0);

	float full_len = (frame_max.x - frame_min.x) - 1.f;
	float powr = full_len / (v_max - v_min);

	float main_w = powr * (*var);

	bool hovrd = false;
	ButtonBehavior(frame_min, frame_max, hovrd);

	if (hovrd && IsHold && mouse_pos_2d.x > frame_min.x + 0.5f &&
		mouse_pos_2d.x < frame_max.x - 0.5f)
	{
		main_w = mouse_pos_2d.x - (frame_min.x + 0.5f);
	}

	Vector main_min = Vector(frame_min.x + 0.5f, frame_min.y + 0.5f, 0);
	Vector main_max = Vector(main_min.x + main_w, frame_min.y + ITEM_SIZE_Y - 0.5f, 0);

	Color color = ButtonColor;
	Color frame_color = FrameColor;

	if (hovrd)
	{
		ChangeColor(color, -30.f);
		ChangeColor(frame_color, -30.f);
	}

	if (IsHold && hovrd)
		ChangeColor(color, 30.f);

	*var = main_w / powr;

	if (*var > v_max)
		*var = v_max;

	if (*var < v_min)
		*var = v_min;

	Box3D_s main_box = CalcBox(main_min, main_max, items_d);
	Box3D_s frame_box = CalcBox(frame_min, frame_max, items_d);

	DrawList_1.AddBox(frame_box, frame_color);
	DrawList_1.AddBox(main_box, color);

	DrawList_1.AddText(label, text_pos, Color::White(), DT_LEFT);

	if (!is_int)
	{
		char* var_buf = new char[128];
		std::sprintf(var_buf, "%6.1lf", (*var));

		Vector var_text_size = CalcTextSize(string(var_buf));
		Vector var_text_pos = Vector(frame_min.x + (items_width / 2) - (var_text_size.x / 2), frame_min.y + (ITEM_SIZE_Y / 2) - (var_text_size.y / 2), 0);

		DrawList_1.AddText(string(var_buf), var_text_pos, TextColor, DT_LEFT | DT_CENTER);
		delete[] var_buf;
	}
	else
	{
		Vector var_text_size = CalcTextSize(to_string((int)(*var)));
		Vector var_text_pos = Vector(frame_min.x + (items_width / 2) - (var_text_size.x / 2), frame_min.y + (ITEM_SIZE_Y / 2) - (var_text_size.y / 2), 0);

		DrawList_1.AddText(to_string((int)(*var)), var_text_pos, TextColor, DT_LEFT | DT_CENTER);
	}

	prev_cursor_pos = Vector(frame_max.x + (text_padding.x * 2) + text_size.x, frame_max.y, 0);
	cursor_pos = Vector(window_padding.x, frame_max.y + items_spacing.y, 0);
}

void C3DMenu::DComboBox(string label, int *var, vector<string> items)
{
	int item_id = GetItemId();

	Vector frame_min = GetCursorPos();
	Vector frame_max = Vector(frame_min.x + items_width, frame_min.y + ITEM_SIZE_Y, 0);

	Vector right_min = Vector(frame_max.x - 5, frame_min.y, 0);
	Vector right_max = Vector(frame_max.x, frame_max.y, 0);

	Vector triangle_min = Vector(right_min.x + 1.3f, right_min.y + 1.f, 0);
	Vector triangle_max = Vector(right_max.x - 1.3f, right_max.y - 1.f, 0);

	Color color = ButtonColor;
	Color frame_color = FrameColor;

	Box3D_s triangle_box = CalcBox(triangle_min, triangle_max, items_d);

	triangle_box.front.ru.z -= (triangle_max.y - triangle_min.y) / 2;
	triangle_box.front.rd.z += (triangle_max.y - triangle_min.y) / 2;

	triangle_box.back.ru.z -= (triangle_max.y - triangle_min.y) / 2;
	triangle_box.back.rd.z += (triangle_max.y - triangle_min.y) / 2;

	bool hovrd = false;
	bool click = ButtonBehavior(frame_min, frame_max, hovrd);

	if (hovrd)
		ChangeColor(frame_color, -30.f);

	if (click && hovrd)
		selected_combo = item_id;

	if (selected_combo == item_id)
	{
		disable_zone_min = Vector(frame_min.x - 1, frame_min.y - 1, 0);
		disable_zone_max = Vector(frame_max.x + 1, frame_max.y + (ITEM_SIZE_Y * items.size()) + 1, 0);

		Vector prev_max = frame_max;
		for (size_t i(0); i < items.size(); i++)
		{
			Color popup_color = FrameColor;

			Vector popup_min = Vector(frame_min.x, prev_max.y, 0);
			Vector popup_max = Vector(popup_min.x + items_width, popup_min.y + ITEM_SIZE_Y, 0);

			bool pop_hovrd = false;
			bool pop_click = ButtonBehavior(popup_min, popup_max, pop_hovrd, true);

			if (*var == i)
				popup_color = color;

			if (pop_hovrd)
				ChangeColor(popup_color, -30.f);

			DrawList_2.AddBox(CalcBox(popup_min, popup_max, items_d), popup_color);

			Vector text_size = CalcTextSize(items[i]);
			Vector text_pos = Vector(popup_min.x + text_padding.x, popup_min.y + (ITEM_SIZE_Y / 2) - (text_size.y / 2), 0);

			DrawList_2.AddText(items[i], text_pos, TextColor, DT_LEFT);

			if (pop_click)
				*var = i;

			prev_max = popup_max;
		}
	}

	//IsClicked - global click
	if (IsClicked && !hovrd && selected_combo == item_id)
		selected_combo = -1;

	DrawList_1.AddBox(CalcBox(frame_min, frame_max, items_d), frame_color);
	DrawList_1.AddBox(CalcBox(right_min, right_max, items_d), color);
	DrawList_1.AddBox(triangle_box, Color(255, 255, 255, 100));

	if (*var >= 0 && *var < (int)items.size())
	{
		Vector selct_text_size = CalcTextSize(items[*var]);
		Vector selct_text_pos = Vector(frame_min.x + text_padding.x, frame_min.y + (ITEM_SIZE_Y / 2) - (selct_text_size.y / 2), 0);
		DrawList_1.AddText(items[*var], selct_text_pos, TextColor, DT_LEFT);
	}

	Vector label_text_size = CalcTextSize(label);
	Vector label_text_pos = Vector(frame_max.x + text_padding.x, frame_min.y + (ITEM_SIZE_Y / 2) - (label_text_size.y / 2), 0);
	DrawList_1.AddText(label, label_text_pos, TextColor, DT_LEFT);

	prev_cursor_pos = Vector(frame_max.x + (text_padding.x * 2) + label_text_size.x, frame_min.y, 0);
	cursor_pos = Vector(window_padding.x, frame_max.y + items_spacing.y, 0);
}

void C3DMenu::SameLine(float w)
{
	if (w != 0)
		next_cursor_pos = Vector(window_padding.x + w, prev_cursor_pos.y, 0);
	else
		next_cursor_pos = Vector(prev_cursor_pos.x + items_spacing.x, prev_cursor_pos.y, 0);
}

void C3DMenu::ChangeColor(Color &clr, float cor)
{
	if (clr[0] + cor > 0 && clr[0] + cor <= 255)
		clr[0] += cor;
	if (clr[1] + cor > 0 && clr[1] + cor <= 255)
		clr[1] += cor;
	if (clr[2] + cor > 0 && clr[2] + cor <= 255)
		clr[2] += cor;
}

bool C3DMenu::ButtonBehavior(Vector min, Vector max, bool &hovered, bool dis_check)
{
	if ((IsHovered(min, disable_zone_min, disable_zone_max) ||
		IsHovered(max, disable_zone_min, disable_zone_max)) && !dis_check)
		return false;

	hovered = IsHovered(mouse_pos_2d, min, max);

	return hovered && IsClicked;
}

bool C3DMenu::IsHovered(Vector cur, Vector min, Vector max)
{
	return cur.WithinAABox(min, max);
}

void C3DMenu::PushItemWidth(float w)
{
	items_width = w;
}

void C3DMenu::ClearDraw()
{
	DrawList_1.Clear();
	DrawList_2.Clear();
	DrawList_3.Clear();
}
void C3DMenu::RenderDraw()
{
	DrawList_1.Draw(this);
	DrawList_2.Draw(this);
	DrawList_3.Draw(this);
}

bool C3DMenu::CheckClick()
{
	static bool is_down = false;
	static bool is_clicked = false;

	IsHold = FastCall::G().t_GetAsyncKeyState(1);

	if (IsHold)
	{
		is_clicked = false;
		is_down = true;
	}
	else if (!IsHold && is_down)
	{
		is_clicked = true;
		is_down = false;
	}
	else
	{
		is_clicked = false;
		is_down = false;
	}

	if (is_clicked)
		return true;

	return false;
}

bool C3DMenu::W2S(const Vector& vOrigin, Vector& vScreen)
{
	return false/* WorldToScreen(vOrigin, vScreen)*/;
}

void C3DMenu::UpdateColors()
{
	/*MenuBackColor = MainCheatConfig.BackgroundColor;
	ButtonColor = MainCheatConfig.ButtonColor;
	DisableButtonColor = MainCheatConfig.DisableButtonColor;
	HeaderColor = MainCheatConfig.TitleColor;
	FrameColor = MainCheatConfig.FrameColor;
	TextColor = MainCheatConfig.TextColor;*/

	MenuBackColor[3] = 190.f;
	ButtonColor[3] = 190.f;
	DisableButtonColor[3] = 190.f;
	HeaderColor[3] = 190.f;
	FrameColor[3] = 190.f;
}
