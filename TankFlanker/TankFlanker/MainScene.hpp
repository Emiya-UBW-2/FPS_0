#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
		private:
			GraphHandle HeartGraph;
			FontPool UI;

			int intParam[1];
			float floatParam[3];
		public:
			void Set() {
				HeartGraph = GraphHandle::Load("data/UI/Heart.png");
			}
			void Draw() {
				auto* DrawParts = DXDraw::Instance();
				//心拍数
				{
					int siz = y_r(32);
					int xP = siz, yP = DrawParts->disp_y - siz - y_r(15);
					int xo, yo;
					HeartGraph.GetSize(&xo, &yo);
					HeartGraph.DrawRotaGraph(xP, yP, (float)(siz) / xo * floatParam[0], 0.f, true);
					UI.Get(y_r(24)).Get_handle().DrawStringFormat(xP + siz / 2, yP + siz / 2 - y_r(24), GetColor(255, 0, 0), "%03d", intParam[0]);
				}
				//スタミナ
				{
					float Xsize = (float)(y_r(200));
					int siz = y_r(12);
					int xP = siz, yP = DrawParts->disp_y - siz - y_r(12);
					DrawBox(xP, yP, xP + (int)Xsize, yP + siz, GetColor(0, 0, 0), TRUE);
					DrawBox(xP, yP, xP + (int)(Xsize * floatParam[1]), yP + siz, (floatParam[1] > 0.3f) ? GetColor(0, 255, 0) : GetColor(255, 200, 0), TRUE);
					DrawBox(xP, yP, xP + (int)Xsize, yP + siz, GetColor(128, 128, 128), FALSE);
				}
				//向き
				{
					float Xsize = (float)(y_r(200));
					int siz = y_r(12);
					int xP = siz + Xsize, yP = DrawParts->disp_y - siz - y_r(64);

					DrawLine(xP, yP, xP + (int)(Xsize*sin(floatParam[2])), yP + (int)(Xsize*-cos(floatParam[2])), GetColor(255, 0, 0), 5);
				}
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
			const int tgt_num = 6;
			int tgtSel = 0;
			float tgtTimer = 0.f;
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
			float m_TurnRatePer{ 0.f };
			//UI関連
			UIClass UI_class;
			//銃関連
			const int gun_num = 3;

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
				this->BackGround.Load();

				for (int i = 0; i < tgt_num; i++) {
					this->Obj.AddObject(ObjType::Target);
					this->Obj.LoadObj("data/model/Target/");
				}
				for (int i = 0; i < 3; i++) {
					this->Obj.AddObject(ObjType::Human);
					this->Obj.LoadObj("data/umamusume/WinningTicket/");
				}
				for (int i = 0; i < gun_num; i++) {
					this->Obj.AddObject(ObjType::Gun);
					this->Obj.LoadObj("data/gun/gun/");
					this->Obj.AddObject(ObjType::Magazine);
					this->Obj.LoadObj("data/gun/gun/", "model_mag");
				}
				//init
				this->Obj.InitObject(&this->BackGround.GetGroundCol());
				//ロード
				SetCreate3DSoundFlag(FALSE);
				Env = SoundHandle::Load("data/Sound/SE/envi.wav");
				SetCreate3DSoundFlag(FALSE);
				Env.vol(64);
				//UI
				UI_class.Set();
				//Set
				for (int i = 0; i < tgt_num; i++) {
					auto& t = this->Obj.GetObj(ObjType::Target, i);
					t->SetMove(deg2rad(90), VECTOR_ref::vget(732.f - 12.5f*100.f*(i / 3), 15.11f, -974.20f + 20 - 20.f*(i % 3)));
				}
				for (int i = 0; i < 3; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, i));
					c->SetGunPtr((std::shared_ptr<GunClass>&)(this->Obj.GetObj(ObjType::Gun, i)));
					//c->ValueSet(deg2rad(0.f), deg2rad(0.f), false, false, VECTOR_ref::vget(-230.f, 0.f, 450.f + (float)i*20.f));
					c->ValueSet(deg2rad(50.f), deg2rad(90.f), false, true, VECTOR_ref::vget(1970.f, 90.f, -973.72f + (float)(i - 1)*20.f));
				}
				for (int i = 0; i < gun_num; i++) {
					auto& m = (std::shared_ptr<GunClass>&)(this->Obj.GetObj(ObjType::Gun, i));
					m->SetMagPtr((std::shared_ptr<MagazineClass>&)(this->Obj.GetObj(ObjType::Magazine, i)));
				}
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
				auto& Chara = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, 0));//自分
				//FirstDoing
				if (IsFirstLoop) {
					SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
					Env.play(DX_PLAYTYPE_LOOP, TRUE);
					SetUseASyncLoadFlag(TRUE);
					Chara->LoadReticle();
					SetUseASyncLoadFlag(FALSE);
				}
				//Input
				{
					MouseActive.GetInput(CheckHitKey_M(KEY_INPUT_TAB) != 0);
					FPSActive.GetInput(CheckHitKey_M(KEY_INPUT_V) != 0);
					RunKey.GetInput(CheckHitKey_M(KEY_INPUT_LSHIFT) != 0);
					ADSKey.GetInput((GetMouseInput_M() & MOUSE_INPUT_RIGHT) != 0);
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
							if (m_TurnRate > -1) {
								m_TurnRate--;
							}
							else {
								m_TurnRate++;
							}
						}
						if (QKey.trigger()) {
							if (m_TurnRate < 1) {
								m_TurnRate++;
							}
							else {
								m_TurnRate--;
							}
						}
						if (!Chara->GetIsRun()) {
							m_TurnRate = 0;
						}

						m_TurnRate = std::clamp(m_TurnRate, -1, 1);
						float xadd = 0.f;
						if (Chara->GetIsSprint()) {
							xadd = 0.279f*(-m_TurnRate);//スプリント
						}
						else if (Chara->GetIsRun()) {
							xadd = 0.1845f*(-m_TurnRate);//走り
						}
						easing_set(&m_TurnRatePer, xadd, 0.9f);
					}

					float cam_per = (camera_main.fov / deg2rad(65) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
					Chara->SetInput(
						std::clamp(-(float)(my - DXDraw::Instance()->disp_y / 2)*1.f, -9.f, 9.f) * cam_per,
						std::clamp(((float)(mx - DXDraw::Instance()->disp_x / 2)*1.f + m_TurnRatePer), -9.f, 9.f) * cam_per,
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

					for (int i = 1; i < 3; i++) {
						auto& c = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, i));
						c->SetInput(
							0.f,
							0.f,
							false,
							false,
							false,
							false,
							false,
							false,
							false,//(GetRand(10)==0),
							true,
							false
						);
					}
				}
				//Execute
				this->Obj.ExecuteObject();
				//col
				for (int j = 0; j < gun_num; j++) {
					auto& Gun = (std::shared_ptr<GunClass>&)(this->Obj.GetObj(ObjType::Gun, j));
					for (int i = 0; i < tgt_num; i++) {
						auto& t = (std::shared_ptr<TargetClass>&)(this->Obj.GetObj(ObjType::Target, i));
						if (Gun->CheckBullet(t->GetCol())) {
							//エフェクト
							Effect_UseControl::Set_Effect(Effect::ef_fire, Gun->GetHitPos(), Gun->GetHitVec(), 1.f);
							//ヒット演算
							if (j == 0) {
								if (tgtSel != -1 && tgtSel != i) {
									auto& tOLD = (std::shared_ptr<TargetClass>&)(this->Obj.GetObj(ObjType::Target, tgtSel));
									tOLD->ResetHit();
								}
								tgtSel = i;
								tgtTimer = 5.f;
								t->SetHitPos(Gun->GetHitPos());
							}
						}
					}
					if (Gun->CheckBullet(&this->BackGround.GetGroundCol())) {
						//エフェクト
						Effect_UseControl::Set_Effect(Effect::ef_fire, Gun->GetHitPos(), Gun->GetHitVec(), 1.f);
					}
				}
				tgtTimer = std::max(tgtTimer - 1.f / FPS, 0.f);
				for (int j = 0; j < gun_num; j++) {
					auto& Gun = (std::shared_ptr<GunClass>&)(this->Obj.GetObj(ObjType::Gun, j));
					if (Gun->GetIsShot()) {
						//エフェクト
						auto mat = Gun->GetMuzzleMatrix();
						Effect_UseControl::Set_Effect(Effect::ef_fire2, mat.pos(), mat.GetRot().zvec()*-1.f, 1.f);
					}
				}
				//視点
				{
					if (FPSActive.on() || ADSKey.press()) {
						camera_main.campos = Leap(Chara->GetEyePosition(), Chara->GetScopePos(), EyePosPer);
						camera_main.camvec = camera_main.campos + Chara->GetEyeVector();
						camera_main.camup = Chara->GetMatrix().GetRot().yvec();
					}
					else {
						MATRIX_ref UpperMat = Chara->GetFrameWorldMatrix(CharaFrame::Upper).GetRot();
						VECTOR_ref CamPos = Chara->GetMatrix().pos() + Chara->GetMatrix().yvec() * Leap(14.f, 6.f, EyePosPer_Prone);
						CamPos += Leap((UpperMat.xvec()*-8.f + UpperMat.yvec()*3.f), (UpperMat.xvec()*-3.f + UpperMat.yvec()*4.f), EyeRunPer);
						camera_main.campos = Leap(CamPos + Chara->GetEyeVector() * Leap(-20.f, 2.f, EyePosPer_Prone), Chara->GetScopePos(), EyePosPer);
						camera_main.camvec = Leap(CamPos, Chara->GetScopePos(), EyePosPer) + Chara->GetEyeVector() * 100.f;
						camera_main.camup = Chara->GetMatrix().GetRot().yvec();
					}
					easing_set(&EyeRunPer, Chara->GetIsRun() ? 1.f : 0.f, 0.95f);
					easing_set(&EyePosPer, Chara->GetIsADS() ? 1.f : 0.f, 0.8f);
					easing_set(&EyePosPer_Prone, Chara->GetIsProne() ? 1.f : 0.f, 0.8f);
					if (Chara->GetIsADS()) {
						easing_set(&camera_main.fov, deg2rad(17), 0.8f);
						easing_set(&camera_main.near_, 10.f, 0.9f);
						easing_set(&camera_main.far_, 12.5f * 300.f, 0.9f);
					}
					else if (Chara->GetIsRun()) {
						easing_set(&camera_main.fov, deg2rad(90), 0.9f);
						easing_set(&camera_main.near_, 3.f, 0.9f);
						easing_set(&camera_main.far_, 12.5f * 150.f, 0.9f);
					}
					else {
						easing_set(&camera_main.fov, deg2rad(75), 0.9f);
						easing_set(&camera_main.near_, 10.f, 0.9f);
						easing_set(&camera_main.far_, 12.5f * 300.f, 0.9f);
					}
					if (Chara->GetShotSwitch()) {
						camera_main.fov -= deg2rad(10);
					}
				}
				//UIパラメーター
				{
					UI_class.SetIntParam(0, (int)(Chara->GetHeartRate()));
					UI_class.SetfloatParam(0, 1.f + sin(Chara->GetHeartRateRad()*4.f)*0.1f);
					UI_class.SetfloatParam(1, Chara->GetStamina() / Chara->GetStaminaMax());
					UI_class.SetfloatParam(2, m_TurnRatePer);
				}
				TEMPSCENE::Update();
				Effect_UseControl::Update_Effect();
				return true;
			}
			void Dispose(void) noexcept override {
				Effect_UseControl::Dispose_Effect();
				this->Obj.DisposeObject();
			}
			//
			void BG_Draw(void) noexcept override {
				this->BackGround.BG_Draw();
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				this->BackGround.Shadow_Draw_NearFar();
			}
			void Shadow_Draw(void) noexcept override {
				this->BackGround.Shadow_Draw();
				this->Obj.DrawObject();
			}

			void Main_Draw(void) noexcept override {
				this->BackGround.Draw();
				this->Obj.DrawObject();
				//シェーダー描画用パラメーターセット
				{
					auto& Chara = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, 0));
					//
					Set_is_Blackout(true);
					Set_Per_Blackout((1.f + sin(Chara->GetHeartRateRad()*4.f)*0.25f) * ((Chara->GetHeartRate() - 60.f) / (180.f - 60.f)));
					//
					Set_is_lens(Chara->GetIsADS());
					if (is_lens()) {
						VECTOR_ref LensPos = ConvWorldPosToScreenPos(Chara->GetLensPos().get());
						if (0.f < LensPos.z() && LensPos.z() < 1.f) {
							Set_xp_lens(LensPos.x());
							Set_yp_lens(LensPos.y());
							LensPos = ConvWorldPosToScreenPos(Chara->GetLensPosSize().get());
							if (0.f < LensPos.z() && LensPos.z() < 1.f) {
								Set_size_lens(std::hypotf(xp_lens() - LensPos.x(), yp_lens() - LensPos.y()));
							}
						}
						LensPos = ConvWorldPosToScreenPos(Chara->GetReticlePos().get());
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
				auto* DrawParts = DXDraw::Instance();
				auto& Chara = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, 0));
				//レティクル表示
				if (Reticle_on) {
					Chara->GetReticle().DrawRotaGraph((int)Reticle_xpos, (int)Reticle_ypos, size_lens() / (3072.f / 2.f), 0.f, true);
				}
				//的ヒット状況表示
				if (tgtSel >= 0) {
					auto& t = (std::shared_ptr<TargetClass>&)(this->Obj.GetObj(ObjType::Target, tgtSel));
					t->DrawHitCard(DrawParts->disp_x / 2 - y_r(300), DrawParts->disp_y / 2 + y_r(100), y_r(100), tgtTimer / 5.f);
				}
			}
			//UI表示
			void UI_Draw(void) noexcept  override {
				UI_class.Draw();
			}
		};
	};
};