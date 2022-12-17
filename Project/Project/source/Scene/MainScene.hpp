#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//通信
		enum class SequenceEnum {
			SelMode,
			CheckPreset,
			Set_Port,
			SetIP,
			SetTick,
			Matching,
			MainGame,
		};
		class NetWorkBrowser {
			//サーバー専用
			ServerControl			m_ServerCtrl;																//
			//クライアント専用
			ClientControl			m_ClientCtrl;																//
			//共通
			bool					m_IsClient{ true };															//
			SequenceEnum			m_Sequence{ SequenceEnum::SelMode };										//
			bool					m_SeqFirst{ false };														//
			float					m_Tick{ 1.f };																//
			NewWorkSetting			m_NewWorkSetting;															//
			int						m_NewWorkSelect{ 0 };														//
			NewSetting				m_NewSetting;																//
			double					m_ClientFrame{ 0.0 };
			float					m_Ping{ 0.f };

			switchs					m_LeftClick;
			float					m_LeftPressTimer{ 0.f };
		public:
			const auto& GetClient(void) const noexcept { return this->m_IsClient; }
			const auto& GetSequence(void) const noexcept { return this->m_Sequence; }
		public:
			const auto&		GetMyPlayerID(void) const noexcept { return (this->m_IsClient) ? this->m_ClientCtrl.GetMyPlayer().ID : this->m_ServerCtrl.GetMyPlayer().ID; }
			const auto		GetNowServerPlayerData(int ID, bool isYradReset) { return (this->m_IsClient) ? this->m_ClientCtrl.GetNowServerPlayerData(ID, isYradReset) : this->m_ServerCtrl.GetNowServerPlayerData(ID, isYradReset); }
			void			GetRecvData(int ID, double ServerFrame) {
				if ((this->m_IsClient) ? this->m_ClientCtrl.GetRecvData(ID) : this->m_ServerCtrl.GetRecvData(ID)) {
					this->m_Ping = (float)(this->m_ClientFrame - ServerFrame)*1000.f;
				}
				printfDx("ping %lf \n", this->m_Ping);
			}
		public:
			void Init(void) {
				m_NewWorkSetting.Load();
				m_NewWorkSelect = 0;
			}
			void FirstExecute(const InputControl& MyInput, const SendInfo& SendMove) {
				this->m_LeftClick.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
				if (!this->m_LeftClick.press()) {
					this->m_LeftPressTimer = 0.f;
				}

				if (this->m_IsClient) {
					m_ClientCtrl.SetMyPlayer(MyInput, SendMove.m_Pos, SendMove.m_Vec, SendMove.m_rad, this->m_ClientFrame, SendMove.m_Damage, SendMove.m_DamageSwitch);
					if ((this->m_Sequence == SequenceEnum::Matching) && m_SeqFirst) {
						m_ClientCtrl.Init(this->m_NewSetting.UsePort, this->m_Tick, this->m_NewSetting.IP);
					}
					if ((this->m_Sequence >= SequenceEnum::Matching) && this->m_ClientCtrl.Execute()) {
						this->m_Sequence = SequenceEnum::MainGame;
					}
				}
				//サーバー
				else {
					m_ServerCtrl.SetMyPlayer(MyInput, SendMove.m_Pos, SendMove.m_Vec, SendMove.m_rad, this->m_ClientFrame, SendMove.m_Damage, SendMove.m_DamageSwitch);
					if ((this->m_Sequence == SequenceEnum::Matching) && m_SeqFirst) {
						m_ServerCtrl.Init(this->m_NewSetting.UsePort, this->m_Tick, IPDATA());
					}
					if ((this->m_Sequence >= SequenceEnum::Matching) && this->m_ServerCtrl.Execute()) {
						this->m_Sequence = SequenceEnum::MainGame;
					}
				}
			}
			void LateExecute(void) {
				if (this->m_Sequence == SequenceEnum::MainGame) {
					this->m_ClientFrame += 1.0 / (double)FPS;
				}
				else {
					this->m_ClientFrame = 0.0;
				}
			}
			void Draw(void) {
				//auto* ObjMngr = ObjectManager::Instance();
				//auto* PlayerMngr = PlayerManager::Instance();
				//auto* DrawParts = DXDraw::Instance();
				auto* Fonts = FontPool::Instance();
				auto Red = GetColor(255, 0, 0);
				//auto Blue = GetColor(50, 50, 255);
				//auto Green = GetColor(43, 163, 91);
				auto White = GetColor(255, 255, 255);
				auto Gray = GetColor(64, 64, 64);
				auto Black = GetColor(0, 0, 0);
				unsigned int color = Red;

				int xp, yp, xs, ys;
				xp = y_r(100);
				yp = y_r(250);
				xs = y_r(500);
				ys = y_r(300);

				int y_h = y_r(30);
				//bool Mid_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0);
				//bool Right_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0);
				int mx = DXDraw::Instance()->m_DispXSize / 2, my = DXDraw::Instance()->m_DispYSize / 2;
				GetMousePoint(&mx, &my);
				//ラムダ
				auto MsgBox = [&](int xp1, int yp1, int xp2, int yp2, std::string String, auto&&... args) {
					color = Black;
					DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, String, args...);
				};
				auto ClickBox = [&](int xp1, int yp1, int xp2, int yp2, std::string String, auto&&... args) {
					bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
					color = (into) ? Black : Gray;
					DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, String, args...);
					return into && this->m_LeftClick.trigger();
				};
				auto AddSubBox = [&](int xp1, int yp1, std::function<void()> UpFunc, std::function<void()> DownFunc) {
					int xp2, yp2;
					{
						xp2 = xp1 + y_r(50);
						yp2 = yp1 + y_h;
						bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
						color = (into) ? Black : Gray;
						DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, xp1 + (xp2 - xp1) / 2, yp1, White, Black, "+");
						if (into) {
							if (this->m_LeftClick.trigger()) {
								UpFunc();
							}
							if (this->m_LeftClick.press()) {
								m_LeftPressTimer += 1.f / FPS;
								if (this->m_LeftPressTimer > 0.5f) {
									UpFunc();
								}
							}
						}
					}
					{
						yp1 += y_r(50);
						yp2 += y_r(50);
						bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
						color = (into) ? Black : Gray;
						DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, xp1 + (xp2 - xp1) / 2, yp1, White, Black, "-");
						if (into) {
							if (this->m_LeftClick.trigger()) {
								DownFunc();
							}
							if (this->m_LeftClick.press()) {
								m_LeftPressTimer += 1.f / FPS;
								if (this->m_LeftPressTimer > 0.5f) {
									DownFunc();
								}
							}
						}
					}
				};
				//
				{
					DrawBox(xp - y_r(10), yp - y_r(10), xp + xs + y_r(10), yp + ys + y_r(10), GetColor(164, 164, 164), TRUE);//背景
					MsgBox(xp, yp, xp + xs, yp + y_h, " %d/%d", (int)this->m_Sequence, (int)SequenceEnum::MainGame);
					//ログ
					{
						int xp1, yp1;
						xp1 = xp;
						yp1 = yp + ys + y_r(10) + y_r(10);
						if (this->m_Sequence > SequenceEnum::SelMode) {
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "種別[%s]", this->m_IsClient ? "クライアント" : "サーバー"); yp1 += y_h;
						}
						if (this->m_Sequence > SequenceEnum::Set_Port) {
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "使用ポート[%d-%d]", this->m_NewSetting.UsePort, this->m_NewSetting.UsePort + Player_num - 1); yp1 += y_h;
						}
						if (this->m_Sequence > SequenceEnum::SetTick) {
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "ティックレート[%4.1f]", Frame_Rate / this->m_Tick); yp1 += y_h;
						}
						if (this->m_Sequence > SequenceEnum::SetIP) {
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "IP=[%d,%d,%d,%d]", this->m_NewSetting.IP.d1, this->m_NewSetting.IP.d2, this->m_NewSetting.IP.d3, this->m_NewSetting.IP.d4); yp1 += y_h;
						}
					}
				}
				auto Prev = this->m_Sequence;
				switch (this->m_Sequence) {
				case SequenceEnum::SelMode:
					if (ClickBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "クライアントになる")) {
						this->m_IsClient = true;
						this->m_Tick = 1.f;
						this->m_Sequence = SequenceEnum::CheckPreset;
					}
					if (ClickBox(xp, yp + y_r(100), xp + xs, yp + y_r(100) + y_h, "サーバーになる")) {
						this->m_IsClient = false;
						this->m_Tick = 1.f;
						this->m_Sequence = SequenceEnum::CheckPreset;
					}
					break;
				case SequenceEnum::CheckPreset:
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "プリセット設定");
					for (int i = 0; i < this->m_NewWorkSetting.GetSize(); i++) {
						auto n = this->m_NewWorkSetting.Get(i);
						if (ClickBox(xp, yp + y_r(50)*(i + 2), xp + xs, yp + y_r(50)*(i + 2) + y_h, "[%d][%d,%d,%d,%d]", n.UsePort, n.IP.d1, n.IP.d2, n.IP.d3, n.IP.d4)) {
							this->m_NewSetting.UsePort = n.UsePort;
							this->m_NewSetting.IP = n.IP;
							this->m_Sequence = SequenceEnum::SetTick;
							m_NewWorkSelect = i;
							break;
						}
					}
					{
						int i = this->m_NewWorkSetting.GetSize();
						if (ClickBox(xp, yp + y_r(50)*(i + 2), xp + xs, yp + y_r(50)*(i + 2) + y_h, "設定を追加する")) {
							m_NewWorkSetting.Add();
							m_NewWorkSelect = i;
							this->m_Sequence = SequenceEnum::Set_Port;
						}
					}
					break;
				case SequenceEnum::Set_Port://ポート
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "ポート=[%d-%d]", this->m_NewSetting.UsePort, this->m_NewSetting.UsePort + Player_num - 1);
					AddSubBox(xp, yp + y_r(100), [&]() { this->m_NewSetting.UsePort++; }, [&]() { this->m_NewSetting.UsePort--; });
					if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
						this->m_Sequence = SequenceEnum::SetIP;//サーバ-は一応いらない
					}
					break;
				case SequenceEnum::SetIP://IP
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "IP=[%d,%d,%d,%d]", this->m_NewSetting.IP.d1, this->m_NewSetting.IP.d2, this->m_NewSetting.IP.d3, this->m_NewSetting.IP.d4);
					for (int i = 0; i < 4; i++) {
						auto* ip_tmp = &this->m_NewSetting.IP.d1;
						switch (i) {
						case 0:ip_tmp = &this->m_NewSetting.IP.d1; break;
						case 1:ip_tmp = &this->m_NewSetting.IP.d2; break;
						case 2:ip_tmp = &this->m_NewSetting.IP.d3; break;
						case 3:ip_tmp = &this->m_NewSetting.IP.d4; break;
						}
						AddSubBox(y_r(100 + 70 * i), yp + y_r(100),
							[&]() {
							if (*ip_tmp == 255) { *ip_tmp = 0; }
							else { (*ip_tmp)++; }
						}, [&]() {
							if (*ip_tmp == 0) { *ip_tmp = 255; }
							else { (*ip_tmp)--; }
						});
					}
					if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
						this->m_Sequence = SequenceEnum::SetTick;
						m_NewWorkSetting.Set(this->m_NewWorkSelect, this->m_NewSetting);
						m_NewWorkSetting.Save();
					}
					break;
				case SequenceEnum::SetTick:
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "ティック=[%4.1f]", Frame_Rate / this->m_Tick);
					AddSubBox(xp, yp + y_r(100), [&]() { this->m_Tick = std::clamp(this->m_Tick - 1.f, 1.f, 20.f); }, [&]() { this->m_Tick = std::clamp(this->m_Tick + 1.f, 1.f, 20.f); });
					if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
						this->m_Sequence = SequenceEnum::Matching;
					}
					break;
				case SequenceEnum::Matching:
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "他プレイヤー待機中…");
					for (int i = 0; i < Player_num; i++) {
						bool isActive = (((this->m_IsClient) ? this->m_ClientCtrl.GetServerDataCommon() : this->m_ServerCtrl.GetServerData()).PlayerData[i].IsActive == 1);
						int yp1 = yp + y_r(50) + y_r(35)*(i + 1);
						color = isActive ? Black : Gray;
						DrawBox(y_r(200), yp1, y_r(200) + y_r(300), yp1 + y_h, color, TRUE);
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, y_r(200), yp1, White, Black, "Player");
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, y_r(200) + y_r(300), yp1, White, Black, (isActive ? "〇" : ""));
					}
					break;
				case SequenceEnum::MainGame:
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "通信中!");
					break;
				default:
					break;
				}
				m_SeqFirst = (Prev != this->m_Sequence);
			}
			void Dispose(void) {
				m_ServerCtrl.Dispose();
				m_ClientCtrl.Dispose();
			}
		};


		class MAINLOOP : public TEMPSCENE, public EffectControl {
		private:
			static const int		Chara_num = 0;
			static const int		Vehicle_num = Player_num;
			static const int		gun_num = Chara_num;
		private:
			//リソース関連
			std::shared_ptr<BackGroundClass>			m_BackGround;				//BG
			SoundHandle				m_Env;
			MV1						hit_pic;					//弾痕  
			//人
			std::vector<std::shared_ptr<CharacterClass>> character_Pool;	//ポインター別持ち
			//戦車データ
			std::vector<VhehicleData>	vehc_data;
			std::vector<std::shared_ptr<VehicleClass>> vehicle_Pool;	//ポインター別持ち
			//操作関連
			float					m_EyePosPer_Prone = 0.f;
			float					m_EyePosPer = 0.f;
			float					m_EyeRunPer{ 0.f };
			switchs					m_FPSActive;
			switchs					m_MouseActive;
			int						m_LookMode{ 0 };
			//UI関連
			UIClass					m_UIclass;
			float					m_HPBuf{ 0.f };
			float					m_ScoreBuf{ 0.f };
			GraphHandle				hit_Graph;
			GraphHandle				aim_Graph;
			GraphHandle				scope_Graph;
			//
			float					m_CamShake{ 0.f };
			VECTOR_ref				m_CamShake1;
			VECTOR_ref				m_CamShake2;
			//銃関連
			bool Reticle_on = false;
			float Reticle_xpos = 0;
			float Reticle_ypos = 0;
			//
			MATRIX_ref				m_FreeLookMat;
			float					m_TPS_Xrad{ 0.f };
			float					m_TPS_Yrad{ 0.f };
			float					m_TPS_XradR{ 0.f };
			float					m_TPS_YradR{ 0.f };
			float					m_TPS_Per{ 1.f };
			//
			float					m_Rader{ 1.f };
			float					m_Rader_r{ 1.f };
			//
			std::vector<DamageEvent>	m_DamageEvents;

			NetWorkBrowser			m_NetWorkBrowser;
			//共通
			double					m_ClientFrame{ 0.0 };

			float					fov_base{ deg2rad(45.f) };
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return m_NetWorkBrowser.GetMyPlayerID(); }
		public:
			MAINLOOP(void) { }

			void			Load_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				//Load
				//BG
				this->m_BackGround = std::make_shared<BackGroundClass>();
				this->m_BackGround->Init();
				ObjMngr->Init(this->m_BackGround);
				//
				{
					int i = 0;
					auto data_t = GetFileNamesInDirectory("data/tank/");
					for (auto& d : data_t) {
						if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
							this->vehc_data.resize(this->vehc_data.size() + 1);
						}
					}
					for (auto& d : data_t) {
						if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
							this->vehc_data[i].Set_Pre(d.cFileName);
							i++;
						}
					}
				}
				this->m_Env = SoundHandle::Load("data/Sound/SE/envi.wav");

				MV1::Load("data/model/hit/model.mv1", &this->hit_pic);						//弾痕モデル

				hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
				aim_Graph = GraphHandle::Load("data/UI/battle_aim.bmp");
				scope_Graph = GraphHandle::Load("data/UI/battle_scope.png");

			}
			void			Set_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				SetAmbientShadow(
					VECTOR_ref::vget(Scale_Rate*-300.f, Scale_Rate*-10.f, Scale_Rate*-300.f),
					VECTOR_ref::vget(Scale_Rate*300.f, Scale_Rate*50.f, Scale_Rate*300.f),
					VECTOR_ref::vget(-0.8f, -0.5f, -0.1f),
					GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				for (auto& t : this->vehc_data) { t.Set(); }										//戦車2
				//
				for (int i = 0; i < Vehicle_num; i++) {
					vehicle_Pool.emplace_back((std::shared_ptr<VehicleClass>&)(*ObjMngr->AddObject(ObjType::Vehicle)));
				}
				this->m_Env.vol(64);
				//
				//UI
				this->m_UIclass.Set();
				//Set
				//人
				for (auto& c : this->character_Pool) {
					size_t index = &c - &this->character_Pool.front();
					VECTOR_ref pos_t = VECTOR_ref::vget(0.f + (float)(index)*20.f, 0.f, 0.f);
					auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -125.f, pos_t + VECTOR_ref::up() * 125.f);
					if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }
					c->ValueSet(deg2rad(0.f), deg2rad(-90.f), false, pos_t, (PlayerID)index);
					c->SetGunPtr(
						(std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, (int)(index * 2 + 0))),
						(std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, (int)(index * 2 + 1)))
					);
					if (index == 0) {
						//c->SetUseRealTimePhysics(true);
						c->SetUseRealTimePhysics(false);
						c->SetCharaType(CharaTypeID::Team);
					}
					else {
						c->SetUseRealTimePhysics(false);
						c->SetCharaType(CharaTypeID::Enemy);
					}
				}
				//登録
				for (auto& v : this->vehicle_Pool) {
					size_t index = &v - &this->vehicle_Pool.front();
					VECTOR_ref pos_t = VECTOR_ref::vget(0.f + (float)(index)*10.f*Scale_Rate, 0.f, 0.f);
					auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(pos_t + VECTOR_ref::up() * -1250.f, pos_t + VECTOR_ref::up() * 1250.f);
					if (HitResult.HitFlag == TRUE) { pos_t = HitResult.HitPosition; }
					v->ValueInit(&vehc_data[index!=0 ? 1 : 0], hit_pic, this->m_BackGround->GetBox2Dworld(), (PlayerID)index);
					v->ValueSet(deg2rad(0), deg2rad(90), pos_t);
				}
				//player
				PlayerMngr->Init(Player_num);
				for (int i = 0; i < Player_num; i++) {
					//PlayerMngr->GetPlayer(i).SetChara((std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i)));
					PlayerMngr->GetPlayer(i).SetChara(nullptr);
					PlayerMngr->GetPlayer(i).SetVehicle((std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i)));
					//PlayerMngr->GetPlayer(i).SetVehicle(nullptr);
				}
				if (!PlayerMngr->GetPlayer(0).IsRide()) {
					this->m_HPBuf = (float)PlayerMngr->GetPlayer(0).GetChara()->GetHP();
					this->m_ScoreBuf = PlayerMngr->GetPlayer(0).GetScore();
				}
				else {
					this->m_HPBuf = (float)PlayerMngr->GetPlayer(0).GetVehicle()->GetHP();
					this->m_ScoreBuf = PlayerMngr->GetPlayer(0).GetScore();
				}

				//Cam
				SetMainCamera().SetCamInfo(deg2rad(OPTION::Instance()->Get_Fov()), 1.f, 100.f);
				SetMainCamera().SetCamPos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
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
				//SE->Add((int)SoundEnum::GateOpen, 1, "data/Sound/SE/GateOpen.wav");
				for (int i = 0; i < 9; i++) {
					SE->Add((int)SoundEnum::Tank_Shot, 3, "data/Sound/SE/gun/fire/" + std::to_string(i) + ".wav");
				}
				for (int i = 0; i < 17; i++) {
					SE->Add((int)SoundEnum::Tank_Ricochet, 3, "data/Sound/SE/ricochet/" + std::to_string(i) + ".wav");
				}
				for (int i = 0; i < 2; i++) {
					SE->Add((int)SoundEnum::Tank_Damage, 3, "data/Sound/SE/damage/" + std::to_string(i) + ".wav");
				}
				SE->Add((int)SoundEnum::Tank_engine, 10, "data/Sound/SE/engine.wav");
				for (int i = 0; i < 7; i++) {
					SE->Add((int)SoundEnum::Tank_Eject, 3, "data/Sound/SE/gun/reload/eject/" + std::to_string(i) + ".wav", false);
				}
				for (int i = 0; i < 5; i++) {
					SE->Add((int)SoundEnum::Tank_Reload, 3, "data/Sound/SE/gun/reload/hand/" + std::to_string(i) + ".wav", false);
				}

				SE->Get((int)SoundEnum::Shot_Gun).SetVol_Local(128);
				SE->Get((int)SoundEnum::Trigger).SetVol_Local(128);
				for (int i = 0; i < 4; i++) {
					SE->Get((int)SoundEnum::Cocking0 + i).SetVol_Local(128);
				}
				SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
				SE->Get((int)SoundEnum::Heart).SetVol_Local(92);
				//SE->Get((int)SoundEnum::GateOpen).SetVol_Local(128);

				SE->Get((int)SoundEnum::Tank_Shot).SetVol(0.5f);
				SE->Get((int)SoundEnum::Tank_engine).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_Ricochet).SetVol(0.65f);
				SE->Get((int)SoundEnum::Tank_Damage).SetVol(0.65f);
				SE->Get((int)SoundEnum::Tank_Eject).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_Reload).SetVol(0.25f);


				//入力
				this->m_FPSActive.Set(true);
				this->m_MouseActive.Set(true);

				this->m_DamageEvents.clear();

				m_NetWorkBrowser.Init();
			}
			//
			bool			Update_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
