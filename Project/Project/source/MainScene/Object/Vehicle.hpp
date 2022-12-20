#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//静的弾痕
		//HIT_PASSIVE hit_obj_p;
		//戦車砲データ
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
		public: //コンストラクタ、デストラクタ
			GunData(void) noexcept { }
			~GunData(void) noexcept { }
		public:
			void			SetFrame(const MV1& obj, int i) noexcept {
				this->m_frame[0].Set(i, obj);
				if (obj.frame_child_num(this->m_frame[0].GetFrameID()) <= 0) {
					return;
				}
				int child_num = (int)obj.frame_child(this->m_frame[0].GetFrameID(), 0);

				if (obj.frame_name(child_num).find("仰角", 0) != std::string::npos) {
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
				this->m_Shot_Sound = getparams::_int(mdata);//サウンド
				this->m_Eject_Sound = getparams::_int(mdata);//サウンド
				this->m_Reload_Sound = getparams::_int(mdata);//サウンド
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
		//戦車データ
		class VhehicleData {
			typedef std::pair<std::shared_ptr<GraphHandle>, int> ViewAndModule;
		private:
			std::string							m_name;							//名称
			HitPoint							m_MaxHP{ 0 };					//HP
			bool								m_IsFloat{ false };				//浮くかどうか
			float								m_DownInWater{ 0.f };			//沈む判定箇所
			float								m_MaxFrontSpeed{ 0.f };			//前進速度(km/h)
			float								m_MaxBackSpeed{ 0.f };			//後退速度(km/h)
			float								m_MaxBodyRad{ 0.f };			//旋回速度(度/秒)
			float								m_MaxTurretRad{ 0.f };			//砲塔駆動速度(度/秒)
			std::vector<GunData>				m_GunFrameData;					//
			b2PolygonShape						m_DynamicBox;					//
			MV1									m_DataObj;						//
			MV1									m_DataCol;						//
			std::vector<frames>					m_wheelframe;					//転輪
			std::vector<frames>					m_wheelframe_nospring;			//誘導輪回転
			std::vector<std::pair<int, float>>	m_armer_mesh;					//装甲ID
			std::vector<int>					m_space_mesh;					//装甲ID
			std::vector<int>					m_module_mesh;					//装甲ID
			std::array<int, 4>					m_square{ 0 };					//車輛の四辺
			std::array<std::vector<frames>, 2>	m_b2upsideframe;				//履帯上
			std::array<std::vector<frames>, 2>	m_downsideframe;				//履帯
			std::array<std::vector<ViewAndModule>, 2>	m_TankViewPic;				//モジュール表示
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
		public: //コンストラクタ、デストラクタ
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
			//事前読み込み
			void		Set_Pre(const char* name) noexcept {
				this->m_name = name;
				MV1::Load("data/tank/" + this->m_name + "/model_LOADCALC.mv1", &this->m_DataObj);//model.pmx//model_LOADCALC.mv1
				MV1::Load("data/tank/" + this->m_name + "/col.mv1", &this->m_DataCol);
			}
			//メイン読み込み
			void			Set(void) noexcept {
				//固有
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
					if (p.find("転輪", 0) != std::string::npos) {
						this->m_wheelframe.resize(this->m_wheelframe.size() + 1);
						this->m_wheelframe.back().Set(i, this->m_DataObj);
					}
					else if ((p.find("輪", 0) != std::string::npos) && (p.find("転輪", 0) == std::string::npos)) {
						this->m_wheelframe_nospring.resize(this->m_wheelframe_nospring.size() + 1);
						this->m_wheelframe_nospring.back().Set(i, this->m_DataObj);
					}
					else if (p.find("旋回", 0) != std::string::npos) {
						this->m_GunFrameData.resize(this->m_GunFrameData.size() + 1);
						this->m_GunFrameData.back().SetFrame(this->m_DataObj, i);
					}
					else if (p == "min") {
						minpos = this->m_DataObj.frame(i);
					}
					else if (p == "max") {
						maxpos = this->m_DataObj.frame(i);
					}
					else if (p.find("２D物理", 0) != std::string::npos || p.find("2D物理", 0) != std::string::npos) { //2D物理
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
						std::sort(this->m_b2upsideframe[0].begin(), this->m_b2upsideframe[0].end(), [](const frames& x, const frames& y) { return x.GetFrameWorldPosition().z() < y.GetFrameWorldPosition().z(); }); //ソート
						std::sort(this->m_b2upsideframe[1].begin(), this->m_b2upsideframe[1].end(), [](const frames& x, const frames& y) { return x.GetFrameWorldPosition().z() < y.GetFrameWorldPosition().z(); }); //ソート
					}
					else if (p.find("履帯設置部", 0) != std::string::npos) { //2D物理
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
				this->m_DynamicBox.SetAsBox(minmaxsub.x() / 2, minmaxsub.z() / 2, b2Vec2(minmaxadd.x() / 2, -minmaxadd.z() / 2), 0.f);	//ダイナミックボディに別のボックスシェイプを定義
				//4隅確定
				this->m_square[0] = GetSide(true, false);			//2		左後部0
				this->m_square[1] = GetSide(true, true);			//10	左前部1
				this->m_square[2] = GetSide(false, false);			//3		右後部2
				this->m_square[3] = GetSide(false, true);			//11	右前部3
				//装甲

				//車体モジュール
				this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_body.png")), -1));
				int human_body_num = 0;
				int ammo_body_num = 0;
				for (int i = 0; i < this->m_DataCol.mesh_num(); i++) {
					std::string p = this->m_DataCol.material_name(i);
					if (p.find("armer", 0) != std::string::npos) {
						this->m_armer_mesh.emplace_back(i, std::stof(getparams::getright(p.c_str())));//装甲
					}
					else if (p.find("space", 0) != std::string::npos) {
						this->m_space_mesh.emplace_back(i);//空間装甲
					}
					else if (p.find("left_foot", 0) != std::string::npos) {
						this->m_module_mesh.emplace_back(i);//モジュール
						this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_track_l.bmp")), (int)this->m_module_mesh.size() - 1));
					}
					else if (p.find("right_foot", 0) != std::string::npos) {
						this->m_module_mesh.emplace_back(i);//モジュール
						this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_track_r.bmp")), (int)this->m_module_mesh.size() - 1));
					}
					else if (p.find("engine", 0) != std::string::npos) {
						this->m_module_mesh.emplace_back(i);//モジュール
						this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_engine_body.bmp")), (int)this->m_module_mesh.size() - 1));
					}
					else if (p.find("human_body", 0) != std::string::npos) {
						this->m_module_mesh.emplace_back(i);//モジュール
						this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_human_body_" + std::to_string(human_body_num) + ".bmp")), (int)this->m_module_mesh.size() - 1));
						human_body_num++;
					}
					else if (p.find("ammo_body", 0) != std::string::npos) {
						this->m_module_mesh.emplace_back(i);//モジュール
						this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_ammo_body_" + std::to_string(ammo_body_num) + ".bmp")), (int)this->m_module_mesh.size() - 1));
						ammo_body_num++;
					}
				}
				this->m_TankViewPic[0].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_look_body.bmp")), 0));
				//砲塔モジュール
				this->m_TankViewPic[1].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_turret.png")), -1));
				int human_turret_num = 0;
				int ammo_turret_num = 0;
				for (int i = 0; i < this->m_DataCol.mesh_num(); i++) {
					std::string p = this->m_DataCol.material_name(i);
					if (p.find("gun", 0) != std::string::npos) {
						this->m_module_mesh.emplace_back(i);//モジュール
						this->m_TankViewPic[1].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_gun.bmp")), (int)this->m_module_mesh.size() - 1));
					}
					else if (p.find("human_turret", 0) != std::string::npos) {
						this->m_module_mesh.emplace_back(i);//モジュール
						this->m_TankViewPic[1].emplace_back(ViewAndModule(std::make_shared<GraphHandle>(GraphHandle::Load("data/UI/body/battle_human_turret_" + std::to_string(human_turret_num) + ".bmp")), (int)this->m_module_mesh.size() - 1));
						human_turret_num++;
					}
					else if (p.find("ammo_turret", 0) != std::string::npos) {
						this->m_module_mesh.emplace_back(i);//モジュール
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
		//砲
		class Guns {
		private:
			float							m_loadtimer{ 0 };			//装てんカウンター
			bool							m_reloadSEFlag{ true };		//
			float							m_Recoil{ 0.f };			//駐退
			float							m_React{ 0.f };				//反動
			int								m_rounds{ 0 };				//弾数
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
			//銃反動
			void		FireReaction(MATRIX_ref* mat_t) const noexcept {
				(*mat_t) *= MATRIX_ref::RotAxis(mat_t->xvec(), this->m_ShotRadAdd.x()) * MATRIX_ref::RotAxis(mat_t->zvec(), this->m_ShotRadAdd.z());
			}
		public:
			//角度指示
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
		public: //コンストラクタ、デストラクタ
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
				auto SE = SoundPool::Instance();
				bool isshot = (key && this->m_loadtimer == 0 && this->m_rounds > 0);
				//射撃
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
		//戦車
		class VehicleClass : public ObjectBaseClass, public EffectControl {
		private:
			//弾痕
			class HIT_ACTIVE {
				struct Hit {
					bool		m_IsActive{ false };	//弾痕フラグ
					int			m_UseFrame{ 0 };		//使用フレーム
					MV1			m_obj;					//弾痕モデル
					moves		m_move;					//座標
				};
				std::array<Hit, 24> hit_obj;			//弾痕
				int hitbuf = 0;							//使用弾痕
			public:
				void			Set(const moves& this_move, const  VECTOR_ref& Put_pos, const  VECTOR_ref& Put_normal, const VECTOR_ref& ammo_nomal/*this->move.vec.Norm()*/, const float&caliber, bool isPene) noexcept {
					float asize = 200.f*caliber;
					auto y_vec = MATRIX_ref::Vtrans(Put_normal, this_move.mat.Inverse());
					auto z_vec = MATRIX_ref::Vtrans(Put_normal.cross(ammo_nomal).Norm(), this_move.mat.Inverse());
					auto scale = VECTOR_ref::vget(asize / std::abs(ammo_nomal.dot(Put_normal)), asize, asize);

					this->hit_obj[this->hitbuf].m_UseFrame = (isPene) ? 0 : 1;				//弾痕
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
					//弾痕
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
			//命中関連
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
			//履帯BOX2D
			class FootWorld {
			private:
				class cat_frame {
				private:
					frames					m_frame;
					float					m_Res_y{ (std::numeric_limits<float>::max)() };
					EffectS					m_gndsmkeffcs;
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
					void			FrameExecute(MV1* pTargetObj, const std::shared_ptr<BackGroundClass>& pMapPtr, bool checkmore) noexcept {
						if (this->m_frame.GetFrameID() >= 0) {
							auto y_vec = pTargetObj->GetMatrix().yvec();
							pTargetObj->frame_Reset(this->m_frame.GetFrameID());
							auto startpos = pTargetObj->frame(this->m_frame.GetFrameID());
							auto colres = pMapPtr->GetGroundCol().CollCheck_Line(
								startpos + y_vec * ((-this->m_frame.GetFrameWorldPosition().y()) + 2.f*Scale_Rate),
								startpos + y_vec * ((-this->m_frame.GetFrameWorldPosition().y()) - 0.3f*Scale_Rate));
							if (checkmore) {
								for (auto& bu : pMapPtr->GetBuildCol()) {
									auto pos_p = (startpos - bu.GetPosition().pos()); pos_p.y(0);
									if (pos_p.size() < 5.f*Scale_Rate) {
										auto col_p = bu.GetCol(
											startpos + y_vec * ((-this->m_frame.GetFrameWorldPosition().y()) + 2.f*Scale_Rate),
											startpos + y_vec * ((-this->m_frame.GetFrameWorldPosition().y()) - 0.3f*Scale_Rate));
										if (col_p.HitFlag == TRUE) {
											colres = col_p;
										}
									}
								}
							}
							this->m_Res_y = (colres.HitFlag == TRUE) ? colres.HitPosition.y : (std::numeric_limits<float>::max)();
							pTargetObj->SetFrameLocalMatrix(this->m_frame.GetFrameID(),
								MATRIX_ref::Mtrans(VECTOR_ref::up() * ((colres.HitFlag == TRUE) ? (this->m_Res_y + y_vec.y() * this->m_frame.GetFrameWorldPosition().y() - startpos.y()) : -0.4f*Scale_Rate)) *
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
				std::shared_ptr<b2World>	m_FootWorld{ nullptr };		//足world
				std::vector<b2Pats>			m_Foot, m_Wheel, m_Yudo;	//
				std::vector<cat_frame>		m_downsideframe;			//履帯
			public:			//getter
				const auto&			Getdownsideframe(void) const noexcept { return this->m_downsideframe; }
			public:
				void			Init(bool IsLeft, const VhehicleData* pUseVeh, MV1* pTargetObj) noexcept {
					this->m_FootWorld = std::make_unique<b2World>(b2Vec2(0.0f, 0.0f));
					float LorR = ((IsLeft) ? 1.f : -1.f);
					//履帯
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
								b2PolygonShape f_dynamicBox; //ダイナミックボディに別のボックスシェイプを定義
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
					//転輪
					this->m_Wheel.clear();
					this->m_Yudo.clear();
					if (!this->m_Foot.empty()) {
						//転輪(動く)
						for (const auto& w : pUseVeh->Get_wheelframe()) {
							VECTOR_ref vects = pTargetObj->GetFrameLocalMatrix(w.GetFrameID()).pos();
							if (vects.x() * LorR > 0) {
								this->m_Wheel.resize(this->m_Wheel.size() + 1);
								b2CircleShape shape;
								shape.m_radius = vects.y() - 0.1f;
								this->m_Wheel.back().Set(CreateB2Body(this->m_FootWorld, b2_kinematicBody, vects.z(), vects.y()), &shape);
							}
						}
						//誘導輪(動かない)
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
				void			FirstExecute(MV1* pTargetObj, const std::shared_ptr<BackGroundClass>& pMapPtr, bool checkmore) noexcept {
					for (auto& t : this->m_downsideframe) {
						t.FrameExecute(pTargetObj, pMapPtr, checkmore);
					}
				}
				void			LateExecute(bool IsLeft, const VhehicleData* pUseVeh, MV1* pTargetObj, const b2Vec2& pGravity, float pWheelRotate, float pSpd) noexcept {
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
					//エフェクト更新
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
		private:
			//操作
			bool												m_ReadySwitch{ false };							//
			bool												m_KeyActive{ true };							//
			std::array<bool, 7>									m_key{ false };									//キー
			MATRIX_ref											m_MouseVec;										//マウスエイム用変数確保
			float												m_AimingDistance{ 500.f*Scale_Rate };			//
			bool												m_view_override{ false };						//
			std::vector<VECTOR_ref>								m_view_rad;										//
			float												m_range{ 6.0f };								//
			float												m_range_r{ this->m_range };						//
			float												m_range_change{ this->m_range / 10.f };			//
			float												m_ratio{ 1.f };									//
			bool												m_changeview{ false };							//
			//プレイヤー周り
			CharaTypeID											m_CharaType{ CharaTypeID::Enemy };				//
			//車体
			const VhehicleData*									m_VecData{ nullptr };							//固有値
			std::vector<Guns>									m_Gun;											//
			float												m_speed{ 0.f };									//移動速度
			float												m_spd_rec{ 0.f };								//
			float												m_speed_add{ 0.f };								//
			float												m_speed_sub{ 0.f };								//
			VECTOR_ref											m_add_vec_real;									//移動ベクトルバッファ
			VECTOR_ref											m_BodyNormal{ VECTOR_ref::up() };				//
			VECTOR_ref											m_Tilt;											//
			VECTOR_ref											m_radAdd;										//
			float												m_yradadd_left{ 0.f };							//
			float												m_yradadd_right{ 0.f };							//
			float												m_engine_time{ 0.f };							//エンジン音声
			HIT_ACTIVE											m_Hit_active;									//
			std::vector<HitSortInfo> hitssort;																	//フレームに当たった順番
			std::vector<MV1_COLL_RESULT_POLY> hitres;															//確保
			bool												m_PosBufOverRideFlag{ false };					//
			VECTOR_ref											m_PosBufOverRide;								//
			VECTOR_ref											m_VecBufOverRide;								//
			VECTOR_ref											m_RadOverRide;									//
			//転輪
			float												m_wheel_Left{ 0.f };							//転輪回転
			float												m_wheel_Right{ 0.f };							//転輪回転
			std::vector<float>									m_wheel_frameYpos{ 0.f };						//転輪のY方向保持
			//ダメージ
			HitPoint											m_HP{ 100 };
			std::vector<HitPoint>								m_HP_parts;
			DamageEvent											m_DamageEvent;									//
			unsigned long long									m_DamageSwitch{ 0 };							//
			unsigned long long									m_DamageSwitchRec{ 0 };							//
			//box2d
			b2Pats												m_b2mine;										//BOX2D
			float												m_spd_buf{ 0.f };								//BOX2D
			std::array<FootWorld, 2>							m_b2Foot;										//履帯BOX2D
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
			const auto		Get_pseed_per(void) const noexcept { return this->m_add_vec_real.size() / (this->m_VecData->GetMaxFrontSpeed() / 3.6f); }			//移動速度のパーセンテージ
			const auto&		Gunloadtime(size_t id_t) const noexcept { return this->m_Gun[id_t].Getloadtime(); }
			const auto&		Gunround(size_t id_t) const noexcept { return this->m_Gun[id_t].Getrounds(); }
			const auto&		Get_Gunsize(void) const noexcept { return this->m_Gun.size(); }
			const auto&		Get_changeview(void) const noexcept { return this->m_changeview; }																	//照準変更時
			const auto&		GetViewRad(void) const noexcept { return this->m_view_rad[0]; }
			const auto&		Get_ratio(void) const noexcept { return this->m_ratio; }																			//UI用
			const auto&		GetAimingDistance(void) const noexcept { return this->m_AimingDistance; }
			const auto		Get_alive(void) const noexcept { return this->m_HP != 0; }																			//生きているか
			const auto		Get_body_yrad(void) const noexcept { auto pp = this->m_move.mat.zvec()*-1.f; return atan2f(pp.x(), pp.z()); }
			const auto		is_ADS(void) const noexcept { return this->m_range == 0.f; }																		//ADS中
			const auto		GetGunMuzzleBase(int ID) const noexcept { return GetObj_const().frame(this->m_Gun[ID].GetGunTrunnionFrameID()); }
			const auto		GetGunMuzzlePos(int ID) const noexcept { return GetObj_const().frame(this->m_Gun[ID].GetGunMuzzleFrameID()); }
			const auto		GetGunMuzzleVec(int ID) const noexcept { return (GetGunMuzzlePos(ID) - GetGunMuzzleBase(ID)).Norm(); }

			const auto		GetSquarePos(int ID) const noexcept { return GetObj_const().frame(this->m_VecData->Get_square(ID)); }


			const auto		Get_EyePos_Base(void) const noexcept { return (is_ADS()) ? GetGunMuzzleBase(0) : (this->m_move.pos + (this->m_move.mat.yvec() * 3.f * Scale_Rate)); }
			const auto		Set_MidPos(void) noexcept { return (this->m_move.pos + (this->m_move.mat.yvec() * 1.5f * Scale_Rate)); }							//HPバーを表示する場所
			//
			void			ReSet_range(void) noexcept { this->m_range = 6.f; }
			void			SetPosBufOverRide(const VECTOR_ref& pPos, const VECTOR_ref& pVec, const VECTOR_ref& rad) noexcept {
				this->m_PosBufOverRideFlag = true;
				this->m_PosBufOverRide = pPos;
				this->m_VecBufOverRide = pVec;
				this->m_RadOverRide = rad;
			}
		public:
			void			ValueInit(const VhehicleData* pVeh_data, const MV1& hit_pic, const std::shared_ptr<b2World>& pB2World, PlayerID pID) noexcept;
			void			ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady, bool isOverrideView) noexcept;													//
			void			Setcamera(Camera3DInfo& m_MainCamera, const float fov_base) noexcept;																	//カメラ設定出力
			const std::pair<bool, bool>		CheckAmmoHit(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept;
			void			HitGround(const VECTOR_ref& pPos, const VECTOR_ref& pNorm, const VECTOR_ref& pVec) noexcept;
			void			DrawModuleView(int xp, int yp, int size) noexcept;																					//被弾チェック
		private://更新関連
			const auto		CheckAmmoHited(const AmmoClass& pAmmo) noexcept;																					//被弾チェック
			const auto		CalcAmmoHited(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept;															//被弾処理

			void			ExecuteSavePrev(void) noexcept;																										//以前の状態保持
			void			ExecuteElse(void) noexcept;																											//その他
			void			ExecuteInput(void) noexcept;																										//操作
			void			ExecuteFrame(void) noexcept;																										//フレーム操作
			void			ExecuteMove(void) noexcept;																											//移動操作
			void			ExecuteMatrix(void) noexcept;																										//SetMat指示更新
			void			DrawCommon(void) noexcept;
		public: //コンストラクタ、デストラクタ
			VehicleClass(void) noexcept { this->m_objType = ObjType::Vehicle; }
			~VehicleClass(void) noexcept {}
		public: //継承
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				this->m_IsDraw = true;
			}
			void			FirstExecute(void) noexcept override {
				//初回のみ更新する内容
				if (this->m_IsFirstLoop) {
					this->m_view_override = false;
				}
				ExecuteSavePrev();			//以前の状態保持
				ExecuteElse();				//その他
				ExecuteInput();				//操作
				ExecuteFrame();				//フレーム操作
				ExecuteMove();				//移動操作
				this->m_PosBufOverRideFlag = false;
			}
			void			LateExecute(void) noexcept override {
				ExecuteMatrix();			//SetMat指示//0.03ms
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

				fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効  FALSE:無効 )
				fog_mode = GetFogMode();														// フォグモードを取得する
				GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
				GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
				fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )

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
