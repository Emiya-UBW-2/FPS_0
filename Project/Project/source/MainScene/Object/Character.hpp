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

			bool												m_IsRun{ false };
			float												m_RunPer{ 0.f };
			float												m_RunTimer{ 0.f };

			VECTOR_ref											m_rad_Buf, m_rad, m_radAdd;
		private: //内部
			void			SetVec(int pDir, bool Press) {
				this->m_Vec[pDir] += (Press ? 1.f : -1.f)*2.f / FPS;
				this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
			}
		public:
			const auto		GetRad() const noexcept { return  this->m_rad; }
			const auto		GetRunPer() const noexcept { return  this->m_RunPer; }

			const auto		GetVecFront() const noexcept { return  this->m_Vec[0]; }
			const auto		GetVecRear() const noexcept { return this->m_Vec[2]; }
			const auto		GetVecLeft() const noexcept { return this->m_Vec[1]; }
			const auto		GetVecRight() const noexcept { return this->m_Vec[3]; }

			const auto		GetPressFront() const noexcept { return this->m_Press_GoFront; }
			const auto		GetPressRear() const noexcept { return this->m_Press_GoRear; }
			const auto		GetPressLeft() const noexcept { return this->m_Press_GoLeft; }
			const auto		GetPressRight() const noexcept { return this->m_Press_GoRight; }

			const auto		GetRun() const noexcept { return this->m_IsRun; }

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
			void			ValueSet(float pxRad, float pyRad) {
				for (int i = 0; i < 4; i++) {
					this->m_Vec[i] = 0.f;
				}
				this->m_KeyActive = false;
				this->m_Press_GoFront = false;
				this->m_Press_GoRear = false;
				this->m_Press_GoLeft = false;
				this->m_Press_GoRight = false;
				this->m_radAdd.clear();
				this->m_RunPer = 0.f;
				//動作にかかわる操作
				this->m_rad_Buf.x(pxRad);
				this->m_rad_Buf.y(pyRad);
				//上記を反映するもの
				this->m_rad = this->m_rad_Buf;
			}
			void			SetInput(
				float pAddxRad, float pAddyRad,
				const VECTOR_ref& pAddRadvec,
				bool pGoFrontPress,
				bool pGoBackPress,
				bool pGoLeftPress,
				bool pGoRightPress,
				bool pRunPress,
				bool pReady
			) {
				this->m_KeyActive = pReady;
				this->m_Press_GoFront = pGoFrontPress && this->m_KeyActive;
				this->m_Press_GoRear = pGoBackPress && this->m_KeyActive;
				this->m_Press_GoLeft = pGoLeftPress && this->m_KeyActive;
				this->m_Press_GoRight = pGoRightPress && this->m_KeyActive;

				if (!this->m_IsRun && (pRunPress && GetPressFront())) {
					this->m_RunTimer = 1.f;
				}
				if (this->m_RunTimer > 0.f) {
					this->m_RunTimer -= 1.f / FPS;
					this->m_Press_GoFront = true;
					this->m_IsRun = true;
				}
				else {
					this->m_RunTimer = 0.f;
					this->m_IsRun = (pRunPress && GetPressFront());
				}
				if (GetPressRear() || (!GetPressFront() && (GetPressLeft() || GetPressRight()))) {
					this->m_IsRun = false;
				}
				//回転
				{
					auto limchange = Leap(1.f, powf(1.f - this->m_Vec[0], 0.5f), this->m_RunPer * 0.8f);
					Easing(&this->m_radAdd, pAddRadvec, 0.95f, EasingType::OutExpo);

					this->m_rad_Buf.x(
						std::clamp(this->m_rad_Buf.x() + pAddxRad, -deg2rad(40.f) * limchange, deg2rad(25.f) * limchange)
						+ this->m_radAdd.x()
					);
					this->m_rad_Buf.y(
						this->m_rad_Buf.y() + pAddyRad
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
			}
		};

		class CharacterClass : public ObjectBaseClass {
		private://キャラパラメーター
			const float SpeedLimit{ 2.f };
			const float UpperTimerLimit = 3.f;
			const float HeartRateMin{ 60.f };//心拍数最小
			const float HeartRateMax{ 180.f };//心拍数最大
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
			bool												m_TurnBody{ false };
			bool												m_ReturnStand{ false };
			CharaAnimeID										m_UpperAnimSelect, m_PrevUpperAnimSel;
			CharaAnimeID										m_BottomAnimSelect;
			//体力
			float												m_HeartRate{ HeartRateMin };//心拍数
			float												m_HeartRate_r{ HeartRateMin };//心拍数
			float												m_HeartRateRad{ 0.f };//呼吸Sin渡し
			//表情
			int													m_Eyeclose{ 0 };
			float												m_EyeclosePer{ 0.f };
			//サウンド
			int													m_CharaSound{ -1 };
			SoundHandle											m_Breath;
			bool												m_HeartSoundFlag{ false };
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
			const auto		GetCharaDir(void) const noexcept {
				if (!this->m_Flightmode) {
					return this->m_UpperMatrix * this->m_move.mat;
				}
				else {
					return this->m_UpperMatrix * this->m_FlightMatrix;
				}
			}
			const auto		GetEyeVector(void) const noexcept { return GetCharaDir().zvec() * -1.f; }
			const auto		GetEyePosition(void) const noexcept { return (GetFrameWorldMat(CharaFrame::LeftEye).pos() + GetFrameWorldMat(CharaFrame::RightEye).pos()) / 2.f + this->GetEyeVector().Norm() * 0.5f; }
			const auto&		GetIsRun(void) const noexcept { return this->m_InputGround.GetRun(); }
			const auto&		GetHeartRate(void) const noexcept { return this->m_HeartRate; }
			const auto&		GetHeartRateRad(void) const noexcept { return this->m_HeartRateRad; }
			const auto&		GetFlightPer(void) const noexcept { return this->m_FlightPer; }

			const auto		GetHeartRandVec(void) const noexcept {
				auto tmp2 = 0.2f * GetRandf(deg2rad(1.f));
				auto tmp3 = 0.5f;
				VECTOR_ref tmpvec = VECTOR_ref::vget(
					tmp2 + 0.0002f * sin(this->m_HeartRateRad) * powf(this->m_HeartRate / HeartRateMin, 3.f),
					tmp2 + 0.0002f * sin(this->m_HeartRateRad * 3) * powf(this->m_HeartRate / HeartRateMin, 3.f),
					0.f
				);
				return tmpvec * tmp3;
			}
		private: //更新関連
			//以前の状態保持														//
			void			ExecuteSavePrev(void) noexcept {
				this->m_PrevUpperAnimSel = this->m_UpperAnimSelect;
				this->m_PrevRunPer2 = this->m_RunPer2;
			}
			//操作																	//0.01ms
			void			ExecuteInput(void) noexcept {
				//
				Easing(&this->m_FlightPer, this->m_Flightmode ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
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
				auto TmpRunPer = Leap(0.85f, 0.7f, this->m_InputGround.GetRunPer());
				if (this->m_Flightmode) { TmpRunPer = 0.95f; }
				if (this->m_TurnBody || (this->m_Speed > 0.1f) || this->m_Flightmode) { Easing(&this->m_yrad_Upper, this->m_InputGround.GetRad().y(), TmpRunPer, EasingType::OutExpo); }
				auto OLDP = this->m_yrad_Bottom;
				Easing(&this->m_yrad_Bottom, this->m_yrad_Upper - FrontP, TmpRunPer, EasingType::OutExpo);

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
						this->m_UpperAnimSelect = CharaAnimeID::Upper_Normal;
						if (this->m_InputGround.GetRun()) {
							this->m_UpperAnimSelect = CharaAnimeID::Upper_Running;
						}
						//首
						Easing(&m_NeckPer, (true) ? 0.f : 1.f, 0.9f, EasingType::OutExpo);
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
						}
					}
					//
					SetAnimLoop(CharaAnimeID::Upper_Running, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Bottom_Turn, 0.5f);
					SetAnimLoop(CharaAnimeID::Bottom_Run, 0.5f * this->m_InputGround.GetVecFront() * this->m_RunPer2);
					SetAnimLoop(CharaAnimeID::Bottom_Walk, 1.15f * this->m_InputGround.GetVecFront());
					SetAnimLoop(CharaAnimeID::Bottom_LeftStep, 1.15f * this->m_InputGround.GetVecLeft());
					SetAnimLoop(CharaAnimeID::Bottom_WalkBack, 1.15f * this->m_InputGround.GetVecRear());
					SetAnimLoop(CharaAnimeID::Bottom_RightStep, 1.15f * this->m_InputGround.GetVecRight());
				}
				//アニメセレクト
				{
					//上半身
					GetAnimeBuf(CharaAnimeID::Upper_Flight) = 1.f;
					GetAnimeBuf(CharaAnimeID::Bottom_Flight) = 1.f;
					//下半身
					Easing(&GetAnimeBuf(CharaAnimeID::Bottom_Turn), (this->m_TurnBody) ? abs(this->m_InputGround.GetRad().y() - this->m_yrad_Upper) / deg2rad(50.f) : 0.f, 0.8f, EasingType::OutExpo);
					//その他
					for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						//上半身
						if (
							//i == (int)CharaAnimeID::Upper_Flight ||
							i == (int)CharaAnimeID::Upper_Normal ||
							i == (int)CharaAnimeID::Upper_Running
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
						if (
							i == (int)CharaAnimeID::Upper_Flight ||
							i == (int)CharaAnimeID::Bottom_Flight
							) {
							GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i) * this->m_FlightPer;
						}
						else {
							GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i) * (1.f - this->m_FlightPer);
						}

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
								if (this->m_CharaSound != 1) {
									this->m_CharaSound = 1;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::LeftFoot).pos(), 12.5f * 5.f);
								}
							}
							//R
							if ((27.f < Time &&Time < 28.f)) {
								if (this->m_CharaSound != 3) {
									this->m_CharaSound = 3;
									SE->Get((int)SoundEnum::RunFoot).Play_3D(0, GetFrameWorldMat(CharaFrame::RightFoot).pos(), 12.5f * 5.f);
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
				}
				//
				this->m_obj.work_anime();
			}
			//SetMat指示更新														//0.03ms
			void			ExecuteMatrix(void) noexcept {
				this->m_RunPer2 = Leap(0.35f, SpeedLimit, this->m_InputGround.GetRunPer());
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
				//this->m_move.mat = MATRIX_ref::RotZ(this->m_InputGround.GetRad().z()) * MATRIX_ref::RotY(this->m_yrad_Bottom);
				if (!this->m_Flightmode) {
					Easing_Matrix(&this->m_move.mat, MATRIX_ref::RotZ(this->m_InputGround.GetRad().z()) * MATRIX_ref::RotY(this->m_yrad_Bottom), 0.8f, EasingType::OutExpo);
					this->m_FlightMatrix = this->m_move.mat;
				}
				else {
					this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.xvec(), this->m_FradAdd.x());
					this->m_FlightMatrix *= MATRIX_ref::RotAxis(this->m_move.mat.zvec(), this->m_FradAdd.y());
					Easing_Matrix(&this->m_move.mat, this->m_FlightMatrix, 0.8f, EasingType::OutExpo);
				}
				Easing(&this->m_move.pos, this->m_PosBuf, 0.9f, EasingType::OutExpo);
				UpdateMove();
			}
			//顔																	//0.01ms
			void			ExecuteShape(void) noexcept {
				SetShapePer(CharaShape::None, 0.f);
				SetShapePer(CharaShape::O, (0.75f + sin(this->m_HeartRateRad * 3)*0.25f)*(0.5f));
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
			}
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Human;
				ValueSet(deg2rad(0.f), deg2rad(0.f), false, VECTOR_ref::vget(0.f, 0.f, 0.f));
			}
			~CharacterClass(void) noexcept {}
		public: //継承
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				GetAnime(CharaAnimeID::Bottom_Stand).per=1.f;
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
					this->m_obj.work_anime();
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
			void			ValueSet(float pxRad, float pyRad, bool IsFlight, const VECTOR_ref& pPos) {
				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					GetAnimeBuf((CharaAnimeID)i) = 0.f;
				}
				this->m_HeartRate = HeartRateMin;
				this->m_Speed = 0.f;
				this->m_RunPer2 = 0.f;
				this->m_PrevRunPer2 = 0.f;
				this->m_HeartRateRad = 0.f;
				this->m_TurnBody = false;
				this->m_HeartSoundFlag = false;
				this->m_FradAdd_Buf.clear();
				this->m_FradAdd.clear();
				//動作にかかわる操作
				this->m_InputGround.ValueSet(pxRad, pyRad);

				this->m_Flightmode = IsFlight;
				this->m_PosBuf = pPos;

				//上記を反映するもの
				this->m_yrad_Upper = this->m_InputGround.GetRad().y();
				this->m_yrad_Bottom = this->m_InputGround.GetRad().y();
				this->m_move.vec.clear();
				this->m_UpperMatrix = MATRIX_ref::RotX(this->m_InputGround.GetRad().x()) * MATRIX_ref::RotY(this->m_InputGround.GetRad().y() - this->m_yrad_Bottom);
				this->m_FradAdd;
				SetMove(MATRIX_ref::RotY(this->m_yrad_Bottom), this->m_PosBuf);
			}
			//
			void			SetInput(
				float pAddxRad, float pAddyRad,
				bool pGoFrontPress,
				bool pGoBackPress,
				bool pGoLeftPress,
				bool pGoRightPress,
				bool pRunPress,
				bool pGoFlight,
				bool pReady
			) {
				this->m_KeyActive = pReady;
				this->m_Flightmode |= pGoFlight && this->m_KeyActive;

				m_InputGround.SetInput(
					pAddxRad*(1.f - this->m_FlightPer), pAddyRad*(1.f - this->m_FlightPer),
					GetHeartRandVec()*(1.f - this->m_FlightPer),
					pGoFrontPress && !this->m_Flightmode,
					pGoBackPress && !this->m_Flightmode,
					pGoLeftPress && !this->m_Flightmode,
					pGoRightPress && !this->m_Flightmode,
					pRunPress && !this->m_Flightmode,
					pReady
				);
				//操縦
				{
					this->m_FradAdd_Buf.x(-pAddxRad);
					this->m_FradAdd_Buf.y(pAddyRad);
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
