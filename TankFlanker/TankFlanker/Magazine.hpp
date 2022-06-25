#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MagazineClass : public ObjectBaseClass {
		private:
			std::vector<std::shared_ptr<CartClass>> m_Cart;
			int m_Capacity{ 0 };
			int m_CapacityMax{ 0 };
			//e
			bool ChamberIntime{ false };
			MATRIX_ref ChamberMatrix;
			float ShotPer{ 0.f };
			//
			MATRIX_ref HandMatrix;
			float HandPer{ 0.f };
		public://ƒQƒbƒ^[
			void SetHandMatrix(const MATRIX_ref& value, float pPer) noexcept {
				this->HandMatrix = value;
				this->HandPer = pPer;
			}
			void SetChamberIntime(bool value) noexcept { this->ChamberIntime = value; }
			void SetChamberMatrix(const MATRIX_ref& value) noexcept { this->ChamberMatrix = value; }
			void SetAmmo(int value) noexcept { this->m_Capacity = std::clamp(value, 0, this->m_CapacityMax); }
			void SubAmmo(void) noexcept { SetAmmo(this->m_Capacity - 1); }
			void AddAmmo(void) noexcept { SetAmmo(this->m_Capacity + 1); }
			bool IsEmpty(void) noexcept { return this->m_Capacity == 0; }
			bool IsFull(void) noexcept { return this->m_Capacity == this->m_CapacityMax; }
			const auto GetAmmoNum(void) noexcept { return this->m_Capacity; }
			const auto GetAmmoAll(void) noexcept { return  this->m_CapacityMax; }
			void SetCartPtr(std::shared_ptr<CartClass>& pCartPtr) { this->m_Cart.emplace_back(pCartPtr); }
		public:
			MagazineClass(void) noexcept { this->m_objType = ObjType::Magazine; }
			~MagazineClass(void) noexcept { }
		public:
			void Init(void) noexcept override {
				ObjectBaseClass::Init();
				this->m_Capacity = (int)(this->m_obj.frame_num()) - 1;
				this->m_CapacityMax = this->m_Capacity;
			}
			void Execute(void) noexcept override {
				this->ShotPer = std::clamp(this->ShotPer + 5.f / FPS, 0.f, (this->ChamberIntime) ? 1.f : 0.f);
				for (auto& b : this->m_Cart) {
					int i = (int)(&b - &this->m_Cart.front());
					b->SetActive(i < this->m_Capacity);
					auto mat = this->m_obj.GetFrameLocalWorldMatrix(1 + i);
					if (i == 0) {
						b->SetMove(
							MATRIX_ref::RotVec2(VECTOR_ref::front(), Leap(mat.GetRot().zvec(), this->HandMatrix.zvec(), this->HandPer)),
							Leap(Leap(mat.pos(), this->ChamberMatrix.pos(), this->ShotPer), this->HandMatrix.pos(), this->HandPer)
						);
					}
					else {
						auto mat2 = this->m_obj.GetFrameLocalWorldMatrix(i);
						b->SetMove(mat.GetRot(), Leap(mat.pos(), mat2.pos(), this->HandPer));
					}
				}
				//‹¤’Ê
				ObjectBaseClass::Execute();
			}
		public:
		};
	};
};
