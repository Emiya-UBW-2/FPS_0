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
			std::shared_ptr<BackGroundClass>			m_BackGround;					//BG
			MV1											m_hit_pic;						//弾痕  
			//いちいち探査しないよう別持ち
			std::vector<std::shared_ptr<VehicleClass>>	m_vehicle_Pool;					//ポインター別持ち
			std::vector<std::shared_ptr<ItemData>>		m_ItemData;						//アイテムデータ
			std::shared_ptr<VehDataControl>				m_VehDataControl;				//戦車データ
			std::vector<std::shared_ptr<AIControl>>		m_AICtrl;						//AI
			//操作関連
			std::unique_ptr<InputClass>					m_InputClass;
			MATRIX_ref									m_FreeLookMat;
			float										m_Concussion{ 0.f };
			float										m_ConcussionPer{ 0.f };
			//UI関連
			UIClass										m_UIclass;
			float										m_HPBuf{ 0.f };
			float										m_ScoreBuf{ 0.f };
			GraphHandle									m_hit_Graph;
			GraphHandle									m_aim_Graph;
			GraphHandle									m_scope_Graph;
			bool										m_Reticle_on = false;
			float										m_Reticle_xpos = 0;
			float										m_Reticle_ypos = 0;
			NetWorkBrowser								m_NetWorkBrowser;
			InventoryClass								m_InventoryClass;
			//
			std::vector<DamageEvent>					m_DamageEvents;
			float										m_fov_base{ deg2rad(45.f) };
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return m_NetWorkBrowser.GetMyPlayerID(); }
		public:
			MAINLOOP(void) noexcept {
				this->m_BackGround = std::make_shared<BackGroundClass>();
				this->m_VehDataControl = std::make_shared<VehDataControl>();
				for (int i = 0; i < Player_num; i++) {
					this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
				}
				this->m_InputClass = std::make_unique<InputClass>();
			}
			~MAINLOOP(void) noexcept {}
		public:
			//Load
			void			Load_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				//BG
				this->m_BackGround->Load();
				ObjMngr->Init(this->m_BackGround);
				//
				this->m_VehDataControl->Load();
				MV1::Load("data/model/hit/model.mv1", &this->m_hit_pic);

				this->m_hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
				this->m_aim_Graph = GraphHandle::Load("data/UI/battle_aim.bmp");
				this->m_scope_Graph = GraphHandle::Load("data/UI/battle_scope.png");
			}
			void			Set_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();
				auto* SE = SoundPool::Instance();

				this->m_BackGround->Init();
				SetAmbientShadow(
					VECTOR_ref::vget(Scale_Rate*-300.f, Scale_Rate*-10.f, Scale_Rate*-300.f),
					VECTOR_ref::vget(Scale_Rate*300.f, Scale_Rate*50.f, Scale_Rate*300.f),
					VECTOR_ref::vget(-0.8f, -0.5f, -0.1f),
					GetColorF(0.42f, 0.41f, 0.40f, 0.0f));
				for (int i = 0; i < Vehicle_num; i++) {
					this->m_vehicle_Pool.emplace_back((std::shared_ptr<VehicleClass>&)(*ObjMngr->AddObject(ObjType::Vehicle)));
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
					for (auto& v : this->m_vehicle_Pool) {
						size_t index = &v - &this->m_vehicle_Pool.front();
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
						v->ValueInit(&vehc_data[index != 0 ? GetRand((int)vehc_data.size() - 1) : 1], this->m_hit_pic, this->m_BackGround->GetBox2Dworld(), (PlayerID)index);
						v->ValueSet(deg2rad(0), rad_t, pos_t);
					}
				}
				//player
				PlayerMngr->Init(Player_num);
				for (int i = 0; i < Player_num; i++) {
					//PlayerMngr->GetPlayer(i).SetVehicle(nullptr);
					PlayerMngr->GetPlayer(i).SetVehicle((std::shared_ptr<VehicleClass>&)(*ObjMngr->GetObj(ObjType::Vehicle, i)));
					auto& Vehicle = PlayerMngr->GetPlayer(i).GetVehicle();
					for (int loop = 0; loop < 5; loop++) {
						PlayerMngr->GetPlayer(i).SetInventory(loop, Vehicle->GetInventoryXSize(loop), Vehicle->GetInventoryYSize(loop));
					}
					{
						if (Vehicle->Get_Gunsize() >= 2) {
							PlayerMngr->GetPlayer(i).FillInventory(0, Vehicle->GetGun()[0].GetAmmoSpec(0), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(0) / 2, PlayerMngr->GetPlayer(i).GetInventoryYSize(0));
							PlayerMngr->GetPlayer(i).FillInventory(0, Vehicle->GetGun()[1].GetAmmoSpec(0), PlayerMngr->GetPlayer(i).GetInventoryXSize(0) / 2, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(0), PlayerMngr->GetPlayer(i).GetInventoryYSize(0));
						}
						else {
							PlayerMngr->GetPlayer(i).FillInventory(0, Vehicle->GetGun()[0].GetAmmoSpec(0), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(0), PlayerMngr->GetPlayer(i).GetInventoryYSize(0));
						}
						PlayerMngr->GetPlayer(i).FillInventory(1, Vehicle->GetGun()[0].GetAmmoSpec(0), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(1), 6);
					}
					{
						PlayerMngr->GetPlayer(i).FillInventory(2, Vehicle->GetTrackPtr(), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(2), PlayerMngr->GetPlayer(i).GetInventoryYSize(2));
						PlayerMngr->GetPlayer(i).FillInventory(3, Vehicle->GetTrackPtr(), 0, 0, PlayerMngr->GetPlayer(i).GetInventoryXSize(3), PlayerMngr->GetPlayer(i).GetInventoryYSize(3));
					}
					{
						auto Select = std::find_if(m_ItemData.begin(), m_ItemData.end(), [&](const std::shared_ptr<ItemData>& d) {return (d->GetPath().find("DieselMiniTank") != std::string::npos); });
						if (Select != m_ItemData.end()) {
							for (int x = 0; x < 5; x++) {
								PlayerMngr->GetPlayer(i).PutInventory(4, x * 2, 0, *Select, -1, false);
							}
						}
					}
					this->m_AICtrl[i]->Init(&this->m_vehicle_Pool, this->m_BackGround, PlayerMngr->GetPlayer(i).GetVehicle());
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
				SE->Add((int)SoundEnum::Tank_move, Vehicle_num, "data/Sound/SE/Engine/move.wav");
				SE->Add((int)SoundEnum::Tank_engine, Vehicle_num, "data/Sound/SE/Engine/engine.wav");
				SE->Add((int)SoundEnum::Tank_Start, Vehicle_num, "data/Sound/SE/Engine/start.wav");
				for (int i = 0; i < 7; i++) {
					SE->Add((int)SoundEnum::Tank_Eject, 3, "data/Sound/SE/gun/reload/eject/" + std::to_string(i) + ".wav", false);
				}
				for (int i = 0; i < 5; i++) {
					SE->Add((int)SoundEnum::Tank_Reload, 3, "data/Sound/SE/gun/reload/hand/" + std::to_string(i) + ".wav", false);
				}
				SE->Add((int)SoundEnum::Tank_near, 10, "data/Sound/SE/near.wav");

				SE->Get((int)SoundEnum::Environment).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_Shot).SetVol(0.5f);
				SE->Get((int)SoundEnum::Tank_engine).SetVol(0.5f);
				SE->Get((int)SoundEnum::Tank_Start).SetVol(0.5f);
				SE->Get((int)SoundEnum::Tank_move).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_Ricochet).SetVol(0.65f);
				SE->Get((int)SoundEnum::Tank_Damage).SetVol(0.65f);
				SE->Get((int)SoundEnum::Tank_Eject).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_Reload).SetVol(0.25f);
				SE->Get((int)SoundEnum::Tank_near).SetVol(1.f);
				//入力
				this->m_InputClass->Init();
				//
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
				DebugParts->SetPoint("Execute=Start(0.3)");
