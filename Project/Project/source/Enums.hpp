#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//íËêî
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };

	namespace Sceneclass {
		enum class ObjType {
			Human,
			Vehicle,
			Ammo,
			Gun,
			Magazine,
			Cart,
		};
		enum class SoundEnum {
			Environment,

			Tank_Shot,
			Tank_Ricochet,
			Tank_Damage,
			Tank_engine,
			Tank_Eject,
			Tank_Reload,

			Shot_Gun,
			Trigger,
			Cocking0,
			Cocking1,
			Cocking2,
			Cocking3,
			RunFoot,
			SlideFoot,
			StandupFoot,
			Heart,
			GateOpen,
			Num,
		};
		enum class CharaTypeID {
			Mine,
			Team,
			Enemy,
		};
	};

	typedef char PlayerID;
	typedef short HitPoint;
};
