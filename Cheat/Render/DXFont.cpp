#include "DXFont.h"
#include "DXRender.h"
  

#define STBRP_STATIC
#define STB_RECT_PACK_IMPLEMENTATION

#include "Fonts/stb_rect_pack.h"

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION

#define STBTT_malloc(x,u)  ((void)(u), malloc(x))
#define STBTT_free(x,u)    ((void)(u), free(x))
#define STBTT_assert(x)  

#include "Fonts/stb_truetype.h"

struct ImPlacementNewDummy {};
inline void* operator new(size_t, ImPlacementNewDummy, void* ptr) { return ptr; }
inline void operator delete(void*, ImPlacementNewDummy, void*) {}
#define PLACEMENT_NEW(_PTR)  new(ImPlacementNewDummy(), _PTR)

VectorEx<CFont*> Fonts;
VectorEx<CFont::FontConfig> ConfigDataVec;


void*              TexID = NULL;
unsigned char*     TexPixelsAlpha8 = NULL;
unsigned int*      TexPixelsRGBA32 = NULL;
int                TexWidth = 0;
int                TexHeight = 0;
int                TexDesiredWidth = 0;
Vec2               TexUvWhitePixel = Vec2();

LPDIRECT3DTEXTURE9  g_FontTexture = NULL;

