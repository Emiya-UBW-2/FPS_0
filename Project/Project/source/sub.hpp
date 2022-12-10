#pragma once
#include"Header.hpp"

//Box2D拡張
namespace std {
	template <>
	struct default_delete<b2Body> {
		void operator()(b2Body* body) const {
			body->GetWorld()->DestroyBody(body);
		}
	};
}; // namespace std
//
namespace FPS_n2 {
	// H : 色相　有効範囲 0.0f ～ 360.0f
	// S : 彩度　有効範囲 0.0f ～ 1.0f
	// V : 明度　有効範囲 0.0f ～ 1.0f
	int GetColorHSV(float H, float S, float V)
	{
		int hi;
		float f, p, q, t;
		float r, g, b;
		int ir, ig, ib;

		hi = (int)(H / 60.0f);
		hi = hi == 6 ? 5 : hi %= 6;
		f = H / 60.0f - (float)hi;
		p = V * (1.0f - S);
		q = V * (1.0f - f * S);
		t = V * (1.0f - (1.0f - f) * S);
		switch (hi)
		{
		case 0: r = V; g = t; b = p; break;
		case 1: r = q; g = V; b = p; break;
		case 2: r = p; g = V; b = t; break;
		case 3: r = p; g = q; b = V; break;
		case 4: r = t; g = p; b = V; break;
		case 5: r = V; g = p; b = q; break;
		}

		ir = (int)(r * 255.0f);
		if (ir > 255) ir = 255;
		else if (ir < 0) ir = 0;

		ig = (int)(g * 255.0f);
		if (ig > 255) ig = 255;
		else if (ig < 0) ig = 0;

		ib = (int)(b * 255.0f);
		if (ib > 255) ib = 255;
		else if (ib < 0) ib = 0;

		return GetColor(ir, ig, ib);
	}
	void GetRGBtoHSV(int r, int g, int b, float* H, float* S, float* V)
	{
		int MAX = std::max(std::max(r, g), b);
		int MIN = std::min(std::min(r, g), b);
		*V = 100.f * MAX / 256;

		if (MAX == MIN) {
			*H = 0.f;
			*S = 0.f;
		}
		else {
			if (MAX == r) {
				*H = 60.f * (g - b) / (MAX - MIN) + 0.f;
			}
			else if (MAX == g) {
				*H = 60.f * (b - r) / (MAX - MIN) + 120.f;
			}
			else if (MAX == b) {
				*H = 60.f * (r - g) / (MAX - MIN) + 240.f;
			}

			if (*H > 360.f) {
				*H = *H - 360.f;
			}
			else if (*H < 0) {
				*H = *H + 360.f;
			}
			*S = 100.f * (MAX - MIN) / MAX;
		}
	}
};
