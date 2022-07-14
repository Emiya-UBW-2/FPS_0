#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//入力
		class CharacterMoveGroundControl {
		private:
			std::array<float, 4>								m_Vec{ 0,0,0,0 };

			bool												m_KeyActive{ true };
			bool												m_Press_GoFront{ false };
			bool												m_Press_GoRear{ false };
			bool												m_Press_GoLeft{ false };
			bool												m_Press_GoRight{ false };
			switchs m_Squat;
			switchs m_Prone;
			switchs QKey;
			switchs EKey;

			bool												m_IsRun{ false };
			bool												m_IsSprint{ false };
			float												m_RunPer{ 0.f };
			float												m_RunTimer{ 0.f };
			float												m_SprintPer{ 0.f };

			float												m_SquatPer{ 0.f };
			float												m_PronePer{ 0.f };

			VECTOR_ref											m_rad_Buf, m_rad, m_radAdd;
			bool												m_PronetoStanding{ false };

			int													m_TurnRate{ 0 };
			float												m_TurnRatePer{ 0.f };
		private: //内部
			void			SetVec(int pDir, bool Press) {
				this->m_Vec[pDir] += (Press ? 1.f : -1.f)*2.f / FPS;
				this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
			}
		public:
			const auto		GetTurnRatePer() const noexcept { return  this->m_TurnRatePer; }
			const auto		GetRad() const noexcept { return  this->m_rad; }
			
			const auto		GetIsSquat(void) const noexcept { return this->m_Squat.on(); }
			const auto		GetSquatPer(void) const noexcept { return this->m_SquatPer; }
			const auto		GetIsProne(void) const noexcept { return this->m_Prone.on(); }
			const auto		GetPronePer(void) const noexcept { return this->m_PronePer; }

			const auto		GetVecFront() const noexcept { return  this->m_Vec[0] || m_IsSprint; }
			const auto		GetVecRear() const noexcept { return this->m_Vec[2]; }
			const auto		GetVecLeft() const noexcept { return this->m_Vec[1]; }
			const auto		GetVecRight() const noexcept { return this->m_Vec[3]; }

			const auto		GetPressFront() const noexcept { return this->m_Press_GoFront; }
			const auto		GetPressRear() const noexcept { return this->m_Press_GoRear; }
			const auto		GetPressLeft() const noexcept { return this->m_Press_GoLeft; }
			const auto		GetPressRight() const noexcept { return this->m_Press_GoRight; }

			const auto		GetRun() const noexcept { return this->m_IsRun; }
			const auto		GetRunPer() const noexcept { return  this->m_RunPer; }
			const auto		GetSprint() const noexcept { return this->m_IsSprint; }

			void			GetSprintPer(float value) noexcept { this->m_SprintPer = value; }
			const auto		GetSprintPer() const noexcept { return  this->m_SprintPer; }

			void			SetPronetoStanding(bool value) noexcept { this->m_PronetoStanding = value; }
			const auto		GetPronetoStanding() const noexcept { return  this->m_PronetoStanding; }

			const auto		GetVec() const noexcept { return VECTOR_ref::vget(GetVecLeft() - GetVecRight(), 0, GetVecRear() - GetVecFront()); }
			const auto		GetFrontP() const noexcept {
				auto FrontP = ((GetPressFront() && !GetPressRear())) ? (atan2f(GetVec().x(), -GetVec().z()) * GetVecFront()) : 0.f;
				FrontP += (!GetPressFront() && GetPressRear()) ? (atan2f(-GetVec().x(), GetVec().z()) * GetVecRear()) : 0.f;
				return FrontP;
			}
			void			SetRadBufZ(float z) noexcept {
				auto zbuf = this->m_rad_Buf.z();
				Easing(&zbuf, z, 0.9f, EasingType::OutExpo);
				this->m_rad_Buf.z(zbuf);
			}
		public:
			void			ValueSet(float pxRad, float pyRad, bool SquatOn, bool ProneOn) {
				for (int i = 0; i < 4; i++) {
					this->m_Vec[i] = 0.f;
				}
				this->m_KeyActive = false;
				this->m_Press_GoFront = false;
				this->m_Press_GoRear = false;
				this->m_Press_GoLeft = false;
				this->m_Press_GoRight = false;
				this->m_radAdd.clear();
				this->m_IsRun = false;
				this->m_IsSprint = false;
				this->m_RunPer = 0.f;
				this->m_RunTimer = 0.f;
				this->m_SprintPer = 0.f;
				//動作にかかわる操作
				this->m_rad_Buf.x(pxRad);
				this->m_rad_Buf.y(pyRad);
				this->m_Squat.Init(SquatOn);
				this->m_Prone.Init(ProneOn);
				//上記を反映するもの
				this->m_rad = this->m_rad_Buf;
				this->m_SquatPer = SquatOn ? 1.f : 0.f;
				this->m_PronePer = ProneOn ? 1.f : 0.f;
			}
			void			SetInput(
				float pAddxRad, float pAddyRad,
				const VECTOR_ref& pAddRadvec,
				bool pGoFrontPress,
				bool pGoBackPress,
				bool pGoLeftPress,
				bool pGoRightPress,
				bool pSquatPress,
				bool pPronePress,
				bool pRunPress,
				bool pReady,
				bool pQPress,
				bool pEPress,
				bool pCannotSprint
			) {
				this->m_KeyActive = pReady;
				this->m_Press_GoFront = pGoFrontPress && this->m_KeyActive;
				this->m_Press_GoRear = pGoBackPress && this->m_KeyActive;
				this->m_Press_GoLeft = pGoLeftPress && this->m_KeyActive;
				this->m_Press_GoRight = pGoRightPress && this->m_KeyActive;

				if (!this->m_IsRun& (pRunPress&& this->m_KeyActive)) {
					this->m_RunTimer = 1.f;
				}
				if (this->m_RunTimer > 0.f) {
					this->m_RunTimer -= 1.f / FPS;
					this->m_Press_GoFront = true;
					this->m_IsRun = true;
				}
				else {
					this->m_RunTimer = 0.f;
					this->m_IsRun = (pRunPress&& this->m_KeyActive);
				}
				if (GetPressRear() || (!GetPressFront() && (GetPressLeft() || GetPressRight()))) {
					this->m_IsRun = false;
				}
				if (GetIsProne() || GetPronetoStanding()) {
					this->m_IsRun = false;
				}

				this->m_IsSprint = this->m_IsRun && (!GetPressFront() && !GetPressRear());

				if (this->m_IsSprint) {
					if (pCannotSprint) {
						this->m_Press_GoFront = true;
						this->m_IsSprint = false;
					}
				}

				this->m_Squat.GetInput(pSquatPress && this->m_KeyActive);
				if (this->m_IsRun) { this->m_Squat.first = false; }

				this->m_Prone.GetInput((pPronePress && this->m_KeyActive) && (this->m_PronePer == 0.f || this->m_PronePer == 1.f));
				if (GetIsProne()) { this->m_Squat.first = false; }

				Easing(&this->m_PronePer, GetIsProne() ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
				if (!GetIsProne() && (0.01f >= this->m_PronePer)) { this->m_PronePer = 0.f; }
				if (GetIsProne() && (0.99f <= this->m_PronePer)) { this->m_PronePer = 1.f; }
				//this->m_PronePer = std::clamp(this->m_PronePer + (GetIsProne() ? 1.f : -3.f) / FPS, 0.f, 1.f);
				{
					QKey.GetInput(pQPress && this->m_KeyActive);
					EKey.GetInput(pEPress && this->m_KeyActive);
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
					if (!GetRun()) {
						this->m_TurnRate = 0;
					}

					this->m_TurnRate = std::clamp(this->m_TurnRate, -1, 1);
					float xadd = 0.f;
					if (this->m_IsSprint) {
						xadd = 0.315f*(-this->m_TurnRate);//スプリント
					}
					else if (GetRun()) {
						xadd = 0.2f*(-this->m_TurnRate);//走り
					}
					Easing(&this->m_TurnRatePer, xadd, 0.9f, EasingType::OutExpo);
				}
				//回転
				{
					auto limchange = Leap(Leap(1.f, powf(1.f - this->GetVecFront(), 0.5f), this->m_RunPer * 0.8f), 0.15f, this->GetPronePer());
					auto tmp = Leap(1.f, 0.1f, this->GetPronePer());
					Easing(&this->m_radAdd, pAddRadvec, 0.95f, EasingType::OutExpo);

					this->m_rad_Buf.x(
						std::clamp(this->m_rad_Buf.x() + pAddxRad * tmp, -deg2rad(40.f) * limchange, deg2rad(25.f) * limchange)
						+ this->m_radAdd.x()
					);
					this->m_rad_Buf.y(
						this->m_rad_Buf.y() + (pAddyRad + this->m_TurnRatePer / 100.f)*tmp
						+ this->m_radAdd.y()
					);

					Easing(&this->m_rad, this->m_rad_Buf, 0.5f, EasingType::OutExpo);
				}
			}
			void			Execute(void) noexcept {
				//移動ベクトル取得
				{
					SetVec(0, GetPressFront());
					SetVec(1, GetPressLeft());
					SetVec(2, GetPressRear());
					SetVec(3, GetPressRight());
				}
				//
				Easing(&this->m_RunPer, this->m_IsRun ? 1.f : 0.f, 0.975f, EasingType::OutExpo);
				Easing(&this->m_SprintPer, this->m_IsSprint ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
				Easing(&this->m_SquatPer, GetIsSquat() ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
			}
		};

		class CharacterClass : public ObjectBaseClass {
		private://キャラパラメーター
			const float SpeedLimit{ 2.f };
			const float UpperTimerLimit = 3.f;
			const float HeartRateMin{ 60.f };//心拍数最小
			const float HeartRateMax{ 180.f };//心拍数最大
			const float StaminaMax{ 100.f };
		private:
			CharacterMoveGroundControl							m_InputGround;
			bool												m_KeyActive{ true };
			//飛行関連
			VECTOR_ref											m_FlightVecBuf;
			MATRIX_ref											m_FlightMatrix;
			VECTOR_ref											m_FradAdd_Buf, m_FradAdd;
			float												m_FlightPer{ 0.f };
			bool												m_Flightmode{ false };
			//
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{ 0 };
			VECTOR_ref											m_PosBuf;
			MATRIX_ref											m_UpperMatrix;
			float												m_Speed{ 0.f };
			float												m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float												m_RunPer2{ 0.f }, m_PrevRunPer2{ 0.f };
			float												m_NeckPer{ 0.f };
			float m_SlingPer{ 0.f };
			float m_PronePer2{ 0.f };
			VECTOR_ref m_ProneNormal{ VECTOR_ref::up() };
			bool												m_TurnBody{ false };
			bool												m_ReturnStand{ false };
			bool m_RunReady{ false };
			bool m_RunReadyFirst{ false };
			bool m_Running{ false };
			CharaAnimeID										m_UpperAnimSelect, m_PrevUpperAnimSel;
			CharaAnimeID										m_BottomAnimSelect;
			MATRIX_ref											m_RightWristRot;
			float												m_Score{ 0.f };			//スコア
			//銃
			float m_ReadyTimer{ 0.f };
			float m_ReadyPer{ 0.f };
			int m_ShotPhase{ 0 };
			float m_LoadAmmoPer{ 0.f };
			//入力
			bool m_Press_Shot{ false };
			bool m_Press_Reload{ false };
			bool m_Press_Aim{ false };
			bool m_ReadySwitch{ false };
			//体力
			float												m_HeartRate{ HeartRateMin };//心拍数
			float												m_HeartRate_r{ HeartRateMin };//心拍数
			float												m_HeartRateRad{ 0.f };//呼吸Sin渡し
			float												m_Stamina{ StaminaMax };//スタミナ
			bool												m_CannotSprint{ false };//スタミナ切れ

			float m_LeftHandPer{ 0.f };
			//表情
			int													m_Eyeclose{ 0 };
			float												m_EyeclosePer{ 0.f };
			//サウンド
			int													m_CharaSound{ -1 };
			SoundHandle											m_Breath;
			bool												m_HeartSoundFlag{ false };
			std::shared_ptr<GunClass>							m_Gun_Ptr{ nullptr };			//銃
		public://ゲッター
			//
			const auto		GetFrameLocalMat(CharaFrame frame) const noexcept { return this->m_obj.GetFrameLocalMatrix(Frames[(int)frame].first); }
			const auto		GetParentFrameLocalMat(CharaFrame frame) const noexcept { return this->m_obj.GetFrameLocalMatrix((int)this->m_obj.frame_parent(Frames[(int)frame].first)); }
			const auto		GetFrameWorldMat(CharaFrame frame) const noexcept { return this->m_obj.GetFrameLocalWorldMatrix(Frames[(int)frame].first); }
			const auto		GetParentFrameWorldMat(CharaFrame frame) const noexcept { return this->m_obj.GetFrameLocalWorldMatrix((int)this->m_obj.frame_parent(Frames[(int)frame].first)); }
			void			ResetFrameLocalMat(CharaFrame frame) noexcept { this->m_obj.frame_Reset(Frames[(int)frame].first); }
			void			SetFrameLocalMat(CharaFrame frame, const MATRIX_ref&value) noexcept { this->m_obj.SetFrameLocalMatrix(Frames[(int)frame].first, value * Frames[(int)frame].second); }
			void			SetShapePer(CharaShape pShape, float Per) noexcept { Shapes[(int)pShape].second = Per; }

			auto&			GetAnimeBuf(CharaAnimeID anim) noexcept { return this->m_AnimPerBuf[(int)anim]; }
			auto&			GetAnime(CharaAnimeID anim) noexcept { return this->m_obj.get_anime((int)anim); }
			void			SetAnimOnce(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimOnce((int)ID, speed); }
			void			SetAnimLoop(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimLoop((int)ID, speed); }

			//
			const auto		GetPressFrontGround() const noexcept { return this->m_InputGround.GetPressFront() && !this->m_Flightmode; }
			const auto		GetPressRearGround() const noexcept { return this->m_InputGround.GetPressRear() && !this->m_Flightmode; }
			const auto		GetPressLeftGround() const noexcept { return this->m_InputGround.GetPressLeft() && !this->m_Flightmode; }
			const auto		GetPressRightGround() const noexcept { return this->m_InputGround.GetPressRight() && !this->m_Flightmode; }
			//
			void SetGunPtr(std::shared_ptr<GunClass>& pGunPtr) noexcept { this->m_Gun_Ptr = pGunPtr; }
			void LoadReticle(void) noexcept {
				if (this->m_Gun_Ptr != nullptr) {
					this->m_Gun_Ptr->LoadReticle();
				}
			}
			void AddScore(float value) noexcept { this->m_Score += value; }
			void SetScore(float value) noexcept { this->m_Score = value; }
			auto&			GetGunPtr(void) noexcept { return this->m_Gun_Ptr; }
			const auto		GetShotSwitch(void) const noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetIsShot() : false; }
			const auto		GetCharaDir(void) const noexcept {
				if (!this->m_Flightmode) {
					return this->m_UpperMatrix * this->m_move.mat;
				}
				else {
					return this->m_UpperMatrix * this->m_FlightMatrix;
				}
			}
			const auto		GetEyeVector(void) const noexcept { return (this->m_Gun_Ptr != nullptr) ? (Leap(GetCharaDir().zvec(), this->m_Gun_Ptr->GetMatrix().zvec(), this->m_ReadyPer) * -1.f) : (GetCharaDir().zvec() * -1.f); }
			const auto		GetEyePosition(void) const noexcept { return (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f + this->GetEyeVector().Norm() * 0.5f; }
			const auto		GetScopePos(void) noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetScopePos() : GetEyePosition(); }
			const auto		GetLensPos(void) noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetLensPos() : VECTOR_ref::zero(); }
			const auto		GetReticlePos(void) noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetReticlePos() : VECTOR_ref::zero(); }
			const auto		GetLensPosSize(void) noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetLensPosSize() : VECTOR_ref::zero(); }
			const auto		GetCanshot(void) const noexcept { return (this->m_Gun_Ptr != nullptr) ? (this->m_Gun_Ptr->GetCanshot() && (this->m_ShotPhase <= 1)) : false; }
			const auto		GetIsEmpty(void) const noexcept { return (this->m_Gun_Ptr != nullptr) ? this->m_Gun_Ptr->GetIsEmpty() : false; }
			const auto		GetAmmoNum(void) const noexcept { return this->m_Gun_Ptr->GetAmmoNum(); }
			const auto		GetAmmoAll(void) const noexcept { return this->m_Gun_Ptr->GetAmmoAll(); }
			const auto&		GetReticle(void) noexcept { return this->m_Gun_Ptr->GetReticle(); }
			const auto&		GetScore(void) const noexcept { return this->m_Score; }
			const auto		GetIsRun(void) const noexcept { return this->m_InputGround.GetRun(); }
			const auto&		GetHeartRate(void) const noexcept { return this->m_HeartRate; }
			const auto&		GetHeartRateRad(void) const noexcept { return this->m_HeartRateRad; }
			const auto&		GetFlightPer(void) const noexcept { return this->m_FlightPer; }

			const auto		GetHeartRandVec(void) const noexcept {
				auto tmp2 = 0.2f * GetRandf(deg2rad(1.f - this->m_InputGround.GetPronePer()));
				auto tmp3 = Leap(0.5f, Leap(0.35f, 0.2f, this->m_InputGround.GetSquatPer()), this->m_InputGround.GetPronePer());
				VECTOR_ref tmpvec = VECTOR_ref::vget(
					tmp2 + 0.0002f * sin(this->m_HeartRateRad) * powf(this->m_HeartRate / HeartRateMin, 3.f),
					tmp2 + 0.0002f * sin(this->m_HeartRateRad * 3) * powf(this->m_HeartRate / HeartRateMin, 3.f),
					0.f
				);
				return tmpvec * tmp3;
			}
			const auto&		GetStamina(void) const noexcept { return this->m_Stamina; }
			const auto&		GetStaminaMax(void) const noexcept { return this->StaminaMax; }
			const auto		GetTurnRatePer(void) const noexcept { return this->m_InputGround.GetTurnRatePer(); }
			const auto		GetIsProne(void) const noexcept { return this->m_InputGround.GetIsProne(); }
			const auto		GetProneShotAnimSel(void) const noexcept { return (GetIsProne()) ? CharaAnimeID::All_ProneShot : CharaAnimeID::Upper_Shot; }
			const auto		GetProneCockingAnimSel(void) const noexcept { return (GetIsProne()) ? CharaAnimeID::All_ProneCocking : CharaAnimeID::Upper_Cocking; }
			const auto		GetProneAimAnimSel(void) const noexcept { return (GetIsProne()) ? CharaAnimeID::All_Prone : CharaAnimeID::Upper_Aim; }
			const auto		GetProneReloadStartAnimSel(void) const noexcept { return (GetIsProne()) ? CharaAnimeID::All_Prone_ReloadStart : CharaAnimeID::Upper_ReloadStart; }
			const auto		GetProneReloadOneAnimSel(void) const noexcept { return (GetIsProne()) ? CharaAnimeID::All_Prone_ReloadOne : CharaAnimeID::Upper_ReloadOne; }
			const auto		GetProneReloadEndAnimSel(void) const noexcept { return (GetIsProne()) ? CharaAnimeID::All_Prone_ReloadEnd : CharaAnimeID::Upper_ReloadEnd; }
			const auto		GetIsADS(void) const noexcept { return this->m_ReadyTimer == 0.f; }
			const auto		GetReadyPer(void) const noexcept { return this->m_ReadyPer; }
		private: //更新関連
			//以前の状態保持														//
			void			ExecuteSavePrev(void) noexcept {
				this->m_PrevUpperAnimSel = this->m_UpperAnimSelect;
				this->m_PrevRunPer2 = this->m_RunPer2;
			}
			//操作																	//0.01ms
			void			ExecuteInput(void) noexcept {
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
						if (GetIsProne() && this->m_Speed > 0.6f) {
							this->m_ReadyTimer = UpperTimerLimit;
						}
					}
					if ((this->m_ShotPhase >= 2) || GetIsRun()) {
						this->m_ReadyTimer = UpperTimerLimit;
					}
				}
				else {
					this->m_ReadyTimer += 1.f / FPS;
					this->m_ReadyTimer = std::clamp(this->m_ReadyTimer, 0.f, GetIsProne() ? 0.1f : UpperTimerLimit);
					if (GetIsProne() && this->m_Speed > 0.6f) {
						this->m_ReadyTimer = UpperTimerLimit;
					}
					if (GetIsRun()) {
						this->m_ReadyTimer = UpperTimerLimit;
					}
				}
				//
				Easing(&this->m_FlightPer, this->m_Flightmode ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
				Easing(&this->m_ReadyPer, (this->m_ReadyTimer < UpperTimerLimit) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				{
					if (!GetIsProne() && (this->m_InputGround.GetPronePer() == 1.f)) {
						if (!this->m_InputGround.GetPronetoStanding()) {
							GetAnime(CharaAnimeID::All_PronetoStand).GoStart();
							this->m_PronePer2 = 0.f;
						}
						this->m_InputGround.SetPronetoStanding(true);
					}
					if (this->m_InputGround.GetPronetoStanding() && (this->m_InputGround.GetPronePer() == 0.f)) {
						this->m_InputGround.SetPronetoStanding(false);
					}

					if (this->m_InputGround.GetPronetoStanding()) {
						if ((GetAnime(CharaAnimeID::All_PronetoStand).GetTimePer()) > 0.9f) {
							Easing(&this->m_PronePer2, 1.f, 0.95f, EasingType::OutExpo);
						}
					}
				}
				//this->m_yrad_Upper、this->m_yrad_Bottom、this->m_rad.z()決定
				this->m_InputGround.Execute();
				if (this->m_Speed <= 0.1f) {
					if (abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) > deg2rad(50.f)) {
						this->m_TurnBody = true;
					}
					if (abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) < deg2rad(0.5f)) {
						this->m_TurnBody = false;
					}
				}
				else {
					this->m_TurnBody = false;
				}
				//
				auto FrontP = this->m_InputGround.GetFrontP();
				auto TmpRunPer = Leap(Leap(0.85f, 0.7f, this->m_InputGround.GetRunPer()), 0.f, this->m_InputGround.GetPronePer());
				if (this->m_Flightmode) { TmpRunPer = 0.95f; }
				if (this->m_TurnBody || (this->m_Speed > 0.1f) || this->m_Flightmode || GetIsProne()) { Easing(&this->m_yrad_Upper, this->m_InputGround.GetRad().y(), TmpRunPer, EasingType::OutExpo); }
				auto OLDP = this->m_yrad_Bottom;
				Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - FrontP * (1.f - this->m_InputGround.GetPronePer()), TmpRunPer, EasingType::OutExpo);

				this->m_InputGround.SetRadBufZ((this->m_yrad_Bottom - OLDP) * 2.f);
			}
			//上半身回転															//0.06ms
			void			ExecuteUpperMatrix(void) noexcept {
				this->m_UpperMatrix = MATRIX_ref::RotX(this->m_InputGround.GetRad().x()) * MATRIX_ref::RotY(this->m_InputGround.GetRad().y() - this->m_yrad_Bottom);
				this->m_obj.frame_Reset(this->Frames[(int)CharaFrame::Upper].first);
				SetFrameLocalMat(CharaFrame::Upper, GetFrameLocalMat(CharaFrame::Upper).GetRot() * Leap_Matrix(this->m_UpperMatrix, MATRIX_ref::zero(), this->m_FlightPer));
			}
			//SetMat指示															//0.03ms
			void			ExecuteAnim(void) noexcept {
				auto SE = SoundPool::Instance();
				//アニメ演算
				{
					//上半身
					{
						this->m_UpperAnimSelect = (!this->m_KeyActive) ? CharaAnimeID::Upper_Ready : CharaAnimeID::Upper_Down;
						if (!this->m_KeyActive) {
							this->m_SlingPer = 1.f;
						}
						if (this->m_ReadySwitch) {
							this->m_RunReadyFirst = false;
							GetAnime(CharaAnimeID::Upper_RunningStart).GoEnd();
						}
						bool canreverse = true;
						if (!this->m_RunReady && !this->m_Running && !GetAnime(CharaAnimeID::Upper_RunningStart).TimeEnd()) {
							this->m_RunReady = true;
							canreverse = false;
						}
						if (this->m_RunReadyFirst) {
							this->m_SlingPer = 0.f;
							GetAnime(CharaAnimeID::Upper_RunningStart).GoStart();
						}
						if (this->m_RunReady) {
							if (!this->m_Running) {
								SetAnimOnce(CharaAnimeID::Upper_RunningStart, 2.f);
								Easing(&this->m_SlingPer, (canreverse && (GetAnime(CharaAnimeID::Upper_RunningStart).time > 16)) ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
								this->m_InputGround.GetSprintPer(0.f);
								if (GetAnime(CharaAnimeID::Upper_RunningStart).TimeEnd()) {
									this->m_Running = true;
									GetAnime(CharaAnimeID::Upper_Running).GoStart();
									if (canreverse) {
										this->m_SlingPer = 1.f;
									}
								}
								m_UpperAnimSelect = CharaAnimeID::Upper_RunningStart;
							}
							else {
								m_UpperAnimSelect = (!this->m_InputGround.GetSprint()) ? CharaAnimeID::Upper_Running : CharaAnimeID::Upper_Sprint;
							}
						}
						else {
							Easing(&this->m_SlingPer, 0.f, 0.9f, EasingType::OutExpo);
							this->m_Running = false;
						}
						if (this->m_ReadyTimer < UpperTimerLimit) {
							m_UpperAnimSelect = GetProneAimAnimSel();
						}
						if (this->m_ShotPhase == 1) {
							m_UpperAnimSelect = GetProneShotAnimSel();
							SetAnimOnce(m_UpperAnimSelect, 1.5f);
							if (GetAnime(m_UpperAnimSelect).TimeEnd() && !this->m_Press_Shot) {
								GetAnime(m_UpperAnimSelect).GoStart();
								if (!this->m_Gun_Ptr->GetIsEmpty()) {
									this->m_ShotPhase = 2;
								}
								else {
									this->m_ShotPhase = 0;
									m_UpperAnimSelect = GetProneAimAnimSel();
								}
							}
						}
						//コッキング
						if (this->m_ShotPhase == 2) {
							m_UpperAnimSelect = GetProneCockingAnimSel();
							SetAnimLoop(m_UpperAnimSelect, 1.5f);
							if (GetAnime(m_UpperAnimSelect).time == 0.f) {
								this->m_ShotPhase = 0;
								this->m_ReadyTimer = 0.1f;
							}
						}
						//リロード開始
						if (this->m_ShotPhase == 3) {
							m_UpperAnimSelect = GetProneReloadStartAnimSel();
							SetAnimLoop(m_UpperAnimSelect, 1.5f);
							if (GetAnime(m_UpperAnimSelect).time == 0.f) {
								this->m_ShotPhase++;
								this->m_ReadyTimer = 0.1f;
							}
						}
						if (this->m_ShotPhase == 4) {
							m_UpperAnimSelect = GetProneReloadOneAnimSel();
							SetAnimLoop(m_UpperAnimSelect, 1.5f);
							if (GetAnime(m_UpperAnimSelect).time == 0.f) {
								if (this->m_Gun_Ptr->GetIsMagFull()) {
									this->m_ShotPhase++;
								}
								this->m_ReadyTimer = 0.1f;
							}
						}
						if (this->m_ShotPhase == 5) {
							m_UpperAnimSelect = GetProneReloadEndAnimSel();
							SetAnimLoop(m_UpperAnimSelect, 1.5f);
							if (GetAnime(m_UpperAnimSelect).time == 0.f) {
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
								if (GetAnime(m_UpperAnimSelect).time >= 37.f) {
									this->m_LoadAmmoPer = 1.f;
								}
							}
							if (this->m_ShotPhase == 4) {
								if (GetAnime(m_UpperAnimSelect).time <= 18.f) {
									this->m_LoadAmmoPer = 1.f;
								}
								else {
									Easing(&this->m_LoadAmmoPer, 0.f, 0.7f, EasingType::OutExpo);
								}
							}
						}

						//首
						Easing(&m_NeckPer, (
							m_UpperAnimSelect == CharaAnimeID::Upper_Down
							|| m_UpperAnimSelect == CharaAnimeID::Upper_Running
							) ? 0.f : 1.f, 0.9f, EasingType::OutExpo);
					}
					//下半身
					{
						if (GetPressFrontGround()) {
							this->m_BottomAnimSelect = (this->m_InputGround.GetRun()) ? CharaAnimeID::Bottom_Run : CharaAnimeID::Bottom_Walk;
						}
						else {
							this->m_BottomAnimSelect = CharaAnimeID::Bottom_Stand;
							if (GetPressLeftGround()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_LeftStep; }
							if (GetPressRightGround()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_RightStep; }
							if (GetPressRearGround()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_WalkBack; }
							if (this->m_InputGround.GetSprint()) { this->m_BottomAnimSelect = CharaAnimeID::Bottom_Run; }
						}
					}
					//
					SetAnimOnce(CharaAnimeID::All_PronetoStand, 1.f);
					SetAnimLoop(CharaAnimeID::Upper_Sprint, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Upper_Running, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Bottom_Turn, 0.5f);
					SetAnimLoop(CharaAnimeID::Bottom_Run, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Bottom_Walk, 1.15f * this->m_InputGround.GetVecFront());
					SetAnimLoop(CharaAnimeID::Bottom_LeftStep, 1.15f * this->m_InputGround.GetVecLeft());
					SetAnimLoop(CharaAnimeID::Bottom_WalkBack, 1.15f * this->m_InputGround.GetVecRear());
					SetAnimLoop(CharaAnimeID::Bottom_RightStep, 1.15f * this->m_InputGround.GetVecRight());
					SetAnimLoop(CharaAnimeID::All_ProneWalk, 1.15f * this->m_Speed);

					Easing(&this->m_LeftHandPer,
						(
							GetAnimeBuf(CharaAnimeID::Upper_RunningStart) > 0.1f
							|| GetAnimeBuf(CharaAnimeID::Upper_Running) > 0.5f
							|| GetAnimeBuf(CharaAnimeID::Upper_Sprint) > 0.5f
							|| GetAnimeBuf(CharaAnimeID::All_ProneWalk) > 0.1f
							|| GetAnimeBuf(CharaAnimeID::All_PronetoStand) > 0.1f
							|| GetAnimeBuf(CharaAnimeID::Upper_Ready) > 0.1f
							) ? 1.f : 0.f, 0.8f, EasingType::OutExpo);
				}
				//アニメセレクト
				{
					//伏せ
					GetAnimeBuf(CharaAnimeID::All_Prone) = (this->m_ShotPhase == 0) ? ((1.f - this->m_Speed) * this->m_InputGround.GetPronePer()) : 0.f;
					GetAnimeBuf(CharaAnimeID::All_ProneWalk) = (this->m_ShotPhase == 0) ? (this->m_Speed * this->m_InputGround.GetPronePer()) : 0.f;

					//上半身
					if (
						m_PrevUpperAnimSel == GetProneAimAnimSel() && m_UpperAnimSelect == GetProneShotAnimSel()) {
						GetAnimeBuf(m_PrevUpperAnimSel) = 0.f;
						GetAnimeBuf(m_UpperAnimSelect) = 1.f;
					}
					if (
						m_PrevUpperAnimSel == GetProneShotAnimSel()
						&& m_UpperAnimSelect == GetProneAimAnimSel()) {
						GetAnimeBuf(m_PrevUpperAnimSel) = 0.f;
						GetAnimeBuf(m_UpperAnimSelect) = 1.f;
					}
					if (
						(m_PrevUpperAnimSel == GetProneShotAnimSel() || m_PrevUpperAnimSel == GetProneCockingAnimSel() || m_PrevUpperAnimSel == GetProneReloadEndAnimSel())
						&& m_UpperAnimSelect == GetProneAimAnimSel()) {
						GetAnimeBuf(m_PrevUpperAnimSel) = 0.f;
						GetAnimeBuf(m_UpperAnimSelect) = 1.f;
					}

					if (
						(m_PrevUpperAnimSel == GetProneCockingAnimSel() || m_PrevUpperAnimSel == GetProneReloadEndAnimSel())
						&& m_UpperAnimSelect == CharaAnimeID::Upper_RunningStart) {
						GetAnimeBuf(m_PrevUpperAnimSel) = 0.f;
						GetAnimeBuf(m_UpperAnimSelect) = 1.f;
					}

					//真ん中
					GetAnimeBuf(CharaAnimeID::Mid_Squat) = this->m_InputGround.GetSquatPer();
					//下半身
					Easing(&GetAnimeBuf(CharaAnimeID::Bottom_Turn), (this->m_TurnBody) ? abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) / deg2rad(50.f) : 0.f, 0.8f, EasingType::OutExpo);
					//銃操作
					GetAnimeBuf(GetProneShotAnimSel()) = ((GetProneShotAnimSel() == m_UpperAnimSelect) ? 1.f : 0.f);
					GetAnimeBuf(GetProneCockingAnimSel()) = ((GetProneCockingAnimSel() == m_UpperAnimSelect) ? 1.f : 0.f);
					GetAnimeBuf(GetProneReloadStartAnimSel()) = ((GetProneReloadStartAnimSel() == m_UpperAnimSelect) ? 1.f : 0.f);
					GetAnimeBuf(GetProneReloadOneAnimSel()) = ((GetProneReloadOneAnimSel() == m_UpperAnimSelect) ? 1.f : 0.f);
					GetAnimeBuf(GetProneReloadEndAnimSel()) = ((GetProneReloadEndAnimSel() == m_UpperAnimSelect) ? 1.f : 0.f);
					//その他
					for (int i = 2; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						//上半身
						if (
							//i == (int)CharaAnimeID::Upper_Flight ||
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
							GetAnimeBuf((CharaAnimeID)i) += ((i == (int)this->m_UpperAnimSelect) ? 1.f : -1.f)*3.f / FPS;
						}
						//下半身
						if (
							//i == (int)CharaAnimeID::Bottom_Flight ||
							i == (int)CharaAnimeID::Bottom_Stand ||
							i == (int)CharaAnimeID::Bottom_Run ||
							i == (int)CharaAnimeID::Bottom_Walk ||
							i == (int)CharaAnimeID::Bottom_LeftStep ||
							i == (int)CharaAnimeID::Bottom_RightStep ||
							i == (int)CharaAnimeID::Bottom_WalkBack
							)
						{
							GetAnimeBuf((CharaAnimeID)i) += ((i == (int)this->m_BottomAnimSelect) ? 1.f : -1.f)*3.f / FPS;
						}
						GetAnimeBuf((CharaAnimeID)i) = std::clamp(GetAnimeBuf((CharaAnimeID)i), 0.f, 1.f);
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
							if (this->m_InputGround.GetPronetoStanding()) {
								if (i == (int)CharaAnimeID::All_PronetoStand) {
									GetAnime((CharaAnimeID)i).per = 1.f * (1.f - this->m_PronePer2);
								}
								else {
									GetAnime((CharaAnimeID)i).per = 0.f;
								}

							}
							else {
								GetAnime((CharaAnimeID)i).per = this->m_AnimPerBuf[i] * this->m_InputGround.GetPronePer();
							}
						}
						else {
							//通常
							if (this->m_InputGround.GetPronetoStanding()) {
								GetAnime((CharaAnimeID)i).per = this->m_AnimPerBuf[i] * (1.f - this->m_InputGround.GetPronePer()) * this->m_PronePer2;
							}
							else {
								GetAnime((CharaAnimeID)i).per = this->m_AnimPerBuf[i] * (1.f - this->m_InputGround.GetPronePer());
							}
						}
						/*
						//反映
						if (
							i == (int)CharaAnimeID::Upper_Flight ||
							i == (int)CharaAnimeID::Bottom_Flight
							) {
							GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i) * this->m_FlightPer;
						}
						else {
							GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i) * (1.f - this->m_FlightPer);
						}
						//*/
					}
					//
				}
				//足音
				if (!this->m_Flightmode) {
					if (this->m_BottomAnimSelect != CharaAnimeID::Bottom_Stand) {
						auto Time = GetAnime(this->m_BottomAnimSelect).time;
						if (this->m_BottomAnimSelect != CharaAnimeID::Bottom_Run) {
							//L
							if ((9.f < Time && Time < 10.f)) {
								if (!GetIsProne()) {
									if (this->m_CharaSound != 1) {
										this->m_CharaSound = 1;
										SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), 12.5f * 5.f);
									}
								}
								else {
									if (this->m_CharaSound != 2) {
										this->m_CharaSound = 2;
										SE->Get((int)SoundEnum::SlideFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), 12.5f * 5.f, 128);
									}
								}
							}
							//R
							if ((27.f < Time &&Time < 28.f)) {
								if (!GetIsProne()) {
									if (this->m_CharaSound != 3) {
										this->m_CharaSound = 3;
										SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
									}
								}
								else {
									if (this->m_CharaSound != 4) {
										this->m_CharaSound = 4;
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
								if (this->m_CharaSound != 5) {
									this->m_CharaSound = 5;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), 12.5f * 5.f);
								}
							}
							//R
							if (
								(8.f < Time &&Time < 9.f) ||
								(28.f < Time &&Time < 29.f)
								) {
								if (this->m_CharaSound != 6) {
									this->m_CharaSound = 6;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
								}
							}
						}
						this->m_ReturnStand = true;
					}
					else if (this->m_ReturnStand) {
						if (this->m_CharaSound != 7) {
							this->m_CharaSound = 7;
							SE->Get((int)SoundEnum::SlideFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f, (int)(192.f * this->m_RunPer2 / SpeedLimit));
						}
						this->m_ReturnStand = false;
					}
					if (0.1f < this->m_InputGround.GetPronePer() &&this->m_InputGround.GetPronePer() < 0.2f) {
						if (this->m_CharaSound != 8) {
							this->m_CharaSound = 8;
							SE->Get((int)SoundEnum::StandupFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
						}
					}
				}
				//
				this->m_obj.work_anime();
			}
			//SetMat指示更新														//0.03ms
			void			ExecuteMatrix(void) noexcept {
				this->m_RunPer2 = Leap(0.35f, (SpeedLimit * (1.f + 0.5f * this->m_InputGround.GetSprintPer())), this->m_InputGround.GetRunPer());
				this->m_RunPer2 = Leap(this->m_RunPer2, 0.15f, this->m_InputGround.GetSquatPer());
				this->m_RunPer2 = Leap(this->m_RunPer2, 0.1f, this->m_InputGround.GetPronePer());
				if (this->m_PrevRunPer2 == 0.f) {
					this->m_PrevRunPer2 = this->m_RunPer2;
				}

				auto OLDpos = this->m_PosBuf;
				VECTOR_ref vecBuf = MATRIX_ref::Vtrans(this->m_InputGround.GetVec(), MATRIX_ref::RotY(this->m_yrad_Upper));
				this->m_Speed = std::clamp(vecBuf.size(), 0.f, 1.f);

				if (this->m_Speed > 0.1f) {
					this->m_move.vec.x((vecBuf.Norm() * this->m_RunPer2 * 60.f / FPS).x());
					this->m_move.vec.z((vecBuf.Norm() * this->m_RunPer2 * 60.f / FPS).z());
				}
				else {
					this->m_move.vec.x(vecBuf.x());
					this->m_move.vec.z(vecBuf.z());
				}
				{
					auto HitResult = this->m_MapCol->CollCheck_Line(this->m_PosBuf + VECTOR_ref::up() * -1.f, this->m_PosBuf + VECTOR_ref::up() * 15.f);
					if (HitResult.HitFlag == TRUE) {
						auto yPos = this->m_PosBuf.y();
						Easing(&yPos, HitResult.HitPosition.y, 0.8f, EasingType::OutExpo);
						this->m_PosBuf.y(yPos);
						if (this->m_move.vec.y() != 0.f) {
							this->m_Flightmode = false;
						}
						this->m_move.vec.y(0.f);
						Easing(&this->m_ProneNormal, VECTOR_ref(HitResult.Normal), 0.95f, EasingType::OutExpo);
					}
					else {
						if (!this->m_Flightmode) {
							this->m_move.vec.yadd(M_GR / (FPS * FPS));
						}
						else {
							this->m_move.vec.y(0.01f);
						}
					}
				}
				this->m_PosBuf += this->m_move.vec;
				if (this->m_Flightmode) {

					Easing(&this->m_FlightVecBuf, this->GetCharaDir().zvec() * -1.f * 3.f * 60.f / FPS, 0.95f, EasingType::OutExpo);

					this->m_PosBuf += this->m_FlightVecBuf;
				}
				else {
					Easing(&this->m_FlightVecBuf, VECTOR_ref::zero(), 0.975f, EasingType::OutExpo);
				}
				col_wall(OLDpos, &this->m_PosBuf, *this->m_MapCol);
				//this->m_move.mat = MATRIX_ref::RotZ(this->m_InputGround.GetRad().z()) * MATRIX_ref::RotY(this->m_yrad_Bottom)
				//	* MATRIX_ref::RotVec2(VECTOR_ref::up(), Leap(VECTOR_ref::up(), this->m_ProneNormal, this->m_InputGround.GetPronePer()));
				if (!this->m_Flightmode) {
					auto mat_tmp = MATRIX_ref::RotZ(this->m_InputGround.GetRad().z()) * MATRIX_ref::RotY(this->m_yrad_Bottom)
						* MATRIX_ref::RotVec2(VECTOR_ref::up(), Leap(VECTOR_ref::up(), this->m_ProneNormal, this->m_InputGround.GetPronePer()));
					Easing_Matrix(&this->m_move.mat, mat_tmp, 0.8f, EasingType::OutExpo);
					this->m_FlightMatrix = this->m_move.mat;
				}
				else {
					this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.xvec(), this->m_FradAdd.x());
					this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.zvec(), this->m_FradAdd.y());
					Easing_Matrix(&this->m_move.mat, this->m_FlightMatrix, 0.8f, EasingType::OutExpo);
				}
				Easing(&this->m_move.pos, this->m_PosBuf, 0.9f, EasingType::OutExpo);
				UpdateMove();
				//銃座標指定(アニメアップデート含む)//0.19ms
				if (this->m_Gun_Ptr != nullptr) {
					//持ち手探索
					VECTOR_ref yVec1, zVec1, Pos1;
					{
						int i = (int)GetProneCockingAnimSel();
						bool change = (GetAnime((CharaAnimeID)i).per == 1.f);
						if (!change) { i = (int)GetProneReloadStartAnimSel(); }
						change = (GetAnime((CharaAnimeID)i).per == 1.f);
						if (!change) { i = (int)GetProneReloadOneAnimSel(); }
						change = (GetAnime((CharaAnimeID)i).per == 1.f);
						if (!change) { i = (int)GetProneReloadEndAnimSel(); }
						change = (GetAnime((CharaAnimeID)i).per == 1.f);
						//
						if (change) {//タイムでやれば歩きイランのでは？
							GetAnime(GetProneAimAnimSel()).per = 1.f;
							GetAnime((CharaAnimeID)i).per = 0.f;
						}
						GetAnime(CharaAnimeID::RightHand).per = 0.f;


						GetAnime(CharaAnimeID::LeftHand).per = ((
							(m_UpperAnimSelect != CharaAnimeID::Upper_RunningStart)
							&& (m_UpperAnimSelect != CharaAnimeID::Upper_Running)
							&& (m_UpperAnimSelect != CharaAnimeID::Upper_Sprint)
							&& (m_UpperAnimSelect != CharaAnimeID::Upper_Ready)

							&& (GetAnimeBuf(CharaAnimeID::All_ProneWalk) <= 0.1f)
							&& (GetAnimeBuf(CharaAnimeID::All_PronetoStand) <= 0.1f)
							) ? 1.f : 0.f);

						printfDx("  %f\n", GetAnime(CharaAnimeID::LeftHand).per);

						//アニメアップデート
						this->m_obj.work_anime();//0.35ms

						auto mat = (GetFrameWorldMat(CharaFrame::RightWrist).GetRot()*GetCharaDir().Inverse());
						yVec1 = MATRIX_ref::Vtrans(VECTOR_ref::vget(0, 0, -1).Norm(), mat);
						zVec1 = MATRIX_ref::Vtrans(VECTOR_ref::vget(-1, -1, 0).Norm(), mat);
						Pos1 = GetFrameWorldMat(CharaFrame::RightHandJoint).pos();

						if (change) {
							GetAnime(GetProneAimAnimSel()).per = 0.f;
							GetAnime((CharaAnimeID)i).per = 1.f;
						}
						GetAnime(CharaAnimeID::RightHand).per = (this->m_ShotPhase < 2) ? ((!this->m_RunReady || (m_UpperAnimSelect != CharaAnimeID::Upper_Ready)) ? 1.f : 0.f) : 0.f;
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
					move_LeftArm();
				}
			}
			//顔																	//0.01ms
			void			ExecuteShape(void) noexcept {
				SetShapePer(CharaShape::None, 0.f);
				SetShapePer(CharaShape::O, (0.75f + sin(this->m_HeartRateRad * 3)*0.25f)*(1.f - this->m_Stamina / StaminaMax));
				if (this->m_EyeclosePer <= 0.05f && (GetRand(100) == 0)) {
					this->m_Eyeclose = 1;
				}
				if (this->m_EyeclosePer >= 0.95f) {
					this->m_Eyeclose = 0;
				}
				Easing(&this->m_EyeclosePer, (float)this->m_Eyeclose, 0.5f, EasingType::OutExpo);
				SetShapePer(CharaShape::EYECLOSE, this->m_EyeclosePer);
			}
			//心拍数																//0.00ms
			void			ExecuteHeartRate(void) noexcept {
				auto SE = SoundPool::Instance();
				auto addRun = (this->m_RunPer2 - this->m_PrevRunPer2);
				if (addRun > 0.f) {
					this->m_HeartRate_r += (10.f + GetRandf(10.f)) / FPS;
				}
				else if (addRun < 0.f) {
					this->m_HeartRate_r -= (5.f + GetRandf(5.f)) / FPS;
				}
				this->m_HeartRate_r += (this->m_Speed * this->m_RunPer2 / SpeedLimit * 1.95f) / FPS;
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
				if (this->m_HeartRateRad >= DX_PI_F * 2) { this->m_HeartRateRad -= DX_PI_F * 2; }
				if (
					(deg2rad(0) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(10)) ||
					(deg2rad(120) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(130)) ||
					(deg2rad(240) <= this->m_HeartRateRad && this->m_HeartRateRad <= deg2rad(250))
					) {
					if (!this->m_HeartSoundFlag) {
						this->m_HeartSoundFlag = true;
						SE->Get((int)SoundEnum::Heart).Play_3D(0, GetFrameWorldMat(CharaFrame::Upper2).pos(), 12.5f * 1.f);
					}
				}
				else {
					this->m_HeartSoundFlag = false;
				}


				this->m_Stamina += std::clamp((100.f - this->m_HeartRate) / 40.f, -2.5f, 2.5f) / FPS;

				if (this->m_InputGround.GetIsSquat()) {
					this->m_Stamina += 1.0f / FPS;
				}
				if (GetIsProne()) {
					this->m_Stamina += 1.5f / FPS;
				}

				if (this->m_InputGround.GetSprint()) {
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
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Human;
				ValueSet(deg2rad(0.f), deg2rad(0.f), false, false, VECTOR_ref::vget(0.f, 0.f, 0.f));
				this->m_Gun_Ptr.reset();
			}
			~CharacterClass(void) noexcept {}
		public: //継承
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
				float cos_t = GetCosFormula((GetFrameWorldMat(CharaFrame::LeftWrist).pos() - GetFrameWorldMat(CharaFrame::LeftArm2).pos()).size(), (GetFrameWorldMat(CharaFrame::LeftArm2).pos() - GetFrameWorldMat(CharaFrame::LeftArm).pos()).size(), (GetFrameWorldMat(CharaFrame::LeftArm).pos() - HandAim.pos).size());
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
				HandAim.mat = Leap_Matrix(mat1, mat2, this->m_LeftHandPer);
				SetFrameLocalMat(CharaFrame::LeftWrist, HandAim.mat);
			}
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				GetAnime(CharaAnimeID::Bottom_Stand).per=1.f;
				GetAnime(CharaAnimeID::Upper_RunningStart).GoEnd();
				//
				SetCreate3DSoundFlag(TRUE);
				this->m_Breath = SoundHandle::Load("data/Sound/SE/voice/WinningTicket/breath.wav");
				SetCreate3DSoundFlag(FALSE);
				this->m_Breath.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_Breath.get());
			}
			void			Execute(void) noexcept override {
				//初回のみ更新する内容
				if (this->m_IsFirstLoop) {
					for (int i = 0; i < this->m_obj.get_anime().size(); i++) { GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i); }
					GetAnime(CharaAnimeID::RightHand).per = 1.f;
					this->m_obj.work_anime();
					this->m_RightWristRot = GetFrameLocalMat(CharaFrame::RightWrist).GetRot();
				}
				ExecuteSavePrev();			//以前の状態保持
				ExecuteInput();				//操作//0.01ms
				ExecuteUpperMatrix();		//上半身回転//0.06ms
				ExecuteAnim();				//AnimUpdte//0.03ms
				ExecuteMatrix();			//SetMat指示//0.03ms
				ExecuteShape();				//顔//スコープ内0.01ms
				ExecuteHeartRate();			//心拍数//0.00ms
			}
			//void			Draw(void) noexcept override { ObjectBaseClass::Draw(); }
		public:
			//
			void			ValueSet(float pxRad, float pyRad, bool SquatOn, bool ProneOn, const VECTOR_ref& pPos) {
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					GetAnimeBuf((CharaAnimeID)i) = 0.f;
				}
				this->m_ReadyPer = 0.f;
				this->m_ReadyTimer = UpperTimerLimit;
				this->m_HeartRate = HeartRateMin;
				this->m_Stamina = StaminaMax;

				this->m_Speed = 0.f;
				this->m_RunPer2 = 0.f;
				this->m_PrevRunPer2 = 0.f;
				this->m_HeartRateRad = 0.f;
				this->m_LeftHandPer = 0.f;
				this->m_TurnBody = false;
				this->m_ShotPhase = 0;
				this->m_RunReady = false;
				this->m_Running = false;
				this->m_Press_Shot = false;
				this->m_Press_Reload = false;
				this->m_Press_Aim = false;
				this->m_KeyActive = false;
				this->m_HeartSoundFlag = false;
				this->m_FradAdd_Buf.clear();
				this->m_FradAdd.clear();
				//
				this->m_Score = 0.f;
				//動作にかかわる操作
				this->m_InputGround.ValueSet(pxRad, pyRad, SquatOn, ProneOn);
				this->m_Flightmode = false;
				this->m_PosBuf = pPos;
				//上記を反映するもの
				this->m_yrad_Upper = this->m_InputGround.GetRad().y();
				this->m_yrad_Bottom = this->m_InputGround.GetRad().y();
				this->m_PosBuf = pPos;
				this->m_move.vec.clear();
				this->m_UpperMatrix = MATRIX_ref::RotX(this->m_InputGround.GetRad().x()) * MATRIX_ref::RotY(this->m_InputGround.GetRad().y() - this->m_yrad_Bottom);
				this->m_FradAdd;
				SetMove(MATRIX_ref::RotY(this->m_yrad_Bottom), this->m_PosBuf);
			}
			//
			void			SetInput(const InputControl& pInput, bool pReady) {

				this->m_Flightmode |= pInput.m_GoFlight && this->m_KeyActive;

				this->m_ReadySwitch = (this->m_KeyActive != pReady);
				this->m_KeyActive = pReady;

				bool CannotMoveinProne = ((GetIsProne() && (!(this->m_ShotPhase == 0))) || !this->m_KeyActive);

				m_InputGround.SetInput(
					pInput.m_AddxRad*(1.f - this->m_FlightPer), pInput.m_AddyRad*(1.f - this->m_FlightPer),
					GetHeartRandVec()*(1.f - this->m_FlightPer),
					pInput.m_GoFrontPress && !this->m_Flightmode && !CannotMoveinProne,
					pInput.m_GoBackPress && !this->m_Flightmode && !CannotMoveinProne,
					pInput.m_GoLeftPress && !this->m_Flightmode && !CannotMoveinProne,
					pInput.m_GoRightPress && !this->m_Flightmode && !CannotMoveinProne,
					pInput.m_SquatPress && !this->m_Flightmode,
					pInput.m_PronePress && !this->m_Flightmode && (this->m_ShotPhase == 0),
					pInput.m_RunPress && !this->m_Flightmode,
					pReady,
					pInput.m_QPress,
					pInput.m_EPress,
					this->m_CannotSprint
				);
				//AIM
				this->m_Press_Shot = pInput.m_ShotPress && this->m_KeyActive;
				this->m_Press_Reload = (pInput.m_ReloadPress && this->m_KeyActive && (this->m_Gun_Ptr->GetAmmoNum() <= this->m_Gun_Ptr->GetAmmoAll()));
				if (0.f != this->m_InputGround.GetPronePer() && this->m_InputGround.GetPronePer() != 1.0f) {
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
				this->m_Press_Aim = pInput.m_AimPress && this->m_KeyActive && (this->m_InputGround.GetPronePer() == 0.f || this->m_InputGround.GetPronePer() == 1.f);
				if (this->m_InputGround.GetPronetoStanding()) {
					this->m_Press_Aim = false;
					this->m_ReadyTimer = UpperTimerLimit;
				}
				//
				{
					auto TmpReady = !(!GetIsRun() || (this->m_ShotPhase >= 2));
					this->m_RunReadyFirst = (TmpReady && !this->m_RunReady);
					this->m_RunReady = TmpReady;
				}
				//操縦
				{
					this->m_FradAdd_Buf.x(-pInput.m_AddxRad);
					this->m_FradAdd_Buf.y(pInput.m_AddyRad);
					Easing(&this->m_FradAdd, this->m_FradAdd_Buf, 0.95f, EasingType::OutExpo);
				}
			}
			//
			void			SetEyeVec(const VECTOR_ref& camvec) noexcept {
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