static unsigned int stb_decompress_length(unsigned char *input)
{
	return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

static unsigned char *stb__barrier, *stb__barrier2, *stb__barrier3, *stb__barrier4;
static unsigned char *stb__dout;
static void stb__match(unsigned char *data, unsigned int length)
{
	// INVERSE of memmove... write each byte before copying the next...
	//IM_ASSERT(stb__dout + length <= stb__barrier);
	if (stb__dout + length > stb__barrier) { stb__dout += length; return; }
	if (data < stb__barrier4) { stb__dout = stb__barrier + 1; return; }
	while (length--) *stb__dout++ = *data++;
}

static void stb__lit(unsigned char *data, unsigned int length)
{
	//IM_ASSERT(stb__dout + length <= stb__barrier);
	if (stb__dout + length > stb__barrier) { stb__dout += length; return; }
	if (data < stb__barrier2) { stb__dout = stb__barrier + 1; return; }
	memcpy(stb__dout, data, length);
	stb__dout += length;
}

#define stb__in2(x)   ((i[x] << 8) + i[(x)+1])
#define stb__in3(x)   ((i[x] << 16) + stb__in2((x)+1))
#define stb__in4(x)   ((i[x] << 24) + stb__in3((x)+1))

static unsigned char *stb_decompress_token(unsigned char *i)
{
	if (*i >= 0x20) { // use fewer if's for cases that expand small
		if (*i >= 0x80)       stb__match(stb__dout - i[1] - 1, i[0] - 0x80 + 1), i += 2;
		else if (*i >= 0x40)  stb__match(stb__dout - (stb__in2(0) - 0x4000 + 1), i[2] + 1), i += 3;
		else /* *i >= 0x20 */ stb__lit(i + 1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
	}
	else { // more ifs for cases that expand large, since overhead is amortized
		if (*i >= 0x18)       stb__match(stb__dout - (stb__in3(0) - 0x180000 + 1), i[3] + 1), i += 4;
		else if (*i >= 0x10)  stb__match(stb__dout - (stb__in3(0) - 0x100000 + 1), stb__in2(3) + 1), i += 5;
		else if (*i >= 0x08)  stb__lit(i + 2, stb__in2(0) - 0x0800 + 1), i += 2 + (stb__in2(0) - 0x0800 + 1);
		else if (*i == 0x07)  stb__lit(i + 3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
		else if (*i == 0x06)  stb__match(stb__dout - (stb__in3(1) + 1), i[4] + 1), i += 5;
		else if (*i == 0x04)  stb__match(stb__dout - (stb__in3(1) + 1), stb__in2(4) + 1), i += 6;
	}
	return i;
}

static unsigned int stb_adler32(unsigned int adler32, unsigned char *buffer, unsigned int buflen)
{
	const unsigned long ADLER_MOD = 65521;
	unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
	unsigned long blocklen, i;

	blocklen = buflen % 5552;
	while (buflen) {
		for (i = 0; i + 7 < blocklen; i += 8) {
			s1 += buffer[0], s2 += s1;
			s1 += buffer[1], s2 += s1;
			s1 += buffer[2], s2 += s1;
			s1 += buffer[3], s2 += s1;
			s1 += buffer[4], s2 += s1;
			s1 += buffer[5], s2 += s1;
			s1 += buffer[6], s2 += s1;
			s1 += buffer[7], s2 += s1;

			buffer += 8;
		}

		for (; i < blocklen; ++i)
			s1 += *buffer++, s2 += s1;

		s1 %= ADLER_MOD, s2 %= ADLER_MOD;
		buflen -= blocklen;
		blocklen = 5552;
	}
	return (unsigned int)(s2 << 16) + (unsigned int)s1;
}

static unsigned int stb_decompress(unsigned char *output, unsigned char *i, unsigned int length)
{
	unsigned int olen;
	if (stb__in4(0) != 0x57bC0000) return 0;
	if (stb__in4(4) != 0)          return 0; // error! stream is > 4GB
	olen = stb_decompress_length(i);
	stb__barrier2 = i;
	stb__barrier3 = i + length;
	stb__barrier = output + olen;
	stb__barrier4 = output;
	i += 16;

	stb__dout = output;
	for (;;) {
		unsigned char *old_i = i;
		i = stb_decompress_token(i);
		if (i == old_i) {
			if (*i == 0x05 && i[1] == 0xfa) {
			//	IM_ASSERT(stb__dout == output + olen);
				if (stb__dout != output + olen) return 0;
				if (stb_adler32(1, output, olen) != (unsigned int)stb__in4(2))
					return 0;
				return olen;
			}
			else {
			//	IM_ASSERT(0); /* NOTREACHED */
				return 0;
			}
		}
		//IM_ASSERT(stb__dout <= output + olen);
		if (stb__dout > output + olen)
			return 0;
	}
}

CFont* CFont::CreateFontFromCompressMemory(const void* compressed_ttf_data, int compressed_ttf_size, float size_pixels, const FontConfig* font_cfg_template, const Wchar* glyph_ranges)
{
	const unsigned int buf_decompressed_size = stb_decompress_length((unsigned char*)compressed_ttf_data);
	unsigned char* buf_decompressed_data = (unsigned char *)malloc(buf_decompressed_size);
	stb_decompress(buf_decompressed_data, (unsigned char*)compressed_ttf_data, (unsigned int)compressed_ttf_size);

	FontConfig font_cfg = font_cfg_template ? *font_cfg_template : FontConfig();
	font_cfg.FontDataOwnedByAtlas = true;

	return CreateFontFromMemory(buf_decompressed_data, (int)buf_decompressed_size, size_pixels, &font_cfg, glyph_ranges);
}

CFont* CFont::CreateFontFromMemory(void* ttf_data, int ttf_size, float size_pixels, const FontConfig* font_cfg_template, const Wchar* glyph_ranges)
{
	FontConfig font_cfg = font_cfg_template ? *font_cfg_template : FontConfig();
	font_cfg.FontData = ttf_data;
	font_cfg.FontDataSize = ttf_size;
	font_cfg.SizePixels = size_pixels;
	if (glyph_ranges)
		font_cfg.GlyphRanges = glyph_ranges;
	return CreateFontFM(&font_cfg);
}

CFont* CFont::CreateFontFM(const FontConfig* font_cfg)
{
	/*if (!font_cfg->MergeMode)
	{*/
	CFont* font = (CFont*)malloc(sizeof(CFont));
	PLACEMENT_NEW(font) CFont();
	Fonts.push_back(font);
	//}

	ConfigDataVec.push_back(*font_cfg);
	FontConfig& new_font_cfg = ConfigDataVec.back();
	if (!new_font_cfg.DstFont)
		new_font_cfg.DstFont = Fonts.back();
	if (!new_font_cfg.FontDataOwnedByAtlas)
	{
		new_font_cfg.FontData = malloc(new_font_cfg.FontDataSize);
		new_font_cfg.FontDataOwnedByAtlas = true;
		memcpy(new_font_cfg.FontData, font_cfg->FontData, (size_t)new_font_cfg.FontDataSize);
	}

	// Invalidate texture
	ClearTexData();

	return new_font_cfg.DstFont;
}

void ClearTexData()
{
	if (TexPixelsAlpha8)
		free(TexPixelsAlpha8);
	if (TexPixelsRGBA32)
		free(TexPixelsRGBA32);
	TexPixelsAlpha8 = NULL;
	TexPixelsRGBA32 = NULL;
}

int ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end)
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

inline bool ImCharIsSpace(int c) { return c == ' ' || c == '\t' || c == 0x3000; }

const char* CFont::CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width)
{
	// Simple word-wrapping for English, not full-featured. Please submit failing cases!
	// FIXME: Much possible improvements (don't cut things like "word !", "word!!!" but cut within "word,,,,", more sensible support for punctuations, support for Unicode punctuations, etc.)

	// For references, possible wrap point marked with ^
	//  "aaa bbb, ccc,ddd. eee   fff. ggg!"
	//      ^    ^    ^   ^   ^__    ^    ^

	// List of hardcoded separators: .,;!?'"

	// Skip extra blanks after a line returns (that includes not counting them in width computation)
	// e.g. "Hello    world" --> "Hello" "World"

	// Cut words that cannot possibly fit within one line.
	// e.g.: "The tropical fish" with ~5 characters worth of width --> "The tr" "opical" "fish"

	float line_width = 0.0f;
	float word_width = 0.0f;
	float blank_width = 0.0f;

	const char* word_end = text;
	const char* prev_word_end = NULL;
	bool inside_word = true;

	const char* s = text;
	while (s < text_end)
	{
		unsigned int c = (unsigned int)*s;
		const char* next_s;
		if (c < 0x80)
			next_s = s + 1;
		else
			next_s = s + ImTextCharFromUtf8(&c, s, text_end);
		if (c == 0)
			break;

		if (c < 32)
		{
			if (c == '\n')
			{
				line_width = word_width = blank_width = 0.0f;
				inside_word = true;
				s = next_s;
				continue;
			}
			if (c == '\r')
			{
				s = next_s;
				continue;
			}
		}

		const float char_width = ((int)c < IndexXAdvance.size() ? IndexXAdvance[(int)c] : FallbackXAdvance) * scale;
		if (ImCharIsSpace(c))
		{
			if (inside_word)
			{
				line_width += blank_width;
				blank_width = 0.0f;
				word_end = s;
			}
			blank_width += char_width;
			inside_word = false;
		}
		else
		{
			word_width += char_width;
			if (inside_word)
			{
				word_end = next_s;
			}
			else
			{
				prev_word_end = word_end;
				line_width += word_width + blank_width;
				word_width = blank_width = 0.0f;
			}

			// Allow wrapping after punctuation.
			inside_word = !(c == '.' || c == ',' || c == ';' || c == '!' || c == '?' || c == '\"');
		}

		// We ignore blank width at the end of the line (they can be skipped)
		if (line_width + word_width >= wrap_width)
		{
			// Words that cannot possibly fit within an entire line will be cut anywhere.
			if (word_width < wrap_width)
				s = prev_word_end ? prev_word_end : word_end;
			break;
		}

		s = next_s;
	}

	return s;
}

void CFont::RenderText(COverManager *draw_list, float size, Vec2 pos, ColU32 col, const char* text_begin)
{
	const char* text_end = NULL;
	const Vec4& clip_rect = Vec4(0, 0, 1920, 1080);
	float wrap_width = 0.0f;
	bool cpu_fine_clip = false;

	if (!text_end)
		text_end = text_begin + strlen(text_begin); // ImGui functions generally already provides a valid text_end, so this is merely to handle direct calls.

	// Align to be pixel perfect
	pos.x = (float)(int)pos.x + DisplayOffset.x;
	pos.y = (float)(int)pos.y + DisplayOffset.y;
	float x = pos.x;
	float y = pos.y;
	if (y > clip_rect.w)
		return;

	const float scale = size / FontSize;
	const float line_height = FontSize * scale;
	const bool word_wrap_enabled = (wrap_width > 0.0f);
	const char* word_wrap_eol = NULL;

	// Skip non-visible lines
	const char* s = text_begin;
	if (!word_wrap_enabled && y + line_height < clip_rect.y)
		while (s < text_end && *s != '\n')  // Fast-forward to next line
			s++;

	// Reserve vertices for remaining worse case (over-reserving is useful and easily amortized)
	const int vtx_count_max = (int)(text_end - s) * 4;
	const int idx_count_max = (int)(text_end - s) * 6;
	const int idx_expected_size = draw_list->IdxBuffer.size() + idx_count_max;
	draw_list->PrimReserve(idx_count_max, vtx_count_max);

	COverManager::DrawVert* vtx_write = draw_list->_VtxWritePtr;
	DrawIdx* idx_write = draw_list->_IdxWritePtr;
	unsigned int vtx_current_idx = draw_list->_VtxCurrentIdx;

	while (s < text_end)
	{
		if (word_wrap_enabled)
		{
			// Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
			if (!word_wrap_eol)
			{
				word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - (x - pos.x));
				if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
					word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
			}

			if (s >= word_wrap_eol)
			{
				x = pos.x;
				y += line_height;
				word_wrap_eol = NULL;

				// Wrapping skips upcoming blanks
				while (s < text_end)
				{
					const char c = *s;
					if (ImCharIsSpace(c)) { s++; }
					else if (c == '\n') { s++; break; }
					else { break; }
				}
				continue;
			}
		}

		// Decode and advance source
		unsigned int c = (unsigned int)*s;
		if (c < 0x80)
		{
			s += 1;
		}
		else
		{
			s += ImTextCharFromUtf8(&c, s, text_end);
			if (c == 0) // Malformed UTF-8?
				break;
		}

		if (c < 32)
		{
			if (c == '\n')
			{
				x = pos.x;
				y += line_height;

				if (y > clip_rect.w)
					break;
				if (!word_wrap_enabled && y + line_height < clip_rect.y)
					while (s < text_end && *s != '\n')  // Fast-forward to next line
						s++;
				continue;
			}
			if (c == '\r')
				continue;
		}

		float char_width = 0.0f;
		if (const Glyph* glyph = FindGlyph((unsigned short)c))
		{
			char_width = glyph->XAdvance * scale;

			// Arbitrarily assume that both space and tabs are empty glyphs as an optimization
			if (c != ' ' && c != '\t')
			{
				// We don't do a second finer clipping test on the Y axis as we've already skipped anything before clip_rect.y and exit once we pass clip_rect.w
				float x1 = x + glyph->X0 * scale;
				float x2 = x + glyph->X1 * scale;
				float y1 = y + glyph->Y0 * scale;
				float y2 = y + glyph->Y1 * scale;
				if (x1 <= clip_rect.z && x2 >= clip_rect.x)
				{
					// Render a character
					float u1 = glyph->U0;
					float v1 = glyph->V0;
					float u2 = glyph->U1;
					float v2 = glyph->V1;

					// CPU side clipping used to fit text in their frame when the frame is too small. Only does clipping for axis aligned quads.
					if (cpu_fine_clip)
					{
						if (x1 < clip_rect.x)
						{
							u1 = u1 + (1.0f - (x2 - clip_rect.x) / (x2 - x1)) * (u2 - u1);
							x1 = clip_rect.x;
						}
						if (y1 < clip_rect.y)
						{
							v1 = v1 + (1.0f - (y2 - clip_rect.y) / (y2 - y1)) * (v2 - v1);
							y1 = clip_rect.y;
						}
						if (x2 > clip_rect.z)
						{
							u2 = u1 + ((clip_rect.z - x1) / (x2 - x1)) * (u2 - u1);
							x2 = clip_rect.z;
						}
						if (y2 > clip_rect.w)
						{
							v2 = v1 + ((clip_rect.w - y1) / (y2 - y1)) * (v2 - v1);
							y2 = clip_rect.w;
						}
						if (y1 >= y2)
						{
							x += char_width;
							continue;
						}
					}

					// We are NOT calling PrimRectUV() here because non-inlined causes too much overhead in a debug build.
					// Inlined here:
					{
						idx_write[0] = (DrawIdx)(vtx_current_idx); idx_write[1] = (DrawIdx)(vtx_current_idx + 1); idx_write[2] = (DrawIdx)(vtx_current_idx + 2);
						idx_write[3] = (DrawIdx)(vtx_current_idx); idx_write[4] = (DrawIdx)(vtx_current_idx + 2); idx_write[5] = (DrawIdx)(vtx_current_idx + 3);
						vtx_write[0].pos.x = x1; vtx_write[0].pos.y = y1; vtx_write[0].col = col; vtx_write[0].uv.x = u1; vtx_write[0].uv.y = v1;
						vtx_write[1].pos.x = x2; vtx_write[1].pos.y = y1; vtx_write[1].col = col; vtx_write[1].uv.x = u2; vtx_write[1].uv.y = v1;
						vtx_write[2].pos.x = x2; vtx_write[2].pos.y = y2; vtx_write[2].col = col; vtx_write[2].uv.x = u2; vtx_write[2].uv.y = v2;
						vtx_write[3].pos.x = x1; vtx_write[3].pos.y = y2; vtx_write[3].col = col; vtx_write[3].uv.x = u1; vtx_write[3].uv.y = v2;
						vtx_write += 4;
						vtx_current_idx += 4;
						idx_write += 6;

					}
				}
			}
		}

		x += char_width;
	}

	// Give back unused vertices
	draw_list->VtxBuffer.resize((int)(vtx_write - draw_list->VtxBuffer.data()));
	draw_list->IdxBuffer.resize((int)(idx_write - draw_list->IdxBuffer.data()));
	draw_list->CmdBuffer[draw_list->CmdBuffer.size() - 1].ElemCount -= (idx_expected_size - draw_list->IdxBuffer.size());
	draw_list->_VtxWritePtr = vtx_write;
	draw_list->_IdxWritePtr = idx_write;
	draw_list->_VtxCurrentIdx = (unsigned int)draw_list->VtxBuffer.size();
}

