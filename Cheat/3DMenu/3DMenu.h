#pragma once
#include  "../Main.h"
#include "../Render/DXOverlayManager.h"

class I3DMenu
{
protected:
	virtual int GetItemId() = 0;
	virtual Vector GetCursorPos() = 0;

	struct Surface_s
	{
		Vector lu;
		Vector ru;
		Vector ld;
		Vector rd;

		Vector lu_2d;
		Vector rd_2d;
		Color color;
	};

	struct Box3D_s
	{
		Surface_s front;
		Surface_s back;
		Color color;
	};

	struct Text_s
	{
		string label;
		Vector pos;
		Color color;
		uint32_t dwFlags;
	};


	virtual Surface_s CalcBackSurface(float distance, Vector Size, bool save_center, Vector offset) = 0;
	virtual Surface_s CalcSurface(Vector min, Vector max, float h = 0) = 0;
	virtual Box3D_s CalcBox(Vector min, Vector max, float h = 0) = 0;


	virtual bool IsHovered(Vector cur, Vector min, Vector max) = 0;
	virtual bool CheckClick() = 0;

	virtual Vec2 IV2(Vector vec) = 0;
	virtual void AngleVectors(const Vector &vAngles, Vector& vForward) = 0;
	virtual void DrawSurface(Surface_s surf, Color color, bool outline = false) = 0;
	virtual void DrawBox(Box3D_s surf, Color color) = 0;
	virtual void CalcMouse() = 0;
	virtual void DrawMouse() = 0;
	virtual void DText(string label, Vector pos, Color color, uint32_t dwFlags) = 0;


	virtual Vector CalcTextSize(string label) = 0;
	virtual void ChangeColor(Color &clr, float cor) = 0;
	virtual bool ButtonBehavior(Vector min, Vector max, bool &hovered, bool dis_check = false) = 0;
	virtual void ClearDraw() = 0;
	virtual void RenderDraw() = 0;

	virtual bool W2S(const Vector& vOrigin, Vector& vScreen) = 0;

	virtual Vector CalcPoint(Vector pnt) = 0;

	virtual void DCheckBox(string label, bool *var) = 0;
	virtual bool DButton(string label, Vector size) = 0;
	virtual void DSliderInt(string label, int *var, int v_min, int v_max) = 0;
	virtual void DSliderFloat(string label, float *var, float v_min, float v_max, bool is_int = false) = 0;
	virtual void DComboBox(string label, int *var, vector<string> items) = 0;

	virtual void SameLine(float w = 0) = 0;
	virtual void PushItemWidth(float w) = 0;

	virtual void UpdateColors() = 0;

	virtual bool Begin(string label, bool open, Vector size, Vector offset, bool back = true) = 0;
	virtual void End() = 0;
};

class C3DMenu : public I3DMenu
{
private:

	/*
			1
			|
		  z |  ,4
			|.'y
			2------3
				x

			   .6-------5
			 .' |    .' |
			7---+--4'   |		7-lu   4-ru
			|   |  |    |
			|  ,0--+----1
			|.'    | .'
			3------2			3-ld  2-rd

	*/

	Color MenuBackColor = Color(18, 18, 22, 190);
	Color ButtonColor = Color(128, 135, 140, 190);
	Color DisableButtonColor = Color(225, 30, 30, 190);
	Color HeaderColor = Color(112, 112, 116, 190);

	Color FrameColor = Color(34, 35, 37, 190);
	Color TextColor = Color(255, 255, 255, 190);

	Vector menu_center_pos;
	Vector menu_size = Vector(150, 1, 80);
	Vector menu_rotate_ang;

	Vector player_view_ang;
	Vector player_view_pos;

	Vector mouse_pos;

	Vector window_padding = Vector(2, 2, 0);
	Vector items_spacing = Vector(2, 2, 0);
	Vector text_padding = Vector(1.5f, 0, 0);

	Vector cursor_pos = Vector(window_padding.x, window_padding.y, 0);
	Vector prev_cursor_pos = Vector(0, 0, 0);
	Vector next_cursor_pos = Vector(0, 0, 0);

	virtual int GetItemId()
	{
		items_num++;
		return items_num;
	}

