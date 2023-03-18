#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class AmmoData : public ItemData {
		private:
			float			m_caliber{ 0.f };
			float			m_speed{ 100.f };				//�e��
			float			m_penetration{ 10.f };			//�ђ�
			HitPoint		m_damage{ 10 };					//�_���[�W
		public://getter
			const auto&		GetCaliber(void) const noexcept { return this->m_caliber; }
			const auto&		GetSpeed(void) const noexcept { return this->m_speed; }
			const auto&		GetPenetration(void) const noexcept { return this->m_penetration; }
			const auto&		GetDamage(void) const noexcept { return this->m_damage; }
		protected:
			void		Set_Sub(const std::string& LEFT, const std::string&RIGHT) noexcept override {
				if (LEFT == "ammo_cal(mm)") {
					this->m_caliber = std::stof(RIGHT)* 0.001f;	//���a
				}
				if (LEFT == "ammo_speed(m/s)") {
					this->m_speed = std::stof(RIGHT);				//�e��
				}
				if (LEFT == "ammo_pene(mm)") {
					this->m_penetration = std::stof(RIGHT);			//�ђ�
				}
				if (LEFT == "ammo_damage") {
					this->m_damage = (HitPoint)std::stoi(RIGHT);		//�_���[�W
				}
			}
		public:
		};
	};
};
