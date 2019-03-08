#include "DXOverlayManager.h"
#include "DXFont.h"

Vec2 COverManager::FontTexUvWhitePixel = Vec2(0,0);

void COverManager::PrimReserve(int idx_count, int vtx_count)
{
	DrawCmd& draw_cmd = CmdBuffer.back();
	draw_cmd.ElemCount += idx_count;

	int cur_vtx_size = VtxBuffer.size();
	VtxBuffer.resize(cur_vtx_size + vtx_count);
	_VtxWritePtr = VtxBuffer.data() + cur_vtx_size;

	int cur_idx_size = IdxBuffer.size();
	IdxBuffer.resize(cur_idx_size + idx_count);
	_IdxWritePtr = IdxBuffer.data() + cur_idx_size;
}
void COverManager::PushCmdBufferEx()
{
	CmdBuffer.push_back(DrawCmd());
}

void COverManager::PushCmdBuffer()
{
	Vec4 oldRect = CmdBuffer.back().ClipRect;
	PushCmdBufferEx();
	CmdBuffer.back().ClipRect = oldRect;
}

void COverManager::SetTexture(LPDIRECT3DTEXTURE9 _Texture)
{
	CmdBuffer.back().Texture = _Texture;
}
void COverManager::SetClipRect(Vec4 rect)
{
	CmdBuffer.back().ClipRect = rect;
}
void COverManager::SetNewClipRect(Vec4 rect)
{
	LPDIRECT3DTEXTURE9 oldTexture = CmdBuffer.back().Texture;
	PushCmdBufferEx();
	CmdBuffer.back().ClipRect = rect;
	CmdBuffer.back().Texture = oldTexture;
}

void COverManager::PrimRect(const Vec2& a, const Vec2& c, ColU32 col)
{
	Vec2 b(c.x, a.y), d(a.x, c.y), uv(COverManager::FontTexUvWhitePixel);
	DrawIdx idx = (DrawIdx)_VtxCurrentIdx;
	_IdxWritePtr[0] = idx; _IdxWritePtr[1] = (DrawIdx)(idx + 1); _IdxWritePtr[2] = (DrawIdx)(idx + 2);
	_IdxWritePtr[3] = idx; _IdxWritePtr[4] = (DrawIdx)(idx + 2); _IdxWritePtr[5] = (DrawIdx)(idx + 3);
	_VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
	_VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
	_VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
	_VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
	_VtxWritePtr += 4;
	_VtxCurrentIdx += 4;
	_IdxWritePtr += 6;
}

void COverManager::PrimQuadUV(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d, const Vec2& uv_a, const Vec2& uv_b, const Vec2& uv_c, const Vec2& uv_d, ColU32 col)
{
	DrawIdx idx = (DrawIdx)_VtxCurrentIdx;
	_IdxWritePtr[0] = idx; _IdxWritePtr[1] = (DrawIdx)(idx + 1); _IdxWritePtr[2] = (DrawIdx)(idx + 2);
	_IdxWritePtr[3] = idx; _IdxWritePtr[4] = (DrawIdx)(idx + 2); _IdxWritePtr[5] = (DrawIdx)(idx + 3);
	_VtxWritePtr[0].pos = a; _VtxWritePtr[0].uv = uv_a; _VtxWritePtr[0].col = col;
	_VtxWritePtr[1].pos = b; _VtxWritePtr[1].uv = uv_b; _VtxWritePtr[1].col = col;
	_VtxWritePtr[2].pos = c; _VtxWritePtr[2].uv = uv_c; _VtxWritePtr[2].col = col;
	_VtxWritePtr[3].pos = d; _VtxWritePtr[3].uv = uv_d; _VtxWritePtr[3].col = col;
	_VtxWritePtr += 4;
	_VtxCurrentIdx += 4;
	_IdxWritePtr += 6;
}

void COverManager::ResetCount()
{
	CmdBuffer.clear();
	VtxBuffer.clear();
	IdxBuffer.clear();
	CmdBuffer.push_back(DrawCmd());
	_VtxWritePtr = NULL;
	_IdxWritePtr = NULL;
	_VtxCurrentIdx = 0;
}

