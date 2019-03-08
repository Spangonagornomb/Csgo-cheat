#include "DXRender.h"
#include "DXFont.h"

void CRender::IRender::DrawStringNew(int16_t x, int16_t y, color_t color, bool outlined, bool centered, const char* text, ...)
{
	va_list args;
	char buf[256];
	va_start(args, text);
	vsprintf_s(buf, text, args);
	va_end(args);

	//m_pDrawMngr->PushCmdBuffer();
	m_pDrawMngr->SetTexture(g_FontTexture);

	if (outlined)
	{
		m_pFont->RenderText(m_pDrawMngr, m_pFont->FontSize, Vec2(x - 1, y), 0x4D000000, buf);
		m_pFont->RenderText(m_pDrawMngr, m_pFont->FontSize, Vec2(x, y - 1), 0x4D000000, buf);
		m_pFont->RenderText(m_pDrawMngr, m_pFont->FontSize, Vec2(x + 1, y), 0x4D000000, buf);
		m_pFont->RenderText(m_pDrawMngr, m_pFont->FontSize, Vec2(x, y + 1), 0x4D000000, buf);
	}

	m_pFont->RenderText(m_pDrawMngr, m_pFont->FontSize, Vec2(x, y), color.color, buf);

	//m_pDrawMngr->PushCmdBuffer();
}
void CRender::IRender::DrawString(int16_t x, int16_t y, color_t color, bool outlined, bool centered, const char* text)
{
	if (int(&color.color + 3) <= 32)
		return;

	m_pDrawMngr->SetTexture(g_FontTexture);

	if (centered)
	{
		Vec2 text_size = CalcTextSize(text);
		x -= text_size.x / 2;
	}



	if (outlined)
	{
		D3DCOLOR color_rw = color.color;

		int max_alpha = 170;
		if (int(&color_rw + 3) < max_alpha)
			max_alpha = int(&color_rw + 3);

		color_t outline_clr = color_t(0,0,0, max_alpha);

		m_pFont->RenderText(m_pDrawMngr, m_pFont->FontSize, Vec2(x - 1, y), outline_clr, text);
		m_pFont->RenderText(m_pDrawMngr, m_pFont->FontSize, Vec2(x, y - 1), outline_clr, text);
		m_pFont->RenderText(m_pDrawMngr, m_pFont->FontSize, Vec2(x + 1, y), outline_clr, text);
		m_pFont->RenderText(m_pDrawMngr, m_pFont->FontSize, Vec2(x, y + 1), outline_clr, text);
	}

	m_pFont->RenderText(m_pDrawMngr, m_pFont->FontSize, Vec2(x, y), color.color, text);

	//m_pDrawMngr->PushCmdBuffer();
}

void CRender::IRender::DrawString(Vec2 Pos, color_t color, const char* text)
{
	DrawString(Pos.x, Pos.y, 15, color, false, false, text);
}
void CRender::IRender::DrawString(CFont* font, Vec2 Pos, color_t color, const char* text)
{
	CFont* oldFnt = &(*m_pFont);
	SetFont(font);
	DrawString(Pos.x, Pos.y, color, false, false, text);
	SetFont(oldFnt);
}

void CRender::IRender::DrawString(int size, Vec2 Pos, color_t color, const char* text)
{
	DrawString(Pos.x, Pos.y, size, color, false, false, text);
}
//void CRender::IRender::DrawString(int size, Vec2 Pos, color_t color, bool outlined, bool centered, const char* text)
//{
//	DrawString(Pos.x, Pos.y, size, color, outlined, centered, text);
//}
void CRender::IRender::DrawString(int size, Vec2 Pos, color_t color, bool outlined, bool centered, const char* text, ...)
{
	va_list args;
	char buf[256];
	va_start(args, text);
	vsprintf_s(buf, text, args);
	va_end(args);
	DrawString(Pos.x, Pos.y, size, color, outlined, centered, buf);
}

void CRender::IRender::DrawStringIcon(int size, Vec2 Pos, color_t color, const char* text)
{
	this->DrawIcon(Pos.x, Pos.y, size, color, false, false, text);
}
void CRender::IRender::DrawStringIcon(int size, Vec2 Pos, color_t color, bool outlined, bool centered, const char* text)
{
	this->DrawIcon(Pos.x, Pos.y, size, color, outlined, centered, text);
}


