#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MagazineClass : public ObjectBaseClass {
			class CartClass {
			public:
				bool isActive{ false };
				moves m_move;
				MV1 m_obj;
			public:
				void Init(const MV1& baseObj) noexcept {
					isActive = true;
					this->m_obj = baseObj.Duplicate();
				}

				void SetMatrix(const MATRIX_ref& value, const VECTOR_ref& pos) noexcept {
					this->m_move.mat = value;
					this->m_move.pos = pos;
				}
				void Execute(void) noexcept {
					if (isActive) {
						this->m_obj.SetMatrix(m_move.MatIn());
					}
				}
				void Draw(void) noexcept {
					if (isActive) {
						if (CheckCameraViewClip_Box(
							(m_move.pos + VECTOR_ref::vget(-1.f, -1.f, -1.f)).get(),
							(m_move.pos + VECTOR_ref::vget(1.f, 1.f, 1.f)).get()) == FALSE
							) {
							this->m_obj.DrawModel();
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
			//
			MATRIX_ref HandMatrix;
			float HandPer = 0.f;
		public://ƒQƒbƒ^[
			void SetMatrix(const MATRIX_ref& value) noexcept {
				this->m_obj.SetMatrix(value);
			}
			void SetHandMatrix(const MATRIX_ref& value, float pPer) noexcept {
				HandMatrix = value;
				HandPer = pPer;
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
				this->m_Capacity = (int)(this->m_obj.frame_num()) - 1;
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
					b.isActive = (i < this->m_Capacity);
					if (i == 0) {
						auto zvec = Leap(
							this->m_obj.GetFrameLocalWorldMatrix(1 + i).GetRot().zvec(),
							HandMatrix.zvec(),
							HandPer);


						b.SetMatrix(
							MATRIX_ref::RotVec2(VECTOR_ref::front(), zvec)
							,
							Leap(
								Leap(this->m_obj.frame(1 + i), this->ChamberMatrix.pos(), this->ShotPer),
								HandMatrix.pos(),
								HandPer)
						);
					}
					else {
						b.SetMatrix(this->m_obj.GetFrameLocalWorldMatrix(1 + i).GetRot(), this->m_obj.frame(1 + i));
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
			void AddAmmo(void) noexcept {
				this->m_Capacity++;
				if (this->m_Capacity >= this->m_CapacityMax) { this->m_Capacity = this->m_CapacityMax; }
			}
			bool IsEmpty(void) noexcept {
				return this->m_Capacity == 0;
			}
			bool IsFull(void) noexcept {
				return this->m_Capacity == this->m_CapacityMax;
			}

			const auto GetAmmoNum(void) noexcept { return this->m_Capacity; }
			const auto GetAmmoAll(void) noexcept { return  this->m_CapacityMax; }
		};
	};
};
