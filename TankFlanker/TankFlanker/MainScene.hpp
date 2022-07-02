#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
		private:
			GraphHandle HeartGraph;
			GraphHandle ScoreBoard;
			FontPool UI;

			int intParam[3]{ 0 };
			float floatParam[4]{ 0 };
		public:
			void Set(void) noexcept {
				HeartGraph = GraphHandle::Load("data/UI/Heart.png");
				ScoreBoard = GraphHandle::Load("data/UI/Score.png");
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
					UI.Get(y_r(32)).Get_handle().DrawStringFormat_RIGHT(xP + y_r(54), yP, GetColor(255, 255, 255), "%02d", (int)floatParam[3]);
					UI.Get(y_r(18)).Get_handle().DrawStringFormat(xP + y_r(54), yP + y_r(14), GetColor(255, 255, 255), ".%02d", (int)((floatParam[3] - (float)((int)floatParam[3]))*100.f));
				}
			}

			void SetIntParam(int ID, int value) { intParam[ID] = value; }
			void SetfloatParam(int ID, float value) { floatParam[ID] = value; }

			auto& GetScoreBoard(void) { return ScoreBoard; }
		};

		class MAINLOOP : public TEMPSCENE, public Effect_UseControl {
		private:
			//リソース関連
			ObjectManager Obj;				//モデル
			BackGroundClass BackGround;		//BG
			SoundHandle Env;
			//
			static const int chara_num = 3;
			static const int tgt_num = 2;
			static const int gun_num = chara_num;
			static const int cart_num = 2;
			//関連
			int tgtSel = 0;
			float tgtTimer = 0.f;
			//ルール
			float m_ReadyTime = 0.f;
			bool m_StartSwitch{ false };
			bool m_RemoveSwitch{ false };
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
			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;

			float TPS_Xrad = 0;
			float TPS_Yrad = 0;
			float TPS_XradR = 0;
			float TPS_YradR = 0;
			float TPS_Per = 1.f;

			class AIControl {
			public:
				bool InTurnOn{ false };
				bool InTurnOff{ false };
				bool InTurnSwitch{ false };
				bool InAimPoint{ false };
				bool InAimStart{ false };
				bool InAiming{ false };
				bool InShot{ false };
				bool CanSprint{ false };
				bool IsProne{ false };
				float AimX{ 0.f };
				float AimY{ 0.f };
				int shotCounter = 5;
				bool GoNextPoint{ false };
			public:
				void Init() {
					InTurnOn = false;
					InTurnOff = false;
					InTurnSwitch = false;
					InAimPoint = false;
					InAimStart = false;
					InAiming = false;
					InShot = false;
					CanSprint = true;
					IsProne = false;
					shotCounter = 5;
					GoNextPoint = false;
				}

				void Execute() {
					this->InTurnSwitch = false;
					//this->InAimPoint = false;
					this->CanSprint = true;
					this->IsProne = false;
					//this->InAiming = false;
					this->InShot = false;
				}
			};

			AIControl m_AI[chara_num];
		public:
			using TEMPSCENE::TEMPSCENE;
			void Set(void) noexcept override {
				SoundPool::Create();
				Set_EnvLight(
					VECTOR_ref::vget(12.5f*-300.f, 12.5f*-10.f, 12.5f*-300.f),
					VECTOR_ref::vget(12.5f*300.f, 12.5f*50.f, 12.5f*300.f),
					VECTOR_ref::vget(-0.25f, -0.5f, 0.0f),
					GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				//Load
				//BG
				this->BackGround.Load();
				for (int i = 0; i < chara_num; i++) {
					for (int j = 0; j < tgt_num; j++) {
						this->Obj.AddObject(ObjType::Target);
						this->Obj.LoadObj("data/model/Target/");
					}
					this->Obj.AddObject(ObjType::ShootingMat);
					this->Obj.LoadObj("data/model/ShootingMat/");
				}
				this->Obj.AddObject(ObjType::Gate);
				this->Obj.LoadObj("data/model/map/", "model_gate");
				//
				for (int i = 0; i < chara_num; i++) {
					this->Obj.AddObject(ObjType::Human);
					this->Obj.LoadObj("data/umamusume/WinningTicket/");
					this->m_AI[i].Init();
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
				//guide
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
				//オブジェ
				for (int i = 0; i < chara_num; i++) {
					for (int j = 0; j < tgt_num; j++) {
						auto& t = this->Obj.GetObj(ObjType::Target, i*tgt_num + j);
						t->SetMove(MATRIX_ref::RotY(deg2rad(-90)), BackGround.ShotPos[j + 1] + VECTOR_ref::vget(0, 0, -20 + 20.f*i));
					}
					auto& m = this->Obj.GetObj(ObjType::ShootingMat, i);
					m->SetMove(MATRIX_ref::RotY(deg2rad(-90)), BackGround.ShotPos[0] + VECTOR_ref::vget(-10.f, 0, -20 + 20.f*i));
				}
				{
					auto& g = this->Obj.GetObj(ObjType::Gate, 0);
					g->SetMove(MATRIX_ref::RotY(deg2rad(-90)), VECTOR_ref::vget(0.f, 0.f, 90.f));
				}
				//人
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
				//ガイドサークル
				{
					auto& c = this->Obj.GetObj(ObjType::Circle, 0);
					c->SetMove(MATRIX_ref::RotY(deg2rad(-90)), BackGround.ShotPos[0] + VECTOR_ref::vget(-10.f, 0, -20));
				}
				//UI
				tgtSel = -1;
				tgtTimer = 0.f;
				//Cam
				camera_main.set_cam_info(deg2rad(65), 1.f, 100.f);
				camera_main.set_cam_pos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
				Set_zoom_lens(3.5f);
				//サウンド
				auto SE = SoundPool::Instance();
				SE->Add((int)SoundEnum::Shot_Gun, 3, "data/Sound/SE/gun/shot.wav");
				SE->Add((int)SoundEnum::Trigger, 1, "data/Sound/SE/gun/trigger.wav");
				for (int i = 0; i < 4; i++) {
					SE->Add((int)SoundEnum::Cocking0 + i, 3, "data/Sound/SE/gun/slide/bolt/" + std::to_string(i) + ".wav");
				}
				SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav");
				SE->Add((int)SoundEnum::SlideFoot, 9, "data/Sound/SE/move/sliding.wav");
				SE->Add((int)SoundEnum::StandupFoot, 3, "data/Sound/SE/move/standup.wav");
				SE->Add((int)SoundEnum::Heart, 9, "data/Sound/SE/move/heart.wav");
				SE->Add((int)SoundEnum::GateOpen, 1, "data/Sound/SE/GateOpen.wav");
				
				SE->Get((int)SoundEnum::Shot_Gun).SetVol_Local(128);
				SE->Get((int)SoundEnum::Trigger).SetVol_Local(128);
				for (int i = 0; i < 4; i++) {
					SE->Get((int)SoundEnum::Cocking0 + i).SetVol_Local(128);
				}
				SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
				SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
				SE->Get((int)SoundEnum::GateOpen).SetVol_Local(128);
				//
				this->m_ReadyTime = 15.f;
				this->m_StartSwitch = false;
				this->m_RemoveSwitch = false;
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
					Chara->LoadReticle();

					this->m_ReadyTime = 10.f;
				}
				//
				{
					auto prev = this->m_ReadyTime;
					this->m_ReadyTime -= 1.f / FPS;
					auto timep = 0.f;

					timep = 0.f;
					this->m_StartSwitch = (prev >= timep && this->m_ReadyTime < timep);
					timep = -10.f;
					this->m_RemoveSwitch = (prev >= timep && this->m_ReadyTime < timep);
					printfDx("%f \n", this->m_ReadyTime);
				}
				{
					auto& g = (std::shared_ptr<GateClass>&)(this->Obj.GetObj(ObjType::Gate, 0));
					if (this->m_StartSwitch) {
						g->SetStart();
					}
					if (this->m_RemoveSwitch) {
						g->SetRemove();
					}
				}
				//Input,AI
				{
					MouseActive.GetInput(CheckHitKey_M(KEY_INPUT_TAB) != 0);
					FPSActive.GetInput(CheckHitKey_M(KEY_INPUT_V) != 0);
					RunKey.GetInput(CheckHitKey_M(KEY_INPUT_LSHIFT) != 0);
					ADSKey.GetInput((GetMouseInput_M() & MOUSE_INPUT_RIGHT) != 0);
					auto MidPress = (GetMouseInput_M() & MOUSE_INPUT_MIDDLE) != 0;
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

					float cam_per = ((camera_main.fov / deg2rad(75)) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
					float pp_x = std::clamp(-(float)(my - DXDraw::Instance()->disp_y / 2)*1.f, -9.f, 9.f) * cam_per;
					float pp_y = std::clamp((float)(mx - DXDraw::Instance()->disp_x / 2)*1.f, -9.f, 9.f) * cam_per;
					
					easing_set(&TPS_Per, (!(FPSActive.on() || ADSKey.press()) &&  !Chara->GetIsProne() && MidPress) ? 1.f : 0.f, 0.9f);

					TPS_Xrad += pp_x;
					TPS_Yrad += pp_y;
					TPS_Xrad = std::clamp(TPS_Xrad, deg2rad(-40), deg2rad(40));
					if (TPS_Yrad >= deg2rad(180)) { TPS_Yrad -= deg2rad(360); }
					if (TPS_Yrad <= deg2rad(-180)) { TPS_Yrad += deg2rad(360); }

					TPS_Xrad *= TPS_Per;
					TPS_Yrad *= TPS_Per;

					easing_set(&TPS_XradR, TPS_Xrad, 0.5f);

					TPS_YradR += (sin(TPS_Yrad)*cos(TPS_YradR) - cos(TPS_Yrad) * sin(TPS_YradR))*20.f / FPS;

					Chara->SetEyeVec((camera_main.camvec - camera_main.campos).Norm());

					for (int i = 0; i < chara_num; i++) {
						if (i == 0
							//&& false
							) {
							Chara->SetInput(
								pp_x*(1.f - TPS_Per),
								pp_y*(1.f - TPS_Per),
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
								CheckHitKey_M(KEY_INPUT_E) != 0,
								CheckHitKey_M(KEY_INPUT_R) != 0
							);
							continue;
						}


						auto& c = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, i));
						this->m_AI[i].Execute();
						{
							bool pp = false;
							for (auto& t : BackGround.TurnOn) {
								auto p = (t - c->GetMatrix().pos());
								p.y(0.f);
								if (p.size() <= 12.5f*10.f) {
									if (!this->m_AI[i].InTurnOn) {
										this->m_AI[i].InTurnSwitch = true;
									}
									this->m_AI[i].InTurnOn = true;
									pp = true;
								}
							}
							if (pp) {
								this->m_AI[i].InTurnOn = false;
							}
						}
						{
							bool pp = false;
							for (auto& t : BackGround.TurnOff) {
								auto p = (t - c->GetMatrix().pos());
								p.y(0.f);
								if (p.size() <= 12.5f*10.f) {
									if (!this->m_AI[i].InTurnOff) {
										this->m_AI[i].InTurnSwitch = true;
										if (this->m_AI[i].GoNextPoint) {
											this->m_AI[i].GoNextPoint = false;

											this->m_AI[i].shotCounter = 5;
											this->m_AI[i].InAimPoint = false;
											this->m_AI[i].InAiming = false;
										}
									}
									this->m_AI[i].InTurnOff = true;
									pp = true;
								}
							}
							if (pp) {
								this->m_AI[i].InTurnOff = false;
							}
						}
						auto t1 = BackGround.ShotPos[0] + VECTOR_ref::vget(0, 0, -20 + 20.f*i);
						if (!this->m_AI[i].InAimStart) {
							auto p = (t1 - c->GetMatrix().pos());
							p.y(0.f);

							if (this->m_AI[i].shotCounter > 0) {
								if (p.size() <= 12.5f*1.5f) {
									this->m_AI[i].InAimPoint = true;
								}
								if (p.size() <= 12.5f*10.f) {
									this->m_AI[i].CanSprint = false;
								}
								if (p.size() <= 12.5f*20.f) {
									auto q = c->GetCharaDir().zvec()*-1.f;
									q.y(0.f);
									if (this->m_AI[i].InAimPoint) {
										auto t2 = BackGround.ShotPos[1] + VECTOR_ref::vget(-10, 0, -20 + 20.f*i);
										p = (t2 - t1).Norm();
										p.y(0.f);
									}
									float vecsin = -q.cross(p.Norm()).y();
									easing_set(&this->m_AI[i].AimY, -0.08f*vecsin, 0.9f);
									this->m_AI[i].AimX = 0.f;
									if (this->m_AI[i].InAimPoint && (!this->m_AI[i].InAimStart && abs(vecsin) <= 0.01f)) {
										this->m_AI[i].InAimStart = true;
										this->m_AI[i].IsProne = true;
									}
								}
							}
							else {
								auto q = c->GetCharaDir().zvec()*-1.f;
								q.y(0.f);
								auto t2 = BackGround.ShotPos[0] + VECTOR_ref::vget(0, 0, -100.f);
								p = (t2 - t1).Norm();
								p.y(0.f);
								if (!this->m_AI[i].GoNextPoint) {
									float vecsin = -q.cross(p.Norm()).y();
									easing_set(&this->m_AI[i].AimY, -0.06f*vecsin, 0.9f);
									this->m_AI[i].AimX = 0.f;
									if (abs(this->m_AI[i].AimY) <= 0.001f) {
										this->m_AI[i].AimY = 0.f;
										this->m_AI[i].GoNextPoint = true;
									}
								}
							}
						}
						else {
							this->m_AI[i].CanSprint = false;
							auto q = c->GetEyeVector().Norm();
							auto yq = q.y();
							q.y(0.f);
							auto XZq = q.size();
							bool aim = false;
							auto& t = (std::shared_ptr<TargetClass>&)(this->Obj.GetObj(ObjType::Target, i*tgt_num + 0));
							auto t2 = t->GetCenterPos();
							auto p = (t2 - t1).Norm();
							auto yp = p.y();
							p.y(0.f);
							auto XZp = p.size();
							//
							float vecsin = -q.cross(p.Norm()).y();
							if (vecsin > 0.1f) {
								easing_set(&this->m_AI[i].AimY, -0.06f*std::clamp(vecsin, 0.5f, 1.0f), 0.9f);
							}
							else if (vecsin < -0.1f) {
								easing_set(&this->m_AI[i].AimY, -0.06f*std::clamp(vecsin, -1.0f, -0.5f), 0.9f);
							}
							else {
								easing_set(&this->m_AI[i].AimY, -0.06f*10.f*vecsin, 0.9f);
								aim = true;
							}
							bool aim2 = (abs(vecsin) < 0.003f);
							//
							vecsin = -(XZq*yp - XZp * yq);
							if (vecsin > 0.1f) {
								easing_set(&this->m_AI[i].AimX, -0.06f*std::clamp(vecsin, 0.5f, 1.0f), 0.9f);
								aim = false;
							}
							else if (vecsin < -0.1f) {
								easing_set(&this->m_AI[i].AimX, -0.06f*std::clamp(vecsin, -1.0f, -0.5f), 0.9f);
								aim = false;
							}
							else {
								easing_set(&this->m_AI[i].AimX, -0.06f*10.f*vecsin, 0.9f);
								if (aim) {
									this->m_AI[i].InAiming = true;
								}
							}
							aim2 &= (abs(vecsin) < 0.003f);
							if (!c->GetCanshot()) {
								this->m_AI[i].AimX = 0.f;
								this->m_AI[i].AimY = 0.f;
							}
							this->m_AI[i].InShot = ((Chara->GetReadyPer() > 0.95f) && aim2) ? (GetRand(50) == 0) : false;
							if (this->m_AI[i].InShot) {
								this->m_AI[i].shotCounter = std::clamp(this->m_AI[i].shotCounter - 1, 0, 5);
							}

							if (this->m_AI[i].shotCounter == 0) {
								this->m_AI[i].InAimStart = false;
								this->m_AI[i].IsProne = true;
								this->m_AI[i].GoNextPoint = false;
							}
						}

						c->SetInput(
							this->m_AI[i].AimX,
							this->m_AI[i].AimY,
							!this->m_AI[i].CanSprint && !this->m_AI[i].InAimPoint,
							false,
							false,
							false,
							false,
							this->m_AI[i].IsProne,
							this->m_AI[i].InShot,
							this->m_AI[i].InAiming,
							this->m_AI[i].CanSprint,
							this->m_ReadyTime < 0.f,
							false,
							this->m_AI[i].InTurnSwitch,
							this->m_AI[i].InAimStart && c->GetIsEmpty()
						);
					}
				}
				//Execute
				this->Obj.ExecuteObject();
				//col
				for (int i = 0; i < chara_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, i));
					auto& Gun = c->GetGunPtr();
					if (Gun != nullptr) {
						for (int j = 0; j < tgt_num; j++) {
							auto s = (i*tgt_num + j);
							auto& t = (std::shared_ptr<TargetClass>&)(this->Obj.GetObj(ObjType::Target, s));
							if (Gun->CheckBullet(t->GetCol())) {
								//エフェクト
								Effect_UseControl::Set_Effect(Effect::ef_fire, Gun->GetHitPos(), Gun->GetHitVec(), 1.f);
								//ヒット演算
								if (i == 0) {
									if (tgtSel != -1 && tgtSel != s) {
										auto& tOLD = (std::shared_ptr<TargetClass>&)(this->Obj.GetObj(ObjType::Target, tgtSel));
										tOLD->ResetHit();
									}
									tgtSel = s;
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
						MATRIX_ref UpperMat = Chara->GetFrameWorldMat(CharaFrame::Upper).GetRot()*MATRIX_ref::RotY(TPS_YradR);
						VECTOR_ref CamPos = Chara->GetMatrix().pos() + Chara->GetMatrix().yvec() * Leap(14.f, 6.f, EyePosPer_Prone);

						VECTOR_ref CamVec = MATRIX_ref::Vtrans(Chara->GetEyeVector(), MATRIX_ref::RotY(TPS_YradR));

						CamVec = MATRIX_ref::Vtrans(CamVec, MATRIX_ref::RotAxis(UpperMat.xvec(), TPS_XradR));
						CamVec = Leap(Chara->GetEyeVector(), CamVec, TPS_Per);

						CamPos += Leap((UpperMat.xvec()*-8.f + UpperMat.yvec()*3.f), (UpperMat.xvec()*-3.f + UpperMat.yvec()*4.f), EyeRunPer);
						camera_main.campos = Leap(CamPos + CamVec * Leap(-20.f, 2.f, EyePosPer_Prone), Chara->GetScopePos(), EyePosPer);
						camera_main.camvec = Leap(CamPos, Chara->GetScopePos(), EyePosPer) + CamVec * 100.f;
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
				auto& Chara = (std::shared_ptr<CharacterClass>&)(this->Obj.GetObj(ObjType::Human, 0));
				//レティクル表示
				if (Reticle_on) {
					Chara->GetReticle().DrawRotaGraph((int)Reticle_xpos, (int)Reticle_ypos, size_lens() / (3072.f / 2.f), 0.f, true);
				}
			}
			//UI表示
			void UI_Draw(void) noexcept  override {
				auto* DrawParts = DXDraw::Instance();
				UI_class.Draw();
				//的ヒット状況表示
				if (tgtSel >= 0) {
					auto& t = (std::shared_ptr<TargetClass>&)(this->Obj.GetObj(ObjType::Target, tgtSel));

					int xp = DrawParts->disp_x / 2 - y_r(300);
					int yp = DrawParts->disp_y / 2 + y_r(100);
					int size = y_r(100);
					int xs = size / 2;
					int ys = size / 2;
					int xp2 = xp + ys * 2;
					int yp2 = yp + ys * 2;
					float AlphaPer = tgtTimer / 5.f;
					if (AlphaPer > 0.01f) {
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*AlphaPer));
						//背景
						UI_class.GetScoreBoard().DrawExtendGraph(xp, yp, xp2, yp2, true);
						//命中箇所
						for (auto& r : t->GetHitPosRec()) {
							float cos_t, sin_t;
							t->GetHitPoint(r, &cos_t, &sin_t);
							DrawCircle(xp + xs + (int)((float)xs * cos_t), yp + ys + (int)((float)ys * sin_t), 2, GetColor(0, 255, 0));
						}
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
						//点数
						int ypAdd = 0;
						for (auto& r : t->GetHitPosRec()) {
							DrawFormatString(xp, yp2 + ypAdd, GetColor(255, 255, 255), "[%4.1f]", t->GetHitPoint(r));
							ypAdd += 18;
						}
					}
				}
			}
		};
	};
};