void COverManager::PathArcToFast(const Vec2& centre, float radius, int amin, int amax)
{
	static Vec2 circle_vtx[12];
	static bool circle_vtx_builds = false;
	const int circle_vtx_count = EX_ARRAYSIZE(circle_vtx);
	if (!circle_vtx_builds)
	{
		for (int i = 0; i < circle_vtx_count; i++)
		{
			const float a = ((float)i / (float)circle_vtx_count) * 2 * EX_PI;
			circle_vtx[i].x = cosf(a);
			circle_vtx[i].y = sinf(a);
		}
		circle_vtx_builds = true;
	}

	if (amin > amax) return;
	if (radius == 0.0f)
	{
		_Path.push_back(centre);
	}
	else
	{
		_Path.reserve(_Path.size() + (amax - amin + 1));
		for (int a = amin; a <= amax; a++)
		{
			const Vec2& c = circle_vtx[a % circle_vtx_count];
			_Path.push_back(Vec2(centre.x + c.x * radius, centre.y + c.y * radius));
		}
	}
}

void COverManager::PathArcTo(const Vec2& centre, float radius, float amin, float amax, int num_segments)
{
	if (radius == 0.0f)
		_Path.push_back(centre);
	_Path.reserve(_Path.size() + (num_segments + 1));
	for (int i = 0; i <= num_segments; i++)
	{
		const float a = amin + ((float)i / (float)num_segments) * (amax - amin);
		_Path.push_back(Vec2(centre.x + cosf(a) * radius, centre.y + sinf(a) * radius));
	}
}

void COverManager::PathRect(const Vec2& a, const Vec2& b, float rounding, int rounding_corners)
{
	float r = rounding;
	r = MinEX(r, fabsf(b.x - a.x) * (((rounding_corners&(1 | 2)) == (1 | 2)) || ((rounding_corners&(4 | 8)) == (4 | 8)) ? 0.5f : 1.0f) - 1.0f);
	r = MinEX(r, fabsf(b.y - a.y) * (((rounding_corners&(1 | 8)) == (1 | 8)) || ((rounding_corners&(2 | 4)) == (2 | 4)) ? 0.5f : 1.0f) - 1.0f);

	if (r <= 0.0f || rounding_corners == 0)
	{
		PathLineTo(a);
		PathLineTo(Vec2(b.x, a.y));
		PathLineTo(b);
		PathLineTo(Vec2(a.x, b.y));
	}
	else
	{
		const float r0 = (rounding_corners & 1) ? r : 0.0f;
		const float r1 = (rounding_corners & 2) ? r : 0.0f;
		const float r2 = (rounding_corners & 4) ? r : 0.0f;
		const float r3 = (rounding_corners & 8) ? r : 0.0f;
		PathArcToFast(Vec2(a.x + r0, a.y + r0), r0, 6, 9);
		PathArcToFast(Vec2(b.x - r1, a.y + r1), r1, 9, 12);
		PathArcToFast(Vec2(b.x - r2, b.y - r2), r2, 0, 3);
		PathArcToFast(Vec2(a.x + r3, b.y - r3), r3, 3, 6);
	}
}

