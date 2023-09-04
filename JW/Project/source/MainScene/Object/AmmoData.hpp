#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ItemData {
		private:
			std::string		m_path;
			std::string		m_name;
		public://getter
			const auto&		GetPath(void) const noexcept { return this->m_path; }
			const auto&		GetName(void) const noexcept { return this->m_name; }
		protected:
			virtual void	Set_Sub(const std::string&, const std::string&) noexcept {}
		public:
			void			Set(std::string path_) {
				this->m_path = path_;
				int mdata = FileRead_open((this->m_path + "data.txt").c_str(), FALSE);
				while (true) {
					if (FileRead_eof(mdata) != 0) { break; }
					auto ALL = getparams::Getstr(mdata);
					//�R�����g�A�E�g
					if (ALL.find("//") != std::string::npos) {
						ALL = ALL.substr(0, ALL.find("//"));
					}
					//
					if (ALL == "") { continue; }
					auto LEFT = getparams::getleft(ALL);
					auto RIGHT = getparams::getright(ALL);
					//�A�C�e���f�[�^�ǂ݂Ƃ�
					if (LEFT == "Name") {
						this->m_name = RIGHT;
					}
					Set_Sub(LEFT, RIGHT);
				}
				FileRead_close(mdata);
			}
		};


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
