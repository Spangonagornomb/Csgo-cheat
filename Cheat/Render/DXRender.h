#pragma once

#include  "../Main.h"
#include  "DXOverlayManager.h"
#include  "DXFont.h"
#include  "color.h"
#include "../X1API/MinHook/hook.h"

using oEndScene = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9* pDevice);
extern oEndScene pEndScene;

using oPresent = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9* pDevice, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
//extern oPresent pPresent;
extern cDetour<oPresent>* pPresent;

using oReset = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
//extern oReset pReset;
extern cDetour<oReset>* pReset;

extern IDirect3DDevice9* g_pDevice;

class IIRender
{
public:

	enum RenderDrawType : uint32_t
	{
		RenderDrawType_None = 0,
		RenderDrawType_Outlined = 1 << 0,
		RenderDrawType_Filled = 1 << 1,
		RenderDrawType_Gradient = 1 << 2,
		RenderDrawType_OutlinedGradient = RenderDrawType_Outlined | RenderDrawType_Gradient,
		RenderDrawType_FilledGradient = RenderDrawType_Filled | RenderDrawType_Gradient
	};

	struct Vertex_t
	{
		Vertex_t() { }

		Vertex_t(int _x, int _y, color_t _color)
		{
			x = static_cast<float>(_x);
			y = static_cast<float>(_y);
			z = 0;
			rhw = 1;
			color = _color.color;
		}

		Vertex_t(float _x, float _y, color_t _color)
		{
			x = _x;
			y = _y;
			z = 0;
			rhw = 1;
			color = _color.color;
		}

		float x, y, z, rhw;
		color_t color = 0;
	};

	virtual class IRender
	{
	private:
		virtual void DX_Init(DWORD* table) = 0;
	public:

		virtual void Initialize() = 0;
		virtual void BeginDraw() = 0;
		virtual void EndDraw() = 0;
	
		virtual void DrawStringNew(int16_t x, int16_t y, color_t color, bool outlined, bool centered, const char* text, ...) = 0;
		virtual void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, color_t color, float thickness) = 0;
		virtual void DrawFilledBox(int16_t x, int16_t y, int16_t width, int16_t height, color_t color, float rounding) = 0;
		virtual void DrawBox(int16_t x, int16_t y, int16_t width, int16_t height, int16_t thickness, color_t color, float rounding) = 0;
		virtual void DrawGradientBox(int16_t x, int16_t y, int16_t width, int16_t height, color_t color1, color_t color2, bool vertical) = 0;
		virtual void DrawGradientBox(int16_t x, int16_t y, int16_t width, int16_t height, color_t color1, color_t color2, color_t color3, color_t color4) = 0;
		virtual void DrawCircle(int16_t x, int16_t y, int16_t radius, uint16_t points, color_t color1) = 0;
		virtual void DrawRing(int16_t x, int16_t y, int16_t radius, uint16_t points, color_t color1, float thickness) = 0;
		virtual void DrawTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, color_t color1, float thickness) = 0;
		virtual void DrawTriangleFilled(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, color_t color1) = 0;
		virtual void DrawImage(LPDIRECT3DTEXTURE9 _texture, Vec2 _a, Vec2 _b) = 0;
	};
};

extern LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern WNDPROC WndProc_o;

