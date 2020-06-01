#pragma once

#include "VR.hpp"

//リサイズ
#define x_r(p1 , p2x) (int(p1) * p2x / deskx)
#define y_r(p1 , p2y) (int(p1) * p2y / desky)

class UI : Mainclass {
private:
	//font
	FontHandle font36;
	FontHandle font24;
	FontHandle font18;
	FontHandle font12;
	//
	int out_disp_x = 1920;
	int out_disp_y = 1080;
	int disp_x = 1920;
	int disp_y = 1080;
	//
	float start_fl = 0.f;

	GraphHandle bufScreen;
	GraphHandle outScreen;

	GraphHandle UI_VIVE;
	GraphHandle UI_safty;
	GraphHandle UI_select;
	GraphHandle UI_trigger;
	GraphHandle UI_mag_fall;
	GraphHandle UI_mag_set;
	float pt_pl = 0.f, pt_pr = 0.f;

	float point_r = 0.f;
	float point_uf = 0.f;
	float point_df = 0.f;
public:
	UI(const int& o_xd, const int& o_yd, const int& xd, const int& yd) {
		out_disp_x = o_xd;
		out_disp_y = o_yd;
		disp_x = xd;
		disp_y = yd;

		font36 = FontHandle::Create(y_r(36, out_disp_y), DX_FONTTYPE_EDGE);
		font24 = FontHandle::Create(y_r(24, out_disp_y), DX_FONTTYPE_EDGE);
		font18 = FontHandle::Create(y_r(18, out_disp_y), DX_FONTTYPE_EDGE);
		font12 = FontHandle::Create(y_r(12, out_disp_y), DX_FONTTYPE_EDGE);

		bufScreen = GraphHandle::Make(disp_x, disp_y, true);
		outScreen = GraphHandle::Make(disp_x, disp_y, true);
		SetTransColor(0, 255, 0);
		UI_VIVE = GraphHandle::Load("data/UI/pic.bmp");
		UI_safty = GraphHandle::Load("data/UI/safty.bmp");
		UI_select = GraphHandle::Load("data/UI/select.bmp");
		UI_trigger = GraphHandle::Load("data/UI/trigger.bmp");
		UI_mag_fall = GraphHandle::Load("data/UI/mag_fall.bmp");
		UI_mag_set = GraphHandle::Load("data/UI/mag_set.bmp");
		SetTransColor(0, 0, 0);
	}
	~UI() {
	}
	void load_window(const char* mes) {
		SetUseASyncLoadFlag(FALSE);
		float bar = 0.f, cnt = 0.f;
		auto all = GetASyncLoadNum();
		while (ProcessMessage() == 0) {
			const auto fps = GetFPS();
			SetDrawScreen(DX_SCREEN_BACK);
			ClearDrawScreen();
			{
				font18.DrawStringFormat(0, out_disp_y - y_r(70, out_disp_y), GetColor(0, 255, 0), " loading... : %04d/%04d  ", all - GetASyncLoadNum(), all);
				font12.DrawStringFormat(out_disp_x - font12.GetDrawWidthFormat("%s 読み込み中 ", mes), out_disp_y - y_r(70, out_disp_y), GetColor(0, 255, 0), "%s 読み込み中 ", mes);
				DrawBox(0, out_disp_y - y_r(50, out_disp_y), int(float(out_disp_x) * bar / float(all)), out_disp_y - y_r(40, out_disp_y), GetColor(0, 255, 0), TRUE);
				easing_set(&bar, float(all - GetASyncLoadNum()), 0.95f, fps);
			}
			ScreenFlip();
			if (GetASyncLoadNum() == 0) {
				cnt += 1.f / GetFPS();
				if (cnt > 1 && bar > float(all - GetASyncLoadNum()) * 0.95f) {
					break;
				}
			}
		}
	}
	//*
	void draw(
		const Mainclass::Chara& chara,
		const float& fps,
		const int& point ,
		const int& p_up,
		const int& p_down,
		const bool& vr = false
	) {
		//
		{
			FontHandle* font_big = (!vr) ? &font36 : &font24;
			FontHandle* font = (!vr) ? &font18 : &font12;
			{
				//ダメージ
				{
					int xp = disp_x / 2, yp = disp_y / 2 + disp_y / 12;

					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*point_df), 0, 255));
					font_big->DrawStringFormat(xp + font_big->GetDrawWidthFormat("[%d]", int(point_r)) / 2, yp, GetColor(50, 150, 255), " +%d", p_down);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*point_uf), 0, 255));
					font_big->DrawStringFormat(xp + font_big->GetDrawWidthFormat("[%d]", int(point_r)) / 2, yp, GetColor(255, 255, 0), " +%d", p_up);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*std::max(point_uf, point_df)), 0, 255));
					font_big->DrawStringFormat(xp - font_big->GetDrawWidthFormat("[%d]", int(point_r)) / 2, yp, GetColor(255, 0, 0), "[%d]", int(point_r));

					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

					if (point - 50.f / fps > point_r) {
						point_r += 50.f / fps;
						easing_set(&point_uf, 1.f, 0.9f, fps);
					}
					else {
						if (point + 10.f / fps < point_r) {
							point_r -= 10.f / fps;
							easing_set(&point_df, 1.f, 0.9f, fps);
						}
						else {
							point_r = float(point);
							easing_set(&point_df, 0.f, 0.975f, fps);
						}
						easing_set(&point_uf, 0.f, 0.975f, fps);
					}
				}
				//弾薬
				{
					int xp = 0, xs = 0, yp = 0, ys = 0;
					if (!vr) {
						xs = x_r(200, out_disp_x);
						xp = x_r(20, out_disp_x);
						ys = y_r(18, out_disp_y);
						yp = disp_y - y_r(50, out_disp_y);
					}
					else {
						xs = x_r(200, out_disp_x);
						xp = disp_x / 2 - x_r(20, out_disp_x)-xs;
						ys = y_r(12, out_disp_y);
						yp = disp_y / 2 + disp_y / 6 + y_r(20, out_disp_y);
					}
					int i = 0;
					{
						font->DrawString(xp, yp, chara.gunptr->name, GetColor(255, 255, 255));
						font->DrawStringFormat(
							xp + xs - font->GetDrawWidthFormat("%04d / %04d", chara.ammoc, chara.gunptr->ammo_max),
							yp + ys + y_r(2, out_disp_y), GetColor(255, 255, 255), "%04d / %04d", chara.ammoc, chara.gunptr->ammo_max);
						i++;
					}
				}
				//右手
				{
					if (!chara.reloadf && chara.ammoc == 0) {
						pt_pr = 2.f;
					}

					int xs = 500 / 4;
					int ys = 408 / 4;

					int xp = disp_x / 2 + disp_y / 6 - xs;
					int yp = disp_y / 2 + disp_y / 6 - ys;
					//元
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(128.f*pt_pr), 0, 128));
					UI_VIVE.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					if (pt_pr >= 0.f) {
						if (!chara.reloadf && chara.ammoc == 0) {
							UI_mag_fall.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
						}
					}
					if (pt_pr >= 0.f) {
						pt_pr -= 1.f / fps;
					}
				}
				//左手
				{
					if (chara.reloadf && !chara.down_mag) {
						pt_pl = 2.f;
					}

					int xs = 500 / 4;
					int ys = 408 / 4;

					int xp = disp_x / 2 - disp_y / 6;
					int yp = disp_y / 2 + disp_y / 6 - ys;
					//元
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(128.f*pt_pl), 0, 128));
					UI_VIVE.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					if (pt_pl >= 0.f) {
						if (chara.reloadf && !chara.down_mag) {
							UI_mag_set.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
						}
					}
					if (pt_pl >= 0.f) {
						pt_pl -= 1.f / fps;
					}
				}
			}
		}
	}
	//*/
};
