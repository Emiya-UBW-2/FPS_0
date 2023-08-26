#pragma once
#include	"MainScene.hpp"
#include "../sub.hpp"
#include "../ObjectManager.hpp"
#include "../MainScene/Player/Player.hpp"
#include "../MainScene/Player/CPU.hpp"
#include "../MainScene/Object/Character.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			MAINLOOP::Set_Sub(void) noexcept {
			m_GameEnd = false;
			if (g_Mode == 1) {
				TotalTime = 600.f;
			}
			else {
				TotalTime = 180.f;
			}


			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			//
			SetAmbientShadow(
				VECTOR_ref::vget(Scale_Rate*-600.f, Scale_Rate*-300.f, Scale_Rate*-600.f),
				VECTOR_ref::vget(Scale_Rate*600.f, Scale_Rate*100.f, Scale_Rate*600.f),
				VECTOR_ref::vget(-0.8f, -0.5f, -0.1f),
				GetColorF(0.92f, 0.91f, 0.90f, 0.0f));
			//
			ObjMngr->Init(this->m_BackGround);

			{
				auto kill = FPS_n2::SaveDataClass::Instance()->GetParam("KillCount");
				auto killTotal = 80;
				if (g_Mode == 1 && (kill >= killTotal)) {
					character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Plane, "data/Plane/Albatros_2/")));
					for (int i = 1; i < Chara_num * 1 / 3; i++) {
						//character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Plane, "data/Plane/Sopwith/")));
						character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Plane, "data/Plane/Albatros/")));
					}
				}
				else {
					for (int i = 0; i < Chara_num * 1 / 3; i++) {
						//character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Plane, "data/Plane/Sopwith/")));
						character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Plane, "data/Plane/Albatros/")));
					}
				}
			}


			for (int i = Chara_num * 1 / 3; i < Chara_num; i++) {
				//character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Plane, "data/Plane/Albatros/")));
				character_Pool.emplace_back((std::shared_ptr<CharacterClass>&)(*ObjMngr->AddObject(ObjType::Plane, "data/Plane/Sopwith/")));
			}
			for (int i = 0; i < Chara_num; i++) {
				this->m_AICtrl.emplace_back(std::make_shared<AIControl>());
			}
			//UI
			this->m_UIclass.Set();

			stand_AnimTime = 5.f;
			//Set
			//人
			for (auto& c : this->character_Pool) {
				size_t index = &c - &this->character_Pool.front();

				VECTOR_ref pos_t;
				float rad_t = 0.f;
				if (index < Chara_num * 1 / 3) {
					pos_t = VECTOR_ref::vget(0.f*Scale_Rate - (float)((int)(index)-Chara_num / 4)*40.f*Scale_Rate, 300.f*Scale_Rate, 300.f*Scale_Rate);
					rad_t = deg2rad(0.f);
				}
				else {
					pos_t = VECTOR_ref::vget(0.f*Scale_Rate - (float)((int)(index - Chara_num / 2) - Chara_num / 4)*40.f*Scale_Rate, 300.f*Scale_Rate, -300.f*Scale_Rate);
					rad_t = deg2rad(180.f + 0.f);
				}
				pos_t += VECTOR_ref::vget(GetRandf(150.f*Scale_Rate), GetRandf(50.f*Scale_Rate), GetRandf(150.f*Scale_Rate));
				c->ValueSet(deg2rad(0.f), rad_t, pos_t, (PlayerID)index);
				if (index < Chara_num * 1 / 3) {
					//c->SetUseRealTimePhysics(true);
					c->SetUseRealTimePhysics(false);
					c->SetCharaType(CharaTypeID::Team);
				}
				else {
					//c->SetUseRealTimePhysics(true);
					c->SetUseRealTimePhysics(false);
					c->SetCharaType(CharaTypeID::Enemy);
				}
			}
			//player
			PlayerMngr->Init(Player_num);
			for (int i = 0; i < Player_num; i++) {
				PlayerMngr->GetPlayer(i).SetChara((std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Plane, i)));
				this->m_AICtrl[i]->Init(this->m_BackGround, (PlayerID)i);
			}
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			this->m_HPBuf = (float)Chara->GetHP();
			//Cam
			SetMainCamera().SetCamInfo(deg2rad(65), 1.f, 100.f);
			SetMainCamera().SetCamPos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
			//サウンド
			this->m_BGM.vol(128);
			SE->Get((int)SoundEnum::Env).SetVol_Local(192);
			SE->Get((int)SoundEnum::Engine).SetVol_Local(64);
			SE->Get((int)SoundEnum::Propeller).SetVol_Local(128);
			SE->Get((int)SoundEnum::Shot2).SetVol_Local(128);
			//入力
			this->m_MouseActive.Set(true);
			//
			this->m_DamageEvents.clear();
			m_NetWorkBrowser.Init();
			//
			StartTimer = 3.f;
		}
		bool			MAINLOOP::Update_Sub(bool*  isPause) noexcept {
			auto* Pad = FPS_n2::PadControl::Instance();
			//ポーズ
			if (GetJoypadNum() > 0) {
				DINPUT_JOYSTATE input;
				int padID = DX_INPUT_PAD1;
				GetJoypadInputState(padID);
				switch (GetJoypadType(padID)) {
				case DX_PADTYPE_OTHER:
				case DX_PADTYPE_DUAL_SHOCK_4:
				case DX_PADTYPE_DUAL_SENSE:
				case DX_PADTYPE_SWITCH_JOY_CON_L://大丈夫？
				case DX_PADTYPE_SWITCH_JOY_CON_R://大丈夫？
				case DX_PADTYPE_SWITCH_PRO_CTRL://大丈夫？
				case DX_PADTYPE_SWITCH_HORI_PAD://大丈夫？
				case DX_PADTYPE_XBOX_360://大丈夫？
				case DX_PADTYPE_XBOX_ONE://大丈夫？
					GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
					this->m_MouseActive.Execute((input.Buttons[9] != 0)/**/);
					break;
				default:
					break;
				}
			}
			else {//キーボード
				this->m_MouseActive.Execute(CheckHitKeyWithCheck(KEY_INPUT_TAB) != 0);
			}

			if (GetIsFirstLoop() || this->m_MouseActive.trigger()) {
				if (!this->m_MouseActive.on()) {
					select = 0;
				}
			}

			Pad->Execute(
				[&]() {
				auto* KeyGuide = FPS_n2::KeyGuideClass::Instance();
				if (!this->m_MouseActive.on()) {
					KeyGuide->Reset();
					KeyGuide->AddGuide("ng.png", "決定");
					KeyGuide->AddGuide("ok.png", "戻る");
					KeyGuide->AddGuide("R_stick.png", "上下選択,調整");
				}
				else {
					KeyGuide->Reset();
					KeyGuide->AddGuide("R_stick.png", "機首下上(ピッチ),左右回転(ロール)");
					KeyGuide->AddGuide("L1.png", "");
					KeyGuide->AddGuide("R1.png", "左右傾け(ヨー)");
					KeyGuide->AddGuide("square.png", "");
					KeyGuide->AddGuide("triangle.png", "加減速");
					KeyGuide->AddGuide("R2.png", "射撃");
					KeyGuide->AddGuide("L_stick.png", "押し込みでフリールック");
					KeyGuide->AddGuide("L2.png", "エイム");
					KeyGuide->AddGuide("option.png", "ポーズ");
				}
			},
				[&]() {
				auto* KeyGuide = FPS_n2::KeyGuideClass::Instance();
				if (!this->m_MouseActive.on()) {
					KeyGuide->Reset();
					KeyGuide->AddGuide("none.jpg", "決定");
					KeyGuide->AddGuide("X.jpg", "戻る");
					KeyGuide->AddGuide("W.jpg", "");
					KeyGuide->AddGuide("S.jpg", "上下選択");
					KeyGuide->AddGuide("A.jpg", "");
					KeyGuide->AddGuide("D.jpg", "調整");
				}
				else {
					KeyGuide->Reset();
					KeyGuide->AddGuide("W.jpg", "");
					KeyGuide->AddGuide("S.jpg", "機首下上(ピッチ)");
					KeyGuide->AddGuide("A.jpg", "");
					KeyGuide->AddGuide("D.jpg", "左右回転(ロール)");
					KeyGuide->AddGuide("Q.jpg", "");
					KeyGuide->AddGuide("E.jpg", "左右傾け(ヨー)");
					KeyGuide->AddGuide("R.jpg", "");
					KeyGuide->AddGuide("F.jpg", "加減速");
					KeyGuide->AddGuide("LM.jpg", "射撃");
					KeyGuide->AddGuide("MM.jpg", "フリールック");
					KeyGuide->AddGuide("RM.jpg", "エイム");
					KeyGuide->AddGuide("Tab.jpg", "ポーズ");
				}
			},
				&this->m_MouseActive);

			*isPause = !this->m_MouseActive.on();
			if (*isPause) {
				auto* SE = SoundPool::Instance();
				if (!OptionWindowClass::Instance()->IsActive()) {
					if (Pad->GetUpKey().trigger()) {
						--select;
						if (select < 0) { select = 2; }
						SelYadd[select] = 10.f;

						SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (Pad->GetDownKey().trigger()) {
						++select;
						if (select > 2) { select = 0; }
						SelYadd[select] = -10.f;

						SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					for (int i = 0; i < 3; i++) {
						Easing(&SelYadd[i], 0.f, 0.95f, EasingType::OutExpo);
					}
					if (Pad->GetOKKey().trigger()) {
						SE->Get((int)SoundEnum::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						switch (select) {
						case 0:
							m_GameEnd = true;
							this->m_MouseActive.Execute(true);
							break;
						case 1:
							OptionWindowClass::Instance()->SetActive(true);
							break;
						case 2:
							this->m_MouseActive.Execute(true);
							break;
						default:
							this->m_MouseActive.Execute(true);
							break;
						}
					}
					if (Pad->GetNGKey().trigger()) {
						SE->Get((int)SoundEnum::UI_NG).Play(0, DX_PLAYTYPE_BACK, TRUE);
						this->m_MouseActive.Execute(true);
					}
				}
				return true;
			}
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("update start");
#endif // DEBUG
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* SE = SoundPool::Instance();
			auto* OptionParts = OPTION::Instance();
#ifdef DEBUG
			//auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
				//FirstDoingv
			if (GetIsFirstLoop()) {
				SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
				if (g_Mode == 0) {
					this->m_BGM.play(DX_PLAYTYPE_BACK, FALSE);
				}
				SE->Get((int)SoundEnum::Env).Play(0, DX_PLAYTYPE_LOOP, TRUE);
				//auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				//Chara->LoadReticle();//プレイヤー変更時注意
				fov_base = GetMainCamera().GetCamFov();
				StartTimer = 3.f;
				Timer = 0.f;
				EndTimer = 3.f;

				m_ResultXofs = -2000.f;
				m_ResultColor = 0.f;
			}
			else {
				StartTimer = std::max(StartTimer - 1.f / FPS, 0.f);
				if (StartTimer == 0) {
					Timer += 1.f / FPS;
				}
				if (g_Mode == 0) {
					this->m_BGM.vol((int)((float)Lerp(128, 255, std::powf(std::clamp(Timer / TotalTime, 0.f, 1.f), 2.f))*OptionParts->Get_BGM()));
					SE->SetVol(Lerp(1.f, 0.f, std::powf(std::clamp(Timer / TotalTime, 0.f, 1.f), 2.f))*OptionParts->Get_SE());
					if (m_ResultXofs > -5.f) {
						this->m_BGM.vol((int)(128.f*m_ResultColor));
					}
				}
			}
			{
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				m_GameEnd |= (!(Chara->GetHP() != 0 && (Timer <= TotalTime)));
				if (m_GameEnd) {
					EndTimer = std::max(EndTimer - 1.f / FPS, 0.f);
				}
			}
			//Input,AI
			{
				float cam_per = ((GetMainCamera().GetCamFov() / deg2rad(75)) / (is_lens() ? zoom_lens() : 1.f)) / 100.f;
				if (this->m_LookMode != 0) {
					cam_per *= 0.2f;
				}
				float pp_x = 0.f, pp_y = 0.f;
				InputControl MyInput;
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				//
				this->m_TPS_Xrad += std::clamp(-Pad->GetLS_Y()*0.5f, -9.f, 9.f) / 100.f;
				this->m_TPS_Yrad += std::clamp(Pad->GetLS_X()*0.5f, -9.f, 9.f) / 100.f;
				//
				pp_x = Pad->GetLS_Y() * cam_per;
				pp_y = Pad->GetLS_X() * cam_per;
				if (Pad->GetMouseWheel().press()) {
					pp_x *= 2.f;
					pp_y *= 2.f;
				}
				//オートエイム
				if (m_AutoAimSel != -1) {
					if (0.f < m_AimPoint.z() && m_AimPoint.z() < 1.f) {
						auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Plane, m_AutoAimSel));
						pp_x += -(float)(c->GetCameraPosition().y() - m_AimPoint.y()) * 0.0001f * 60.f / FPS;
						pp_y += (float)(c->GetCameraPosition().x() - m_AimPoint.x()) * 0.0003f * 60.f / FPS;
					}
				}
				MyInput.SetInput(
					pp_x*(1.f - this->m_FreeLook_Per),
					pp_y*(1.f - this->m_FreeLook_Per),
					Pad->GetUpKey().press(), Pad->GetDownKey().press(), Pad->GetLeftKey().press(), Pad->GetRightKey().press(),
					false,
					Pad->GetQKey().press(), Pad->GetEKey().press(),
					false, false, false, false,
					Pad->GetOKKey().press(),
					Pad->GetAccelKey().press(),
					Pad->GetBrakeKey().press(),
					false,
					(Pad->GetShotKey().press() && this->m_MouseActive.on()),
					(Pad->GetLookKey().press() && this->m_MouseActive.on())
				);


				//一番前を狙う
				auto AimFront = [&]() {
					//前にいる一番狙いやすい敵を狙う

					auto Mypos = Chara->GetMove().pos;
					auto vec_mat = Chara->GetMove().mat;
					auto vec_z = vec_mat.zvec() * -1.f;

					float dot_t = 0.f;
					int tmp_id = -1;
					for (auto& c : this->character_Pool) {
						auto vec = (c->GetMove().pos - Mypos);
						auto len = vec.size();
						auto dot = vec_z.dot(vec.Norm());
						if (&Chara == &c ||
							c->GetHP() == 0 ||
							Chara->GetCharaType() == c->GetCharaType() ||
							c->aim_cnt >= 1 ||
							len >= 350.f*Scale_Rate
							) {
							continue;
						}
						if (dot_t < dot) {
							dot_t = dot;
							tmp_id = (int)(&c - &(this->character_Pool[0]));
						}
					}
					return tmp_id;
				};
				auto AimBack = [&]() {
					//後方にいる一番近い敵を狙う

					auto Mypos = Chara->GetMove().pos;
					auto vec_mat = Chara->GetMove().mat;
					auto vec_z = vec_mat.zvec() * -1.f;

					float len_t = 1000.f*Scale_Rate;
					int tmp_id = -1;
					for (auto& c : this->character_Pool) {
						auto vec = (c->GetMove().pos - Mypos);
						auto len = vec.size();
						auto dot = vec_z.dot(vec.Norm());
						if (&Chara == &c ||
							c->GetHP() == 0 ||
							Chara->GetCharaType() == c->GetCharaType() ||
							c->aim_cnt >= 1 ||
							len >= 350.f*Scale_Rate ||
							dot > 0.f
							) {
							continue;
						}
						if (len_t > len) {
							len_t = len;
							tmp_id = (int)(&c - &(this->character_Pool[0]));
						}
					}
					return tmp_id;
				};

				this->m_LookOn = -1;
				this->m_LookMode = Pad->GetMouseWheel().press() ? 1 : 0;


				if (Pad->GetLookKey().press() && this->m_MouseActive.on()) {
					this->m_LookMode = 0;
				}
				Easing(&this->m_FreeLook_Per, ((this->m_LookMode != 0)) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				Easing(&this->m_Aim_Per, (Pad->GetLookKey().press() && this->m_MouseActive.on()) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);

				this->m_TPS_Xrad = std::clamp(this->m_TPS_Xrad, deg2rad(-60), deg2rad(60));
				if (this->m_TPS_Yrad >= deg2rad(180)) { this->m_TPS_Yrad -= deg2rad(360); }
				if (this->m_TPS_Yrad <= deg2rad(-180)) { this->m_TPS_Yrad += deg2rad(360); }

				this->m_TPS_Xrad *= this->m_FreeLook_Per;
				this->m_TPS_Yrad *= this->m_FreeLook_Per;
				Easing(&this->m_TPS_XradR, m_TPS_Xrad, 0.5f, EasingType::OutExpo);

				this->m_TPS_YradR += (sin(this->m_TPS_Yrad)*cos(this->m_TPS_YradR) - cos(this->m_TPS_Yrad) * sin(this->m_TPS_YradR))*20.f / FPS;
				MyInput.SetRadBuf(PlayerMngr->GetPlayer(GetMyPlayerID()).GetRadBuf());
				//ネットワーク
				m_NetWorkBrowser.FirstExecute(MyInput, PlayerMngr->GetPlayer(GetMyPlayerID()).GetNetSendMove());
				//クライアント
				if (m_NetWorkBrowser.GetClient()) {
					for (auto& c : this->character_Pool) {
						if (c->GetMyPlayerID() == GetMyPlayerID()) {
							c->SetUseRealTimePhysics(false);
							//c->SetUseRealTimePhysics(true);
							c->SetCharaType(CharaTypeID::Team);
						}
						else {
							c->SetUseRealTimePhysics(false);
							if (c->GetMyPlayerID() < Chara_num * 1 / 3) {
								c->SetCharaType(CharaTypeID::Team);
							}
							else {
								c->SetCharaType(CharaTypeID::Enemy);
							}
						}
					}
				}
				//
				for (int i = 0; i < Chara_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Plane, i));
					c->aim_cnt = 0;
				}
				for (int i = 0; i < Chara_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Plane, i));
					bool isready = (StartTimer == 0.f) && c->GetHP() > 0;

					if (m_NetWorkBrowser.GetSequence() == SequenceEnum::MainGame) {
						auto tmp = this->m_NetWorkBrowser.GetNowServerPlayerData(i, false);
						if (i == GetMyPlayerID()) {
							MyInput.SetKeyInput(tmp.Input.GetKeyInput());//キーフレームだけサーバーに合わせる
							c->SetInput(MyInput, isready);
							m_NetWorkBrowser.GetRecvData(i, tmp.Frame);
						}
						else {
							if (!m_NetWorkBrowser.GetClient()) {
								m_AICtrl[i]->AI_move(&tmp.Input);
							}
							c->SetInput(tmp.Input, isready);
							bool override_true = true;
							override_true = (tmp.CalcCheckSum() != 0);
							if (override_true) {
								c->SetPosBufOverRide(tmp.PosBuf, tmp.VecBuf, tmp.radBuf);
							}

						}
						//ダメージイベント処理
						if (ObjMngr->GetObj(ObjType::Plane, i) != nullptr) {
							if (tmp.DamageSwitch != c->GetDamageSwitchRec()) {
								this->m_DamageEvents.emplace_back(tmp.Damage);
								c->SetDamageSwitchRec(tmp.DamageSwitch);
							}
						}
					}
					else {
						if (i == GetMyPlayerID()) {
							c->SetInput(MyInput, isready);
						}
						else {
							InputControl OtherInput;
							m_AICtrl[i]->AI_move(&OtherInput);//めっちゃ重い
							c->SetInput(OtherInput, isready);
						}
						//ダメージイベント処理
						if (ObjMngr->GetObj(ObjType::Plane, i) != nullptr) {
							if (c->GetDamageSwitch() != c->GetDamageSwitchRec()) {
								this->m_DamageEvents.emplace_back(c->GetDamageEvent());
								c->SetDamageSwitchRec(c->GetDamageSwitch());
							}
						}
					}
				}
				m_NetWorkBrowser.LateExecute();
				//ダメージイベント
				for (auto& c : this->character_Pool) {
					for (int j = 0; j < this->m_DamageEvents.size(); j++) {
						if (c->SetDamageEvent(this->m_DamageEvents[j])) {
							std::swap(this->m_DamageEvents.back(), m_DamageEvents[j]);
							this->m_DamageEvents.pop_back();
							j--;
						}
					}
				}
			}
			//Execute
			ObjMngr->ExecuteObject();
			//いらないオブジェクトの除去
			ObjMngr->DeleteCheck();
			//弾の更新
			{
				for (int i = 0; i < Chara_num; i++) {
					auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Plane, i));
					c->m_NearAmmo = false;
				}

				int loop = 0;
				while (true) {
					auto ammo = ObjMngr->GetObj(ObjType::Ammo, loop);
					if (ammo != nullptr) {
						auto& a = (std::shared_ptr<AmmoClass>&)(*ammo);
						if (a->IsActive()) {
							//AmmoClass
							VECTOR_ref pos_tmp = a->GetMove().pos;
							VECTOR_ref norm_tmp;
							auto ColResGround = a->ColCheckGround(&norm_tmp);
							bool is_HitAll = false;
							auto& c = *ObjMngr->GetObj(ObjType::Plane, a->GetShootedID());//(std::shared_ptr<CharacterClass>&)
							for (auto& tgt : this->character_Pool) {
								if (tgt->GetHP() == 0) { continue; }
								if (tgt->GetMyPlayerID() == a->GetShootedID()) { continue; }
								auto Pos = a->GetMove().pos;
								auto res = tgt->CheckLineHit(a->GetMove().repos, &Pos);
								is_HitAll |= res;
								if (GetMinLenSegmentToPoint(a->GetMove().repos, a->GetMove().pos, tgt->GetMove().pos) <= 20.0f*Scale_Rate) {
									tgt->m_NearAmmo = true;
								}
								if (res) {
									a->Penetrate();
									if (Timer <= TotalTime) {
										tgt->HitDamage(c->GetMatrix().pos(), a->GetDamage());
										if (tgt->GetHP() <= a->GetDamage()) {
											PlayerMngr->GetPlayer(a->GetShootedID()).AddScore(1);
										}
									}
									EffectControl::SetOnce_Any((EffectResource::Effect)2, a->GetMove().pos, a->GetMove().vec.Norm()*-1.f, 1.f * Scale_Rate);
									break;
								}
							}
							if (ColResGround && !is_HitAll) {
								a->HitGround(pos_tmp);
								EffectControl::SetOnce_Any((EffectResource::Effect)2, a->GetMove().pos, norm_tmp, a->GetCaliberSize() / 0.02f * Scale_Rate);
							}
						}
					}
					else {
						break;
					}
					loop++;
				}
			}
			//背景更新
			this->m_BackGround->FirstExecute();
			ObjMngr->LateExecuteObject();
			//首の向き
			{
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(WatchSelect).GetChara();//
				MATRIX_ref FreeLook = MATRIX_ref::RotX(std::clamp(m_TPS_XradR, deg2rad(-20), deg2rad(20))) * MATRIX_ref::RotY(m_TPS_YradR);
				Chara->SetCamEyeVec(FreeLook.zvec()*-1.f);
			}
			//視点
			{
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(WatchSelect).GetChara();//
				if (Chara->GetSendCamShake()) {
					this->m_CamShake = 0.2f;
				}
				this->m_CamShake = 0.1f * std::clamp(Chara->GetAccel(), 0.f, 1.f);

				Easing(&this->m_CamShake1, VECTOR_ref::vget(GetRandf(this->m_CamShake), GetRandf(this->m_CamShake), GetRandf(this->m_CamShake))*2.f, 0.8f, EasingType::OutExpo);
				Easing(&this->m_CamShake2, m_CamShake1, 0.8f, EasingType::OutExpo);
				this->m_CamShake = std::max(this->m_CamShake - 1.f / FPS, 0.f);

				if (!m_GameEnd) {
					MATRIX_ref FreeLook = MATRIX_ref::RotX(-m_TPS_XradR) * MATRIX_ref::RotY(m_TPS_YradR);
					VECTOR_ref tmp = MATRIX_ref::Vtrans(FreeLook.zvec()*-1.f, m_TPSLookMat);
					if (this->m_LookOn != -1) {
						auto& c = this->character_Pool.at(this->m_LookOn);
						tmp = (c->GetMatrix().pos() - Chara->GetMatrix().pos()).Norm();
					}
					Easing(&m_FreeLookVec, tmp, 0.5f, EasingType::OutExpo);

					Easing_Matrix(&m_TPSLookMat, Chara->GetMatrix().GetRot(), 0.9f, EasingType::OutExpo);

					VECTOR_ref CamUp = m_TPSLookMat.yvec();
					VECTOR_ref CamVec = Lerp(m_TPSLookMat.zvec() * -1.f, m_FreeLookVec, this->m_FreeLook_Per);
					VECTOR_ref CamPos = Chara->GetMatrix().pos() + CamUp * Lerp(2.f, 6.f, this->m_FreeLook_Per)*Scale_Rate;

					SetMainCamera().SetCamPos(
						CamPos + CamVec * -Lerp(8.f, 12.f, this->m_FreeLook_Per)*Scale_Rate + this->m_CamShake2 * 10.f,
						Chara->GetMatrix().pos() + CamVec * Lerp(50.f*Scale_Rate, 100.f*Scale_Rate, this->m_Aim_Per),
						CamUp);
				}
				else {
					VECTOR_ref CamPos = GetMainCamera().GetCamPos();
					VECTOR_ref CamVec = GetMainCamera().GetCamVec();
					VECTOR_ref CamUp = GetMainCamera().GetCamUp();

					CamPos.yadd(2.f*60.f / FPS);
					Easing(&CamVec, Chara->GetMatrix().pos(), 0.9f, EasingType::OutExpo);
					Easing(&CamUp, VECTOR_ref::up(), 0.9f, EasingType::OutExpo);

					SetMainCamera().SetCamPos(
						CamPos,
						CamVec,
						CamUp);
				}

				auto fov_t = GetMainCamera().GetCamFov();
				auto near_t = GetMainCamera().GetCamNear();
				auto far_t = GetMainCamera().GetCamFar();
				Easing(&near_t, Scale_Rate * 1.f, 0.9f, EasingType::OutExpo);
				Easing(&far_t, Scale_Rate * 500.f, 0.9f, EasingType::OutExpo);

				if (Pad->GetLookKey().press() && this->m_MouseActive.on()) {
					Easing(&fov_t, deg2rad(25), 0.9f, EasingType::OutExpo);
				}
				else {
					auto SpeedNormal = 51.39f*0.5f;//秒速換算 92.5km/hくらい
					Easing(&fov_t, deg2rad(60) * Lerp((1.f + (std::clamp(Chara->GetSpeed() / (SpeedNormal*3.6f), 0.85f, 1.5f) - 1.f)*0.5f), 1.f, this->m_FreeLook_Per), 0.9f, EasingType::OutExpo);
				}
				SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
			}
			{
				if (CheckHitKeyWithCheck(KEY_INPUT_0) != 0) { WatchSelect = GetMyPlayerID(); }
				if (CheckHitKeyWithCheck(KEY_INPUT_1) != 0) { WatchSelect = 1; }
				if (CheckHitKeyWithCheck(KEY_INPUT_2) != 0) { WatchSelect = 2; }
				if (CheckHitKeyWithCheck(KEY_INPUT_3) != 0) { WatchSelect = 3; }
				if (CheckHitKeyWithCheck(KEY_INPUT_4) != 0) { WatchSelect = 4; }
				if (CheckHitKeyWithCheck(KEY_INPUT_5) != 0) { WatchSelect = 5; }
				if (CheckHitKeyWithCheck(KEY_INPUT_6) != 0) { WatchSelect = 6; }
				if (CheckHitKeyWithCheck(KEY_INPUT_7) != 0) { WatchSelect = 7; }
				if (CheckHitKeyWithCheck(KEY_INPUT_8) != 0) { WatchSelect = 8; }
				if (CheckHitKeyWithCheck(KEY_INPUT_9) != 0) { WatchSelect = 9; }
				//
				if (WatchSelect != GetMyPlayerID()) {
					auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(WatchSelect).GetChara();//
					if (Chara->GetHP() == 0) {
						for (auto& c : this->character_Pool) {
							if ((c->GetMyPlayerID() != GetMyPlayerID()) && (c->GetHP() != 0)) {
								WatchSelect = c->GetMyPlayerID();
								break;
							}
						}
					}
				}
			}
			this->m_BackGround->Execute();
			//オートエイム
			if (m_AutoAimSel != -1) {
				auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Plane, m_AutoAimSel));
				if (c->GetHP() == 0) {
					m_AutoAimSel = -1;
					m_AutoAimTimer = 0.f;
				}
			}
			if (m_AutoAimTimer == 0.f) {
				m_AutoAimSel = -1;
			}
			m_AutoAimTimer = std::max(m_AutoAimTimer - 1.f / FPS, 0.f);
			if (Pad->GetLookKey().press() && this->m_MouseActive.on()) {
				if (0.f < m_AimPoint.z() && m_AimPoint.z() < 1.f) {
					int select_t = -1;
					float len = 10000.f*Scale_Rate;
					auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
					auto Start = Chara->GetMatrix().pos();
					auto End = Start + Chara->GetMatrix().zvec()*-1.f *300.f*Scale_Rate;
					for (int i = 0; i < Chara_num; i++) {
						auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Plane, i));
						if (c->GetHP() == 0) { continue; }
						if (!(Chara->GetCharaType() != c->GetCharaType())) { continue; }
						if (!(0.f < c->GetCameraPosition().z() && c->GetCameraPosition().z() < 1.f)) { continue; }
						auto Target = c->GetMatrix().pos();
						auto tmp = (Target - Start).Length();
						if (tmp < 100.f*Scale_Rate) {
							auto Len = std::hypotf((float)(c->GetCameraPosition().x() - m_AimPoint.x()), (float)(c->GetCameraPosition().y() - m_AimPoint.y()));
							if (Len < 200) {
								if (len > Len) {
									len = Len;
									select_t = i;
								}
							}
						}
					}
					if (select_t != -1) {
						m_AutoAimSel = select_t;
						m_AutoAimTimer = 3.f;
					}
					//
				}
			}
