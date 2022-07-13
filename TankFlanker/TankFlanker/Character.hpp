#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CharacterClass : public ObjectBaseClass {
		private://キャラパラメーター
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
			bool m_RunReady{ false };
			bool m_RunReadyFirst{ false };
			bool m_Running{ false };
			bool m_ReturnStand = false;
			CharaAnimeID UpperAnimSelect, PrevUpperAnimSel;
			CharaAnimeID BottomAnimSelect;
			MATRIX_ref m_RightWristRot;
			float m_Score{ 0.f };			//スコア
			//銃
			float m_ReadyTimer{ 0.f };
			float m_ReadyPer{ 0.f };
			int m_ShotPhase{ 0 };
			float m_LoadAmmoPer{ 0.f };
			//入力
			switchs m_Squat;
			switchs m_Prone;
			switchs QKey;
			switchs EKey;
			bool m_Press_GoFront{ false };
			bool m_Press_GoRear{ false };
			bool m_Press_GoLeft{ false };
			bool m_Press_GoRight{ false };
			bool m_Press_Shot{ false };
			bool m_Press_Reload{ false };
			bool m_Press_Aim{ false };
			bool m_Ready_Start{ false };
			bool m_ReadySwitch{ false };
			float m_HeartRate{ HeartRateMin };//心拍数
			float m_HeartRate_r{ HeartRateMin };//心拍数
			float m_HeartRateRad{ 0.f };//呼吸Sin渡し
			float m_Stamina{ StaminaMax };//スタミナ
			bool m_CannotSprint{ false };//スタミナ切れ
			int m_TurnRate{ 0 };
			float m_TurnRatePer{ 0.f };

			float m_LeftHandPer{ 0.f };
			MATRIX_ref m_UpperMatrix;
			//表情
			int m_Eyeclose{ 0 };
			float m_EyeclosePer{ 0.f };

			float m_NeckPer{ 0.f };
			//サウンド
			int charaSound{ -1 };
			SoundHandle m_Breath;
			bool heartp;
			std::shared_ptr<GunClass> m_Gun_Ptr{ nullptr };			//銃
		public://ゲッター
			void SetGunPtr(std::shared_ptr<GunClass>& pGunPtr) noexcept { this->m_Gun_Ptr = pGunPtr; }
			void LoadReticle(void) noexcept {
				if (this->m_Gun_Ptr != nullptr) {
					this->m_Gun_Ptr->LoadReticle();
				}
			}
			void AddScore(float value) noexcept { this->m_Score += value; }
			void SetScore(float value) noexcept { this->m_Score = value; }
			const auto GetCharaDir(void) const noexcept { return this->m_UpperMatrix * this->m_move.mat; }//プライベートで良い
			auto& GetGunPtr(void) noexcept { return this->m_Gun_Ptr; }
			const auto GetShotSwitch(void) const noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetIsShot() : false; }
			const auto GetEyeVector(void) const noexcept { return (this->m_Gun_Ptr != nullptr) ? (Leap(GetCharaDir().zvec(), this->m_Gun_Ptr->GetMatrix().zvec(), this->m_ReadyPer) * -1.f) : (GetCharaDir().zvec() * -1.f); }
			const auto GetEyePosition(void) const noexcept { return (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f + this->GetEyeVector().Norm() * 0.5f; }
			const auto GetScopePos(void) noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetScopePos() : GetEyePosition(); }
			const auto GetLensPos(void) noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetLensPos() : VECTOR_ref::zero(); }
			const auto GetReticlePos(void) noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetReticlePos() : VECTOR_ref::zero(); }
			const auto GetLensPosSize(void) noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetLensPosSize() : VECTOR_ref::zero(); }
			const auto GetCanshot(void) const noexcept { return (this->m_Gun_Ptr != nullptr) ? (this->m_Gun_Ptr->GetCanshot() && (this->m_ShotPhase <= 1)) : false; }
			const auto GetIsEmpty(void) const noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetIsEmpty() : false; }
			const auto GetAmmoNum(void) const noexcept { return this->m_Gun_Ptr->GetAmmoNum(); }
			const auto GetAmmoAll(void) const noexcept { return this->m_Gun_Ptr->GetAmmoAll(); }
			const auto& GetReticle(void) noexcept { return this->m_Gun_Ptr->GetReticle(); }
			const auto& GetScore(void) const noexcept { return this->m_Score; }
			const auto& GetIsRun(void) const noexcept { return this->m_IsRun; }
			const auto& GetHeartRate(void) const noexcept { return this->m_HeartRate; }
			const auto& GetHeartRateRad(void) const noexcept { return this->m_HeartRateRad; }
			const auto& GetStamina(void) const noexcept { return this->m_Stamina; }
			const auto& GetStaminaMax(void) const noexcept { return this->StaminaMax; }
			const auto& GetTurnRatePer(void) const noexcept { return this->m_TurnRatePer; }
			const auto GetIsProne(void) const noexcept { return this->m_Prone.on(); }
			const auto GetProneShotAnimSel(void) const noexcept { return (GetIsProne()) ? CharaAnimeID::All_ProneShot : CharaAnimeID::Upper_Shot; }
			const auto GetProneCockingAnimSel(void) const noexcept { return (GetIsProne()) ? CharaAnimeID::All_ProneCocking : CharaAnimeID::Upper_Cocking; }
			const auto GetProneAimAnimSel(void) const noexcept { return (GetIsProne()) ? CharaAnimeID::All_Prone : CharaAnimeID::Upper_Aim; }
			const auto GetProneReloadStartAnimSel(void) const noexcept { return (GetIsProne()) ? CharaAnimeID::All_Prone_ReloadStart : CharaAnimeID::Upper_ReloadStart; }
			const auto GetProneReloadOneAnimSel(void) const noexcept { return (GetIsProne()) ? CharaAnimeID::All_Prone_ReloadOne : CharaAnimeID::Upper_ReloadOne; }
			const auto GetProneReloadEndAnimSel(void) const noexcept { return (GetIsProne()) ? CharaAnimeID::All_Prone_ReloadEnd : CharaAnimeID::Upper_ReloadEnd; }
			const auto GetIsADS(void) const noexcept { return this->m_ReadyTimer == 0.f; }
			const auto GetReadyPer(void) const noexcept { return this->m_ReadyPer; }


		private:
			void SetVec(int pDir, bool Press) {
				this->m_Vec[pDir] += (Press ? 1.f : -1.f)*2.f / FPS;
				this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
			}
		private:
			//以前の状態保持
			void ExecuteSavePrev(void) noexcept {
				this->PrevUpperAnimSel = this->UpperAnimSelect;
				this->m_PrevRunPer2 = this->m_RunPer2;
			}
			//アニメ
			void ExecuteAnim(void) noexcept {
				auto SE = SoundPool::Instance();
				//アニメ演算
				{
					//上半身
					{
						UpperAnimSelect = (!this->m_Ready_Start) ? CharaAnimeID::Upper_Ready : CharaAnimeID::Upper_Down;
						if (!this->m_Ready_Start) {
							this->m_SlingPer = 1.f;
						}
						if (this->m_ReadySwitch) {
							this->m_RunReadyFirst = false;
							this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).GoEnd();
						}
						bool canreverse = true;
						if (!this->m_RunReady && !this->m_Running && !this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).TimeEnd()) {
							this->m_RunReady = true;
							canreverse = false;
						}
						if (this->m_RunReadyFirst) {
							this->m_SlingPer = 0.f;
							this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).GoStart();
						}
						if (this->m_RunReady) {
							if (!this->m_Running) {
								SetAnimOnce((int)CharaAnimeID::Upper_RunningStart, 2.f);
								easing_set(&this->m_SlingPer, (canreverse && (this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time > 16)) ? 1.f : 0.f, 0.9f);
								this->m_SprintPer = 0.f;
								if (this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).TimeEnd()) {
									this->m_Running = true;
									this->m_obj.get_anime((int)CharaAnimeID::Upper_Running).GoStart();
									if (canreverse) {
										this->m_SlingPer = 1.f;
									}
								}
								UpperAnimSelect = CharaAnimeID::Upper_RunningStart;
							}
							else {
								UpperAnimSelect = (!this->m_IsSprint) ? CharaAnimeID::Upper_Running : CharaAnimeID::Upper_Sprint;
							}
						}
						else {
							easing_set(&this->m_SlingPer, 0.f, 0.9f);
							this->m_Running = false;
						}
						if (this->m_ReadyTimer < UpperTimerLimit) {
							UpperAnimSelect = GetProneAimAnimSel();
						}
						if (this->m_ShotPhase == 1) {
							UpperAnimSelect = GetProneShotAnimSel();
							SetAnimOnce((int)UpperAnimSelect, 1.5f);
							if (this->m_obj.get_anime((int)UpperAnimSelect).TimeEnd() && !this->m_Press_Shot) {
								this->m_obj.get_anime((int)UpperAnimSelect).GoStart();
								if (!this->m_Gun_Ptr->GetIsEmpty()) {
									this->m_ShotPhase = 2;
								}
								else {
									this->m_ShotPhase = 0;
									UpperAnimSelect = GetProneAimAnimSel();
								}
							}
						}
						//コッキング
						if (this->m_ShotPhase == 2) {
							UpperAnimSelect = GetProneCockingAnimSel();
							SetAnimLoop((int)UpperAnimSelect, 1.5f);
							if (this->m_obj.get_anime((int)UpperAnimSelect).time == 0.f) {
								this->m_ShotPhase = 0;
								this->m_ReadyTimer = 0.1f;
							}
						}
						//リロード開始
						if (this->m_ShotPhase == 3) {
							UpperAnimSelect = GetProneReloadStartAnimSel();
							SetAnimLoop((int)UpperAnimSelect, 1.5f);
							if (this->m_obj.get_anime((int)UpperAnimSelect).time == 0.f) {
								this->m_ShotPhase++;
								this->m_ReadyTimer = 0.1f;
							}
						}
						if (this->m_ShotPhase == 4) {
							UpperAnimSelect = GetProneReloadOneAnimSel();
							SetAnimLoop((int)UpperAnimSelect, 1.5f);
							if (this->m_obj.get_anime((int)UpperAnimSelect).time == 0.f) {
								if (this->m_Gun_Ptr->GetIsMagFull()) {
									this->m_ShotPhase++;
								}
								this->m_ReadyTimer = 0.1f;
							}
						}
						if (this->m_ShotPhase == 5) {
							UpperAnimSelect = GetProneReloadEndAnimSel();
							SetAnimLoop((int)UpperAnimSelect, 1.5f);
							if (this->m_obj.get_anime((int)UpperAnimSelect).time == 0.f) {
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
						if (this->m_ShotPhase < 3) {
							this->m_LoadAmmoPer = 0.f;
						}
						else {
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

						//首
						easing_set(&m_NeckPer, (
							UpperAnimSelect == CharaAnimeID::Upper_Down
							|| UpperAnimSelect == CharaAnimeID::Upper_Running
							) ? 0.f : 1.f, 0.9f);
					}
					//下半身
					{
						if (this->m_Press_GoFront) {
							BottomAnimSelect = (this->m_IsRun) ? CharaAnimeID::Bottom_Run : CharaAnimeID::Bottom_Walk;
						}
						else {
							BottomAnimSelect = CharaAnimeID::Bottom_Stand;
							if (this->m_Press_GoLeft) { BottomAnimSelect = CharaAnimeID::Bottom_LeftStep; }
							if (this->m_Press_GoRight) { BottomAnimSelect = CharaAnimeID::Bottom_RightStep; }
							if (this->m_Press_GoRear) { BottomAnimSelect = CharaAnimeID::Bottom_WalkBack; }
							if (this->m_IsSprint) { BottomAnimSelect = CharaAnimeID::Bottom_Run; }
						}
					}
					//
					SetAnimOnce((int)CharaAnimeID::All_PronetoStand, 1.f);
					SetAnimLoop((int)CharaAnimeID::Upper_Sprint, 0.5f * this->m_Vec[0] * this->m_RunPer2);
					SetAnimLoop((int)CharaAnimeID::Upper_Running, 0.5f * this->m_Vec[0] * this->m_RunPer2);
					SetAnimLoop((int)CharaAnimeID::Bottom_Turn, 0.5f);
					SetAnimLoop((int)CharaAnimeID::Bottom_Run, 0.5f * this->m_Vec[0] * this->m_RunPer2);
					SetAnimLoop((int)CharaAnimeID::Bottom_Walk, 1.15f * this->m_Vec[0]);
					SetAnimLoop((int)CharaAnimeID::Bottom_LeftStep, 1.15f * this->m_Vec[1]);
					SetAnimLoop((int)CharaAnimeID::Bottom_WalkBack, 1.15f * this->m_Vec[2]);
					SetAnimLoop((int)CharaAnimeID::Bottom_RightStep, 1.15f * this->m_Vec[3]);
					SetAnimLoop((int)CharaAnimeID::All_ProneWalk, 1.15f * this->m_MoveVector);

					easing_set(&this->m_LeftHandPer,
						(
							this->m_AnimPerBuf[(int)CharaAnimeID::Upper_RunningStart] > 0.1f
							|| this->m_AnimPerBuf[(int)CharaAnimeID::Upper_Running] > 0.5f
							|| this->m_AnimPerBuf[(int)CharaAnimeID::Upper_Sprint] > 0.5f
							|| this->m_AnimPerBuf[(int)CharaAnimeID::All_ProneWalk] > 0.1f
							|| this->m_AnimPerBuf[(int)CharaAnimeID::All_PronetoStand] > 0.1f
							|| this->m_AnimPerBuf[(int)CharaAnimeID::Upper_Ready] > 0.1f
							) ? 1.f : 0.f, 0.8f);
				}
				//アニメセレクト
				{
					//伏せ
					this->m_AnimPerBuf[(int)CharaAnimeID::All_Prone] = (this->m_ShotPhase == 0) ? ((1.f - this->m_MoveVector) * this->m_PronePer) : 0.f;
					this->m_AnimPerBuf[(int)CharaAnimeID::All_ProneWalk] = (this->m_ShotPhase == 0) ? (this->m_MoveVector * this->m_PronePer) : 0.f;

					//上半身
					if (
						PrevUpperAnimSel == GetProneAimAnimSel() && UpperAnimSelect == GetProneShotAnimSel()) {
						this->m_AnimPerBuf[(int)PrevUpperAnimSel] = 0.f;
						this->m_AnimPerBuf[(int)UpperAnimSelect] = 1.f;
					}
					if (
						PrevUpperAnimSel == GetProneShotAnimSel()
						&& UpperAnimSelect == GetProneAimAnimSel()) {
						this->m_AnimPerBuf[(int)PrevUpperAnimSel] = 0.f;
						this->m_AnimPerBuf[(int)UpperAnimSelect] = 1.f;
					}
					if (
						(PrevUpperAnimSel == GetProneShotAnimSel() || PrevUpperAnimSel == GetProneCockingAnimSel() || PrevUpperAnimSel == GetProneReloadEndAnimSel())
						&& UpperAnimSelect == GetProneAimAnimSel()) {
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
					//銃操作
					this->m_AnimPerBuf[(int)GetProneShotAnimSel()] = ((GetProneShotAnimSel() == UpperAnimSelect) ? 1.f : 0.f);
					this->m_AnimPerBuf[(int)GetProneCockingAnimSel()] = ((GetProneCockingAnimSel() == UpperAnimSelect) ? 1.f : 0.f);
					this->m_AnimPerBuf[(int)GetProneReloadStartAnimSel()] = ((GetProneReloadStartAnimSel() == UpperAnimSelect) ? 1.f : 0.f);
					this->m_AnimPerBuf[(int)GetProneReloadOneAnimSel()] = ((GetProneReloadOneAnimSel() == UpperAnimSelect) ? 1.f : 0.f);
					this->m_AnimPerBuf[(int)GetProneReloadEndAnimSel()] = ((GetProneReloadEndAnimSel() == UpperAnimSelect) ? 1.f : 0.f);
					//その他
					for (int i = 2; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						//上半身
						if (
							i == (int)CharaAnimeID::Upper_Ready ||
							i == (int)CharaAnimeID::Upper_Down ||
							i == (int)CharaAnimeID::Upper_RunningStart ||
							i == (int)CharaAnimeID::Upper_Running ||
							i == (int)CharaAnimeID::Upper_Sprint ||
							i == (int)GetProneAimAnimSel() ||
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
							if ((9.f < Time && Time < 10.f)) {
								if (!GetIsProne()) {
									if (charaSound != 1) {
										charaSound = 1;
										SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), 12.5f * 5.f);
									}
								}
								else {
									if (charaSound != 2) {
										charaSound = 2;
										SE->Get((int)SoundEnum::SlideFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), 12.5f * 5.f, 128);
									}
								}
							}
							//R
							if ((27.f < Time &&Time < 28.f)) {
								if (!GetIsProne()) {
									if (charaSound != 3) {
										charaSound = 3;
										SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
									}
								}
								else {
									if (charaSound != 4) {
										charaSound = 4;
										SE->Get((int)SoundEnum::SlideFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f, 128);
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
								if (charaSound != 5) {
									charaSound = 5;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), 12.5f * 5.f);
								}
							}
							//R
							if (
								(8.f < Time &&Time < 9.f) ||
								(28.f < Time &&Time < 29.f)
								) {
								if (charaSound != 6) {
									charaSound = 6;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
								}
							}
						}
						this->m_ReturnStand = true;
					}
					else if (this->m_ReturnStand) {
						if (charaSound != 7) {
							charaSound = 7;
							SE->Get((int)SoundEnum::SlideFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f, (int)(192.f * this->m_RunPer2 / SpeedLimit));
						}
						this->m_ReturnStand = false;
					}
					if (0.1f < this->m_PronePer&&this->m_PronePer < 0.2f) {
						if (charaSound != 8) {
							charaSound = 8;
							SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
						}
					}
				}
				//
			}
			//
			void move_LeftArm() noexcept {
				auto matBase = GetFrameWorldMat(CharaFrame::Upper2).GetRot().Inverse();
				//matBase = GetCharaDir().Inverse();
				ResetFrameLocalMat(CharaFrame::LeftArm);
				ResetFrameLocalMat(CharaFrame::LeftArm2);
				ResetFrameLocalMat(CharaFrame::LeftWrist);
				matBase = GetParentFrameWorldMat(CharaFrame::LeftArm).GetRot().Inverse();

				VECTOR_ref GunPos = this->m_Gun_Ptr->GetFrameWorldMat(GunFrame::LeftHandPos).pos();
				VECTOR_ref Gunyvec = this->m_Gun_Ptr->GetFrameWorldMat(GunFrame::LeftHandYvec).pos() - GunPos;
				VECTOR_ref Gunzvec = this->m_Gun_Ptr->GetFrameWorldMat(GunFrame::LeftHandZvec).pos() - GunPos;

				moves HandAim;// = this->LEFTHAND.move;
				HandAim.pos = Leap(GunPos, GetFrameWorldMat(CharaFrame::LeftWrist).pos(), this->m_LeftHandPer);
				//基準
				auto vec_a1 = MATRIX_ref::Vtrans((HandAim.pos - GetFrameWorldMat(CharaFrame::LeftArm).pos()).Norm(), matBase);//基準
				auto vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(0.f, -1.f, vec_a1.y() / -abs(vec_a1.z()))).Norm();//x=0とする
				//vec_a1L1 = VECTOR_ref(VECTOR_ref::vget(0.f, -1.f, 0.f)).Norm();//x=0とする
				float cos_t = getcos_tri((GetFrameWorldMat(CharaFrame::LeftWrist).pos() - GetFrameWorldMat(CharaFrame::LeftArm2).pos()).size(), (GetFrameWorldMat(CharaFrame::LeftArm2).pos() - GetFrameWorldMat(CharaFrame::LeftArm).pos()).size(), (GetFrameWorldMat(CharaFrame::LeftArm).pos() - HandAim.pos).size());
				auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
				//上腕
				SetFrameLocalMat(CharaFrame::LeftArm, MGetIdent());
				MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(GetFrameWorldMat(CharaFrame::LeftArm2).pos() - GetFrameWorldMat(CharaFrame::LeftArm).pos(), matBase), vec_t);
				SetFrameLocalMat(CharaFrame::LeftArm, a1_inv);
				//下腕
				matBase = GetParentFrameWorldMat(CharaFrame::LeftArm2).GetRot().Inverse();
				SetFrameLocalMat(CharaFrame::LeftArm2, MGetIdent());
				MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
					MATRIX_ref::Vtrans(GetFrameWorldMat(CharaFrame::LeftWrist).pos() - GetFrameWorldMat(CharaFrame::LeftArm2).pos(), matBase),
					MATRIX_ref::Vtrans(HandAim.pos - GetFrameWorldMat(CharaFrame::LeftArm2).pos(), matBase));
				SetFrameLocalMat(CharaFrame::LeftArm2, a2_inv);
				//手
				matBase = GetParentFrameWorldMat(CharaFrame::LeftWrist).GetRot().Inverse();
				MATRIX_ref mat1;
				MATRIX_ref mat2 = GetFrameLocalMat(CharaFrame::LeftWrist);
				{
					auto Lmat = GetFrameLocalMat(CharaFrame::LeftWrist);
					auto Wmat = GetFrameWorldMat(CharaFrame::LeftWrist);
					auto zvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(1, -1, 0), Wmat.GetRot());
					auto yvec = MATRIX_ref::Vtrans(VECTOR_ref::vget(1, 1, 0), Wmat.GetRot());
					mat1 = MATRIX_ref::RotVec2(
						MATRIX_ref::Vtrans(zvec, matBase),
						MATRIX_ref::Vtrans(Gunzvec, matBase));
					SetFrameLocalMat(CharaFrame::LeftWrist, mat1);
					mat1 = MATRIX_ref::RotVec2(
						MATRIX_ref::Vtrans(yvec, matBase),
						MATRIX_ref::Vtrans(Gunyvec, matBase)) * mat1;

					mat1 = Lmat * mat1;
				}
				HandAim.mat = Leap(mat1, mat2, this->m_LeftHandPer);
				SetFrameLocalMat(CharaFrame::LeftWrist, HandAim.mat);
			}
			//
			void ExecuteHeartRate(void) noexcept {
				auto SE = SoundPool::Instance();
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
					if (!this->heartp) {
						this->heartp = true;
						SE->Get((int)SoundEnum::Heart).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper2).pos(), 12.5f * 1.f);
					}
				}
				else {
					this->heartp = false;
				}


				this->m_Stamina += std::clamp((100.f - this->m_HeartRate) / 40.f, -2.5f, 2.5f) / FPS;

				if (this->m_Squat.on()) {
					this->m_Stamina += 1.0f / FPS;
				}
				if (GetIsProne()) {
					this->m_Stamina += 1.5f / FPS;
				}

				if (this->m_IsSprint) {
					this->m_Stamina += -0.75f / FPS;
				}
				else {
					this->m_Stamina += 1.0f / FPS;
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
						this->m_Breath.play_3D(GetFrameWorldMat(CharaFrame::Upper2).pos(), 12.5f * 5.f);
					}
				}
				if (this->m_Breath.check()) {
					this->m_Breath.SetPosition(GetFrameWorldMat(CharaFrame::Upper2).pos());
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
				this->m_obj.get_anime((int)CharaAnimeID::Upper_RunningStart).GoEnd();
				//
				SetCreate3DSoundFlag(TRUE);
				this->m_Breath = SoundHandle::Load("data/Sound/SE/voice/WinningTicket/breath.wav");
				SetCreate3DSoundFlag(FALSE);
				this->m_Breath.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_Breath.get());
			}
			void Execute(void) noexcept override {
				if (this->m_IsFirstLoop) {
					for (int i = 0; i < this->m_obj.get_anime().size(); i++) { this->m_obj.get_anime(i).per = 0.f; }
					this->m_obj.get_anime((int)CharaAnimeID::RightHand).per = 1.f;
					this->m_obj.work_anime();
					this->m_RightWristRot = GetFrameLocalMat(CharaFrame::RightWrist).GetRot();
				}
				ExecuteSavePrev();
				//操作//0.01ms
				{
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
						if ((this->m_Press_Shot || this->m_Press_Reload) && (this->m_ShotPhase == 0)) {
							if (this->m_ReadyTimer >= 0.1f) {
								this->m_ReadyTimer = 0.1f;
							}
							if (this->m_ReadyPer >= 0.9f) {
								this->m_ReadyPer = 1.f;
								if (this->m_Press_Shot) {
									this->m_ShotPhase = 1;
									if (this->m_Gun_Ptr != nullptr) {
										this->m_Gun_Ptr->SetBullet();
									}
								}
								if (this->m_Press_Reload) {
									this->m_ShotPhase = 3;
								}
							}
						}
						else {
							this->m_ReadyTimer += 1.f / FPS;
							this->m_ReadyTimer = std::clamp(this->m_ReadyTimer, 0.f, this->m_Press_Aim ? 0.f : UpperTimerLimit);
							this->m_ReadyTimer = std::clamp(this->m_ReadyTimer, 0.f, GetIsProne() ? 0.1f : UpperTimerLimit);
							if (GetIsProne() && this->m_MoveVector > 0.6f) {
								this->m_ReadyTimer = UpperTimerLimit;
							}
						}
						if ((this->m_ShotPhase >= 2) || this->m_IsRun) {
							this->m_ReadyTimer = UpperTimerLimit;
						}
					}
					else {
						this->m_ReadyTimer += 1.f / FPS;
						this->m_ReadyTimer = std::clamp(this->m_ReadyTimer, 0.f, GetIsProne() ? 0.1f : UpperTimerLimit);
						if (GetIsProne() && this->m_MoveVector > 0.6f) {
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

					if (!GetIsProne() && (this->m_PronePer == 1.f)) {
						if (!this->m_PronetoStanding) {
							this->m_obj.get_anime((int)CharaAnimeID::All_PronetoStand).GoStart();
							this->m_PronePer2 = 0.f;
						}
						this->m_PronetoStanding = true;
					}
					if (this->m_PronetoStanding && (this->m_PronePer == 0.f)) {
						this->m_PronetoStanding = false;
					}

					if (this->m_PronetoStanding) {
						if ((this->m_obj.get_anime((int)CharaAnimeID::All_PronetoStand).GetTimePer()) > 0.9f) {
							easing_set(&this->m_PronePer2, 1.f, 0.95f);
						}
					}

					easing_set(&this->m_PronePer, GetIsProne() ? 1.f : 0.f, 0.95f);
					if (!GetIsProne() && (0.01f >= this->m_PronePer)) { this->m_PronePer = 0.f; }
					if (GetIsProne() && (0.99f <= this->m_PronePer)) { this->m_PronePer = 1.f; }

					//this->m_PronePer = std::clamp(this->m_PronePer + (GetIsProne() ? 1.f : -3.f) / FPS, 0.f, 1.f);
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
						if (this->m_TurnBody || (this->m_MoveVector > 0.1f) || GetIsProne()) { easing_set(&this->m_yrad_Upper, this->m_rad.y(), TmpRunPer); }
						auto OLDP = this->m_yrad_Bottom;
						easing_set(&this->m_yrad_Bottom, this->m_yrad_Upper - FrontP * (1.f - this->m_PronePer), TmpRunPer);
						{
							auto zbuf = this->m_rad_Buf.z();
							easing_set(&zbuf, (this->m_yrad_Bottom - OLDP) * 2.f, 0.9f);
							this->m_rad_Buf.z(zbuf);
						}
					}
				}
				//上半身回転//0.06ms
				{
					this->m_UpperMatrix = MATRIX_ref::RotX(this->m_rad.x()) * MATRIX_ref::RotY(this->m_rad.y() - this->m_yrad_Bottom);
					this->m_obj.frame_Reset(this->Frames[(int)CharaFrame::Upper].first);
					SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * this->m_UpperMatrix);
				}
				//AnimUpdte//0.03ms
				ExecuteAnim();
				//SetMat指示//0.03ms
				{
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
						}
						else {
							this->m_move.vec.yadd(M_GR / (FPS * FPS));
						}
					}
					this->m_posBuf += this->m_move.vec;
					col_wall(OLDpos, &this->m_posBuf, *this->m_MapCol);
					this->m_move.mat = MATRIX_ref::RotZ(this->m_rad.z()) * MATRIX_ref::RotY(this->m_yrad_Bottom)
						* MATRIX_ref::RotVec2(VECTOR_ref::up(), Leap(VECTOR_ref::up(), this->m_ProneNormal, this->m_PronePer));
					easing_set(&this->m_move.pos, this->m_posBuf, 0.9f);
					UpdateMove();
				}
				//銃座標指定(アニメアップデート含む)//0.19ms
				if (this->m_Gun_Ptr != nullptr) {
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


						this->m_obj.get_anime((int)CharaAnimeID::LeftHand).per = ((
							(UpperAnimSelect != CharaAnimeID::Upper_RunningStart)
							&& (UpperAnimSelect != CharaAnimeID::Upper_Running)
							&& (UpperAnimSelect != CharaAnimeID::Upper_Sprint)
							&& (UpperAnimSelect != CharaAnimeID::Upper_Ready)

							&& (this->m_AnimPerBuf[(int)CharaAnimeID::All_ProneWalk] <= 0.1f)
							&& (this->m_AnimPerBuf[(int)CharaAnimeID::All_PronetoStand] <= 0.1f)
							) ? 1.f : 0.f);

						printfDx("  %f\n", this->m_obj.get_anime((int)CharaAnimeID::LeftHand).per);

						//アニメアップデート
						this->m_obj.work_anime();//0.35ms

						auto mat = (GetFrameWorldMat(CharaFrame::RightWrist).GetRot()*GetCharaDir().Inverse());
						yVec1 = MATRIX_ref::Vtrans(VECTOR_ref::vget(0, 0, -1).Norm(), mat);
						zVec1 = MATRIX_ref::Vtrans(VECTOR_ref::vget(-1, -1, 0).Norm(), mat);
						Pos1 = GetFrameWorldMat(CharaFrame::RightHandJoint).pos();

						if (change) {
							this->m_obj.get_anime((int)GetProneAimAnimSel()).per = 0.f;
							this->m_obj.get_anime(i).per = 1.f;
						}
						this->m_obj.get_anime((int)CharaAnimeID::RightHand).per = (this->m_ShotPhase < 2) ? ((!this->m_RunReady || (UpperAnimSelect != CharaAnimeID::Upper_Ready)) ? 1.f : 0.f) : 0.f;
						//必要最低限のアニメ再更新
						this->m_obj.work_anime((int)GetProneAimAnimSel());
						this->m_obj.work_anime(i);
						this->m_obj.work_anime((int)CharaAnimeID::RightHand);
					}
					//背負い場所探索
					VECTOR_ref yVec2, zVec2, Pos2;
					{
						yVec2 = (MATRIX_ref::RotZ(deg2rad(30)) * GetFrameWorldMat(CharaFrame::Upper2).GetRot() * GetCharaDir().Inverse()).xvec();
						zVec2 = (MATRIX_ref::RotZ(deg2rad(30)) * GetFrameWorldMat(CharaFrame::Upper2).GetRot() * GetCharaDir().Inverse()).yvec();
						Pos2 =
							GetFrameWorldMat(CharaFrame::Upper2).pos() +
							GetFrameWorldMat(CharaFrame::Upper2).GetRot().yvec() * -1.75f +
							GetFrameWorldMat(CharaFrame::Upper2).GetRot().zvec() * 1.75f;
					}
					auto yVec = Leap(yVec1, yVec2, this->m_SlingPer);
					auto zVec = Leap(zVec1, zVec2, this->m_SlingPer);
					auto PosBuf = Leap(Pos1, Pos2, this->m_SlingPer);
					auto tmp_gunrat = MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, -1).Norm(), zVec);
					tmp_gunrat *= MATRIX_ref::RotVec2(tmp_gunrat.yvec(), yVec);
					tmp_gunrat *= GetCharaDir() * MATRIX_ref::Mtrans(PosBuf);
					this->m_Gun_Ptr->SetGunMatrix(tmp_gunrat, this->m_ShotPhase - 2);
					//弾を手持ち//スコープ内0.01ms
					{
						auto Thumb = GetFrameWorldMat(CharaFrame::RightThumb);
						auto Pointer = GetFrameWorldMat(CharaFrame::RightPointer);
						MATRIX_ref mat =
							MATRIX_ref::RotVec2(VECTOR_ref::vget(0, 0, -1), MATRIX_ref::Vtrans(VECTOR_ref::vget(-1, -1, 0), Pointer.GetRot()))
							* MATRIX_ref::Mtrans((Thumb.pos() + Pointer.pos()) / 2.f);
						this->m_Gun_Ptr->SetAmmoHandMatrix(mat, this->m_LoadAmmoPer);
					}
					//左手座標指定
					{
						move_LeftArm();
					}
				}
				//顔//スコープ内0.01ms
				{
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
				}
				//心拍数//0.00ms
				ExecuteHeartRate();
			}

			void Draw(void) noexcept override {
				MATRIX_ref Mat1 = GetFrameWorldMat(CharaFrame::LeftArm);
				MATRIX_ref Mat2 = GetFrameWorldMat(CharaFrame::LeftArm2);
				MATRIX_ref Mat3 = GetFrameWorldMat(CharaFrame::LeftWrist);
				MATRIX_ref GunMat = this->m_Gun_Ptr->GetFrameWorldMat(GunFrame::LeftHandPos);

				DrawLine3D(Mat2.pos().get(), GunMat.pos().get(), GetColor(255, 0, 0));

				ObjectBaseClass::Draw();
			}

		public:
			void ValueSet(float pxRad, float pyRad, bool SquatOn, bool ProneOn, const VECTOR_ref& pPos) {
				for (int i = 0; i < 4; i++) {
					this->m_Vec[i] = 0.f;
				}
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					this->m_AnimPerBuf[i] = 0.f;
				}
				this->m_ReadyPer = 0.f;
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
				this->m_LeftHandPer = 0.f;
				this->m_TurnBody = false;
				this->m_ShotPhase = 0;
				this->m_IsSprint = false;
				this->m_RunReady = false;
				this->m_Running = false;
				this->m_Press_GoFront = false;
				this->m_Press_GoRear = false;
				this->m_Press_GoLeft = false;
				this->m_Press_GoRight = false;
				this->m_Press_Shot = false;
				this->m_Press_Reload = false;
				this->m_Press_Aim = false;
				this->m_Ready_Start = false;
				this->m_IsRun = false;
				this->heartp = false;
				//
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
				this->m_move.vec.clear();
				this->m_UpperMatrix = MATRIX_ref::RotX(this->m_rad.x()) * MATRIX_ref::RotY(this->m_rad.y() - this->m_yrad_Bottom);
				SetMove(MATRIX_ref::RotY(this->m_yrad_Bottom), this->m_posBuf);
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
				bool pRunPress,
				bool pReady,
				bool pQPress,
				bool pEPress,
				bool pReloadPress
			) {
				this->m_ReadySwitch = (this->m_Ready_Start != pReady);
				this->m_Ready_Start = pReady;
				this->m_Press_GoFront = pGoFrontPress && this->m_Ready_Start;
				this->m_Press_GoRear = pGoBackPress && this->m_Ready_Start;
				this->m_Press_GoLeft = pGoLeftPress && this->m_Ready_Start;
				this->m_Press_GoRight = pGoRightPress && this->m_Ready_Start;
				if (GetIsProne() && (!(this->m_ShotPhase == 0))) {
					this->m_Press_GoFront = false;
					this->m_Press_GoRear = false;
					this->m_Press_GoLeft = false;
					this->m_Press_GoRight = false;
				}
				this->m_Press_Shot = pShotPress && this->m_Ready_Start;
				this->m_Press_Reload = (pReloadPress && this->m_Ready_Start && (this->m_Gun_Ptr->GetAmmoNum() <= this->m_Gun_Ptr->GetAmmoAll()));

				if (0.f != this->m_PronePer && this->m_PronePer != 1.0f) {
					if (this->m_Press_Shot) {
						this->m_Press_Shot = false;
					}
					this->m_Press_Reload = false;
				}
				if (!GetCanshot()) {
					if (this->m_ShotPhase == 0) {
						if (this->m_Press_Shot) {
							this->m_Press_Shot = false;
							this->m_Press_Reload = true;
						}
					}
				}

				this->m_Press_Aim = pAimPress && this->m_Ready_Start && (this->m_PronePer == 0.f || this->m_PronePer == 1.f);

				if (this->m_PronetoStanding) {
					this->m_Press_Aim = false;
					this->m_ReadyTimer = UpperTimerLimit;
				}

				if (!this->m_IsRun& (pRunPress&& this->m_Ready_Start)) {
					this->m_RunTimer = 1.f;
				}
				if (this->m_RunTimer > 0.f) {
					this->m_RunTimer -= 1.f / FPS;
					this->m_IsRun = true;
				}
				else {
					this->m_RunTimer = 0.f;
					this->m_IsRun = (pRunPress&& this->m_Ready_Start);
				}
				//
				if (this->m_Press_GoRear || (!this->m_Press_GoFront && (this->m_Press_GoLeft || this->m_Press_GoRight))) {
					this->m_IsRun = false;
				}
				if (GetIsProne() || this->m_PronetoStanding) {
					this->m_IsRun = false;
				}

				this->m_IsSprint = this->m_IsRun && (!this->m_Press_GoFront && !this->m_Press_GoRear);

				if (this->m_IsSprint) {
					if (this->m_CannotSprint) {
						this->m_Press_GoFront = true;
						this->m_IsSprint = false;
					}
				}

				this->m_Squat.GetInput(pSquatPress&& this->m_Ready_Start);
				if (this->m_IsRun) { this->m_Squat.first = false; }

				this->m_Prone.GetInput((pPronePress&& this->m_Ready_Start) && (this->m_ShotPhase == 0) && (this->m_PronePer == 0.f || this->m_PronePer == 1.f));
				if (GetIsProne()) { this->m_Squat.first = false; }
				{
					auto TmpReady = !(!this->m_IsRun || (this->m_ShotPhase >= 2));
					this->m_RunReadyFirst = (TmpReady && !this->m_RunReady);
					this->m_RunReady = TmpReady;
				}
				{
					QKey.GetInput(pQPress && this->m_Ready_Start);
					EKey.GetInput(pEPress && this->m_Ready_Start);
					if (EKey.trigger()) {
						if (this->m_TurnRate > -1) {
							this->m_TurnRate--;
						}
						else {
							this->m_TurnRate++;
						}
					}
					if (QKey.trigger()) {
						if (this->m_TurnRate < 1) {
							this->m_TurnRate++;
						}
						else {
							this->m_TurnRate--;
						}
					}
					if (!this->m_IsRun) {
						this->m_TurnRate = 0;
					}

					this->m_TurnRate = std::clamp(this->m_TurnRate, -1, 1);
					float xadd = 0.f;
					if (this->m_IsSprint) {
						xadd = 0.315f*(-this->m_TurnRate);//スプリント
					}
					else if (this->m_IsRun) {
						xadd = 0.2f*(-this->m_TurnRate);//走り
					}
					easing_set(&this->m_TurnRatePer, xadd, 0.9f);
				}
				{
					auto limchange = Leap(Leap(1.f, powf(1.f - this->m_Vec[0], 0.5f), this->m_RunPer * 0.8f), 0.15f, this->m_PronePer);
					auto tmp = Leap(1.f, 0.1f, this->m_PronePer);
					auto tmp2 = 0.2f * GetRandf(deg2rad(1.f - this->m_PronePer));
					auto tmp3 = Leap(0.5f, Leap(0.35f, 0.2f, this->m_SquatPer), this->m_PronePer);

					easing_set(&this->m_xrad_Add, tmp2 + 0.0002f * sin(this->m_HeartRateRad) * powf(this->m_HeartRate / HeartRateMin, 3.f), 0.95f);
					easing_set(&this->m_yrad_Add, tmp2 + 0.0002f * sin(this->m_HeartRateRad * 3) * powf(this->m_HeartRate / HeartRateMin, 3.f), 0.95f);

					this->m_rad_Buf.x(std::clamp(this->m_rad_Buf.x() + pAddxRad * tmp, -deg2rad(40.f) * limchange, deg2rad(25.f) * limchange));
					this->m_rad_Buf.yadd((pAddyRad + this->m_TurnRatePer / 100.f) * tmp);
					this->m_rad_Buf.xadd(this->m_xrad_Add * tmp3);
					this->m_rad_Buf.yadd(this->m_yrad_Add * tmp3);
					easing_set(&this->m_rad, this->m_rad_Buf, 0.5f);
				}
			}
			void SetEyeVec(const VECTOR_ref& camvec) noexcept {
				this->m_obj.frame_Reset(this->Frames[(int)CharaFrame::Head].first);
				auto v1 = (GetFrameWorldMat(CharaFrame::Head).GetRot() * GetCharaDir().Inverse()).zvec()*-1.f;
				auto v2 = Leap(MATRIX_ref::Vtrans(camvec.Norm(), GetCharaDir().Inverse()), v1, m_NeckPer);

				auto radlimit = deg2rad(70);
				if (v1.dot(v2) <= cos(radlimit)) {
					v2 = v1 * cos(radlimit) + v1.cross(v1.cross(v2)) * (-sin(radlimit));
				}
				SetFrameLocalMat(CharaFrame::Head, MATRIX_ref::RotVec2(v1, v2) * GetFrameLocalMat(CharaFrame::Head).GetRot());
			}
		};
	};
};
