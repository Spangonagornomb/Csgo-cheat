#pragma once
#include "DXOverlayManager.h"

typedef unsigned short Wchar;

extern void*              TexID;
extern unsigned char*     TexPixelsAlpha8;
extern unsigned int*      TexPixelsRGBA32;
extern int                TexWidth;
extern int                TexHeight;
extern int                TexDesiredWidth;
extern Vec2               TexUvWhitePixel;

extern LPDIRECT3DTEXTURE9  g_FontTexture;

void ClearTexData();
bool CreateFontsTexture();
int MaxEx(int lhs, int rhs);

class CFont
{
public:

	struct FontConfig
	{
		void*           FontData = NULL;
		int             FontDataSize = 0;
		bool            FontDataOwnedByAtlas = true;
		int             FontNo = 0;
		float           SizePixels;
		int             OversampleH = 3, OversampleV = 1;
		bool            PixelSnapH = false;
		Vec2          GlyphExtraSpacing = Vec2(0, 0);
		const Wchar*  GlyphRanges = NULL;
		bool            MergeMode = false;
		bool            MergeGlyphCenterV = false;

		CFont*         DstFont = NULL;
	};

	struct Glyph
	{
		Wchar                   Codepoint;
		float                   XAdvance;
		float                   X0, Y0, X1, Y1;
		float                   U0, V0, U1, V1;     // Texture coordinates
	};

	float                       FontSize = 0;         
	float                       Scale = 1.f;        
	Vec2                        DisplayOffset = Vec2(0.f, 1.f);      
	VectorEx<Glyph>             Glyphs;            
	VectorEx<float>             IndexXAdvance;     
	VectorEx<unsigned short>    IndexLookup;     
	const Glyph*                FallbackGlyph = NULL;    
	float                       FallbackXAdvance; 
	Wchar                       FallbackChar;      

	short                       ConfigDataCount;    
	FontConfig*                 ConfigData = NULL;      
	float                       Ascent, Descent;  
	int                         MetricsTotalSurface;

	CFont()
	{

	};

	CFont* CreateFontFromCompressMemory(const void* compressed_ttf_data, int compressed_ttf_size, float size_pixels, const FontConfig* font_cfg_template, const Wchar* glyph_ranges);
	CFont* CreateFontFromMemory(void* ttf_data, int ttf_size, float size_pixels, const FontConfig* font_cfg_template, const Wchar* glyph_ranges);
	CFont* CreateFontFM(const FontConfig* font_cfg);
	void   RenderText(COverManager *draw_list, float size, Vec2 pos, ColU32 col, const char* text_begin);


	Vec2 CalcTextSize(string text);

	const char* CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width);

	const Glyph* FindGlyph(unsigned short c) const
	{
		if (c < IndexLookup.size())
		{
			const unsigned short i = IndexLookup[c];
			if (i != (unsigned short)-1)
				return &Glyphs.data()[i];
		}
		return FallbackGlyph;
	}

	void GrowIndex(int new_size)
	{
		//IM_ASSERT(IndexXAdvance.Size == IndexLookup.Size);
		int old_size = IndexLookup.size();
		if (new_size <= old_size)
			return;
		IndexXAdvance.resize(new_size);
		IndexLookup.resize(new_size);
		for (int i = old_size; i < new_size; i++)
		{
			IndexXAdvance[i] = -1.0f;
			IndexLookup[i] = (unsigned short)-1;
		}
	}

	void BuildLookupTable()
	{
		int max_codepoint = 0;
		for (int i = 0; i != Glyphs.size(); i++)
			max_codepoint = MaxEx(max_codepoint, (int)Glyphs[i].Codepoint);

		//IM_ASSERT(Glyphs.Size < 0xFFFF); // -1 is reserved
		IndexXAdvance.clear();
		IndexLookup.clear();
		GrowIndex(max_codepoint + 1);
		for (int i = 0; i < Glyphs.size(); i++)
		{
			int codepoint = (int)Glyphs[i].Codepoint;
			IndexXAdvance[codepoint] = Glyphs[i].XAdvance;
			IndexLookup[codepoint] = (unsigned short)i;
		}

		// Create a glyph to handle TAB
		// FIXME: Needs proper TAB handling but it needs to be contextualized (or we could arbitrary say that each string starts at "column 0" ?)
		if (FindGlyph((unsigned short)' '))
		{
			if (Glyphs.back().Codepoint != '\t')   // So we can call this function multiple times
				Glyphs.resize(Glyphs.size() + 1);
			Glyph& tab_glyph = Glyphs.back();
			tab_glyph = *FindGlyph((unsigned short)' ');
			tab_glyph.Codepoint = '\t';
			tab_glyph.XAdvance *= 4;
			IndexXAdvance[(int)tab_glyph.Codepoint] = (float)tab_glyph.XAdvance;
			IndexLookup[(int)tab_glyph.Codepoint] = (unsigned short)(Glyphs.size() - 1);
		}

		FallbackGlyph = NULL;
		FallbackGlyph = FindGlyph(FallbackChar);
		FallbackXAdvance = FallbackGlyph ? FallbackGlyph->XAdvance : 0.0f;
		for (int i = 0; i < max_codepoint + 1; i++)
			if (IndexXAdvance[i] < 0.0f)
				IndexXAdvance[i] = FallbackXAdvance;
	}
};

extern VectorEx<CFont*>			   Fonts;
extern VectorEx<CFont::FontConfig> ConfigDataVec;