#ifdef DEBUG
				//auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
				//FirstDoingv
				if (GetIsFirstLoop()) {
					SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
					this->m_Env.play(DX_PLAYTYPE_LOOP, TRUE);
					if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						Chara->LoadReticle();//プレイヤー変更時注意
					}
					fov_base = GetMainCamera().GetCamFov();
				}
				//Input,AI
				{
					float cam_per = ((GetMainCamera().GetCamFov() / deg2rad(75)) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
					float pp_x = 0.f, pp_y = 0.f;
					bool look_key = false;
					bool eyechange_key = false;
					bool Lockon_key = false;
					InputControl MyInput;

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
								pp_x = std::clamp(-(float)(-input.Rz) / 100.f*1.f, -9.f, 9.f) * cam_per;
								pp_y = std::clamp((float)(input.Z) / 100.f*1.f, -9.f, 9.f) * cam_per;

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
								auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
								MyInput.SetInput(
									pp_x*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().y(),
									pp_y*(1.f - this->m_TPS_Per) - Chara->GetRecoilRadAdd().x(),
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
						this->m_MouseActive.Execute(CheckHitKeyWithCheck(KEY_INPUT_TAB) != 0);
						int mx = DXDraw::Instance()->m_DispXSize / 2, my = DXDraw::Instance()->m_DispYSize / 2;
						if (this->m_MouseActive.on()) {
							if (this->m_MouseActive.trigger()) {
								SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
							}
							GetMousePoint(&mx, &my);
							SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
							SetMouseDispFlag(FALSE);
						}
						else {
							SetMouseDispFlag(TRUE);
						}
						pp_x = std::clamp(-(float)(my - DXDraw::Instance()->m_DispYSize / 2)*1.f, -9.f, 9.f) * cam_per;
						pp_y = std::clamp((float)(mx - DXDraw::Instance()->m_DispXSize / 2)*1.f, -9.f, 9.f) * cam_per;
						look_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0) && this->m_MouseActive.on();
						eyechange_key = CheckHitKeyWithCheck(KEY_INPUT_V) != 0;
						Lockon_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0) && this->m_MouseActive.on();

						VECTOR_ref ReCoil;
						if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
							auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
							ReCoil = Chara->GetRecoilRadAdd();
						}
						MyInput.SetInput(
							pp_x*(1.f - this->m_TPS_Per) - ReCoil.y(),
							pp_y*(1.f - this->m_TPS_Per) - ReCoil.x(),
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

					this->m_TPS_Xrad += pp_x;
					this->m_TPS_Yrad += pp_y;
					this->m_TPS_Xrad = std::clamp(this->m_TPS_Xrad, deg2rad(-60), deg2rad(60));
					if (this->m_TPS_Yrad >= deg2rad(180)) { this->m_TPS_Yrad -= deg2rad(360); }
					if (this->m_TPS_Yrad <= deg2rad(-180)) { this->m_TPS_Yrad += deg2rad(360); }

					this->m_TPS_Xrad *= this->m_TPS_Per;
					this->m_TPS_Yrad *= this->m_TPS_Per;

					Easing(&this->m_TPS_XradR, this->m_TPS_Xrad, 0.5f, EasingType::OutExpo);

					this->m_TPS_YradR += (sin(this->m_TPS_Yrad)*cos(this->m_TPS_YradR) - cos(this->m_TPS_Yrad) * sin(this->m_TPS_YradR))*20.f / FPS;
					MyInput.SetRadBuf(PlayerMngr->GetPlayer(GetMyPlayerID()).GetRadBuf());
					//ネットワーク
					m_NetWorkBrowser.FirstExecute(MyInput, PlayerMngr->GetPlayer(GetMyPlayerID()).GetNetSendMove());
					//クライアント
					if (m_NetWorkBrowser.GetClient()) {
						for (auto& c : this->character_Pool) {
							if (c->GetMyPlayerID() == GetMyPlayerID() && !PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
								c->SetUseRealTimePhysics(false);
								//c->SetUseRealTimePhysics(true);
								c->SetCharaType(CharaTypeID::Mine);
							}
							else {
								c->SetUseRealTimePhysics(false);
								c->SetCharaType(CharaTypeID::Enemy);
							}
						}
						for (auto& v : this->vehicle_Pool) {
							if (v->GetMyPlayerID() == GetMyPlayerID() && PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
								v->SetCharaType(CharaTypeID::Mine);
							}
							else {
								v->SetCharaType(CharaTypeID::Enemy);
							}
						}
					}
					//
					bool isready = true;
					for (int i = 0; i < Player_num; i++) {
						auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Human, i));
						auto& v = (std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i));
						if (m_NetWorkBrowser.GetSequence() == SequenceEnum::MainGame) {
							auto tmp = m_NetWorkBrowser.GetNowServerPlayerData(i, PlayerMngr->GetPlayer(i).IsRide());
							if (i == GetMyPlayerID()) {
								MyInput.SetKeyInput(tmp.Input.GetKeyInput());//キーフレームだけサーバーに合わせる
								if (!PlayerMngr->GetPlayer(i).IsRide()) {
									c->SetInput(MyInput, isready);
								}
								else {
									v->SetInput(MyInput, isready, false);
								}
								m_NetWorkBrowser.GetRecvData(i, tmp.Frame);
							}
							else {
								if (!PlayerMngr->GetPlayer(i).IsRide()) {
									c->SetInput(tmp.Input, isready);
								}
								else {
									v->SetInput(tmp.Input, isready, true);
								}
								bool override_true = true;
								if (!PlayerMngr->GetPlayer(i).IsRide()) {
									override_true = (tmp.CalcCheckSum() != 0);
								}
								else {
									override_true = true;
									for (auto& v2 : this->vehicle_Pool) {
										if ((v != v2) && ((v->GetMove().pos - v2->GetMove().pos).size() <= 10.f*Scale_Rate)) {
											override_true = false;
											break;
										}
									}
								}
								if (override_true) {
									if (!PlayerMngr->GetPlayer(i).IsRide()) {
										c->SetPosBufOverRide(tmp.PosBuf, tmp.VecBuf, tmp.radBuf);
									}
									else {
										v->SetPosBufOverRide(tmp.PosBuf, tmp.VecBuf, tmp.radBuf);
									}
								}

							}
							//ダメージイベント処理
							if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
								if (ObjMngr->GetObj(ObjType::Human, i) != nullptr) {
									if (tmp.DamageSwitch != c->GetDamageSwitchRec()) {
										this->m_DamageEvents.emplace_back(tmp.Damage);
										c->SetDamageSwitchRec(tmp.DamageSwitch);
									}
								}
							}
							else {
								if (ObjMngr->GetObj(ObjType::Vehicle, i) != nullptr) {
									if (tmp.DamageSwitch != v->GetDamageSwitchRec()) {
										this->m_DamageEvents.emplace_back(tmp.Damage);
										v->SetDamageSwitchRec(tmp.DamageSwitch);
									}
								}
							}
						}
						else {
							if (i == GetMyPlayerID()) {
								if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
									c->SetInput(MyInput, isready);
								}
								else {
									v->SetInput(MyInput, isready, false);
								}
							}
							//ダメージイベント処理
							if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
								if (ObjMngr->GetObj(ObjType::Human, i) != nullptr) {
									if (c->GetDamageSwitch() != c->GetDamageSwitchRec()) {
										this->m_DamageEvents.emplace_back(c->GetDamageEvent());
										c->SetDamageSwitchRec(c->GetDamageSwitch());
									}
								}
							}
							else {
								if (ObjMngr->GetObj(ObjType::Vehicle, i) != nullptr) {
									if (v->GetDamageSwitch() != v->GetDamageSwitchRec()) {
										this->m_DamageEvents.emplace_back(v->GetDamageEvent());
										v->SetDamageSwitchRec(v->GetDamageSwitch());
									}
								}
							}
						}
					}
					m_NetWorkBrowser.LateExecute();
					//ダメージイベント
					for (auto& c : this->character_Pool) {
						for (int j = 0; j < this->m_DamageEvents.size(); j++) {
							if (c->SetDamageEvent(this->m_DamageEvents[j])) {
								std::swap(this->m_DamageEvents.back(), this->m_DamageEvents[j]);
								this->m_DamageEvents.pop_back();
								j--;
							}
						}
					}
					for (auto& v : this->vehicle_Pool) {
						for (int j = 0; j < this->m_DamageEvents.size(); j++) {
							if (v->SetDamageEvent(this->m_DamageEvents[j])) {
								std::swap(this->m_DamageEvents.back(), this->m_DamageEvents[j]);
								this->m_DamageEvents.pop_back();
								j--;
							}
						}
					}
				}
				//レーザーサイト
				if (PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
					auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
					auto StartPos = Vehicle->GetGunMuzzlePos(0);
					auto EndPos = StartPos + Vehicle->GetGunMuzzleVec(0) * 500.f*Scale_Rate;
					Vehicle->GetMapColNearest(StartPos, &EndPos);
					while (true) {
						auto colres = this->m_BackGround->GetGroundCol().CollCheck_Line(StartPos, EndPos);
						if (colres.HitFlag == TRUE) {
							if (EndPos == colres.HitPosition) { break; }
							EndPos = colres.HitPosition;
						}
						else {
							break;
						}
					}
					for (auto& v : this->vehicle_Pool) {
						if (v->GetMyPlayerID() == GetMyPlayerID()) { continue; }
						if (v->RefreshCol(StartPos, EndPos, 10.f*Scale_Rate)) {
							v->GetColNearestInAllMesh(StartPos, &EndPos);
						}
						v->SetAimingDistance(-1.f);
					}
					Vehicle->SetAimingDistance((StartPos - EndPos).size());
				}
				//Execute
				ObjMngr->ExecuteObject();
				//
				for (int j = 0; j < gun_num; j++) {
					auto& Gun = (std::shared_ptr<GunClass>&)(*ObjMngr->GetObj(ObjType::Gun, j));
					if (Gun->GetIsShot()) {
						//エフェクト
						auto mat = Gun->GetMuzzleMatrix();
						EffectControl::SetOnce(EffectResource::Effect::ef_fire2, mat.pos(), mat.GetRot().zvec()*-1.f, 1.f);
					}
				}
				//いらないオブジェクトの除去
				ObjMngr->DeleteCheck();
				//弾の更新
				{
					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
							if (a->IsActive()) {
								//AmmoClass
								MV1_COLL_RESULT_POLY ColResGround = a->ColCheckGround();
								VECTOR_ref pos_tmp = a->GetMove().pos;
								VECTOR_ref norm_tmp;
								bool hitwall = this->m_BackGround->GetWallCol(a->GetMove().repos, &pos_tmp, &norm_tmp, a->GetCaliberSize());//0.00762f
								bool is_HitAll = false;
								auto& c = *ObjMngr->GetObj(ObjType::Human, a->GetShootedID());//(std::shared_ptr<CharacterClass>&)
								for (auto& tgt : this->character_Pool) {
									if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
									auto res = tgt->CheckAmmoHit(a.get(), c->GetMove().pos);
									is_HitAll |= res.first;
									if (res.second) { break; }
								}
								auto& v = *ObjMngr->GetObj(ObjType::Vehicle, a->GetShootedID());
								for (auto& tgt : this->vehicle_Pool) {
									if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
									auto res = tgt->CheckAmmoHit(a.get(), v->GetMove().pos);
									is_HitAll |= res.first;
									if (res.second) { break; }
								}
								if ((ColResGround.HitFlag == TRUE || hitwall) && !is_HitAll) {
									a->HitGround(pos_tmp);

									if (hitwall) {
										EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, pos_tmp, norm_tmp, a->GetCaliberSize() / 0.1f * Scale_Rate);
									}
									else if (ColResGround.HitFlag == TRUE) {
										EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, ColResGround.HitPosition, ColResGround.Normal, a->GetCaliberSize() / 0.1f * Scale_Rate);
									}
									/*
									if (ObjMngr->GetObj(ObjType::Vehicle, a->GetShootedID())) {
										if (ColResGround.HitFlag == TRUE) {
										}
										else if (hitwall) {
											((std::shared_ptr<VehicleClass>&)v)->HitGround(pos_tmp, (pos_tmp - a->GetMove().repos).Norm(), a->GetMove().vec);
										}
									}
									//*/
								}
							}
						}
						else {
							break;
						}
						loop++;
					}
				}
				this->m_BackGround->FirstExecute();
				ObjMngr->LateExecuteObject();
				//視点
				if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
					auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
					if (Chara->GetSendCamShake()) {
						this->m_CamShake = 1.f;
					}
					Easing(&this->m_CamShake1, VECTOR_ref::vget(GetRandf(this->m_CamShake), GetRandf(this->m_CamShake), GetRandf(this->m_CamShake)), 0.8f, EasingType::OutExpo);
					Easing(&this->m_CamShake2, this->m_CamShake1, 0.8f, EasingType::OutExpo);
					this->m_CamShake = std::max(this->m_CamShake - 1.f / FPS, 0.f);

					if (this->m_FPSActive.on() || Chara->GetIsADS()) {
						VECTOR_ref CamPos = Lerp(Chara->GetEyePosition(), Chara->GetScopePos(), this->m_EyePosPer);
						SetMainCamera().SetCamPos(CamPos, CamPos + Chara->GetEyeVector(), Chara->GetMatrix().GetRot().yvec());
					}
					else {
						MATRIX_ref FreeLook;
						FreeLook = MATRIX_ref::RotAxis(Chara->GetMatrix().xvec(), this->m_TPS_XradR) * MATRIX_ref::RotAxis(Chara->GetMatrix().yvec(), this->m_TPS_YradR);
						Easing_Matrix(&m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);

						VECTOR_ref CamVec = Lerp(Chara->GetEyeVector(), MATRIX_ref::Vtrans(Chara->GetEyeVector(), m_FreeLookMat), this->m_TPS_Per);

						MATRIX_ref UpperMat = Chara->GetFrameWorldMat(CharaFrame::Upper).GetRot();
						VECTOR_ref CamPos = Chara->GetMatrix().pos() + Chara->GetMatrix().yvec() * 14.f;
						CamPos += Lerp((UpperMat.xvec()*-8.f + UpperMat.yvec()*3.f), (UpperMat.xvec()*-3.f + UpperMat.yvec()*4.f), this->m_EyeRunPer);

						SetMainCamera().SetCamPos(
							Lerp(CamPos + CamVec * Lerp(Lerp(-20.f, -50.f, this->m_TPS_Per), 2.f, this->m_EyePosPer_Prone), Chara->GetScopePos(), this->m_EyePosPer),
							Lerp(CamPos, Chara->GetScopePos(), this->m_EyePosPer) + CamVec * 100.f,
							Chara->GetEyeVecMat().yvec() + this->m_CamShake2 * 0.25f);
					}
					Easing(&this->m_EyeRunPer, Chara->GetIsRun() ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
					Easing(&this->m_EyePosPer, Chara->GetIsADS() ? 1.f : 0.f, 0.8f, EasingType::OutExpo);//
					auto fov_t = GetMainCamera().GetCamFov();
					auto near_t = GetMainCamera().GetCamNear();
					auto far_t = GetMainCamera().GetCamFar();
					if (this->m_FPSActive.on()) {
						float fov = 0;
						if (Chara->GetIsADS()) {
							fov = deg2rad(40);
						}
						else if (Chara->GetIsRun()) {
							fov = deg2rad(100);
						}
						else {
							fov = deg2rad(95);
						}
						Easing(&near_t, Scale_Rate * 0.5f, 0.9f, EasingType::OutExpo);
						Easing(&far_t, Scale_Rate * 20.f, 0.9f, EasingType::OutExpo);

						if (Chara->GetShotSwitch()) {
							fov -= deg2rad(5);
							Easing(&fov_t, fov, 0.5f, EasingType::OutExpo);
						}
						else {
							Easing(&fov_t, fov, 0.9f, EasingType::OutExpo);
						}
					}
					else {
						if (Chara->GetIsADS()) {
							//Easing(&fov_t, deg2rad(90), 0.9f, EasingType::OutExpo);
							Easing(&fov_t, deg2rad(30), 0.9f, EasingType::OutExpo);
							Easing(&near_t, 10.f, 0.9f, EasingType::OutExpo);
							Easing(&far_t, Scale_Rate * 300.f, 0.9f, EasingType::OutExpo);
						}
						else if (Chara->GetIsRun()) {
							Easing(&fov_t, deg2rad(70), 0.9f, EasingType::OutExpo);
							Easing(&near_t, 3.f, 0.9f, EasingType::OutExpo);
							Easing(&far_t, Scale_Rate * 150.f, 0.9f, EasingType::OutExpo);
						}
						else {
							Easing(&fov_t, deg2rad(55), 0.9f, EasingType::OutExpo);
							Easing(&near_t, 10.f, 0.9f, EasingType::OutExpo);
							Easing(&far_t, Scale_Rate * 300.f, 0.9f, EasingType::OutExpo);
						}
					}
					SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
				}
				else {
					auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
					Vehicle->Setcamera(SetMainCamera(), fov_base);
					{
						MATRIX_ref FreeLook;
						FreeLook = MATRIX_ref::RotAxis((GetMainCamera().GetCamVec() - GetMainCamera().GetCamPos()).cross(GetMainCamera().GetCamUp()), this->m_TPS_XradR) * MATRIX_ref::RotAxis(GetMainCamera().GetCamUp(), this->m_TPS_YradR);
						Easing_Matrix(&this->m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);

						VECTOR_ref CamVec = (GetMainCamera().GetCamVec() - GetMainCamera().GetCamPos()).Norm();
						CamVec = Lerp(CamVec, MATRIX_ref::Vtrans(CamVec, this->m_FreeLookMat), this->m_TPS_Per);

						VECTOR_ref CamPos = Vehicle->Get_EyePos_Base();

						SetMainCamera().SetCamPos(
							Lerp(GetMainCamera().GetCamPos(), (CamPos + CamVec * -100.f), this->m_TPS_Per),
							CamPos + CamVec * 100.f,
							GetMainCamera().GetCamUp());
					}
				}
				this->m_BackGround->Execute();

				//UIパラメーター
				{
					this->m_UIclass.SetIntParam(1, (int)this->m_ScoreBuf);
					this->m_ScoreBuf += std::clamp((PlayerMngr->GetPlayer(0).GetScore() - this->m_ScoreBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(2, 1);

					if (!PlayerMngr->GetPlayer(0).IsRide()) {
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						this->m_UIclass.SetIntParam(3, (int)Chara->GetHP());
						this->m_UIclass.SetIntParam(4, (int)Chara->GetHPMax());
						this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
						this->m_HPBuf += std::clamp((Chara->GetHP() - this->m_HPBuf)*100.f, -5.f, 5.f) / FPS;
					}
					else {
						auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
						this->m_UIclass.SetIntParam(3, (int)Vehicle->GetHP());
						this->m_UIclass.SetIntParam(4, (int)Vehicle->GetHPMax());
						this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
						this->m_HPBuf += std::clamp((Vehicle->GetHP() - this->m_HPBuf)*100.f, -5.f, 5.f) / FPS;
					}

					this->m_UIclass.SetIntParam(6, (int)1.f);
					this->m_UIclass.SetIntParam(7, (int)1.f);
					this->m_UIclass.SetIntParam(8, (int)1.f);


					this->m_UIclass.SetIntParam(12, (int)0);//現在選択
					this->m_UIclass.SetIntParam(13, (int)1);//銃の総数
					if (!PlayerMngr->GetPlayer(0).IsRide()) {
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						this->m_UIclass.SetStrParam(1, Chara->GetGunPtrNow()->GetName().c_str());
						this->m_UIclass.SetIntParam(14, (int)(Chara->GetAmmoNum()));
						this->m_UIclass.SetIntParam(15, (int)(Chara->GetAmmoAll()));
						this->m_UIclass.SetItemGraph(0, &Chara->GetGunPtrNow()->GetGunPic());
					}
					if (!PlayerMngr->GetPlayer(0).IsRide()) {
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						this->m_UIclass.SetStrParam(2, Chara->GetGunPtr((Chara->GetGunPtrNowID() + 1) % Chara->GetGunPtrNum())->GetName().c_str());
						this->m_UIclass.SetItemGraph(1, &Chara->GetGunPtr((Chara->GetGunPtrNowID() + 1) % Chara->GetGunPtrNum())->GetGunPic());
					}
					/*
					if (!PlayerMngr->GetPlayer(0).IsRide()) {
						auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						this->m_UIclass.SetStrParam(3, Chara->GetGunPtrNow()->GetName().c_str());
						this->m_UIclass.SetItemGraph(2, &Chara->GetGunPtrNow()->GetGunPic());
					}
					//*/
				}
				EffectControl::Execute();
				return true;
			}
			void			Dispose_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();

				m_NetWorkBrowser.Dispose();
				EffectControl::Dispose();
				PlayerMngr->Dispose();
				ObjMngr->DisposeObject();
				for (auto& v : this->vehicle_Pool) {
					v.reset();
				}
				this->vehicle_Pool.clear();
				this->m_BackGround->Dispose();
			}
			//
			void			Depth_Draw_Sub(void) noexcept override {
				//auto* ObjMngr = ObjectManager::Instance();
				//auto* PlayerMngr = PlayerManager::Instance();

				this->m_BackGround->Draw();
				//ObjMngr->DrawDepthObject();
			}
			void			BG_Draw_Sub(void) noexcept override {
				this->m_BackGround->BG_Draw();
			}
			void			ShadowDraw_NearFar_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw_NearFar();
			}
			void			ShadowDraw_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				//this->m_BackGround->Shadow_Draw();
				ObjMngr->DrawObject_Shadow();
			}
			void			MainDraw_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				SetFogStartEnd(GetMainCamera().GetCamNear(), GetMainCamera().GetCamFar()*2.f);
				this->m_BackGround->Draw();
				ObjMngr->DrawObject();
				//ObjMngr->DrawDepthObject();
				//シェーダー描画用パラメーターセット
				if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
					auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
					//
					Set_is_Blackout(true);
					Set_Per_Blackout((1.f + sin(Chara->GetHeartRateRad()*4.f)*0.25f) * ((Chara->GetHeartRate() - 60.f) / (180.f - 60.f)));
					//
					Set_is_lens(Chara->GetIsADS() && Chara->GetReticleSize()>1.f);
					if (Chara->GetIsADS()) {
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
					Set_zoom_lens(Chara->GetReticleSize());
				}
				else {
					auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
					auto EndPos = Vehicle->GetGunMuzzlePos(0) + Vehicle->GetGunMuzzleVec(0) * Vehicle->GetAimingDistance();
					//
					VECTOR_ref LensPos = ConvWorldPosToScreenPos(EndPos.get());
					if (0.f < LensPos.z() && LensPos.z() < 1.f) {
						Reticle_xpos = LensPos.x();
						Reticle_ypos = LensPos.y();
						Reticle_on = true;
					}
				}
				for (auto& c : this->character_Pool) {
					if (c->GetMyPlayerID() == GetMyPlayerID()) { continue; }
					auto pos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
					VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
					if (0.f < campos.z() && campos.z() < 1.f) {
						c->SetCameraPosition(campos);
						c->SetCameraSize(std::max(20.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
					}
				}
				for (auto& v : this->vehicle_Pool) {
					if (v->GetMyPlayerID() == GetMyPlayerID()) { continue; }
					auto pos = v->Set_MidPos();
					VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
					if (0.f < campos.z() && campos.z() < 1.f) {
						v->SetCameraPosition(campos);
						v->SetCameraSize(std::max(80.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
					}
				}
			}
			void			MainDrawbyDepth_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				ObjMngr->DrawDepthObject();
			}
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				//auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
				//auto* Fonts = FontPool::Instance();
				auto* DrawParts = DXDraw::Instance();
				//auto Red = GetColor(255, 0, 0);
				//auto Blue = GetColor(50, 50, 255);
				//auto Green = GetColor(43, 163, 91);
				//auto White = GetColor(212, 255, 239);
				//auto Gray = GetColor(64, 64, 64);
				//auto Black = GetColor(0, 0, 0);
				//unsigned int color = Red;
				//
				{
					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
							a->Draw_Hit_UI(hit_Graph);
						}
						else {
							break;
						}
						loop++;
					}
				}
				//UI
				if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
				}
				else {
					if (Vehicle->is_ADS()) {
						scope_Graph.DrawExtendGraph(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);
					}
				}
				this->m_UIclass.Draw();
				if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
				}
				else {
					Vehicle->DrawModuleView(y_r(50+100), DrawParts->m_DispYSize - y_r(100 +100), y_r(200));
				}
				//通信設定
				if (!this->m_MouseActive.on()) {
					m_NetWorkBrowser.Draw();
				}
				//
			}
			void			DrawUI_In_Sub(void) noexcept override {
				auto* DrawParts = DXDraw::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Chara = PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				if (!PlayerMngr->GetPlayer(GetMyPlayerID()).IsRide()) {
					//レティクル表示
					if (Reticle_on) {
						int x, y;
						Chara->GetGunPtrNow()->GetReticlePic().GetSize(&x, &y);
						Chara->GetGunPtrNow()->GetReticlePic().DrawRotaGraph((int)Reticle_xpos, (int)Reticle_ypos, size_lens() / (y / 2.f), Chara->GetReticleRad(), true);
					}
					if (!Chara->GetIsADS()) {
						aim_Graph.DrawRotaGraph(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, (float)(y_r(100)) / 100.f, 0.f, true);
					}
				}
				else{
					if (Reticle_on) {
						aim_Graph.DrawRotaGraph((int)Reticle_xpos, (int)Reticle_ypos, (float)(y_r(100)) / 100.f, 0.f, true);
					}
				}
			}
		};
	};
};
