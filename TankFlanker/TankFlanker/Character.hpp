
#pragma once

#include"Header.hpp"
namespace FPS_n2 {
	namespace Sceneclass {
		class CharacterClass : public ObjectBaseClass {
			//キャラパラメーター
			float SpeedLimit{ 2.f };
			float UpperTimerLimit = 3.f;
		private:
			std::pair<int, moves> Center;
			std::pair<int, moves> Upper;
			std::pair<int, moves> Upper2;
			std::pair<int, moves> RightHandJoint;
			std::pair<int, moves> RightWrist;
			std::pair<int, moves> LeftEye;
			std::pair<int, moves> RightEye;
			moves  model_move;
			float m_yDown{ 0.f };

			std::array<float, 4> m_Vec;
			float m_VecTotal{ 0.f };
			std::array<float, (int)CharaAnimeID::AnimeIDMax> m_AnimPerSave;

			float m_xrad_Buf{ 0.f }, m_yrad_Buf{ 0.f };
			float m_xrad{ 0.f }, m_yrad{ 0.f }, m_zrad{ 0.f };
			float m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			bool m_TurnBody{ false };

			float UpperTimer{ 0.f };
			bool shotFlag_First{ false };
			bool shotFlag{ false };
			bool boltFlag{ false };
			float viewPer{ 0.f };
			bool m_SetReset{ true };
			GunClass* Gun_Ptr{ nullptr };
			float RunPer{ 0.f };
			float RunPer2{ 0.f };
			bool m_isRun{ false };
			float m_RunTimer{ false };
			bool m_isSprint{ false };
			float m_SprintPer{ 0.f };
			bool m_Ready{ false };
			float m_ReadyPer{ 0.f };
			bool m_RunReady{ false };
			bool m_RunReadyFirst{ false };
			bool m_Running{ false };
			float m_RunReadyPer{ 0.f };

			float m_SquatPer{ 0.f };
			bool m_SquatSwitch{ false };
			size_t m_SquatCount{ 0 };

			float m_PronePer{ 0.f };
			bool m_ProneSwitch{ false };
			size_t m_ProneCount{ 0 };

			bool m_Press_GoFront{ false };
			bool m_Press_GoRear{ false };
			bool m_Press_GoLeft{ false };
			bool m_Press_GoRight{ false };
			bool m_Press_Shot{ false };
			bool m_Press_Aim{ false };

