#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CircleClass : public ObjectBaseClass {
		public:
			CircleClass(void) noexcept { this->m_objType = ObjType::Circle; }
			~CircleClass(void) noexcept {}
		public:
			void Execute(void) noexcept override {
				auto p = 0.5f*12.5f / 100.f;
				this->m_obj.SetMatrix(MATRIX_ref::GetScale(VECTOR_ref::vget(p, 1.f, p))*this->m_move.MatIn());
			}
			//[“x’l•`‰æ
			void Depth_Draw(void) noexcept override {
				auto q = (((VECTOR_ref)GetCameraPosition() - this->m_move.pos).size()) / 12.5f - 2.f;
				MV1SetMeshDifColorScale(this->m_obj.get(), 0, GetColorF(1.f, 1.f, 1.f, std::clamp(q, 0.f, 1.f)));
				this->m_obj.DrawModel();
			}
			void Draw(void) noexcept override {
				//ObjectBaseClass::Draw();
			}
			void DrawShadow(void) noexcept override {
				//ObjectBaseClass::Draw();
			}
		};
	};
};
