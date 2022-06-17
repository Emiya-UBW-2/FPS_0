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
				void Init(const MV1& baseObj) {
					isActive = true;
					this->obj = baseObj.Duplicate();
				}

				void SetMatrix(const MATRIX_ref& value, const VECTOR_ref& pos) {
					this->move.mat = value;
					this->move.pos = pos;
				}
				void Execute() {
					if (isActive) {
						this->obj.SetMatrix(move.MatIn());
					}
				}
				void Draw() {
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
			void SetMatrix(const MATRIX_ref& value) {
				this->obj.SetMatrix(value);
			}
			void SetChamberIntime(bool value) {
				ChamberIntime = value;
			}
			void SetChamberMatrix(const MATRIX_ref& value) {
				ChamberMatrix = value;
			}
		public:
			MagazineClass() {
				m_objType = ObjType::Magazine;
			}
			~MagazineClass() {

			}

			void Init() override {
				ObjectBaseClass::Init();

				MV1::Load(this->m_FilePath + "ammo.pmd", &Ammoobj);
				m_Capacity = this->obj.frame_num() - 1;
				m_CapacityMax = m_Capacity;
				m_Cart.resize(m_Capacity);
				for (auto& b : m_Cart) {
					b.Init(Ammoobj);
				}
			}
			void Execute() override {
				if (ChamberIntime) {
					this->ShotPer = std::clamp(this->ShotPer + 5.f / FPS, 0.f, 1.f);
				}
				else {
					this->ShotPer = 0.f;
				}

				int i = 0;
				for (auto& b : m_Cart) {
					b.isActive = (i < m_Capacity - 1);
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

			void Draw() override {
				ObjectBaseClass::Draw();
				for (auto& b : m_Cart) {
					b.Draw();
				}
			}
		public:
			void SubAmmo() {
				m_Capacity--;
				if (m_Capacity <= 0) { m_Capacity = 0; }
			}
			bool IsEmpty() {
				return m_Capacity == 0;
			}
		};
	};
};
