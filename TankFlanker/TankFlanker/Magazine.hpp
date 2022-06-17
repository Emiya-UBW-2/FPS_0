#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MagazineClass : public ObjectBaseClass {
			class CartClass {
			public:
				bool isActive{ false };
				moves move;
				MV1 obj;
			public:
				void Init(const MV1& baseObj) noexcept {
					isActive = true;
					this->obj = baseObj.Duplicate();
				}

				void SetMatrix(const MATRIX_ref& value, const VECTOR_ref& pos) noexcept {
					this->move.mat = value;
					this->move.pos = pos;
				}
				void Execute(void) noexcept {
					if (isActive) {
						this->obj.SetMatrix(move.MatIn());
					}
				}
				void Draw(void) noexcept {
					if (isActive) {
						if (CheckCameraViewClip_Box(
							(move.pos + VECTOR_ref::vget(-1.f, -1.f, -1.f)).get(),
							(move.pos + VECTOR_ref::vget(1.f, 1.f, 1.f)).get()) == FALSE
							) {
							this->obj.DrawModel();
						}
					}
				}
			};
		private:
			MV1 Ammoobj;
			std::vector<CartClass> m_Cart;
			int m_Capacity;
			int m_CapacityMax;


			//e
			bool ChamberIntime{ false };
			MATRIX_ref ChamberMatrix;
			float ShotPer = 0.f;
		public://ƒQƒbƒ^[
			void SetMatrix(const MATRIX_ref& value) noexcept {
				this->obj.SetMatrix(value);
			}
			void SetChamberIntime(bool value) noexcept {
				ChamberIntime = value;
			}
			void SetChamberMatrix(const MATRIX_ref& value) noexcept {
				ChamberMatrix = value;
			}
		public:
			MagazineClass(void) noexcept {
				this->m_objType = ObjType::Magazine;
			}
			~MagazineClass(void) noexcept {

			}

			void Init(void) noexcept override {
				ObjectBaseClass::Init();

				MV1::Load(this->m_FilePath + "ammo.pmd", &Ammoobj);
				this->m_Capacity = (int)(this->obj.frame_num()) - 1;
				this->m_CapacityMax = this->m_Capacity;
				this->m_Cart.resize(this->m_Capacity);
				for (auto& b : this->m_Cart) {
					b.Init(Ammoobj);
				}
			}
			void Execute(void) noexcept override {
				if (ChamberIntime) {
					this->ShotPer = std::clamp(this->ShotPer + 5.f / FPS, 0.f, 1.f);
				}
				else {
					this->ShotPer = 0.f;
				}

				int i = 0;
				for (auto& b : this->m_Cart) {
					b.isActive = (i < this->m_Capacity - 1);
					if (i == 0) {
						b.SetMatrix(
							this->obj.GetFrameLocalWorldMatrix(1 + i).GetRot()
							,
							Leap(this->obj.frame(1 + i), this->ChamberMatrix.pos(), this->ShotPer)
						);
					}
					else {
						b.SetMatrix(this->obj.GetFrameLocalWorldMatrix(1 + i).GetRot(), this->obj.frame(1 + i));
					}
					b.Execute();
					i++;
				}
				//‹¤’Ê
				ObjectBaseClass::Execute();
			}

			void Draw(void) noexcept override {
				ObjectBaseClass::Draw();
				for (auto& b : this->m_Cart) {
					b.Draw();
				}
			}
		public:
			void SubAmmo(void) noexcept {
				this->m_Capacity--;
				if (this->m_Capacity <= 0) { this->m_Capacity = 0; }
			}
			bool IsEmpty(void) noexcept {
				return this->m_Capacity == 0;
			}
		};
	};
};