const Wchar* GetGlyphRangesDefault()
{
	static const Wchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0,
	};
	return &ranges[0];
}

void RenderCustomTexData(int pass, void* p_rects)
{

	const int TEX_DATA_W = 90;
	const int TEX_DATA_H = 27;

	VectorEx<stbrp_rect>& rects = *(VectorEx<stbrp_rect>*)p_rects;
	if (pass == 0)
	{
		// Request rectangles
		stbrp_rect r;
		memset(&r, 0, sizeof(r));
		r.w = (TEX_DATA_W * 2) + 1;
		r.h = TEX_DATA_H + 1;
		rects.push_back(r);
	}
	else if (pass == 1)
	{
		const stbrp_rect& r = rects[0];
		for (int y = 0, n = 0; y < 3; y++)
			for (int x = 0; x < 3; x++, n++)
			{
				const int offset0 = (int)(r.x + x) + (int)(r.y + y) * TexWidth;
				const int offset1 = offset0 + 1 + 3;
				TexPixelsAlpha8[offset0] = 0xFF;
				TexPixelsAlpha8[offset1] = 0xFF;
			}
		const Vec2 tex_uv_scale(1.0f / TexWidth, 1.0f / TexHeight);
		COverManager::FontTexUvWhitePixel = Vec2((r.x + 0.5f) * tex_uv_scale.x, (r.y + 0.5f) * tex_uv_scale.y);
	}
}