void CRender::IRender::DrawStringIcon(Vec2 Pos, color_t color, const char* text)
{
	this->DrawIcon(Pos.x, Pos.y, 15, color, false, false, text);
}

void CRender::IRender::DrawString(int16_t x, int16_t y, int size, color_t color, bool outlined, bool centered, const char* text)
{
	CFont* oldFnt = &(*m_pFont);
	if (size > MAX_FONT_SIZE)
		size = MAX_FONT_SIZE;

	SetFont(SzFonts[size]);

	DrawString(x, y, color, outlined, centered, text);
	SetFont(oldFnt);
}
void CRender::IRender::DrawIcon(int16_t x, int16_t y, int size, color_t color, bool outlined, bool centered, const char* text)
{
	CFont* oldFnt = &(*m_pFont);

	if (size > MAX_FONT_SIZE)
		size = MAX_FONT_SIZE;

	SetFont(SzFontsIcon[size]);
	DrawString(x, y, color, outlined, centered, text);
	SetFont(oldFnt);
}

void CRender::IRender::DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, color_t color, float thickness)
{
	Vec2 a(x1, y1), b(x2, y2);

	m_pDrawMngr->PathLineTo(a + Vec2(0.5f, 0.5f));
	m_pDrawMngr->PathLineTo(b + Vec2(0.5f, 0.5f));
	m_pDrawMngr->PathStroke(color.color, false, thickness);
}

void CRender::IRender::DrawLine(Vec2 a, Vec2 b, color_t color, float thickness)
{
	m_pDrawMngr->PathLineTo(a + Vec2(0.5f, 0.5f));
	m_pDrawMngr->PathLineTo(b + Vec2(0.5f, 0.5f));
	m_pDrawMngr->PathStroke(color.color, false, thickness);
}

void CRender::IRender::DrawFilledBox(int16_t x, int16_t y, int16_t width, int16_t height, color_t color, float rounding)
{
	Vec2 a(x, y), b(x + width, y + height);

	int rounding_corners_flags = -1;

	if (rounding > 0.0f)
	{
		m_pDrawMngr->PathRect(a, b, rounding, rounding_corners_flags);
		m_pDrawMngr->PathFillConvex(color.color);
	}
	else
	{
		m_pDrawMngr->PrimReserve(6, 4);
		m_pDrawMngr->PrimRect(a, b, color.color);
	}	
}

void CRender::IRender::DrawFilledBox(Vec2 pos, Vec2 size, color_t color, float rounding)
{
	Vec2 a(pos), b(pos.x + size.x, pos.y + size.y);

	int rounding_corners_flags = -1;

	if (rounding > 0.0f)
	{
		m_pDrawMngr->PathRect(a, b, rounding, rounding_corners_flags);
		m_pDrawMngr->PathFillConvex(color.color);
	}
	else
	{
		m_pDrawMngr->PrimReserve(6, 4);
		m_pDrawMngr->PrimRect(a, b, color.color);
	}
}

void CRender::IRender::DrawFilledBox(Rect rect, color_t color, float rounding)
{
	Vec2 a(rect.min), b(rect.max);

	int rounding_corners_flags = -1;

	if (rounding > 0.0f)
	{
		m_pDrawMngr->PathRect(a, b, rounding, rounding_corners_flags);
		m_pDrawMngr->PathFillConvex(color.color);
	}
	else
	{
		m_pDrawMngr->PrimReserve(6, 4);
		m_pDrawMngr->PrimRect(a, b, color.color);
	}
}

