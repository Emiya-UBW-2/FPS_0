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

			int				m_Capacity{ 1 };
		public://getter
			const auto&		GetPath(void) const noexcept { return this->m_path; }
			const auto&		GetName(void) const noexcept { return this->m_name; }
			const auto&		GetItemType(void) const noexcept { return this->m_ItemType; }
			const auto&		GetInfo(void) const noexcept { return this->m_Info; }
			const auto&		GetSlotPic(void) const noexcept { return this->m_SlotPic; }
			const auto&		GetXsize(void) const noexcept { return this->m_xsize; }
			const auto&		GetYsize(void) const noexcept { return this->m_ysize; }
			const auto&		GetCapacity(void) const noexcept { return this->m_Capacity; }
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
					if (LEFT == "ItemCapacity") {
						m_Capacity = std::stoi(RIGHT);
					}
					Set_Sub(LEFT, RIGHT);
				}
				FileRead_close(mdata);
			}
		};

		class CellItem {
			int							m_Count{ 1 };
			std::shared_ptr<ItemData>	Data{ nullptr };
			bool						Is90{ false };
			int							SlotID{ 0 };
		public:
			const auto& GetItemData(void) const noexcept { return this->Data; }
			const auto&	GetXsize(void) const noexcept { return this->Is90 ? this->Data->GetYsize() : this->Data->GetXsize(); }
			const auto&	GetYsize(void) const noexcept { return this->Is90 ? this->Data->GetXsize() : this->Data->GetYsize(); }
			const auto&	GetCount(void) const noexcept { return this->m_Count; }
			const auto& GetIs90(void) const noexcept { return this->Is90; }
			const auto& GetSlotID(void) const noexcept { return this->SlotID; }
			void Rotate() { this->Is90 ^= 1; }
		public:
			void Set(const std::shared_ptr<ItemData>& data, int cap, int Slot) {
				this->Data = data;
				if (cap < 0) {
					this->m_Count = this->Data->GetCapacity();
				}
				else {
					this->m_Count = cap;
				}
				SlotID = Slot;
			}
			bool Sub(HitPoint* value) {
				auto prev = this->m_Count;
				this->m_Count = std::max(this->m_Count - *value, 0);
				*value -= (HitPoint)(prev - this->m_Count);
				return (prev != this->m_Count && this->m_Count == 0);//0になったとき
			}

			void Dispose() {
				this->Data.reset();
			}
			void Draw(int xp, int yp) {
				auto* Fonts = FontPool::Instance();
				auto Red = GetColor(255, 0, 0);
				auto White = GetColor(255, 255, 255);
				auto Black = GetColor(0, 0, 0);

				this->Data->GetSlotPic().DrawRotaGraph(xp + GetXsize()*y_r(64) / 2, yp + GetYsize()*y_r(64) / 2, (float)y_r(64) / 64.f, deg2rad(this->Is90 ? 90.f : 0.f), false);
				Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(12), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM,
					xp + GetXsize()*y_r(64), yp + GetYsize()*y_r(64),
					((100 * this->m_Count / this->Data->GetCapacity()) > 34) ? White : Red, Black, "%d/%d", this->m_Count, this->Data->GetCapacity());
			}
		};
	};
};