#ifdef DEBUG
			DebugParts->SetPoint("---");
#endif // DEBUG
			//UIパラメーター
			{
				auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
				//Time,Score
				this->m_UIclass.SetfloatParam(0, TotalTime - Timer);
				this->m_UIclass.SetIntParam(6, PlayerMngr->GetPlayer(0).GetScore());
				//HP
				this->m_UIclass.SetIntParam(3, (int)Chara->GetHP());
				this->m_UIclass.SetIntParam(4, (int)Chara->GetHPMax());
				this->m_UIclass.SetIntParam(5, (int)(this->m_HPBuf + 0.5f));
				this->m_HPBuf += std::clamp((Chara->GetHP() - this->m_HPBuf)*20.f, -15.f, 15.f) / FPS;
				//SPeed,ALT

				this->m_UIclass.SetIntParam(0, (int)(this->m_CamShake2.x()*100.f - Chara->GetYaw()*100.f));
				this->m_UIclass.SetIntParam(1, (int)(this->m_CamShake2.y()*100.f + Chara->GetPitch()*100.f));
				this->m_UIclass.SetIntParam(2, (int)(Chara->GetRoll()*100.f));

				Easing(&m_DrawSpeed, Chara->GetSpeed(), 0.9f, EasingType::OutExpo);

				this->m_UIclass.SetfloatParam(3, m_DrawSpeed);
				this->m_UIclass.SetfloatParam(4, Chara->GetMove().pos.y() / Scale_Rate);
				if (SpeedUpdateTime <= 0.f) {
					this->m_UIclass.SetfloatParam(1, m_DrawSpeed);
					this->m_UIclass.SetfloatParam(2, Chara->GetMove().pos.y() / Scale_Rate);
					SpeedUpdateTime = 0.25f;
				}
				else {
					SpeedUpdateTime -= 1.f / FPS;
				}

				this->m_UIclass.SetItemGraph(0, &Gauge_Graph);
				this->m_UIclass.SetItemGraph(1, &Gauge_Graph);

			}
			EffectControl::Execute();
			//
			for (auto& c : this->character_Pool) {
				VECTOR_ref campos; campos.z(-1.f);
				c->SetCameraPosition(campos);
			}
			m_AimPoint.z(-1.f);
			//
