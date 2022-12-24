#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//�ÓI�e��
		//HIT_PASSIVE hit_obj_p;
		//��ԖC�f�[�^
		class GunData {
		private:
			std::string							m_name;
			float								m_loadTime{ 0.f };
			int									m_AmmoCapacity = 0;
			int									m_Shot_Sound = -1;
			int									m_Eject_Sound = -1;
			int									m_Reload_Sound = -1;
			float								m_UpRadLimit{ 0.f };
			float								m_DownRadLimit{ 0.f };
			std::vector<AmmoData>				m_AmmoSpec;
			std::array<frames, 3>				m_frame;
		public:
			const auto&		GetName(void) const noexcept { return this->m_name; }
			const auto&		GetLoadTime(void) const noexcept { return this->m_loadTime; }
			const auto&		GetAmmoCapacity(void) const noexcept { return this->m_AmmoCapacity; }
			const auto&		GetShotSound(void) const noexcept { return this->m_Shot_Sound; }
			const auto&		GetEjectSound(void) const noexcept { return this->m_Eject_Sound; }
			const auto&		GetReloadSound(void) const noexcept { return this->m_Reload_Sound; }
			const auto&		GetAmmoSpec(void) const noexcept { return this->m_AmmoSpec; }
			const auto&		Get_frame(size_t pID)const noexcept { return this->m_frame[pID]; }
			const auto&		GetUpRadLimit(void) const noexcept { return this->m_UpRadLimit; }
			const auto&		GetDownRadLimit(void) const noexcept { return this->m_DownRadLimit; }
		public: //�R���X�g���N�^�A�f�X�g���N�^
			GunData(void) noexcept { }
			~GunData(void) noexcept { }
		public:
			void			SetFrame(const MV1& obj, int i) noexcept {
				this->m_frame[0].Set(i, obj);
				if (obj.frame_child_num(this->m_frame[0].GetFrameID()) <= 0) {
					return;
				}
				int child_num = (int)obj.frame_child(this->m_frame[0].GetFrameID(), 0);

				if (obj.frame_name(child_num).find("�p", 0) != std::string::npos) {
					this->m_frame[1].Set(child_num, obj);
				}
				if (this->m_frame[1].GetFrameID() == -1) {
					return;
				}

				if (obj.frame_child_num(this->m_frame[1].GetFrameID()) <= 0) {
					return;
				}
				int child2_num = (int)obj.frame_child(this->m_frame[1].GetFrameID(), 0);
				{
					this->m_frame[2].Set(child2_num, obj);
				}
			}
			void			Set(int mdata, const std::string& stt) noexcept {
				this->m_name = getparams::getright(stt);
				this->m_loadTime = getparams::_float(mdata);
				this->m_AmmoCapacity = getparams::_int(mdata);
				this->m_Shot_Sound = getparams::_int(mdata);//�T�E���h
				this->m_Eject_Sound = getparams::_int(mdata);//�T�E���h
				this->m_Reload_Sound = getparams::_int(mdata);//�T�E���h
				this->m_UpRadLimit = getparams::_float(mdata);
				this->m_DownRadLimit = getparams::_float(mdata);
				while (true) {
					auto stp = getparams::Getstr(mdata);
					if (stp.find("useammo" + std::to_string(this->m_AmmoSpec.size())) == std::string::npos) {
						break;
					}
					this->m_AmmoSpec.resize(this->m_AmmoSpec.size() + 1);
					this->m_AmmoSpec.back().Set("data/ammo/", getparams::getright(stp));
				}
			}
		};
		//��ԃf�[�^
		class VehDataControl {
		public:
			class VhehicleData {
				typedef std::pair<std::shared_ptr<GraphHandle>, int> ViewAndModule;
			private:
				std::string							m_name;							//����
				HitPoint							m_MaxHP{ 0 };					//HP
				bool								m_IsFloat{ false };				//�������ǂ���
				float								m_DownInWater{ 0.f };			//���ޔ���ӏ�
				float								m_MaxFrontSpeed{ 0.f };			//�O�i���x(km/h)
				float								m_MaxBackSpeed{ 0.f };			//��ޑ��x(km/h)
				float								m_MaxBodyRad{ 0.f };			//���񑬓x(�x/�b)
				float								m_MaxTurretRad{ 0.f };			//�C���쓮���x(�x/�b)
				std::vector<GunData>				m_GunFrameData;					//
				b2PolygonShape						m_DynamicBox;					//
				MV1									m_DataObj;						//
				MV1									m_DataCol;						//
				std::vector<frames>					m_wheelframe;					//�]��
				std::vector<frames>					m_wheelframe_nospring;			//�U���։�]
				std::vector<std::pair<int, float>>	m_armer_mesh;					//���bID
				std::vector<int>					m_space_mesh;					//���bID
				std::vector<int>					m_module_mesh;					//���bID
				std::array<int, 4>					m_square{ 0 };					//���p�̎l��
				std::array<std::vector<frames>, 2>	m_b2upsideframe;				//���я�
				std::array<std::vector<frames>, 2>	m_downsideframe;				//����
				std::array<std::vector<ViewAndModule>, 2>	m_TankViewPic;				//���W���[���\��
			private:
				const auto		GetSide(bool isLeft, bool isFront) const noexcept {
					int ans = 0;
					float tmp = 0.f;
					for (auto& f : this->m_wheelframe) {
						if ((isLeft ? 1.f : -1.f) * f.GetFrameWorldPosition().x() >= 0) {
							ans = f.GetFrameID();
							tmp = f.GetFrameWorldPosition().z();
							break;
						}
					}
					for (auto& f : this->m_wheelframe) {
						if (ans != f.GetFrameID()) {
							if ((isLeft ? 1.f : -1.f) * f.GetFrameWorldPosition().x() >= 0) {
								if (isFront) {
									if (tmp > f.GetFrameWorldPosition().z()) {
										ans = f.GetFrameID();
										tmp = f.GetFrameWorldPosition().z();
									}
								}
								else {
									if (tmp < f.GetFrameWorldPosition().z()) {
										ans = f.GetFrameID();
										tmp = f.GetFrameWorldPosition().z();
									}
								}
							}
						}
					}
					return ans;
				}
			public:			//getter
				const auto&		GetName(void) const noexcept { return this->m_name; }
				const auto&		GetMaxHP(void) const noexcept { return this->m_MaxHP; }
				const auto&		GetIsFloat(void) const noexcept { return this->m_IsFloat; }
				const auto&		GetDownInWater(void) const noexcept { return this->m_DownInWater; }
				const auto&		GetMaxFrontSpeed(void) const noexcept { return this->m_MaxFrontSpeed; }
				const auto&		GetMaxBackSpeed(void) const noexcept { return this->m_MaxBackSpeed; }
				const auto&		GetMaxBodyRad(void) const noexcept { return this->m_MaxBodyRad; }
				const auto&		GetMaxTurretRad(void) const noexcept { return this->m_MaxTurretRad; }
				const auto&		Get_gunframe(void) const noexcept { return this->m_GunFrameData; }
				const auto&		GetDynamicBox(void) const noexcept { return this->m_DynamicBox; }
				const auto&		Get_wheelframe(void) const noexcept { return this->m_wheelframe; }
				const auto&		Get_wheelframe_nospring(void) const noexcept { return this->m_wheelframe_nospring; }
				const auto&		Get_armer_mesh(void) const noexcept { return this->m_armer_mesh; }
				const auto&		Get_space_mesh(void) const noexcept { return this->m_space_mesh; }
				const auto&		Get_module_mesh(void) const noexcept { return this->m_module_mesh; }
				const auto&		Get_module_view(void) const noexcept { return this->m_TankViewPic; }
				const auto&		Get_square(size_t ID_t)const noexcept { return this->m_square[ID_t]; }
				const auto&		Get_square(void) const noexcept { return this->m_square; }
				const auto&		Get_b2upsideframe(size_t ID_t)const noexcept { return this->m_b2upsideframe[ID_t]; }
				const auto&		Get_b2upsideframe(void) const noexcept { return this->m_b2upsideframe; }
				const auto&		Get_b2downsideframe(void) const noexcept { return this->m_downsideframe; }
			public: //�R���X�g���N�^�A�f�X�g���N�^
				VhehicleData(void) noexcept { }
				VhehicleData(const VhehicleData& o) noexcept {
					this->m_name = o.m_name;
					this->m_MaxHP = o.m_MaxHP;
					this->m_IsFloat = o.m_IsFloat;
					this->m_DownInWater = o.m_DownInWater;
					this->m_MaxFrontSpeed = o.m_MaxFrontSpeed;
					this->m_MaxBackSpeed = o.m_MaxBackSpeed;
					this->m_MaxBodyRad = o.m_MaxBodyRad;
					this->m_MaxTurretRad = o.m_MaxTurretRad;
					this->m_GunFrameData = o.m_GunFrameData;
					this->m_DynamicBox = o.m_DynamicBox;
					this->m_wheelframe = o.m_wheelframe;
					this->m_wheelframe_nospring = o.m_wheelframe_nospring;
					this->m_armer_mesh = o.m_armer_mesh;
					this->m_space_mesh = o.m_space_mesh;
					this->m_module_mesh = o.m_module_mesh;
					this->m_square = o.m_square;
					this->m_b2upsideframe = o.m_b2upsideframe;
					this->m_downsideframe = o.m_downsideframe;
					this->m_TankViewPic = o.m_TankViewPic;
				}
				~VhehicleData(void) noexcept { }
			public:
				//���O�ǂݍ���
				void		Set_Pre(const char* name) noexcept {
					this->m_name = name;
					MV1::Load("data/tank/" + this->m_name + "/model_LOADCALC.mv1", &this->m_DataObj);//model.pmx//model_LOADCALC.mv1
					MV1::Load("data/tank/" + this->m_name + "/col.mv1", &this->m_DataCol);
				}
				//���C���ǂݍ���
				void			Set(void) noexcept {
					//�ŗL
					this->m_DownInWater = 0.f;
					for (int i = 0; i < this->m_DataObj.mesh_num(); i++) {
						auto p = this->m_DataObj.mesh_maxpos(i).y();
						if (this->m_DownInWater < p) {
							this->m_DownInWater = p;
						}
					}
					this->m_DownInWater /= 2.f;
					VECTOR_ref minpos, maxpos;							//
					for (int i = 0; i < this->m_DataObj.frame_num(); i++) {
						std::string p = this->m_DataObj.frame_name(i);
						if (p.find("�]��", 0) != std::string::npos) {
							this->m_wheelframe.resize(this->m_wheelframe.size() + 1);
							this->m_wheelframe.back().Set(i, this->m_DataObj);
						}
						else if ((p.find("��", 0) != std::string::npos) && (p.find("�]��", 0) == std::string::npos)) {
							this->m_wheelframe_nospring.resize(this->m_wheelframe_nospring.size() + 1);
							this->m_wheelframe_nospring.back().Set(i, this->m_DataObj);
						}
						else if (p.find("����", 0) != std::string::npos) {
							this->m_GunFrameData.resize(this->m_GunFrameData.size() + 1);
							this->m_GunFrameData.back().SetFrame(this->m_DataObj, i);
						}
						else if (p == "min") {
							minpos = this->m_DataObj.frame(i);
						}
						else if (p == "max") {
							maxpos = this->m_DataObj.frame(i);
						}
						else if (p.find("�QD����", 0) != std::string::npos || p.find("2D����", 0) != std::string::npos) { //2D����
							this->m_b2upsideframe[0].clear();
							this->m_b2upsideframe[1].clear();
							for (int z = 0; z < this->m_DataObj.frame_child_num(i); z++) {
								if (this->m_DataObj.frame(i + 1 + z).x() > 0) {
									this->m_b2upsideframe[0].resize(this->m_b2upsideframe[0].size() + 1);
									this->m_b2upsideframe[0].back().Set(i + 1 + z, this->m_DataObj);
								}
								else {
									this->m_b2upsideframe[1].resize(this->m_b2upsideframe[1].size() + 1);
									this->m_b2upsideframe[1].back().Set(i + 1 + z, this->m_DataObj);
								}
							}
							std::sort(this->m_b2upsideframe[0].begin(), this->m_b2upsideframe[0].end(), [](const frames& x, const frames& y) { return x.GetFrameWorldPosition().z() < y.GetFrameWorldPosition().z(); }); //�\�[�g
							std::sort(this->m_b2upsideframe[1].begin(), this->m_b2upsideframe[1].end(), [](const frames& x, const frames& y) { return x.GetFrameWorldPosition().z() < y.GetFrameWorldPosition().z(); }); //�\�[�g
						}
						else if (p.find("���ѐݒu��", 0) != std::string::npos) { //2D����
							this->m_downsideframe[0].clear();
							this->m_downsideframe[1].clear();
							for (int z = 0; z < this->m_DataObj.frame_child_num(i); z++) {
								if (this->m_DataObj.frame(i + 1 + z).x() > 0) {
									this->m_downsideframe[0].resize(this->m_downsideframe[0].size() + 1);
									this->m_downsideframe[0].back().Set(i + 1 + z, this->m_DataObj);
								}
								else {
									this->m_downsideframe[1].resize(this->m_downsideframe[1].size() + 1);
									this->m_downsideframe[1].back().Set(i + 1 + z, this->m_DataObj);
								}
							}
						}
					}
					//
					VECTOR_ref minmaxsub = maxpos - minpos;
					VECTOR_ref minmaxadd = minpos + maxpos;
					this->m_DynamicBox.SetAsBox(minmaxsub.x() / 2, minmaxsub.z() / 2, b2Vec2(minmaxadd.x() / 2, -minmaxadd.z() / 2), 0.f);	//�_�C�i�~�b�N�{�f�B�ɕʂ̃{�b�N�X�V�F�C�v���`
					//4���m��
					this->m_square[0] = GetSide(true, false);			//2		���㕔0
					this->m_square[1] = GetSide(true, true);			//10	���O��1
					this->m_square[2] = GetSide(false, false);			//3		�E�㕔2
					this->m_square[3] = GetSide(false, true);			//11	�E�O��3
					//���b

					//�ԑ̃��W���[��
					this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_body.png")), -1));
					int human_body_num = 0;
					int ammo_body_num = 0;
					for (int i = 0; i < this->m_DataCol.mesh_num(); i++) {
						std::string p = this->m_DataCol.material_name(i);
						if (p.find("armer", 0) != std::string::npos) {
							this->m_armer_mesh.emplace_back(i, std::stof(getparams::getright(p.c_str())));//���b
						}
						else if (p.find("space", 0) != std::string::npos) {
							this->m_space_mesh.emplace_back(i);//��ԑ��b
						}
						else if (p.find("left_foot", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//���W���[��
							this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_track_l.bmp")), (int)this->m_module_mesh.size() - 1));
						}
						else if (p.find("right_foot", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//���W���[��
							this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_track_r.bmp")), (int)this->m_module_mesh.size() - 1));
						}
						else if (p.find("engine", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//���W���[��
							this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_engine_body.bmp")), (int)this->m_module_mesh.size() - 1));
						}
						else if (p.find("human_body", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//���W���[��
							this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_human_body_" + std::to_string(human_body_num) + ".bmp")), (int)this->m_module_mesh.size() - 1));
							human_body_num++;
						}
						else if (p.find("ammo_body", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//���W���[��
							this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_ammo_body_" + std::to_string(ammo_body_num) + ".bmp")), (int)this->m_module_mesh.size() - 1));
							ammo_body_num++;
						}
					}
					this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_look_body.bmp")), 0));
					//�C�����W���[��
					this->m_TankViewPic[1].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_turret.png")), -1));
					int human_turret_num = 0;
					int ammo_turret_num = 0;
					for (int i = 0; i < this->m_DataCol.mesh_num(); i++) {
						std::string p = this->m_DataCol.material_name(i);
						if (p.find("gun", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//���W���[��
							this->m_TankViewPic[1].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_gun.bmp")), (int)this->m_module_mesh.size() - 1));
						}
						else if (p.find("human_turret", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//���W���[��
							this->m_TankViewPic[1].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_human_turret_" + std::to_string(human_turret_num) + ".bmp")), (int)this->m_module_mesh.size() - 1));
							human_turret_num++;
						}
						else if (p.find("ammo_turret", 0) != std::string::npos) {
							this->m_module_mesh.emplace_back(i);//���W���[��
							this->m_TankViewPic[1].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_ammo_turret_" + std::to_string(ammo_turret_num) + ".bmp")), (int)this->m_module_mesh.size() - 1));
							ammo_turret_num++;
						}
					}
					this->m_TankViewPic[1].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_look_turret.bmp")), 0));
					//data
					{
						int mdata = FileRead_open(("data/tank/" + this->m_name + "/data.txt").c_str(), FALSE);
						this->m_MaxHP = (HitPoint)getparams::_int(mdata);
						this->m_IsFloat = getparams::_bool(mdata);
						this->m_MaxFrontSpeed = getparams::_float(mdata);
						this->m_MaxBackSpeed = getparams::_float(mdata);
						this->m_MaxBodyRad = getparams::_float(mdata);
						this->m_MaxTurretRad = deg2rad(getparams::_float(mdata));
						auto stt = getparams::_str(mdata);
						for (auto& g : this->m_GunFrameData) { g.Set(mdata, stt); }
						FileRead_close(mdata);
					}
					this->m_DataObj.Dispose();
					this->m_DataCol.Dispose();
				}
			};
		private:
			std::vector<VhehicleData>	vehc_data;
		public:
			const auto&		GetVehData(void) const noexcept { return this->vehc_data; }
		public:
			void	Load() noexcept {
				auto data_t = GetFileNamesInDirectory("data/tank/");
				std::vector<std::string> DirNames;
				for (auto& d : data_t) {
					if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						DirNames.emplace_back(d.cFileName);
					}
				}
				this->vehc_data.resize(DirNames.size());
				for (auto& d : DirNames) {
					this->vehc_data[&d - &DirNames.front()].Set_Pre(d.c_str());
				}
				DirNames.clear();
			}
			void	Set() noexcept {
				for (auto& t : this->vehc_data) {
					t.Set();
				}
			}
			void	Dispose() noexcept {
				this->vehc_data.clear();
			}
		};
		//�C
		class Guns {
		private:
			float							m_loadtimer{ 0 };			//���Ă�J�E���^�[
			bool							m_reloadSEFlag{ true };		//
			float							m_Recoil{ 0.f };			//����
			float							m_React{ 0.f };				//����
			int								m_rounds{ 0 };				//�e��
			const GunData*					m_GunSpec{ nullptr };		//
			std::vector<const AmmoData*>	m_AmmoSpec{ nullptr };		//
			VECTOR_ref						m_ShotRadAdd;				//
		public:			//getter
			const auto&	Getrounds(void) const noexcept { return m_rounds; }
			const auto&	Getloadtime(void) const noexcept { return m_loadtimer; }
			const auto&	GetAmmoSpec(void) const noexcept { return this->m_AmmoSpec[0]; }
			const auto&	GetCaliberSize(void) const noexcept { return GetAmmoSpec()->GetCaliber(); }
			const auto&	GetGunTrunnionFrameID(void) const noexcept { return this->m_GunSpec->Get_frame(1).GetFrameID(); }
			const auto&	GetGunMuzzleFrameID(void) const noexcept { return this->m_GunSpec->Get_frame(2).GetFrameID(); }
			const auto&	GetShotSound(void) const noexcept { return this->m_GunSpec->GetShotSound(); }
			const auto&	GetReloadSound(void) const noexcept { return this->m_GunSpec->GetReloadSound(); }
			const auto&	GetEjectSound(void) const noexcept { return this->m_GunSpec->GetEjectSound(); }
			//�e����
			void		FireReaction(MATRIX_ref* mat_t) const noexcept {
				(*mat_t) *= MATRIX_ref::RotAxis(mat_t->xvec(), this->m_ShotRadAdd.x()) * MATRIX_ref::RotAxis(mat_t->zvec(), this->m_ShotRadAdd.z());
			}
		public:
			//�p�x�w��
			void		ExecuteGunFrame(float pGunXrad, float pGunYrad, MV1* obj_body_t, MV1* col_body_t) noexcept {
				float yrad = pGunYrad;
				float xrad = std::clamp(pGunXrad, deg2rad(this->m_GunSpec->GetDownRadLimit()), deg2rad(this->m_GunSpec->GetUpRadLimit()));
				frames id;
				id = this->m_GunSpec->Get_frame(0);
				if (id.GetFrameID() >= 0) {
					obj_body_t->SetFrameLocalMatrix(id.GetFrameID(), MATRIX_ref::RotY(yrad) * MATRIX_ref::Mtrans(id.GetFrameLocalPosition()));
					col_body_t->SetFrameLocalMatrix(id.GetFrameID(), MATRIX_ref::RotY(yrad) * MATRIX_ref::Mtrans(id.GetFrameLocalPosition()));
				}
				id = this->m_GunSpec->Get_frame(1);
				if (id.GetFrameID() >= 0) {
					obj_body_t->SetFrameLocalMatrix(id.GetFrameID(), MATRIX_ref::RotX(xrad) * MATRIX_ref::Mtrans(id.GetFrameLocalPosition()));
					col_body_t->SetFrameLocalMatrix(id.GetFrameID(), MATRIX_ref::RotX(xrad) * MATRIX_ref::Mtrans(id.GetFrameLocalPosition()));
				}
				id = this->m_GunSpec->Get_frame(2);
				if (id.GetFrameID() >= 0) {
					obj_body_t->SetFrameLocalMatrix(id.GetFrameID(), MATRIX_ref::Mtrans(VECTOR_ref::front() * (this->m_Recoil * 0.5f * Scale_Rate)) * MATRIX_ref::Mtrans(id.GetFrameLocalPosition()));
					col_body_t->SetFrameLocalMatrix(id.GetFrameID(), MATRIX_ref::Mtrans(VECTOR_ref::front() * (this->m_Recoil * 0.5f * Scale_Rate)) * MATRIX_ref::Mtrans(id.GetFrameLocalPosition()));
				}
				Easing(&this->m_ShotRadAdd, MATRIX_ref::RotY(yrad).xvec() * -1.f * deg2rad(-this->m_React * GetAmmoSpec()->GetCaliber() * 50.f), 0.85f, EasingType::OutExpo);
			}
		public: //�R���X�g���N�^�A�f�X�g���N�^
			Guns(void) noexcept { }
			Guns(const Guns&) noexcept { }
		public:
			void		Init(const GunData* pResorce) noexcept {
				this->m_GunSpec = pResorce;
				this->m_rounds = this->m_GunSpec->GetAmmoCapacity();
				this->m_AmmoSpec.clear();
				for (auto& s : this->m_GunSpec->GetAmmoSpec()) {
					this->m_AmmoSpec.emplace_back(&s);
				}
				this->m_ShotRadAdd.Set(0, 0, 0);
			}
			bool		Execute(bool key, bool playSound) noexcept {
				auto* SE = SoundPool::Instance();
				bool isshot = (key && this->m_loadtimer == 0 && this->m_rounds > 0);
				//�ˌ�
				if (isshot) {
					this->m_loadtimer = this->m_GunSpec->GetLoadTime();
					this->m_rounds = std::max<int>(this->m_rounds - 1, 0);
					this->m_Recoil = 1.f;
					this->m_React = std::clamp(this->m_React + GetAmmoSpec()->GetCaliber() * 10.f, 0.f, 3.f);
					this->m_reloadSEFlag = true;
					if (playSound) { SE->Get((int)SoundEnum::Tank_Eject).Play(GetEjectSound()); }
				}
				if (this->m_reloadSEFlag && this->m_loadtimer < 1.f) {
					this->m_reloadSEFlag = false;
					if (playSound) { SE->Get((int)SoundEnum::Tank_Reload).Play(GetReloadSound()); }
				}
				this->m_loadtimer = std::max(this->m_loadtimer - 1.f / FPS, 0.f);
				this->m_Recoil = std::max(this->m_Recoil - 1.f / FPS, 0.f);
				this->m_React = std::max(this->m_React - 1.f / FPS, 0.f);
				return isshot;
			}
			void		Dispose(void) noexcept {
				this->m_loadtimer = 0.f;
				this->m_reloadSEFlag = true;
				this->m_Recoil = 0.f;
				this->m_React = 0.f;
				this->m_rounds = 0;
				this->m_GunSpec = nullptr;
				this->m_AmmoSpec.clear();
				this->m_ShotRadAdd.clear();
			}
		};
		//�e��
		class HIT_ACTIVE {
			struct Hit {
				bool		m_IsActive{ false };	//�e���t���O
				int			m_UseFrame{ 0 };		//�g�p�t���[��
				MV1			m_obj;					//�e�����f��
				moves		m_move;					//���W
			};
			std::array<Hit, 24> hit_obj;			//�e��
			int hitbuf = 0;							//�g�p�e��
		public:
			void			Set(const moves& this_move, const  VECTOR_ref& Put_pos, const  VECTOR_ref& Put_normal, const VECTOR_ref& ammo_nomal/*this->move.vec.Norm()*/, const float&caliber, bool isPene) noexcept {
				float asize = 200.f*caliber;
				auto y_vec = MATRIX_ref::Vtrans(Put_normal, this_move.mat.Inverse());
				auto z_vec = MATRIX_ref::Vtrans(Put_normal.cross(ammo_nomal).Norm(), this_move.mat.Inverse());
				auto scale = VECTOR_ref::vget(asize / std::abs(ammo_nomal.dot(Put_normal)), asize, asize);

				this->hit_obj[this->hitbuf].m_UseFrame = (isPene) ? 0 : 1;				//�e��
				this->hit_obj[this->hitbuf].m_move.mat = MATRIX_ref::GetScale(scale)* MATRIX_ref::Axis1_YZ(y_vec, z_vec);
				this->hit_obj[this->hitbuf].m_move.pos = MATRIX_ref::Vtrans(Put_pos - this_move.pos, this_move.mat.Inverse()) + y_vec * 0.005f;
				this->hit_obj[this->hitbuf].m_IsActive = true;
				++this->hitbuf %= this->hit_obj.size();
			}
		public:
			void			Init(const MV1& hit_pic) noexcept {
				for (auto& h : this->hit_obj) {
					h.m_IsActive = false;
					h.m_obj = hit_pic.Duplicate();
				}
			}
			void			Execute(const MV1& pObj) noexcept {
				//�e��
				for (auto& h : this->hit_obj) {
					if (h.m_IsActive) {
						auto matrix_tmp = pObj.GetMatrix();
						h.m_obj.SetMatrix(h.m_move.mat* matrix_tmp.GetRot()*MATRIX_ref::Mtrans(matrix_tmp.pos() + MATRIX_ref::Vtrans(h.m_move.pos, matrix_tmp.GetRot())));
					}
				}
			}
			void			Draw(void) noexcept {
				for (auto& h : this->hit_obj) {
					if (h.m_IsActive) {
						h.m_obj.DrawFrame(h.m_UseFrame);
					}
				}
			}
			void			Dispose(void) noexcept {
				for (auto& h : this->hit_obj) {
					h.m_IsActive = false;
					h.m_obj.Dispose();
					h.m_UseFrame = 0;
					h.m_move.mat.clear();
					h.m_move.pos.clear();
				}
			}
		};
		//�����֘A
		class HitSortInfo {
			size_t					m_hitmesh{ SIZE_MAX };
			float					m_hitDistance{ -1 };
		public:
			const auto		operator<(const HitSortInfo& tgt) const noexcept { return this->m_hitDistance < tgt.m_hitDistance; }
			void			Set(size_t f_t, float s_t = (std::numeric_limits<float>::max)())noexcept {
				this->m_hitmesh = f_t;
				this->m_hitDistance = s_t;
			}
			const auto		GetHitMesh(void) const noexcept { return this->m_hitmesh; }
			const auto		IsHit(void) const noexcept { return (this->m_hitDistance != (std::numeric_limits<float>::max)()); }
		};
		//����BOX2D
		class FootWorld {
		private:
			class cat_frame {
			private:
				frames					m_frame;
				float					m_Res_y{ (std::numeric_limits<float>::max)() };
				EffectControl::EffectS	m_gndsmkeffcs;
				float					m_gndsmksize{ 1.f };
			public:
				cat_frame(void) noexcept {}
				//~cat_frame(void) noexcept {}
			public://getter
				const auto&		GetColResult_Y(void) const noexcept { return this->m_Res_y; }
			public:
				void			Init(const frames& pFrame) noexcept {
					this->m_frame = pFrame;
					this->m_gndsmkeffcs.SetLoop(EffectResource::Instance()->effsorce.back(), VECTOR_ref::vget(0, -1, 0), VECTOR_ref::vget(0, 0, 1), 0.1f*Scale_Rate);
					this->m_gndsmksize = 0.1f;
				}
				//
				void			FrameExecute(MV1* pTargetObj, const std::shared_ptr<BackGroundClass>& pBackGround) noexcept {
					if (this->m_frame.GetFrameID() >= 0) {
						auto y_vec = pTargetObj->GetMatrix().yvec();
						pTargetObj->frame_Reset(this->m_frame.GetFrameID());
						auto startpos = pTargetObj->frame(this->m_frame.GetFrameID());
						auto pos_t1 = startpos + y_vec * ((-this->m_frame.GetFrameWorldPosition().y()) + 2.f*Scale_Rate);
						auto pos_t2 = startpos + y_vec * ((-this->m_frame.GetFrameWorldPosition().y()) - 0.3f*Scale_Rate);
						auto ColRes = pBackGround->CheckLinetoMap(pos_t1, &pos_t2, true);
						this->m_Res_y = (ColRes) ? pos_t2.y() : (std::numeric_limits<float>::max)();
						pTargetObj->SetFrameLocalMatrix(this->m_frame.GetFrameID(),
							MATRIX_ref::Mtrans(VECTOR_ref::up() * ((ColRes) ? (this->m_Res_y + y_vec.y() * this->m_frame.GetFrameWorldPosition().y() - startpos.y()) : -0.4f*Scale_Rate)) *
							MATRIX_ref::Mtrans(this->m_frame.GetFrameWorldPosition())
						);
					}
				}
				void			EffectExecute(MV1* pTargetObj, float pSpd) noexcept {
					if (this->m_Res_y != (std::numeric_limits<float>::max)()) {
						Easing(&this->m_gndsmksize, 0.2f + std::abs(pSpd) * 5.5f, 0.975f, EasingType::OutExpo);
					}
					else {
						this->m_gndsmksize = 0.2f;
					}
					this->m_gndsmkeffcs.SetEffectPos(pTargetObj->frame(this->m_frame.GetFrameID()) + pTargetObj->GetMatrix().yvec() * (-this->m_frame.GetFrameWorldPosition().y()));
					this->m_gndsmkeffcs.SetEffectScale(std::clamp(this->m_gndsmksize, 0.2f, 1.5f)*Scale_Rate);
				}
				//
				void			Dispose(void) noexcept {
					this->m_gndsmkeffcs.Dispose();
				}
			};
		private:
			std::shared_ptr<b2World>	m_FootWorld{ nullptr };		//��world
			std::vector<b2Pats>			m_Foot, m_Wheel, m_Yudo;	//
			std::vector<cat_frame>		m_downsideframe;			//����
		public:			//getter
			const auto&			Getdownsideframe(void) const noexcept { return this->m_downsideframe; }
		public:
			void			Init(bool IsLeft, const VehDataControl::VhehicleData* pUseVeh, MV1* pTargetObj) noexcept {
				this->m_FootWorld = std::make_unique<b2World>(b2Vec2(0.0f, 0.0f));
				float LorR = ((IsLeft) ? 1.f : -1.f);
				//����
				this->m_Foot.clear();
				{
					b2BodyDef bd;
					b2Body* BodyB = this->m_FootWorld->CreateBody(&bd);
					b2EdgeShape shape;
					shape.Set(b2Vec2(-40.0f, -10.0f), b2Vec2(40.0f, -10.0f));
					BodyB->CreateFixture(&shape, 0.0f);
					b2Body* BodyA = BodyB;
					b2Vec2 anchor;
					for (const auto& w : pUseVeh->Get_b2upsideframe(IsLeft ? 0 : 1)) {
						anchor = b2Vec2(w.GetFrameWorldPosition().z(), w.GetFrameWorldPosition().y());
						if (w.GetFrameWorldPosition().x() * LorR > 0) {
							this->m_Foot.resize(this->m_Foot.size() + 1);
							b2PolygonShape f_dynamicBox; //�_�C�i�~�b�N�{�f�B�ɕʂ̃{�b�N�X�V�F�C�v���`
							f_dynamicBox.SetAsBox(0.2f*Scale_Rate, 0.05f*Scale_Rate);
							this->m_Foot.back().Set(CreateB2Body(this->m_FootWorld, b2_dynamicBody, anchor.x, anchor.y), &f_dynamicBox);
							b2RevoluteJointDef m_jointDef; m_jointDef.Initialize(BodyA, this->m_Foot.back().Get(), anchor);
							this->m_FootWorld->CreateJoint(&m_jointDef);
							BodyA = this->m_Foot.back().Get();
						}
					}
					if (!this->m_Foot.empty()) {
						b2RevoluteJointDef m_jointDef; m_jointDef.Initialize(BodyA, BodyB, anchor);
						this->m_FootWorld->CreateJoint(&m_jointDef);
					}
				}
				{
					const auto& w = pUseVeh->Get_b2downsideframe()[((IsLeft) ? 0 : 1)];
					this->m_downsideframe.resize(w.size());
					for (auto& t : this->m_downsideframe) {
						t.Init(w[&t - &this->m_downsideframe.front()]);
					}
				}
				//�]��
				this->m_Wheel.clear();
				this->m_Yudo.clear();
				if (!this->m_Foot.empty()) {
					//�]��(����)
					for (const auto& w : pUseVeh->Get_wheelframe()) {
						VECTOR_ref vects = pTargetObj->GetFrameLocalMatrix(w.GetFrameID()).pos();
						if (vects.x() * LorR > 0) {
							this->m_Wheel.resize(this->m_Wheel.size() + 1);
							b2CircleShape shape;
							shape.m_radius = vects.y() - 0.1f;
							this->m_Wheel.back().Set(CreateB2Body(this->m_FootWorld, b2_kinematicBody, vects.z(), vects.y()), &shape);
						}
					}
					//�U����(�����Ȃ�)
					for (const auto& w : pUseVeh->Get_wheelframe_nospring()) {
						VECTOR_ref vects = pTargetObj->GetFrameLocalMatrix(w.GetFrameID()).pos();
						if (vects.x() * LorR > 0) {
							this->m_Yudo.resize(this->m_Yudo.size() + 1);
							b2CircleShape shape;
							shape.m_radius = 0.05f*Scale_Rate;
							this->m_Yudo.back().Set(CreateB2Body(this->m_FootWorld, b2_kinematicBody, vects.z(), vects.y()), &shape);
						}
					}
				}
			}
			//
			void			FirstExecute(MV1* pTargetObj, const std::shared_ptr<BackGroundClass>& pBackGround) noexcept {
				for (auto& t : this->m_downsideframe) {
					t.FrameExecute(pTargetObj, pBackGround);
				}
			}
			void			LateExecute(bool IsLeft, const VehDataControl::VhehicleData* pUseVeh, MV1* pTargetObj, const b2Vec2& pGravity, float pWheelRotate, float pSpd) noexcept {
				int LR_t = (IsLeft ? 1 : -1);
				if (this->m_Foot.size() != 0) {
					auto Wheel_t = this->m_Wheel.begin();
					for (const auto& w : pUseVeh->Get_wheelframe()) {
						VECTOR_ref vects = pTargetObj->GetFrameLocalMatrix(w.GetFrameID()).pos();
						if (vects.x() * LR_t > 0) {
							Wheel_t->SetTransform(b2Vec2(vects.z(), vects.y()), pWheelRotate);
							Wheel_t++;
						}
					}
					size_t i = 0;
					for (const auto& w : pUseVeh->Get_wheelframe_nospring()) {
						VECTOR_ref vects = pTargetObj->GetFrameLocalMatrix(w.GetFrameID()).pos();
						if (vects.x() * LR_t > 0) {
							this->m_Yudo[i++].SetTransform(b2Vec2(vects.z(), vects.y()), pWheelRotate);
						}
					}
					for (auto& t : this->m_Foot) {
						t.SetLinearVelocity(pGravity); //
					}

					this->m_FootWorld->Step(0.1f, 3, 3);

					for (auto& t : this->m_Foot) {
						t.pos = VECTOR_ref::vget(t.pos.x(), t.Pos().y, t.Pos().x);
					}
				}
				{
					size_t i = 0;
					for (const auto& w : pUseVeh->Get_b2upsideframe((IsLeft) ? 0 : 1)) {
						if (w.GetFrameWorldPosition().x() * LR_t > 0) {
							auto& t = this->m_Foot[i++];
							t.pos = VECTOR_ref::vget(w.GetFrameWorldPosition().x(), t.pos.y(), t.pos.z());
							pTargetObj->SetFrameLocalMatrix(w.GetFrameID(), MATRIX_ref::Mtrans(t.pos));
						}
					}
				}
				//�G�t�F�N�g�X�V
				for (auto& t : this->m_downsideframe) {
					t.EffectExecute(pTargetObj, pSpd);
				}
			}
			//
			void			Dispose(void) noexcept {
				for (auto& t : this->m_Foot) { t.Dispose(); }
				this->m_Foot.clear();
				for (auto& t : this->m_Wheel) { t.Dispose(); }
				this->m_Wheel.clear();
				for (auto& t : this->m_Yudo) { t.Dispose(); }
				this->m_Yudo.clear();
				this->m_FootWorld.reset();
				for (auto& t : this->m_downsideframe) {
					t.Dispose();
				}
				this->m_downsideframe.clear();
			}
		};
	};
};