		private:
			void SetAnimLoop(int ID, float speed) {
				this->obj.get_anime(ID).time += 30.f / FPS * speed;
				if (this->obj.get_anime(ID).TimeEnd()) { this->obj.get_anime(ID).time = 0.f; }
			}
			void SetUpperAnim() {
				std::vector<CharaAnimeID> animes;
				animes.emplace_back(CharaAnimeID::Upper_Down);
				animes.emplace_back(CharaAnimeID::Upper_Aim);
				animes.emplace_back(CharaAnimeID::Upper_Shot);
				animes.emplace_back(CharaAnimeID::Upper_Cocking);
				animes.emplace_back(CharaAnimeID::Upper_RunningStart);
				animes.emplace_back(CharaAnimeID::Upper_Running);
				animes.emplace_back(CharaAnimeID::Upper_Sprint);

				CharaAnimeID UpperAnimSel = CharaAnimeID::Upper_Down;				//銃下げ
				auto* RunStartAnim = &this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart);
				auto* RunningAnim = &this->obj.get_anime((int)CharaAnimeID::Upper_Running);
				{
					bool canreverse = true;
					if (!this->m_RunReady && !this->m_Running && !RunStartAnim->TimeEnd()) {
						this->m_RunReady = true;
						canreverse = false;
					}
					if (this->m_RunReadyFirst) {
						this->m_RunReadyPer = 0.f;
						RunStartAnim->time = 0.f;
					}
					if (this->m_RunReady) {
						easing_set(&this->m_ReadyPer, 1.f, 0.7f);
						if (!this->m_Running) {
							UpperAnimSel = CharaAnimeID::Upper_RunningStart;
							RunStartAnim->time += 30.f / FPS * 2.f;
							easing_set(&this->m_RunReadyPer, (canreverse && RunStartAnim->time > 16) ? 1.f : 0.f, 0.9f);
							this->m_SprintPer = 0.f;
							if (RunStartAnim->TimeEnd()) {
								this->m_Running = true;
								RunStartAnim->time = RunStartAnim->alltime;
								RunningAnim->time = 0.f;
								if (canreverse) {
									this->m_RunReadyPer = 1.f;
								}
							}
						}
						else {
							if (!this->m_isSprint) {
								UpperAnimSel = CharaAnimeID::Upper_Running;
							}
							else {
								UpperAnimSel = CharaAnimeID::Upper_Sprint;
							}
						}
					}
					else {
						easing_set(&this->m_ReadyPer, 0.f, 0.9f);
						easing_set(&this->m_RunReadyPer, 0.f, 0.9f);
						this->m_Running = false;
					}
				}
				if (this->shotFlag) {
					UpperAnimSel = CharaAnimeID::Upper_Shot;
					if (!this->obj.get_anime((int)CharaAnimeID::Upper_Shot).TimeEnd()) {
						this->obj.get_anime((int)CharaAnimeID::Upper_Shot).time += 30.f / FPS * 1.5f;
					}
					if (this->obj.get_anime((int)CharaAnimeID::Upper_Shot).TimeEnd() && !this->m_Press_Shot) {
						this->shotFlag = false;
						this->obj.get_anime((int)CharaAnimeID::Upper_Shot).time = 0.f;
						this->boltFlag = true;
					}
				}
				if (this->boltFlag) {
					UpperAnimSel = CharaAnimeID::Upper_Cocking;
					this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).time += 30.f / FPS * 1.5f;
					if (this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).TimeEnd()) {
						this->boltFlag = false;
						this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).time = 0.f;
						this->UpperTimer = 0.1f;
						this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).per = 0.f;
						this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per = 1.f;
					}
				}
				if (!this->shotFlag && !this->boltFlag) {
					if (this->UpperTimer < UpperTimerLimit) {
						UpperAnimSel = CharaAnimeID::Upper_Aim;
					}
				}
				for (const auto& a : animes) {
					if (
						UpperAnimSel == CharaAnimeID::Upper_Shot ||
						UpperAnimSel == CharaAnimeID::Upper_Cocking
						) {
						this->obj.get_anime((int)a).per = ((a == UpperAnimSel) ? 1.f : 0.f);
					}
					else if (
						UpperAnimSel == CharaAnimeID::Upper_Aim
						) {
						this->obj.get_anime((int)a).per += ((a == UpperAnimSel) ? 5.f : -5.f) / FPS;
					}
					else {
						this->obj.get_anime((int)a).per += ((a == UpperAnimSel) ? 2.f : -2.f) / FPS;
					}
					this->obj.get_anime((int)a).per = std::clamp(this->obj.get_anime((int)a).per, 0.f, 1.f);
				}

				SetAnimLoop((int)CharaAnimeID::Upper_Sprint, 0.5f*(this->m_Vec[0] * RunPer2));
				SetAnimLoop((int)CharaAnimeID::Upper_Running, 0.5f*(this->m_Vec[0] * RunPer2));
			}
			void SetBottomAnim() {
				std::vector<CharaAnimeID> animes;
				animes.emplace_back(CharaAnimeID::Bottom_Run);
				animes.emplace_back(CharaAnimeID::Bottom_Walk);
				animes.emplace_back(CharaAnimeID::Bottom_LeftStep);
				animes.emplace_back(CharaAnimeID::Bottom_RightStep);
				animes.emplace_back(CharaAnimeID::Bottom_WalkBack);

				CharaAnimeID BottomAnimSel = CharaAnimeID::Bottom_Stand;
				if (this->m_Press_GoFront) {
					if (this->m_isRun) {
						BottomAnimSel = CharaAnimeID::Bottom_Run;
					}
					else {
						BottomAnimSel = CharaAnimeID::Bottom_Walk;
					}
				}
				else {
					if (this->m_Press_GoLeft) {
						BottomAnimSel = CharaAnimeID::Bottom_LeftStep;
					}
					if (this->m_Press_GoRight) {
						BottomAnimSel = CharaAnimeID::Bottom_RightStep;
					}
					if (this->m_Press_GoRear) {
						BottomAnimSel = CharaAnimeID::Bottom_WalkBack;
					}
					if (this->m_isSprint) {
						BottomAnimSel = CharaAnimeID::Bottom_Run;
					}
				}

				for (const auto& a : animes) {
					this->obj.get_anime((int)a).per += ((a == BottomAnimSel) ? 2.f : -2.f) / FPS;
					this->obj.get_anime((int)a).per = std::clamp(this->obj.get_anime((int)a).per, 0.f, 1.f);
				}
				auto RadAbs = abs(this->m_yrad - this->m_yrad_Upper);
				this->obj.get_anime((int)CharaAnimeID::Bottom_Turn).per = (this->m_Vec[0] == 0.f) ? RadAbs / deg2rad(50.f) : 0.f;

				//stand
				{
					float standPer = 0.f;
					if (this->m_Vec[0] == 0.f) {
						standPer = 1.f - this->obj.get_anime((int)CharaAnimeID::Bottom_Turn).per;
					}
					else if (this->m_isRun) {
						standPer = 1.f - this->obj.get_anime((int)CharaAnimeID::Bottom_Run).per;
					}
					else {
						if (this->m_Vec[2] == 0.f) {
							standPer = 1.f - this->obj.get_anime((int)CharaAnimeID::Bottom_Walk).per;
						}
						else {
							standPer = 1.f - this->obj.get_anime((int)CharaAnimeID::Bottom_WalkBack).per;
						}
					}
					easing_set(&this->obj.get_anime((int)CharaAnimeID::Bottom_Stand).per, standPer, 0.95f);
				}

				SetAnimLoop((int)CharaAnimeID::Bottom_Run, 0.5f*(this->m_Vec[0] * RunPer2));
				SetAnimLoop((int)CharaAnimeID::Bottom_Walk, 3.25f*(this->m_Vec[0] * 0.35f));
				SetAnimLoop((int)CharaAnimeID::Bottom_Turn, 0.5f);
				SetAnimLoop((int)CharaAnimeID::Bottom_LeftStep, 3.25f*(this->m_Vec[1] * 0.35f));
				SetAnimLoop((int)CharaAnimeID::Bottom_RightStep, 3.25f*(this->m_Vec[3] * 0.35f));
				SetAnimLoop((int)CharaAnimeID::Bottom_WalkBack, 3.25f*(this->m_Vec[2] * 0.35f));
				auto tmp = std::clamp(m_VecTotal, 0.f, 1.f);
				SetAnimLoop((int)CharaAnimeID::All_ProneWalk, 1.25f*tmp);
			}
			const auto GetCharaDir() { return this->Upper.second.mat * this->move.mat; }
			void SetVec(int pDir, bool Press) {
				if (Press) {
					this->m_Vec[pDir] += 2.f / 60.f * 60.f / FPS;
				}
				else {
					this->m_Vec[pDir] -= 2.f / 60.f * 60.f / FPS;
				}
				this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
			}
		public:
			CharacterClass() {
				ValueSet(0.f, 0.f, false, VECTOR_ref::vget(-230.f, 0.f, 450.f));
			}
			~CharacterClass() {}
		public:
			void ValueSet(float pxRad, float pyRad, bool SquatOn, const VECTOR_ref& pPos) {
				//this->moves move;
				this->m_TurnBody = false;
				//this->moves;
				this->shotFlag_First = false;
				this->shotFlag = false;
				this->boltFlag = false;
				this->viewPer = 1.f;
				this->m_SetReset = true;
				this->Gun_Ptr = nullptr;
				this->RunPer = 0.f;
				this->m_isSprint = false;
				this->m_Ready = false;
				this->m_ReadyPer = 0.f;
				this->m_RunReady = false;
				this->m_Running = false;
				this->m_SquatPer = 0.f;
				this->m_SquatCount = 0;
				this->m_PronePer = 0.f;
				this->m_ProneCount = 0;
				this->m_Press_GoFront = false;
				this->m_Press_GoRear = false;
				this->m_Press_GoLeft = false;
				this->m_Press_GoRight = false;
				this->m_Press_Shot = false;
				this->m_Press_Aim = false;
				this->m_isRun = false;
				this->UpperTimer = UpperTimerLimit;
				move.mat.clear();
				move.pos.clear();
				move.vec.clear();
				this->m_zrad = 0.f;
				this->m_Vec[0] = 0.f;
				this->m_Vec[1] = 0.f;
				this->m_Vec[2] = 0.f;
				this->m_Vec[3] = 0.f;
				this->m_VecTotal = 0.f;
				//動作にかかわる操作
				this->m_SquatSwitch = SquatOn;
				this->m_ProneSwitch = false;
				this->m_xrad_Buf = pxRad;
				this->m_yrad_Buf = pyRad;
				move.pos = pPos;
				//上記を反映するもの
				move.mat = MATRIX_ref::RotY(this->m_yrad_Bottom);
				model_move = move;
				this->m_xrad = this->m_xrad_Buf;
				this->m_yrad = this->m_yrad_Buf;
				this->m_yrad_Upper = this->m_yrad;
				this->m_yrad_Bottom = this->m_yrad;
			}


			void Set(GunClass* pGunPtr) {
				this->Gun_Ptr = pGunPtr;
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
				if (this->m_ProneSwitch && (this->shotFlag || this->boltFlag)) {
					this->m_Press_GoFront = false;
					this->m_Press_GoRear = false;
					this->m_Press_GoLeft = false;
					this->m_Press_GoRight = false;
				}
				this->m_Press_Shot = pShotPress;
				if (0.05f < this->m_PronePer&&this->m_PronePer < 0.95f) {
					this->m_Press_Shot = false;
				}


				this->m_Press_Aim = pAimPress;
				if (!this->m_isRun& pRunPress) {
					m_RunTimer = 1.f;
				}
				if (m_RunTimer > 0.f) {
					m_RunTimer -= 1.f / FPS;
					this->m_isRun = true;
				}
				else {
					m_RunTimer = 0.f;
					this->m_isRun = pRunPress;
				}
				//
				if (this->m_Press_GoRear || (!this->m_Press_GoFront && (this->m_Press_GoLeft || this->m_Press_GoRight))) {
					this->m_isRun = false;
				}
				this->m_isSprint = this->m_isRun && (!this->m_Press_GoFront && !this->m_Press_GoRear);
				this->m_SquatCount = std::clamp<size_t>(this->m_SquatCount + 1, 0, pSquatPress ? 2 : 0);
				if (this->m_SquatCount == 1) { this->m_SquatSwitch ^= 1; }
				if (this->m_isRun) { this->m_SquatSwitch = false; }

				this->m_ProneCount = std::clamp<size_t>(this->m_ProneCount + 1, 0, (pPronePress && !(this->shotFlag || this->boltFlag)) ? 2 : 0);
				if (this->m_ProneCount == 1) { this->m_ProneSwitch ^= 1; }
				if (this->m_isRun) { this->m_ProneSwitch = false; }
				if (this->m_ProneSwitch) { this->m_SquatSwitch = false; }
				{
					auto TmpReady = !(!this->m_isRun || this->boltFlag);
					this->m_RunReadyFirst = (TmpReady && !this->m_RunReady);
					this->m_RunReady = TmpReady;
				}
				{

					float PerBuf = this->RunPer* 0.8f;
					float limchange = powf(1.f - this->m_Vec[0], 0.5f)*PerBuf + 1.f*(1.f - PerBuf);

					limchange = 0.15f*this->m_PronePer + limchange * (1.f - this->m_PronePer);

					auto tmp = (0.1f*this->m_PronePer + 1.f*(1.f - this->m_PronePer));
					this->m_xrad_Buf = std::clamp(this->m_xrad_Buf + pAddxRad * tmp, -deg2rad(40.f)*limchange, deg2rad(25.f)*limchange);
					this->m_yrad_Buf += pAddyRad * tmp;
					easing_set(&this->m_xrad, this->m_xrad_Buf, 0.5f);
					easing_set(&this->m_yrad, this->m_yrad_Buf, 0.5f);
				}
			}
			void Init() override {
				ObjectBaseClass::Init();
#if true
				this->Center.first = 2;
				this->Upper.first = 5;
				this->Upper2.first = 6;
				this->LeftEye.first = 11;
				this->RightEye.first = 13;
				this->RightWrist.first = 119;
				this->RightHandJoint.first = 120;
#else
				this->Center.first = 2;
				this->Upper.first = 6;
				this->Upper2.first = 7;
				this->LeftEye.first = 9;
				this->RightEye.first = 9;
				this->RightWrist.first = 37;
				this->RightHandJoint.first = 57;
#endif

				this->Upper.second.pos = this->obj.GetFrameLocalMatrix(this->Upper.first).pos();

				this->obj.get_anime((int)CharaAnimeID::Upper_Down).per = 1.f;
				this->obj.get_anime((int)CharaAnimeID::Bottom_Stand).per = 1.f;
				this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time = this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).alltime;
			}
			void Execute() {
				{
					//移動ベクトル取得
					{
						SetVec(0, this->m_Press_GoFront || this->m_isSprint);
						SetVec(1, this->m_Press_GoLeft);
						SetVec(2, this->m_Press_GoRear);
						SetVec(3, this->m_Press_GoRight);
					}
					//
					this->shotFlag_First = false;
					if (this->m_Press_Shot && !this->shotFlag && !this->boltFlag) {
						this->UpperTimer = 0.1f;
						if (this->viewPer >= 0.9f) {
							this->viewPer = 1.f;
							this->shotFlag = true;
							this->shotFlag_First = true;
							this->Gun_Ptr->SetBullet();
						}
					}
					else {
						this->UpperTimer += 1.f / FPS;
						this->UpperTimer = std::clamp(this->UpperTimer, 0.f, this->m_Press_Aim ? 0.f : UpperTimerLimit);
						this->UpperTimer = std::clamp(this->UpperTimer, 0.f, this->m_ProneSwitch ? 0.1f : UpperTimerLimit);
						if (this->m_ProneSwitch && m_VecTotal > 0.6f) {
							this->UpperTimer = UpperTimerLimit;
						}
					}
					if (this->boltFlag || this->m_isRun) {
						this->UpperTimer = UpperTimerLimit;
					}
					easing_set(&this->viewPer, (this->UpperTimer < UpperTimerLimit) ? 1.f : 0.f, 0.9f);
					easing_set(&this->RunPer, this->m_isRun ? 1.f : 0.f, 0.975f);
					easing_set(&this->m_SprintPer, this->m_isSprint ? 1.f : 0.f, 0.95f);
					easing_set(&this->m_SquatPer, this->m_SquatSwitch ? 1.f : 0.f, 0.9f);
					easing_set(&this->m_PronePer, this->m_ProneSwitch ? 1.f : 0.f, 0.925f);
					//m_yrad_Upper、m_yrad_Bottom、m_zrad決定
					{
						auto RadAbs = abs(this->m_yrad - this->m_yrad_Upper);
						if (RadAbs > deg2rad(50.f)) { this->m_TurnBody = true; }
						if (RadAbs < deg2rad(0.5f)) {
							if (this->m_TurnBody) { this->m_yrad_Upper = this->m_yrad; }
							this->m_TurnBody = false;
						}
						if (this->m_Vec[0] > 0.f || this->m_Vec[2] > 0.f || this->m_Vec[3] > 0.f || this->m_Vec[1] > 0.f || this->m_ProneSwitch) { this->m_TurnBody = true; }
						//
						auto FrontP = (this->m_isSprint || (this->m_Press_GoFront && !this->m_Press_GoRear)) ? (atan2f(this->m_Vec[1] - this->m_Vec[3], this->m_Vec[0] - this->m_Vec[2])*this->m_Vec[0]) : 0.f;
						FrontP += (!this->m_Press_GoFront && this->m_Press_GoRear) ? (atan2f(-(this->m_Vec[1] - this->m_Vec[3]), -(this->m_Vec[0] - this->m_Vec[2]))*this->m_Vec[2]) : 0.f;
						auto TmpRunPer = 0.7f*this->RunPer + 0.9f*(1.f - this->RunPer);
						TmpRunPer = 0.f* this->m_PronePer + TmpRunPer * (1.f - this->m_PronePer);
						if (this->m_TurnBody) { easing_set(&this->m_yrad_Upper, this->m_yrad, TmpRunPer); }
						auto OLDP = this->m_yrad_Bottom;
						easing_set(&this->m_yrad_Bottom, this->m_yrad_Upper - FrontP * (1.f - this->m_PronePer), TmpRunPer);
						easing_set(&this->m_zrad, (this->m_yrad_Bottom - OLDP)*2.f, 0.9f);
					}
				}
				//上半身回転
				{
					this->Upper.second.mat = MATRIX_ref::RotX(this->m_xrad) * MATRIX_ref::RotY(this->m_yrad - this->m_yrad_Bottom);
					this->obj.frame_Reset(this->Upper.first);
					auto TmpOne = this->obj.GetFrameLocalMatrix(this->Upper.first).GetRot() * this->Upper.second.mat;
					this->obj.SetFrameLocalMatrix(this->Upper.first, TmpOne * MATRIX_ref::Mtrans(this->Upper.second.pos));
				}
				//AnimUpdte
				{
					SetUpperAnim();
					this->obj.get_anime((int)CharaAnimeID::Mid_Squat).per = this->m_SquatPer;
					SetBottomAnim();
				}
				//SetMat指示
				{
					RunPer2 = ((SpeedLimit*(1.f + 0.5f*this->m_SprintPer)) * this->RunPer + 0.35f * (1.f - this->RunPer));
					RunPer2 = RunPer2 *(1.f - this->m_SquatPer) + 0.15f * this->m_SquatPer;

					RunPer2 = RunPer2 * (1.f - this->m_PronePer) + 0.1f * this->m_PronePer;

					auto OLDpos = this->move.pos;
					auto yOLD = this->move.vec.y();
					this->move.vec.clear();
					this->move.vec += MATRIX_ref::RotY(this->m_yrad_Upper).zvec()*-(this->m_Vec[0] - this->m_Vec[2]);
					this->move.vec += MATRIX_ref::RotY(this->m_yrad_Upper).xvec()*(this->m_Vec[1] - this->m_Vec[3]);
					m_VecTotal = this->move.vec.size();
					if (this->move.vec.size() > 0.1f) {
						this->move.vec = this->move.vec.Norm()*RunPer2 * 60.f / FPS;
					}
					this->move.vec.y(yOLD);
					{
						auto HitResult = this->m_MapCol->CollCheck_Line(
							this->move.pos + VECTOR_ref::up()*-1.f,
							this->move.pos + VECTOR_ref::up()*15.f);
						if (HitResult.HitFlag == TRUE) {
							auto yPos = this->move.pos.y();
							easing_set(&yPos, HitResult.HitPosition.y, 0.8f);
							this->move.pos.y(yPos);
							this->move.vec.y(0.f);
						}
						else {
							this->move.vec.yadd(M_GR / (FPS*FPS));
						}
					}
					this->move.pos += this->move.vec;
					auto NowPos = this->move.pos - OLDpos;
					col_wall(OLDpos, &this->move.pos, *this->m_MapCol);
					this->move.mat = MATRIX_ref::RotZ(this->m_zrad) * MATRIX_ref::RotY(this->m_yrad_Bottom);

					{
						easing_set(&this->model_move.pos, this->move.pos, 0.9f);
						this->model_move.mat = this->move.mat;
					}
					this->obj.SetMatrix(this->model_move.MatIn());
				}
				//伏せむけ
				{
					for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						m_AnimPerSave[i] = this->obj.get_anime(i).per;
					}
				}
				//伏せ
				{
					for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						if (i == (int)CharaAnimeID::All_Prone) { continue; }
						if (
							i == (int)CharaAnimeID::All_Prone 
							|| i == (int)CharaAnimeID::All_ProneShot
							|| i == (int)CharaAnimeID::All_ProneCocking
							|| i == (int)CharaAnimeID::All_ProneWalk
							) {
							continue;
						}
						this->obj.get_anime(i).per *= 1.f - this->m_PronePer;
					}
					if (!this->shotFlag && !this->boltFlag) {
						auto tmp = std::clamp(m_VecTotal, 0.f, 1.f);
						this->obj.get_anime((int)CharaAnimeID::All_Prone).per = this->m_PronePer*(1.f - tmp);
						this->obj.get_anime((int)CharaAnimeID::All_ProneWalk).per = this->m_PronePer*tmp;
					}
					else {
						this->obj.get_anime((int)CharaAnimeID::All_Prone).per = 0.f;
					}
					if (this->m_ProneSwitch) {
						this->obj.get_anime((int)CharaAnimeID::All_ProneShot).per = m_AnimPerSave[(int)CharaAnimeID::Upper_Shot];
						this->obj.get_anime((int)CharaAnimeID::All_ProneShot).time = this->obj.get_anime((int)CharaAnimeID::Upper_Shot).time;
						this->obj.get_anime((int)CharaAnimeID::All_ProneCocking).per = m_AnimPerSave[(int)CharaAnimeID::Upper_Cocking];
						this->obj.get_anime((int)CharaAnimeID::All_ProneCocking).time = this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).time;
					}
				}
				//銃座標指定(アップデート含む)
				{
						//持ち手探索
						VECTOR_ref yVec1, zVec1, Pos1;
						{
							bool changeCocking = false;
							bool changeCockingProne = false;
							if (this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).per == 1.f) {
								changeCocking = true;
								this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per = 1.f;
								this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).per = 0.f;
							}
							if (this->obj.get_anime((int)CharaAnimeID::All_ProneCocking).per == 1.f) {
								changeCockingProne = true;
								this->obj.get_anime((int)CharaAnimeID::All_Prone).per = 1.f;
								this->obj.get_anime((int)CharaAnimeID::All_ProneCocking).per = 0.f;
							}
							this->obj.get_anime((int)CharaAnimeID::RightHand).per = 0.f;
							this->obj.work_anime();

							yVec1 = MATRIX_ref::Vtrans(VECTOR_ref::vget(0, 0, -1).Norm(), (this->obj.GetFrameLocalWorldMatrix(RightWrist.first).GetRot() * GetCharaDir().Inverse()));
							zVec1 = MATRIX_ref::Vtrans(VECTOR_ref::vget(-1, -1, 0).Norm(), (this->obj.GetFrameLocalWorldMatrix(RightWrist.first).GetRot() * GetCharaDir().Inverse()));
							Pos1 = this->obj.GetFrameLocalWorldMatrix(RightHandJoint.first).pos();
							if (changeCocking) {
								this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per = 0.f;
								this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).per = 1.f;
							}
							if (changeCockingProne) {
								this->obj.get_anime((int)CharaAnimeID::All_Prone).per = 0.f;
								this->obj.get_anime((int)CharaAnimeID::All_ProneCocking).per = 1.f;
							}
							this->obj.get_anime((int)CharaAnimeID::RightHand).per = (1.f - this->m_ReadyPer);
						}
						//背負い場所探索
						VECTOR_ref yVec2, zVec2, Pos2;
						{
							yVec2 = (MATRIX_ref::RotZ(deg2rad(30))*this->obj.GetFrameLocalWorldMatrix(this->Upper2.first).GetRot()*GetCharaDir().Inverse()).xvec();
							zVec2 = (MATRIX_ref::RotZ(deg2rad(30))*this->obj.GetFrameLocalWorldMatrix(this->Upper2.first).GetRot()*GetCharaDir().Inverse()).yvec();
							Pos2 =
								this->obj.GetFrameLocalWorldMatrix(this->Upper2.first).pos() +
								this->obj.GetFrameLocalWorldMatrix(this->Upper2.first).GetRot().yvec()*-1.75f +
								this->obj.GetFrameLocalWorldMatrix(this->Upper2.first).GetRot().zvec()*1.75f;
						}
						auto yVec = yVec2 * this->m_RunReadyPer + yVec1 * (1.f - this->m_RunReadyPer);
						auto zVec = zVec2 * this->m_RunReadyPer + zVec1 * (1.f - this->m_RunReadyPer);
						auto PosBuf = Pos2 * this->m_RunReadyPer + Pos1 * (1.f - this->m_RunReadyPer);
						auto tmp_gunrat = MATRIX_ref::RotVec2(VECTOR_ref::front()*-1.f, zVec);
						tmp_gunrat *= MATRIX_ref::RotVec2(tmp_gunrat.yvec(), yVec);
						tmp_gunrat *= GetCharaDir() * MATRIX_ref::Mtrans(PosBuf);
						Gun_Ptr->SetMatrix(tmp_gunrat, this->boltFlag);
					}
				//アニメアップデート
				this->obj.work_anime();
				{
					for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						this->obj.get_anime(i).per = m_AnimPerSave[i];
					}
				}
				//物理アップデート
				if (this->m_SetReset) {
					this->m_SetReset = false;
					this->obj.PhysicsResetState();
				}
				else {
					this->obj.PhysicsCalculation(1000.0f / FPS*120.f);
				}
				//デバッグ
				{
					//printfDx("UpperTimer : %5.5f\n", this->UpperTimer);
					printfDx("ReadyPer   : %5.5f\n", this->m_ReadyPer);
					printfDx("SprintPer  : %5.5f\n", this->m_SprintPer);
					printfDx("RunPer     : %5.5f\n", this->RunPer);
				}
			}
		public:
			const auto GetEyeVector() { return (GetCharaDir().zvec()*-1.f)*(1.f - this->viewPer) + (Gun_Ptr->GetMatrix().zvec()*-1.f) * (this->viewPer); }
			const auto GetEyePosition() { return (this->obj.frame(LeftEye.first) + this->obj.frame(RightEye.first)) / 2.f + this->GetEyeVector().Norm()*0.5f; }
			const auto IsADS() { return this->UpperTimer == 0.f; }
			const auto IsRun() { return this->m_isRun; }
			const auto IsSprint() { return this->m_isSprint; }
			const auto IsProne() { return this->m_ProneSwitch; }
			const auto ShotSwitch() { return this->shotFlag_First; }
			const auto GetUpper2Position() { return this->obj.frame(Upper.first); }
			const auto GetUpper2WorldMatrix() { return this->obj.GetFrameLocalWorldMatrix(Upper.first); }
		};
	};
};
