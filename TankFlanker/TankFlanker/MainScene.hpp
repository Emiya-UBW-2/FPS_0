
#pragma once

#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			BackGroundClass BackGround;		//BG
			GunClass Gun;					//銃
			CharacterClass Chara;			//キャラ動作
			std::array<TargetClass,6> Target;
			//操作関連
			float EyePosPer_Prone = 0.f;
			float EyePosPer = 0.f;
			float EyeRunPer = 0.f;
			size_t m_FPS{ 0 };
			bool m_Flagfps{ true };
			bool m_RunPressFlag{ false };
			bool m_AimPressFlag{ false };
			size_t m_Ecnt{ 0 };
			bool m_FlagE{ false };
			size_t m_Qcnt{ 0 };
			bool m_FlagQ{ false };
			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;
		private:
		public:
			using TEMPSCENE::TEMPSCENE;
			void Set(void) noexcept override {
				Set_EnvLight(
					VECTOR_ref::vget(1.f, 1.f, 1.f),
					VECTOR_ref::vget(-1.f, -1.f, -1.f),
					VECTOR_ref::vget(-0.25f, -0.5f, 0.0f),
					GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				TEMPSCENE::Set();
				//Load
				BackGround.Load();
				Gun.LoadModel("data/model/gun/");
				Chara.LoadModel("data/umamusume/ticket/");
				Gun.LoadReticle("data/model/gun/reticle.png");
				for (auto& t : Target) {
					t.LoadModel("data/model/Target/");
				}
				//init
				Chara.Init();
				Gun.Init();
				for (auto& t : Target) {
					t.Init();
				}
				Chara.SetCol(&BackGround.GetGroundCol());
				Gun.SetCol(&BackGround.GetGroundCol());
				for (auto& t : Target) {
					t.SetCol(&BackGround.GetGroundCol());
				}
				//Set
				Chara.Set(&Gun);
				Target[0].SetMat(deg2rad(90), VECTOR_ref::vget(732.f, 15.11f, -974.20f + 20.f));
				Target[1].SetMat(deg2rad(90), VECTOR_ref::vget(732.f, 15.11f, -974.20f));
				Target[2].SetMat(deg2rad(90), VECTOR_ref::vget(732.f, 15.11f, -974.20f - 20.f));

				Target[3].SetMat(deg2rad(90), VECTOR_ref::vget(732.f - 12.5f*100.f, 15.11f, -974.20f + 20.f));
				Target[4].SetMat(deg2rad(90), VECTOR_ref::vget(732.f - 12.5f*100.f, 15.11f, -974.20f));
				Target[5].SetMat(deg2rad(90), VECTOR_ref::vget(732.f - 12.5f*100.f, 15.11f, -974.20f - 20.f));
				//Cam
				camera_main.set_cam_info(deg2rad(65), 1.f, 100.f);
				camera_main.set_cam_pos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
				lens_zoom = 3.5f;

				m_Flagfps = false;
			}
			//
			bool Update(void) noexcept override {
				if (IsFirstLoop) {
					SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
				}
				float xadd = 0.f;
				int mx, my;
				GetMousePoint(&mx, &my);
				SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
				SetMouseDispFlag(FALSE);
				{
					m_Ecnt = std::clamp<size_t>(m_Ecnt + 1, 0, (CheckHitKey(KEY_INPUT_E) != 0) ? 2 : 0);
					if (m_Ecnt == 1) {
						m_FlagE ^= 1;
						if (m_FlagE) { m_FlagQ = false; }
					}
					m_Qcnt = std::clamp<size_t>(m_Qcnt + 1, 0, (CheckHitKey(KEY_INPUT_Q) != 0) ? 2 : 0);
					if (m_Qcnt == 1) {
						m_FlagQ ^= 1;
						if (m_FlagQ) { m_FlagE = false; }
					}
					if (!Chara.IsRun()) {
						m_FlagQ = false;
						m_FlagE = false;
					}

					if (m_FlagQ || m_FlagE) {
						if (Chara.IsSprint()) {
							xadd = 0.093f*3.f*(m_FlagQ ? -1.f : 1.f);//スプリント
						}
						else if (Chara.IsRun()) {
							xadd = 0.0615f*3.f*(m_FlagQ ? -1.f : 1.f);//走り
						}
					}
					m_RunPressFlag = CheckHitKey(KEY_INPUT_LSHIFT) != 0;
					m_AimPressFlag = (GetMouseInput() & MOUSE_INPUT_RIGHT) != 0;
				}
				//
				Chara.SetInput(
					std::clamp(-(float)(my - DXDraw::Instance()->disp_y / 2)*100.f / 100.f *1.f* (camera_main.fov / deg2rad(65) / (use_lens ? lens_zoom : 1.f)), -9.f, 9.f) / 100.f,
					std::clamp(((float)(mx - DXDraw::Instance()->disp_x / 2) + xadd)*100.f / 100.f *1.f* (camera_main.fov / deg2rad(65) / (use_lens ? lens_zoom : 1.f)), -9.f, 9.f) / 100.f,
					CheckHitKey(KEY_INPUT_W) != 0,
					CheckHitKey(KEY_INPUT_S) != 0,
					CheckHitKey(KEY_INPUT_A) != 0,
					CheckHitKey(KEY_INPUT_D) != 0,
					CheckHitKey(KEY_INPUT_C) != 0,
					CheckHitKey(KEY_INPUT_X) != 0,
					(GetMouseInput() & MOUSE_INPUT_LEFT) != 0,
					m_AimPressFlag,
					m_RunPressFlag
				);

				Chara.Execute();
				Gun.Execute();

				if (Gun.GetIsHit()) {
					Effect_UseControl::SetSpeed_Effect(Effect::ef_fire, 1.f);
					Effect_UseControl::Set_Effect(Effect::ef_fire, Gun.GetHitPos(), Gun.GetHitVec(), 1.f);
				}

				for (auto& t : Target) {
					Gun.CheckCol(t.GetCol());
					if (Gun.GetIsHit()) {
						t.SetHitPos(Gun.GetHitPos());
						Effect_UseControl::SetSpeed_Effect(Effect::ef_fire, 1.f);
						Effect_UseControl::Set_Effect(Effect::ef_fire, Gun.GetHitPos(), Gun.GetHitVec(), 1.f);
					}
					t.Execute();
				}

				m_FPS = std::clamp<size_t>(m_FPS + 1, 0, (CheckHitKey(KEY_INPUT_V) != 0) ? 2 : 0);
				if (m_FPS == 1) { m_Flagfps ^= 1; }

				if (m_Flagfps || m_AimPressFlag) {
					camera_main.campos = Chara.GetEyePosition();
					camera_main.camvec = camera_main.campos + Chara.GetEyeVector();

					camera_main.camup = Chara.GetMatrix().GetRot().yvec();
				}
				else {
					VECTOR_ref EyeVector;
					EyeVector += Chara.GetEyeVector() * (1.f - EyePosPer);
					EyeVector += (Gun.GetMatrix().zvec()*-1.f) * EyePosPer;

					//EyeVector*=-1.f;
					//EyeVector = VECTOR_ref::right();

					MATRIX_ref CamMat = Chara.GetUpper2WorldMatrix().GetRot();
					VECTOR_ref CamPos;
					CamPos = Chara.GetMatrix().pos() + VECTOR_ref::vget(0, (14.f*(1.f - EyePosPer_Prone) + 6.f*EyePosPer_Prone), 0);
					CamPos += (CamMat.xvec()*-8.f + CamMat.yvec()*3.f)*(1.f - EyeRunPer);
					CamPos += (CamMat.xvec()*-3.f + CamMat.yvec()*4.f)*EyeRunPer;

					camera_main.camvec = CamPos + EyeVector * 100.f;
					camera_main.campos = CamPos + EyeVector * (-20.f*(1.f- EyePosPer_Prone) + 2.f*EyePosPer_Prone);//*2.f;

					camera_main.camup = Chara.GetMatrix().GetRot().yvec();

					if (m_RunPressFlag) {
						easing_set(&EyeRunPer, 1.f, 0.95f);
					}
					else {
						easing_set(&EyeRunPer, 0.f, 0.9f);
					}
				}
				{
					camera_main.campos = camera_main.campos + (Gun.GetScopePos() - camera_main.campos) * EyePosPer;
					camera_main.camvec = camera_main.camvec + ((Gun.GetScopePos() + Gun.GetMatrix().zvec()*-1.f) - camera_main.camvec) * EyePosPer;
					if (Chara.IsADS()) {
						easing_set(&EyePosPer, 1.f, 0.8f);
						easing_set(&EyePosPer_Prone, 0.f, 0.8f);
						easing_set(&camera_main.fov, deg2rad(17), 0.8f);
						camera_main.near_ = 10.f;
						camera_main.far_ = 12.5f * 200.f;
					}
					else {
						easing_set(&EyePosPer, 0.f, 0.8f);
						if (Chara.IsRun()) {
							easing_set(&camera_main.fov, deg2rad(85), 0.9f);
							easing_set(&camera_main.near_, 3.f, 0.9f);
							easing_set(&camera_main.far_, 12.5f * 50.f, 0.9f);
						}
						else {
							easing_set(&camera_main.fov, deg2rad(65), 0.9f);
							easing_set(&camera_main.near_, 10.f, 0.9f);
							easing_set(&camera_main.far_, 12.5f * 200.f, 0.9f);
						}
						if (Chara.IsProne()) {
							easing_set(&EyePosPer_Prone, 1.f, 0.8f);
						}
						else {
							easing_set(&EyePosPer_Prone, 0.f, 0.8f);
						}
					}
					if (Chara.ShotSwitch()) {
						camera_main.fov -= deg2rad(10);
					}
				}
				if (Chara.ShotSwitch()) {
					auto mat = Gun.GetMuzzleMatrix();
					//Effect_UseControl::SetSpeed_Effect(Effect::ef_fire2, 5.f);
					Effect_UseControl::Set_Effect(Effect::ef_fire2, mat.pos(), mat.GetRot().zvec()*-1.f, 1.f);
				}

				TEMPSCENE::Update();
				Effect_UseControl::Update_Effect();
				return true;
			}
			void Dispose(void) noexcept override {
				Effect_UseControl::Dispose_Effect();
				Gun.Dispose();
				Chara.Dispose();
				for (auto& t : Target) {
					t.Dispose();
				}
			}
			//
			void UI_Draw(void) noexcept  override {
			}
			void BG_Draw(void) noexcept override {
				BackGround.BG_Draw();
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				BackGround.Shadow_Draw_NearFar();
			}
			void Shadow_Draw(void) noexcept override {
				BackGround.Shadow_Draw();
				Gun.Draw();
				Chara.Draw();
			}

			void Main_Draw(void) noexcept override {
				BackGround.Draw();
				for (auto& t : Target) {
					t.Draw();
				}
				Gun.Draw();
				Chara.Draw();
				
				//SetFogEnable(TRUE);
				SetFogStartEnd(50.f, 100.f);
				SetFogColor(0, 0, 0);

				use_lens = Chara.IsADS();
				if (use_lens) {
					VECTOR_ref LensPos = ConvWorldPosToScreenPos(Gun.GetLensPos().get());
					if (0.f < LensPos.z() && LensPos.z() < 1.f) {
						lens_xpos = LensPos.x();
						lens_ypos = LensPos.y();

						VECTOR_ref LensPos = ConvWorldPosToScreenPos(Gun.GetLensPosSize().get());
						if (0.f < LensPos.z() && LensPos.z() < 1.f) {
							lens_size = std::hypotf(lens_xpos - LensPos.x(), lens_ypos - LensPos.y());
						}
					}
					LensPos = ConvWorldPosToScreenPos(Gun.GetReticlePos().get());
					if (0.f < LensPos.z() && LensPos.z() < 1.f) {
						Reticle_xpos = LensPos.x();
						Reticle_ypos = LensPos.y();
						Reticle_on = (lens_size > std::hypotf(lens_xpos - Reticle_xpos, lens_ypos - Reticle_ypos));
					}
				}
				else {
					Reticle_on = false;
				}
			}
			void LAST_Draw(void) noexcept override {
				if (Reticle_on) {
					Gun.GetReticle().DrawRotaGraph(Reticle_xpos, Reticle_ypos, lens_size / (4096.f / 2.f)*1.25f, 0.f, true);
				}

				int x = 0;
				int y = 10;
				for (auto& t : Target) {
					t.DrawHitCard(x,y);
					x += 200 + 10;
				}
			}
		};
	};
};