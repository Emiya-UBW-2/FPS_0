#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class GunAnimeID : int {
			Cocking,
			ReloadStart,
			ReloadOne,
			ReloadEnd,
			Shot,
			AnimeIDMax,
		};
		enum class GunFrame {
			Center,
			Cart,
			CartVec,
			Muzzle,
			Eyepos,
			Lens,
			LensSize,
			Magpos,
			LeftHandPos,
			LeftHandYvec,
			LeftHandZvec,
			MoveAxis,
			MovePoint,
			Max,
		};
		static const char* GunFrameName[(int)GunFrame::Max] = {
			"�Z���^�[",
			"cart",
			"cartvec",
			"muzzle",
			"EyePos",
			"lens",
			"lenssize",
			"magpos",
			"lefthand",
			"lefthand_yvec",
			"lefthand_zvec",
			"���쎲",
			"����_",
		};
		enum class SHOTTYPE {
			BOLT,
			SEMI,
			FULL,
		};
		enum class RELOADTYPE {
			AMMO,
			MAG,
		};
	};
};