void CRender::IRender::DrawBox(int16_t x, int16_t y, int16_t width, int16_t height, int16_t thickness, color_t color, float rounding)
{
	Vec2 a(x, y), b(x + width, y + height);
	m_pDrawMngr->PathRect(a + Vec2(0.5f, 0.5f), b - Vec2(0.5f, 0.5f), rounding, -1);
	m_pDrawMngr->PathStroke(color.color, true, thickness);
}
void CRender::IRender::DrawCoalBox(int16_t x, int16_t y, int16_t w, int16_t h, int16_t thickness, color_t color, bool outline, float del)
{
	float iw = w / del;
	float ih = h / del;


	DrawLine(Vec2(x - 0.5f, y), Vec2(x + iw, y), color, thickness);// |--
	DrawLine(Vec2(x, y - 0.5f), Vec2(x, y + ih), color, thickness);// |

	DrawLine(Vec2(x + w - iw, y), Vec2(x + w + 0.5f, y), color, thickness);// --|
	DrawLine(Vec2(x + w, y - 0.5f), Vec2(x + w, y + ih), color, thickness);//   |

	DrawLine(Vec2(x, y + h - ih), Vec2(x, y + h - 0.5f), color, thickness);// |
	DrawLine(Vec2(x - 0.5f, y + h), Vec2(x + iw, y + h), color, thickness);// |--

	DrawLine(Vec2(x + w, y + h - ih), Vec2(x + w, y + h + 0.5f), color, thickness);//   |
	DrawLine(Vec2(x + w + 0.5f, y + h), Vec2(x + w - iw, y + h), color, thickness);// --|

	if (outline)
	{
		color.set_r(0);
		color.set_g(0);
		color.set_b(0);

		DrawCoalBox(x - 1, y - 1, w + 2, h + 2, thickness, color, false, del);
		DrawCoalBox(x + 1, y + 1, w - 2, h - 2, thickness, color, false, del);

		DrawLine(Vec2(x + iw, y + 1), Vec2(x + iw, y - 1 - 0.5f), color, thickness);
		DrawLine(Vec2(x + w - iw, y + 1), Vec2(x + w - iw, y - 1 - 0.5f), color, thickness);
		DrawLine(Vec2(x + 1, y + ih), Vec2(x - 1  - 0.5f, y + ih), color, thickness);
		DrawLine(Vec2(x + 1 + w + 0.5f, y + ih), Vec2(x - 1 + w, y + ih), color, thickness);

		DrawLine(Vec2(x + 1, y + h - ih), Vec2(x - 1 - 0.5f, y + h - ih), color, thickness);
		DrawLine(Vec2(x + iw, y + 1 + h + 0.5f), Vec2(x + iw, y - 1 + h), color, thickness);
		DrawLine(Vec2(x + 1 + w + 0.5f, y + h - ih), Vec2(x - 1 + w, y + h - ih), color, thickness);
		DrawLine(Vec2(x + w - iw, y + 1 + h + 0.5f), Vec2(x + w - iw, y - 1 + h), color, thickness);
	}
}
void CRender::IRender::DrawBox(Vec2 a, Vec2 b, color_t color, float rounding)
{
	//Vec2 a(x, y), b(x + width, y + height);
	m_pDrawMngr->PathRect(a + Vec2(0.5f, 0.5f), b - Vec2(0.5f, 0.5f), rounding, -1);
	m_pDrawMngr->PathStroke(color.color, true, 1.f);
}

void CRender::IRender::DrawGradientBox(int16_t x, int16_t y, int16_t width, int16_t height, color_t color1, color_t color2, bool vertical)
{
	DrawGradientBox(x, y, width, height, color1, vertical ? color1 : color2, vertical ? color2 : color1, color2);
}

void CRender::IRender::DrawGradientBox(int16_t x, int16_t y, int16_t width, int16_t height, color_t color1, color_t color2, color_t color3, color_t color4)
{
	const Vec2 uv = COverManager::FontTexUvWhitePixel;

	Vec2 a(x, y), c(x + width, y + height);

	m_pDrawMngr->PrimReserve(6, 4);

	unsigned int *_VtxCurrentIdx = &m_pDrawMngr->_VtxCurrentIdx;

	m_pDrawMngr->PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx)); m_pDrawMngr->PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx + 1)); m_pDrawMngr->PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx + 2));
	m_pDrawMngr->PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx)); m_pDrawMngr->PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx + 2)); m_pDrawMngr->PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx + 3));

	m_pDrawMngr->PrimWriteVtx(a, uv, color1.color);
	m_pDrawMngr->PrimWriteVtx(Vec2(c.x, a.y), uv, color2.color);
	m_pDrawMngr->PrimWriteVtx(c, uv, color3.color);
	m_pDrawMngr->PrimWriteVtx(Vec2(a.x, c.y), uv, color4.color);
}