int MaxEx(int lhs, int rhs) { return lhs >= rhs ? lhs : rhs; }
float MaxEx(float lhs, float rhs) { return lhs >= rhs ? lhs : rhs; }
inline int UpperPowerOfTwo(int v) { v--; v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16; v++; return v; }

bool Build()
{
	//IM_ASSERT(ConfigData.Size > 0);

	TexID = NULL;
	TexWidth = TexHeight = 0;
	TexUvWhitePixel = Vec2(0, 0);
	ClearTexData();

	struct ImFontTempBuildData
	{
		stbtt_fontinfo      FontInfo;
		stbrp_rect*         Rects;
		stbtt_pack_range*   Ranges;
		int                 RangesCount;
	};
	ImFontTempBuildData* tmp_array = (ImFontTempBuildData*)malloc((size_t)ConfigDataVec.size() * sizeof(ImFontTempBuildData));

	// Initialize font information early (so we can error without any cleanup) + count glyphs
	int total_glyph_count = 0;
	int total_glyph_range_count = 0;
	for (int input_i = 0; input_i < ConfigDataVec.size(); input_i++)
	{
		CFont::FontConfig& cfg = ConfigDataVec[input_i];
		ImFontTempBuildData& tmp = tmp_array[input_i];

		//IM_ASSERT(cfg.DstFont && (!cfg.DstFont->IsLoaded() || cfg.DstFont->ContainerAtlas == this));
		const int font_offset = stbtt_GetFontOffsetForIndex((unsigned char*)cfg.FontData, cfg.FontNo);
		//IM_ASSERT(font_offset >= 0);
		if (!stbtt_InitFont(&tmp.FontInfo, (unsigned char*)cfg.FontData, font_offset))
			return false;

		// Count glyphs
		if (!cfg.GlyphRanges)
			cfg.GlyphRanges = GetGlyphRangesDefault();
		for (const Wchar* in_range = cfg.GlyphRanges; in_range[0] && in_range[1]; in_range += 2)
		{
			total_glyph_count += (in_range[1] - in_range[0]) + 1;
			total_glyph_range_count++;
		}
	}

	// Start packing. We need a known width for the skyline algorithm. Using a cheap heuristic here to decide of width. User can override TexDesiredWidth if they wish.
	// After packing is done, width shouldn't matter much, but some API/GPU have texture size limitations and increasing width can decrease height.
	TexWidth = (TexDesiredWidth > 0) ? TexDesiredWidth : (total_glyph_count > 4000) ? 4096 : (total_glyph_count > 2000) ? 2048 : (total_glyph_count > 1000) ? 1024 : 512;
	TexHeight = 0;
	const int max_tex_height = 1024 * 32;
	stbtt_pack_context spc;
	stbtt_PackBegin(&spc, NULL, TexWidth, max_tex_height, 0, 1, NULL);

	// Pack our extra data rectangles first, so it will be on the upper-left corner of our texture (UV will have small values).
	VectorEx<stbrp_rect> extra_rects;
	RenderCustomTexData(0, &extra_rects);
	stbtt_PackSetOversampling(&spc, 1, 1);
	stbrp_pack_rects((stbrp_context*)spc.pack_info, &extra_rects[0], extra_rects.size());
	for (int i = 0; i < extra_rects.size(); i++)
		if (extra_rects[i].was_packed)
			TexHeight = MaxEx(TexHeight, extra_rects[i].y + extra_rects[i].h);


	/*VectorEx<stbrp_rect>& rectseexx = *(VectorEx<stbrp_rect>*)(&extra_rects);
	const stbrp_rect& r = rectseexx[0];
	const Vec2 tex_uv_scale(1.0f / TexWidth, 1.0f / TexHeight);
	COverManager::FontTexUvWhitePixel = Vec2((r.x + 0.5f) * tex_uv_scale.x, (r.y + 0.5f) * tex_uv_scale.y);*/

	// Allocate packing character data and flag packed characters buffer as non-packed (x0=y0=x1=y1=0)
	int buf_packedchars_n = 0, buf_rects_n = 0, buf_ranges_n = 0;
	stbtt_packedchar* buf_packedchars = (stbtt_packedchar*)malloc(total_glyph_count * sizeof(stbtt_packedchar));
	stbrp_rect* buf_rects = (stbrp_rect*)malloc(total_glyph_count * sizeof(stbrp_rect));
	stbtt_pack_range* buf_ranges = (stbtt_pack_range*)malloc(total_glyph_range_count * sizeof(stbtt_pack_range));
	memset(buf_packedchars, 0, total_glyph_count * sizeof(stbtt_packedchar));
	memset(buf_rects, 0, total_glyph_count * sizeof(stbrp_rect));              // Unnecessary but let's clear this for the sake of sanity.
	memset(buf_ranges, 0, total_glyph_range_count * sizeof(stbtt_pack_range));

	// First font pass: pack all glyphs (no rendering at this point, we are working with rectangles in an infinitely tall texture at this point)
	for (int input_i = 0; input_i < ConfigDataVec.size(); input_i++)
	{
		CFont::FontConfig& cfg = ConfigDataVec[input_i];
		ImFontTempBuildData& tmp = tmp_array[input_i];

		// Setup ranges
		int glyph_count = 0;
		int glyph_ranges_count = 0;
		for (const Wchar* in_range = cfg.GlyphRanges; in_range[0] && in_range[1]; in_range += 2)
		{
			glyph_count += (in_range[1] - in_range[0]) + 1;
			glyph_ranges_count++;
		}
		tmp.Ranges = buf_ranges + buf_ranges_n;
		tmp.RangesCount = glyph_ranges_count;
		buf_ranges_n += glyph_ranges_count;
		for (int i = 0; i < glyph_ranges_count; i++)
		{
			const Wchar* in_range = &cfg.GlyphRanges[i * 2];
			stbtt_pack_range& range = tmp.Ranges[i];
			range.font_size = cfg.SizePixels;
			range.first_unicode_codepoint_in_range = in_range[0];
			range.num_chars = (in_range[1] - in_range[0]) + 1;
			range.chardata_for_range = buf_packedchars + buf_packedchars_n;
			buf_packedchars_n += range.num_chars;
		}

		// Pack
		tmp.Rects = buf_rects + buf_rects_n;
		buf_rects_n += glyph_count;
		stbtt_PackSetOversampling(&spc, cfg.OversampleH, cfg.OversampleV);
		int n = stbtt_PackFontRangesGatherRects(&spc, &tmp.FontInfo, tmp.Ranges, tmp.RangesCount, tmp.Rects);
		stbrp_pack_rects((stbrp_context*)spc.pack_info, tmp.Rects, n);

		// Extend texture height
		for (int i = 0; i < n; i++)
			if (tmp.Rects[i].was_packed)
				TexHeight = MaxEx(TexHeight, tmp.Rects[i].y + tmp.Rects[i].h);
	}
	//IM_ASSERT(buf_rects_n == total_glyph_count);
	//IM_ASSERT(buf_packedchars_n == total_glyph_count);
	//IM_ASSERT(buf_ranges_n == total_glyph_range_count);

	// Create texture
	TexHeight = UpperPowerOfTwo(TexHeight);
	TexPixelsAlpha8 = (unsigned char*)malloc(TexWidth * TexHeight);
	memset(TexPixelsAlpha8, 0, TexWidth * TexHeight);
	spc.pixels = TexPixelsAlpha8;
	spc.height = TexHeight;

	// Second pass: render characters
	for (int input_i = 0; input_i < ConfigDataVec.size(); input_i++)
	{
		CFont::FontConfig& cfg = ConfigDataVec[input_i];
		ImFontTempBuildData& tmp = tmp_array[input_i];
		stbtt_PackSetOversampling(&spc, cfg.OversampleH, cfg.OversampleV);
		stbtt_PackFontRangesRenderIntoRects(&spc, &tmp.FontInfo, tmp.Ranges, tmp.RangesCount, tmp.Rects);
		tmp.Rects = NULL;
	}

	// End packing
	stbtt_PackEnd(&spc);
	free(buf_rects);
	buf_rects = NULL;

	// Third pass: setup ImFont and glyphs for runtime
	for (int input_i = 0; input_i < ConfigDataVec.size(); input_i++)
	{
		CFont::FontConfig& cfg = ConfigDataVec[input_i];
		ImFontTempBuildData& tmp = tmp_array[input_i];
		CFont* dst_font = cfg.DstFont; // We can have multiple input fonts writing into a same destination font (when using MergeMode=true)

		float font_scale = stbtt_ScaleForPixelHeight(&tmp.FontInfo, cfg.SizePixels);
		int unscaled_ascent, unscaled_descent, unscaled_line_gap;
		stbtt_GetFontVMetrics(&tmp.FontInfo, &unscaled_ascent, &unscaled_descent, &unscaled_line_gap);

		float ascent = unscaled_ascent * font_scale;
		float descent = unscaled_descent * font_scale;
		if (!cfg.MergeMode)
		{
			//dst_font->ContainerAtlas = this;
			dst_font->ConfigData = &cfg;
			dst_font->ConfigDataCount = 0;
			dst_font->FontSize = cfg.SizePixels;
			dst_font->Ascent = ascent;
			dst_font->Descent = descent;
			dst_font->Glyphs.resize(0);
			dst_font->MetricsTotalSurface = 0;
		}
		dst_font->ConfigDataCount++;
		float off_y = (cfg.MergeMode && cfg.MergeGlyphCenterV) ? (ascent - dst_font->Ascent) * 0.5f : 0.0f;

		dst_font->FallbackGlyph = NULL; // Always clear fallback so FindGlyph can return NULL. It will be set again in BuildLookupTable()
		for (int i = 0; i < tmp.RangesCount; i++)
		{
			stbtt_pack_range& range = tmp.Ranges[i];
			for (int char_idx = 0; char_idx < range.num_chars; char_idx += 1)
			{
				const stbtt_packedchar& pc = range.chardata_for_range[char_idx];
				if (!pc.x0 && !pc.x1 && !pc.y0 && !pc.y1)
					continue;

				const int codepoint = range.first_unicode_codepoint_in_range + char_idx;
				if (cfg.MergeMode && dst_font->FindGlyph((unsigned short)codepoint))
					continue;

				stbtt_aligned_quad q;
				float dummy_x = 0.0f, dummy_y = 0.0f;
				stbtt_GetPackedQuad(range.chardata_for_range, TexWidth, TexHeight, char_idx, &dummy_x, &dummy_y, &q, 0);

				dst_font->Glyphs.resize(dst_font->Glyphs.size() + 1);
				CFont::Glyph& glyph = dst_font->Glyphs.back();
				glyph.Codepoint = (Wchar)codepoint;
				glyph.X0 = q.x0; glyph.Y0 = q.y0; glyph.X1 = q.x1; glyph.Y1 = q.y1;
				glyph.U0 = q.s0; glyph.V0 = q.t0; glyph.U1 = q.s1; glyph.V1 = q.t1;
				glyph.Y0 += (float)(int)(dst_font->Ascent + off_y + 0.5f);
				glyph.Y1 += (float)(int)(dst_font->Ascent + off_y + 0.5f);
				glyph.XAdvance = (pc.xadvance + cfg.GlyphExtraSpacing.x);  // Bake spacing into XAdvance
				if (cfg.PixelSnapH)
					glyph.XAdvance = (float)(int)(glyph.XAdvance + 0.5f);
				dst_font->MetricsTotalSurface += (int)((glyph.U1 - glyph.U0) * TexWidth + 1.99f) * (int)((glyph.V1 - glyph.V0) * TexHeight + 1.99f); // +1 to account for average padding, +0.99 to round
			}
		}
		cfg.DstFont->BuildLookupTable();
	}

	// Cleanup temporaries
	free(buf_packedchars);
	free(buf_ranges);
	free(tmp_array);

	// Render into our custom data block
	RenderCustomTexData(1, &extra_rects);

	return true;
}

