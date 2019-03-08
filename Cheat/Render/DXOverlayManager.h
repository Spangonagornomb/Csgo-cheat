#pragma once
#include "../Main.h"

#define COL32_R_SHIFT    0
#define COL32_G_SHIFT    8
#define COL32_B_SHIFT    16
#define COL32_A_SHIFT    24
#define COL32_A_MASK     0xFF000000
#define COL32(R,G,B,A)    (((ColU32)(A)<<COL32_A_SHIFT) | ((ColU32)(B)<<COL32_B_SHIFT) | ((ColU32)(G)<<COL32_G_SHIFT) | ((ColU32)(R)<<COL32_R_SHIFT))

struct Vec2
{
	float x, y;
	Vec2() { x = y = 0; }
	Vec2(float _x, float _y) { x = _x; y = _y; }

};

static inline Vec2 operator*(const Vec2& lhs, const float rhs) { return Vec2(lhs.x*rhs, lhs.y*rhs); }
static inline Vec2 operator/(const Vec2& lhs, const float rhs) { return Vec2(lhs.x / rhs, lhs.y / rhs); }
static inline Vec2 operator+(const Vec2& lhs, const Vec2& rhs) { return Vec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline Vec2 operator-(const Vec2& lhs, const Vec2& rhs) { return Vec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline Vec2 operator*(const Vec2& lhs, const Vec2& rhs) { return Vec2(lhs.x*rhs.x, lhs.y*rhs.y); }
static inline Vec2 operator/(const Vec2& lhs, const Vec2& rhs) { return Vec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline Vec2& operator+=(Vec2& lhs, const Vec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline Vec2& operator-=(Vec2& lhs, const Vec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline Vec2& operator*=(Vec2& lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline Vec2& operator/=(Vec2& lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }

struct Rect
{
	Vec2 min;
	Vec2 max;
	Rect() { min = Vec2(); max = Vec2(); };
	Rect(Vec2 _min, Vec2 _max) { min = _min; max = _max; };
};

struct Vec4
{
	float x, y, z, w;
	Vec4() { x = y = z = w = 0; }
	Vec4(Vec2 min, Vec2 max) { x = min.x; y = min.y; z = max.x; w = max.y; }
	Vec4(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
};

typedef unsigned int ColU32;
typedef unsigned short DrawIdx;

class COverManager
{
private:
	bool AntiAliasedShapes = true;
public:

	static Vec2 FontTexUvWhitePixel;

	struct DrawVert
	{
		Vec2   pos = Vec2();
		Vec2   uv = Vec2();
		ColU32 col = 0;
	};

	struct DrawCmd
	{
		unsigned int		ElemCount = 0;
		LPDIRECT3DTEXTURE9	Texture = NULL;
		Vec4				ClipRect = Vec4(-8192.0f, -8192.0f, +8192.0f, +8192.0f);
	};

	VectorEx<DrawVert> VtxBuffer;
	VectorEx<DrawCmd> CmdBuffer;
	VectorEx<DrawIdx> IdxBuffer;

	unsigned int          _VtxCurrentIdx;
	DrawVert*             _VtxWritePtr;
	DrawIdx*              _IdxWritePtr;
	VectorEx<Vec2>        _Path;


	COverManager()
	{
		_VtxWritePtr = nullptr;
		_IdxWritePtr = nullptr;
		CmdBuffer.clear();
		CmdBuffer.push_back(DrawCmd());
	}

	float MinEX(float lhs, float rhs) { return lhs < rhs ? lhs : rhs; }
	Vec2 MinEXV(const Vec2& lhs, const Vec2& rhs) { return Vec2(MinEX(lhs.x, rhs.x), MinEX(lhs.y, rhs.y)); }

	void PushCmdBuffer();
	void PushCmdBufferEx();

	void SetTexture(LPDIRECT3DTEXTURE9 _Texture);
	void SetClipRect(Vec4 rect);
	void SetNewClipRect(Vec4 rect);

	void PrimReserve(int idx_count, int vtx_count);
	void PrimRect(const Vec2& a, const Vec2& c, ColU32 col);
	void PrimQuadUV(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d, const Vec2& uv_a, const Vec2& uv_b, const Vec2& uv_c, const Vec2& uv_d, ColU32 col);

	inline void PrimWriteVtx(const Vec2& pos, const Vec2& uv, ColU32 col) { _VtxWritePtr->pos = pos; _VtxWritePtr->uv = uv; _VtxWritePtr->col = col; _VtxWritePtr++; _VtxCurrentIdx++; }
	inline void PrimWriteIdx(DrawIdx idx) { *_IdxWritePtr = idx; _IdxWritePtr++; }

	void AddConvexPolyFilled(const Vec2* points, const int points_count, ColU32 col, bool anti_aliased);
	void AddPolyline(const Vec2* points, const int points_count, ColU32 col, bool closed, float thickness, bool anti_aliased);

	void PathClear(){ _Path.resize(0); }
	void PathLineTo(const Vec2& pos) { _Path.push_back(pos);}
	void PathFillConvex(ColU32 col) { AddConvexPolyFilled(_Path.data(), _Path.size(), col, true); PathClear();}
	void PathArcTo(const Vec2& centre, float radius, float amin, float amax, int num_segments);

	float ImInvLength(const Vec2& lhs, float fail_value) { float d = lhs.x*lhs.x + lhs.y*lhs.y; if (d > 0.0f) return 1.0f / sqrtf(d); return fail_value; }

	void PathArcToFast(const Vec2& centre, float radius, int amin, int amax);
	void PathRect(const Vec2& a, const Vec2& b, float rounding, int rounding_corners);
	void PathStroke(ColU32 col, bool closed, float thickness = 1.0f) { AddPolyline(_Path.data(), _Path.size(), col, closed, thickness, true); PathClear(); }

	void ResetCount();
};