void CRender::IRender::DrawGradientBox(Vec2 x, Vec2 y, color_t color1, color_t color2, bool vertical)
{
	DrawGradientBox(x, y, color1, vertical ? color1 : color2, vertical ? color2 : color1, color2);
}
void CRender::IRender::DrawGradientBox(Vec2 x, Vec2 y, color_t color1, color_t color2, color_t color3, color_t color4)
{
	const Vec2 uv = COverManager::FontTexUvWhitePixel;

	Vec2 a(x), c(y);

	m_pDrawMngr->PrimReserve(6, 4);

	unsigned int *_VtxCurrentIdx = &m_pDrawMngr->_VtxCurrentIdx;

	m_pDrawMngr->PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx)); m_pDrawMngr->PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx + 1)); m_pDrawMngr->PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx + 2));
	m_pDrawMngr->PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx)); m_pDrawMngr->PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx + 2)); m_pDrawMngr->PrimWriteIdx((DrawIdx)(*_VtxCurrentIdx + 3));

	m_pDrawMngr->PrimWriteVtx(a, uv, color1.color);
	m_pDrawMngr->PrimWriteVtx(Vec2(c.x, a.y), uv, color2.color);
	m_pDrawMngr->PrimWriteVtx(c, uv, color4.color);
	m_pDrawMngr->PrimWriteVtx(Vec2(a.x, c.y), uv, color3.color);

}

void CRender::IRender::DrawCircle(int16_t x, int16_t y, int16_t radius, uint16_t points, color_t color)
{
	const float a_max = EX_PI * 2.0f * ((float)points - 1.0f) / (float)points;
	m_pDrawMngr->PathArcTo(Vec2(x, y), radius, 0.0f, a_max, points);
	m_pDrawMngr->PathFillConvex(color.color);
}

void CRender::IRender::DrawRing(int16_t x, int16_t y, int16_t radius, uint16_t points, color_t color, float thickness)
{
	const float a_max = EX_PI * 2.0f * ((float)points - 1.0f) / (float)points;
	m_pDrawMngr->PathArcTo(Vec2(x, y), radius - 0.5f, 0.0f, a_max, points);
	m_pDrawMngr->PathStroke(color.color, true, thickness);
}

void CRender::IRender::DrawRing3D(int16_t x, int16_t y, int16_t z, int16_t radius, uint16_t points, color_t color1, float thickness)
{
	Vector pos = Vector(x, y, z);

	float step = (float)M_PI * 2.0f / points;

	for (float a = 0; a < (M_PI * 2.0f); a += step)
	{
		Vector start(radius * cosf(a) + pos.x, radius * sinf(a) + pos.y, pos.z);
		Vector end(radius * cosf(a + step) + pos.x, radius * sinf(a + step) + pos.y, pos.z);

		Vector start2d;
		Vector end2d;

		Vec2 start22d(start2d.x, start2d.y);
		Vec2 end22d(end2d.x, end2d.y);
		if (CGlobal::WorldToScreen(start, start2d) &&
			CGlobal::WorldToScreen(end, end2d))
		{
			start22d.x = start2d.x;
			start22d.y = start2d.y;

			end22d.x = end2d.x;
			end22d.y = end2d.y;

			DrawLine(start22d, end22d, color1, thickness);
		}
	}
}

void CRender::IRender::DrawTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, color_t color, float thickness)
{
	m_pDrawMngr->PathLineTo(Vec2(x1, y1));
	m_pDrawMngr->PathLineTo(Vec2(x2, y2));
	m_pDrawMngr->PathLineTo(Vec2(x3, y3));
	m_pDrawMngr->PathStroke(color.color, true, thickness);
}

void CRender::IRender::DrawTriangleFilled(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, color_t color)
{
	m_pDrawMngr->PathLineTo(Vec2(x1, y1));
	m_pDrawMngr->PathLineTo(Vec2(x2, y2));
	m_pDrawMngr->PathLineTo(Vec2(x3, y3));
	m_pDrawMngr->PathFillConvex(color.color);
}

void CRender::IRender::DrawImage(LPDIRECT3DTEXTURE9 _texture, Vec2 _a, Vec2 _b)
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

	m_pDrawMngr->PushCmdBuffer();

	m_pDrawMngr->PrimReserve(6, 4);
	m_pDrawMngr->PrimQuadUV(a, b, c, d, uv_b, uv_c, uv_d, uv_a, 0xFFFFFFFF);
	m_pDrawMngr->SetTexture(_texture);

	m_pDrawMngr->PushCmdBuffer();
}
