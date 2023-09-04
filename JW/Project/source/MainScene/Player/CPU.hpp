#pragma once
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"
#include	"../../sub.hpp"
#include "../../ObjectManager.hpp"

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
				wayp_pre.resize(12);
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
			void Draw_Debug(const VECTOR_ref&) {
			}
#endif
		};

		class AIControl {
			const float										SearchTotal{ 1.f };
		private:
			//
			std::shared_ptr<BackGroundClassBase>			m_BackGround;				//BG
			//
			AI												cpu_do;						//AI用
			PlayerID										m_MyCharaID{ 0 };
			VECTOR_ref										tgtpos;
			float											SearchCount{ 0.f };
			int												tgtID{ -1 };
			bool											tgtLock{ false };
			float											AfterKillCount{ 0.f };		//キルした後のカウント
			float											AvoidCount{ 0.f };
			bool											IsAvoidLR{ false };

			float											GabaCount{ 0.f };
			//
		private:
			int Get_next_waypoint(std::vector<int>, VECTOR_ref, VECTOR_ref) {
				int now = -1;
				return now;
			}
		public:
			//AI操作
			void SetNextWaypoint(const VECTOR_ref& vec_z);
			void AI_move(InputControl* MyInput) noexcept;
		public:
			void Init(const std::shared_ptr<BackGroundClassBase>& BackBround_t, PlayerID MyCharaID) noexcept;
			void Execute() noexcept {}
			void Draw() noexcept;
			void Dispose() noexcept {

			}
		};
	};
};
