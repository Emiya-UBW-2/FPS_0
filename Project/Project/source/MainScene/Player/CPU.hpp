#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//AI用
		class AI {
		public:
			float ai_time_a{ 0.f };
			float ai_time_d{ 0.f };
			float ai_time_q{ 0.f };
			float ai_time_e{ 0.f };
			float ai_time_shoot{ 0.f };
			float ai_time_tankback{ 0.f };
			float ai_time_tankback_ing{ 0.f };
			float ai_time_find{ 0.f };
			VECTOR_ref LastFindPoint;
			float ai_time_turn{ 0.f };
			int ai_tankback_cnt = 0;
			float ai_time_walk{ 0.f };
			bool ai_reload{ false };
			int ai_phase{ 0 };
			std::vector<int> wayp_pre{ 0 };
			int ai_AimTarget{ 0 };

			AI(void) noexcept {
				wayp_pre.resize(6);
			}
			~AI(void) noexcept {
				wayp_pre.clear();
			}

			void Spawn(int now) {
				this->ai_phase = 0;
				this->ai_AimTarget = -1;
				this->ai_time_shoot = 0.f;
				this->ai_time_a = 0.f;
				this->ai_time_d = 0.f;
				this->ai_time_e = 0.f;
				this->ai_time_tankback = 0.f;
				this->ai_time_tankback_ing = 0.f;
				this->ai_time_find = 0.f;
				this->ai_time_turn = 0.f;
				this->ai_tankback_cnt = 0;
				this->fill_wayp_pre(now);
			}
			void fill_wayp_pre(int now) {
				for (auto& w : this->wayp_pre) { w = now; }
			}
			void Set_wayp_pre(int now) {
				for (size_t i = (this->wayp_pre.size() - 1); i >= 1; i--) { this->wayp_pre[i] = this->wayp_pre[i - 1]; }
				this->wayp_pre[0] = now;
			}
			void Set_wayp_return(void) noexcept {
				auto ppp = this->wayp_pre[1];
				for (size_t i = (this->wayp_pre.size() - 1); i >= 1; i--) { this->wayp_pre[i] = this->wayp_pre[0]; }
				this->wayp_pre[0] = ppp;
			}
#ifdef DEBUG
			void Draw_Debug(const std::vector<VECTOR_ref>* way_point) {
				for (size_t i = 0; i < this->wayp_pre.size() - 1; i++) {
					VECTOR_ref startpos = (*way_point)[this->wayp_pre[i]];
					VECTOR_ref endpos = (*way_point)[this->wayp_pre[i + 1]];
					startpos.yadd(5.5f*Scale_Rate);
					endpos.yadd(5.5f*Scale_Rate);
					DrawCapsule_3D(startpos, endpos, 0.25f*Scale_Rate, GetColor(0, 255, 0), GetColor(0, 255, 0));
				}
			}
#endif
		};

		class AIControl {
			AI									cpu_do;								//AI用
			std::shared_ptr<VehicleClass>		MyVeh;

			std::vector<std::shared_ptr<VehicleClass>>* vehicle_Pool{ nullptr };
			std::shared_ptr<BackGroundClass>			m_BackGround;				//BG
		private:
			int Get_next_waypoint(std::vector<int> wayp_pre, VECTOR_ref poss, VECTOR_ref zvec = VECTOR_ref::zero()) {
				int now = -1;
				auto tmp = VECTOR_ref::vget(0, 100.f*Scale_Rate, 0);
				for (auto& w : m_BackGround->GetWayPoint()) {
					auto id = &w - &m_BackGround->GetWayPoint().front();
					bool tt = true;
					for (auto& ww : wayp_pre) {
						if (id == ww) {
							tt = false;
						}
					}
					if (tt) {
						if (tmp.size() >= (w - poss).size()) {
							auto startpos = w + VECTOR_ref::vget(0, 0.5f*Scale_Rate, 0);
							auto endpos = poss + VECTOR_ref::vget(0, 0.5f*Scale_Rate, 0);

							if (!m_BackGround->CheckLinetoMap(startpos, &endpos, true, false)) {
								if (zvec == VECTOR_ref::zero() || zvec.Norm().dot((w - poss).Norm()) < 0.f) {
									tmp = (w - poss);
									now = int(id);
								}
							}
						}
					}
				}
				return now;
			}
		public:
			//AI操作
			void SetNextWaypoint(const VECTOR_ref& vec_z) {
				int now = Get_next_waypoint(this->cpu_do.wayp_pre, this->MyVeh->GetMove().pos, vec_z);
				if (now != -1) {
					this->cpu_do.Set_wayp_pre(now);
				}
			}
			void AI_move(InputControl* MyInput) noexcept {
				bool W_key{ false };
				bool A_key{ false };
				bool S_key{ false };
				bool D_key{ false };

				bool shotMain_Key{ false };
				bool shotSub_Key{ false };
				int32_t x_m, y_m;
				//AI
				//操作
				VECTOR_ref vec_to = VECTOR_ref::zero();
				//
				auto vec_mat = MyVeh->GetMove().mat;
				auto vec_gunmat = MyVeh->GetLookVec();
				auto vec_x = vec_gunmat.xvec();
				auto vec_y = vec_gunmat.yvec();
				auto vec_zp = vec_gunmat.zvec() * -1.f;
				auto vec_z = vec_mat.zvec() * -1.f;
				//狙うキャラを探索+AIのフェーズ選択
				{
					auto CheckCanLookTarget = [&](const std::shared_ptr<VehicleClass>& tgt, VECTOR_ref* Res) {


						if (!tgt->Get_alive()) { return false; }
						VECTOR_ref StartPos = MyVeh->Get_EyePos_Base();
						VECTOR_ref EndPos = tgt->GetMove().pos + VECTOR_ref::vget(0.f, 1.5f*Scale_Rate, 0.f);
						VECTOR_ref vec_tmp = EndPos - StartPos;
						if (vec_tmp.size() > 150.f*Scale_Rate) { return false; }
						if (vec_tmp.size() > 30.f*Scale_Rate) {
							auto vec_gunmat2 = MyVeh->GetGunMuzzleMatrix(0);
							auto vec_zp2 = vec_gunmat2.zvec() * -1.f;
							if (!(
								(vec_zp2.dot(vec_tmp.Norm()) > std::cos(deg2rad(45))) ||
								(vec_z.dot(vec_tmp.Norm()) > std::cos(deg2rad(45)))
								)) {
								return false;
							}
						}
						if (this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, false, false)) { return false; }
						if (Res) { *Res = vec_tmp; }
						return true;
					};

					bool ans = false;
					if (this->cpu_do.ai_phase != 1) {
						for (auto& tgt : *vehicle_Pool) {
							if (MyVeh == tgt) { continue; }
							VECTOR_ref vec_tmp;
							if (CheckCanLookTarget(tgt, &vec_tmp)) {
								if (vec_to == VECTOR_ref::zero()) { vec_to = vec_tmp; } //基準の作成
								if (vec_to.Length() >= vec_tmp.Length()) {
									vec_to = vec_tmp;
									this->cpu_do.ai_AimTarget = (int)(&tgt - &vehicle_Pool->front());
									this->cpu_do.LastFindPoint = tgt->GetMove().pos;
									ans = true;
								}
							}
						}
					}
					else {
						{
							auto& tgt = vehicle_Pool->at(this->cpu_do.ai_AimTarget);
							VECTOR_ref StartPos = MyVeh->Get_EyePos_Base();
							VECTOR_ref vec_to2 = tgt->GetMove().pos - StartPos;
							for (auto& tgt2 : *vehicle_Pool) {
								if (MyVeh == tgt2) { continue; }
								auto Index = (int)(&tgt2 - &vehicle_Pool->front());
								if (Index == this->cpu_do.ai_AimTarget) { continue; }
								VECTOR_ref vec_tmp;
								if (CheckCanLookTarget(tgt2, &vec_tmp)) {
									if (vec_to2 == VECTOR_ref::zero()) { vec_to2 = vec_tmp; } //基準の作成
									if (vec_to2.Length() > vec_tmp.Length()) {
										vec_to2 = vec_tmp;
										this->cpu_do.ai_AimTarget = Index;
										this->cpu_do.LastFindPoint = tgt2->GetMove().pos;
										ans = true;
									}
								}
							}
						}


						auto& tgt = vehicle_Pool->at(this->cpu_do.ai_AimTarget);
						VECTOR_ref vec_tmp;
						if (CheckCanLookTarget(tgt, &vec_tmp)) {
							this->cpu_do.LastFindPoint = tgt->GetMove().pos;
							ans = true;
						}
						else {
							VECTOR_ref StartPos = MyVeh->Get_EyePos_Base();
							VECTOR_ref EndPos = this->cpu_do.LastFindPoint + VECTOR_ref::vget(0.f, 1.5f*Scale_Rate,0.f);
							vec_tmp = EndPos - StartPos;
						}
						vec_to = vec_tmp + VECTOR_ref::vget(GetRandf(2.f*Scale_Rate), GetRandf(2.f*Scale_Rate), GetRandf(2.f*Scale_Rate));


						if (!tgt->Get_alive()) {
							this->cpu_do.ai_time_find = 0.f;
						}
					}
					if (!ans) {
						this->cpu_do.ai_time_find = std::max(this->cpu_do.ai_time_find - 1.f / FPS, 0.f);
						if (this->cpu_do.ai_time_find == 0.f) {
							this->cpu_do.ai_AimTarget=-1;
							this->cpu_do.ai_phase = 0;
						}
					}
					else {
						if (this->cpu_do.ai_time_find != 10.f) {
							this->cpu_do.ai_time_turn = 18.f;
						}
						this->cpu_do.ai_time_find = 10.f;
						this->cpu_do.ai_phase = 1;
					}
					this->cpu_do.ai_time_turn = std::max(this->cpu_do.ai_time_turn - 1.f / FPS, 0.f);
				}
				//
				switch (this->cpu_do.ai_phase) {
				case 0://戦車乗車中通常フェイズ
				{
					W_key = true;

					//向き指定
					vec_to = m_BackGround->GetWayPoint()[this->cpu_do.wayp_pre.front()] - this->MyVeh->GetMove().pos;
					vec_to.y(0.f);

					vec_z = MyVeh->GetMove().mat.zvec();
					//到達時に判断
					if (vec_to.size() <= 1.5f*Scale_Rate) {
						SetNextWaypoint(vec_z);
					}

					{
						auto Zbuf = vec_zp.Norm();
						Zbuf.z(std::hypotf(Zbuf.x(), Zbuf.z()));
						auto Tobuf = vec_to.Norm();
						Tobuf.z(std::hypotf(Tobuf.x(), Tobuf.z()));
						x_m = -int(Zbuf.Norm().cross(Tobuf.Norm()).x() * 40);
					}
					{
						auto Zbuf = vec_zp;
						Zbuf.y(0.f);
						auto Tobuf = vec_to;
						Tobuf.y(0.f);
						y_m = int(Zbuf.Norm().cross(Tobuf.Norm()).y() * 40);
					}

					if (this->cpu_do.ai_tankback_cnt >= 2) {
						this->cpu_do.ai_tankback_cnt = 0;
						SetNextWaypoint(vec_z*-1.f);
					}

					VECTOR_ref vec_z2 = vec_z; vec_z2.y(0); vec_z2 = vec_z2.Norm();
					VECTOR_ref vec_to2 = vec_to; vec_to2.y(0); vec_to2 = vec_to2.Norm();

					float cost = vec_to2.cross(vec_z2).y();
					float sint = sqrtf(std::abs(1.f - cost * cost));
					auto view_yrad = std::atan2f(cost, sint); //cos取得2D

					if (this->cpu_do.ai_time_tankback_ing > 0.f && true) {//無効化x
						this->cpu_do.ai_time_tankback_ing -= 1.f / FPS;
						W_key = false;
						S_key = true;
						A_key = (GetRand(100) > 50);
						D_key = !A_key;
					}
					else {
						this->cpu_do.ai_time_tankback_ing = 0.f;
						if (std::abs(view_yrad) > deg2rad(30)) {
							W_key = false;
							if (x_m < 0) {
								//this->S_key = true;
							}
						}
						if (MyVeh->Getvec_real().Length() <= MyVeh->GetMove().vec.size() *(0.5f)) {
							this->cpu_do.ai_time_tankback += 1.f / FPS;
						}
						else {
							this->cpu_do.ai_time_tankback = 0.f;
						}
						if (this->cpu_do.ai_time_tankback > 3.f) {
							this->cpu_do.ai_time_tankback = 0.f;
							this->cpu_do.ai_time_tankback_ing = 5.f;
							this->cpu_do.ai_tankback_cnt++;
						}
						if (view_yrad > 0) {
							A_key = false;
							D_key = true;
						}
						else if (view_yrad < 0) {
							A_key = true;
							D_key = false;
						}
					}
				}
				break;
				case 1://戦車乗車中戦闘フェイズ
				{
					{
						auto Zbuf = vec_zp.Norm();
						Zbuf.z(std::hypotf(Zbuf.x(), Zbuf.z()));
						auto Tobuf = vec_to.Norm();
						Tobuf.z(std::hypotf(Tobuf.x(), Tobuf.z()));
						x_m = -int(Zbuf.Norm().cross(Tobuf.Norm()).x() * 40);
					}
					{
						auto Zbuf = vec_zp;
						Zbuf.y(0.f);
						auto Tobuf = vec_to;
						Tobuf.y(0.f);
						y_m = int(Zbuf.Norm().cross(Tobuf.Norm()).y() * 40);
					}
					{
						shotMain_Key = true;

						shotMain_Key &= (this->cpu_do.ai_time_find == 10.f);

						auto vec_gunmat = MyVeh->GetGunMuzzleMatrix(0);
						auto vec_zp2 = vec_gunmat.zvec() * -1.f;
						{
							auto Zbuf = vec_zp2.Norm();
							Zbuf.z(std::hypotf(Zbuf.x(), Zbuf.z()));
							auto Tobuf = vec_to.Norm();
							Tobuf.z(std::hypotf(Tobuf.x(), Tobuf.z()));
							shotMain_Key &= (std::abs(Zbuf.Norm().cross(Tobuf.Norm()).x()) < 0.1f);
						}
						{
							auto Zbuf = vec_zp2;
							Zbuf.y(0.f);
							auto Tobuf = vec_to;
							Tobuf.y(0.f);
							shotMain_Key &= (std::abs(Zbuf.Norm().cross(Tobuf.Norm()).y()) < 0.1f);
						}
					}
					if (MyVeh->Get_Gunsize() >= 2) {
						shotSub_Key = (GetRand(100) <= 5);

						shotSub_Key &= (this->cpu_do.ai_time_find == 10.f);

						auto vec_gunmat = MyVeh->GetGunMuzzleMatrix(1);
						auto vec_zp2 = vec_gunmat.zvec() * -1.f;
						{
							auto Zbuf = vec_zp2.Norm();
							Zbuf.z(std::hypotf(Zbuf.x(), Zbuf.z()));
							auto Tobuf = vec_to.Norm();
							Tobuf.z(std::hypotf(Tobuf.x(), Tobuf.z()));
							shotSub_Key &= (std::abs(Zbuf.Norm().cross(Tobuf.Norm()).x()) < std::sin(deg2rad(2)));
						}
						{
							auto Zbuf = vec_zp2;
							Zbuf.y(0.f);
							auto Tobuf = vec_to;
							Tobuf.y(0.f);
							shotSub_Key &= (std::abs(Zbuf.Norm().cross(Tobuf.Norm()).y()) < std::sin(deg2rad(2)));
						}
					}
					if (this->cpu_do.ai_time_turn <= 18.f - 3.f) {
						auto Zbuf = vec_z;
						Zbuf.y(0.f);
						auto Tobuf = vec_to;
						Tobuf.y(0.f);
						auto SinBuf = -Zbuf.Norm().cross(Tobuf.Norm()).y();
						if (std::abs(SinBuf) > 0.2f) {
							A_key = (SinBuf > 0);
							D_key = !A_key;
						}
						else {
							auto cosBuf = Zbuf.Norm().dot(Tobuf.Norm());
							if (cosBuf < 0.f) {
								A_key = true;
								D_key = !A_key;
							}
						}
					}
					if (18.f - 12.f <= this->cpu_do.ai_time_turn && this->cpu_do.ai_time_turn <= 18.f - 6.f) {
						W_key = false;
						S_key = !W_key;
					}
					else {
						W_key = true;
						S_key = !W_key;
					}
					if (this->cpu_do.ai_time_turn < 0.f) {
						this->cpu_do.ai_time_turn = 18.f;
					}


					if (this->cpu_do.ai_time_find <= 5.f) {
						W_key = true;
						S_key = !W_key;
					}
					//
					{
						auto& tgt = vehicle_Pool->at(this->cpu_do.ai_AimTarget);
						VECTOR_ref StartPos = MyVeh->Get_EyePos_Base();
						VECTOR_ref vec_to2 = tgt->GetMove().pos - StartPos;
						if (vec_to2.Length() <= 8.f*Scale_Rate) {
							W_key = false;
							S_key = false;
						}
					}

					//スタック回避
					if (this->cpu_do.ai_time_tankback_ing > 0.f && true) {//無効化x
						this->cpu_do.ai_time_tankback_ing -= 1.f / FPS;
						W_key = !W_key;
						S_key = !W_key;
						A_key = (GetRand(100) > 50);
						D_key = !A_key;
					}
					else {
						this->cpu_do.ai_time_tankback_ing = 0.f;
						if (MyVeh->Getvec_real().Length() <= MyVeh->GetMove().vec.size() *(0.5f)) {
							this->cpu_do.ai_time_tankback += 1.f / FPS;
						}
						else {
							this->cpu_do.ai_time_tankback = 0.f;
						}
						if (this->cpu_do.ai_time_tankback > 3.f) {
							this->cpu_do.ai_time_tankback = 0.f;
							this->cpu_do.ai_time_tankback_ing = 5.f;
							this->cpu_do.ai_tankback_cnt++;
						}
					}
				}
				break;
				default:
					break;
				}


				//W_key = false;
				//A_key = false;
				//S_key = false;
				//D_key = false;

				//shotMain_Key = false;
				//shotSub_Key = false;

				MyInput->SetInput(
					(float)x_m / 100.f,
					(float)y_m / 100.f,
					W_key, S_key, A_key, D_key,
					false,
					false, false,
					false, false, false, false,
					shotSub_Key,//
					false,
					false,
					false,
					shotMain_Key,
					false
				);
			}
		public:
			void Init(std::vector<std::shared_ptr<VehicleClass>>* vehiclePool_t, std::shared_ptr<BackGroundClass>& BackBround_t, const std::shared_ptr<VehicleClass>& MyVeh_t) noexcept {
				vehicle_Pool = vehiclePool_t;
				MyVeh = MyVeh_t;
				this->m_BackGround = BackBround_t;

				//AIの選択をリセット
				int now = Get_next_waypoint(this->cpu_do.wayp_pre, this->MyVeh->GetMove().pos);
				this->cpu_do.Spawn((now != -1) ? now : 0);
			}
			void Execute() noexcept {

			}
			void Draw() noexcept {
#ifdef DEBUG
				if (!MyVeh->Get_alive()) { return; }
				if (MyVeh == vehicle_Pool->at(0)) { return; }
				cpu_do.Draw_Debug(&m_BackGround->GetWayPoint());
				if (this->cpu_do.ai_phase == 1) {
					auto& tgt = vehicle_Pool->at(this->cpu_do.ai_AimTarget);
					VECTOR_ref StartPos = MyVeh->Get_EyePos_Base() + VECTOR_ref::vget(0.f, 5.f*Scale_Rate, 0.f);
					VECTOR_ref EndPos = tgt->GetMove().pos + VECTOR_ref::vget(0.f, 5.f*Scale_Rate, 0.f);

					DrawCapsule_3D(StartPos, EndPos, 0.1f*Scale_Rate, GetColor(255, 0, 0), GetColor(255, 0, 0));
				}
				//*
				{
					auto vec_gunmat = MyVeh->GetGunMuzzleMatrix(0);
					auto vec_x = vec_gunmat.xvec();
					auto vec_y = vec_gunmat.yvec();
					auto vec_zp = vec_gunmat.zvec() * -1.f;

					VECTOR_ref StartPos = MyVeh->Get_EyePos_Base() + VECTOR_ref::vget(0.f, 5.f*Scale_Rate, 0.f);

					DrawCapsule_3D(StartPos, StartPos + vec_x * Scale_Rate*5.f, 0.1f*Scale_Rate, GetColor(255, 0, 0), GetColor(255, 0, 0));
					DrawCapsule_3D(StartPos, StartPos + vec_y * Scale_Rate*5.f, 0.1f*Scale_Rate, GetColor(0, 255, 0), GetColor(255, 0, 0));
					DrawCapsule_3D(StartPos, StartPos + vec_zp * Scale_Rate*5.f, 0.1f*Scale_Rate, GetColor(0, 0, 255), GetColor(255, 0, 0));
				}
				//*/
#endif
			}
		};
	};
};