void COverManager::AddConvexPolyFilled(const Vec2* points, const int points_count, ColU32 col, bool anti_aliased)
{
	const Vec2 uv = COverManager::FontTexUvWhitePixel;
	anti_aliased = AntiAliasedShapes;

	if (anti_aliased)
	{
		// Anti-aliased Fill
		const float AA_SIZE = 1.0f;
		const ColU32 col_trans = col & COL32(255, 255, 255, 0);
		const int idx_count = (points_count - 2) * 3 + points_count * 6;
		const int vtx_count = (points_count * 2);
		PrimReserve(idx_count, vtx_count);

		// Add indexes for fill
		unsigned int vtx_inner_idx = _VtxCurrentIdx;
		unsigned int vtx_outer_idx = _VtxCurrentIdx + 1;
		for (int i = 2; i < points_count; i++)
		{
			_IdxWritePtr[0] = (DrawIdx)(vtx_inner_idx); _IdxWritePtr[1] = (DrawIdx)(vtx_inner_idx + ((i - 1) << 1)); _IdxWritePtr[2] = (DrawIdx)(vtx_inner_idx + (i << 1));
			_IdxWritePtr += 3;
		}

		// Compute normals
		Vec2* temp_normals = (Vec2*)alloca(points_count * sizeof(Vec2));
		for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
		{
			const Vec2& p0 = points[i0];
			const Vec2& p1 = points[i1];
			Vec2 diff = p1 - p0;
			diff *= ImInvLength(diff, 1.0f);
			temp_normals[i0].x = diff.y;
			temp_normals[i0].y = -diff.x;
		}

		for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
		{
			// Average normals
			const Vec2& n0 = temp_normals[i0];
			const Vec2& n1 = temp_normals[i1];
			Vec2 dm = (n0 + n1) * 0.5f;
			float dmr2 = dm.x*dm.x + dm.y*dm.y;
			if (dmr2 > 0.000001f)
			{
				float scale = 1.0f / dmr2;
				if (scale > 100.0f) scale = 100.0f;
				dm *= scale;
			}
			dm *= AA_SIZE * 0.5f;

			// Add vertices
			_VtxWritePtr[0].pos = (points[i1] - dm); _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;        // Inner
			_VtxWritePtr[1].pos = (points[i1] + dm); _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;  // Outer
			_VtxWritePtr += 2;

			// Add indexes for fringes
			_IdxWritePtr[0] = (DrawIdx)(vtx_inner_idx + (i1 << 1)); _IdxWritePtr[1] = (DrawIdx)(vtx_inner_idx + (i0 << 1)); _IdxWritePtr[2] = (DrawIdx)(vtx_outer_idx + (i0 << 1));
			_IdxWritePtr[3] = (DrawIdx)(vtx_outer_idx + (i0 << 1)); _IdxWritePtr[4] = (DrawIdx)(vtx_outer_idx + (i1 << 1)); _IdxWritePtr[5] = (DrawIdx)(vtx_inner_idx + (i1 << 1));
			_IdxWritePtr += 6;
		}
		_VtxCurrentIdx += (DrawIdx)vtx_count;
	}
	else
	{
		// Non Anti-aliased Fill
		const int idx_count = (points_count - 2) * 3;
		const int vtx_count = points_count;
		PrimReserve(idx_count, vtx_count);
		for (int i = 0; i < vtx_count; i++)
		{
			_VtxWritePtr[0].pos = points[i]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
			_VtxWritePtr++;
		}
		for (int i = 2; i < points_count; i++)
		{
			_IdxWritePtr[0] = (DrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (DrawIdx)(_VtxCurrentIdx + i - 1); _IdxWritePtr[2] = (DrawIdx)(_VtxCurrentIdx + i);
			_IdxWritePtr += 3;
		}
		_VtxCurrentIdx += (DrawIdx)vtx_count;
	}
}

void COverManager::AddPolyline(const Vec2* points, const int points_count, ColU32 col, bool closed, float thickness, bool anti_aliased)
{
	if (points_count < 2)
		return;

	const Vec2 uv = COverManager::FontTexUvWhitePixel;
	anti_aliased = AntiAliasedShapes;

	int count = points_count;
	if (!closed)
		count = points_count - 1;

	const bool thick_line = thickness > 1.0f;
	if (anti_aliased)
	{
		// Anti-aliased stroke
		const float AA_SIZE = 1.0f;
		const ColU32 col_trans = col & COL32(255, 255, 255, 0);

		const int idx_count = thick_line ? count * 18 : count * 12;
		const int vtx_count = thick_line ? points_count * 4 : points_count * 3;
		PrimReserve(idx_count, vtx_count);

		// Temporary buffer
		Vec2* temp_normals = (Vec2*)alloca(points_count * (thick_line ? 5 : 3) * sizeof(Vec2));
		Vec2* temp_points = temp_normals + points_count;

		for (int i1 = 0; i1 < count; i1++)
		{
			const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
			Vec2 diff = points[i2] - points[i1];
			diff *= ImInvLength(diff, 1.0f);
			temp_normals[i1].x = diff.y;
			temp_normals[i1].y = -diff.x;
		}
		if (!closed)
			temp_normals[points_count - 1] = temp_normals[points_count - 2];

		if (!thick_line)
		{
			if (!closed)
			{
				temp_points[0] = points[0] + temp_normals[0] * AA_SIZE;
				temp_points[1] = points[0] - temp_normals[0] * AA_SIZE;
				temp_points[(points_count - 1) * 2 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * AA_SIZE;
				temp_points[(points_count - 1) * 2 + 1] = points[points_count - 1] - temp_normals[points_count - 1] * AA_SIZE;
			}

			// FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
			unsigned int idx1 = _VtxCurrentIdx;
			for (int i1 = 0; i1 < count; i1++)
			{
				const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
				unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : idx1 + 3;

				// Average normals
				Vec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
				float dmr2 = dm.x*dm.x + dm.y*dm.y;
				if (dmr2 > 0.000001f)
				{
					float scale = 1.0f / dmr2;
					if (scale > 100.0f) scale = 100.0f;
					dm *= scale;
				}
				dm *= AA_SIZE;
				temp_points[i2 * 2 + 0] = points[i2] + dm;
				temp_points[i2 * 2 + 1] = points[i2] - dm;

				// Add indexes
				_IdxWritePtr[0] = (DrawIdx)(idx2 + 0); _IdxWritePtr[1] = (DrawIdx)(idx1 + 0); _IdxWritePtr[2] = (DrawIdx)(idx1 + 2);
				_IdxWritePtr[3] = (DrawIdx)(idx1 + 2); _IdxWritePtr[4] = (DrawIdx)(idx2 + 2); _IdxWritePtr[5] = (DrawIdx)(idx2 + 0);
				_IdxWritePtr[6] = (DrawIdx)(idx2 + 1); _IdxWritePtr[7] = (DrawIdx)(idx1 + 1); _IdxWritePtr[8] = (DrawIdx)(idx1 + 0);
				_IdxWritePtr[9] = (DrawIdx)(idx1 + 0); _IdxWritePtr[10] = (DrawIdx)(idx2 + 0); _IdxWritePtr[11] = (DrawIdx)(idx2 + 1);
				_IdxWritePtr += 12;

				idx1 = idx2;
			}

			// Add vertexes
			for (int i = 0; i < points_count; i++)
			{
				_VtxWritePtr[0].pos = points[i];          _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
				_VtxWritePtr[1].pos = temp_points[i * 2 + 0]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col_trans;
				_VtxWritePtr[2].pos = temp_points[i * 2 + 1]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col_trans;
				_VtxWritePtr += 3;
			}
		}
		else
		{
			const float half_inner_thickness = (thickness - AA_SIZE) * 0.5f;
			if (!closed)
			{
				temp_points[0] = points[0] + temp_normals[0] * (half_inner_thickness + AA_SIZE);
				temp_points[1] = points[0] + temp_normals[0] * (half_inner_thickness);
				temp_points[2] = points[0] - temp_normals[0] * (half_inner_thickness);
				temp_points[3] = points[0] - temp_normals[0] * (half_inner_thickness + AA_SIZE);
				temp_points[(points_count - 1) * 4 + 0] = points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
				temp_points[(points_count - 1) * 4 + 1] = points[points_count - 1] + temp_normals[points_count - 1] * (half_inner_thickness);
				temp_points[(points_count - 1) * 4 + 2] = points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness);
				temp_points[(points_count - 1) * 4 + 3] = points[points_count - 1] - temp_normals[points_count - 1] * (half_inner_thickness + AA_SIZE);
			}

			// FIXME-OPT: Merge the different loops, possibly remove the temporary buffer.
			unsigned int idx1 = _VtxCurrentIdx;
			for (int i1 = 0; i1 < count; i1++)
			{
				const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
				unsigned int idx2 = (i1 + 1) == points_count ? _VtxCurrentIdx : idx1 + 4;

				// Average normals
				Vec2 dm = (temp_normals[i1] + temp_normals[i2]) * 0.5f;
				float dmr2 = dm.x*dm.x + dm.y*dm.y;
				if (dmr2 > 0.000001f)
				{
					float scale = 1.0f / dmr2;
					if (scale > 100.0f) scale = 100.0f;
					dm *= scale;
				}
				Vec2 dm_out = dm * (half_inner_thickness + AA_SIZE);
				Vec2 dm_in = dm * half_inner_thickness;
				temp_points[i2 * 4 + 0] = points[i2] + dm_out;
				temp_points[i2 * 4 + 1] = points[i2] + dm_in;
				temp_points[i2 * 4 + 2] = points[i2] - dm_in;
				temp_points[i2 * 4 + 3] = points[i2] - dm_out;

				// Add indexes
				_IdxWritePtr[0] = (DrawIdx)(idx2 + 1); _IdxWritePtr[1] = (DrawIdx)(idx1 + 1); _IdxWritePtr[2] = (DrawIdx)(idx1 + 2);
				_IdxWritePtr[3] = (DrawIdx)(idx1 + 2); _IdxWritePtr[4] = (DrawIdx)(idx2 + 2); _IdxWritePtr[5] = (DrawIdx)(idx2 + 1);
				_IdxWritePtr[6] = (DrawIdx)(idx2 + 1); _IdxWritePtr[7] = (DrawIdx)(idx1 + 1); _IdxWritePtr[8] = (DrawIdx)(idx1 + 0);
				_IdxWritePtr[9] = (DrawIdx)(idx1 + 0); _IdxWritePtr[10] = (DrawIdx)(idx2 + 0); _IdxWritePtr[11] = (DrawIdx)(idx2 + 1);
				_IdxWritePtr[12] = (DrawIdx)(idx2 + 2); _IdxWritePtr[13] = (DrawIdx)(idx1 + 2); _IdxWritePtr[14] = (DrawIdx)(idx1 + 3);
				_IdxWritePtr[15] = (DrawIdx)(idx1 + 3); _IdxWritePtr[16] = (DrawIdx)(idx2 + 3); _IdxWritePtr[17] = (DrawIdx)(idx2 + 2);
				_IdxWritePtr += 18;

				idx1 = idx2;
			}

			// Add vertexes
			for (int i = 0; i < points_count; i++)
			{
				_VtxWritePtr[0].pos = temp_points[i * 4 + 0]; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col_trans;
				_VtxWritePtr[1].pos = temp_points[i * 4 + 1]; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
				_VtxWritePtr[2].pos = temp_points[i * 4 + 2]; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
				_VtxWritePtr[3].pos = temp_points[i * 4 + 3]; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col_trans;
				_VtxWritePtr += 4;
			}
		}
		_VtxCurrentIdx += (DrawIdx)vtx_count;
	}
	else
	{
		// Non Anti-aliased Stroke
		const int idx_count = count * 6;
		const int vtx_count = count * 4;      // FIXME-OPT: Not sharing edges
		PrimReserve(idx_count, vtx_count);

		for (int i1 = 0; i1 < count; i1++)
		{
			const int i2 = (i1 + 1) == points_count ? 0 : i1 + 1;
			const Vec2& p1 = points[i1];
			const Vec2& p2 = points[i2];
			Vec2 diff = p2 - p1;
			diff *= ImInvLength(diff, 1.0f);

			const float dx = diff.x * (thickness * 0.5f);
			const float dy = diff.y * (thickness * 0.5f);
			_VtxWritePtr[0].pos.x = p1.x + dy; _VtxWritePtr[0].pos.y = p1.y - dx; _VtxWritePtr[0].uv = uv; _VtxWritePtr[0].col = col;
			_VtxWritePtr[1].pos.x = p2.x + dy; _VtxWritePtr[1].pos.y = p2.y - dx; _VtxWritePtr[1].uv = uv; _VtxWritePtr[1].col = col;
			_VtxWritePtr[2].pos.x = p2.x - dy; _VtxWritePtr[2].pos.y = p2.y + dx; _VtxWritePtr[2].uv = uv; _VtxWritePtr[2].col = col;
			_VtxWritePtr[3].pos.x = p1.x - dy; _VtxWritePtr[3].pos.y = p1.y + dx; _VtxWritePtr[3].uv = uv; _VtxWritePtr[3].col = col;
			_VtxWritePtr += 4;

			_IdxWritePtr[0] = (DrawIdx)(_VtxCurrentIdx); _IdxWritePtr[1] = (DrawIdx)(_VtxCurrentIdx + 1); _IdxWritePtr[2] = (DrawIdx)(_VtxCurrentIdx + 2);
			_IdxWritePtr[3] = (DrawIdx)(_VtxCurrentIdx); _IdxWritePtr[4] = (DrawIdx)(_VtxCurrentIdx + 2); _IdxWritePtr[5] = (DrawIdx)(_VtxCurrentIdx + 3);
			_IdxWritePtr += 6;
			_VtxCurrentIdx += 4;
		}
	}
}