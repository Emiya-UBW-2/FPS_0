#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class InputClass {
			float										m_MouseX{ 0.f };
			float										m_MouseY{ 0.f };
			switchs										m_FPSActive;
			switchs										m_MouseActive;
			int											m_LookMode{ 0 };

			float										m_TPS_Xrad{ 0.f };
			float										m_TPS_Yrad{ 0.f };
			float										m_TPS_XradR{ 0.f };
			float										m_TPS_YradR{ 0.f };
			float										m_TPS_Per{ 1.f };

			InputControl								m_MyInput;
		public:
			const auto&		GetMouseX(void) const noexcept { return m_MouseX; }
			const auto&		GetMouseY(void) const noexcept { return m_MouseY; }
			const auto&		GetMouseActive(void) const noexcept { return m_MouseActive; }
			const auto&		GetTPS_XradR(void) const noexcept { return m_TPS_XradR; }
			const auto&		GetTPS_YradR(void) const noexcept { return m_TPS_YradR; }
			const auto&		GetTPS_Per(void) const noexcept { return m_TPS_Per; }
			const auto&		GetInput(void) const noexcept { return m_MyInput; }
			void			SetInput(unsigned int pFlags) noexcept { this->m_MyInput.SetKeyInput(pFlags); }
		public:
			void Init() {
				this->m_FPSActive.Set(true);
				this->m_MouseActive.Set(true);
			}
			void Execute(float FovPer,bool isAlive) {
				bool look_key = false;
				bool eyechange_key = false;
				bool Lockon_key = false;

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
							m_MouseX = std::clamp(-(float)(-input.Rz) / 100.f*1.f, -9.f, 9.f) * FovPer;
							m_MouseY = std::clamp((float)(input.Z) / 100.f*1.f, -9.f, 9.f) * FovPer;

							float deg = rad2deg(std::atan2f((float)input.X, -(float)input.Y));
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
							//視点切り替え
							look_key = (input.Buttons[11] != 0);
							//eyechange_key = (input.Buttons[11]!=0);
							//十字
							deg = (float)(input.POV[0]) / 100.f;
							bool right_key = (40.f <= deg && deg <= 140.f);
							bool left_key = (220.f <= deg && deg <= 320.f);
							bool up_key = (310.f <= deg || deg <= 50.f);
							bool down_key = (130.f <= deg && deg <= 230.f);
							//ボタン
							Lockon_key = (input.Buttons[0] != 0);/*△*/
							//_key = (input.Buttons[2] != 0);/*×*/
							VECTOR_ref ReCoil;
							m_MyInput.SetInput(
								m_MouseX*(1.f - this->m_TPS_Per) - ReCoil.y(),
								m_MouseY*(1.f - this->m_TPS_Per) - ReCoil.x(),
								w_key, s_key, a_key, d_key,
								(input.Buttons[10] != 0),
								(input.Buttons[6] != 0), (input.Buttons[7] != 0),
								right_key, left_key, up_key, down_key,
								(input.Buttons[10] != 0), (input.Buttons[5] != 0), (input.Buttons[4] != 0), (input.Buttons[3] != 0)/*□*/, (input.Buttons[1] != 0)/*〇*/,
								false
							);
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
					this->m_MouseActive.Execute((CheckHitKeyWithCheck(KEY_INPUT_TAB) != 0) && isAlive);
					if (!isAlive) {
						this->m_MouseActive.Set(true);
					}
					int mx = DXDraw::Instance()->m_DispXSize / 2, my = DXDraw::Instance()->m_DispYSize / 2;
					if (!this->m_MouseActive.on() || !isAlive) {
						SetMouseDispFlag(TRUE);
					}
					else {
						if (this->m_MouseActive.trigger()) {
							SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
						}
						GetMousePoint(&mx, &my);
						SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
						SetMouseDispFlag(FALSE);
					}
					m_MouseX = std::clamp(-(float)(my - DXDraw::Instance()->m_DispYSize / 2)*1.f, -9.f, 9.f) * FovPer;
					m_MouseY = std::clamp((float)(mx - DXDraw::Instance()->m_DispXSize / 2)*1.f, -9.f, 9.f) * FovPer;
					look_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0) && this->m_MouseActive.on();
					eyechange_key = CheckHitKeyWithCheck(KEY_INPUT_V) != 0;
					Lockon_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0) && this->m_MouseActive.on();

					VECTOR_ref ReCoil;
					m_MyInput.SetInput(
						m_MouseX*(1.f - this->m_TPS_Per) - ReCoil.y(),
						m_MouseY*(1.f - this->m_TPS_Per) - ReCoil.x(),
						(CheckHitKeyWithCheck(KEY_INPUT_W) != 0), (CheckHitKeyWithCheck(KEY_INPUT_S) != 0), (CheckHitKeyWithCheck(KEY_INPUT_A) != 0), (CheckHitKeyWithCheck(KEY_INPUT_D) != 0),
						(CheckHitKeyWithCheck(KEY_INPUT_LSHIFT) != 0),
						(CheckHitKeyWithCheck(KEY_INPUT_Q) != 0), (CheckHitKeyWithCheck(KEY_INPUT_E) != 0),
						(CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0), (CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0), (CheckHitKeyWithCheck(KEY_INPUT_UP) != 0), (CheckHitKeyWithCheck(KEY_INPUT_DOWN) != 0),

						(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0),
						(CheckHitKeyWithCheck(KEY_INPUT_R) != 0),
						(CheckHitKeyWithCheck(KEY_INPUT_X) != 0),
						(CheckHitKeyWithCheck(KEY_INPUT_C) != 0),
						(((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0) && this->m_MouseActive.on()),
						(((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0) && this->m_MouseActive.on())
					);
				}


				this->m_FPSActive.Execute(eyechange_key);
				if (look_key) {
					this->m_LookMode = 1;
				}
				else {
					this->m_LookMode = 0;
				}
				Easing(&this->m_TPS_Per, (!this->m_FPSActive.on() && (this->m_LookMode == 1)) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

				this->m_TPS_Xrad += m_MouseX;
				this->m_TPS_Yrad += m_MouseY;
				this->m_TPS_Xrad = std::clamp(this->m_TPS_Xrad, deg2rad(-60), deg2rad(60));
				if (this->m_TPS_Yrad >= deg2rad(180)) { this->m_TPS_Yrad -= deg2rad(360); }
				if (this->m_TPS_Yrad <= deg2rad(-180)) { this->m_TPS_Yrad += deg2rad(360); }

				this->m_TPS_Xrad *= this->m_TPS_Per;
				this->m_TPS_Yrad *= this->m_TPS_Per;

				Easing(&this->m_TPS_XradR, this->m_TPS_Xrad, 0.5f, EasingType::OutExpo);

				this->m_TPS_YradR += (sin(this->m_TPS_Yrad)*cos(this->m_TPS_YradR) - cos(this->m_TPS_Yrad) * sin(this->m_TPS_YradR))*20.f / FPS;
			}
		};
	};
};
