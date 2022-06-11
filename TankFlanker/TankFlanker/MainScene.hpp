#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
		private:
			GraphHandle HeartGraph;
			FontPool UI;

			int intParam[1];
			float floatParam[1];
		public:
			void Set() {
				HeartGraph = GraphHandle::Load("data/UI/Heart.png");
			}
			void Draw() {
				auto* DrawParts = DXDraw::Instance();

				int siz = y_r(32);
				int xP = siz, yP = DrawParts->disp_y - siz;
				int xo, yo;
				HeartGraph.GetSize(&xo, &yo);
				HeartGraph.DrawRotaGraph(xP, yP, (float)(siz) / xo * floatParam[0], 0.f, true);
				UI.Get(y_r(24)).Get_handle().DrawStringFormat(xP + siz / 2, yP + siz / 2 - y_r(24), GetColor(255, 0, 0), "%03d", intParam[0]);
			}

			void SetIntParam(int ID, int value) { intParam[ID] = value; }
			void SetfloatParam(int ID, float value) { floatParam[ID] = value; }
		};

		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			//リソース関連
			ObjectManager Obj;				//モデル
			BackGroundClass BackGround;		//BG
			SoundHandle Env;
			//関連
			int tgtSel = 0;
			float tgtTimer = 0.f;
			int tgt_num = 6;
			//操作関連
			float EyePosPer_Prone = 0.f;
			float EyePosPer = 0.f;
			float EyeRunPer = 0.f;
			switchs FPSActive;
			switchs MouseActive;
			switchs RunKey;
			switchs ADSKey;
			switchs QKey;
			switchs EKey;
			int m_TurnRate{ 0 };
			//UI関連
			UIClass UI_class;
			//銃関連
			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;
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

				for (int i = 0; i < tgt_num; i++) {
					Obj.AddObject(ObjType::Target);
					Obj.LoadObj("data/model/Target/");
				}
				Obj.AddObject(ObjType::Gun);
				Obj.LoadObj("data/gun/gun/");
				Obj.AddObject(ObjType::Human);
				Obj.LoadObj("data/umamusume/ticket/");

				auto& Gun = (std::shared_ptr<GunClass>&)(Obj.GetObj(ObjType::Gun, 0));
				auto& Chara = (std::shared_ptr<CharacterClass>&)(Obj.GetObj(ObjType::Human, 0));

				Gun->LoadReticle("data/gun/gun/reticle.png");
				//init
				Obj.InitObject(&BackGround.GetGroundCol());
				//ロード
				SetCreate3DSoundFlag(FALSE);
				Env = SoundHandle::Load("data/Sound/SE/envi.wav");
				SetCreate3DSoundFlag(FALSE);
				Env.vol(64);
				//UI
				UI_class.Set();
				//Set
				for (int i = 0; i < tgt_num; i++) {
					auto& t = Obj.GetObj(ObjType::Target, i);
					t->SetMove(deg2rad(90), VECTOR_ref::vget(732.f - 12.5f*100.f*(i / 3), 15.11f, -974.20f + 20 - 20.f*(i % 3)));
				}
				Chara->SetGunPtr(Gun);

				tgtSel = -1;
				tgtTimer = 0.f;
				//Cam
				camera_main.set_cam_info(deg2rad(65), 1.f, 100.f);
				camera_main.set_cam_pos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
				Set_zoom_lens(3.5f);
				//入力
				FPSActive.Init(false);
				MouseActive.Init(true);
			}
			//
			bool Update(void) noexcept override {
				auto& Chara = (std::shared_ptr<CharacterClass>&)(Obj.GetObj(ObjType::Human, 0));
				auto& Gun = (std::shared_ptr<GunClass>&)(Obj.GetObj(ObjType::Gun, 0));
				//FirstDoing
				if (IsFirstLoop) {
					SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
					Env.play(DX_PLAYTYPE_LOOP, TRUE);
				}
				//Input
				{
					MouseActive.GetInput(CheckHitKey_M(KEY_INPUT_TAB) != 0);
					FPSActive.GetInput(CheckHitKey_M(KEY_INPUT_V) != 0);
					RunKey.GetInput(CheckHitKey_M(KEY_INPUT_LSHIFT) != 0);
					ADSKey.GetInput((GetMouseInput_M() & MOUSE_INPUT_RIGHT) != 0);
					float xadd = 0.f;
					int mx = DXDraw::Instance()->disp_x / 2, my = DXDraw::Instance()->disp_y / 2;
					if (MouseActive.on()) {
						if (MouseActive.trigger()) {
							SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
						}
						GetMousePoint(&mx, &my);
						SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
						SetMouseDispFlag(FALSE);
					}
					else {
						SetMouseDispFlag(TRUE);
					}
					{
						EKey.GetInput(CheckHitKey_M(KEY_INPUT_E) != 0);
						QKey.GetInput(CheckHitKey_M(KEY_INPUT_Q) != 0);
						if (EKey.trigger()) {
							m_TurnRate--;
						}
						if (QKey.trigger()) {
							m_TurnRate++;
						}
						if (!Chara->IsRun()) {
							m_TurnRate = 0;
						}

						m_TurnRate = std::clamp(m_TurnRate, -1, 1);

						if (Chara->IsSprint()) {
							xadd = 0.093f*3.f*(-m_TurnRate);//スプリント
						}
						else if (Chara->IsRun()) {
							xadd = 0.0615f*3.f*(-m_TurnRate);//走り
						}
					}
					Chara->SetInput(
						std::clamp(-(float)(my - DXDraw::Instance()->disp_y / 2)*100.f / 100.f *1.f* (camera_main.fov / deg2rad(65) / (is_lens() ? zoom_lens() : 1.f)), -9.f, 9.f) / 100.f,
						std::clamp(((float)(mx - DXDraw::Instance()->disp_x / 2) + xadd)*100.f / 100.f *1.f* (camera_main.fov / deg2rad(65) / (is_lens() ? zoom_lens() : 1.f)), -9.f, 9.f) / 100.f,
						CheckHitKey_M(KEY_INPUT_W) != 0,
						CheckHitKey_M(KEY_INPUT_S) != 0,
						CheckHitKey_M(KEY_INPUT_A) != 0,
						CheckHitKey_M(KEY_INPUT_D) != 0,
						CheckHitKey_M(KEY_INPUT_C) != 0,
						CheckHitKey_M(KEY_INPUT_X) != 0,
						(GetMouseInput_M() & MOUSE_INPUT_LEFT) != 0,
						ADSKey.press(),
						RunKey.press()
					);
				}
				//Execute
				Obj.ExecuteObject();
				//col
				if (Gun->CheckBullet(&BackGround.GetGroundCol())) {
					//エフェクト
					Effect_UseControl::Set_Effect(Effect::ef_fire, Gun->GetHitPos(), Gun->GetHitVec(), 1.f);
				}
				for (int i = 0; i < tgt_num; i++) {
					auto& t = (std::shared_ptr<TargetClass>&)(Obj.GetObj(ObjType::Target, i));
					if (Gun->CheckBullet(t->GetCol())) {
						if (tgtSel != -1 && tgtSel != i) {
							auto& tOLD = (std::shared_ptr<TargetClass>&)(Obj.GetObj(ObjType::Target, tgtSel));
							tOLD->ResetHit();
						}
						tgtSel = i;
						tgtTimer = 5.f;
						t->SetHitPos(Gun->GetHitPos());
						//エフェクト
						Effect_UseControl::Set_Effect(Effect::ef_fire, Gun->GetHitPos(), Gun->GetHitVec(), 1.f);
					}
				}
				tgtTimer = std::max(tgtTimer - 1.f / FPS, 0.f);
				if (Chara->ShotSwitch()) {
					//エフェクト
					auto mat = Gun->GetMuzzleMatrix();
					Effect_UseControl::Set_Effect(Effect::ef_fire2, mat.pos(), mat.GetRot().zvec()*-1.f, 1.f);
				}
				//視点
				{
					if (FPSActive.on() || ADSKey.press()) {
						camera_main.campos = Leap(Chara->GetEyePosition(), Gun->GetScopePos(), EyePosPer);
						camera_main.camvec = camera_main.campos + Chara->GetEyeVector();
						camera_main.camup = Chara->GetMatrix().GetRot().yvec();
					}
					else {
						MATRIX_ref CamMat = Chara->GetFrameWorldMatrix(CharaFrame::Upper).GetRot();
						VECTOR_ref CamPos = Chara->GetMatrix().pos() + VECTOR_ref::vget(0, Leap(14.f, 6.f, EyePosPer_Prone), 0);
						CamPos += Leap((CamMat.xvec()*-8.f + CamMat.yvec()*3.f), (CamMat.xvec()*-3.f + CamMat.yvec()*4.f), EyeRunPer);

						camera_main.campos = Leap(CamPos + Chara->GetEyeVector() * Leap(-20.f, 2.f, EyePosPer_Prone), Gun->GetScopePos(), EyePosPer);
						camera_main.camvec = Leap(CamPos + Chara->GetEyeVector() * 100.f, Gun->GetScopePos() + Chara->GetEyeVector(), EyePosPer);
						camera_main.camup = Chara->GetMatrix().GetRot().yvec();

						easing_set(&EyeRunPer, RunKey.press() ? 1.f : 0.f, 0.95f);
					}
					if (Chara->IsADS()) {
						easing_set(&EyePosPer, 1.f, 0.8f);
						easing_set(&EyePosPer_Prone, 0.f, 0.8f);
						easing_set(&camera_main.fov, deg2rad(17), 0.8f);
						camera_main.near_ = 10.f;
						camera_main.far_ = 12.5f * 200.f;
					}
					else {
						easing_set(&EyePosPer, 0.f, 0.8f);
						if (Chara->IsRun()) {
							easing_set(&camera_main.fov, deg2rad(85), 0.9f);
							easing_set(&camera_main.near_, 3.f, 0.9f);
							easing_set(&camera_main.far_, 12.5f * 50.f, 0.9f);
						}
						else {
							easing_set(&camera_main.fov, deg2rad(65), 0.9f);
							easing_set(&camera_main.near_, 10.f, 0.9f);
							easing_set(&camera_main.far_, 12.5f * 200.f, 0.9f);
						}
						if (Chara->IsProne()) {
							easing_set(&EyePosPer_Prone, 1.f, 0.8f);
						}
						else {
							easing_set(&EyePosPer_Prone, 0.f, 0.8f);
						}
					}
					if (Chara->ShotSwitch()) {
						camera_main.fov -= deg2rad(10);
					}
				}
				TEMPSCENE::Update();
				Effect_UseControl::Update_Effect();
				return true;
			}
			void Dispose(void) noexcept override {
				Effect_UseControl::Dispose_Effect();
				Obj.DisposeObject();
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
				Obj.DrawObject();
			}

			void Main_Draw(void) noexcept override {
				BackGround.Draw();
				Obj.DrawObject();
				//シェーダー描画用パラメーターセット
				{
					auto& Chara = (std::shared_ptr<CharacterClass>&)(Obj.GetObj(ObjType::Human, 0));
					auto& Gun = (std::shared_ptr<GunClass>&)(Obj.GetObj(ObjType::Gun, 0));

					Set_is_Blackout(true);
					Set_Per_Blackout((1.f + sin(Chara->GetHeartRateRad()*4.f)*0.25f) * ((Chara->GetHeartRate() - 60.f) / (180.f - 60.f)));
					Set_is_lens(Chara->IsADS());
					if (is_lens()) {
						VECTOR_ref LensPos = ConvWorldPosToScreenPos(Gun->GetLensPos().get());
						if (0.f < LensPos.z() && LensPos.z() < 1.f) {
							Set_xp_lens(LensPos.x());
							Set_yp_lens(LensPos.y());

							LensPos = ConvWorldPosToScreenPos(Gun->GetLensPosSize().get());
							if (0.f < LensPos.z() && LensPos.z() < 1.f) {
								Set_size_lens(std::hypotf(xp_lens() - LensPos.x(), yp_lens() - LensPos.y()));
							}
						}
						LensPos = ConvWorldPosToScreenPos(Gun->GetReticlePos().get());
						if (0.f < LensPos.z() && LensPos.z() < 1.f) {
							Reticle_xpos = LensPos.x();
							Reticle_ypos = LensPos.y();
							Reticle_on = (size_lens() > std::hypotf(xp_lens() - Reticle_xpos, yp_lens() - Reticle_ypos));
						}
					}
					else {
						Reticle_on = false;
					}
				}
			}
			void LAST_Draw(void) noexcept override {
				auto& Chara = (std::shared_ptr<CharacterClass>&)(Obj.GetObj(ObjType::Human, 0));
				auto& Gun = (std::shared_ptr<GunClass>&)(Obj.GetObj(ObjType::Gun, 0));
				//レティクル表示
				if (Reticle_on) {
					Gun->GetReticle().DrawRotaGraph((int)Reticle_xpos, (int)Reticle_ypos, size_lens() / (4096.f / 2.f)*1.25f, 0.f, true);
				}
				//的ヒット状況表示
				{
					auto* DrawParts = DXDraw::Instance();
					if (tgtSel >= 0) {
						auto& t = (std::shared_ptr<TargetClass>&)(Obj.GetObj(ObjType::Target, tgtSel));
						t->DrawHitCard(DrawParts->disp_x / 2 - y_r(300), DrawParts->disp_y / 2 + y_r(100), y_r(100), tgtTimer / 5.f);
					}
				}
				//UI表示
				UI_class.SetIntParam(0, (int)(Chara->GetHeartRate()));
				UI_class.SetfloatParam(0, 1.f + sin(Chara->GetHeartRateRad()*4.f)*0.1f);
				UI_class.Draw();
			}
		};
	};
};