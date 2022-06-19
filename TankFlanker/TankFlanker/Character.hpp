#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CharacterClass : public ObjectBaseClass {
			//キャラパラメーター
			const float SpeedLimit{ 2.f };
			const float UpperTimerLimit = 3.f;
			const float HeartRateMin{ 60.f };//心拍数最小
			const float HeartRateMax{ 180.f };//心拍数最大
			const float StaminaMax{ 100.f };
		private:
			VECTOR_ref m_posBuf;
			std::array<float, 4> m_Vec;
			std::array<float, (int)CharaAnimeID::AnimeIDMax> m_AnimPerBuf;
			float m_MoveVector{ 0.f };
			float m_xrad_Add{ 0.f }, m_yrad_Add{ 0.f };
			VECTOR_ref m_rad_Buf, m_rad;
			float m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float m_RunTimer{ 0.f };
			float m_RunPer{ 0.f };
			float m_RunPer2{ 0.f }, m_PrevRunPer2{ 0.f };
			float m_SprintPer{ 0.f };
			float m_SlingPer{ 0.f };
			float m_SquatPer{ 0.f };
			float m_PronePer{ 0.f };
			float m_PronePer2{ 0.f };
			VECTOR_ref m_ProneNormal{ VECTOR_ref::up() };
			bool m_PronetoStanding{ false };
			bool m_TurnBody{ false };
			bool m_IsRun{ false };
			bool m_IsSprint{ false };
			bool m_Ready{ false };
			bool m_RunReady{ false };
			bool m_RunReadyFirst{ false };
			bool m_Running{ false };
			bool m_ReturnStand = false;
			CharaAnimeID UpperAnimSelect, PrevUpperAnimSel;
			CharaAnimeID BottomAnimSelect;
			MATRIX_ref m_RightWristRot;
			bool m_IsFirstLoop{ true };
			//スコア
			float m_Score{ 0.f };
			//銃
			float m_ReadyTimer{ 0.f };
			float m_ReadyPer{ 0.f };
			int m_ShotPhase{ 0 };
			float m_LoadAmmoPer{ 0.f };
			//入力
			switchs m_Squat;
			switchs m_Prone;
			bool m_Press_GoFront{ false };//
			bool m_Press_GoRear{ false };
			bool m_Press_GoLeft{ false };
			bool m_Press_GoRight{ false };
			bool m_Press_Shot{ false };
			bool m_Press_Aim{ false };
			//スタミナ
			float m_HeartRate{ HeartRateMin };//心拍数
			float m_HeartRate_r{ HeartRateMin };//心拍数
			float m_HeartRateRad{ 0.f };//呼吸Sin渡し
			float m_Stamina{ StaminaMax };
			bool m_CannotSprint{ false };
			//表情
			int m_Eyeclose{ 0 };
			float m_EyeclosePer{ 0.f };
			//銃
			std::shared_ptr<GunClass> m_Gun_Ptr{ nullptr };
			//サウンド
			SoundHandle m_RunFootL;
			SoundHandle m_RunFootR;
			SoundHandle m_Sliding;
			SoundHandle m_SlidingL;
			SoundHandle m_SlidingR;
			SoundHandle m_Standup;
			SoundHandle m_Breath;
			std::array<SoundHandle, 3> m_Heart;
			size_t m_HeartSESel;
		public://ゲッター
			const auto GetCharaDir(void) const noexcept { return this->Frames[(int)CharaFrame::Upper].second.mat * this->m_move.mat; }//プライベートで良い
			//
			void SetGunPtr(std::shared_ptr<GunClass>& pGunPtr) noexcept { this->m_Gun_Ptr = pGunPtr; }
			auto& GetGunPtr(void) noexcept { return this->m_Gun_Ptr; }
			void LoadReticle(void) noexcept {
				if (this->m_Gun_Ptr != nullptr) {
					this->m_Gun_Ptr->LoadReticle();
				}
			}
			const auto GetShotSwitch(void) const noexcept {
				if (this->m_Gun_Ptr != nullptr) {
					return this->m_Gun_Ptr->GetIsShot();
				}
				else {
					return false;
				}
			}
			const auto GetEyeVector(void) const noexcept {
				if (this->m_Gun_Ptr != nullptr) {
					return Leap(GetCharaDir().zvec(), this->m_Gun_Ptr->GetMatrix().zvec(), this->m_ReadyPer) * -1.f;
				}
				else {
					return GetCharaDir().zvec() * -1.f;
				}
			}
			const auto GetEyePosition(void) const noexcept { return (this->m_obj.frame(Frames[(int)CharaFrame::LeftEye].first) + this->m_obj.frame(Frames[(int)CharaFrame::RightEye].first)) / 2.f + this->GetEyeVector().Norm() * 0.5f; }
			const auto GetScopePos(void) noexcept {
				if (this->m_Gun_Ptr != nullptr) {
					return this->m_Gun_Ptr->GetScopePos();
				}
				else {
					return GetEyePosition();
				}
			}
			const auto GetLensPos(void) noexcept {
				if (this->m_Gun_Ptr != nullptr) {
					return this->m_Gun_Ptr->GetLensPos();
				}
				else {
					return VECTOR_ref::zero();
				}
			}
			const auto GetReticlePos(void) noexcept {
				if (this->m_Gun_Ptr != nullptr) {
					return this->m_Gun_Ptr->GetReticlePos();
				}
				else {
					return VECTOR_ref::zero();
				}
			}
			const auto GetLensPosSize(void) noexcept {
				if (this->m_Gun_Ptr != nullptr) {
					return this->m_Gun_Ptr->GetLensPosSize();
				}
				else {
					return VECTOR_ref::zero();
				}
			}
			const auto& GetReticle(void) noexcept { return this->m_Gun_Ptr->GetReticle(); }
			//
			void AddScore(float value) noexcept { this->m_Score += value; }
			void SetScore(float value) noexcept { this->m_Score = value; }
			const auto& GetScore(void) const noexcept { return this->m_Score; }
			const auto& GetIsRun(void) const noexcept { return this->m_IsRun; }
			const auto& GetIsSprint(void) const noexcept { return this->m_IsSprint; }
			const auto& GetHeartRate(void) const noexcept { return this->m_HeartRate; }
			const auto& GetHeartRateRad(void) const noexcept { return this->m_HeartRateRad; }
			const auto& GetStamina(void) const noexcept { return this->m_Stamina; }
			const auto& GetStaminaMax(void) const noexcept { return this->StaminaMax; }
			const auto GetFrameWorldMatrix(CharaFrame frame) const noexcept { return this->m_obj.GetFrameLocalWorldMatrix(Frames[(int)frame].first); }
			const auto GetIsProne(void) const noexcept { return this->m_Prone.on(); }
			const auto GetProneShotAnimSel(void) const noexcept { return (this->m_Prone.on()) ? CharaAnimeID::All_ProneShot : CharaAnimeID::Upper_Shot; }
			const auto GetProneCockingAnimSel(void) const noexcept { return (this->m_Prone.on()) ? CharaAnimeID::All_ProneCocking : CharaAnimeID::Upper_Cocking; }

			const auto GetProneAimAnimSel(void) const noexcept { return (this->m_Prone.on()) ? CharaAnimeID::All_Prone : CharaAnimeID::Upper_Aim; }
			const auto GetProneReloadStartAnimSel(void) const noexcept { return (this->m_Prone.on()) ? CharaAnimeID::All_Prone_ReloadStart : CharaAnimeID::Upper_ReloadStart; }
			const auto GetProneReloadOneAnimSel(void) const noexcept { return (this->m_Prone.on()) ? CharaAnimeID::All_Prone_ReloadOne : CharaAnimeID::Upper_ReloadOne; }
			const auto GetProneReloadEndAnimSel(void) const noexcept { return (this->m_Prone.on()) ? CharaAnimeID::All_Prone_ReloadEnd : CharaAnimeID::Upper_ReloadEnd; }

			const auto GetAmmoNum(void) const noexcept { return this->m_Gun_Ptr->GetAmmoNum(); }
			const auto GetAmmoAll(void) const noexcept { return this->m_Gun_Ptr->GetAmmoAll(); }

			const auto GetIsADS(void) const noexcept { return this->m_ReadyTimer == 0.f; }
		private:
			void SetAnimLoop(int ID, float speed) {
				this->m_obj.get_anime(ID).time += 30.f / FPS * speed;
				if (this->m_obj.get_anime(ID).TimeEnd()) { this->m_obj.get_anime(ID).time = 0.f; }
			}
			void SetVec(int pDir, bool Press) {
				this->m_Vec[pDir] += (Press ? 1.f : -1.f)*2.f / FPS;
				this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
			}
		private:
			void ExecuteAnim(void) noexcept {
				//アニメ演算
				{
					//上半身
					{
						PrevUpperAnimSel = UpperAnimSelect;
						UpperAnimSelect = CharaAnimeID::Upper_Down;
						bool canreverse = true;
						if (!this->m_RunReady && !this->m_Running && !this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).TimeEnd()) {
							this->m_RunReady = true;
							canreverse = false;
						}
						if (this->m_RunReadyFirst) {
							this->m_SlingPer = 0.f;
							this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time = 0.f;
						}
						if (this->m_RunReady) {
							if (!this->m_Running) {
								this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time += 30.f / FPS * 2.f;
								easing_set(&this->m_SlingPer, (canreverse && this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time > 16) ? 1.f : 0.f, 0.9f);
								this->m_SprintPer = 0.f;
								if (this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).TimeEnd()) {
									this->m_Running = true;
									this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time = this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).alltime;
									this->m_obj.get_anime((int)CharaAnimeID::Upper_Running).time = 0.f;
									if (canreverse) {
										this->m_SlingPer = 1.f;
									}
								}
								UpperAnimSelect = CharaAnimeID::Upper_RunningStart;
							}
							else if (!this->m_IsSprint) {
								UpperAnimSelect = CharaAnimeID::Upper_Running;
							}
							else {
								UpperAnimSelect = CharaAnimeID::Upper_Sprint;
							}
						}
						else {
							easing_set(&this->m_SlingPer, 0.f, 0.9f);
							this->m_Running = false;
						}
						if (this->m_ReadyTimer < UpperTimerLimit) {
							UpperAnimSelect = CharaAnimeID::Upper_Aim;
						}
						if (this->m_ShotPhase == 1) {
							UpperAnimSelect = GetProneShotAnimSel();
							if (!this->m_obj.get_anime((int)UpperAnimSelect).TimeEnd()) {
								this->m_obj.get_anime((int)UpperAnimSelect).time += 30.f / FPS * 1.5f;
							}
							if (this->m_obj.get_anime((int)UpperAnimSelect).TimeEnd() && !this->m_Press_Shot) {
								this->m_obj.get_anime((int)UpperAnimSelect).time = 0.f;
								if (!this->m_Gun_Ptr->GetIsEmpty()) {
									this->m_ShotPhase = 2;
								}
								else {
									this->m_ShotPhase = 3;
								}
							}
						}
						//コッキング
						if (this->m_ShotPhase == 2) {
							UpperAnimSelect = GetProneCockingAnimSel();
							if (!this->m_obj.get_anime((int)UpperAnimSelect).TimeEnd()) {
								this->m_obj.get_anime((int)UpperAnimSelect).time += 30.f / FPS * 1.5f;
							}
							if (this->m_obj.get_anime((int)UpperAnimSelect).TimeEnd()) {
								this->m_obj.get_anime((int)UpperAnimSelect).time = 0.f;
								this->m_ShotPhase = 0;
								this->m_ReadyTimer = 0.1f;
							}
						}
						//リロード開始
						if (this->m_ShotPhase == 3) {
							UpperAnimSelect = GetProneReloadStartAnimSel();
							if (!this->m_obj.get_anime((int)UpperAnimSelect).TimeEnd()) {
								this->m_obj.get_anime((int)UpperAnimSelect).time += 30.f / FPS * 1.5f;
							}
							if (this->m_obj.get_anime((int)UpperAnimSelect).TimeEnd()) {
								this->m_obj.get_anime((int)UpperAnimSelect).time = 0.f;
								this->m_ShotPhase++;
								this->m_ReadyTimer = 0.1f;
							}
						}
						if (this->m_ShotPhase == 4) {
							UpperAnimSelect = GetProneReloadOneAnimSel();
							if (!this->m_obj.get_anime((int)UpperAnimSelect).TimeEnd()) {
								this->m_obj.get_anime((int)UpperAnimSelect).time += 30.f / FPS * 1.5f;
							}
							if (this->m_obj.get_anime((int)UpperAnimSelect).TimeEnd()) {
								this->m_obj.get_anime((int)UpperAnimSelect).time = 0.f;
								if (this->m_Gun_Ptr->GetIsMagFull()) {
									this->m_ShotPhase++;
								}
								this->m_ReadyTimer = 0.1f;
							}
						}
						if (this->m_ShotPhase == 5) {
							UpperAnimSelect = GetProneReloadEndAnimSel();
							if (!this->m_obj.get_anime((int)UpperAnimSelect).TimeEnd()) {
								this->m_obj.get_anime((int)UpperAnimSelect).time += 30.f / FPS * 1.5f;
							}
							if (this->m_obj.get_anime((int)UpperAnimSelect).TimeEnd()) {
								this->m_obj.get_anime((int)UpperAnimSelect).time = 0.f;
								this->m_ShotPhase = 0;
								this->m_ReadyTimer = 0.1f;
							}
						}
						//
						if (this->m_ShotPhase == 2 || this->m_ShotPhase == 3) {
							if (this->m_Gun_Ptr != nullptr) {
								this->m_Gun_Ptr->SetCart();
							}
						}
						if (this->m_ShotPhase >= 3) {
							if (this->m_ShotPhase == 3) {
								if (this->m_obj.get_anime((int)UpperAnimSelect).time >= 37.f) {
									this->m_LoadAmmoPer = 1.f;
								}
							}
							if (this->m_ShotPhase == 4) {
								if (this->m_obj.get_anime((int)UpperAnimSelect).time <= 18.f) {
									this->m_LoadAmmoPer = 1.f;
								}
								else {
									easing_set(&this->m_LoadAmmoPer, 0.f, 0.7f);
								}
							}
						}
						else {
							this->m_LoadAmmoPer = 0.f;
						}
						//
						SetAnimLoop((int)CharaAnimeID::Upper_Sprint, 0.5f * this->m_Vec[0] * this->m_RunPer2);
						SetAnimLoop((int)CharaAnimeID::Upper_Running, 0.5f * this->m_Vec[0] * this->m_RunPer2);
					}
					//下半身
					{
						BottomAnimSelect = CharaAnimeID::Bottom_Stand;
						if (this->m_Press_GoFront) {
							if (this->m_IsRun) {
								BottomAnimSelect = CharaAnimeID::Bottom_Run;
							}
							else {
								BottomAnimSelect = CharaAnimeID::Bottom_Walk;
							}
						}
						else {
							if (this->m_Press_GoLeft) {
								BottomAnimSelect = CharaAnimeID::Bottom_LeftStep;
							}
							if (this->m_Press_GoRight) {
								BottomAnimSelect = CharaAnimeID::Bottom_RightStep;
							}
							if (this->m_Press_GoRear) {
								BottomAnimSelect = CharaAnimeID::Bottom_WalkBack;
							}
							if (this->m_IsSprint) {
								BottomAnimSelect = CharaAnimeID::Bottom_Run;
							}
						}
						SetAnimLoop((int)CharaAnimeID::Bottom_Turn, 0.5f);
						SetAnimLoop((int)CharaAnimeID::Bottom_Run, 0.5f * this->m_Vec[0] * this->m_RunPer2);
						SetAnimLoop((int)CharaAnimeID::Bottom_Walk, 1.15f * this->m_Vec[0]);
						SetAnimLoop((int)CharaAnimeID::Bottom_LeftStep, 1.15f * this->m_Vec[1]);
						SetAnimLoop((int)CharaAnimeID::Bottom_WalkBack, 1.15f * this->m_Vec[2]);
						SetAnimLoop((int)CharaAnimeID::Bottom_RightStep, 1.15f * this->m_Vec[3]);
						SetAnimLoop((int)CharaAnimeID::All_ProneWalk, 1.15f * this->m_MoveVector);
					}
					//
					{
						this->m_obj.get_anime((int)CharaAnimeID::All_PronetoStand).time += 30.f / FPS;
						if (this->m_obj.get_anime((int)CharaAnimeID::All_PronetoStand).TimeEnd()) {
							this->m_obj.get_anime((int)CharaAnimeID::All_PronetoStand).time = this->m_obj.get_anime((int)CharaAnimeID::All_PronetoStand).alltime;
						}
					}
				}
				//アニメセレクト
				{
					//上半身
					if (PrevUpperAnimSel == CharaAnimeID::Upper_Aim && UpperAnimSelect == GetProneShotAnimSel()) {
						this->m_AnimPerBuf[(int)PrevUpperAnimSel] = 0.f;
					}
					if (
						(PrevUpperAnimSel == GetProneCockingAnimSel() || PrevUpperAnimSel == GetProneReloadEndAnimSel())
						&& UpperAnimSelect == CharaAnimeID::Upper_Aim) {
						this->m_AnimPerBuf[(int)PrevUpperAnimSel] = 0.f;
						this->m_AnimPerBuf[(int)UpperAnimSelect] = 1.f;
					}

					if (
						(PrevUpperAnimSel == GetProneCockingAnimSel() || PrevUpperAnimSel == GetProneReloadEndAnimSel())
						&& UpperAnimSelect == CharaAnimeID::Upper_RunningStart) {
						this->m_AnimPerBuf[(int)PrevUpperAnimSel] = 0.f;
						this->m_AnimPerBuf[(int)UpperAnimSelect] = 1.f;
					}

					//真ん中
					this->m_AnimPerBuf[(int)CharaAnimeID::Mid_Squat] = this->m_SquatPer;
					//下半身
					easing_set(&this->m_AnimPerBuf[(int)CharaAnimeID::Bottom_Turn], (this->m_TurnBody) ? abs(this->m_rad.y() - this->m_yrad_Upper) / deg2rad(50.f) : 0.f, 0.8f);
					//伏せ
					this->m_AnimPerBuf[(int)CharaAnimeID::All_Prone] = (this->m_ShotPhase == 0) ? ((1.f - this->m_MoveVector) * this->m_PronePer) : 0.f;
					this->m_AnimPerBuf[(int)CharaAnimeID::All_ProneWalk] = (this->m_ShotPhase == 0) ? (this->m_MoveVector * this->m_PronePer) : 0.f;
					//銃操作
					this->m_AnimPerBuf[(int)GetProneShotAnimSel()] = ((GetProneShotAnimSel() == UpperAnimSelect) ? 1.f : 0.f);
					this->m_AnimPerBuf[(int)GetProneCockingAnimSel()] = ((GetProneCockingAnimSel() == UpperAnimSelect) ? 1.f : 0.f);
					this->m_AnimPerBuf[(int)GetProneReloadStartAnimSel()] = ((GetProneReloadStartAnimSel() == UpperAnimSelect) ? 1.f : 0.f);
					this->m_AnimPerBuf[(int)GetProneReloadOneAnimSel()] = ((GetProneReloadOneAnimSel() == UpperAnimSelect) ? 1.f : 0.f);
					this->m_AnimPerBuf[(int)GetProneReloadEndAnimSel()] = ((GetProneReloadEndAnimSel() == UpperAnimSelect) ? 1.f : 0.f);
					//その他
					for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						//上半身
						if (
							i == (int)CharaAnimeID::Upper_Down ||
							i == (int)CharaAnimeID::Upper_Aim ||
							i == (int)CharaAnimeID::Upper_RunningStart ||
							i == (int)CharaAnimeID::Upper_Running ||
							i == (int)CharaAnimeID::Upper_Sprint ||
							i == (int)GetProneShotAnimSel() ||
							i == (int)GetProneCockingAnimSel() ||
							i == (int)GetProneReloadStartAnimSel() ||
							i == (int)GetProneReloadOneAnimSel() ||
							i == (int)GetProneReloadEndAnimSel()
							)
						{
							this->m_AnimPerBuf[i] += ((i == (int)UpperAnimSelect) ? 1.f : -1.f)*3.f / FPS;
						}
						//下半身
						if (
							i == (int)CharaAnimeID::Bottom_Stand ||
							i == (int)CharaAnimeID::Bottom_Run ||
							i == (int)CharaAnimeID::Bottom_Walk ||
							i == (int)CharaAnimeID::Bottom_LeftStep ||
							i == (int)CharaAnimeID::Bottom_RightStep ||
							i == (int)CharaAnimeID::Bottom_WalkBack
							)
						{
							this->m_AnimPerBuf[i] += ((i == (int)BottomAnimSelect) ? 1.f : -1.f)*3.f / FPS;
						}
						this->m_AnimPerBuf[i] = std::clamp(this->m_AnimPerBuf[i], 0.f, 1.f);
						//反映
						if (i == (int)CharaAnimeID::All_Prone ||
							i == (int)CharaAnimeID::All_ProneShot ||
							i == (int)CharaAnimeID::All_ProneCocking ||
							i == (int)CharaAnimeID::All_ProneWalk ||
							i == (int)CharaAnimeID::All_PronetoStand ||
							i == (int)CharaAnimeID::All_Prone_ReloadStart ||
							i == (int)CharaAnimeID::All_Prone_ReloadOne ||
							i == (int)CharaAnimeID::All_Prone_ReloadEnd
							)
						{
							//伏せ
							if (this->m_PronetoStanding) {
								if (i == (int)CharaAnimeID::All_PronetoStand) {
									this->m_obj.get_anime(i).per = 1.f * (1.f - this->m_PronePer2);
								}
								else {
									this->m_obj.get_anime(i).per = 0.f;
								}
							}
							else {
								this->m_obj.get_anime(i).per = this->m_AnimPerBuf[i] * this->m_PronePer;
							}
						}
						else {
							//通常
							if (this->m_PronetoStanding) {
								this->m_obj.get_anime(i).per = this->m_AnimPerBuf[i] * (1.f - this->m_PronePer) * this->m_PronePer2;
							}
							else {
								this->m_obj.get_anime(i).per = this->m_AnimPerBuf[i] * (1.f - this->m_PronePer);
							}
						}
					}
					//
				}
				//足音
				{
					if (BottomAnimSelect != CharaAnimeID::Bottom_Stand) {
						auto Time = this->m_obj.get_anime((int)BottomAnimSelect).time;
						if (BottomAnimSelect != CharaAnimeID::Bottom_Run) {
							//L
							if ((9.f < Time &&Time < 10.f)) {
								if (!this->m_Prone.on()) {
									if (!this->m_RunFootL.check()) {
										this->m_RunFootL.play_3D(GetFrameWorldMatrix(CharaFrame::LeftFoot).pos(), 12.5f * 5.f);
									}
								}
								else {
									if (!this->m_SlidingL.check()) {
										this->m_SlidingL.play_3D(GetFrameWorldMatrix(CharaFrame::LeftFoot).pos(), 12.5f * 5.f);
									}
								}
							}
							//R
							if ((27.f < Time &&Time < 28.f)) {
								if (!this->m_Prone.on()) {
									if (!this->m_RunFootR.check()) {
										this->m_RunFootR.play_3D(GetFrameWorldMatrix(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
									}
								}
								else {
									if (!this->m_SlidingR.check()) {
										this->m_SlidingR.play_3D(GetFrameWorldMatrix(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
									}
								}
							}
						}
						else {
							//L
							if (
								(18.f < Time &&Time < 19.f) ||
								(38.f < Time &&Time < 39.f)
								) {
								if (!this->m_RunFootL.check()) {
									this->m_RunFootL.play_3D(GetFrameWorldMatrix(CharaFrame::LeftFoot).pos(), 12.5f * 5.f);
								}
							}
							//R
							if (
								(8.f < Time &&Time < 9.f) ||
								(28.f < Time &&Time < 29.f)
								) {
								if (!this->m_RunFootR.check()) {
									this->m_RunFootR.play_3D(GetFrameWorldMatrix(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
								}
							}
						}
						this->m_ReturnStand = true;
					}
					else if (this->m_ReturnStand) {
						if (!this->m_Sliding.check()) {
							this->m_Sliding.vol((int)(192.f * this->m_RunPer2 / SpeedLimit));
							this->m_Sliding.play_3D(GetFrameWorldMatrix(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
						}
						this->m_ReturnStand = false;
					}
					if (0.1f < this->m_PronePer&&this->m_PronePer < 0.2f) {
						if (!this->m_Standup.check()) {
							this->m_Standup.play_3D(GetFrameWorldMatrix(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
						}
					}
				}
				//
			}
			void ExecuteHeartRate(void) noexcept {
				auto addRun = (this->m_RunPer2 - this->m_PrevRunPer2);
				if (addRun > 0.f) {
					this->m_HeartRate_r += (10.f + GetRandf(10.f)) / FPS;
				}
				else if (addRun < 0.f) {
					this->m_HeartRate_r -= (5.f + GetRandf(5.f)) / FPS;
				}
				this->m_HeartRate_r += (this->m_MoveVector * this->m_RunPer2 / SpeedLimit * 1.95f) / FPS;
				this->m_HeartRate_r -= (2.f + GetRandf(4.f)) / FPS;
				this->m_HeartRate_r = std::clamp(this->m_HeartRate_r, HeartRateMin, HeartRateMax);

				if (this->m_HeartRate < this->m_HeartRate_r) {
					this->m_HeartRate += 5.f / FPS;
				}
				else if (this->m_HeartRate >= this->m_HeartRate_r) {
					this->m_HeartRate -= 5.f / FPS;
				}
				//this->m_HeartRate = this->m_HeartRate_r;
				this->m_HeartRateRad += deg2rad(this->m_HeartRate) / FPS;
				if (this->m_HeartRateRad >= DX_PI_F * 2) {
					this->m_HeartRateRad -= DX_PI_F * 2;
				}
				if (
					(deg2rad(0) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(10)) ||
					(deg2rad(120) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(130)) ||
					(deg2rad(240) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(250))
					) {
					if (!this->m_Heart[this->m_HeartSESel].check()) {
						this->m_Heart[this->m_HeartSESel].play_3D(GetFrameWorldMatrix(CharaFrame::Upper2).pos(), 12.5f * 0.85f);
						++this->m_HeartSESel %= this->m_Heart.size();
					}
				}


				this->m_Stamina += std::clamp((100.f - this->m_HeartRate) / 40.f, -2.5f, 2.5f) / FPS;

				if (!this->m_IsSprint && !this->m_IsRun) {
					this->m_Stamina += 1.0f / FPS;
				}
				if (this->m_Squat.on()) {
					this->m_Stamina += 1.0f / FPS;
				}
				if (this->m_Prone.on()) {
					this->m_Stamina += 1.5f / FPS;
				}

				if (this->m_IsSprint) {
					this->m_Stamina += -0.75f / FPS;
				}

				this->m_Stamina = std::clamp(this->m_Stamina, 0.f, StaminaMax);

				if (this->m_Stamina <= 0.f) {
					this->m_CannotSprint = true;
				}
				if (this->m_CannotSprint) {
					if (this->m_Stamina > StaminaMax * 0.3f) {
						this->m_CannotSprint = false;
					}
				}

				if (this->m_Stamina <= StaminaMax * 0.3f) {
					if (!this->m_Breath.check()) {
						this->m_Breath.play_3D(GetFrameWorldMatrix(CharaFrame::Upper2).pos(), 12.5f * 5.f);
					}
				}
				if (this->m_Breath.check()) {
					this->m_Breath.SetPosition(GetFrameWorldMatrix(CharaFrame::Upper2).pos());
				}
			}
		public:
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Human;
				ValueSet(deg2rad(0.f), deg2rad(0.f), false, false, VECTOR_ref::vget(0.f, 0.f, 0.f));
				this->m_Gun_Ptr.reset();
			}
			~CharacterClass(void) noexcept {}
		public:
			void Init(void) noexcept override {
				ObjectBaseClass::Init();

				this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time = this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).alltime;

				SetCreate3DSoundFlag(TRUE);
				this->m_RunFootL = SoundHandle::Load("data/Sound/SE/m_move/runfoot.wav");
				this->m_RunFootR = SoundHandle::Load("data/Sound/SE/m_move/runfoot.wav");
				this->m_Sliding = SoundHandle::Load("data/Sound/SE/m_move/sliding.wav");
				this->m_SlidingL = SoundHandle::Load("data/Sound/SE/m_move/sliding.wav");
				this->m_SlidingR = SoundHandle::Load("data/Sound/SE/m_move/sliding.wav");
				this->m_Standup = SoundHandle::Load("data/Sound/SE/m_move/standup.wav");
				this->m_Breath = SoundHandle::Load("data/Sound/SE/voice/WinningTicket/breath.wav");
				for (int i = 0; i < 3; i++) {
					this->m_Heart[i] = SoundHandle::Load("data/Sound/SE/m_move/heart.wav");
				}
				SetCreate3DSoundFlag(FALSE);

				this->m_RunFootL.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_RunFootL.get());
				this->m_RunFootR.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_RunFootR.get());
				this->m_Sliding.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_Sliding.get());
				this->m_SlidingL.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_SlidingL.get());
				this->m_SlidingR.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_SlidingR.get());
				this->m_Standup.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_Standup.get());
				this->m_Breath.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_Breath.get());
				for (int i = 0; i < 3; i++) {
					this->m_Heart[i].vol(92);
					Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_Heart[i].get());
				}
				this->m_IsFirstLoop = true;
			}
			//*
			void Execute(void) noexcept override {
				if (this->m_IsFirstLoop) {
					for (int i = 0; i < this->m_obj.get_anime().size(); i++) { this->m_obj.get_anime(i).per = 0.f; }
					this->m_obj.get_anime((int)CharaAnimeID::RightHand).per = 1.f;
					this->m_obj.work_anime();
					this->m_RightWristRot = this->m_obj.GetFrameLocalMatrix(Frames[(int)CharaFrame::RightWrist].first).GetRot();
				}
				this->m_IsFirstLoop = false;
				//操作
				{
					//0.01ms
					//移動ベクトル取得
					{
						SetVec(0, this->m_Press_GoFront || this->m_IsSprint);
						SetVec(1, this->m_Press_GoLeft);
						SetVec(2, this->m_Press_GoRear);
						SetVec(3, this->m_Press_GoRight);
					}
					//
					if (this->m_Gun_Ptr != nullptr) {
						this->m_Gun_Ptr->SetIsShot(false);
						if (this->m_Press_Shot && (this->m_ShotPhase == 0)) {
							this->m_ReadyTimer = 0.1f;
							if (this->m_ReadyPer >= 0.9f) {
								this->m_ReadyPer = 1.f;
								this->m_ShotPhase = 1;
								if (this->m_Gun_Ptr != nullptr) {
									this->m_Gun_Ptr->SetBullet();
								}
							}
						}
						else {
							this->m_ReadyTimer += 1.f / FPS;
							this->m_ReadyTimer = std::clamp(this->m_ReadyTimer, 0.f, this->m_Press_Aim ? 0.f : UpperTimerLimit);
							this->m_ReadyTimer = std::clamp(this->m_ReadyTimer, 0.f, this->m_Prone.on() ? 0.1f : UpperTimerLimit);
							if (this->m_Prone.on() && this->m_MoveVector > 0.6f) {
								this->m_ReadyTimer = UpperTimerLimit;
							}
						}
						if ((this->m_ShotPhase >= 2) || this->m_IsRun) {
							this->m_ReadyTimer = UpperTimerLimit;
						}
					}
					else {
						this->m_ReadyTimer += 1.f / FPS;
						this->m_ReadyTimer = std::clamp(this->m_ReadyTimer, 0.f, this->m_Prone.on() ? 0.1f : UpperTimerLimit);
						if (this->m_Prone.on() && this->m_MoveVector > 0.6f) {
							this->m_ReadyTimer = UpperTimerLimit;
						}
						if (this->m_IsRun) {
							this->m_ReadyTimer = UpperTimerLimit;
						}
					}
					easing_set(&this->m_ReadyPer, (this->m_ReadyTimer < UpperTimerLimit) ? 1.f : 0.f, 0.9f);
					easing_set(&this->m_RunPer, this->m_IsRun ? 1.f : 0.f, 0.975f);
					easing_set(&this->m_SprintPer, this->m_IsSprint ? 1.f : 0.f, 0.95f);
					easing_set(&this->m_SquatPer, this->m_Squat.on() ? 1.f : 0.f, 0.9f);

					if (!this->m_Prone.on() && (this->m_PronePer == 1.f)) {
						if (!this->m_PronetoStanding) {
							this->m_obj.get_anime((int)CharaAnimeID::All_PronetoStand).time = 0.f;
							this->m_PronePer2 = 0.f;
						}
						this->m_PronetoStanding = true;
					}
					if (this->m_PronetoStanding && (this->m_PronePer == 0.f)) {
						this->m_PronetoStanding = false;
					}

					if (this->m_PronetoStanding) {
						if ((this->m_obj.get_anime((int)CharaAnimeID::All_PronetoStand).time / this->m_obj.get_anime((int)CharaAnimeID::All_PronetoStand).alltime) > 0.9f) {
							easing_set(&this->m_PronePer2, 1.f, 0.95f);
						}
					}

					easing_set(&this->m_PronePer, this->m_Prone.on() ? 1.f : 0.f, 0.95f);
					if (!this->m_Prone.on() && (0.01f >= this->m_PronePer)) { this->m_PronePer = 0.f; }
					if (this->m_Prone.on() && (0.99f <= this->m_PronePer)) { this->m_PronePer = 1.f; }

					//this->m_PronePer = std::clamp(this->m_PronePer + (this->m_Prone.on() ? 1.f : -3.f) / FPS, 0.f, 1.f);
					//this->m_yrad_Upper、this->m_yrad_Bottom、this->m_rad.z()決定
					{
						if (this->m_MoveVector <= 0.1f) {
							if (abs(this->m_rad.y() - this->m_yrad_Upper) > deg2rad(50.f)) {
								this->m_TurnBody = true;
							}
							if (abs(this->m_rad.y() - this->m_yrad_Upper) < deg2rad(0.5f)) {
								this->m_TurnBody = false;
							}
						}
						else {
							this->m_TurnBody = false;
						}
						//
						auto FrontP = (this->m_IsSprint || (this->m_Press_GoFront && !this->m_Press_GoRear)) ? (atan2f(this->m_Vec[1] - this->m_Vec[3], this->m_Vec[0] - this->m_Vec[2]) * this->m_Vec[0]) : 0.f;
						FrontP += (!this->m_Press_GoFront && this->m_Press_GoRear) ? (atan2f(-(this->m_Vec[1] - this->m_Vec[3]), -(this->m_Vec[0] - this->m_Vec[2])) * this->m_Vec[2]) : 0.f;
						auto TmpRunPer = Leap(Leap(0.85f, 0.7f, this->m_RunPer), 0.f, this->m_PronePer);
						if (this->m_TurnBody || (this->m_MoveVector > 0.1f) || this->m_Prone.on()) { easing_set(&this->m_yrad_Upper, this->m_rad.y(), TmpRunPer); }
						auto OLDP = this->m_yrad_Bottom;
						easing_set(&this->m_yrad_Bottom, this->m_yrad_Upper - FrontP * (1.f - this->m_PronePer), TmpRunPer);
						{
							auto zbuf = this->m_rad_Buf.z();
							easing_set(&zbuf, (this->m_yrad_Bottom - OLDP) * 2.f, 0.9f);
							this->m_rad_Buf.z(zbuf);
						}
					}
				}
				//上半身回転
				{
					//0.06ms
					auto * F = &this->Frames[(int)CharaFrame::Upper];
					F->second.mat = MATRIX_ref::RotX(this->m_rad.x()) * MATRIX_ref::RotY(this->m_rad.y() - this->m_yrad_Bottom);
					this->m_obj.frame_Reset(F->first);
					this->m_obj.SetFrameLocalMatrix(F->first, this->m_obj.GetFrameLocalMatrix(F->first).GetRot() * F->second.MatIn());
				}
				//AnimUpdte
				ExecuteAnim();//0.03ms
				//SetMat指示
				{
					//0.03ms
					this->m_PrevRunPer2 = this->m_RunPer2;
					this->m_RunPer2 = Leap(0.35f, (SpeedLimit * (1.f + 0.5f * this->m_SprintPer)), this->m_RunPer);
					this->m_RunPer2 = Leap(this->m_RunPer2, 0.15f, this->m_SquatPer);
					this->m_RunPer2 = Leap(this->m_RunPer2, 0.1f, this->m_PronePer);
					if (this->m_PrevRunPer2 == 0.f) {
						this->m_PrevRunPer2 = this->m_RunPer2;
					}

					auto OLDpos = this->m_posBuf;
					VECTOR_ref vecBuf = MATRIX_ref::Vtrans(VECTOR_ref::vget(this->m_Vec[1] - this->m_Vec[3], 0, this->m_Vec[2] - this->m_Vec[0]), MATRIX_ref::RotY(this->m_yrad_Upper));
					this->m_MoveVector = std::clamp(vecBuf.size(), 0.f, 1.f);

					if (this->m_MoveVector > 0.1f) {
						this->m_move.vec.x((vecBuf.Norm() * this->m_RunPer2 * 60.f / FPS).x());
						this->m_move.vec.z((vecBuf.Norm() * this->m_RunPer2 * 60.f / FPS).z());
					}
					else {
						this->m_move.vec.x(vecBuf.x());
						this->m_move.vec.z(vecBuf.z());
					}
					{
						auto HitResult = this->m_MapCol->CollCheck_Line(this->m_posBuf + VECTOR_ref::up() * -1.f, this->m_posBuf + VECTOR_ref::up() * 15.f);
						if (HitResult.HitFlag == TRUE) {
							auto yPos = this->m_posBuf.y();
							easing_set(&yPos, HitResult.HitPosition.y, 0.8f);
							this->m_posBuf.y(yPos);
							this->m_move.vec.y(0.f);

							easing_set(&this->m_ProneNormal, VECTOR_ref(HitResult.Normal), 0.95f);
							//auto HitResult2 = this->m_MapCol->CollCheck_Line(GetEyePosition() + VECTOR_ref::up() * -15.f, GetEyePosition() + VECTOR_ref::up() * 15.f);
							//if (HitResult2.HitFlag == TRUE) {
							//	easing_set(&this->m_ProneNormal, (VECTOR_ref(HitResult.Normal) + VECTOR_ref(HitResult2.Normal)) / 2.f, 0.95f);
							//}
							//else {
							//	easing_set(&this->m_ProneNormal, VECTOR_ref(HitResult.Normal), 0.95f);
							//}
						}
						else {
							this->m_move.vec.yadd(2.f * M_GR / (FPS * FPS));
						}
					}
					this->m_posBuf += this->m_move.vec;
					col_wall(OLDpos, &this->m_posBuf, *this->m_MapCol);
					this->m_move.mat = MATRIX_ref::RotZ(this->m_rad.z()) * MATRIX_ref::RotY(this->m_yrad_Bottom)
						* MATRIX_ref::RotVec2(VECTOR_ref::up(), Leap(VECTOR_ref::up(), this->m_ProneNormal, this->m_PronePer));
					easing_set(&this->m_move.pos, this->m_posBuf, 0.9f);
					this->m_obj.SetMatrix(this->m_move.MatIn());
				}
				//銃座標指定(アニメアップデート含む)
				if (this->m_Gun_Ptr != nullptr) {
					//0.19ms
					//持ち手探索
					VECTOR_ref yVec1, zVec1, Pos1;
					{
						int i = (int)GetProneCockingAnimSel();
						bool change = (this->m_obj.get_anime(i).per == 1.f);
						if (!change) { i = (int)GetProneReloadStartAnimSel(); }
						change = (this->m_obj.get_anime(i).per == 1.f);
						if (!change) { i = (int)GetProneReloadOneAnimSel(); }
						change = (this->m_obj.get_anime(i).per == 1.f);
						if (!change) { i = (int)GetProneReloadEndAnimSel(); }
						change = (this->m_obj.get_anime(i).per == 1.f);
						//
						if (change) {//タイムでやれば歩きイランのでは？
							this->m_obj.get_anime((int)GetProneAimAnimSel()).per = 1.f;
							this->m_obj.get_anime(i).per = 0.f;
						}
						this->m_obj.get_anime((int)CharaAnimeID::RightHand).per = 0.f;
						//アニメアップデート
						this->m_obj.work_anime();//0.35ms

						auto mat = (this->m_obj.GetFrameLocalWorldMatrix(Frames[(int)CharaFrame::RightWrist].first).GetRot()*GetCharaDir().Inverse());
						yVec1 = MATRIX_ref::Vtrans(VECTOR_ref::vget(0, 0, -1).Norm(), mat);
						zVec1 = MATRIX_ref::Vtrans(VECTOR_ref::vget(-1, -1, 0).Norm(), mat);
						Pos1 = this->m_obj.GetFrameLocalWorldMatrix(Frames[(int)CharaFrame::RightHandJoint].first).pos();

						if (change) {
							this->m_obj.get_anime((int)GetProneAimAnimSel()).per = 0.f;
							this->m_obj.get_anime(i).per = 1.f;
						}
						this->m_obj.get_anime((int)CharaAnimeID::RightHand).per = (this->m_ShotPhase < 2) ? (!this->m_RunReady ? 1.f : 0.f) : 0.f;
						//必要最低限のアニメ再更新
						this->m_obj.work_anime((int)GetProneAimAnimSel());
						this->m_obj.work_anime(i);
						this->m_obj.work_anime((int)CharaAnimeID::RightHand);
					}
					//背負い場所探索
					VECTOR_ref yVec2, zVec2, Pos2;
					{
						yVec2 = (MATRIX_ref::RotZ(deg2rad(30)) * this->m_obj.GetFrameLocalWorldMatrix(this->Frames[(int)CharaFrame::Upper2].first).GetRot() * GetCharaDir().Inverse()).xvec();
						zVec2 = (MATRIX_ref::RotZ(deg2rad(30)) * this->m_obj.GetFrameLocalWorldMatrix(this->Frames[(int)CharaFrame::Upper2].first).GetRot() * GetCharaDir().Inverse()).yvec();
						Pos2 =
							this->m_obj.GetFrameLocalWorldMatrix(this->Frames[(int)CharaFrame::Upper2].first).pos() +
							this->m_obj.GetFrameLocalWorldMatrix(this->Frames[(int)CharaFrame::Upper2].first).GetRot().yvec() * -1.75f +
							this->m_obj.GetFrameLocalWorldMatrix(this->Frames[(int)CharaFrame::Upper2].first).GetRot().zvec() * 1.75f;
					}
					auto yVec = Leap(yVec1, yVec2, this->m_SlingPer);
					auto zVec = Leap(zVec1, zVec2, this->m_SlingPer);
					auto PosBuf = Leap(Pos1, Pos2, this->m_SlingPer);
					auto tmp_gunrat = MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, -1).Norm(), zVec);
					tmp_gunrat *= MATRIX_ref::RotVec2(tmp_gunrat.yvec(), yVec);
					tmp_gunrat *= GetCharaDir() * MATRIX_ref::Mtrans(PosBuf);
					this->m_Gun_Ptr->SetMatrix(tmp_gunrat, this->m_ShotPhase - 2);
					//弾を手持ち
					{
						//スコープ内0.01ms
						auto Thumb = GetFrameWorldMatrix(CharaFrame::RightThumb);
						auto Pointer = GetFrameWorldMatrix(CharaFrame::RightPointer);
						MATRIX_ref mat =
							MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, -1), MATRIX_ref::Vtrans(VECTOR_ref::vget(-1, -1, 0), Pointer.GetRot()))
							* MATRIX_ref::Mtrans((Thumb.pos() + Pointer.pos()) / 2.f);
						this->m_Gun_Ptr->SetAmmoHandMatrix(mat, this->m_LoadAmmoPer);
					}
				}
				//顔
				{
					//スコープ内0.01ms
					SetShape(CharaShape::None, 0.f);
					SetShape(CharaShape::O, (0.75f + sin(this->m_HeartRateRad * 3)*0.25f)*(1.f - this->m_Stamina / StaminaMax));
					if (this->m_EyeclosePer <= 0.05f && (GetRand(100) == 0)) {
						this->m_Eyeclose = 1;
					}
					if (this->m_EyeclosePer >= 0.95f) {
						this->m_Eyeclose = 0;
					}
					easing_set(&this->m_EyeclosePer, (float)this->m_Eyeclose, 0.5f);
					SetShape(CharaShape::EYECLOSE, this->m_EyeclosePer);
					ExecuteShape();
				}
				//心拍数
				ExecuteHeartRate();//0.00ms
			}
			//*/
		public:
			void ValueSet(float pxRad, float pyRad, bool SquatOn, bool ProneOn, const VECTOR_ref& pPos) {
				for (int i = 0; i < 4; i++) {
					this->m_Vec[i] = 0.f;
				}
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					this->m_AnimPerBuf[i] = 0.f;
				}
				this->m_ReadyPer = 1.f;
				this->m_ReadyTimer = UpperTimerLimit;
				this->m_HeartRate = HeartRateMin;
				this->m_Stamina = StaminaMax;

				this->m_RunPer = 0.f;
				this->m_MoveVector = 0.f;
				this->m_RunPer2 = 0.f;
				this->m_PrevRunPer2 = 0.f;
				this->m_HeartRateRad = 0.f;
				this->m_xrad_Add = 0.f;
				this->m_yrad_Add = 0.f;
				this->m_TurnBody = false;
				this->m_ShotPhase = 0;
				this->m_SetReset = true;
				this->m_IsSprint = false;
				this->m_Ready = false;
				this->m_RunReady = false;
				this->m_Running = false;
				this->m_Press_GoFront = false;
				this->m_Press_GoRear = false;
				this->m_Press_GoLeft = false;
				this->m_Press_GoRight = false;
				this->m_Press_Shot = false;
				this->m_Press_Aim = false;
				this->m_IsRun = false;

				this->m_HeartSESel = 0;

				this->m_Score = 0.f;

				//動作にかかわる操作
				this->m_Squat.Init(SquatOn);
				this->m_Prone.Init(ProneOn);
				this->m_rad_Buf.x(pxRad);
				this->m_rad_Buf.y(pyRad);
				//上記を反映するもの
				this->m_rad = this->m_rad_Buf;
				this->m_yrad_Upper = this->m_rad.y();
				this->m_yrad_Bottom = this->m_rad.y();
				this->m_SquatPer = SquatOn ? 1.f : 0.f;
				this->m_PronePer = ProneOn ? 1.f : 0.f;
				this->m_posBuf = pPos;
				SetMove(this->m_yrad_Bottom, this->m_posBuf);
			}
			void SetInput(
				float pAddxRad, float pAddyRad,
				bool pGoFrontPress,
				bool pGoBackPress,
				bool pGoLeftPress,
				bool pGoRightPress,
				bool pSquatPress,
				bool pPronePress,
				bool pShotPress,
				bool pAimPress,
				bool pRunPress
			) {
				this->m_Press_GoFront = pGoFrontPress;
				this->m_Press_GoRear = pGoBackPress;
				this->m_Press_GoLeft = pGoLeftPress;
				this->m_Press_GoRight = pGoRightPress;
				if (this->m_Prone.on() && (!(this->m_ShotPhase == 0))) {
					this->m_Press_GoFront = false;
					this->m_Press_GoRear = false;
					this->m_Press_GoLeft = false;
					this->m_Press_GoRight = false;
				}
				this->m_Press_Shot = pShotPress;
				if (0.f != this->m_PronePer && this->m_PronePer != 1.0f) {
					this->m_Press_Shot = false;
				}
				if ((this->m_Gun_Ptr != nullptr) && (!this->m_Gun_Ptr->GetCanshot())) {
					this->m_Press_Shot = false;
				}


				this->m_Press_Aim = pAimPress;

				if (this->m_PronetoStanding) {
					this->m_Press_Aim = false;
					this->m_ReadyTimer = UpperTimerLimit;
				}

				if (!this->m_IsRun& pRunPress) {
					this->m_RunTimer = 1.f;
				}
				if (this->m_RunTimer > 0.f) {
					this->m_RunTimer -= 1.f / FPS;
					this->m_IsRun = true;
				}
				else {
					this->m_RunTimer = 0.f;
					this->m_IsRun = pRunPress;
				}
				//
				if (this->m_Press_GoRear || (!this->m_Press_GoFront && (this->m_Press_GoLeft || this->m_Press_GoRight))) {
					this->m_IsRun = false;
				}
				if (this->m_Prone.on() || this->m_PronetoStanding) {
					this->m_IsRun = false;
				}

				this->m_IsSprint = this->m_IsRun && (!this->m_Press_GoFront && !this->m_Press_GoRear);

				if (this->m_IsSprint) {
					if (this->m_CannotSprint) {
						this->m_Press_GoFront = true;
						this->m_IsSprint = false;
					}
				}

				this->m_Squat.GetInput(pSquatPress);
				if (this->m_IsRun) { this->m_Squat.first = false; }

				this->m_Prone.GetInput(pPronePress && (this->m_ShotPhase == 0) && (this->m_PronePer == 0.f || this->m_PronePer == 1.f));
				if (this->m_Prone.on()) { this->m_Squat.first = false; }
				{
					auto TmpReady = !(!this->m_IsRun || (this->m_ShotPhase >= 2));
					this->m_RunReadyFirst = (TmpReady && !this->m_RunReady);
					this->m_RunReady = TmpReady;
				}
				{
					auto limchange = Leap(Leap(1.f, powf(1.f - this->m_Vec[0], 0.5f), this->m_RunPer * 0.8f), 0.15f, this->m_PronePer);
					auto tmp = Leap(1.f, 0.1f, this->m_PronePer);
					auto tmp2 = 0.2f * GetRandf(deg2rad(1.f - this->m_PronePer));
					auto tmp3 = Leap(0.5f, Leap(0.35f, 0.2f, this->m_SquatPer), this->m_PronePer);

					easing_set(&this->m_xrad_Add, tmp2 + 0.0002f * sin(this->m_HeartRateRad) * powf(this->m_HeartRate / HeartRateMin, 3.f), 0.95f);
					easing_set(&this->m_yrad_Add, tmp2 + 0.0002f * sin(this->m_HeartRateRad * 3) * powf(this->m_HeartRate / HeartRateMin, 3.f), 0.95f);

					this->m_rad_Buf.x(std::clamp(this->m_rad_Buf.x() + pAddxRad * tmp, -deg2rad(40.f) * limchange, deg2rad(25.f) * limchange));
					this->m_rad_Buf.yadd(pAddyRad * tmp);
					this->m_rad_Buf.xadd(this->m_xrad_Add * tmp3);
					this->m_rad_Buf.yadd(this->m_yrad_Add * tmp3);
					easing_set(&this->m_rad, this->m_rad_Buf, 0.5f);
				}
			}
		};
	};
};
