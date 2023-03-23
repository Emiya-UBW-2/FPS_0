#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public EffectControl {
		private:
			static const int		Chara_num = 0;
			static const int		Vehicle_num = Player_num;
			static const int		gun_num = Chara_num;
		private:
			//リソース関連
			std::shared_ptr<BackGroundClass>			m_BackGround;		//BG
			MV1						hit_pic;								//弾痕  
			//いちいち探査しないよう別持ち
			std::vector<std::shared_ptr<VehicleClass>>	vehicle_Pool;		//ポインター別持ち
			//アイテムデータ
			std::vector<std::shared_ptr<ItemData>>		m_ItemData;		//ポインター別持ち
			//戦車データ
			std::shared_ptr<VehDataControl>				m_VehDataControl;

			std::vector<std::shared_ptr<AIControl>>		AICtrl;
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
			InventoryClass			m_InventoryClass;
			//共通
			double					m_ClientFrame{ 0.0 };

			float					fov_base{ deg2rad(45.f) };
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return m_NetWorkBrowser.GetMyPlayerID(); }
		public:
			MAINLOOP(void) noexcept {
				this->m_BackGround = std::make_shared<BackGroundClass>();
				this->m_VehDataControl = std::make_shared<VehDataControl>();
				AICtrl.resize(Player_num);
				for (int i = 0; i < Player_num; i++) {
					AICtrl[i] = std::make_shared<AIControl>();
				}
			}
			//Load
			void			Load_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				//auto* SE = SoundPool::Instance();
				//BG
				this->m_BackGround->Load();
				ObjMngr->Init(this->m_BackGround);
				//
				this->m_VehDataControl->Load();
				MV1::Load("data/model/hit/model.mv1", &this->hit_pic);

				hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
				aim_Graph = GraphHandle::Load("data/UI/battle_aim.bmp");
				scope_Graph = GraphHandle::Load("data/UI/battle_scope.png");
			}
			void			Set_Sub(void) noexcept override {
				this->m_BackGround->Init();
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto* SE = SoundPool::Instance();
				SetAmbientShadow(
					VECTOR_ref::vget(Scale_Rate*-300.f, Scale_Rate*-10.f, Scale_Rate*-300.f),
					VECTOR_ref::vget(Scale_Rate*300.f, Scale_Rate*50.f, Scale_Rate*300.f),
					VECTOR_ref::vget(-0.8f, -0.5f, -0.1f),
					GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				for (int i = 0; i < Vehicle_num; i++) {
					vehicle_Pool.emplace_back((std::shared_ptr<VehicleClass>&)(*ObjMngr->AddObject(ObjType::Vehicle)));
				}
				ObjMngr->AddObject(ObjType::HindD, "data/model/hindD/");
				{
					auto& Hind = *ObjMngr->GetObj(ObjType::HindD, 0);
					Hind->SetMove(MATRIX_ref::zero(), VECTOR_ref::vget(0.f, 30.f*Scale_Rate, 0.f));
					Hind->GetObj().get_anime(0).per = 1.f;
				}
				{
					std::string Path = "data/item/";
					auto data_t = GetFileNamesInDirectory(Path.c_str());
					for (auto& d : data_t) {
						if (d.cFileName[0] != '.') {
							m_ItemData.emplace_back(std::make_shared<ItemData>());
							m_ItemData.back()->Set(Path + d.cFileName + "/");
						}
					}
				}
				{
					std::string Path = "data/ammo/";
					auto data_t = GetFileNamesInDirectory(Path.c_str());
					for (auto& d : data_t) {
						if (d.cFileName[0] != '.') {
							m_ItemData.emplace_back(std::make_shared<AmmoData>());
							m_ItemData.back()->Set(Path + d.cFileName + "/");
						}
					}
				}
				this->m_VehDataControl->Set(m_ItemData);
				//UI
				this->m_UIclass.Set();
				//Set
				//戦車
				{
					VECTOR_ref BasePos;

					std::vector<int> OtherSelect;
					for (auto& v : this->vehicle_Pool) {
						size_t index = &v - &this->vehicle_Pool.front();
						int ID = 0;
						while (true) {
							ID = GetRand(this->m_BackGround->GetRoadPointNum() - 1);
							bool Hit = (std::find_if(OtherSelect.begin(), OtherSelect.end(), [&](int tmp) { return tmp == ID; }) != OtherSelect.end());

							if (index != 0) {
								auto LEN = (BasePos - this->m_BackGround->GetRoadPoint(ID)->pos()); LEN.y(0.f);
								if (LEN.Length() <= 100.f*Scale_Rate) {
									Hit = true;
								}
							}
							if (!Hit) {
								auto Mat = *this->m_BackGround->GetRoadPoint(ID);
								VECTOR_ref pos_t = Mat.pos();
								if (
									(-280.f*Scale_Rate / 2.f < pos_t.x() && pos_t.x() < 290.f*Scale_Rate / 2.f) &&
									(-280.f*Scale_Rate / 2.f < pos_t.z() && pos_t.z() < 280.f*Scale_Rate / 2.f)
									) {
									OtherSelect.emplace_back(ID);
									break;
								}
							}
						}

						auto Mat = *this->m_BackGround->GetRoadPoint(ID);
						VECTOR_ref pos_t = Mat.pos();
						float rad_t = std::atan2f(Mat.zvec().x(), -Mat.zvec().z());
						auto pos_t1 = pos_t + VECTOR_ref::up() * 1250.f;
						auto pos_t2 = pos_t + VECTOR_ref::up() * -1250.f;
						if (this->m_BackGround->CheckLinetoMap(pos_t1, &pos_t2, true, false)) {
							pos_t = pos_t2;
						}
						if (index == 0) {
							BasePos = pos_t;
						}

						auto& vehc_data = this->m_VehDataControl->GetVehData();
						v->ValueInit(&vehc_data[index != 0 ? GetRand((int)vehc_data.size() - 1) : 3], hit_pic, this->m_BackGround->GetBox2Dworld(), (PlayerID)index);
						v->ValueSet(deg2rad(0), rad_t, pos_t);
					}
				}
				//player
				PlayerMngr->Init(Player_num);
				for (int i = 0; i < Player_num; i++) {
					//PlayerMngr->GetPlayer(i).SetVehicle(nullptr);
					PlayerMngr->GetPlayer(i).SetVehicle((std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i)));
					auto& Vehicle = PlayerMngr->GetPlayer(i).GetVehicle();
					{
						auto Select = std::find_if(m_ItemData.begin(), m_ItemData.end(), [&](const std::shared_ptr<ItemData>& d) {return (d == Vehicle->GetGun()[0].GetAmmoSpec(0)); });
						if (Select != m_ItemData.end()) {
							{
								int xp = 0;
								int yp = 0;
								while (true) {
									PlayerMngr->GetPlayer(i).PutInventory(0, xp, yp, *Select, -1, false);
									xp += (*Select)->GetXsize();
									if (xp >= 5) {
										xp = 0;
										yp += (*Select)->GetYsize();
										if (yp >= 6) {
											break;
										}
									}
								}
							}
							{
								int xp = 0;
								int yp = 0;
								while (true) {
									PlayerMngr->GetPlayer(i).PutInventory(1, xp, yp, *Select, -1, false);
									xp += (*Select)->GetXsize();
									if (xp >= 5) {
										xp = 0;
										yp += (*Select)->GetYsize();
										if (yp >= 6) {
											break;
										}
									}
								}
							}
						}
					}
					if (Vehicle->Get_Gunsize() >= 2) {
						auto Select = std::find_if(m_ItemData.begin(), m_ItemData.end(), [&](const std::shared_ptr<ItemData>& d) {return (d == Vehicle->GetGun()[1].GetAmmoSpec(0)); });
						if (Select != m_ItemData.end()) {
							{
								int xp = 5;
								int yp = 0;
								while (true) {
									PlayerMngr->GetPlayer(i).PutInventory(0, xp, yp, *Select, -1, false);
									xp += (*Select)->GetXsize();
									if (xp >= 10) {
										xp = 5;
										yp += (*Select)->GetYsize();
										if (yp >= 6) {
											break;
										}
									}
								}
							}
							for (int x = 0; x < 5; x++) {
								PlayerMngr->GetPlayer(i).PutInventory(1, x, 6, *Select, -1, false);
							}
						}
					}
					{
						for (int y = 0; y < 10; y++) {
							PlayerMngr->GetPlayer(i).PutInventory(2, 0, y, Vehicle->GetTrackPtr(), -1, false);
						}
						for (int y = 0; y < 10; y++) {
							PlayerMngr->GetPlayer(i).PutInventory(3, 0, y, Vehicle->GetTrackPtr(), -1, false);
						}
					}
					{
						auto Select = std::find_if(m_ItemData.begin(), m_ItemData.end(), [&](const std::shared_ptr<ItemData>& d) {return (d->GetPath().find("DieselMiniTank") != std::string::npos); });
						if (Select != m_ItemData.end()) {
							for (int x = 0; x < 5; x++) {
								PlayerMngr->GetPlayer(i).PutInventory(4, x * 2, 0, *Select, -1, false);
							}
						}
					}
					AICtrl[i]->Init(&vehicle_Pool, this->m_BackGround, PlayerMngr->GetPlayer(i).GetVehicle());
				}
				this->m_HPBuf = (float)PlayerMngr->GetPlayer(0).GetVehicle()->GetHP();
				this->m_ScoreBuf = PlayerMngr->GetPlayer(0).GetScore();
				//Cam
				SetMainCamera().SetCamInfo(deg2rad(OPTION::Instance()->Get_Fov()), 1.f, 100.f);
				SetMainCamera().SetCamPos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
				//サウンド
				SE->Add((int)SoundEnum::Environment, 1, "data/Sound/SE/envi.wav");
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
				SE->Get((int)SoundEnum::Environment).SetVol(0.25f);
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
				this->m_NetWorkBrowser.Init();
				this->m_InventoryClass.Init();
			}
			//
			bool			Update_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto* SE = SoundPool::Instance();
				auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
