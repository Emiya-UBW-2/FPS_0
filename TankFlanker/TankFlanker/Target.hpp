
#pragma once

#include"Header.hpp"
namespace FPS_n2 {
	namespace Sceneclass {
		class TargetClass : public ObjectBaseClass {
		public:
			void Init() override {
				ObjectBaseClass::Init();

				this->move.mat = MATRIX_ref::RotY(deg2rad(90));
				this->move.pos = VECTOR_ref::vget(732.f, 15.11f, -974.20f);
				this->obj.SetMatrix(this->move.MatIn());
			}
			void Execute() {
			}
			void Draw() override {
				ObjectBaseClass::Draw();
			}
		public:
		};
	};
};
