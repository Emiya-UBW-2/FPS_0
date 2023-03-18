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
			bool CanPutInventory(int ID, int xp, int yp, const std::shared_ptr<ItemData>& data) noexcept {
				if (this->m_Inventorys[ID][xp][yp].get() == nullptr) {
					for (int x = -3; x <= -1; x++) {
						if (0 <= (xp + x)) {
							if (this->m_Inventorys[ID][xp + x][yp].get() != nullptr) {
								if (this->m_Inventorys[ID][xp + x][yp]->GetXsize() + x > 0) {
									return false;
								}
							}
						}
					}
					for (int y = -3; y <= -1; y++) {
						if (0 <= (yp + y)) {
							if (this->m_Inventorys[ID][xp][yp + y].get() != nullptr) {
								if (this->m_Inventorys[ID][xp][yp + y]->GetXsize() + y - 1 > 0) {
									return false;
								}
							}
						}
					}
					//置けるかチェック
					{
						int xsize{ 1 };
						int ysize{ 1 };
						data->GetSlotPic().GetSize(&xsize, &ysize);
						xsize /= 64;
						ysize /= 64;
						if ((xp + xsize - 1 < this->m_Inventorys[ID].size()) && (yp + ysize - 1 < this->m_Inventorys[ID][xp].size())) {
							for (int x = 1; x <= xsize - 1; x++) {
								if ((xp + x) < this->m_Inventorys[ID].size()) {
									if (this->m_Inventorys[ID][xp + x][yp].get() != nullptr) {
										return false;
									}
								}
							}
							for (int y = 1; y <= ysize - 1; y++) {
								if ((yp + y) < this->m_Inventorys[ID][xp].size()) {
									if (this->m_Inventorys[ID][xp][yp + y].get() != nullptr) {
										return false;
									}
								}
							}
							return true;
						}
						return false;
					}
				}
				else {
					return false;
				}
			}
			void PutInventory(int ID, int x, int y, const std::shared_ptr<ItemData>& data) noexcept {
				this->m_Inventorys[ID][x][y] = std::make_shared<CellItem>();
				this->m_Inventorys[ID][x][y]->Set(data);
			}
			void DeleteInventory(int ID, int x, int y) noexcept {
				this->m_Inventorys[ID][x][y].reset();
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
		public:
			void Init(void) noexcept {
				this->m_Vehicle = nullptr;
				this->m_Inventorys.resize(5);
				SetInventory(0, 10, 5);
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