class CRender : public IIRender
{
public:
	virtual class IRender
	{
	public:
		IDirect3DDevice9* m_pDevice; 
	private:

		IDirect3DStateBlock9*	m_pStateBlockDraw;
		DWORD					dwOld_D3DRS_COLORWRITEENABLE = 0;

		virtual void DX_Init(DWORD* table);

		struct Image_t
		{
			float w, h;
			void *pArr;
			UINT szArr;
			LPDIRECT3DTEXTURE9 pImage;
			LPDIRECT3DTEXTURE9 Update(IDirect3DDevice9* pDevice)
			{
				pImage = nullptr;
				D3DXCreateTextureFromFileInMemoryEx(pDevice, pArr, szArr, w, h,
					D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pImage);
				return pImage;
			}
		};

		VectorEx<Image_t> ImageList;

		int ScreenWidth = 0;
		int ScreenHeight = 0;

		COverManager* m_pDrawMngr;
	
	public:

		CFont* m_pFont;

		VectorEx<COverManager*> DrawLists;////////

		VectorEx<CFont*> SzFonts = VectorEx<CFont*>(MAX_FONT_SIZE+1);
		VectorEx<CFont*> SzFontsIcon = VectorEx<CFont*>(MAX_FONT_SIZE+1);

		void UpdateSzFonts();

		LPDIRECT3DTEXTURE9 AddImage(void *pArr, UINT szArr, float w, float h)
		{
			Image_t img;
			img.w = w;
			img.h = h;
			img.pArr = pArr;
			img.szArr = szArr;
			ImageList.push_back(img);
			return img.Update(m_pDevice);
		}

		int TotalIdxCount = 0;
		int TotalVtxCount = 0;

		virtual void Initialize();

		virtual void BeginDraw();
		virtual void EndDraw();

		void InvalidateDeviceObjects();
		void CreateDeviceObjects();

		void SetOverlay(COverManager *mngr);
		void ReleaseOverlay();

	
		void SetFont(CFont *font);

		Vec2 CalcTextSize(string text)
		{
			return m_pFont->CalcTextSize(text);
		}

		Vec2 CalcTextSize(string text, CFont *fnt)
		{
			return fnt->CalcTextSize(text);
		}

		COverManager* GetOverlay()
		{
			return m_pDrawMngr;
		}

		virtual void DrawStringNew(int16_t x, int16_t y, color_t color, bool outlined, bool centered, const char* text, ...);
		void DrawString(int16_t x, int16_t y, color_t color, bool outlined, bool centered, const char* text);
		void DrawString(Vec2 Pos, color_t color, const char* text);
		void DrawString(CFont* font, Vec2 Pos, color_t color, const char* text);
		void DrawString(int size, Vec2 Pos, color_t color, const char* text);
		//void DrawString(int size, Vec2 Pos, color_t color, bool outlined, bool centered, const char* text);
		void DrawString(int size, Vec2 Pos, color_t color, bool outlined, bool centered, const char* text, ...);
		void DrawStringIcon(int size, Vec2 Pos, color_t color, const char* text);
		void DrawStringIcon(int size, Vec2 Pos, color_t color, bool outlined, bool centered, const char* text);
		void DrawStringIcon(Vec2 Pos, color_t color, const char* text);
		void DrawString(int16_t x, int16_t y, int size, color_t color, bool outlined, bool centered, const char* text);
		void DrawIcon(int16_t x, int16_t y, int size, color_t color, bool outlined, bool centered, const char* text);
		virtual void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, color_t color = 0, float thickness = 1);
		void DrawLine(Vec2 a, Vec2 b, color_t color = 0, float thickness = 1);
		virtual void DrawFilledBox(int16_t x, int16_t y, int16_t width, int16_t height, color_t color = 0, float rounding = 0);
		void DrawFilledBox(Vec2 pos, Vec2 size, color_t color = 0, float rounding = 0);
		void DrawFilledBox(Rect rect, color_t color = 0, float rounding = 0);

		virtual void DrawBox(int16_t x, int16_t y, int16_t width, int16_t height, int16_t thickness = 1, color_t color = 0, float rounding = 0);
		void DrawCoalBox(int16_t x, int16_t y, int16_t width, int16_t height, int16_t thickness = 1, color_t color = 0, bool outline = false, float del = 4);
		void DrawBox(Vec2 a, Vec2 b, color_t color = 0, float rounding = 0);
		virtual void DrawGradientBox(int16_t x, int16_t y, int16_t width, int16_t height, color_t color1 = 0, color_t color2 = 0, bool vertical = false);
		virtual void DrawGradientBox(int16_t x, int16_t y, int16_t width, int16_t height, color_t color1 = 0, color_t color2 = 0, color_t color3 = 0, color_t color4 = 0);

		void DrawGradientBox(Vec2 x, Vec2 y, color_t color1 = 0, color_t color2 = 0, bool vertical = false);
		void DrawGradientBox(Vec2 x, Vec2 y, color_t color1 = 0, color_t color2 = 0, color_t color3 = 0, color_t color4 = 0);

		virtual void DrawCircle(int16_t x, int16_t y, int16_t radius, uint16_t points = 64, color_t color1 = 0xFF);
		virtual void DrawRing(int16_t x, int16_t y, int16_t radius, uint16_t points = 64, color_t color1 = 0xFF, float thickness = 1.0f);
		void DrawRing3D(int16_t x, int16_t y, int16_t z, int16_t radius, uint16_t points = 64, color_t color1 = 0xFF, float thickness = 1.0f);
		virtual void DrawTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, color_t color1 = 0xFF, float thickness = 1.0f);
		virtual void DrawTriangleFilled(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, color_t color1 = 0xFF);
		virtual void DrawImage(LPDIRECT3DTEXTURE9 _texture, Vec2 _a, Vec2 _b);
	
	};
};