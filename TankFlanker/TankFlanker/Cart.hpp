#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CartClass : public ObjectBaseClass {
		public:
			CartClass(void) noexcept { this->m_objType = ObjType::Cart; }
			~CartClass(void) noexcept { }
		public:
			void SetCartMatrix(const MATRIX_ref& value, const VECTOR_ref& pos) noexcept {
				this->m_move.mat = value;
				this->m_move.pos = pos;
				UpdateMove();
			}
		};
	};
};
