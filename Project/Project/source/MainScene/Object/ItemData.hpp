#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ItemData {
		private:
			std::string		m_path;
			std::string		m_name;
			ItemType		m_ItemType{ ItemType::Track };					//ダメージ
			std::string		m_Info;
			GraphHandle		m_SlotPic;
			int				m_xsize{ 1 };
			int				m_ysize{ 1 };
		public://getter
			const auto&		GetPath(void) const noexcept { return this->m_path; }
			const auto&		GetName(void) const noexcept { return this->m_name; }
			const auto&		GetItemType(void) const noexcept { return this->m_ItemType; }
			const auto&		GetInfo(void) const noexcept { return this->m_Info; }
			const auto&		GetSlotPic(void) const noexcept { return this->m_SlotPic; }
			const auto&		GetXsize(void) const noexcept { return this->m_xsize; }
			const auto&		GetYsize(void) const noexcept { return this->m_ysize; }
		protected:
			virtual void	Set_Sub(const std::string&, const std::string&) noexcept {}
		public:
			void			Set(std::string path_) {
				this->m_path = path_;
				m_SlotPic = GraphHandle::Load(this->m_path + "pic.png");
				m_SlotPic.GetSize(&m_xsize, &m_ysize);
				m_xsize /= 64;
				m_ysize /= 64;

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
					Set_Sub(LEFT, RIGHT);
				}
				FileRead_close(mdata);
			}
		};

		class CellItem {
			std::shared_ptr<ItemData> Data;
			bool Is90{ false };
		public:
			const auto& GetItemData(void) const noexcept { return this->Data; }
			const auto&	GetXsize(void) const noexcept { return this->Is90 ? this->Data->GetYsize() : this->Data->GetXsize(); }
			const auto&	GetYsize(void) const noexcept { return this->Is90 ? this->Data->GetXsize() : this->Data->GetYsize(); }
			const auto& GetIs90(void) const noexcept { return this->Is90; }
			void Rotate() { this->Is90 ^= 1; }
		public:
			void Set(const std::shared_ptr<ItemData>& data) { this->Data = data; }
			void Dispose() {
				this->Data.reset();
			}
			void Draw(int xp, int yp) {
				this->Data->GetSlotPic().DrawRotaGraph(xp + GetXsize()*y_r(64) / 2, yp + GetYsize()*y_r(64) / 2, (float)y_r(64) / 64.f, deg2rad(this->Is90 ? 90.f : 0.f), false);
			}
		};
	};
};
