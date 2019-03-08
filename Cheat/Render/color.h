#pragma once

#include <d3d9.h>
#pragma comment(lib,"d3d9.lib")

#include <stdint.h>
#include <cmath>
#include "../Engine/SDK/Color.hpp"

//rgba float[4] to color_t, it should be faster than color_t(float*) constructor 
#define PF2COL(color) reinterpret_cast<color_t>(( \
(((color[3] * 255.f) & 0xff) << 24) | (((color[0] * 255.f) & 0xff) << 16) | \
(((color[1] * 255.f) & 0xff) << 8) | ((color[2] * 255.f) & 0xff)))

struct color_t
{
	D3DCOLOR color = 0;

	inline uint8_t* a() { return reinterpret_cast<uint8_t*>(&color + 3); }
	inline uint8_t* r() { return reinterpret_cast<uint8_t*>(&color + 2); }
	inline uint8_t* g() { return reinterpret_cast<uint8_t*>(&color + 1); }
	inline uint8_t* b() { return reinterpret_cast<uint8_t*>(&color + 0); }

	inline const float get_a() { return *a(); }
	inline const float get_r() { return *r(); }
	inline const float get_g() { return *g(); }
	inline const float get_b() { return *b(); }


	inline int* red() { return (int*)r(); }
	inline int* green() { return (int*)g(); }
	inline int* blue() { return (int*)b(); }

	inline void set_a(const float a) { reinterpret_cast<uint8_t*>(&color)[3] = static_cast<uint8_t>(a * 255.f); }
	inline void set_r(const float r) { reinterpret_cast<uint8_t*>(&color)[2] = static_cast<uint8_t>(r * 255.f); }
	inline void set_g(const float g) { reinterpret_cast<uint8_t*>(&color)[1] = static_cast<uint8_t>(g * 255.f); }
	inline void set_b(const float b) { reinterpret_cast<uint8_t*>(&color)[0] = static_cast<uint8_t>(b * 255.f); }

	color_t() : color(0) {}
	color_t(int value) : color(value) {}
	color_t(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF) : color(D3DCOLOR_RGBA(r, g, b, a)) {}
	color_t(float* value) : color(D3DCOLOR_COLORVALUE(value[0], value[1], value[2], value[3])) {}
	color_t(SDK::Color clr) :color(D3DCOLOR_RGBA(clr.r(), clr.g(), clr.b(), clr.a())) {}

	inline color_t& operator=(const int& rhs)
	{
		color = rhs;
		return *this;
	}
	inline color_t& operator=(const D3DCOLOR& rhs)
	{
		color = rhs;
		return *this;
	}
	inline color_t& operator=(const float* rhs)
	{
		color = D3DCOLOR_COLORVALUE(rhs[0], rhs[1], rhs[2], rhs[3]);
		return *this;
	}
	inline operator const D3DCOLOR() const { return color; }
	inline operator const uint8_t*() const { return (uint8_t*)this; }

	void SetHSV(float h, float s, float v, float a = 1.f)
	{
		if (s == 0.f)// gray
		{
			color = D3DCOLOR_COLORVALUE(v, v, v, a);
			return;
		}

		h = fmodf(h, 1.f) / (60.f / 360.f);

		const int i = static_cast<int>(h);
		const float f = h - static_cast<float>(i);
		const float q = v * (1.f - s * f);
		const float t = v * (1.f - s * (1.0f - f));
		const float p = v * (1.f - s);

		float r, g, b;

		switch (i)
		{
		case 0: r = v; g = t; b = p; break;
		case 1: r = q; g = v; b = p; break;
		case 2: r = p; g = v; b = t; break;
		case 3: r = p; g = q; b = v; break;
		case 4: r = t; g = p; b = v; break;
		default: r = v; g = p; b = q; break;
		}

		color = D3DCOLOR_COLORVALUE(r, g, b, a);
	}
};

#define DEF_COLOR(value, name) namespace Colors { static color_t name = color_t(value); }

DEF_COLOR(0xFF000000, Black);
DEF_COLOR(0xFFFFFFFF, White);

DEF_COLOR(0xFFFF0000, Red);
DEF_COLOR(0xFF00FF00, Green);
DEF_COLOR(0xFF0000FF, Blue);

DEF_COLOR(0xFFFFFF00, Yellow);
DEF_COLOR(0xFF00FFFF, SkyBlue);
DEF_COLOR(0xFFFF00FF, Pink);

//Flat UI Colors
DEF_COLOR(0xFF1abc9c, TURQUOISE)
DEF_COLOR(0xFF2ecc71, EEMERALD)
DEF_COLOR(0xFF2ecc71, EEMERALD_BUTTON)
DEF_COLOR(0xFF3498db, PETER_RIVER)

DEF_COLOR(0xFF3498db, PETER_RIVER_ITEM)

DEF_COLOR(0xFF9b59b6, AMETHYST)
DEF_COLOR(0xFF34495e, WET_ASPHALT)
DEF_COLOR(0xFF34495e, WET_ASPHALT2)
DEF_COLOR(0xFF16a085, GREEN_SEA)
DEF_COLOR(0xFF27ae60, NEPHRITIS)
DEF_COLOR(0xFF27ae60, NEPHRITIS_BUTTON)
DEF_COLOR(0xFF2980b9, BELIZE_HOLE)
DEF_COLOR(0xFF8e44ad, WISTERIA)
DEF_COLOR(0xFF2c3e50, MIDNIGHT_BLUE)
DEF_COLOR(0xFF2c3e50, MIDNIGHT_BLUE_BUTTON)

DEF_COLOR(0xFF2c3e50, MIDNIGHT_BLUE_ITEMCHECK)
DEF_COLOR(0xff2f3640, BLACK_BUFFER)
DEF_COLOR(0xff2f3640, BLACK_BUFFER_QTEXT)
DEF_COLOR(0xff2f3640, BLACK_BUFFER_ACTIVE)
DEF_COLOR(0xff2f3640, BLACK_BUFFER_SLIDER)

DEF_COLOR(0xFFf1c40f, SUN_FLOWER)
DEF_COLOR(0xFFe67e22, CARROT)
DEF_COLOR(0xFFe74c3c, ALIZARIN)
DEF_COLOR(0xFFecf0f1, CLOUDS)
DEF_COLOR(0xFF95a5a6, CONCRETE)

DEF_COLOR(0xFF95a5a6, CONCRETE_ITEM)
DEF_COLOR(0xFF95a5a6, CONCRETE_ITEM_CHECKED)

DEF_COLOR(0xFFf39c12, ORANGE)
DEF_COLOR(0xFFd35400, PUMPKIN)
DEF_COLOR(0xFFc0392b, POMEGRANATE)
DEF_COLOR(0xFFbdc3c7, SILVER)
DEF_COLOR(0xFF7f8c8d, ASBESTOS)

//American UI
DEF_COLOR(0xFF2d3436, DRACULA_ORCHID);

//Deutch
DEF_COLOR(0xFFd1d8e0, TWINKIE_BLUE);
DEF_COLOR(0xFFd1d8e0, TWINKIE_BLUE_ACTIVE)

DEF_COLOR(0xFFFF2E27, SCROLL_RED)
DEF_COLOR(0xFF126337, SCROLL_GREEN)
DEF_COLOR(0xFF20283C, SCROLL_BLUE)