	virtual Vector GetCursorPos()
	{
		Vector Ent = next_cursor_pos.IsZero() ? cursor_pos : next_cursor_pos;
		next_cursor_pos.Zero();
		return Ent;
	}

	Vector disable_zone_min;
	Vector disable_zone_max;


	virtual Surface_s CalcBackSurface(float distance, Vector Size, bool save_center, Vector offset);
	virtual Surface_s CalcSurface(Vector min, Vector max, float h = 0);
	virtual Box3D_s CalcBox(Vector min, Vector max, float h = 0);

	Box3D_s MenuBackBox;

	//ImFont* menu_font = nullptr;

	struct DrawList_s
	{
		vector<Surface_s> SurfaceList;
		vector<Box3D_s> BoxList;
		vector<Text_s> TextList;

		void AddBox(Box3D_s surf, Color color)
		{
			surf.color = color;
			BoxList.push_back(surf);
		}

		void AddSurface(Surface_s surf, Color color)
		{
			surf.color = color;
			SurfaceList.push_back(surf);
		}

		void AddText(string label, Vector pos, Color color, uint32_t dwFlags)
		{
			Text_s ent;
			ent.label = label;
			ent.pos = pos;
			ent.color = color;
			ent.dwFlags = dwFlags;
			TextList.push_back(ent);
		}

		void Clear()
		{
			SurfaceList.clear();
			BoxList.clear();
			TextList.clear();
		}

		void Draw(C3DMenu *menu_list)
		{
			if (menu_list)
			{
				if (!SurfaceList.empty())
					for (auto v : SurfaceList)
						menu_list->DrawSurface(v, v.color);
				if (!BoxList.empty())
					for (auto v : BoxList)
						menu_list->DrawBox(v, v.color);
				if (!TextList.empty())
					for (auto v : TextList)
						menu_list->DText(v.label, v.pos, v.color, v.dwFlags);
			}
		}
	};

	DrawList_s DrawList_1;
	DrawList_s DrawList_2;
	DrawList_s DrawList_3;

	float distance = 50.f;

	float header_h = 4;

	float items_d = 0.4f;

	float items_width = 100;

	float text_scale = 0.183f;

	virtual bool IsHovered(Vector cur, Vector min, Vector max);
	virtual bool CheckClick();

	virtual Vec2 IV2(Vector vec)
	{
		return Vec2(vec.x, vec.y);
	}

	virtual void AngleVectors(const Vector &vAngles, Vector& vForward)
	{
		float	sp, sy, cp, cy;

		sy = sin(DEG2RAD(vAngles[1]));
		cy = cos(DEG2RAD(vAngles[1]));

		sp = sin(DEG2RAD(vAngles[0]));
		cp = cos(DEG2RAD(vAngles[0]));

		vForward.x = cp * cy;
		vForward.y = cp * sy;
		vForward.z = -sp;
	}

	virtual void DrawSurface(Surface_s surf, Color color, bool outline = false);
	virtual void DrawBox(Box3D_s surf, Color color);
	virtual void CalcMouse();
	virtual void DrawMouse();
	virtual void DText(string label, Vector pos, Color color, uint32_t dwFlags)
	{

	}

	virtual Vector CalcTextSize(string label)
	{
		return Vector(0, 0, 0);
	}

	virtual void ChangeColor(Color &clr, float cor);

	virtual bool ButtonBehavior(Vector min, Vector max, bool &hovered, bool dis_check = false);

	int items_num = 0;
	int selected_combo = -1;

	virtual void ClearDraw();
	virtual void RenderDraw();
public:

	virtual bool W2S(const Vector& vOrigin, Vector& vScreen);

	virtual Vector CalcPoint(Vector pnt);
	Vector mouse_pos_2d;

	virtual void DCheckBox(string label, bool *var);
	virtual bool DButton(string label, Vector size);
	virtual void DSliderInt(string label, int *var, int v_min, int v_max);
	virtual void DSliderFloat(string label, float *var, float v_min, float v_max, bool is_int = false);
	virtual void DComboBox(string label, int *var, vector<string> items);

	virtual void SameLine(float w = 0);
	virtual void PushItemWidth(float w);

	virtual void UpdateColors();

	virtual bool Begin(string label, bool open, Vector size, Vector offset, bool back = true);
	virtual void End();

	bool IsClicked = false;
	bool IsHold = false;
};