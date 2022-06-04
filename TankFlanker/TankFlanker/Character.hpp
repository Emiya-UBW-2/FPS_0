
#pragma once

#include"Header.hpp"
namespace FPS_n2 {
	namespace Sceneclass {
		class CharacterClass {
			enum class CharaAnimeID : int {
				Bottom_Run,
				RightHand,

				Upper_Aim,
				Upper_Down,
				Upper_Shot,
				Upper_Cocking,

				Bottom_Stand,
				Bottom_Walk,
				Bottom_Turn,
				Bottom_LeftStep,
				Bottom_RightStep,

				Upper_Running,
				Bottom_WalkBack,
				Upper_RunningStart,
			};

			//�L�����p�����[�^�[
			float SpeedLimit{ 2.f };
			float UpperTimerLimit = 3.f;
		private:
			std::pair<int, moves> Upper;
			std::pair<int, moves> Upper2;
			std::pair<int, moves> RightHandJoint;
			std::pair<int, moves> RightWrist;
			std::pair<int, moves> LeftEye;
			std::pair<int, moves> RightEye;

			MV1 obj;

			moves  model_move;
			float model_Yrad{ 0 };

			moves move;
			float m_yPos{ 0.f };
			float m_yDown{ 0.f };
			float Speed_Front{ 0.f }, Speed_Back{ 0.f };
			float Step_Left{ 0.f }, Step_Right{ 0.f };
			float m_xrad = 0.f, m_yrad = 0.f;
			float m_yrad_real = 0.f;
			float m_yrad_real2 = 0.f;
			bool m_TurnBody = false;
			float UpperTimer = 100.f;
			bool shotFlag_First{ false };
			bool shotFlag{ false };
			bool boltFlag{ false };
			float viewPer{ 0.f };
			bool m_SetReset{ true };
			bool m_WallHit{ true };
			GunClass* Gun_Ptr{ nullptr };
			float RunPer = 0.f;
			float RunPer2 = 0.f;
			bool m_isRun{ false };

