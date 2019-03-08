#include "DXRender.h"
#include "../GUI/Gui.h"
#include "Fonts/Arial.h"
#include "Fonts/WeaponIcons.h"


IDirect3DDevice9* g_pDevice = NULL;

void CRender::IRender::BeginDraw()
{
	TotalIdxCount = 0;
	TotalVtxCount = 0;
	DrawLists.clear();

	m_pDevice->CreateStateBlock(D3DSBT_ALL, &m_pStateBlockDraw);
	if (m_pStateBlockDraw)
		m_pStateBlockDraw->Capture();
	m_pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
	m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
}

static LPDIRECT3DVERTEXBUFFER9  g_pVB = NULL;
static LPDIRECT3DINDEXBUFFER9   g_pIB = NULL;
static int                      g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

struct CUSTOMVERTEX
{
	float    pos[3];
	D3DCOLOR col;
	float    uv[2];
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

void CRender::IRender::EndDraw()
{
	if (!g_pVB || g_VertexBufferSize < TotalVtxCount)
	{
		if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
		g_VertexBufferSize = TotalVtxCount + 5000;
		if (m_pDevice->CreateVertexBuffer(g_VertexBufferSize * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL) < 0)
			return;
	}
	if (!g_pIB || g_IndexBufferSize < TotalIdxCount)
	{
		if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
		g_IndexBufferSize = TotalIdxCount + 10000;
		if (m_pDevice->CreateIndexBuffer(g_IndexBufferSize * sizeof(DrawIdx), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, ((sizeof(DrawIdx) == 2) ? D3DFMT_INDEX16 : D3DFMT_INDEX32), D3DPOOL_DEFAULT, &g_pIB, NULL) < 0)
			return;
	}

	IDirect3DStateBlock9* d3d9_state_block = NULL;
	if (m_pDevice->CreateStateBlock(D3DSBT_PIXELSTATE, &d3d9_state_block) < 0)
		return;

	CUSTOMVERTEX* vtx_dst;
	DrawIdx* idx_dst;

	if (g_pVB->Lock(0, (UINT)(TotalVtxCount * sizeof(CUSTOMVERTEX)), (void**)&vtx_dst, D3DLOCK_DISCARD) < 0)
		return;
	if (g_pIB->Lock(0, (UINT)(TotalIdxCount * sizeof(DrawIdx)), (void**)&idx_dst, D3DLOCK_DISCARD) < 0)
		return;

	for (int n = 0; n < DrawLists.size(); n++)
	{
		const COverManager::DrawVert* vtx_src = DrawLists[n]->VtxBuffer.data();
		for (int i = 0; i < DrawLists[n]->VtxBuffer.size(); i++)
		{
			vtx_dst->pos[0] = vtx_src->pos.x;
			vtx_dst->pos[1] = vtx_src->pos.y;
			vtx_dst->pos[2] = 0.0f;
			//	vtx_dst->col = (vtx_src->col & 0xFF00FF00) | ((vtx_src->col & 0xFF0000) >> 16) | ((vtx_src->col & 0xFF) << 16);     // RGBA --> ARGB for DirectX9
			vtx_dst->col = vtx_src->col;
			vtx_dst->uv[0] = vtx_src->uv.x;
			vtx_dst->uv[1] = vtx_src->uv.y;
			vtx_dst++;
			vtx_src++;
		}
		memcpy(idx_dst, DrawLists[n]->IdxBuffer.data(), DrawLists[n]->IdxBuffer.size() * sizeof(DrawIdx));
		idx_dst += DrawLists[n]->IdxBuffer.size();
	}

	g_pVB->Unlock();
	g_pIB->Unlock();
	
	m_pDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
	m_pDevice->SetIndices(g_pIB);
	m_pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

	D3DVIEWPORT9 vp;
	vp.X = vp.Y = 0;
	vp.Width = (DWORD)ScreenWidth;
	vp.Height = (DWORD)ScreenHeight;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	m_pDevice->SetViewport(&vp);

	m_pDevice->SetPixelShader(NULL);
	m_pDevice->SetVertexShader(NULL);
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, false);
	m_pDevice->SetRenderState(D3DRS_ZENABLE, false);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	m_pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);

	m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
	m_pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
	
	m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	//const float L = 0.375f, R = ScreenWidth + 0.375f, T = 0.375f, B = ScreenHeight + 0.375f;
	const float L = 0.5f, R = ScreenWidth + 0.5f, T = 0.5f, B = ScreenHeight + 0.5f;
	D3DMATRIX mat_identity = { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } };
	D3DMATRIX mat_projection =
	{
		2.0f / (R - L),   0.0f,         0.0f,  0.0f,
		0.0f,         2.0f / (T - B),   0.0f,  0.0f,
		0.0f,         0.0f,         0.5f,  0.0f,
		(L + R) / (L - R),  (T + B) / (B - T),  0.5f,  1.0f,
	};
	m_pDevice->SetTransform(D3DTS_WORLD, &mat_identity);
	m_pDevice->SetTransform(D3DTS_VIEW, &mat_identity);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &mat_projection);
	
	int vtx_offset = 0;
	int idx_offset = 0;

	for (int n = 0; n < DrawLists.size(); n++)
	{
		COverManager* Over = DrawLists[n];
		for (int i = 0; i < Over->CmdBuffer.size(); i++)
		{
			const COverManager::DrawCmd *cmd = &Over->CmdBuffer[i];

			const RECT r = { (LONG)cmd->ClipRect.x, (LONG)cmd->ClipRect.y, (LONG)cmd->ClipRect.z, (LONG)cmd->ClipRect.w };

			m_pDevice->SetTexture(0, cmd->Texture);
			m_pDevice->SetScissorRect(&r);
			m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vtx_offset, 0, (UINT)Over->VtxBuffer.size(), idx_offset, cmd->ElemCount / 3);

			idx_offset += cmd->ElemCount;
		}
		vtx_offset += Over->VtxBuffer.size();
	}


	d3d9_state_block->Apply();
	d3d9_state_block->Release();

	m_pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
	if (m_pStateBlockDraw)
	{
		m_pStateBlockDraw->Apply();
		m_pStateBlockDraw->Release();
	}
}

