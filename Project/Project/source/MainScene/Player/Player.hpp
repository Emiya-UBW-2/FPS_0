#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PlayerControl {
		private:
			std::shared_ptr<VehicleClass>										m_Vehicle{ nullptr };
			std::vector<std::vector<std::vector<std::shared_ptr<CellItem>>>>	m_Inventorys;
			float							m_Score{ 0.f };							//スコア
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

			auto&		GetInventorys(void) noexcept { return this->m_Inventorys; }

			const auto		GetNetSendMove(void) const noexcept {
				SendInfo ans;
				if (IsRide()) {
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
		private:
			void SetInventory(int ID, int x, int y) noexcept {
				this->m_Inventorys[ID].resize(x);
				for (auto& xp : this->m_Inventorys[ID]) { xp.resize(y); }
			}
		public:
			bool CanPutInventory(int ID, int xp, int yp, int xsize, int ysize, const std::shared_ptr<ItemData>* Drag, const std::shared_ptr<CellItem>* DragIn = nullptr) noexcept {
				//自機の履帯以外は乗らない
				if (Drag) {
					if (ID == 2 || ID == 3) {
						if ((*Drag) != m_Vehicle->GetTrackPtr()) {
							return false;
						}
					}
				}
				//
				if (DragIn && *DragIn == this->m_Inventorys[ID][xp][yp]) {}//該当部分がInで埋まっている
				else if (!this->m_Inventorys[ID][xp][yp].get()) {}//該当部分が空
				else { return false; }//該当部分が何かで埋まってる
				//該当部の1マスが空だとして、その上にアイテムがないか
				for (int x = 0; x <= xp; x++) {
					for (int y = 0; y <= yp; y++) {
						auto& yo = this->m_Inventorys.at(ID).at(x).at(y);
						if (yo.get() && (&yo != DragIn) && !(xp == x && yp == y)) {
							if (yo->GetXsize() > (xp - x) && yo->GetYsize() > (yp - y)) {
								return false;
							}
						}
					}
				}
				//指示サイズのものが置けるかチェック
				if (xsize > 1 || ysize > 1) {
					auto xLimit = (int)this->m_Inventorys[ID].size();
					auto yLimit = (int)this->m_Inventorys[ID][xp].size();
					if (xp + xsize > xLimit || yp + ysize > yLimit) {
						return false;//マス越え
					}
					for (int x = xp; x < xp + xsize; x++) {
						for (int y = yp; y < yp + ysize; y++) {
							if (!CanPutInventory(ID, x, y, 1, 1, Drag, DragIn)) {
								return false;
							}
						}
					}
				}
				return true;
			}
			void PutInventory(int ID, int x, int y, const std::shared_ptr<ItemData>& data, int cap, bool Is90) noexcept {
				this->m_Inventorys.at(ID).at(x).at(y) = std::make_shared<CellItem>();
				this->m_Inventorys.at(ID).at(x).at(y)->Set(data, cap, ID);
				if (Is90) {
					this->m_Inventorys.at(ID).at(x).at(y)->Rotate();
				}
			}
			void DeleteInventory(int ID, int x, int y) noexcept {
				this->m_Inventorys.at(ID).at(x).at(y).reset();
			}
			void DeleteInventory(const std::shared_ptr<CellItem>& tgt) noexcept {
				for (auto& I : this->m_Inventorys) {
					for (auto& xo : I) {
						for (auto& yo : xo) {
							if (yo == tgt) {
								yo.reset();
								return;
							}
						}
					}
				}
			}
			const std::shared_ptr<CellItem>* GetInventory(int ID, int xp, int yp) const noexcept {
				if (this->m_Inventorys[ID][xp][yp].get() == nullptr) {
					for (int x = 0; x <= xp; x++) {
						for (int y = 0; y <= yp; y++) {
							auto& yo = this->m_Inventorys[ID][x][y];
							if (yo.get() && !(xp == x && yp == y)) {
								if (yo->GetXsize() > (xp - x) && yo->GetYsize() > (yp - y)) {
									return &yo;
								}
							}
						}
					}
					return nullptr;
				}
				else {
					return &this->m_Inventorys[ID][xp][yp];
				}
			}
			const std::shared_ptr<CellItem>* GetInventory(int ID, std::function<bool(const std::shared_ptr<CellItem>&)> Check) const noexcept {
				for (auto& xo : this->m_Inventorys[ID]) {
					for (auto& yo : xo) {
						if (yo.get()) {
							if (Check(yo)) {
								return &yo;
							}
						}
					}
				}
				return nullptr;
			}
		public:
			void Init(void) noexcept {
				this->m_Vehicle = nullptr;
				this->m_Inventorys.resize(5);
				SetInventory(0, 10, 6);
				SetInventory(1, 5, 9);
				SetInventory(2, 2, 10);
				SetInventory(3, 2, 10);
				SetInventory(4, 10, 15);
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
				for (auto& p : this->m_Player) {
					p.Init();
				}
			}
			void Dispose(void) noexcept {
				for (auto& p : this->m_Player) {
					p.Dispose();
				}
				this->m_Player.clear();
			}
		};

	};
};
