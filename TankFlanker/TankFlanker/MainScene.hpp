
#pragma once

#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			BackGroundClass BackGround;		//BG
			GunClass Gun;					//e
			CharacterClass Chara;			//ƒLƒƒƒ‰“®ì
			//‘€ìŠÖ˜A
			float EyePosPer = 0.f;
			float EyeRunPer = 0.f;
			size_t m_FPS{ 0 };
			bool m_Flagfps{ true };

			bool m_RunPressFlag{ false };
		private:
		public:
			using TEMPSCENE::TEMPSCENE;
			void Set(void) noexcept override {
				TEMPSCENE::Set();
				//Load
				BackGround.Load();
				Gun.LoadModel("data/model/gun/model","data/model/gun/reticle.png");
				Chara.LoadModel("data/umamusume/ticket/model");
				//Set
				Chara.Set(&Gun);
				//Cam
				camera_main.set_cam_info(deg2rad(65), 1.f, 100.f);
				camera_main.set_cam_pos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));


				lens_zoom = 3.5f;
			}
			//
			bool Update(void) noexcept override {
				{
					int m_x = DXDraw::Instance()->disp_x / 2;
					int m_y = DXDraw::Instance()->disp_y / 2;
					if (IsFirstLoop) {
						SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
					}
					int mx, my;
					GetMousePoint(&mx, &my);
					m_RunPressFlag = CheckHitKey(KEY_INPUT_LSHIFT) != 0;
					Chara.Execute(
						std::clamp((float)(m_y - my)*50.f / 100.f *3.f* (camera_main.fov / deg2rad(65) / (use_lens ? lens_zoom : 1.f)), -3.f, 3.f) / 100.f,
						std::clamp((float)(mx - m_x)*50.f / 100.f *3.f* (camera_main.fov / deg2rad(65) / (use_lens ? lens_zoom : 1.f)), -9.f, 9.f) / 100.f,
						CheckHitKey(KEY_INPUT_W) != 0,
						CheckHitKey(KEY_INPUT_A) != 0,
						CheckHitKey(KEY_INPUT_D) != 0,
						(GetMouseInput() & MOUSE_INPUT_LEFT) != 0,
						(GetMouseInput() & MOUSE_INPUT_RIGHT) != 0,// || true,
						m_RunPressFlag,
						BackGround.GetGroundCol()
					);
					SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
				}
				{
					Gun.Execute();
				}

				use_lens = Chara.IsADS();
				lens_size = 400.f;

				//lens_size = Gun.GetLensSize();
				//lens_size = Gun.GetLensPos();

				m_FPS = std::clamp<size_t>(m_FPS + 1, 0, (CheckHitKey(KEY_INPUT_V) != 0) ? 2 : 0);
				if (m_FPS == 1) { m_Flagfps ^= 1; }
				if (m_Flagfps) {
					camera_main.campos = Chara.GetEyePosition();
					camera_main.campos = camera_main.campos + (Gun.GetScopePos() - camera_main.campos) * EyePosPer;

					camera_main.camvec = Chara.GetEyeVector();
					camera_main.camvec = camera_main.campos + camera_main.camvec + ((Gun.GetMatrix().zvec()*-1.f) - camera_main.camvec) * EyePosPer;
					if (Chara.IsADS()) {
						camera_main.near_ = 3.f;
						camera_main.far_ = 1000.f;
					}
					else {
						camera_main.near_ = 1.f;
						camera_main.far_ = 100.f;
					}
				}
				else {
					auto EyeVector = Chara.GetEyeVector();
					EyeVector = EyeVector + ((Gun.GetMatrix().zvec()*-1.f) - EyeVector) * EyePosPer;

					MATRIX_ref CamMat = Chara.GetUpper2WorldMatrix().GetRot();

					VECTOR_ref CamPos;
					CamPos += (Chara.GetCharaModelMatrix().pos() + VECTOR_ref::vget(0, 14.f, 0) + CamMat.xvec()*-8.f + CamMat.yvec()*3.f + CamMat.zvec()*1.f)*EyeRunPer;
					CamPos += (Chara.GetCharaMatrix().pos() + VECTOR_ref::vget(0, 14.f, 0) + CamMat.xvec()*-3.f + CamMat.yvec()*4.f + CamMat.zvec()*3.f)*(1.f - EyeRunPer);

					camera_main.camvec = CamPos + EyeVector * 100.f;
					camera_main.campos = CamPos + EyeVector * -20.f;

					if (!m_RunPressFlag) {
						easing_set(&EyeRunPer, 1.f, 0.95f);
					}
					else {
						easing_set(&EyeRunPer, 0.f, 0.9f);
					}
				}
				{
					if (Chara.IsADS()) {
						easing_set(&EyePosPer, 1.f, 0.9f);
						easing_set(&camera_main.fov, deg2rad(25), 0.8f);
					}
					else {
						easing_set(&EyePosPer, 0.f, 0.9f);
						easing_set(&camera_main.fov, deg2rad(65), 0.9f);
					}
					if (Chara.ShotSwitch()) {
						camera_main.fov -= deg2rad(10);
					}
				}
				if (Chara.ShotSwitch()) {
					auto mat = Gun.GetMuzzleMatrix();
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
				Gun.Draw();
				Chara.Draw();
			}
			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;

			void Main_Draw(void) noexcept override {
				BackGround.Draw();
				Gun.Draw();
				Chara.Draw();
				
				VECTOR_ref LensPos = ConvWorldPosToScreenPos(Gun.GetLensPos().get());
				if (0.f < LensPos.z() && LensPos.z() < 1.f) {
					lens_xpos = LensPos.x();
					lens_ypos = LensPos.y();

					VECTOR_ref LensPos = ConvWorldPosToScreenPos(Gun.GetLensPosSize().get());
					if (0.f < LensPos.z() && LensPos.z() < 1.f) {
						lens_size = std::hypotf(lens_xpos - LensPos.x(), lens_ypos - LensPos.y());
					}
				}
				if (!use_lens) {
					Reticle_on = false;
				}
				LensPos = ConvWorldPosToScreenPos(Gun.GetReticlePos().get());
				if (0.f < LensPos.z() && LensPos.z() < 1.f) {
					Reticle_on = true;
					Reticle_xpos = LensPos.x();
					Reticle_ypos = LensPos.y();
					if (lens_size <= std::hypotf(lens_xpos - Reticle_xpos, lens_ypos - Reticle_ypos)) {
						Reticle_on = false;
					}
				}
			}
			void LAST_Draw(void) noexcept override {
				if (Reticle_on) {
					Gun.GetReticle().DrawRotaGraph(Reticle_xpos, Reticle_ypos, lens_size / (4096.f / 2.f)*1.25f, 0.f, true);
				}
			}
		};
	};
};