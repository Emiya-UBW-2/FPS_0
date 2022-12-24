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
			int ai_tankback_cnt = 0;
			float ai_time_walk{ 0.f };
			bool ai_reload{ false };
			int ai_phase{ 0 };
			std::vector<int> wayp_pre{ 0 };

			AI(void) noexcept {
				wayp_pre.resize(6);
			}
			~AI(void) noexcept {
				wayp_pre.clear();
			}

			void Spawn(int now) {
				this->ai_phase = 0;
				this->ai_time_shoot = 0.f;
				this->ai_time_a = 0.f;
				this->ai_time_d = 0.f;
				this->ai_time_e = 0.f;
				this->ai_time_tankback = 0.f;
				this->ai_time_tankback_ing = 0.f;
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
					startpos.yadd(0.5f*Scale_Rate);
					endpos.yadd(0.5f*Scale_Rate);
					DrawCapsule_3D(startpos, endpos, 1.f*Scale_Rate, GetColor(0, 255, 0), GetColor(0, 255, 0));
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
				int32_t x_m, y_m;
				//AI
				//操作
				VECTOR_ref vec_to = VECTOR_ref::zero();
				//
				auto vec_mat = MyVeh->GetMove().mat;
				auto vec_gunmat = MyVeh->GetGunMuzzleMatrix(0);
				auto vec_x = vec_gunmat.xvec();
				auto vec_y = vec_gunmat.yvec();
				auto vec_zp = vec_gunmat.yvec() * -1.f;
				auto vec_z = vec_mat.zvec() * -1.f;
				//狙うキャラを探索+AIのフェーズ選択
				{
					bool ans = false;
					VECTOR_ref StartPos = MyVeh->Get_EyePos_Base();
					//*
					for (auto& tgt : *vehicle_Pool) {
						if (&MyVeh == &tgt) { continue; }
						if (!tgt->Get_alive()) { continue; }
						VECTOR_ref EndPos = tgt->GetMove().pos + VECTOR_ref::vget(0.f, 1.5f*Scale_Rate, 0.f);
						if (this->m_BackGround->CheckLinetoMap(StartPos, &EndPos, false, false)) { continue; }
						VECTOR_ref vec_tmp = EndPos - StartPos;
						if (vec_to == VECTOR_ref::zero()) { vec_to = vec_tmp; } //基準の作成
						if (vec_to.Length() >= vec_tmp.Length()) {
							vec_to = vec_tmp;
							ans = true;
						}
					}
					//*/
					if (!ans) {
						this->cpu_do.ai_phase = 0;
					}
					else if (vec_z.dot(vec_to.Norm()) >= 0 && vec_to.size() <= 300.f) {
						this->cpu_do.ai_phase = 1;
					}
					this->cpu_do.ai_phase = 0;
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

					x_m = int(vec_y.dot(vec_to.Norm()) * 40);
					y_m = -int(vec_x.dot(vec_to.Norm()) * 40);

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
					A_key = (GetRand(100) > 50);
					D_key = !A_key;
					S_key = true;
				}
				break;
				default:
					break;
				}
				x_m = std::clamp(x_m, -40, 40);
				y_m = std::clamp(y_m, -40, 40);
				MyInput->SetInput(
					(float)x_m,
					(float)y_m,
					W_key, S_key, A_key, D_key,
					false, false, false, false, false, false, false,
					false,
					false, false, false,
					false, false
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
	cpu_do.Draw_Debug(&m_BackGround->GetWayPoint());
#endif
			}
		};
	};
};