void CRender::IRender::InvalidateDeviceObjects()
{
	if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
	if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
	if (g_FontTexture) { g_FontTexture->Release(); g_FontTexture = NULL; }

	for (auto img : ImageList)
	{
		img.pImage = nullptr;
	}

	if (m_pStateBlockDraw)
		m_pStateBlockDraw = nullptr;
}

void CRender::IRender::CreateDeviceObjects()
{
	CreateFontsTexture();
	for (auto img : ImageList)
	{
		img.Update(m_pDevice);
	}
}

void CRender::IRender::SetOverlay(COverManager *mngr)
{
	m_pDrawMngr = mngr;
}
void CRender::IRender::SetFont(CFont *font)
{
	m_pFont = font;
}


void CRender::IRender::ReleaseOverlay()
{
	if (m_pDrawMngr)
	{
		DrawLists.push_back(m_pDrawMngr);
		TotalIdxCount += m_pDrawMngr->IdxBuffer.size();
		TotalVtxCount += m_pDrawMngr->VtxBuffer.size();
	}
}

void CRender::IRender::UpdateSzFonts()
{

	CFont::FontConfig font_config;
	font_config.OversampleH = 3;
	font_config.OversampleV = 3;
	font_config.PixelSnapH = 1;

	static const Wchar ranges[] =
	{
		0x0020, 0x00FF,
		0x0400, 0x044F,
		0,
	};
	//SzFonts[17] = SzFonts[17]->CreateFontFromCompressMemory(Arial_compressed_data, Arial_compressed_size, (float)17, &font_config, ranges);
	for (int i(9); i < MAX_FONT_SIZE+1; i++)
	{
		SzFonts[i] = SzFonts[i]->CreateFontFromCompressMemory(Arial_compressed_data, Arial_compressed_size, (float)i, &font_config, ranges);
	}

	for (int i(9); i < MAX_FONT_SIZE+1; i++)
	{
		SzFontsIcon[i] = SzFontsIcon[i]->CreateFontFromCompressMemory(Icons_compressed_data, Icons_compressed_size, (float)i, &font_config, ranges);
	}
}

WNDPROC WndProc_o = nullptr;
static bool is_down = false;
static bool is_clicked = false;
bool check_closed = false;

LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	if (FastCall::G().t_GetAsyncKeyState(MainSettings().MenuButton))
	{
		is_clicked = false;
		is_down = true;
	}
	else if (!FastCall::G().t_GetAsyncKeyState(MainSettings().MenuButton) && is_down)
	{
		is_clicked = true;
		is_down = false;
	}
	else
	{
		is_clicked = false;
		is_down = false;
	}

	if (!CGlobal::IsGuiVisble && !is_clicked && check_closed)
	{
		check_closed = false;
	}

	if (is_clicked)
	{
		CGlobal::IsGuiVisble = !CGlobal::IsGuiVisble;
	
		ADD_LOG("Menu: Open: %i\n", CGlobal::IsGuiVisble);

		if (!check_closed)
			check_closed = true;
	}

	bool Is3DEnabled = GP_Enable3DMenu && GP_Enable3DMenu->Enable && GP_Enable3DMenu->Check();

	if ((CGlobal::IsGuiVisble || Is3DEnabled) && X1Gui().WndProcHandler(hWnd, msg, wParam, lParam, Is3DEnabled))
		return (CGlobal::IsGuiVisble && !Is3DEnabled) ? true : FastCall::G().t_CallWindowProcA(WndProc_o, hWnd, msg, wParam, lParam);

	return FastCall::G().t_CallWindowProcA(WndProc_o, hWnd, msg, wParam, lParam);
}