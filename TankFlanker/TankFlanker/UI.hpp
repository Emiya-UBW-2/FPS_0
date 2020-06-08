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
	float pt_pe = 0.f;
	float point_r = 0.f;
	float point_uf = 0.f;
	float point_df = 0.f;
	//
	float ready_f = 0.f;
	float ready_yp = 0.f;
	//
	int tgt_pic_sel = -1;
	float tgt_pic_on = 1.f;
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
	template<class Y, class D>
	int select_window(
		bool useVR_e,
		std::vector<Mainclass::Gun>& gun_data,
		std::unique_ptr<Y, D>& vrparts
	) {
		float fov = deg2rad(useVR_e ? 90 : 45);
		int sel_g = 0;
		VECTOR_ref campos;
		float gun_yrad = 90.f;
		VECTOR_ref pos_HMD;
		MATRIX_ref mat_HMD;
		uint8_t changecnt = 0;
		bool endp = false;
		bool startp = false;
		float ber_r = 0.f;
		start_fl = 0.f;
		//
		while (ProcessMessage() == 0) {
			const auto fps = GetFPS();
			const auto waits = GetNowHiPerformanceCount();
			if (!startp) {
				//VR用
				vrparts->GetDevicePositionVR(vrparts->get_hmd_num(), &pos_HMD, &mat_HMD);
				if (useVR_e) {
					if (vrparts->get_hand1_num() != -1) {
						auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand1_num()];
						if (ptr_.turn && ptr_.now) {
							changecnt = std::clamp<uint8_t>(changecnt + 1, 0, (((ptr_.on[1] & BUTTON_SIDE) != 0) ? 2 : 0));
							//引き金
							if ((ptr_.on[0] & BUTTON_TRIGGER) != 0) {
								startp = true;
							}
						}
					}
				}
				else {
					changecnt = std::clamp<uint8_t>(changecnt + 1, 0, ((CheckHitKey(KEY_INPUT_P) != 0) ? 2 : 0));
					if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
						startp = true;
					}
					//マウスで回転
				}
				//
				if (changecnt == 1) {
					++sel_g %= gun_data.size();
					ber_r = 0.f;
				}
			}
			else {
				changecnt = 0;
			}

			easing_set(&ber_r, float(out_disp_y / 4), 0.95f, fps);

			{
				if (!startp) {
					campos = mat_HMD.zvec() * 0.6f;
					gun_yrad += 10.f/fps;
					if (gun_yrad >= 180.f) {
						gun_yrad = -180.f;
					}
				}
				else {
					easing_set(&campos, VGet(0.f, 0.f, 1.f), 0.95f, fps);
					easing_set(&gun_yrad, 90.f, 0.95f, fps);
					start_fl += 1.f / fps;
					if (start_fl > 3.f) {
						endp = true;
					}
				}
			}
			//VR空間に適用
			vrparts->Move_Player();
			{
				bufScreen.SetDraw_Screen();
				auto& v = gun_data[sel_g];
				{
					int xp = disp_x / 2 - disp_y / 6;
					int yp = disp_y / 2 - disp_y / 6;
					font18.DrawStringFormat(xp, yp, GetColor(0, 255, 0), "Name  :%s", v.name.c_str());
				}
				outScreen.SetDraw_Screen(0.1f, 10.f, fov, campos, VGet(0, 0, 0), VGet(0.f, 1.f, 0.f));
				{
					v.mod.obj.SetMatrix(MATRIX_ref::RotY(deg2rad(gun_yrad)));
					v.mod.obj.DrawModel();
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(255 - int(255.f * start_fl / 1.f), 0, 255));
					bufScreen.DrawExtendGraph(0, 0, disp_x, disp_y, true);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f * start_fl / 3.f), 0, 255));
					DrawBox(0, 0, disp_x, disp_y, GetColor(255, 255, 255), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				if (useVR_e) {
					for (char i = 0; i < 2; i++) {
						GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
						outScreen.DrawGraph(0, 0, false);
						vrparts->PutEye((ID3D11Texture2D*)GetUseDirect3D11BackBufferTexture2D(), i);
					}
				}
				GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
				{
					outScreen.DrawExtendGraph(0, 0, out_disp_x, out_disp_y, true);
				}
			}
			DXDraw::Screen_Flip();
			vrparts->Eye_Flip(waits);//フレーム開始の数ミリ秒前にstartするまでブロックし、レンダリングを開始する直前に呼び出す必要があります。
			if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
				sel_g = -1;
				break;
			}
			if (endp) {
				break;
			}
		}
		return sel_g;
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
	void set_draw(
		const Mainclass::Chara& chara,
		const bool& c_start,
		const bool& c_end,
		const float& c_timer,
		const int& point ,
		const int& p_up,
		const int& p_down,
		const bool& vr = false
	) {
		//
		const float fps = GetFPS();
		bufScreen.SetDraw_Screen();
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
				//開始
				{
					int xp = disp_x / 2;
					int yp = disp_y / 2-int(ready_yp);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f*ready_f), 0, 255));
					if (!c_end) {
						if (!c_start) {
							font->DrawString(xp - font->GetDrawWidth("READY") / 2, yp, "READY", GetColor(255, 0, 0));
							ready_f = 1.f;
							ready_yp = 0.f;
						}
						else {
							ready_f -= 1.f / fps;
							if (ready_f <= 0.75f) {
								ready_f = 0.75f;
								easing_set(&ready_yp, float(disp_y / 6), 0.95f, fps);

								font->DrawStringFormat(xp - font->GetDrawWidthFormat("%d:%05.2f", 0, c_timer) / 2, yp, GetColor(255, 0, 0), "%d:%05.2f", 0, c_timer);
							}
							else {
								font->DrawString(xp - font->GetDrawWidth("START!") / 2, yp, "START!", GetColor(255, 0, 0));
							}
						}
					}
					else {
						easing_set(&ready_f, 1.f, 0.9f, fps);
						font->DrawString(xp - font->GetDrawWidth("TIME OUT!") / 2, yp, "TIME OUT!", GetColor(255, 0, 0));
						easing_set(&ready_yp, float(disp_y / 8+ ((!vr) ? y_r(18, out_disp_y) : y_r(12, out_disp_y))), 0.95f, fps);
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
				//スコアボード
				if (c_end) {
					int xs = disp_y / 4;
					int ys = disp_y / 4;
					int xp = disp_x / 2 - xs / 2;
					int yp = disp_y / 2 - ys / 2;
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
					DrawBox(xp, yp, xp + xs, yp + ys, GetColor(0, 0, 0), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					{
						//スコアズ
						font->DrawStringFormat(xp, yp, GetColor(255, 0, 0), "TOTAL POINT : %d", point); yp += (!vr) ? y_r(18, out_disp_y) : y_r(12, out_disp_y);
					}
				}
				//モードその他
				{
					if (chara.reloadf && !chara.down_mag) {
						pt_pe = 2.f;
					}

					int xp = disp_x / 2;
					int yp = disp_y / 2 + disp_y / 8;
					//元
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(128.f*pt_pe), 0, 128));
					if (pt_pe >= 0.f) {
						if (chara.ammoc == 0) {
							font->DrawString(xp - font->GetDrawWidth("EMPTY") / 2, yp, "EMPTY", GetColor(255, 0, 0)); yp += (!vr) ? y_r(18, out_disp_y) : y_r(12, out_disp_y);
						}
						pt_pe -= 1.f / fps;
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
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
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(192.f*pt_pr), 0, 192));
					UI_VIVE.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					if (pt_pr >= 0.f) {
						if (!chara.reloadf && chara.ammoc == 0) {
							UI_mag_fall.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
						}
						/*
							UI_safty.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
							UI_select.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
							UI_trigger.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
						*/
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
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(192.f*pt_pl), 0, 192));
					UI_VIVE.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					if (pt_pl >= 0.f) {
						if (chara.reloadf && !chara.down_mag) {
							UI_mag_set.DrawExtendGraph(xp, yp, xp + xs, yp + ys, true);
						}
						pt_pl -= 1.f / fps;
					}
				}
			}
		}
	}
	void draw() {
		bufScreen.DrawExtendGraph(0, 0, disp_x, disp_y, true);
	}

	template <class T>
	void TGT_drw(
		std::vector<T> &tgt_pic,
		const VECTOR_ref& pos,
		const VECTOR_ref& vec,
		const int& tgt_pic_x,
		const int& tgt_pic_y
	) {
		const float fps = GetFPS();
		{
			int i = 0;
			bool fl = false;
			for (auto& tp : tgt_pic) {
				auto q = tp.obj.CollCheck_Line(pos, VECTOR_ref(pos) + vec * 100.f, 0, 1);
				if (q.HitFlag == TRUE) {
					tgt_pic_sel = i;
					fl = true;
				}
				i++;
			}
			if (fl) {
				easing_set(&tgt_pic_on, 1.f, 0.9f, fps);
			}
			else {
				easing_set(&tgt_pic_on, 0.f, 0.95f, fps);
			}
		}
		if (tgt_pic_sel >= 0) {
			int xp = 60;
			int yp = 60;
			int xs = 160;
			int ys = 160 * tgt_pic_y / tgt_pic_x;

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f* tgt_pic_on));
			VECTOR_ref p = ConvWorldPosToScreenPos(((tgt_pic[tgt_pic_sel].obj.frame(tgt_pic[tgt_pic_sel].x_frame) + tgt_pic[tgt_pic_sel].obj.frame(tgt_pic[tgt_pic_sel].y_frame)) / 2.f).get());
			if (p.z() >= 0.f&&p.z() <= 1.f) {
				DrawLine(xp + xs / 2, yp + ys / 2, int(p.x()*out_disp_x / disp_x), int(p.y()*out_disp_y / disp_y), GetColor(255, 0, 0), 2);
			}
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128 + int(127.f* tgt_pic_on));
			tgt_pic[tgt_pic_sel].pic.DrawExtendGraph(xp, yp, xp + xs, yp + ys, false);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f* tgt_pic_on));
			DrawBox(xp, yp, xp + xs, yp + ys, GetColor(255, 0, 0), FALSE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}
};
