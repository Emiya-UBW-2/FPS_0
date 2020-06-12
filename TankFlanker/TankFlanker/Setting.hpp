#pragma once

#include "DXLib_ref/DXLib_ref.h"

class Setting{
private:
	GraphHandle bufScreen;
	FontHandle font18;
	FontHandle font12;
	uint8_t up_c = 0;
	uint8_t down_c = 0;

	uint8_t selup_c = 0;
	uint8_t seldown_c = 0;
	uint8_t sel_p = 0;
	float sel_cnt = 0.f;

	bool dof_o = false;
	bool bloom_o = false;
	bool shadow_o = false;
	int shadow_level_o = 13;
	bool useVR_o = true;
	bool getlog_o = true;
public:
	bool dof_e = false;
	bool bloom_e = false;
	bool shadow_e = false;
	int shadow_level_e = 13;
	bool useVR_e = true;
	bool getlog_e = true;

	Setting() {
		SetOutApplicationLogValidFlag(FALSE);  /*log*/
		int mdata = FileRead_open("data/setting.txt", FALSE);
		dof_e = getparams::_bool(mdata);
		bloom_e = getparams::_bool(mdata);
		shadow_e = getparams::_bool(mdata);
		shadow_level_e = getparams::_int(mdata);
		useVR_e = getparams::_bool(mdata);
		getlog_e = getparams::_bool(mdata);
		FileRead_close(mdata);

		dof_o = dof_e;
		bloom_o = bloom_e;
		shadow_o = shadow_e;
		shadow_level_o = shadow_level_e;
		useVR_o = useVR_e;
		getlog_o = getlog_e;
	}

	~Setting() {
		save();
	}