			bool m_Ready{ false };
			float m_ReadyPer{ 0.f };
			bool m_RunReady{ false };
			bool m_Running{ false };
			float m_RunReadyPer{ 0.f };
		public:
			CharacterClass() {
				//this->moves move;
				this->Speed_Front = 0.f;
				this->Speed_Back = 0.f;
				this->Step_Left = 0.f;
				this->Step_Right = 0.f;
				this->m_xrad = 0.f;
				this->m_yrad = 0.f;
				this->m_yrad_real = 0.f;
				this->m_yrad_real2 = 0.f;
				this->m_TurnBody = false;
				//this->moves;
				this->UpperTimer = 100.f;
				this->shotFlag_First = false;
				this->shotFlag = false;
				this->boltFlag = false;
				this->viewPer = 0.f;
				this->m_SetReset = true;
				this->m_WallHit = false;
				this->Gun_Ptr = nullptr;
				this->RunPer = 0.f;
				this->m_isRun = false;
				this->m_Ready = false;
				this->m_ReadyPer = 0.f;
				this->m_RunReady = false;
				this->m_Running = false;
			}
			~CharacterClass() {}
		public:
			void LoadModel(const char* filepath) {
				FILEINFO FileInfo;
				std::string Path = filepath;
				if (FileRead_findFirst((Path + ".mv1").c_str(), &FileInfo) != (DWORD_PTR)-1) {
					MV1::Load((Path + ".pmx").c_str(), &this->obj, DX_LOADMODEL_PHYSICS_REALTIME);
					//MV1::Load((Path + ".mv1").c_str(), &this->obj, DX_LOADMODEL_PHYSICS_REALTIME);
				}
				else {
					MV1::Load((Path + ".pmx").c_str(), &this->obj, DX_LOADMODEL_PHYSICS_REALTIME);
					MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
					MV1SaveModelToMV1File(this->obj.get(), (Path + ".mv1").c_str());
					MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
				}
				MV1::SetAnime(&this->obj, this->obj);

			}
			void Set(GunClass* pGunPtr) {
#if true
				this->Upper.first = 5;
				this->Upper2.first = 6;
				this->LeftEye.first = 11;
				this->RightEye.first = 13;
				this->RightWrist.first = 119;
				this->RightHandJoint.first = 120;
#else
				this->Upper.first = 6;
				this->Upper2.first = 7;
				this->LeftEye.first = 9;
				this->RightEye.first = 9;
				this->RightWrist.first = 37;
				this->RightHandJoint.first = 57;
#endif

				this->Upper.second.pos = this->obj.GetFrameLocalMatrix(this->Upper.first).pos();

				this->Gun_Ptr = pGunPtr;

				move.mat.clear();
				move.pos.clear();
				move.vec.clear();

				model_move.pos = move.pos;
				model_Yrad = 0.f;
			}
		private:
			void SetAnimLoop(int ID, float speed) {
				this->obj.get_anime(ID).time += 30.f / FPS * speed;
				if (this->obj.get_anime(ID).TimeEnd()) { this->obj.get_anime(ID).time = 0.f; }
			}
			void SetBottomAnim(int ID) {
				std::vector<CharaAnimeID> animes;
				animes.emplace_back(CharaAnimeID::Bottom_Run);
				animes.emplace_back(CharaAnimeID::Bottom_Walk);
				animes.emplace_back(CharaAnimeID::Bottom_LeftStep);
				animes.emplace_back(CharaAnimeID::Bottom_RightStep);
				animes.emplace_back(CharaAnimeID::Bottom_WalkBack);
				for (const auto& a : animes) {
					this->obj.get_anime((int)a).per += ((a == (CharaAnimeID)ID) ? 2.f : -2.f) / FPS;
					this->obj.get_anime((int)a).per = std::clamp(this->obj.get_anime((int)a).per, 0.f, 1.f);
				}
				this->obj.get_anime((int)CharaAnimeID::Bottom_Turn).per = (this->Speed_Front == 0.f) ? (abs(rad2deg(this->m_yrad - this->m_yrad_real))) / 50.f : 0.f;

				SetAnimLoop((int)CharaAnimeID::Bottom_Run, 0.5f*(this->Speed_Front * RunPer2));
				SetAnimLoop((int)CharaAnimeID::Bottom_Walk, 3.25f*(this->Speed_Front * 0.35f));
				SetAnimLoop((int)CharaAnimeID::Bottom_Turn, 0.5f);
				SetAnimLoop((int)CharaAnimeID::Bottom_LeftStep, 3.25f*(this->Step_Left * 0.35f));
				SetAnimLoop((int)CharaAnimeID::Bottom_RightStep, 3.25f*(this->Step_Right * 0.35f));
				SetAnimLoop((int)CharaAnimeID::Bottom_WalkBack, 3.25f*(this->Speed_Back * 0.35f));
			}
			const auto GetCharaDir() { return this->Upper.second.mat * this->move.mat; }

