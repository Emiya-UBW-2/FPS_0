#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ItemType :int {
			Track,
			Max,
		};
		static const char* ItemtypeName[(int)ItemType::Max] = {
			"Track",
		};

		class ItemData {
		private:
			std::string		m_path;
			std::string		m_name;
			ItemType		m_ItemType{ ItemType::Track };					//ダメージ
			std::string		m_Info;
		public://getter
			const auto&		GetPath(void) const noexcept { return this->m_path; }
			const auto&		GetName(void) const noexcept { return this->m_name; }
			const auto&		GetItemType(void) const noexcept { return this->m_ItemType; }
			const auto&		GetInfo(void) const noexcept { return this->m_Info; }
		public:
			void			Set(std::string path_) {
				this->m_path = path_;
				int mdata = FileRead_open((this->m_path + "data.txt").c_str(), FALSE);
				this->m_name = getparams::_str(mdata);
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
					if (LEFT == "ItemType") {
						for (int i = 0; i < (int)ItemType::Max; i++) {
							if (RIGHT == ItemtypeName[i]) {
								m_ItemType = (ItemType)i;
							}
						}
					}
					if (LEFT == "ItemInfo") {
						m_Info = RIGHT;
					}
				}
				FileRead_close(mdata);
			}
		};
	};
};
