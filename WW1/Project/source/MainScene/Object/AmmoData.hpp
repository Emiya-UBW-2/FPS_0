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
					//コメントアウト
					if (ALL.find("//") != std::string::npos) {
						ALL = ALL.substr(0, ALL.find("//"));
					}
					//
					if (ALL == "") { continue; }
					auto LEFT = getparams::getleft(ALL);
					auto RIGHT = getparams::getright(ALL);
					//アイテムデータ読みとり
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
			float			m_speed{ 100.f };				//弾速
			float			m_penetration{ 10.f };			//貫通
			HitPoint		m_damage{ 10 };					//ダメージ
		public://getter
			const auto&		GetCaliber(void) const noexcept { return this->m_caliber; }
			const auto&		GetSpeed(void) const noexcept { return this->m_speed; }
			const auto&		GetPenetration(void) const noexcept { return this->m_penetration; }
			const auto&		GetDamage(void) const noexcept { return this->m_damage; }
		protected:
			void		Set_Sub(const std::string& LEFT, const std::string&RIGHT) noexcept override {
				if (LEFT == "ammo_cal(mm)") {
					this->m_caliber = std::stof(RIGHT)* 0.001f;	//口径
				}
				if (LEFT == "ammo_speed(m/s)") {
					this->m_speed = std::stof(RIGHT);				//弾速
				}
				if (LEFT == "ammo_pene(mm)") {
					this->m_penetration = std::stof(RIGHT);			//貫通
				}
				if (LEFT == "ammo_damage") {
					this->m_damage = (HitPoint)std::stoi(RIGHT);		//ダメージ
				}
			}
		public:
		};
	};
};
