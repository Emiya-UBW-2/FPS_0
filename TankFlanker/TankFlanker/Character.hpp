#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CharacterClass : public ObjectBaseClass {
			//キャラパラメーター
			float SpeedLimit{ 2.f };
			float UpperTimerLimit = 3.f;
		private:
			moves  move_r;
			std::array<float, 4> m_Vec;
			std::array<float, (int)CharaAnimeID::AnimeIDMax> m_AnimPerSave;
			float m_VecTotal{ 0.f };
			float m_xrad_Add{ 0.f }, m_yrad_Add{ 0.f };
			float m_xrad_Buf{ 0.f }, m_yrad_Buf{ 0.f };
			float m_xrad{ 0.f }, m_yrad{ 0.f }, m_zrad{ 0.f };
			float m_yrad_Upper{ 0.f }, m_yrad_Bottom{ 0.f };
			float m_UpperTimer{ 0.f };
			float m_RunPer2{ 0.f };
			float m_PrevRunPer2{ 0.f };
			float m_RunTimer{ false };
			float m_ReadyPer{ 0.f };
			float m_RunReadyPer{ 0.f };
			//
			float m_SprintPer{ 0.f };
			float m_ViewPer{ 0.f };
			float m_SquatPer{ 0.f };
			float m_PronePer{ 0.f };
			float m_RunPer{ 0.f };
			VECTOR_ref m_ProneNormal{ VECTOR_ref::up() };
			bool m_TurnBody{ false };
			bool m_ShotFlag_First{ false };
			bool m_ShotFlag{ false };
			bool m_BoltFlag{ false };
			bool m_SetReset{ true };
			bool m_IsRun{ false };
			bool m_IsSprint{ false };
			bool m_Ready{ false };
			bool m_RunReady{ false };
			bool m_RunReadyFirst{ false };
			bool m_Running{ false };
			bool m_ReturnStand = false;
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
			float m_HeartRate{ 60.f };//心拍数
			float m_HeartRate_r{ 60.f };//心拍数
			float m_HeartRateRad{ 0.f };//呼吸Sin渡し
			//銃
			std::shared_ptr<GunClass> m_Gun_Ptr{ nullptr };
			//サウンド
			SoundHandle m_RunFootL;
			SoundHandle m_RunFootR;
			SoundHandle m_Sliding;
			SoundHandle m_SlidingL;
			SoundHandle m_SlidingR;
			SoundHandle m_Standup;
		private://ゲッター
			const auto GetCharaDir() { return this->Frames[(int)CharaFrame::Upper].second.mat * this->move.mat; }
		public:
			const auto GetFrameWorldMatrix(CharaFrame frame) { return this->obj.GetFrameLocalWorldMatrix(Frames[(int)frame].first); }
			const auto GetEyeVector() { return Leap(GetCharaDir().zvec(), this->m_Gun_Ptr->GetMatrix().zvec(), this->m_ViewPer)*-1.f; }
			const auto GetEyePosition() { return (this->obj.frame(Frames[(int)CharaFrame::LeftEye].first) + this->obj.frame(Frames[(int)CharaFrame::RightEye].first)) / 2.f + this->GetEyeVector().Norm()*0.5f; }
			const auto IsADS() { return this->m_UpperTimer == 0.f; }
			const auto& IsRun() { return this->m_IsRun; }
			const auto& IsSprint() { return this->m_IsSprint; }
			const auto IsProne() { return this->m_Prone.on(); }
			const auto& ShotSwitch() { return this->m_ShotFlag_First; }
			const auto& GetHeartRate() { return this->m_HeartRate; }
			const auto& GetHeartRateRad() { return this->m_HeartRateRad; }
			void SetGunPtr(std::shared_ptr<GunClass>& pGunPtr) { this->m_Gun_Ptr = pGunPtr; }
		private:
			void SetAnimLoop(int ID, float speed) {
				this->obj.get_anime(ID).time += 30.f / FPS * speed;
				if (this->obj.get_anime(ID).TimeEnd()) { this->obj.get_anime(ID).time = 0.f; }
			}
			void ExecuteAnim() {
				std::vector<CharaAnimeID> Upperanimes;
				Upperanimes.emplace_back(CharaAnimeID::Upper_Down);
				Upperanimes.emplace_back(CharaAnimeID::Upper_Aim);
				Upperanimes.emplace_back(CharaAnimeID::Upper_Shot);
				Upperanimes.emplace_back(CharaAnimeID::Upper_Cocking);
				Upperanimes.emplace_back(CharaAnimeID::Upper_RunningStart);
				Upperanimes.emplace_back(CharaAnimeID::Upper_Running);
				Upperanimes.emplace_back(CharaAnimeID::Upper_Sprint);

				std::vector<CharaAnimeID> Bottomanimes;
				Bottomanimes.emplace_back(CharaAnimeID::Bottom_Run);
				Bottomanimes.emplace_back(CharaAnimeID::Bottom_Walk);
				Bottomanimes.emplace_back(CharaAnimeID::Bottom_LeftStep);
				Bottomanimes.emplace_back(CharaAnimeID::Bottom_RightStep);
				Bottomanimes.emplace_back(CharaAnimeID::Bottom_WalkBack);

				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					this->obj.get_anime(i).per = this->m_AnimPerSave[i];
				}
				//上半身
				{
					{
						bool canreverse = true;
						if (!this->m_RunReady && !this->m_Running && !this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).TimeEnd()) {
							this->m_RunReady = true;
							canreverse = false;
						}
						if (this->m_RunReadyFirst) {
							this->m_RunReadyPer = 0.f;
							this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time = 0.f;
						}
						if (this->m_RunReady) {
							easing_set(&this->m_ReadyPer, 1.f, 0.7f);
							if (!this->m_Running) {
								this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time += 30.f / FPS * 2.f;
								easing_set(&this->m_RunReadyPer, (canreverse && this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time > 16) ? 1.f : 0.f, 0.9f);
								this->m_SprintPer = 0.f;
								if (this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).TimeEnd()) {
									this->m_Running = true;
									this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time = this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).alltime;
									this->obj.get_anime((int)CharaAnimeID::Upper_Running).time = 0.f;
									if (canreverse) {
										this->m_RunReadyPer = 1.f;
									}
								}
							}
						}
						else {
							easing_set(&this->m_ReadyPer, 0.f, 0.9f);
							easing_set(&this->m_RunReadyPer, 0.f, 0.9f);
							this->m_Running = false;
						}
						if (this->m_ShotFlag) {
							if (!this->obj.get_anime((int)CharaAnimeID::Upper_Shot).TimeEnd()) {
								this->obj.get_anime((int)CharaAnimeID::Upper_Shot).time += 30.f / FPS * 1.5f;
							}
							if (this->obj.get_anime((int)CharaAnimeID::Upper_Shot).TimeEnd() && !this->m_Press_Shot) {
								this->m_ShotFlag = false;
								this->obj.get_anime((int)CharaAnimeID::Upper_Shot).time = 0.f;
								this->m_BoltFlag = true;
							}
						}
						if (this->m_BoltFlag) {
							this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).time += 30.f / FPS * 1.5f;
							if (this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).TimeEnd()) {
								this->m_BoltFlag = false;
								this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).time = 0.f;
								this->m_UpperTimer = 0.1f;
								this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).per = 0.f;
								this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per = 1.f;
							}
						}
					}
					CharaAnimeID UpperAnimSel = CharaAnimeID::Upper_Down;				//銃下げ
					if (this->m_RunReady) {
						if (!this->m_Running) {
							UpperAnimSel = CharaAnimeID::Upper_RunningStart;
						}
						else if (!this->m_IsSprint) {
							UpperAnimSel = CharaAnimeID::Upper_Running;
						}
						else {
							UpperAnimSel = CharaAnimeID::Upper_Sprint;
						}
					}
					if (this->m_ShotFlag) {
						UpperAnimSel = CharaAnimeID::Upper_Shot;
					}
					if (this->m_BoltFlag) {
						UpperAnimSel = CharaAnimeID::Upper_Cocking;
					}
					if (!this->m_ShotFlag && !this->m_BoltFlag && this->m_UpperTimer < UpperTimerLimit) {
						UpperAnimSel = CharaAnimeID::Upper_Aim;
					}
					for (const auto& a : Upperanimes) {
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
					SetAnimLoop((int)CharaAnimeID::Upper_Sprint, 0.5f*(this->m_Vec[0] * this->m_RunPer2));
					SetAnimLoop((int)CharaAnimeID::Upper_Running, 0.5f*(this->m_Vec[0] * this->m_RunPer2));
				}
				//真ん中
				{
					this->obj.get_anime((int)CharaAnimeID::Mid_Squat).per = this->m_SquatPer;
				}
				//下半身
				{
					CharaAnimeID BottomAnimSel = CharaAnimeID::Bottom_Stand;
					if (this->m_Press_GoFront) {
						if (this->m_IsRun) {
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
						if (this->m_IsSprint) {
							BottomAnimSel = CharaAnimeID::Bottom_Run;
						}
					}

					for (const auto& a : Bottomanimes) {
						this->obj.get_anime((int)a).per += ((a == BottomAnimSel) ? 2.f : -2.f) / FPS;
						this->obj.get_anime((int)a).per = std::clamp(this->obj.get_anime((int)a).per, 0.f, 1.f);
					}
					this->obj.get_anime((int)CharaAnimeID::Bottom_Turn).per = (this->m_Vec[0] == 0.f) ? abs(this->m_yrad - this->m_yrad_Upper) / deg2rad(50.f) : 0.f;

					//stand
					{
						float standPer = 0.f;
						if (this->m_Vec[0] == 0.f) {
							standPer = 1.f - this->obj.get_anime((int)CharaAnimeID::Bottom_Turn).per;
						}
						else if (this->m_IsRun) {
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

					SetAnimLoop((int)CharaAnimeID::Bottom_Turn, 0.5f);
					SetAnimLoop((int)CharaAnimeID::Bottom_Run, 0.5f*this->m_Vec[0] * this->m_RunPer2);
					SetAnimLoop((int)CharaAnimeID::Bottom_Walk, 1.15f*this->m_Vec[0]);
					SetAnimLoop((int)CharaAnimeID::Bottom_LeftStep, 1.15f*this->m_Vec[1]);
					SetAnimLoop((int)CharaAnimeID::Bottom_WalkBack, 1.15f*this->m_Vec[2]);
					SetAnimLoop((int)CharaAnimeID::Bottom_RightStep, 1.15f*this->m_Vec[3]);
					SetAnimLoop((int)CharaAnimeID::All_ProneWalk, 1.15f*this->m_VecTotal);
					//足音
					if (BottomAnimSel != CharaAnimeID::Bottom_Stand) {
						auto Time = this->obj.get_anime((int)BottomAnimSel).time;
						if (BottomAnimSel != CharaAnimeID::Bottom_Run) {
							//L
							if ((9.f < Time &&Time < 10.f)) {
								if (!this->m_Prone.on()) {
									if (!this->m_RunFootL.check()) {
										this->m_RunFootL.play_3D(GetFrameWorldMatrix(CharaFrame::LeftFoot).pos(), 12.5f*5.f);
									}
								}
								else {
									if (!this->m_SlidingL.check()) {
										this->m_SlidingL.play_3D(GetFrameWorldMatrix(CharaFrame::LeftFoot).pos(), 12.5f*5.f);
									}
								}
							}
							//R
							if ((27.f < Time &&Time < 28.f)) {
								if (!this->m_Prone.on()) {
									if (!this->m_RunFootR.check()) {
										this->m_RunFootR.play_3D(GetFrameWorldMatrix(CharaFrame::RightFoot).pos(), 12.5f*5.f);
									}
								}
								else {
									if (!this->m_SlidingR.check()) {
										this->m_SlidingR.play_3D(GetFrameWorldMatrix(CharaFrame::RightFoot).pos(), 12.5f*5.f);
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
									this->m_RunFootL.play_3D(GetFrameWorldMatrix(CharaFrame::LeftFoot).pos(), 12.5f*5.f);
								}
							}
							//R
							if (
								(8.f < Time &&Time < 9.f) ||
								(28.f < Time &&Time < 29.f)
								) {
								if (!this->m_RunFootR.check()) {
									this->m_RunFootR.play_3D(GetFrameWorldMatrix(CharaFrame::RightFoot).pos(), 12.5f*5.f);
								}
							}
						}
						this->m_ReturnStand = true;
					}
					else if (this->m_ReturnStand) {
						if (!this->m_Sliding.check()) {
							this->m_Sliding.vol((int)(192.f*this->m_RunPer2 / SpeedLimit));
							this->m_Sliding.play_3D(GetFrameWorldMatrix(CharaFrame::RightFoot).pos(), 12.5f*5.f);
						}
						this->m_ReturnStand = false;
					}
					if (0.1f < this->m_PronePer&&this->m_PronePer < 0.2f) {
						if (!this->m_Standup.check()) {
							this->m_Standup.play_3D(GetFrameWorldMatrix(CharaFrame::RightFoot).pos(), 12.5f*5.f);
						}
					}
				}

				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					this->m_AnimPerSave[i] = this->obj.get_anime(i).per;
				}
				//伏せ
				{
					for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
						if (i == (int)CharaAnimeID::All_Prone ||
							i == (int)CharaAnimeID::All_ProneShot ||
							i == (int)CharaAnimeID::All_ProneCocking ||
							i == (int)CharaAnimeID::All_ProneWalk
							) {
							continue;
						}
						this->obj.get_anime(i).per *= 1.f - this->m_PronePer;
					}
					if (!this->m_ShotFlag && !this->m_BoltFlag) {
						this->obj.get_anime((int)CharaAnimeID::All_Prone).per = this->m_PronePer*(1.f - this->m_VecTotal);
						this->obj.get_anime((int)CharaAnimeID::All_ProneWalk).per = this->m_PronePer*this->m_VecTotal;
					}
					else {
						this->obj.get_anime((int)CharaAnimeID::All_Prone).per = 0.f;
					}
					if (this->m_Prone.on()) {
						this->obj.get_anime((int)CharaAnimeID::All_ProneShot).per = this->m_AnimPerSave[(int)CharaAnimeID::Upper_Shot];
						this->obj.get_anime((int)CharaAnimeID::All_ProneShot).time = this->obj.get_anime((int)CharaAnimeID::Upper_Shot).time;
						this->obj.get_anime((int)CharaAnimeID::All_ProneCocking).per = this->m_AnimPerSave[(int)CharaAnimeID::Upper_Cocking];
						this->obj.get_anime((int)CharaAnimeID::All_ProneCocking).time = this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).time;
					}
				}
			}
			void ExecuteHeartRate() {
				auto addRun = (this->m_RunPer2 - this->m_PrevRunPer2);
				if (addRun > 0.f) {
					this->m_HeartRate_r += (10.f + GetRandf(10.f)) / FPS;
				}
				else if (addRun < 0.f) {
					this->m_HeartRate_r -= (5.f + GetRandf(5.f)) / FPS;
				}
				this->m_HeartRate_r += (this->m_VecTotal * this->m_RunPer2 / SpeedLimit * 2.25f) / FPS;
				this->m_HeartRate_r -= (2.f + GetRandf(4.f)) / FPS;
				this->m_HeartRate_r = std::clamp(this->m_HeartRate_r, 60.f, 180.f);

				if (this->m_HeartRate < this->m_HeartRate_r) {
					this->m_HeartRate += 5.f / FPS;
				}
				else if (this->m_HeartRate >= this->m_HeartRate_r) {
					this->m_HeartRate -= 5.f / FPS;
				}
				//this->m_HeartRate = this->m_HeartRate_r;
			}
			void SetVec(int pDir, bool Press) {
				if (Press) {
					this->m_Vec[pDir] += 2.f / 60.f * 60.f / FPS;
				}
				else {
					this->m_Vec[pDir] -= 2.f / 60.f * 60.f / FPS;
				}
				this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
			}
			void ValueSet(float pxRad, float pyRad, bool SquatOn, bool ProneOn, const VECTOR_ref& pPos) {
				//this->moves move;
				this->m_TurnBody = false;
				//this->moves;
				this->m_ShotFlag_First = false;
				this->m_ShotFlag = false;
				this->m_BoltFlag = false;
				this->m_ViewPer = 1.f;
				this->m_SetReset = true;
				this->m_Gun_Ptr.reset();
				this->m_RunPer = 0.f;
				this->m_IsSprint = false;
				this->m_Ready = false;
				this->m_ReadyPer = 0.f;
				this->m_RunReady = false;
				this->m_Running = false;
				this->m_Press_GoFront = false;
				this->m_Press_GoRear = false;
				this->m_Press_GoLeft = false;
				this->m_Press_GoRight = false;
				this->m_Press_Shot = false;
				this->m_Press_Aim = false;
				this->m_IsRun = false;
				this->m_UpperTimer = UpperTimerLimit;
				this->m_zrad = 0.f;
				this->m_Vec[0] = 0.f;
				this->m_Vec[1] = 0.f;
				this->m_Vec[2] = 0.f;
				this->m_Vec[3] = 0.f;
				this->m_VecTotal = 0.f;
				this->m_RunPer2 = 0.f;
				this->m_PrevRunPer2 = 0.f;
				this->m_HeartRate = 60.f;
				this->m_HeartRateRad = 0.f;
				this->m_xrad_Add = 0.f;
				this->m_yrad_Add = 0.f;
				//動作にかかわる操作
				this->m_Squat.Init(SquatOn);
				this->m_Prone.Init(ProneOn);
				this->m_xrad_Buf = pxRad;
				this->m_yrad_Buf = pyRad;
				//上記を反映するもの
				this->m_xrad = this->m_xrad_Buf;
				this->m_yrad = this->m_yrad_Buf;
				this->m_yrad_Upper = this->m_yrad;
				this->m_yrad_Bottom = this->m_yrad;
				this->m_SquatPer = SquatOn ? 1.f : 0.f;
				this->m_PronePer = ProneOn ? 1.f : 0.f;
				SetMove(this->m_yrad_Bottom, pPos);
				this->move_r = move;
			}
		public:
			CharacterClass() {
				m_objType = ObjType::Human;
				//ValueSet(0.f, 0.f, false, false, VECTOR_ref::vget(-230.f, 0.f, 450.f));
				ValueSet(deg2rad(0.f), deg2rad(90.f), false, false, VECTOR_ref::vget(2039.f, 90.f, -966.f));
			}
			~CharacterClass() {}
		public:
			void Init() override {
				ObjectBaseClass::Init();
				this->obj.get_anime((int)CharaAnimeID::Upper_Down).per = 1.f;
				this->obj.get_anime((int)CharaAnimeID::Bottom_Stand).per = 1.f;
				this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time = this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).alltime;

				for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
					this->m_AnimPerSave[i] = this->obj.get_anime(i).per;
				}

				SetCreate3DSoundFlag(TRUE);
				this->m_RunFootL = SoundHandle::Load("data/Sound/SE/move/runfoot.wav");
				this->m_RunFootR = SoundHandle::Load("data/Sound/SE/move/runfoot.wav");
				this->m_Sliding = SoundHandle::Load("data/Sound/SE/move/sliding.wav");
				this->m_SlidingL = SoundHandle::Load("data/Sound/SE/move/sliding.wav");
				this->m_SlidingR = SoundHandle::Load("data/Sound/SE/move/sliding.wav");
				this->m_Standup = SoundHandle::Load("data/Sound/SE/move/standup.wav");
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
			}
			void Execute() override {
				{
					//移動ベクトル取得
					{
						SetVec(0, this->m_Press_GoFront || this->m_IsSprint);
						SetVec(1, this->m_Press_GoLeft);
						SetVec(2, this->m_Press_GoRear);
						SetVec(3, this->m_Press_GoRight);
					}
					//
					this->m_ShotFlag_First = false;
					if (this->m_Press_Shot && !this->m_ShotFlag && !this->m_BoltFlag) {
						this->m_UpperTimer = 0.1f;
						if (this->m_ViewPer >= 0.9f) {
							this->m_ViewPer = 1.f;
							this->m_ShotFlag = true;
							this->m_ShotFlag_First = true;
							this->m_Gun_Ptr->SetBullet();
						}
					}
					else {
						this->m_UpperTimer += 1.f / FPS;
						this->m_UpperTimer = std::clamp(this->m_UpperTimer, 0.f, this->m_Press_Aim ? 0.f : UpperTimerLimit);
						this->m_UpperTimer = std::clamp(this->m_UpperTimer, 0.f, this->m_Prone.on() ? 0.1f : UpperTimerLimit);
						if (this->m_Prone.on() && this->m_VecTotal > 0.6f) {
							this->m_UpperTimer = UpperTimerLimit;
						}
					}
					if (this->m_BoltFlag || this->m_IsRun) {
						this->m_UpperTimer = UpperTimerLimit;
					}
					easing_set(&this->m_ViewPer, (this->m_UpperTimer < UpperTimerLimit) ? 1.f : 0.f, 0.9f);
					easing_set(&this->m_RunPer, this->m_IsRun ? 1.f : 0.f, 0.975f);
					easing_set(&this->m_SprintPer, this->m_IsSprint ? 1.f : 0.f, 0.95f);
					easing_set(&this->m_SquatPer, this->m_Squat.on() ? 1.f : 0.f, 0.9f);
					easing_set(&this->m_PronePer, this->m_Prone.on() ? 1.f : 0.f, 0.95f);
					//this->m_yrad_Upper、this->m_yrad_Bottom、this->m_zrad決定
					{
						if (abs(this->m_yrad - this->m_yrad_Upper) > deg2rad(50.f)) { this->m_TurnBody = true; }
						if (abs(this->m_yrad - this->m_yrad_Upper) < deg2rad(0.5f)) {
							if (this->m_TurnBody) { this->m_yrad_Upper = this->m_yrad; }
							this->m_TurnBody = false;
						}
						if (this->m_Vec[0] > 0.f || this->m_Vec[2] > 0.f || this->m_Vec[3] > 0.f || this->m_Vec[1] > 0.f || this->m_Prone.on()) { this->m_TurnBody = true; }
						//
						auto FrontP = (this->m_IsSprint || (this->m_Press_GoFront && !this->m_Press_GoRear)) ? (atan2f(this->m_Vec[1] - this->m_Vec[3], this->m_Vec[0] - this->m_Vec[2])*this->m_Vec[0]) : 0.f;
						FrontP += (!this->m_Press_GoFront && this->m_Press_GoRear) ? (atan2f(-(this->m_Vec[1] - this->m_Vec[3]), -(this->m_Vec[0] - this->m_Vec[2]))*this->m_Vec[2]) : 0.f;
						auto TmpRunPer = Leap(0.9f, 0.7f, this->m_RunPer);
						TmpRunPer = Leap(TmpRunPer, 0.f, this->m_PronePer);
						if (this->m_TurnBody) { easing_set(&this->m_yrad_Upper, this->m_yrad, TmpRunPer); }
						auto OLDP = this->m_yrad_Bottom;
						easing_set(&this->m_yrad_Bottom, this->m_yrad_Upper - FrontP * (1.f - this->m_PronePer), TmpRunPer);
						easing_set(&this->m_zrad, (this->m_yrad_Bottom - OLDP)*2.f, 0.9f);
					}
				}
				//上半身回転
				{
					//(int)CharaFrame::Upper
					auto* F = &this->Frames[(int)CharaFrame::Upper];
					F->second.mat = MATRIX_ref::RotX(this->m_xrad) * MATRIX_ref::RotY(this->m_yrad - this->m_yrad_Bottom);
					this->obj.frame_Reset(F->first);
					this->obj.SetFrameLocalMatrix(F->first, this->obj.GetFrameLocalMatrix(F->first).GetRot() * F->second.MatIn());
				}
				//AnimUpdte
				ExecuteAnim();
				//SetMat指示
				{
					this->m_PrevRunPer2 = this->m_RunPer2;
					this->m_RunPer2 = Leap(0.35f, (SpeedLimit*(1.f + 0.5f*this->m_SprintPer)), this->m_RunPer);
					this->m_RunPer2 = Leap(this->m_RunPer2, 0.15f, this->m_SquatPer);
					this->m_RunPer2 = Leap(this->m_RunPer2, 0.1f, this->m_PronePer);
					if (this->m_PrevRunPer2 == 0.f) {
						this->m_PrevRunPer2 = this->m_RunPer2;
					}

					auto OLDpos = this->move.pos;
					auto yOLD = this->move.vec.y();
					this->move.vec.clear();
					this->move.vec += MATRIX_ref::RotY(this->m_yrad_Upper).zvec()*-(this->m_Vec[0] - this->m_Vec[2]);
					this->move.vec += MATRIX_ref::RotY(this->m_yrad_Upper).xvec()*(this->m_Vec[1] - this->m_Vec[3]);
					this->m_VecTotal = std::clamp(this->move.vec.size(), 0.f, 1.f);

					if (this->m_VecTotal > 0.1f) {
						this->move.vec = this->move.vec.Norm()*this->m_RunPer2 * 60.f / FPS;
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

							auto HitResult2 = this->m_MapCol->CollCheck_Line(
								GetEyePosition() + VECTOR_ref::up()*-15.f,
								GetEyePosition() + VECTOR_ref::up()*15.f);
							if (HitResult2.HitFlag == TRUE) {
								easing_set(&this->m_ProneNormal, (VECTOR_ref(HitResult.Normal) + VECTOR_ref(HitResult2.Normal)) / 2.f, 0.95f);
							}
							else {
								easing_set(&this->m_ProneNormal, VECTOR_ref(HitResult.Normal), 0.95f);
							}

						}
						else {
							this->move.vec.yadd(2.f * M_GR / (FPS*FPS));
						}
					}
					this->move.pos += this->move.vec;
					auto NowPos = this->move.pos - OLDpos;
					col_wall(OLDpos, &this->move.pos, *this->m_MapCol);
					this->move.mat = MATRIX_ref::RotZ(this->m_zrad) * MATRIX_ref::RotY(this->m_yrad_Bottom)
						* MATRIX_ref::RotVec2(VECTOR_ref::up(), Leap(VECTOR_ref::up(), this->m_ProneNormal, this->m_PronePer));

					{
						easing_set(&this->move_r.pos, this->move.pos, 0.9f);
						this->move_r.mat = this->move.mat;
					}
					this->obj.SetMatrix(this->move_r.MatIn());
				}
				//銃座標指定(アニメアップデート含む)
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

						yVec1 = MATRIX_ref::Vtrans(VECTOR_ref::vget(0, 0, -1).Norm(), (this->obj.GetFrameLocalWorldMatrix(Frames[(int)CharaFrame::RightWrist].first).GetRot() * GetCharaDir().Inverse()));
						zVec1 = MATRIX_ref::Vtrans(VECTOR_ref::vget(-1, -1, 0).Norm(), (this->obj.GetFrameLocalWorldMatrix(Frames[(int)CharaFrame::RightWrist].first).GetRot() * GetCharaDir().Inverse()));
						Pos1 = this->obj.GetFrameLocalWorldMatrix(Frames[(int)CharaFrame::RightHandJoint].first).pos();
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
						yVec2 = (MATRIX_ref::RotZ(deg2rad(30))*this->obj.GetFrameLocalWorldMatrix(this->Frames[(int)CharaFrame::Upper2].first).GetRot()*GetCharaDir().Inverse()).xvec();
						zVec2 = (MATRIX_ref::RotZ(deg2rad(30))*this->obj.GetFrameLocalWorldMatrix(this->Frames[(int)CharaFrame::Upper2].first).GetRot()*GetCharaDir().Inverse()).yvec();
						Pos2 =
							this->obj.GetFrameLocalWorldMatrix(this->Frames[(int)CharaFrame::Upper2].first).pos() +
							this->obj.GetFrameLocalWorldMatrix(this->Frames[(int)CharaFrame::Upper2].first).GetRot().yvec()*-1.75f +
							this->obj.GetFrameLocalWorldMatrix(this->Frames[(int)CharaFrame::Upper2].first).GetRot().zvec()*1.75f;
					}
					auto yVec = Leap(yVec1, yVec2, this->m_RunReadyPer);
					auto zVec = Leap(zVec1, zVec2, this->m_RunReadyPer);
					auto PosBuf = Leap(Pos1, Pos2, this->m_RunReadyPer);
					auto tmp_gunrat = MATRIX_ref::RotVec2(VECTOR_ref::front()*-1.f, zVec);
					tmp_gunrat *= MATRIX_ref::RotVec2(tmp_gunrat.yvec(), yVec);
					tmp_gunrat *= GetCharaDir() * MATRIX_ref::Mtrans(PosBuf);
					this->m_Gun_Ptr->SetMatrix(tmp_gunrat, this->m_BoltFlag);
				}
				//アニメアップデート
				this->obj.work_anime();
				//物理アップデート
				if (this->m_SetReset) {
					this->m_SetReset = false;
					this->obj.PhysicsResetState();
				}
				else {
					this->obj.PhysicsCalculation(1000.0f / FPS * 240.f);
				}
				//心拍数
				ExecuteHeartRate();
				//デバッグ
				{
					//printfDx("UpperTimer : %5.5f\n", this->m_UpperTimer);
					//printfDx("ReadyPer   : %5.5f\n", this->m_ReadyPer);
					//printfDx("SprintPer  : %5.5f\n", this->m_SprintPer);
					//printfDx("RunPer     : %5.5f\n", this->m_RunPer);
				}
			}
		public:
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
				if (this->m_Prone.on() && (this->m_ShotFlag || this->m_BoltFlag)) {
					this->m_Press_GoFront = false;
					this->m_Press_GoRear = false;
					this->m_Press_GoLeft = false;
					this->m_Press_GoRight = false;
				}
				this->m_Press_Shot = pShotPress;
				if (0.01f < this->m_PronePer&&this->m_PronePer < 0.99f) {
					this->m_Press_Shot = false;
				}


				this->m_Press_Aim = pAimPress;
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
				this->m_IsSprint = this->m_IsRun && (!this->m_Press_GoFront && !this->m_Press_GoRear);
				this->m_Squat.GetInput(pSquatPress);
				if (this->m_IsRun) { this->m_Squat.first = false; }

				this->m_Prone.GetInput(pPronePress && !(this->m_ShotFlag || this->m_BoltFlag));
				if (this->m_IsRun) { this->m_Prone.first = false; }
				if (this->m_Prone.on()) { this->m_Squat.first = false; }
				{
					auto TmpReady = !(!this->m_IsRun || this->m_BoltFlag);
					this->m_RunReadyFirst = (TmpReady && !this->m_RunReady);
					this->m_RunReady = TmpReady;
				}
				{
					float limchange = Leap(1.f, powf(1.f - this->m_Vec[0], 0.5f), this->m_RunPer* 0.8f);
					limchange = Leap(limchange, 0.15f, this->m_PronePer);

					auto tmp = Leap(1.f, 0.1f, this->m_PronePer);
					this->m_xrad_Buf = std::clamp(this->m_xrad_Buf + pAddxRad * tmp, -deg2rad(40.f)*limchange, deg2rad(25.f)*limchange);
					this->m_yrad_Buf += pAddyRad * tmp;

					float tmp2 = 0.2f * GetRandf(deg2rad(1.f - this->m_PronePer));

					this->m_HeartRateRad += deg2rad(this->m_HeartRate) / FPS;

					easing_set(&this->m_xrad_Add, tmp2 + 0.0002f * sin(this->m_HeartRateRad) *powf(this->m_HeartRate / 60.f, 3.f), 0.95f);
					easing_set(&this->m_yrad_Add, tmp2 + 0.0002f * sin(this->m_HeartRateRad * 3) *powf(this->m_HeartRate / 60.f, 3.f), 0.95f);

					float tmp3 = Leap(0.5f, 0.2f, this->m_PronePer);
					tmp3 = Leap(0.35f, tmp3, this->m_SquatPer);

					this->m_xrad_Buf += this->m_xrad_Add*tmp3;
					this->m_yrad_Buf += this->m_yrad_Add*tmp3;

					easing_set(&this->m_xrad, this->m_xrad_Buf, 0.5f);
					easing_set(&this->m_yrad, this->m_yrad_Buf, 0.5f);
				}
			}
		};
	};
};
