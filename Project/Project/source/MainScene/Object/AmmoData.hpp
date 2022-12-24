#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class AmmoData {
		private:
			std::string		m_name;
			std::string		m_path;
			float			m_caliber{ 0.f };
			float			m_speed{ 100.f };				//�e��
			float			m_penetration{ 10.f };			//�ђ�
			HitPoint		m_damage{ 10 };					//�_���[�W
		public://getter
			const auto&		GetName(void) const noexcept { return this->m_name; }
			const auto&		GetPath(void) const noexcept { return this->m_path; }
			const auto&		GetCaliber(void) const noexcept { return this->m_caliber; }
			const auto&		GetSpeed(void) const noexcept { return this->m_speed; }
			const auto&		GetPenetration(void) const noexcept { return this->m_penetration; }
			const auto&		GetDamage(void) const noexcept { return this->m_damage; }
		public:
			void			Set(std::string path_, std::string named) {
				this->m_name = named;
				this->m_path = path_ + named + "/";
				int mdata = FileRead_open((this->m_path + "data.txt").c_str(), FALSE);
				this->m_caliber = getparams::_float(mdata) * 0.001f;	//���a
				this->m_speed = getparams::_float(mdata);				//�e��
				this->m_penetration = getparams::_float(mdata);			//�ђ�
				this->m_damage = (HitPoint)getparams::_int(mdata);		//�_���[�W
				FileRead_close(mdata);
			}
		};
	};
};
