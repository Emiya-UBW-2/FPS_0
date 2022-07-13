#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ShootingMatClass : public ObjectBaseClass {
		public:
			ShootingMatClass(void) noexcept { this->m_objType = ObjType::ShootingMat; }
			~ShootingMatClass(void) noexcept {}
		public:
		};
	};
};
