#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CartClass : public ObjectBaseClass {
		public:
			CartClass(void) noexcept { this->m_objType = ObjType::Cart; }
			~CartClass(void) noexcept { }
		};
	};
};