			// �v���C���[�֌W�̒�`
#define PLAYER_ENUM_DEFAULT_SIZE	(1.8f * 12.5f)		// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_HIT_WIDTH			(0.4f * 12.5f)		// �����蔻��J�v�Z���̔��a
#define PLAYER_HIT_HEIGHT			(1.8f * 12.5f)		// �����蔻��J�v�Z���̍���
#define PLAYER_HIT_TRYNUM			(16)				// �ǉ����o�������̍ő厎�s��
#define PLAYER_HIT_SLIDE_LENGTH		(0.015f * 12.5f)	// ��x�̕ǉ����o�������ŃX���C�h�����鋗��
//�ǔ��胆�j�o�[�T��
			static bool col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos, const MV1& col_obj_t) noexcept {
				auto MoveVector = *NowPos - OldPos;
				// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
				auto HitDim = col_obj_t.CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), 0, 0);
				std::vector<MV1_COLL_RESULT_POLY*> kabe_;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
				// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
				for (int i = 0; i < HitDim.HitNum; ++i) {
					auto& h_d = HitDim.Dim[i];
					//�ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{0.1f��荂���|���S���̂ݓ����蔻����s��
					if (
						(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
						&& (h_d.Position[0].y > OldPos.y() + 0.1f || h_d.Position[1].y > OldPos.y() + 0.1f || h_d.Position[2].y > OldPos.y() + 0.1f)
						&& (h_d.Position[0].y < OldPos.y() + 1.6f || h_d.Position[1].y < OldPos.y() + 1.6f || h_d.Position[2].y < OldPos.y() + 1.6f)
						) {
						kabe_.emplace_back(&h_d);// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
					}
				}
				bool HitFlag = false;
				// �ǃ|���S���Ƃ̓����蔻�菈��
				if (kabe_.size() > 0) {
					HitFlag = false;
					for (auto& KeyBind : kabe_) {
						if (Hit_Capsule_Tri(*NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_WIDTH, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, KeyBind->Position[0], KeyBind->Position[1], KeyBind->Position[2])) {				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
							HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
							if (MoveVector.size() >= 0.0001f) {	// x����y�������� 0.0001f �ȏ�ړ������ꍇ�͈ړ������Ɣ���
								// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
								*NowPos = VECTOR_ref(KeyBind->Normal).cross(MoveVector.cross(KeyBind->Normal)) + OldPos;
								bool j = false;
								for (auto& b_ : kabe_) {
									if (Hit_Capsule_Tri(*NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_WIDTH, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {
										j = true;
										break;// �������Ă����烋�[�v���甲����
									}
								}
								if (!j) {
									HitFlag = false;
									break;//�ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂ŕǂɓ��������t���O��|������Ń��[�v���甲����
								}
							}
							else {
								break;
							}
						}
					}
					//*
					if (
						HitFlag
						) {		// �ǂɓ������Ă�����ǂ��牟���o���������s��
						for (int k = 0; k < PLAYER_HIT_TRYNUM; ++k) {			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
							bool HitF = false;
							for (auto& KeyBind : kabe_) {
								if (Hit_Capsule_Tri(*NowPos + VECTOR_ref::vget(0.0f, 0.2f, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, KeyBind->Position[0], KeyBind->Position[1], KeyBind->Position[2])) {// �v���C���[�Ɠ������Ă��邩�𔻒�
									*NowPos += VECTOR_ref(KeyBind->Normal) * PLAYER_HIT_SLIDE_LENGTH;					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
									bool j = false;
									for (auto& b_ : kabe_) {
										if (Hit_Capsule_Tri(*NowPos + VECTOR_ref::vget(0.0f, 0.2f, 0.0f), *NowPos + VECTOR_ref::vget(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {// �������Ă����烋�[�v�𔲂���
											j = true;
											break;
										}
									}
									if (!j) {// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
										break;
									}
									HitF = true;
								}
							}
							if (!HitF) {//�S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
								break;
							}
						}
					}
					//*/
					kabe_.clear();
				}
				MV1CollResultPolyDimTerminate(HitDim);	// ���o�����v���C���[�̎��͂̃|���S�������J������
				return HitFlag;
			}
		public:
			void Execute(
				float pAddxRad, float pAddyRad,
				bool pGoFrontPress,
				bool pGoBackPress,
				bool pGoLeftPress,
				bool pGoRightPress,
				bool pShotPress,
				bool pAimPress,
				bool pRunPress,
				const MV1& MapCol
			) {
				this->m_isRun = pRunPress;

				CharaAnimeID BottomAnimSel = CharaAnimeID::Bottom_Stand;
				{
					if (pGoFrontPress) {
						if (this->m_isRun) {
							BottomAnimSel = CharaAnimeID::Bottom_Run;
						}
						else {
							BottomAnimSel = CharaAnimeID::Bottom_Walk;
						}
					}
					else {
						if (pGoLeftPress) {
							BottomAnimSel = CharaAnimeID::Bottom_LeftStep;
						}
						if (pGoRightPress) {
							BottomAnimSel = CharaAnimeID::Bottom_RightStep;
						}
						if (pGoBackPress) {
							BottomAnimSel = CharaAnimeID::Bottom_WalkBack;
						}
					}
					if (pGoFrontPress) {
						this->Speed_Front += 2.f / 60.f * 60.f / FPS;
					}
					else {
						this->Speed_Front -= 2.f / 60.f * 60.f / FPS;
						if (pGoLeftPress || pGoRightPress) {
							this->m_isRun = false;
						}
					}
					if (pGoBackPress) {
						this->Speed_Back += 2.f / 60.f * 60.f / FPS;
						this->m_isRun = false;
					}
					else {
						this->Speed_Back -= 2.f / 60.f * 60.f / FPS;
						if (!pGoFrontPress) {
							if (pGoLeftPress || pGoRightPress) {
								this->m_isRun = false;
							}
						}
					}
					if (pGoLeftPress) {
						this->Step_Left += 2.f / 60.f * 60.f / FPS;
					}
					if (pGoRightPress) {
						this->Step_Right += 2.f / 60.f * 60.f / FPS;
					}
					if (!pGoLeftPress) {
						this->Step_Left -= 2.f / 60.f * 60.f / FPS;
					}
					if (!pGoRightPress) {
						this->Step_Right -= 2.f / 60.f * 60.f / FPS;
					}
					this->Speed_Front = std::clamp(this->Speed_Front, 0.f, 1.f);
					this->Speed_Back = std::clamp(this->Speed_Back, 0.f, 1.f);
					this->Step_Left = std::clamp(this->Step_Left, 0.f, 1.f);
					this->Step_Right = std::clamp(this->Step_Right, 0.f, 1.f);
				}
				{
					if (this->m_isRun) {
						easing_set(&RunPer, 1.f, 0.975f);
					}
					else {
						easing_set(&RunPer, 0.f, 0.975f);
					}
				}
				{
					this->m_Ready = !this->m_isRun || this->boltFlag;
					if (this->m_Ready) {
						this->m_RunReady = false;
						//this->m_ReadyPer = 1.f;
						easing_set(&this->m_ReadyPer, 1.f, 0.9f);
					}
					else {
						this->m_RunReady = true;
						this->m_ReadyPer = 0.f;
						//easing_set(&this->m_ReadyPer, 0.f, 0.9f);
					}
				}
				{
					float PerBuf = this->RunPer * 0.8f;
					float limchange = powf(1.f - this->Speed_Front, 0.5f)*PerBuf + 1.f*(1.f - PerBuf);
					this->m_xrad = std::clamp(this->m_xrad + pAddxRad, -deg2rad(40.f)*limchange, deg2rad(25.f)*limchange);
					this->m_yrad += pAddyRad;
				}
				if (abs(rad2deg(this->m_yrad - this->m_yrad_real)) > 50.f) { this->m_TurnBody = true; }
				if (abs(rad2deg(this->m_yrad - this->m_yrad_real)) < 0.5f) {
					if (this->m_TurnBody) { this->m_yrad_real = this->m_yrad; }
					this->m_TurnBody = false;
				}
				if (this->Speed_Front > 0.f || this->Speed_Back > 0.f || this->Step_Right > 0.f || this->Step_Left > 0.f) { this->m_TurnBody = true; }
				
				{
					auto FrontP = (pGoFrontPress && !pGoBackPress) ? (atan2f(this->Step_Left - this->Step_Right, this->Speed_Front - this->Speed_Back)) : 0.f;
					FrontP += (!pGoFrontPress && pGoBackPress) ? (atan2f(-(this->Step_Left - this->Step_Right), -(this->Speed_Front - this->Speed_Back))) : 0.f;
					if (this->m_isRun) {
						if (this->m_TurnBody) { easing_set(&this->m_yrad_real, this->m_yrad, 0.7f); }
						easing_set(&this->m_yrad_real2, this->m_yrad_real - FrontP, 0.7f);
					}
					else {
						if (this->m_TurnBody) { easing_set(&this->m_yrad_real, this->m_yrad, 0.9f); }
						easing_set(&this->m_yrad_real2, this->m_yrad_real - FrontP, 0.9f);
					}
				}

				this->Upper.second.mat = MATRIX_ref::RotX(this->m_xrad) * MATRIX_ref::RotY(this->m_yrad - this->m_yrad_real2);
				this->obj.SetFrameLocalMatrix(this->Upper.first, this->Upper.second.MatIn());

				this->obj.frame_Reset(this->Upper.first);
				auto TmpOne = this->obj.GetFrameLocalMatrix(this->Upper.first).GetRot() * this->Upper.second.mat;
				this->obj.SetFrameLocalMatrix(this->Upper.first, TmpOne * MATRIX_ref::Mtrans(this->Upper.second.pos));
				printfDx("UpperTimer : %5.5f\n", this->m_ReadyPer);

				if (this->m_RunReady) {
					if (!this->m_Running) {
						if (this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).per == 0.f) {
							this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).per = 1.f;
							this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time = 0.f;
						}
						if (!this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).TimeEnd()) {
							this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time += 30.f / FPS;
							if (this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).TimeEnd()) {
								this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time = this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).alltime;
							}
							if (this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).time > 16) {
								easing_set(&this->m_RunReadyPer, 1.f, 0.9f);
							}
						}
						else {
							this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).per = 0.f;
							this->m_Running = true;
							this->obj.get_anime((int)CharaAnimeID::Upper_Running).per = 1.f;
							this->obj.get_anime((int)CharaAnimeID::Upper_Running).time = 0.f;
							this->m_RunReadyPer = 1.f;
						}
					}
					else {

					}
				}
				else {
					easing_set(&this->m_RunReadyPer, 0.f, 0.9f);
					this->obj.get_anime((int)CharaAnimeID::Upper_RunningStart).per = 0.f;
					this->obj.get_anime((int)CharaAnimeID::Upper_Running).per = 1.f*(1.f - this->m_ReadyPer);
					this->m_Running = false;
				}