#ifdef DEBUG
				auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
				DebugParts->SetPoint("Execute=Start(0.9)");
#endif // DEBUG
				//FirstDoingv
				if (GetIsFirstLoop()) {
					SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
					SE->Get((int)SoundEnum::Environment).Play(0, DX_PLAYTYPE_LOOP, TRUE);
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
								MyInput.SetInput(
									pp_x*(1.f - this->m_TPS_Per) - ReCoil.y(),
									pp_y*(1.f - this->m_TPS_Per) - ReCoil.x(),
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
					this->m_NetWorkBrowser.FirstExecute(MyInput, PlayerMngr->GetPlayer(GetMyPlayerID()).GetNetSendMove());
					this->m_InventoryClass.FirstExecute();
					//クライアント
					if (this->m_NetWorkBrowser.GetIsClient()) {
						for (auto& v : this->vehicle_Pool) {
							if (v->GetMyPlayerID() == GetMyPlayerID()) {
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
						auto& v = PlayerMngr->GetPlayer(i).GetVehicle();
						if (this->m_NetWorkBrowser.GetSequence() == SequenceEnum::MainGame) {
							auto tmp = m_NetWorkBrowser.GetNowServerPlayerData(i, true);
							if (i == GetMyPlayerID()) {
								MyInput.SetKeyInput(tmp.Input.GetKeyInput());//キーフレームだけサーバーに合わせる
								v->SetInput(MyInput, isready, false);
								this->m_NetWorkBrowser.GetRecvData(i, tmp.Frame);
							}
							else {
								if (!m_NetWorkBrowser.GetIsClient()) {
									AICtrl[i]->AI_move(&tmp.Input);
								}
								v->SetInput(tmp.Input, isready, true);
								bool override_true = true;
								for (auto& v2 : this->vehicle_Pool) {
									if ((v != v2) && ((v->GetMove().pos - v2->GetMove().pos).size() <= 10.f*Scale_Rate)) {
										override_true = false;
										break;
									}
								}
								if (override_true) {
									v->SetPosBufOverRide(tmp.PosBuf, tmp.VecBuf, tmp.radBuf);
								}

							}
							//ダメージイベント処理
							if (v.get() != nullptr) {
								if (tmp.DamageSwitch != v->GetDamageSwitchRec()) {
									this->m_DamageEvents.emplace_back(tmp.Damage);
									v->SetDamageSwitchRec(tmp.DamageSwitch);
								}
							}
						}
						else {
							if (i == GetMyPlayerID()) {
								v->SetInput(MyInput, isready, false);
							}
							else {
								InputControl OtherInput;
								AICtrl[i]->AI_move(&OtherInput);//めっちゃ重い
								v->SetInput(OtherInput, isready, false);
							}
							//ダメージイベント処理
							if (v.get() != nullptr) {
								if (v->GetDamageSwitch() != v->GetDamageSwitchRec()) {
									this->m_DamageEvents.emplace_back(v->GetDamageEvent());
									v->SetDamageSwitchRec(v->GetDamageSwitch());
								}
							}
						}
					}
					this->m_NetWorkBrowser.LateExecute();
					this->m_InventoryClass.LateExecute();
					//ダメージイベント
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
#ifdef DEBUG
				DebugParts->SetPoint("Execute=0.1ms");
#endif // DEBUG
				//レーザーサイト
				{
					auto StartPos = Vehicle->GetGunMuzzlePos(0);
					auto EndPos = StartPos + Vehicle->GetGunMuzzleVec(0) * 100.f*Scale_Rate;
					this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, true, false);
					for (auto& v : this->vehicle_Pool) {
						if (v->GetMyPlayerID() == GetMyPlayerID()) { continue; }
						if (v->RefreshCol(StartPos, EndPos, 10.f*Scale_Rate)) {
							v->GetColNearestInAllMesh(StartPos, &EndPos);
						}
					}
					Vehicle->SetAimingDistance((StartPos - EndPos).size());
				}
#ifdef DEBUG
				DebugParts->SetPoint("Execute=4.5ms");
#endif // DEBUG
				//アイテムのアタッチ
				{
					int loop = 0;
					while (true) {
						auto item = ObjMngr->GetObj(ObjType::Item, loop);
						if (item) {
							auto& ip = (std::shared_ptr<ItemClass>&)(*item);
							if (!ip->IsActive()) {
								ip->SetVehPool(&vehicle_Pool);
								ip->SetActive(true);
							}
						}
						else {
							break;
						}
						loop++;
					}
				}
				//Execute
				ObjMngr->ExecuteObject();
				//いらないオブジェクトの除去
				ObjMngr->DeleteCheck();
#ifdef DEBUG
				DebugParts->SetPoint("Execute=1.0ms");
#endif // DEBUG
				//弾の更新
				{
					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);

							if (a->IsActive()) {
								//AmmoClass
								VECTOR_ref repos_tmp = a->GetMove().repos;
								VECTOR_ref pos_tmp = a->GetMove().pos;
								VECTOR_ref norm_tmp;
								bool ColRes = this->m_BackGround->CheckLinetoMap(repos_tmp, &pos_tmp, true, false, &norm_tmp);
								ColRes |= this->m_BackGround->GetWallCol(repos_tmp, &pos_tmp, &norm_tmp, a->GetCaliberSize());
								bool is_HitAll = false;
								auto& v = *ObjMngr->GetObj(ObjType::Vehicle, a->GetShootedID());
								for (auto& tgt : this->vehicle_Pool) {
									if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
									auto res = tgt->CheckAmmoHit(a.get(), v->GetMove().pos);
									is_HitAll |= res.first;
									if (res.second) { break; }
								}
								if (ColRes && !is_HitAll) {
									a->HitGround(pos_tmp);
									EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, pos_tmp, norm_tmp, a->GetCaliberSize() / 0.1f * Scale_Rate);
									/*
									if (ObjMngr->GetObj(ObjType::Vehicle, a->GetShootedID())) {
										((std::shared_ptr<VehicleClass>&)v)->HitGround(pos_tmp, (pos_tmp - repos_tmp).Norm(), a->GetMove().vec);
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
				//木の更新
				{
					for (auto& v : this->vehicle_Pool) {
						this->m_BackGround->CheckTreetoSquare(v->GetSquarePos(0), v->GetSquarePos(2), v->GetSquarePos(3), v->GetSquarePos(1), v->GetMove().pos,
							(v->GetMove().pos - v->GetMove().repos).Length() * 60.f / FPS);
					}
				}
				//ハインド
				{
					auto& Hind = *ObjMngr->GetObj(ObjType::HindD, 0);
					Hind->SetAnimLoop(0, 1.f);
					Hind->GetObj().work_anime();
				}
				this->m_BackGround->FirstExecute();
				ObjMngr->LateExecuteObject();
				//視点
				{
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
					/*
					SetMainCamera().SetCamPos(
						VECTOR_ref::vget(0, 600.f*Scale_Rate, 1.f),
						VECTOR_ref::zero(),
						VECTOR_ref::up());
					SetMainCamera().SetCamInfo(100.f, 1000.f*Scale_Rate, deg2rad(45));
					//*/
				}
				this->m_BackGround->Execute();

				//UIパラメーター
				{
					this->m_UIclass.SetIntParam(1, (int)this->m_ScoreBuf);
					this->m_ScoreBuf += std::clamp((PlayerMngr->GetPlayer(0).GetScore() - this->m_ScoreBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(2, 1);

					this->m_UIclass.SetIntParam(3, (int)Vehicle->GetHP());
					this->m_UIclass.SetIntParam(4, (int)Vehicle->GetHPMax());
					this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
					this->m_HPBuf += std::clamp((Vehicle->GetHP() - this->m_HPBuf)*100.f, -500.f, 500.f) / FPS;

					this->m_UIclass.SetIntParam(6, (int)1.f);
					this->m_UIclass.SetIntParam(7, (int)1.f);
					this->m_UIclass.SetIntParam(8, (int)1.f);


					this->m_UIclass.SetIntParam(12, (int)0);//現在選択
					this->m_UIclass.SetIntParam(13, (int)1);//銃の総数
				}
				EffectControl::Execute();
#ifdef DEBUG
				DebugParts->SetPoint("Execute=1.0ms");
#endif // DEBUG
				return true;
			}
			void			Dispose_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();

				this->m_NetWorkBrowser.Dispose();
				this->m_InventoryClass.Dispose();
				EffectControl::Dispose();
				PlayerMngr->Dispose();
				ObjMngr->DisposeObject();
				for (auto& v : this->vehicle_Pool) {
					v.reset();
				}
				this->vehicle_Pool.clear();
				this->m_BackGround->Dispose();
				this->m_VehDataControl->Dispose();
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
			void			ShadowDraw_Far_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw_Far();
			}

			void			ShadowDraw_NearFar_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw_NearFar();
			}
			void			ShadowDraw_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				this->m_BackGround->Shadow_Draw();
				ObjMngr->DrawObject_Shadow();
			}
			void			MainDraw_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
				SetFogStartEnd(GetMainCamera().GetCamNear(), GetMainCamera().GetCamFar()*2.f);
				this->m_BackGround->Draw();
				ObjMngr->DrawObject();
				//ObjMngr->DrawDepthObject();
				//シェーダー描画用パラメーターセット
				auto EndPos = Vehicle->GetGunMuzzlePos(0) + Vehicle->GetGunMuzzleVec(0) * Vehicle->GetAimingDistance();
				VECTOR_ref LensPos = ConvWorldPosToScreenPos(EndPos.get());
				if (0.f < LensPos.z() && LensPos.z() < 1.f) {
					Reticle_xpos = LensPos.x();
					Reticle_ypos = LensPos.y();
					Reticle_on = true;
					if (!this->m_MouseActive.on()) {
						Reticle_on = false;
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


				for (int i = 0; i < Player_num; i++) {
					AICtrl[i]->Draw();
				}

			}
			void			MainDrawbyDepth_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();

				ObjMngr->DrawDepthObject();
			}
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override {
				//return;
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
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
							if (GetMyPlayerID() == a->GetMyPlayerID()) {
								a->Draw_Hit_UI(hit_Graph);
							}
						}
						else {
							break;
						}
						loop++;
					}
				}
				//UI
				if (Vehicle->is_ADS()) {
					scope_Graph.DrawExtendGraph(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);
				}
				this->m_UIclass.Draw();
				Vehicle->DrawModuleView(y_r(50 + 100), DrawParts->m_DispYSize - y_r(100 + 100), y_r(200));
				//通信設定
				//if (!this->m_MouseActive.on()) {
				//	this->m_NetWorkBrowser.Draw();
				//}
				//Pause
				this->m_InventoryClass.Draw(!this->m_MouseActive.on(), Vehicle->GetMove().pos);
			}
			void			DrawUI_In_Sub(void) noexcept override {
				//auto* DrawParts = DXDraw::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto* Fonts = FontPool::Instance();
				auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
				if (Reticle_on) {
					aim_Graph.DrawRotaGraph((int)Reticle_xpos, (int)Reticle_ypos, (float)(y_r(100)) / 100.f, 0.f, true);

					unsigned int color = GetColor(0, 255, 0);
					auto Time = Vehicle->GetTotalloadtime(0);
					if (Vehicle->Gunloadtime(0) != 0.f) {
						color = GetColor(255, 0, 0);
						Time = Vehicle->Gunloadtime(0);
					}
					Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, (int)Reticle_xpos + y_r(50), (int)Reticle_ypos, color, GetColor(0, 0, 0), "%05.2f s", Time);
				}
			}
		};
	};
};
