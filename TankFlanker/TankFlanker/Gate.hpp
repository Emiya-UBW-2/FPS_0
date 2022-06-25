#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GateClass : public ObjectBaseClass {
			bool m_setstart{ false };
		public:
			GateClass(void) noexcept { this->m_objType = ObjType::Gate; }
			~GateClass(void) noexcept {}
		public:
			void Init(void) noexcept override {
				ObjectBaseClass::Init();
				this->m_obj.get_anime(1).per = 1.f;
				this->m_obj.get_anime(1).GoStart();
				this->m_setstart = false;
			}
			void Execute(void) noexcept override {
				if (this->m_setstart) {
					SetAnimOnce(1, 1.f);
				}
				this->m_obj.work_anime();
			}
			void Draw(void) noexcept override {
				this->m_obj.DrawModel();
			}
			void DrawShadow(void) noexcept override {
				this->m_obj.DrawModel();
			}
			void SetStart(void) noexcept {
				if (!this->m_setstart) {
					this->m_setstart = true;
					auto SE = SoundPool::Instance();
					SE->Get((int)SoundEnum::GateOpen).Play_3D(0, GetMatrix().pos(), 12.5f*50.f);
				}
			}
		};
	};
};