				SetAnimLoop((int)CharaAnimeID::Upper_Running, 0.5f*(this->Speed_Front * RunPer2));

				SetBottomAnim((int)BottomAnimSel);
				//
				if (this->shotFlag) {
					this->obj.get_anime((int)CharaAnimeID::Upper_Shot).per = 1.f;
					SetAnimLoop((int)CharaAnimeID::Upper_Shot, 1.5f);
					if (this->obj.get_anime((int)CharaAnimeID::Upper_Shot).time == 0.f) {
						this->obj.get_anime((int)CharaAnimeID::Upper_Shot).per = 0.f;
						this->shotFlag = false;
						this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per = 1.f;
						this->boltFlag = true;
					}
					this->shotFlag_First = false;
				}
				if (this->boltFlag) {
					this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).per = 1.f;
					SetAnimLoop((int)CharaAnimeID::Upper_Cocking, 1.5f);
					if (this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).time == 0.f) {
						this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).per = 0.f;
						this->boltFlag = false;
						this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per = 1.f;
						this->UpperTimer = 0.1f;
					}
				}
				if (!this->shotFlag && !this->boltFlag) {
					if (this->UpperTimer < UpperTimerLimit) {
						this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per += 15.f / 60.f * 60.f / FPS;
					}
					else {
						this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per -= 5.f / 60.f * 60.f / FPS;
					}
				}
				else {
					this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per = 0.f;
				}
				//
				this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per = std::clamp(this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per, 0.f, 1.f);
				//stand
				{
					float standPer = 0.f;
					if (this->Speed_Front == 0.f) {
						standPer = 1.f - this->obj.get_anime((int)CharaAnimeID::Bottom_Turn).per;
					}
					else if (this->m_isRun) {
						standPer = 1.f - this->obj.get_anime((int)CharaAnimeID::Bottom_Run).per;
					}
					else {
						if (this->Speed_Back == 0.f) {
							standPer = 1.f - this->obj.get_anime((int)CharaAnimeID::Bottom_Walk).per;
						}
						else {
							standPer = 1.f - this->obj.get_anime((int)CharaAnimeID::Bottom_WalkBack).per;
						}
					}
					easing_set(&this->obj.get_anime((int)CharaAnimeID::Bottom_Stand).per, standPer, 0.95f);
				}
				//�e����
				this->obj.get_anime((int)CharaAnimeID::Upper_Down).per = (!this->shotFlag && !this->boltFlag) ? 1.f - this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per : 0.f;
				//SetMat�w��
				{
					RunPer2 = SpeedLimit * RunPer + 0.35f * (1.f - RunPer);

					auto OLDpos = this->move.pos;
					this->move.vec.clear();
					this->move.vec += MATRIX_ref::RotY(this->m_yrad_real).zvec()*-(this->Speed_Front - this->Speed_Back);
					this->move.vec += MATRIX_ref::RotY(this->m_yrad_real).xvec()*(this->Step_Left - this->Step_Right);
					if (this->move.vec.size() > 0.1f) {
						this->move.vec = this->move.vec.Norm()*RunPer2;
					}
					{
						auto HitResult = MapCol.CollCheck_Line(
							this->move.pos + VECTOR_ref::up()*-1.f,
							this->move.pos + VECTOR_ref::up()*10.f);
						if (HitResult.HitFlag == TRUE) {
							easing_set(&m_yPos, HitResult.HitPosition.y + -1.f, 0.8f);
							this->m_yDown = 0.f;
						}
						else {
							this->m_yPos += this->m_yDown;
							this->m_yDown += M_GR / (FPS * FPS);
						}
					}
					this->move.pos += this->move.vec * 60.f / FPS;
					//*
					auto NowPos = this->move.pos - OLDpos;
					if (col_wall(OLDpos, &this->move.pos, MapCol)) {
						if (!this->m_WallHit) {
							this->m_WallHit = true;
							this->m_SetReset = true;
						}
					}
					else {
						this->m_WallHit = false;
					}
					//*/
					this->move.pos.y(m_yPos);
					this->move.mat = MATRIX_ref::RotY(this->m_yrad_real2);

					{
						easing_set(&this->model_move.pos, this->move.pos, 0.9f);
						this->model_move.mat = this->move.mat;
					}
					this->obj.SetMatrix(this->model_move.MatIn());
				}
				//
				{
					this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per *= this->m_ReadyPer;
					this->obj.get_anime((int)CharaAnimeID::Upper_Down).per *= this->m_ReadyPer;
					this->obj.get_anime((int)CharaAnimeID::Upper_Shot).per *= this->m_ReadyPer;
					//this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).per *= this->m_ReadyPer;
					//Upper_Running,

					bool change5 = false;
					if (this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).per == 1.f) {
						change5 = true;
						this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per = 1.f;
						this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).per = 0.f;
					}
					this->obj.get_anime((int)CharaAnimeID::RightHand).per = 0.f;
					this->obj.work_anime();

					auto yVec = MATRIX_ref::Vtrans(VECTOR_ref::vget(0, 0, -1).Norm(), (this->obj.GetFrameLocalWorldMatrix(RightWrist.first).GetRot() * GetCharaDir().Inverse()));
					auto zVec = MATRIX_ref::Vtrans(VECTOR_ref::vget(-1, -1, 0).Norm(), (this->obj.GetFrameLocalWorldMatrix(RightWrist.first).GetRot() * GetCharaDir().Inverse()));
					auto PosBuf = this->obj.GetFrameLocalWorldMatrix(RightHandJoint.first).pos();
					if (change5) {
						this->obj.get_anime((int)CharaAnimeID::Upper_Aim).per = 0.f;
						this->obj.get_anime((int)CharaAnimeID::Upper_Cocking).per = 1.f;
					}
					this->obj.get_anime((int)CharaAnimeID::RightHand).per = 1.f * this->m_ReadyPer;
					this->obj.work_anime();
					//�w����
					auto yVec2 = this->obj.GetFrameLocalMatrix(this->Upper2.first).GetRot().xvec();
					auto zVec2 = this->obj.GetFrameLocalMatrix(this->Upper2.first).GetRot().yvec();
					auto PosBuf2 =
						this->obj.GetFrameLocalWorldMatrix(this->Upper2.first).pos() + 
					this->obj.GetFrameLocalWorldMatrix(this->Upper2.first).GetRot().yvec()*-1.75f +
					this->obj.GetFrameLocalWorldMatrix(this->Upper2.first).GetRot().zvec()*1.75f;

					yVec = yVec2* this->m_RunReadyPer + yVec * (1.f - this->m_RunReadyPer);
					zVec = zVec2 * this->m_RunReadyPer + zVec * (1.f - this->m_RunReadyPer);
					PosBuf = PosBuf2* this->m_RunReadyPer + PosBuf * (1.f - this->m_RunReadyPer);

					auto tmp_gunrat = MATRIX_ref::RotVec2(VECTOR_ref::front()*-1.f, zVec);
					tmp_gunrat *= MATRIX_ref::RotVec2(tmp_gunrat.yvec(), yVec);
					tmp_gunrat *= GetCharaDir() * MATRIX_ref::Mtrans(PosBuf);

					Gun_Ptr->SetMatrix(tmp_gunrat, this->boltFlag);
					if (pShotPress && !this->shotFlag && !this->boltFlag) {
						this->UpperTimer = 0.1f;
						if (this->viewPer >= 0.9f) {
							this->viewPer = 1.f;
							this->shotFlag = true;
							this->shotFlag_First = true;
						}
					}
					else {
						this->UpperTimer = std::clamp(this->UpperTimer + 1.f / FPS, 0.f, pAimPress ? 0.f : UpperTimerLimit);
					}

					if (this->boltFlag || this->m_isRun) {
						this->UpperTimer = UpperTimerLimit;
					}

					if (this->UpperTimer < UpperTimerLimit) {
						easing_set(&this->viewPer, 1.f, 0.9f);
					}
					else {
						easing_set(&this->viewPer, 0.f, 0.9f);
					}
					//printfDx("UpperTimer : %5.5f\n", this->UpperTimer);
				}
				//�����A�b�v�f�[�g
				if (this->m_SetReset) {
					this->m_SetReset = false;
					this->obj.PhysicsResetState();
				}
				else {
					this->obj.PhysicsCalculation(1000.0f / FPS);
				}
			}
			void Draw() {
				this->obj.DrawModel();
			}
			void Dispose() {
				this->obj.Dispose();

			}
		public:
			const auto GetEyeVector() { return (GetCharaDir().zvec()*-1.f)*(1.f - this->viewPer) + (Gun_Ptr->GetMatrix().zvec()*-1.f) * (this->viewPer); }
			const auto GetEyePosition() { return (this->obj.frame(LeftEye.first) + this->obj.frame(RightEye.first)) / 2.f + this->GetEyeVector().Norm()*0.5f; }
			const auto IsADS() { return this->UpperTimer == 0.f; }
			const auto IsRun() { return this->m_isRun; }
			const auto ShotSwitch() { return this->shotFlag_First; }
			const auto GetUpper2Position() { return this->obj.frame(Upper.first); }
			const auto GetCharaMatrix() { return this->obj.GetMatrix(); }
			const auto GetCharaModelMatrix() { return this->move.MatIn(); }
			const auto GetUpper2WorldMatrix() { return this->obj.GetFrameLocalWorldMatrix(Upper.first); }
		};
	};
};
