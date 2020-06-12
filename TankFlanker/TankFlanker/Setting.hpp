#pragma once

#include "DXLib_ref/DXLib_ref.h"

class Setting{
private:
	GraphHandle bufScreen;
	FontHandle font18;
	FontHandle font12;
	uint8_t up_c = 0;
	uint8_t down_c = 0;

	uint8_t sel_p = 0;
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
	void set_draw_setting(void) {
		bufScreen.SetDraw_Screen();
		{
			DrawBox(2, 2, 640, 480, GetColor(255, 0, 0), FALSE);
			up_c = std::clamp<uint8_t>(up_c + 1, 0, (CheckHitKey(KEY_INPUT_P) != 0) ? 2 : 0);
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
			down_c = std::clamp<uint8_t>(down_c + 1, 0, (CheckHitKey(KEY_INPUT_P) != 0) ? 2 : 0);
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

			int xp = 10 + 2;
			int yp = 24 + 2;
			font18.DrawStringFormat(xp, yp, (sel_p == 0) ? GetColor(255, 255, 0) : GetColor(255, 0, 0), "hostpass    %s", (dof_e ? "true" : "false")); yp += 20;
			font18.DrawStringFormat(xp, yp, (sel_p == 1) ? GetColor(255, 255, 0) : GetColor(255, 0, 0), "bloom       %s", (bloom_e ? "true" : "false")); yp += 20;
			font18.DrawStringFormat(xp, yp, (sel_p == 2) ? GetColor(255, 255, 0) : GetColor(255, 0, 0), "shadow      %s", (shadow_e ? "true" : "false")); yp += 20;
			font18.DrawStringFormat(xp, yp, (sel_p == 3) ? GetColor(255, 255, 0) : GetColor(255, 0, 0), "shadowlevel %d", shadow_level_e); yp += 20;
			font18.DrawStringFormat(xp, yp, (sel_p == 4) ? GetColor(255, 255, 0) : GetColor(255, 0, 0), "usevr       %s", (useVR_e ? "true" : "false")); yp += 20;
			font18.DrawStringFormat(xp, yp, (sel_p == 5) ? GetColor(255, 255, 0) : GetColor(255, 0, 0), "getlog      %s", (getlog_e ? "true" : "false")); yp += 20;
		}
	}
	auto& settinggraphs(void) { return bufScreen; }
};
