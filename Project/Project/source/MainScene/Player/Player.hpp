#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PlayerControl {
		private:
			std::shared_ptr<VehicleClass>	m_Vehicle{ nullptr };
			float							m_Score{ 0.f };							//ƒXƒRƒA
		public:
			PlayerControl(void) noexcept {
				this->m_Score = 0.f;
			}
			~PlayerControl(void) noexcept {
				this->Dispose();
			}
		public:
			void		SetVehicle(const std::shared_ptr<VehicleClass>& pVehicle) { m_Vehicle = pVehicle; }
			void		AddScore(float value) { this->m_Score += value; }
			void		SubScore(float value) { this->m_Score -= value; }
			void		SetScore(float value) { this->m_Score = value; }
			auto&		GetVehicle(void) noexcept { return m_Vehicle; }
			const auto&	GetScore(void) const noexcept { return this->m_Score; }
			const auto	IsRide(void) const noexcept { return (bool)m_Vehicle; }

			const auto		GetNetSendMove(void) const noexcept {
				SendInfo ans;
				if (!IsRide()) {
				}
				else {
					ans.m_Pos = m_Vehicle->GetMove().pos;
					ans.m_Vec = m_Vehicle->GetMove().vec;
					ans.m_rad.y(this->m_Vehicle->Get_body_yrad());
					ans.m_Damage = &m_Vehicle->GetDamageEvent();
					ans.m_DamageSwitch = (this->m_Vehicle->GetDamageSwitch() ? 1 : 0);
				}
				return ans;
			}

			const auto		GetPos(void) const noexcept { return m_Vehicle->GetMatrix().pos(); }
			const auto		GetAim(void) const noexcept { return m_Vehicle->GetLookVec().zvec()*-1.f; }
			const auto		GetRadBuf(void) const noexcept { return m_Vehicle->GetViewRad(); }
		public:
			void Init(void) noexcept {
				this->m_Vehicle = nullptr;
			}

			void Dispose(void) noexcept {
				this->m_Vehicle = nullptr;
			}
		};

		class PlayerManager :public SingletonBase<PlayerManager> {
		private:
			friend class SingletonBase<PlayerManager>;
		private:
			std::vector<PlayerControl> m_Player;
		public:
			auto&		GetPlayer(int ID) { return m_Player[ID]; }
		public:
			void Init(int playerNum) {
				this->m_Player.resize(playerNum);
			}
			void Dispose(void) noexcept {
				this->m_Player.clear();
			}
		};

	};
};