	void save(void) {
		std::ofstream outputfile("data/setting.txt");
		outputfile << "hostpass=" << (dof_e ? "true" : "false") << "\n";
		outputfile << "bloom=" << (bloom_e ? "true" : "false") << "\n";
		outputfile << "shadow=" << (shadow_e ? "true" : "false") << "\n";
		outputfile << "shadow_level=" << shadow_level_e << "\n";
		outputfile << "usevr=" << (useVR_e ? "true" : "false") << "\n";
		outputfile << "getlog=" << (getlog_e ? "true" : "false") << "\n";
		outputfile.close();
	}
	void ready_draw_setting(void) {
		bufScreen = GraphHandle::Make(640, 480);
		font18 = FontHandle::Create(18, DX_FONTTYPE_EDGE);
		font12 = FontHandle::Create(12, DX_FONTTYPE_EDGE);
	}
	bool set_draw_setting(void) {
		bool restart = false;
		{
			selup_c = std::clamp<uint8_t>(selup_c + 1, 0, (CheckHitKey(KEY_INPUT_UP) != 0) ? 2 : 0);
			if (selup_c == 1) {
				if (sel_p == 0) {
					sel_p = 6;
				}
				--sel_p;
			}
			seldown_c = std::clamp<uint8_t>(seldown_c + 1, 0, (CheckHitKey(KEY_INPUT_DOWN) != 0) ? 2 : 0);
			if (seldown_c == 1) {
				++sel_p;
				if (sel_p == 6) {
					sel_p = 0;
				}
			}

			up_c = std::clamp<uint8_t>(up_c + 1, 0, (CheckHitKey(KEY_INPUT_RIGHT) != 0) ? 2 : 0);
			if (up_c == 1) {
				switch (sel_p) {
				case 0:
					dof_e ^= 1;
					break;
				case 1:
					bloom_e ^= 1;
					break;
				case 2:
					shadow_e ^= 1;
					break;
				case 3:
					++shadow_level_e;
					break;
				case 4:
					useVR_e ^= 1;
					break;
				case 5:
					getlog_e ^= 1;
					break;
				}
			}
			down_c = std::clamp<uint8_t>(down_c + 1, 0, (CheckHitKey(KEY_INPUT_LEFT) != 0) ? 2 : 0);
			if (down_c == 1) {
				switch (sel_p) {
				case 0:
					dof_e ^= 1;
					break;
				case 1:
					bloom_e ^= 1;
					break;
				case 2:
					shadow_e ^= 1;
					break;
				case 3:
					--shadow_level_e;
					break;
				case 4:
					useVR_e ^= 1;
					break;
				case 5:
					getlog_e ^= 1;
					break;
				}
			}
			shadow_level_e = std::clamp(shadow_level_e, 10, 14);
			//再起動フラグ
			{
				restart = true;
				if (dof_o != dof_e) { restart = false; }
				if (bloom_o != bloom_e) { restart = false; }
				if (shadow_o != shadow_e) { restart = false; }
				if (shadow_level_o != shadow_level_e) { restart = false; }
				if (useVR_o != useVR_e) { restart = false; }
				if (getlog_o != getlog_e) { restart = false; }
			}
		}
		bufScreen.SetDraw_Screen();
		{
			DrawBox(2, 2, 640, 480, GetColor(255, 0, 0), FALSE);
			int xp = 10 + 2;
			int xs1 = 200;
			int xs2 = 220;
			int yp = 24 + 2;
			//
			if (sel_p == 0) {
				font18.DrawString(xp, yp, "Depth of Field", GetColor(255, 255, 0));
				font18.DrawStringFormat(xp + xs1, yp, GetColor(255, 255, 0), "%s %s %s", ((sel_cnt >= 0.5f) ? "<" : " "), (dof_e ? "true" : "false"), ((sel_cnt >= 0.5f) ? ">" : " "));
				font18.DrawString(xp + xs2, yp + 18, "ピンぼけの有無", GetColor(255, 255, 0));
			}
			else {
				font12.DrawString(xp, yp, "Depth of Field", GetColor(255, 0, 0));
				font12.DrawStringFormat(xp + xs1, yp, GetColor(255, 0, 0), "  %s  ", (dof_e ? "true" : "false"));
				font12.DrawString(xp + xs2, yp + 18, "ピンぼけの有無", GetColor(255, 0, 0));
			}
			yp += 40;
			//
			if (sel_p == 1) {
				font18.DrawString(xp, yp, "Bloom", GetColor(255, 255, 0));
				font18.DrawStringFormat(xp + xs1, yp, GetColor(255, 255, 0), "%s %s %s", ((sel_cnt >= 0.5f) ? "<" : " "), (shadow_e ? "true" : "false"), ((sel_cnt >= 0.5f) ? ">" : " "));
				font18.DrawString(xp + xs2, yp + 18, "光沢の有無", GetColor(255, 255, 0));
			}
			else {
				font12.DrawString(xp, yp, "Bloom", GetColor(255, 0, 0));
				font12.DrawStringFormat(xp + xs1, yp, GetColor(255, 0, 0), "  %s  ", (shadow_e ? "true" : "false"));
				font12.DrawString(xp + xs2, yp + 18, "光沢の有無", GetColor(255, 0, 0));
			}
			yp += 40;
			//
			if (sel_p == 2) {
				font18.DrawString(xp, yp, "Shadow", GetColor(255, 255, 0));
				font18.DrawStringFormat(xp + xs1, yp, GetColor(255, 255, 0), "%s %s %s", ((sel_cnt >= 0.5f) ? "<" : " "), (shadow_e ? "true" : "false"), ((sel_cnt >= 0.5f) ? ">" : " "));
				font18.DrawString(xp + xs2, yp + 18, "影の有無", GetColor(255, 255, 0));
			}
			else {
				font12.DrawString(xp, yp, "Shadow", GetColor(255, 0, 0));
				font12.DrawStringFormat(xp + xs1, yp, GetColor(255, 0, 0), "  %s  ", (shadow_e ? "true" : "false"));
				font12.DrawString(xp + xs2, yp + 18, "影の有無", GetColor(255, 0, 0));
			}
			yp += 40;
			//
			if (sel_p == 3) {
				font18.DrawString(xp, yp, "Shadow Level", GetColor(255, 255, 0));
				font18.DrawStringFormat(xp + xs1, yp, GetColor(255, 255, 0), "%s %d %s", ((sel_cnt >= 0.5f) ? "<" : " "), (shadow_level_e - 10), ((sel_cnt >= 0.5f) ? ">" : " "));
				font18.DrawString(xp + xs2, yp + 18, "影のクオリティ", GetColor(255, 255, 0));
			}
			else {
				font12.DrawString(xp, yp, "Shadow Level", GetColor(255, 0, 0));
				font12.DrawStringFormat(xp + xs1, yp, GetColor(255, 0, 0), "  %d  ", (shadow_level_e - 10));
				font12.DrawString(xp + xs2, yp + 18, "影のクオリティ", GetColor(255, 0, 0));
			}
			yp += 40;
			//
			if (sel_p == 4) {
				font18.DrawString(xp, yp, "Use VR", GetColor(255, 255, 0));
				font18.DrawStringFormat(xp + xs1, yp, GetColor(255, 255, 0), "%s %s %s", ((sel_cnt >= 0.5f) ? "<" : " "), (useVR_e ? "true" : "false"), ((sel_cnt >= 0.5f) ? ">" : " "));
				font18.DrawString(xp + xs2, yp + 18, "VRモードの使用", GetColor(255, 255, 0));
			}
			else {
				font12.DrawString(xp, yp, "Use VR", GetColor(255, 0, 0));
				font12.DrawStringFormat(xp + xs1, yp, GetColor(255, 0, 0), "  %s  ", (useVR_e ? "true" : "false"));
				font12.DrawString(xp + xs2, yp + 18, "VRモードの使用", GetColor(255, 0, 0));
			}
			yp += 40;
			//
			if (sel_p == 5) {
				font18.DrawString(xp, yp, "Get Log", GetColor(255, 255, 0));
				font18.DrawStringFormat(xp + xs1, yp, GetColor(255, 255, 0), "%s %s %s", ((sel_cnt >= 0.5f) ? "<" : " "), (getlog_e ? "true" : "false"), ((sel_cnt >= 0.5f) ? ">" : " "));
				font18.DrawString(xp + xs2, yp + 18, "DXLibのログファイルを出力するか否か", GetColor(255, 255, 0));
			}
			else {
				font12.DrawString(xp, yp, "Get Log", GetColor(255, 0, 0));
				font12.DrawStringFormat(xp + xs1, yp, GetColor(255, 0, 0), "  %s  ", (getlog_e ? "true" : "false"));
				font12.DrawString(xp + xs2, yp + 18, "DXLibのログファイルを出力するか否か", GetColor(255, 0, 0));
			}
			yp += 40;
			//
			font18.DrawString(640 - font18.GetDrawWidth("再起動して設定を適応") - 6, 480 - 18 - 6, "再起動して設定を適応", (!restart) ? GetColor(255, 0, 0) : GetColor(128, 0, 0));
		}
		sel_cnt += 1.f / GetFPS();
		if (sel_cnt >= 1.f) {
			sel_cnt = 0.f;
		}
		return restart;
	}
	auto& settinggraphs(void) { return bufScreen; }
};