void    GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel = NULL)
{
	// Build atlas on demand
	if (TexPixelsAlpha8 == NULL)
	{
		Build();
	}

	*out_pixels = TexPixelsAlpha8;
	if (out_width) *out_width = TexWidth;
	if (out_height) *out_height = TexHeight;
	if (out_bytes_per_pixel) *out_bytes_per_pixel = 1;
}

void GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel)
{
	// Convert to RGBA32 format on demand
	// Although it is likely to be the most commonly used format, our font rendering is 1 channel / 8 bpp
	if (!TexPixelsRGBA32)
	{
		unsigned char* pixels;
		GetTexDataAsAlpha8(&pixels, NULL, NULL);
		TexPixelsRGBA32 = (unsigned int*)malloc((size_t)(TexWidth * TexHeight * 4));
		const unsigned char* src = pixels;
		unsigned int* dst = TexPixelsRGBA32;
		for (int n = TexWidth * TexHeight; n > 0; n--)
			*dst++ = COL32(255, 255, 255, (unsigned int)(*src++));
	}

	*out_pixels = (unsigned char*)TexPixelsRGBA32;
	if (out_width) *out_width = TexWidth;
	if (out_height) *out_height = TexHeight;
	if (out_bytes_per_pixel) *out_bytes_per_pixel = 4;
}

