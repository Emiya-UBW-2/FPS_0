#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class ItemType :int {
			Track,
			Ammo,
			Fuel,
			Max,
		};
		static const char* ItemtypeName[(int)ItemType::Max] = {
			"Ammo",
			"Track",
			"Fuel",
		};
	};
};
