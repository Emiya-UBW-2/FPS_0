#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PlayerControl {
		private:
			std::shared_ptr<CharacterClass>	m_Chara{ nullptr };
			std::shared_ptr<VehicleClass>	m_Vehicle{ nullptr };
			float							m_Score{ 0.f };							//スコア
		public:
			PlayerControl(void) {
				this->m_Score = 0.f;
			}
			~PlayerControl(void) {
				this->Dispose();
			}
		public:
			void		SetChara(const std::shared_ptr<CharacterClass>& pChara) { m_Chara = pChara; }
			void		SetVehicle(const std::shared_ptr<VehicleClass>& pVehicle) { m_Vehicle = pVehicle; }
			void		AddScore(float value) { this->m_Score += value; }
			void		SubScore(float value) { this->m_Score -= value; }
			void		SetScore(float value) { this->m_Score = value; }
			auto&		GetChara(void) { return m_Chara; }
			auto&		GetVehicle(void) { return m_Vehicle; }
			const auto&	GetScore(void) const noexcept { return this->m_Score; }
			const auto	IsRide(void) const noexcept { return (bool)m_Vehicle; }

			const auto		GetNetSendMove(void) const noexcept {
				SendInfo ans;
				if (!IsRide()) {
					ans.m_Pos = m_Chara->GetMove().pos;
					ans.m_Vec = m_Chara->GetMove().vec;
					ans.m_Vec.y(0);
					ans.m_rad = m_Chara->GetRadBuf();
					ans.m_Damage = &m_Chara->GetDamageEvent();
					ans.m_DamageSwitch = m_Chara->GetDamageSwitch();
				}
				else {
					ans.m_Pos = m_Vehicle->GetMove().pos;
					ans.m_Vec = m_Vehicle->GetMove().vec;
					ans.m_rad.y(m_Vehicle->Get_body_yrad());
					ans.m_Damage = &m_Vehicle->GetDamageEvent();
					ans.m_DamageSwitch = (m_Vehicle->GetDamageSwitch() ? 1 : 0);
				}
				return ans;
			}

			const auto		GetPos(void) const noexcept { return ((!IsRide()) ? m_Chara->GetMatrix().pos() : m_Vehicle->GetMatrix().pos()); }
			const auto		GetAim(void) const noexcept { return ((!IsRide()) ? m_Chara->GetCharaDir().zvec()*-1.f : m_Vehicle->GetLookVec().zvec()*-1.f); }
			const auto		GetRadBuf(void) const noexcept { return ((!IsRide()) ? m_Chara->GetRadBuf() : m_Vehicle->GetViewRad()); }
		public:
			void Init(void) {
				m_Chara = nullptr;
				m_Vehicle = nullptr;
			}

			void Dispose(void) {
				m_Chara = nullptr;
				m_Vehicle = nullptr;
			}
		};

		class PlayerManager {
			std::vector<PlayerControl> m_Player;
		private:
			//シングルトン化
#if true

		private:
			static inline  PlayerManager*	m_Singleton = nullptr;
		public:
			static void Create(void) {
				if (m_Singleton == nullptr) {
					m_Singleton = new PlayerManager();
				}
			}
			static PlayerManager* Instance(void) {
				if (m_Singleton == nullptr) {
					m_Singleton = new PlayerManager();
				}
				return m_Singleton;
			}
		private:

#endif
		public:
			auto&		GetPlayer(int ID) { return m_Player[ID]; }
		public:
			void Init(int playerNum) {
				m_Player.resize(playerNum);
			}
			void Dispose(void) {
				m_Player.clear();
			}
		};

	};
};