#endif // DEBUG
				//FirstDoingv
				if (GetIsFirstLoop()) {
					SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
					SE->Get((int)SoundEnum::Environment).Play(0, DX_PLAYTYPE_LOOP, TRUE);
					this->m_fov_base = GetMainCamera().GetCamFov();
				}
				//Input,AI
				{
					this->m_InputClass->Execute(((GetMainCamera().GetCamFov() / this->m_fov_base) / (is_lens() ? zoom_lens() : 1.f)) / 100.f, Vehicle->Get_alive());
					//ネットワーク
					this->m_NetWorkBrowser.FirstExecute(this->m_InputClass->GetInput(), PlayerMngr->GetPlayer(GetMyPlayerID()).GetNetSendMove());
					this->m_InventoryClass.FirstExecute();
					//クライアント
					if (this->m_NetWorkBrowser.GetIsClient()) {
						for (auto& v : this->m_vehicle_Pool) {
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
								this->m_InputClass->SetInput(tmp.Input.GetKeyInput());//キーフレームだけサーバーに合わせる
								v->SetInput(this->m_InputClass->GetInput(), isready, false);
								this->m_NetWorkBrowser.GetRecvData(i, tmp.Frame);
							}
							else {
								if (!m_NetWorkBrowser.GetIsClient()) {
									m_AICtrl[i]->AI_move(&tmp.Input);
								}
								v->SetInput(tmp.Input, isready, true);
								bool override_true = true;
								for (auto& v2 : this->m_vehicle_Pool) {
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
								v->SetInput(this->m_InputClass->GetInput(), isready, false);
							}
							else {
								InputControl OtherInput;
								m_AICtrl[i]->AI_move(&OtherInput);//めっちゃ重い
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
					for (auto& v : this->m_vehicle_Pool) {
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
				DebugParts->SetPoint("Execute=0.2ms");
#endif // DEBUG
				//レーザーサイト
				{
					auto StartPos = Vehicle->GetGunMuzzlePos(0);
					auto EndPos = StartPos + Vehicle->GetGunMuzzleVec(0) * 100.f*Scale_Rate;
					this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, true, false);
					for (auto& v : this->m_vehicle_Pool) {
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
								ip->SetVehPool(&m_vehicle_Pool);
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
				DebugParts->SetPoint("Execute=0.7ms");
#endif // DEBUG
				//弾の更新
				{
					m_Concussion = std::max(m_Concussion - 1.f / FPS, 0.f);

					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);

							if (a->IsActive()) {
								//AmmoClass
								VECTOR_ref repos_tmp = a->GetMove().repos;
								VECTOR_ref pos_tmp = a->GetMove().pos;
								if (GetMyPlayerID() != a->GetShootedID()) {
									if (GetMinLenSegmentToPoint(repos_tmp, pos_tmp, GetMainCamera().GetCamPos()) < 5.f*Scale_Rate) {
										m_Concussion = 1.f;
									}
								}

								VECTOR_ref norm_tmp;
								bool ColRes = this->m_BackGround->CheckLinetoMap(repos_tmp, &pos_tmp, true, false, &norm_tmp);
								ColRes |= this->m_BackGround->GetWallCol(repos_tmp, &pos_tmp, &norm_tmp, a->GetCaliberSize());
								bool is_HitAll = false;
								auto& v = *ObjMngr->GetObj(ObjType::Vehicle, a->GetShootedID());
								for (auto& tgt : this->m_vehicle_Pool) {
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
					//コンカッション
					Set_is_Blackout(m_Concussion > 0.f);
					if (m_Concussion == 1.f) {
						Vehicle->ShakePer(0.05f);
						SE->Get((int)SoundEnum::Tank_near).Play_3D(0, GetMainCamera().GetCamPos(), 10.f*Scale_Rate, 128);//, DX_PLAYTYPE_LOOP
					}
					if (m_Concussion > 0.9f) {
						Easing(&m_ConcussionPer, 1.f, 0.1f, EasingType::OutExpo);
					}
					else if (m_Concussion > 0.25f) {
						if (m_ConcussionPer > 0.25f) {
							Easing(&m_ConcussionPer, 0.f, 0.8f, EasingType::OutExpo);
						}
						else {
							m_ConcussionPer = 0.25f;
						}
					}
					else {
						Easing(&m_ConcussionPer, 0.f, 0.8f, EasingType::OutExpo);
					}
					Set_Per_Blackout(m_ConcussionPer * 1.5f);

					//Set_is_lens(true);
					//Set_xp_lens(y_r(960));
					//Set_yp_lens(y_r(540));
					//Set_size_lens(y_r(300));
					//Set_zoom_lens(3.f);
				}
				//木の更新
				{
					for (auto& v : this->m_vehicle_Pool) {
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
					auto& VehicleView = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
					VehicleView->Setcamera(SetMainCamera(), this->m_fov_base);
					VECTOR_ref BaseVec = (GetMainCamera().GetCamVec() - GetMainCamera().GetCamPos()).Norm();
					VECTOR_ref BaseUp = GetMainCamera().GetCamUp();
					MATRIX_ref FreeLook = MATRIX_ref::RotAxis(BaseVec.cross(BaseUp), this->m_InputClass->GetTPS_XradR()) * MATRIX_ref::RotAxis(BaseUp, this->m_InputClass->GetTPS_YradR());
					Easing_Matrix(&this->m_FreeLookMat, FreeLook, 0.5f, EasingType::OutExpo);
					VECTOR_ref CamVec = Lerp(BaseVec, MATRIX_ref::Vtrans(BaseVec, this->m_FreeLookMat), this->m_InputClass->GetTPS_Per());
					VECTOR_ref CamPos = VehicleView->Get_EyePos_Base();

					SetMainCamera().SetCamPos(
						Lerp(GetMainCamera().GetCamPos(), (CamPos + CamVec * -100.f), this->m_InputClass->GetTPS_Per()),
						CamPos + CamVec * 100.f,
						BaseUp);
				}
				this->m_BackGround->Execute();

				//UIパラメーター
				{
					this->m_UIclass.SetIntParam(1, (int)this->m_ScoreBuf);
					this->m_ScoreBuf += std::clamp((PlayerMngr->GetPlayer(0).GetScore() - this->m_ScoreBuf)*100.f, -5.f, 5.f) / FPS;

					this->m_UIclass.SetIntParam(2, 1);

					this->m_UIclass.SetStrParam(0, Vehicle->GetName());
					this->m_UIclass.SetIntParam(3, (int)Vehicle->GetHP());
					this->m_UIclass.SetIntParam(4, (int)Vehicle->GetHPMax());
					this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
					this->m_HPBuf += std::clamp((Vehicle->GetHP() - this->m_HPBuf)*100.f, -500.f, 500.f) / FPS;

					this->m_UIclass.SetIntParam(6, (int)Vehicle->GetFuel());
					this->m_UIclass.SetIntParam(7, (int)Vehicle->GetFuelMax());
					this->m_UIclass.SetIntParam(8, (int)Vehicle->GetFuel());


					this->m_UIclass.SetIntParam(12, (int)0);//現在選択
					this->m_UIclass.SetIntParam(13, (int)Vehicle->Get_Gunsize());//銃の総数


					this->m_UIclass.SetStrParam(1, Vehicle->GetGun()[0].GetGunSpec()->GetName());
					this->m_UIclass.SetIntParam(14, (int)1);//現在選択
					this->m_UIclass.SetIntParam(15, (int)1);//銃の総数
					this->m_UIclass.SetItemGraph(0, &m_aim_Graph);
					if (Vehicle->Get_Gunsize() >= 2) {
						this->m_UIclass.SetStrParam(2, Vehicle->GetGun()[1].GetGunSpec()->GetName());
						this->m_UIclass.SetIntParam(16, (int)1);//現在選択
						this->m_UIclass.SetIntParam(17, (int)1);//銃の総数
						this->m_UIclass.SetItemGraph(1, &m_aim_Graph);
					}
				}
				EffectControl::Execute();
#ifdef DEBUG
				DebugParts->SetPoint("Execute=0.7ms");
#endif // DEBUG
				return true;
			}
			void			Dispose_Sub(void) noexcept override {
				auto* ObjMngr = ObjectManager::Instance();
				auto* PlayerMngr = PlayerManager::Instance();

				this->m_NetWorkBrowser.Dispose();
				this->m_InventoryClass.Dispose();
				for (auto& v : this->m_vehicle_Pool) {
					v.reset();
				}
				this->m_vehicle_Pool.clear();
				EffectControl::Dispose();
				PlayerMngr->Dispose();
				ObjMngr->DisposeObject();
				this->m_BackGround->Dispose();
				this->m_VehDataControl->Dispose();
			}
			//
			void			Depth_Draw_Sub(void) noexcept override {}
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
					this->m_Reticle_xpos = LensPos.x();
					this->m_Reticle_ypos = LensPos.y();
					this->m_Reticle_on = true;
					if (!this->m_InputClass->GetMouseActive().on()) {
						this->m_Reticle_on = false;
					}
				}
				for (auto& v : this->m_vehicle_Pool) {
					if (v->GetMyPlayerID() == GetMyPlayerID()) { continue; }
					auto pos = v->Set_MidPos();
					VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
					if (0.f < campos.z() && campos.z() < 1.f) {
						v->SetCameraPosition(campos);
						v->SetCameraSize(std::max(80.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
					}
				}


				for (int i = 0; i < Player_num; i++) {
					m_AICtrl[i]->Draw();
				}

			}
			void			MainDrawbyDepth_Sub(void) noexcept override {}
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
				{
					int loop = 0;
					while (true) {
						auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
						if (ammo != nullptr) {
							auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
							if (GetMyPlayerID() == a->GetMyPlayerID()) {
								a->Draw_Hit_UI(this->m_hit_Graph);
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
					this->m_scope_Graph.DrawExtendGraph(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);
				}
				if (Vehicle->Get_alive()) {
					this->m_UIclass.Draw();
					Vehicle->DrawModuleView(y_r(50 + 100), DrawParts->m_DispYSize - y_r(100 + 150), y_r(200));
				}
				//通信設定
				//if (!this->m_MouseActive.on()) {
				//	this->m_NetWorkBrowser.Draw();
				//}
				//Pause
				this->m_InventoryClass.Draw(!this->m_InputClass->GetMouseActive().on(), Vehicle->GetMove().pos);
			}
			void			DrawUI_In_Sub(void) noexcept override {
				auto* PlayerMngr = PlayerManager::Instance();
				auto* Fonts = FontPool::Instance();
				auto& Vehicle = PlayerMngr->GetPlayer(GetMyPlayerID()).GetVehicle();
				if (Vehicle->Get_alive()) {
					if (this->m_Reticle_on) {
						this->m_aim_Graph.DrawRotaGraph((int)this->m_Reticle_xpos, (int)this->m_Reticle_ypos, (float)(y_r(100)) / 100.f, 0.f, true);

						unsigned int color = GetColor(0, 255, 0);
						auto Time = Vehicle->GetTotalloadtime(0);
						if (Vehicle->Gunloadtime(0) != 0.f) {
							color = GetColor(255, 0, 0);
							Time = Vehicle->Gunloadtime(0);
						}
						Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, (int)this->m_Reticle_xpos + y_r(50), (int)this->m_Reticle_ypos, color, GetColor(0, 0, 0), "%05.2f s", Time);
					}
				}
			}
		};
	};
};
