#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//���
		class VehicleClass : public ObjectBaseClass, public EffectControl {
		private:
			//����
			bool												m_ReadySwitch{ false };							//
			bool												m_KeyActive{ true };							//
			std::array<bool, 7>									m_key{ false };									//�L�[
			MATRIX_ref											m_MouseVec;										//�}�E�X�G�C���p�ϐ��m��
			float												m_AimingDistance{ 500.f*Scale_Rate };			//
			bool												m_view_override{ false };						//
			std::vector<VECTOR_ref>								m_view_rad;										//
			float												m_range{ 6.0f };								//
			float												m_range_r{ this->m_range };						//
			float												m_range_change{ this->m_range / 10.f };			//
			float												m_ratio{ 1.f };									//
			bool												m_changeview{ false };							//
			//�v���C���[����
			CharaTypeID											m_CharaType{ CharaTypeID::Enemy };				//
			//�ԑ�
			const VehDataControl::VhehicleData*									m_VecData{ nullptr };							//�ŗL�l
			std::vector<Guns>									m_Gun;											//
			float												m_speed{ 0.f };									//�ړ����x
			float												m_spd_rec{ 0.f };								//
			float												m_speed_add{ 0.f };								//
			float												m_speed_sub{ 0.f };								//
			VECTOR_ref											m_add_vec_real;									//�ړ��x�N�g���o�b�t�@
			VECTOR_ref											m_BodyNormal{ VECTOR_ref::up() };				//
			VECTOR_ref											m_Tilt;											//
			VECTOR_ref											m_radAdd;										//
			float												m_yradadd_left{ 0.f };							//
			float												m_yradadd_right{ 0.f };							//
			float												m_engine_time{ 0.f };							//�G���W������
			HIT_ACTIVE											m_Hit_active;									//
			std::vector<HitSortInfo> hitssort;																	//�t���[���ɓ�����������
			std::vector<MV1_COLL_RESULT_POLY> hitres;															//�m��
			bool												m_PosBufOverRideFlag{ false };					//
			VECTOR_ref											m_PosBufOverRide;								//
			VECTOR_ref											m_VecBufOverRide;								//
			VECTOR_ref											m_RadOverRide;									//
			//�]��
			float												m_wheel_Left{ 0.f };							//�]�։�]
			float												m_wheel_Right{ 0.f };							//�]�։�]
			std::vector<float>									m_wheel_frameYpos{ 0.f };						//�]�ւ�Y�����ێ�
			//�_���[�W
			HitPoint											m_HP{ 100 };
			std::vector<HitPoint>								m_HP_parts;
			DamageEvent											m_DamageEvent;									//
			unsigned long long									m_DamageSwitch{ 0 };							//
			unsigned long long									m_DamageSwitchRec{ 0 };							//
			//box2d
			b2Pats												m_b2mine;										//BOX2D
			float												m_spd_buf{ 0.f };								//BOX2D
			std::array<FootWorld, 2>							m_b2Foot;										//����BOX2D
		public:			//setter,getter
			const auto		SetDamageEvent(const DamageEvent& value) noexcept {
				if (this->m_MyID == value.ID && this->m_objType == value.CharaType) {
					SubHP(value.Damage, value.rad);
					return true;
				}
				return false;
			}
			void			SetCharaType(CharaTypeID value) noexcept { this->m_CharaType = value; }
			void			SetDamageSwitchRec(unsigned long long value) noexcept { this->m_DamageSwitchRec = value; }
			void			SetAimingDistance(float value) noexcept { this->m_AimingDistance = value; }
			void			SubHP(HitPoint damage_t, float)  noexcept { this->m_HP = std::clamp<HitPoint>(this->m_HP - damage_t, 0, this->m_VecData->GetMaxHP()); }
			void			SubHP_Parts(HitPoint damage_t, int parts_Set_t) noexcept { this->m_HP_parts[parts_Set_t] = std::max<HitPoint>(this->m_HP_parts[parts_Set_t] - damage_t, 0); }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_DamageEvent; }
			const auto&		GetDamageSwitch(void) const noexcept { return this->m_DamageSwitch; }
			const auto&		GetDamageSwitchRec(void) const noexcept { return this->m_DamageSwitchRec; }
			const auto&		GetHP(void) const noexcept { return this->m_HP; }
			const auto&		Get_HP_parts(void) const noexcept { return this->m_HP_parts; }
			const auto&		GetHPMax(void) const noexcept { return this->m_VecData->GetMaxHP(); }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto&		GetName(void) const noexcept { return this->m_VecData->GetName(); }
			const auto&		GetLookVec(void) const noexcept { return this->m_MouseVec; }
			const auto&		Getvec_real(void) const noexcept { return this->m_add_vec_real; }
			const auto		Get_pseed_per(void) const noexcept { return this->m_add_vec_real.size() / (this->m_VecData->GetMaxFrontSpeed() / 3.6f); }			//�ړ����x�̃p�[�Z���e�[�W
			const auto&		Gunloadtime(size_t id_t) const noexcept { return this->m_Gun[id_t].Getloadtime(); }
			const auto&		Gunround(size_t id_t) const noexcept { return this->m_Gun[id_t].Getrounds(); }
			const auto&		Get_Gunsize(void) const noexcept { return this->m_Gun.size(); }
			const auto&		Get_changeview(void) const noexcept { return this->m_changeview; }																	//�Ə��ύX��
			const auto&		GetViewRad(void) const noexcept { return this->m_view_rad[0]; }
			const auto&		Get_ratio(void) const noexcept { return this->m_ratio; }																			//UI�p
			const auto&		GetAimingDistance(void) const noexcept { return this->m_AimingDistance; }
			const auto		Get_alive(void) const noexcept { return this->m_HP != 0; }																			//�����Ă��邩
			const auto		Get_body_yrad(void) const noexcept { auto pp = this->m_move.mat.zvec()*-1.f; return std::atan2f(pp.x(), pp.z()); }
			const auto		is_ADS(void) const noexcept { return this->m_range == 0.f; }																		//ADS��
			const auto		GetGunMuzzleMatrix(int ID) const noexcept { return GetObj_const().GetFrameLocalWorldMatrix(this->m_Gun[ID].GetGunTrunnionFrameID()).GetRot(); }
			const auto		GetGunMuzzleBase(int ID) const noexcept { return GetObj_const().frame(this->m_Gun[ID].GetGunTrunnionFrameID()); }
			const auto		GetGunMuzzlePos(int ID) const noexcept { return GetObj_const().frame(this->m_Gun[ID].GetGunMuzzleFrameID()); }
			const auto		GetGunMuzzleVec(int ID) const noexcept { return (GetGunMuzzlePos(ID) - GetGunMuzzleBase(ID)).Norm(); }

			const auto		GetSquarePos(int ID) const noexcept { return GetObj_const().frame(this->m_VecData->Get_square(ID)); }


			const auto		Get_EyePos_Base(void) const noexcept { return (is_ADS()) ? GetGunMuzzleBase(0) : (this->m_move.pos + (this->m_move.mat.yvec() * 3.f * Scale_Rate)); }
			const auto		Set_MidPos(void) noexcept { return (this->m_move.pos + (this->m_move.mat.yvec() * 1.5f * Scale_Rate)); }							//HP�o�[��\������ꏊ
			//
			void			ReSet_range(void) noexcept { this->m_range = 6.f; }
			void			SetPosBufOverRide(const VECTOR_ref& pos_t, const VECTOR_ref& pVec, const VECTOR_ref& rad) noexcept {
				this->m_PosBufOverRideFlag = true;
				this->m_PosBufOverRide = pos_t;
				this->m_VecBufOverRide = pVec;
				this->m_RadOverRide = rad;
			}
		public:
			void			ValueInit(const VehDataControl::VhehicleData* pVeh_data, const MV1& hit_pic, const std::shared_ptr<b2World>& pB2World, PlayerID pID) noexcept;
			void			ValueSet(float pxRad, float pyRad, const VECTOR_ref& pos_t) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady, bool isOverrideView) noexcept;													//
			void			Setcamera(Camera3DInfo& m_MainCamera, const float fov_base) noexcept;																	//�J�����ݒ�o��
			const std::pair<bool, bool>		CheckAmmoHit(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept;
			void			HitGround(const VECTOR_ref& pos_t, const VECTOR_ref& pNorm, const VECTOR_ref& pVec) noexcept;
			void			DrawModuleView(int xp, int yp, int size) noexcept;																					//��e�`�F�b�N
		private://�X�V�֘A
			const auto		CheckAmmoHited(const AmmoClass& pAmmo) noexcept;																					//��e�`�F�b�N
			const auto		CalcAmmoHited(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept;															//��e����

			void			ExecuteSavePrev(void) noexcept;																										//�ȑO�̏�ԕێ�
			void			ExecuteElse(void) noexcept;																											//���̑�
			void			ExecuteInput(void) noexcept;																										//����
			void			ExecuteFrame(void) noexcept;																										//�t���[������
			void			ExecuteMove(void) noexcept;																											//�ړ�����
			void			ExecuteMatrix(void) noexcept;																										//SetMat�w���X�V
			void			DrawCommon(void) noexcept;
		public: //�R���X�g���N�^�A�f�X�g���N�^
			VehicleClass(void) noexcept { this->m_objType = ObjType::Vehicle; }
			~VehicleClass(void) noexcept {}
		public: //�p��
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				this->m_IsDraw = true;
			}
			void			FirstExecute(void) noexcept override {
				//����̂ݍX�V������e
				if (this->m_IsFirstLoop) {
					this->m_view_override = false;
				}
				ExecuteSavePrev();			//�ȑO�̏�ԕێ�
				ExecuteElse();				//���̑�
				ExecuteInput();				//����
				ExecuteFrame();				//�t���[������
				ExecuteMove();				//�ړ�����
				this->m_PosBufOverRideFlag = false;
			}
			void			LateExecute(void) noexcept override {
				ExecuteMatrix();			//SetMat�w��//0.03ms
				EffectControl::Execute();
			}
			void			DrawShadow(void) noexcept override {
				if (!is_ADS()) {
					GetObj().DrawModel();
				}
			}
			void			Draw(void) noexcept override {
				int fog_enable;
				int fog_mode;
				int fog_r, fog_g, fog_b;
				float fog_start, fog_end;
				float fog_density;

				fog_enable = GetFogEnable();													// �t�H�O���L�����ǂ������擾����( TRUE:�L��  FALSE:���� )
				fog_mode = GetFogMode();														// �t�H�O���[�h���擾����
				GetFogColor(&fog_r, &fog_g, &fog_b);											// �t�H�O�J���[���擾����
				GetFogStartEnd(&fog_start, &fog_end);											// �t�H�O���n�܂鋗���ƏI�����鋗�����擾����( 0.0f �` 1.0f )
				fog_density = GetFogDensity();													// �t�H�O�̖��x���擾����( 0.0f �` 1.0f )

				SetFogEnable(TRUE);
				SetFogColor(64, 64, 64);
				SetFogStartEnd(Scale_Rate*50.f, Scale_Rate*300.f);

				DrawCommon();

				SetFogEnable(fog_enable);
				SetFogMode(fog_mode);
				SetFogColor(fog_r, fog_g, fog_b);
				SetFogStartEnd(fog_start, fog_end);
				SetFogDensity(fog_density);
			}
			void			Dispose(void) noexcept override {
				ObjectBaseClass::Dispose();
				EffectControl::Dispose();
				for (auto& f : this->m_b2Foot) {
					f.Dispose();
				}
				//this->m_b2mine.Dispose();
				this->m_wheel_frameYpos.clear();
				for (auto& cg : this->m_Gun) {
					cg.Dispose();
				}
				this->m_Gun.clear();
				this->m_Hit_active.Dispose();
				this->m_HP = 0;
				this->m_HP_parts.clear();
				this->hitres.clear();
				this->hitssort.clear();
			}
		};
	};
};
