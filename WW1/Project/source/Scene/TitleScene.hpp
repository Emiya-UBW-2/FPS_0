#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TitleScene : public TEMPSCENE {
		private:
			switchs UpKey;
			switchs DownKey;
			switchs LeftKey;
			switchs RightKey;
			switchs OKKey;
			switchs NGKey;

			int select{ 0 };
			float SelYadd[3] = { 0.f,0.f,0.f };

			float GameFadeIn{ 0.f };
			float GameStart{ 0.f };
			float GameStart2{ 0.f };
			float GameStart3{ 0.f };
		public:
			TitleScene(void) noexcept { }
			void			Set_Sub(void) noexcept override {
				Get_Next()->Load();
				auto SE = SoundPool::Instance();
				//サウンド
				//this->m_BGM = SoundHandle::Load("data/Sound/BGM/Beethoven8_2.wav");
				SE->Add((int)SoundEnum::StandUp, 1, "data/Sound/SE/move/sliding.wav", false);
				SE->Add((int)SoundEnum::Siren, 1, "data/Sound/SE/siren.wav", false);

				SE->Add((int)SoundEnum::UI_Select, 2, "data/Sound/UI/cursor.wav", false);
				SE->Add((int)SoundEnum::UI_OK, 1, "data/Sound/UI/hit.wav", false);
				SE->Add((int)SoundEnum::UI_NG, 1, "data/Sound/UI/cancel.wav", false);
				//this->m_BGM.vol(128);

				select = 0;
				GameFadeIn = 3.f;
				GameStart = 0.f;
				GameStart2 = 0.f;
				GameStart3 = 0.f;

				auto* KeyGuide = FPS_n2::KeyGuideClass::Instance();

				KeyGuide->AddGuide("none.jpg", "決定");
				KeyGuide->AddGuide("X.jpg", "戻る");
				KeyGuide->AddGuide("W.jpg", "");
				KeyGuide->AddGuide("S.jpg", "上下選択");
				KeyGuide->AddGuide("A.jpg", "");
				KeyGuide->AddGuide("D.jpg", "調整");
			}
			//
			bool			Update_Sub(bool* isPause) noexcept override {
				if (*isPause) {
					return true;
				}
#ifdef DEBUG
				auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
				DebugParts->SetPoint("update start");
#endif // DEBUG
				auto SE = SoundPool::Instance();
				//FirstDoingv
				if (GetIsFirstLoop()) {
				}

				GameFadeIn = std::max(GameFadeIn - 1.f / FPS, 0.f);

				if (GetJoypadNum() > 0) {
					DINPUT_JOYSTATE input;
					int padID = DX_INPUT_PAD1;
					GetJoypadInputState(padID);
					switch (GetJoypadType(padID)) {
					case DX_PADTYPE_OTHER:
					case DX_PADTYPE_DUAL_SHOCK_4:
					case DX_PADTYPE_DUAL_SENSE:
					case DX_PADTYPE_SWITCH_JOY_CON_L:
					case DX_PADTYPE_SWITCH_JOY_CON_R:
					case DX_PADTYPE_SWITCH_PRO_CTRL:
					case DX_PADTYPE_SWITCH_HORI_PAD:
						GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
						{
							//pp_x = std::clamp(-(float)(input.Rz) / 100.f*0.35f, -9.f, 9.f) * cam_per;
							//pp_y = std::clamp((float)(input.Z) / 100.f*0.35f, -9.f, 9.f) * cam_per;
							float deg = rad2deg(atan2f((float)input.X, -(float)input.Y));
							bool w_key = false;
							bool s_key = false;
							bool a_key = false;
							bool d_key = false;
							if (!(input.X == 0 && input.Y == 0)) {
								w_key = (-50.f <= deg && deg <= 50.f);
								a_key = (-140.f <= deg && deg <= -40.f);
								s_key = (130.f <= deg || deg <= -130.f);
								d_key = (40.f <= deg && deg <= 140.f);
							}
							//十字
							//deg = (float)(input.POV[0]) / 100.f;
							//bool right_key = (40.f <= deg && deg <= 140.f);
							//bool left_key = (220.f <= deg && deg <= 320.f);
							//bool up_key = (310.f <= deg || deg <= 50.f);
							//bool down_key = (130.f <= deg && deg <= 230.f);

							//ボタン
							//(input.Buttons[0] != 0)/*□*/
							//(input.Buttons[1] != 0)/*×*/
							//(input.Buttons[2] != 0)/*〇*/
							//(input.Buttons[3] != 0)/*△*/
							//(input.Buttons[4] != 0)/*L1*/
							//(input.Buttons[5] != 0)/*R1*/
							//(input.Buttons[6] != 0)/*L2*/
							//(input.Buttons[7] != 0)/*R2*/
							//(input.Buttons[8] != 0)/**/
							//(input.Buttons[9] != 0)/**/
							//(input.Buttons[10] != 0)/*L3*/
							//(input.Buttons[11] != 0)/*R3*/
							UpKey.Execute(w_key);
							DownKey.Execute(s_key);
							LeftKey.Execute(a_key);
							RightKey.Execute(d_key);
							OKKey.Execute((input.Buttons[1] != 0)/*×*/);
							NGKey.Execute((input.Buttons[2] != 0)/*〇*/);

						}
						break;
					case DX_PADTYPE_XBOX_360:
					case DX_PADTYPE_XBOX_ONE:
						break;
					default:
						break;
					}
				}
				else {//キーボード
					UpKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_W) != 0 || CheckHitKeyWithCheck(KEY_INPUT_UP) != 0);
					DownKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_S) != 0 || CheckHitKeyWithCheck(KEY_INPUT_DOWN) != 0);
					LeftKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_A) != 0 || CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0);
					RightKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_D) != 0 || CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0);
					OKKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0);
					NGKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_X) != 0);
				}
				if (!OptionWindowClass::Instance()->IsActive()) {
					if (GameStart == 0.f && GameStart2 == 0.f) {
						if (UpKey.trigger()) {
							--select;
							if (select < 0) { select = 2; }
							SelYadd[select] = 10.f;

							SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (DownKey.trigger()) {
							++select;
							if (select > 2) { select = 0; }
							SelYadd[select] = -10.f;

							SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
					}
					for (int i = 0; i < 3; i++) {
						Easing(&SelYadd[i], 0.f, 0.95f, EasingType::OutExpo);
					}

					switch (select) {
					case 0:
						if (OKKey.trigger()) {
							SE->Get((int)SoundEnum::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (!(GameStart == 0.f && !OKKey.trigger())) {
							GameStart += 1.f / FPS / 0.5f;
						}
						break;
					case 1:
						if (OKKey.trigger()) {
							SE->Get((int)SoundEnum::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (!(GameStart2 == 0.f && !OKKey.trigger())) {
							GameStart2 += 1.f / FPS / 0.5f;
						}
						break;
					case 2:
						if (OKKey.trigger()) {
							SE->Get((int)SoundEnum::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (OKKey.trigger()) {
							OptionWindowClass::Instance()->SetActive(true);
						}
						break;
					default:
						break;
					}
				}

				if (GameStart >= 1.f || GameStart2>=1.f) {
					g_Mode = select;
					return false;
				}
				return true;
			}
			void			Dispose_Sub(void) noexcept override {
				//auto SE = SoundPool::Instance();
			}
			//
			void			Depth_Draw_Sub(void) noexcept override {}
			void			BG_Draw_Sub(void) noexcept override {}
			void			ShadowDraw_Far_Sub(void) noexcept override {}
			void			ShadowDraw_NearFar_Sub(void) noexcept override {}
			void			ShadowDraw_Sub(void) noexcept override {}
			void			MainDraw_Sub(void) noexcept override {}
			void			MainDrawbyDepth_Sub(void) noexcept override {}
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept  override {
			}
			void			DrawUI_In_Sub(void) noexcept override {
				auto Red = GetColor(255, 0, 0);
				auto Red75 = GetColor(192, 0, 0);
				auto White = GetColor(255, 255, 255);
				auto Gray75 = GetColor(128, 128, 128);
				auto Gray = GetColor(64, 64, 64);

				auto* DrawParts = DXDraw::Instance();
				auto* Fonts = FontPool::Instance();
				auto kill = FPS_n2::SaveDataClass::Instance()->GetParam("KillCount");
				auto killTotal = 80;
				if (kill >= killTotal) {
					DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(64, 8, 8), TRUE);
				}
				else {
					DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(16, 8, 8), TRUE);
				}
				int xp1, yp1;

				xp1 = y_r(64 + 600);
				yp1 = y_r(64 + 48 * 2);
				if (kill >= killTotal) {
					auto WhiteR = GetColor(255, 64, 64);
					auto GrayR = GetColor(128, 16, 16);
					Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 3 / 2), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), GrayR, Gray, "Final EpiSode");
					Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48 * 2), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, WhiteR, Gray, "Final EpiSode");
				}
				else {
					DrawFetteString(xp1, yp1, 1.33f, false, "Final EpiSode");
				}
				//
				xp1 = y_r(1920 - 256 - 54 * 2 - (int)(GameStart*100.f));
				yp1 = y_r(1080 - 108 - 108 * 2 + (int)SelYadd[0]);

				{
					auto per = std::clamp((1.f - GameStart), 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
					Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 3 / 2 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), (select == 0) ? Red75 : Gray75, Gray, "Start Game");
					Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48 * 2 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, (select == 0) ? Red : White, Gray, "Start Game");
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}

				xp1 = y_r(1920 - 256 - 54 * 1 - (int)(GameStart2*100.f));
				yp1 = y_r(1080 - 108 - 108 * 1 + (int)SelYadd[1]);

				{
					auto per = std::clamp((1.f - GameStart2), 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
					if (kill >= killTotal) {
						auto WhiteR = GetColor(255, 64, 64);
						auto GrayR = GetColor(128, 16, 16);

						Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 - y_r(500), yp1, Gray75, Gray, "%02d / %02d", kill, killTotal);

						Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 3 / 2 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), (select == 1) ? Red75 : GrayR, Gray, "Red Baron Mode");
						Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48 * 2 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, (select == 1) ? Red : WhiteR, Gray, "Red Baron Mode");
					}
					else {
						DrawFetteString(xp1, yp1, 1.f, (select == 1), "Free Flight");

						Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 - y_r(350), yp1, White, Gray, "%02d / %02d", kill, killTotal);
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}

				xp1 = y_r(1920 - 256);
				yp1 = y_r(1080 - 108 + (int)SelYadd[2]);
				DrawFetteString(xp1, yp1, 1.f, (select == 2), "Option");


				{
					auto per = std::clamp((GameFadeIn), 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

					DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);

					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				{
					auto per = std::clamp(std::max(GameStart, GameStart2), 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

					DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(255, 255, 255), TRUE);

					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
			}
		};
	};
};
