#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void MagazineClass::SetCartPtr(void) {
			auto* ObjMngr = ObjectManager::Instance();
			for (auto& b : this->m_Cart) {
				b = ((std::shared_ptr<CartClass>&)(*ObjMngr->AddObject(ObjType::Cart, GetAmmoSpec()->GetPath().c_str(), "ammo")));
			}
		}
	};
};