bool CreateFontsTexture()
{
	// Build texture atlas
	unsigned char* pixels;
	int width, height, bytes_per_pixel;
	GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

	ADD_LOG("width %i\n", width);
	ADD_LOG("height %i\n", height);

	g_FontTexture = NULL;

	if (g_pDevice)
		if (g_pDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_FontTexture, NULL) < 0)
			return false;

	D3DLOCKED_RECT tex_locked_rect;

	if (!g_FontTexture)
		return false;

	if (g_FontTexture->LockRect(0, &tex_locked_rect, NULL, 0) != D3D_OK)
		return false;

	for (int y = 0; y < height; y++)
		memcpy((unsigned char *)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));

	if (!g_FontTexture)
		return false;

	g_FontTexture->UnlockRect(0);
}

Vec2 CFont::CalcTextSize(string label)
{
	float size = FontSize;
	float max_width = FLT_MAX;
	float wrap_width = -1.0f;

	const char* text_begin = label.c_str();
	const char* text_end = NULL;
	const char** remaining = NULL;

	if (!text_end)
		text_end = text_begin + strlen(text_begin); // FIXME-OPT: Need to avoid this.

	const float line_height = size;
	const float scale = 1.f;

	Vec2 text_size = Vec2(0, 0);
	float line_width = 0.0f;

	const bool word_wrap_enabled = (wrap_width > 0.0f);
	const char* word_wrap_eol = NULL;

	const char* s = text_begin;
	while (s < text_end)
	{
		if (word_wrap_enabled)
		{
			// Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
			if (!word_wrap_eol)
			{
				word_wrap_eol = CalcWordWrapPositionA(scale, s, text_end, wrap_width - line_width);
				if (word_wrap_eol == s) // Wrap_width is too small to fit anything. Force displaying 1 character to minimize the height discontinuity.
					word_wrap_eol++;    // +1 may not be a character start point in UTF-8 but it's ok because we use s >= word_wrap_eol below
			}

			if (s >= word_wrap_eol)
			{
				if (text_size.x < line_width)
					text_size.x = line_width;
				text_size.y += line_height;
				line_width = 0.0f;
				word_wrap_eol = NULL;

				// Wrapping skips upcoming blanks
				while (s < text_end)
				{
					const char c = *s;
					if (ImCharIsSpace(c)) { s++; }
					else if (c == '\n') { s++; break; }
					else { break; }
				}
				continue;
			}
		}

		// Decode and advance source
		const char* prev_s = s;
		unsigned int c = (unsigned int)*s;
		if (c < 0x80)
		{
			s += 1;
		}
		else
		{
			s += ImTextCharFromUtf8(&c, s, text_end);
			if (c == 0) // Malformed UTF-8?
				break;
		}

		if (c < 32)
		{
			if (c == '\n')
			{
				text_size.x = MaxEx(text_size.x, line_width);
				text_size.y += line_height;
				line_width = 0.0f;
				continue;
			}
			if (c == '\r')
				continue;
		}

		const float char_width = ((int)c < IndexXAdvance.size() ? IndexXAdvance[(int)c] : FallbackXAdvance) * scale;
		if (line_width + char_width >= max_width)
		{
			s = prev_s;
			break;
		}

		line_width += char_width;
	}

	if (text_size.x < line_width)
		text_size.x = line_width;

	if (line_width > 0 || text_size.y == 0.0f)
		text_size.y += line_height;

	if (remaining)
		*remaining = s;

	//	return text_size;

		//ImVec2 text_size = font->CalcTextSizeA(font_size, FLT_MAX, wrap_width, text, text_display_end, NULL);

		// Cancel out character spacing for the last character of a line (it is baked into glyph->XAdvance field)
	const float font_scale = 1.f;
	const float character_spacing_x = 1.0f * font_scale;
	if (text_size.x > 0.0f)
		text_size.x -= character_spacing_x;
	text_size.x = (float)(int)(text_size.x + 0.95f);
	//text_size.x *= 1.2f;

	return Vec2(text_size.x, text_size.y);
}