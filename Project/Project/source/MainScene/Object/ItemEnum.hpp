#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ItemType :int {
			Track,
			Ammo,
			Max,
		};
		static const char* ItemtypeName[(int)ItemType::Max] = {
			"Ammo",
			"Track",
		};
	};
};
