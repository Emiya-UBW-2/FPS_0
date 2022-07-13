#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GateClass : public ObjectBaseClass {
			bool m_setstart{ false };
			bool m_setRemove{ false };

			float m_OpacityRate{ 1.f };
		public:
			GateClass(void) noexcept { this->m_objType = ObjType::Gate; }
			~GateClass(void) noexcept {}
		public:
			void Init(void) noexcept override {
				ObjectBaseClass::Init();
				this->m_obj.get_anime(1).per = 1.f;
				this->m_obj.get_anime(1).GoStart();
				this->m_setstart = false;
				this->m_setRemove = false;
			}
			void Execute(void) noexcept override {
				if (this->m_setstart) {
					SetAnimOnce(1, 1.f);
				}
				this->m_obj.work_anime();
				if (this->m_setRemove) {
					this->m_move.pos += this->m_move.mat.xvec()*(10.f / FPS);
					UpdateMove();

					this->m_obj.SetOpacityRate(this->m_OpacityRate);
					this->m_OpacityRate = std::clamp(this->m_OpacityRate - 0.5f / FPS, 0.f, 1.f);
					if (this->m_OpacityRate == 0.f) {
						this->m_IsActive = false;
					}
				}
			}
			void Draw(void) noexcept override {
				if (this->m_IsActive) {
					this->m_obj.DrawModel();
				}
			}
			void DrawShadow(void) noexcept override {
				if (this->m_IsActive) {
					this->m_obj.DrawModel();
				}
			}
			void SetStart(void) noexcept {
				if (!this->m_setstart) {
					this->m_setstart = true;
					auto SE = SoundPool::Instance();
					SE->Get((int)SoundEnum::GateOpen).Play_3D(0, GetMatrix().pos(), 12.5f*50.f);
				}
			}
			void SetRemove(void) noexcept {
				if (!this->m_setRemove) {
					this->m_setRemove = true;
					//auto SE = SoundPool::Instance();
					//SE->Get((int)SoundEnum::GateOpen).Play_3D(0, GetMatrix().pos(), 12.5f*50.f);
				}
			}
		};
	};
};