#ifdef DEBUG
			DebugParts->SetPoint("update end");
#endif // DEBUG
			if (g_Mode == 1) {
				if (EndTimer == 0.f) {
					return false;
				}
			}
			else {
				if (EndTimer == 0.f) {
					if (CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0) {
						m_ResultXofs = 0.f;
					}
				}
				if (m_ResultXofs > -5.f) {
					if (m_ResultColor == 0.f) {
						auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
						if (!(Chara->GetHP() != 0 && (Timer <= TotalTime))) {
							int Kill = (int)(PlayerMngr->GetPlayer(0).GetScore());
							FPS_n2::SaveDataClass::Instance()->SetParam("KillCount", FPS_n2::SaveDataClass::Instance()->GetParam("KillCount") + Kill);
						}
						return false;
					}
				}
			}
			return true;
		}
		void			MAINLOOP::Dispose_Sub(void) noexcept {
			if (g_Mode == 0) {
				this->m_BGM.stop();
			}

			auto* SE = SoundPool::Instance();
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto* OptionParts = OPTION::Instance();

			SE->Get((int)SoundEnum::Env).StopAll(0);
			SE->SetVol(OptionParts->Get_SE());

			for (auto& c : character_Pool) {
				c.reset();
			}
			character_Pool.clear();

			m_AICtrl.clear();
			m_NetWorkBrowser.Dispose();
			EffectControl::Dispose();
			PlayerMngr->Dispose();
			ObjMngr->DisposeObject();
			this->m_BackGround->Dispose();
			this->m_BackGround.reset();
		}

		void			MAINLOOP::ShadowDraw_NearFar_Sub(void) noexcept{
			this->m_BackGround->Shadow_Draw_NearFar();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject_Shadow();
		}
		void			MAINLOOP::ShadowDraw_Sub(void) noexcept{
			auto* ObjMngr = ObjectManager::Instance();

			//this->m_BackGround->Shadow_Draw();
			ObjMngr->DrawObject_Shadow();
		}
		void			MAINLOOP::MainDraw_Sub(void) noexcept{

			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
			SetFogStartEnd(Scale_Rate * 1200.f, Scale_Rate * 1600.f);
			this->m_BackGround->Draw();

			ObjMngr->DrawObject();

			//ObjMngr->DrawDepthObject();
			//シェーダー描画用パラメーターセット
			{
				//
				Set_is_Blackout(true);
				Set_Per_Blackout(0.5f + (1.f + sin(Chara->GetHeartRateRad()*4.f)*0.25f) * ((Chara->GetHeartRate() - 60.f) / (180.f - 60.f)));
				//
				Set_is_lens(false);
			}
			for (auto& c : this->character_Pool) {
				if (c->GetMyPlayerID() == GetMyPlayerID()) { continue; }
				//auto pos = c->GetFrameWorldMat(CharaFrame::Upper).pos();
				auto pos = c->GetMove().pos + c->GetMove().mat.zvec()*-1.f * 5.f*Scale_Rate;
				VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
				if (0.f < campos.z() && campos.z() < 1.f) {
					c->SetCameraPosition(campos);
					c->SetCameraSize(std::max(20.f / ((pos - GetCameraPosition()).size() / 2.f), 0.2f));
				}
			}
			if (Chara->GetHP() != 0) {
				auto pos = Chara->GetMatrix().pos() + Chara->GetMatrix().zvec()*-1.f*100.f*Scale_Rate;
				VECTOR_ref campos = ConvWorldPosToScreenPos(pos.get());
				if (0.f < campos.z() && campos.z() < 1.f) {
					m_AimPoint = campos;
				}
			}
			if (false)
			{
				for (int i = 0; i < Chara_num; i++) {
					m_AICtrl[i]->Draw();
				}
			}
			//
			/*
			if (this->m_FreeLook_Per > 0.1f) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*this->m_FreeLook_Per), 0, 255));
				SetUseLighting(FALSE);
				int dev = 36;

				int devX = 3;
				for (int Xdeg = -devX; Xdeg <= devX; Xdeg++) {
					float Xrad = deg2rad(Xdeg * 90 / devX);
					for (int deg = 0; deg < dev; deg++) {
						float prev_rad = deg2rad((deg - 1) * 360 / dev);
						float now_rad = deg2rad(deg * 360 / dev);

						VECTOR_ref prev = (MATRIX_ref::RotX(Xrad)*MATRIX_ref::RotY(prev_rad)).zvec();
						VECTOR_ref now = (MATRIX_ref::RotX(Xrad)*MATRIX_ref::RotY(now_rad)).zvec();
						//
						prev = prev * (5.f*Scale_Rate) + GetCameraPosition();
						now = now * (5.f*Scale_Rate) + GetCameraPosition();
						DrawCapsule_3D(prev, now, 0.01f*Scale_Rate, GetColor(0, 255, 0), GetColor(0, 255, 0));
					}
				}
				SetUseLighting(TRUE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//*/
		}
		void			MAINLOOP::MainDrawbyDepth_Sub(void) noexcept{
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawDepthObject();
		}
		//UI表示
		void			MAINLOOP::DrawUI_Base_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();
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
			{
				auto per = std::clamp(std::abs((float)(Chara->GetHP() - this->m_HPBuf))*0.1f, 0.f, 0.5f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

				OIL_Graph.DrawExtendGraph(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, true);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			//UI
			this->m_UIclass.Draw();
			//通信設定
			if (!this->m_MouseActive.on()) {
				//m_NetWorkBrowser.Draw();
			}
			//ポーズ
			if (!this->m_MouseActive.on()) {
				{
					auto per = std::clamp(0.7f, 0.f, 1.f);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

					DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);

					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				int xp1, yp1;
				//auto* Fonts = FontPool::Instance();
				//auto Red = GetColor(255, 0, 0);
				//auto Red75 = GetColor(192, 0, 0);
				//auto White = GetColor(255, 255, 255);
				//auto Gray75 = GetColor(128, 128, 128);
				//auto Gray = GetColor(64, 64, 64);

				xp1 = y_r(1920 - 256 - 54 * 2);
				yp1 = y_r(1080 - 108 - 108 * 2 + (int)SelYadd[0]);
				DrawFetteString(xp1, yp1, 1.f, (select == 0), "Retire");

				xp1 = y_r(1920 - 256 - 54 * 1);
				yp1 = y_r(1080 - 108 - 108 * 1 + (int)SelYadd[1]);
				DrawFetteString(xp1, yp1, 1.f, (select == 1), "Option");

				xp1 = y_r(1920 - 256);
				yp1 = y_r(1080 - 108 + (int)SelYadd[2]);
				DrawFetteString(xp1, yp1, 1.f, (select == 2), "Return Game");
			}
			//
		}
		void			MAINLOOP::DrawUI_In_Sub(void) noexcept{
			auto* ObjMngr = ObjectManager::Instance();
			auto* DrawParts = DXDraw::Instance();
			auto* Fonts = FontPool::Instance();

			auto* PlayerMngr = PlayerManager::Instance();
			auto& Chara = (std::shared_ptr<CharacterClass>&)PlayerMngr->GetPlayer(GetMyPlayerID()).GetChara();

			if (m_AutoAimSel != -1) {
				auto& c = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Plane, m_AutoAimSel));
				if (0.f < c->GetCameraPosition().z() && c->GetCameraPosition().z() < 1.f) {
					Enemyaimpoint_Graph.DrawRotaGraph(
						(int)c->GetCameraPosition().x(),
						(int)c->GetCameraPosition().y(),
						200.f*2.f / 256.f*this->m_Aim_Per, m_AimRot, true);
				}
			}
			if (0.f < m_AimPoint.z() && m_AimPoint.z() < 1.f) {
				//SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*this->m_Aim_Per), 0, 255));
				aim_Graph.DrawRotaGraph((int)m_AimPoint.x(), (int)m_AimPoint.y(), 200.f*2.f / 256.f*std::clamp(this->m_Aim_Per, 0.3f, 1.f), m_AimRot, true);
				//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			m_AimRot += deg2rad(60.f)*60.f / FPS;

			//後ろの敵
			{
				auto CamVec = (GetMainCamera().GetCamVec() - GetMainCamera().GetCamPos()).Norm();
				auto CamX = GetMainCamera().GetCamUp().cross(CamVec);

				for (auto& c : this->character_Pool) {
					if (c->GetHP() == 0) { continue; }
					if (c->GetCharaType() != CharaTypeID::Enemy) { continue; }
					if (!(0.f < c->GetCameraPosition().z() && c->GetCameraPosition().z() < 1.f)) {
						auto Vec = (c->GetMove().pos - GetMainCamera().GetCamPos());
						auto x = CamVec.cross(Vec).dot(CamX);
						auto y = CamVec.dot(Vec);

						float len = std::clamp(Vec.Length() / (300.f*Scale_Rate), 0.f, 1.f)*256.f;
						float rad = std::atan2f(-x, y);
						Enemy_Graph.DrawRotaGraph(DrawParts->m_DispXSize / 2 + (int)(sin(rad)*len), DrawParts->m_DispYSize / 2 - (int)(cos(rad)*len), 1.f*(float)y_r(100) / 100.f, rad, true);
					}
				}
			}
			//
			{
				auto per = std::clamp((StartTimer - 2.f)*2.f, 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			{
				auto per = std::clamp((1.f - EndTimer)*2.f, 0.f, 1.f);

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(16, 8, 8), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

				if (per >= 1.f) {
					if (g_Mode == 0) {
						int Kill = (int)(PlayerMngr->GetPlayer(0).GetScore());
						int Rank = 3;
						if (Kill >= 1) {
							Rank = 2;
						}
						if (Kill >= 5 && (Chara->GetHP() != 0)) {
							Rank = 1;
						}
						if (Kill >= 8) {
							Rank = 0;
						}

						{
							int xp1, yp1;
							//auto White = GetColor(255, 255, 255);
							//auto Gray75 = GetColor(128, 128, 128);
							auto Gray = GetColor(64, 64, 64);


							xp1 = DrawParts->m_DispXSize / 2;
							yp1 = DrawParts->m_DispYSize / 2 - y_r(200);

							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*m_ResultColor), 0, 255));
							DrawFetteString(xp1 - y_r(100), yp1, 1.33f, false, "ReSult");
							yp1 += y_r(64 * 2);

							DrawFetteString(xp1, yp1, 1.33f, false, "Shoot Down");
							DrawFetteString(xp1 + y_r(100), yp1, 1.f, false, "%02d", Kill);
							yp1 += y_r(64 * 2);

							const char* RankStr[] = {
								"Red Baron",//8キル
								"Ace",		//5キルかつ生存
								"Sergeant",	//1キル以上
								"Newbie",	//0キル
							};
							unsigned int RankColorBack[] = {
								GetColor(128,0,0),
								GetColor(216,128,0),
								GetColor(128,128,128),
								GetColor(0,0,128),
							};
							unsigned int RankColor[] = {
								GetColor(192,0,0),
								GetColor(255,192,0),
								GetColor(216,216,216),
								GetColor(0,0,192),
							};

							Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 3 / 2), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), RankColorBack[Rank], Gray, "Rank");
							Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48 * 2), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, RankColor[Rank], Gray, "Rank");
							Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(32 * 3 / 2), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40) + y_r(40), yp1 + y_r(20), RankColorBack[Rank], Gray, RankStr[Rank]);
							Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(32 * 2), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1, RankColor[Rank], Gray, RankStr[Rank]);
							yp1 += y_r(64 * 2);
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
						}
					}
					if (m_ResultXofs < -10.f) {
						m_ResultColor = std::clamp(m_ResultColor + 1.f / FPS, 0.f, 1.f);
					}
					if (m_ResultXofs > -5.f) {
						m_ResultColor = std::clamp(m_ResultColor - 1.f / FPS, 0.f, 1.f);
					}

					if (m_ResultXofs < -100.f) {
						Easing(&m_ResultXofs, 0.f, 0.9f, EasingType::OutExpo);
					}
					else {
						if (m_ResultXofs < -5.f) {
							m_ResultXofs += 10.f / FPS;
						}
						else {
							Easing(&m_ResultXofs, 0.f, 0.9f, EasingType::OutExpo);
						}
					}
				}
				else {
					if (Chara->GetHP() == 0) {
						m_ResultXofs = -400.f;
					}
				}
			}
		}
	};
};
