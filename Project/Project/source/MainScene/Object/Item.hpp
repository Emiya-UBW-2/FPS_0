#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ItemClass : public ObjectBaseClass {
		private:
			std::vector<std::shared_ptr<VehicleClass>>* vehicle_Pool{ nullptr };

			int							m_Count{ 1 };
			std::shared_ptr<ItemData>	m_ItemData{ nullptr };
		public://getter
			const auto& GetItemData(void) const noexcept { return this->m_ItemData; }
		public: //コンストラクタ、デストラクタ
			ItemClass(void) noexcept { this->m_objType = ObjType::Item; }
			~ItemClass(void) noexcept {}
		public: //継承
			void SetVehPool(std::vector<std::shared_ptr<VehicleClass>>* vehiclePool_t) noexcept { vehicle_Pool = vehiclePool_t; }
			void SetData(const std::shared_ptr<ItemData>& pItemData, int cap) noexcept {
				this->m_ItemData = pItemData;
				if (cap < 0) {
					this->m_Count = this->m_ItemData->GetCapacity();
				}
				else {
					this->m_Count = cap;
				}
			}
			const auto&		GetCount(void) const noexcept { return this->m_Count; }
			//
			void DrawItemData(int xp1, int yp1, int xp2, int yp2) noexcept {
				auto* Fonts = FontPool::Instance();
				if (this->m_ItemData) {
					DrawBox(xp1, yp1, xp2, yp2, GetColor(128, 128, 128), TRUE);
					Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::MIDDLE,
						xp1, yp1 + (yp2 - yp1) / 2, GetColor(255, 255, 255), GetColor(0, 0, 0), "%s", this->m_ItemData->GetName().c_str());
				}
			}
			//
			void			Init() noexcept override {
				ObjectBaseClass::Init();
				SetUseRealTimePhysics(false);
				SetActive(false);
			}
			//
			void			FirstExecute(void) noexcept override {
				this->m_move.repos = this->m_move.pos;
				this->m_move.Update_Physics(0.f, 1.f);
				//AmmoClass
				VECTOR_ref repos_tmp = this->m_move.repos + VECTOR_ref::up()*Scale_Rate*2.f;
				VECTOR_ref pos_tmp = this->m_move.pos;
				VECTOR_ref norm_tmp;
				bool ColRes = this->m_BackGround->CheckLinetoMap(repos_tmp, &pos_tmp, true, false, &norm_tmp);
				bool ColVeh = false;
				for (auto& tgt : *vehicle_Pool) {
					if (tgt->CheckLine(repos_tmp, &pos_tmp, &norm_tmp)) {
						ColVeh |= true;
						this->m_move.vec = tgt->Getvec_real()*4.5f;
						this->m_move.vec.xadd(GetRandf(2.5f*Scale_Rate / FPS));
						this->m_move.vec.zadd(GetRandf(2.5f*Scale_Rate / FPS));

						this->m_move.mat *= MATRIX_ref::RotX(GetRandf(deg2rad(90)));
						this->m_move.mat *= MATRIX_ref::RotY(GetRandf(deg2rad(90)));
						this->m_move.mat *= MATRIX_ref::RotZ(GetRandf(deg2rad(90)));
					}
				}
				if (ColRes || ColVeh) {
					this->m_move.mat *= MATRIX_ref::RotVec2(this->m_move.mat.yvec(), Lerp(this->m_move.mat.yvec(), norm_tmp, std::powf(0.95f, 60.f / FPS)));
					this->m_move.pos = pos_tmp;
					this->m_move.vec.y(0.f);
				}
				if (!ColVeh) {
					auto recY = this->m_move.vec.y();
					Easing(&this->m_move.vec, VECTOR_ref::zero(), 0.95f, EasingType::OutExpo);
					this->m_move.vec.y(recY);
				}

				UpdateMove();
			}
			//
			void			DrawShadow(void) noexcept override {
				GetObj().DrawModel();
			}
			//
			void			Draw(void) noexcept override {
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-20, 0, -20)).get(),
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(20, 20, 20)).get()) == FALSE
						) {
						auto Length = (this->GetObj().GetMatrix().pos() - DxLib::GetCameraPosition()).Length();
						if (Length < 50.f*Scale_Rate) {
							this->GetObj().DrawModel();
						}
					}
				}
			}
			//
		};
	};
};
