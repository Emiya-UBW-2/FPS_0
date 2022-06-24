#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
		private:
			GraphHandle HeartGraph;
			FontPool UI;

			int intParam[3];
			float floatParam[4];
		public:
			void Set(void) noexcept {
				HeartGraph = GraphHandle::Load("data/UI/Heart.png");
			}
			void Draw(void) noexcept {
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
					float Xsize = (float)(y_r(125));
					int siz = y_r(12);
					int xP = siz + (int)Xsize, yP = DrawParts->disp_y - siz - y_r(64);
					int xpt = xP, ypt = yP;
					for (int i = 0; i < 5; i++) {
						float max = Xsize * (i + 1) / 5;
						float rad = floatParam[2] * (i + 1) / 5;
						DrawLine(
							xpt, ypt,
							xP + (int)(max*sin(rad)), yP + (int)(max*-cos(rad)),
							GetColor(255, 0, 0), 5 - i);
						xpt = xP + (int)(max*sin(rad));
						ypt = yP + (int)(max*-cos(rad));
					}
					{
						float max = Xsize;
						float rad = floatParam[2];
						xpt = xP + (int)(max*sin(rad));
						ypt = yP + (int)(max*-cos(rad));

						float yap = Xsize / 5;
						DrawLine(
							xpt - (int)(yap*sin(rad * 2.f - deg2rad(15))), ypt - (int)(yap*-cos(rad * 2.f - deg2rad(15))),
							xpt, ypt,
							GetColor(255, 0, 0), 2);
						DrawLine(
							xpt - (int)(yap*sin(rad * 2.f + deg2rad(15))), ypt - (int)(yap*-cos(rad * 2.f + deg2rad(15))),
							xpt, ypt,
							GetColor(255, 0, 0), 2);
					}

					UI.Get(y_r(24)).Get_handle().DrawStringFormat_RIGHT(xP - siz / 2, yP - y_r(14), GetColor((int)(192.f - 64.f*floatParam[2] * 2.f), 0, 0), "Q");
					UI.Get(y_r(24)).Get_handle().DrawStringFormat(xP + siz / 2, yP - y_r(14), GetColor((int)(192.f + 64.f*floatParam[2] * 2.f), 0, 0), "E");
				}
				//弾数
				{
					int xP = DrawParts->disp_x - y_r(24), yP = DrawParts->disp_y - y_r(32);

					UI.Get(y_r(32)).Get_handle().DrawStringFormat_RIGHT(xP - y_r(48), yP - y_r(18), GetColor(255, 255, 255), "%02d", intParam[1]);
					UI.Get(y_r(24)).Get_handle().DrawStringFormat_RIGHT(xP - y_r(30), yP - y_r(6), GetColor(192, 192, 192), "/");
					UI.Get(y_r(18)).Get_handle().DrawStringFormat_RIGHT(xP, yP, GetColor(192, 192, 192), "%02d", intParam[2]);
				}
				//スコア
				{
					int xP = y_r(150), yP = DrawParts->disp_y - y_r(64);
					UI.Get(y_r(32)).Get_handle().DrawStringFormat(xP, yP, GetColor(255, 255, 255), "%02d", (int)floatParam[3]);
					UI.Get(y_r(18)).Get_handle().DrawStringFormat(xP + y_r(54), yP + y_r(14), GetColor(255, 255, 255), ".%02d", (int)((floatParam[3] - (float)((int)floatParam[3]))*100.f));
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
			//
			static const int chara_num = 3;
			//関連
			static const int tgt_num = chara_num * 2;
			int tgtSel = 0;
			float tgtTimer = 0.f;
			//ルール
			float m_ReadyTime = 0.f;
			bool m_StartSwitch{ false };
			//操作関連
			float EyePosPer_Prone = 0.f;
			float EyePosPer = 0.f;
			float EyeRunPer = 0.f;
			switchs FPSActive;
			switchs MouseActive;
			switchs RunKey;
			switchs ADSKey;
			//UI関連
			UIClass UI_class;
			float scoreBuf{ 0.f };
			//銃関連
			static const int gun_num = chara_num;
			static const int cart_num = 2;

			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;

			bool InTurnOn[chara_num];
			bool InTurnOff[chara_num];
			bool InTurnSwitch[chara_num];
			bool InAimPoint[chara_num];
			bool InAimStart[chara_num];
			bool InAiming[chara_num];
			bool InShot[chara_num];
			bool CanSprint[chara_num];
			bool IsProne[chara_num];
			float AimX[chara_num];
			float AimY[chara_num];
		public:
			using TEMPSCENE::TEMPSCENE;
			void Set(void) noexcept override {
				Set_EnvLight(
					VECTOR_ref::vget(12.5f*-300.f, 12.5f*-10.f, 12.5f*-300.f),
					VECTOR_ref::vget(12.5f*300.f, 12.5f*50.f, 12.5f*300.f),
					VECTOR_ref::vget(-0.25f, -0.5f, 0.0f),
					GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				//Load
				this->BackGround.Load();

				for (int i = 0; i < tgt_num; i++) {
					this->Obj.AddObject(ObjType::Target);
					this->Obj.LoadObj("data/model/Target/");
				}
				for (int i = 0; i < chara_num; i++) {
					this->Obj.AddObject(ObjType::ShootingMat);
					this->Obj.LoadObj("data/model/ShootingMat/");
				}
				for (int i = 0; i < chara_num; i++) {
					this->Obj.AddObject(ObjType::Human);
					this->Obj.LoadObj("data/umamusume/WinningTicket/");
					InTurnOn[i] = false;
					InTurnOff[i] = false;
					InTurnSwitch[i] = false;
					InAimPoint[i] = false;
					InAimStart[i] = false;
					InAiming[i] = false;
					InShot[i] = false;
					CanSprint[i] = true;
					IsProne[i] = false;
				}
				for (int i = 0; i < gun_num; i++) {
					this->Obj.AddObject(ObjType::Gun);
					this->Obj.LoadObj("data/gun/Mosin/");
					this->Obj.AddObject(ObjType::Magazine);
					this->Obj.LoadObj("data/gun/Mosin/", "model_mag");

					auto& m = (std::shared_ptr<MagazineClass>&)(this->Obj.GetObj(ObjType::Magazine, i));
					m->GetAmmoAll();
					for (int j = 0; j < gun_num; j++) {
						this->Obj.AddObject(ObjType::Cart);
						this->Obj.LoadObj("data/gun/Mosin/", "ammo");
					}
				}

				this->Obj.AddObject(ObjType::Gate);
				this->Obj.LoadObj("data/model/map/","model_gate");


				this->Obj.AddObject(ObjType::Circle);
				this->Obj.LoadObj("data/model/Circle/");


				//init
				this->Obj.InitObject(&this->BackGround.GetGroundCol());
				//ロード
				SetCreate3DSoundFlag(FALSE);
				Env = SoundHandle::Load("data/Sound/SE/envi.wav");
				SetCreate3DSoundFlag(FALSE);
				Env.vol(64);
				//UI
				UI_class.Set();
				this->scoreBuf = 0.f;
				//
				TEMPSCENE::Set();
				//Set
				for (int i = 0; i < tgt_num; i++) {
					auto& t = this->Obj.GetObj(ObjType::Target, i);
					t->SetMove(deg2rad(-90), BackGround.ShotPos[1 + (i / chara_num)] + VECTOR_ref::vget(0, 0, -20 + 20.f*(i % chara_num)));
				}
				for (int i = 0; i < chara_num; i++) {
					auto& t = this->Obj.GetObj(ObjType::ShootingMat, i);
					t->SetMove(deg2rad(-90), BackGround.ShotPos[0] + VECTOR_ref::vget(-10.f, 0, -20 + 20.f*(i % chara_num)));
				}
				for (int i = 0; i < chara_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, i));
					c->SetGunPtr((std::shared_ptr<GunClass>&)(this->Obj.GetObj(ObjType::Gun, i)));
					c->ValueSet(deg2rad(0.f), deg2rad(-90.f), false, false, VECTOR_ref::vget(0.f, 0.f, -52.5f + (float)(i - 1)*20.f));
				}
				for (int i = 0; i < gun_num; i++) {
					auto& g = (std::shared_ptr<GunClass>&)(this->Obj.GetObj(ObjType::Gun, i));
					auto& m = (std::shared_ptr<MagazineClass>&)(this->Obj.GetObj(ObjType::Magazine, i));
					g->SetMagPtr(m);
					for (int j = 0; j < cart_num; j++) {
						m->SetCartPtr((std::shared_ptr<CartClass>&)(this->Obj.GetObj(ObjType::Cart, i*cart_num + j)));
					}
				}
				{
					auto& t = this->Obj.GetObj(ObjType::Circle, 0);
					t->SetMove(deg2rad(-90), BackGround.ShotPos[0] + VECTOR_ref::vget(-10.f, 0, -20));
				}
				{
					auto& t = this->Obj.GetObj(ObjType::Gate, 0);
					t->SetMove(deg2rad(-90), VECTOR_ref::vget(0.f, 0.f,90.f));
				}
				tgtSel = -1;
				tgtTimer = 0.f;
				//Cam
				camera_main.set_cam_info(deg2rad(65), 1.f, 100.f);
				camera_main.set_cam_pos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
				Set_zoom_lens(3.5f);
				//
				this->m_ReadyTime = 15.f;
				this->m_StartSwitch = false;
				//入力
				FPSActive.Init(false);
				MouseActive.Init(false);
			}
			//
			bool Update(void) noexcept override {
				auto& Chara = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, 0));//自分
				//FirstDoing
				if (IsFirstLoop) {
					SetMousePoint(DXDraw::Instance()->disp_x / 2, DXDraw::Instance()->disp_y / 2);
					Env.play(DX_PLAYTYPE_LOOP, TRUE);
					Chara->LoadReticle();

					this->m_ReadyTime = 10.f;
				}
				//
				{
					auto prev = this->m_ReadyTime;
					this->m_ReadyTime -= 1.f / FPS;
					this->m_StartSwitch = (prev >= 0.f && this->m_ReadyTime < 0.f);
					printfDx("%f \n", this->m_ReadyTime);
				}
				//Input
				{
					MouseActive.GetInput(CheckHitKey_M(KEY_INPUT_TAB) != 0 || this->m_StartSwitch);
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

					//float cam_per = (camera_main.fov / deg2rad(65) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
					/*
					Chara->SetInput(
						std::clamp(-(float)(my - DXDraw::Instance()->disp_y / 2)*1.f, -9.f, 9.f) * cam_per,
						std::clamp((float)(mx - DXDraw::Instance()->disp_x / 2)*1.f, -9.f, 9.f) * cam_per,
						CheckHitKey_M(KEY_INPUT_W) != 0,
						CheckHitKey_M(KEY_INPUT_S) != 0,
						CheckHitKey_M(KEY_INPUT_A) != 0,
						CheckHitKey_M(KEY_INPUT_D) != 0,
						CheckHitKey_M(KEY_INPUT_C) != 0,
						CheckHitKey_M(KEY_INPUT_X) != 0,
						(GetMouseInput_M() & MOUSE_INPUT_LEFT) != 0,
						ADSKey.press(),
						RunKey.press(),
						this->m_ReadyTime < 0.f,
						CheckHitKey_M(KEY_INPUT_Q) != 0,
						CheckHitKey_M(KEY_INPUT_E) != 0
					);
					//*/
					for (int i = 0; i < chara_num; i++) {
						auto& c = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, i));
						InTurnSwitch[i] = false;
						{
							bool pp = false;
							for (auto& t : BackGround.TurnOn) {
								auto p = (t - c->GetMatrix().pos());
								p.y(0.f);
								if (p.size() <= 12.5f*10.f) {
									if (!InTurnOn[i]) {
										InTurnSwitch[i] = true;
									}
									InTurnOn[i] = true;
									pp = true;
								}
							}
							if (pp) {
								InTurnOn[i] = false;
							}
						}
						{
							bool pp = false;
							for (auto& t : BackGround.TurnOff) {
								auto p = (t - c->GetMatrix().pos());
								p.y(0.f);
								if (p.size() <= 12.5f*10.f) {
									if (!InTurnOff[i]) {
										InTurnSwitch[i] = true;
									}
									InTurnOff[i] = true;
									pp = true;
								}
							}
							if (pp) {
								InTurnOff[i] = false;
							}
						}
						//InAimPoint[i] = false;
						CanSprint[i] = true;
						IsProne[i] = false;
						//InAiming[i] = false;
						InShot[i] = false;
						auto t1 = BackGround.ShotPos[0] + VECTOR_ref::vget(0, 0, -20 + 20.f*(i % chara_num));
						if (!InAimStart[i]) {
							auto p = (t1 - c->GetMatrix().pos());
							p.y(0.f);
							if (p.size() <= 12.5f*2.f) {
								InAimPoint[i] = true;
							}
							if (p.size() <= 12.5f*10.f) {
								CanSprint[i] = false;
							}
							if (p.size() <= 12.5f*20.f) {

								auto q = c->GetCharaDir().zvec()*-1.f;
								q.y(0.f);
								if (InAimPoint[i]) {
									auto t2 = BackGround.ShotPos[1] + VECTOR_ref::vget(0, 0, -20 + 20.f*(i % chara_num));
									p = (t2 - t1).Norm();
									p.y(0.f);
								}


								float vecsin = -q.cross(p.Norm()).y();

								easing_set(&AimY[i], -0.06f*vecsin, 0.9f);

								AimX[i] = 0.f;

								if (InAimPoint[i] && (!InAimStart[i] && abs(vecsin)<=0.01f)) {
									InAimStart[i] = true;
									IsProne[i] = true;
								}
							}
						}
						else {
							CanSprint[i] = false;

							auto q = c->GetEyeVector().Norm();
							auto yq = q.y();
							q.y(0.f);
							auto XZq = q.size();

							bool aim = false;

							auto& t = (std::shared_ptr<TargetClass>&)(this->Obj.GetObj(ObjType::Target, i));
							auto t2 = t->GetCenterPos();
							auto p = (t2 - t1).Norm();
							auto yp = p.y();
							p.y(0.f);
							auto XZp = p.size();
							//
							float vecsin = -q.cross(p.Norm()).y();
							if (vecsin > 0.1f) {
								easing_set(&AimY[i], -0.06f*std::clamp(vecsin, 0.5f, 1.0f), 0.9f);
							}
							else if (vecsin < -0.1f) {
								easing_set(&AimY[i], -0.06f*std::clamp(vecsin, -1.0f, -0.5f), 0.9f);
							}
							else {
								easing_set(&AimY[i], -0.06f*10.f*vecsin, 0.9f);

								aim = true;
							}
							bool aim2 = (abs(vecsin) < 0.003f);
							//
							vecsin = -(XZq*yp - XZp * yq);
							if (vecsin > 0.1f) {
								easing_set(&AimX[i], -0.06f*std::clamp(vecsin, 0.5f, 1.0f), 0.9f);
								aim = false;
							}
							else if (vecsin < -0.1f) {
								easing_set(&AimX[i], -0.06f*std::clamp(vecsin, -1.0f, -0.5f), 0.9f);
								aim = false;
							}
							else {
								easing_set(&AimX[i], -0.06f*10.f*vecsin, 0.9f);

								if (aim) {
									InAiming[i] = true;
								}
							}
							aim2 &= (abs(vecsin) < 0.003f);
							//
							if (aim2) {
								InShot[i] = (Chara->GetReadyPer() > 0.95f) ? (GetRand(50) == 0) : false;
							}
							else {
								InShot[i] = false;
							}
						}

						c->SetInput(
							AimX[i],
							AimY[i],
							!CanSprint[i] && !InAimPoint[i],
							false,
							false,
							false,
							false,
							IsProne[i],
							InShot[i],
							InAiming[i],
							CanSprint[i],
							this->m_ReadyTime < 0.f,
							false,
							InTurnSwitch[i]
						);
					}
				}

				{
					auto& t = (std::shared_ptr<GateClass>&)(this->Obj.GetObj(ObjType::Gate, 0));
					if (this->m_StartSwitch) {
						t->SetStart();
					}
				}

				//Execute
				this->Obj.ExecuteObject();

				//col
				for (int j = 0; j < chara_num; j++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, j));
					if (c->GetGunPtr() != nullptr) {
						auto& Gun = c->GetGunPtr();
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
								}
								c->AddScore(t->SetHitPos(Gun->GetHitPos()));
							}
						}
						if (Gun->CheckBullet(&this->BackGround.GetGroundCol())) {
							//エフェクト
							Effect_UseControl::Set_Effect(Effect::ef_fire, Gun->GetHitPos(), Gun->GetHitVec(), 1.f);
						}
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
					easing_set(&EyePosPer, Chara->GetIsADS() ? 1.f : 0.f, 0.8f);//

					//EyePosPer = 0.f;

					easing_set(&EyePosPer_Prone, Chara->GetIsProne() ? 1.f : 0.f, 0.8f);
					if (Chara->GetIsADS()) {
						//easing_set(&camera_main.fov, deg2rad(90), 0.9f);
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
					UI_class.SetIntParam(1, (int)(Chara->GetAmmoNum()));
					UI_class.SetIntParam(2, (int)(Chara->GetAmmoAll()));

					UI_class.SetfloatParam(0, 1.f + sin(Chara->GetHeartRateRad()*4.f)*0.1f);
					UI_class.SetfloatParam(1, Chara->GetStamina() / Chara->GetStaminaMax());
					UI_class.SetfloatParam(2, Chara->GetTurnRatePer());
					UI_class.SetfloatParam(3, this->scoreBuf);

					this->scoreBuf += std::clamp((Chara->GetScore() - this->scoreBuf)*100.f, -5.f, 5.f) / FPS;

					

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
			void Depth_Draw(void) noexcept override {
				this->BackGround.Draw();
				//this->Obj.DrawDepthObject();
			}
			void BG_Draw(void) noexcept override {
				this->BackGround.BG_Draw();
			}
			void Shadow_Draw_NearFar(void) noexcept override {
				this->BackGround.Shadow_Draw_NearFar();
				//this->Obj.DrawObject_Shadow();
			}
			void Shadow_Draw(void) noexcept override {
				this->BackGround.Shadow_Draw();
				this->Obj.DrawObject_Shadow();
			}

			void Main_Draw(void) noexcept override {
				this->BackGround.Draw();
				this->Obj.DrawObject();
				//this->Obj.DrawDepthObject();
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
				for (auto& t : BackGround.TurnOff) {
					DrawCone3D(t.get(), (t + VECTOR_ref::up()*12.5f*10.f).get(), 12.5f*10.f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
				}
				for (auto& t : BackGround.TurnOn) {
					DrawCone3D(t.get(), (t + VECTOR_ref::up()*12.5f*10.f).get(), 12.5f*10.f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
				}
			}
			void Main_Draw2(void) noexcept override {
				this->Obj.DrawDepthObject();
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