#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	//íËêî
	static const float Frame_Rate{ 60.f };

	static const float Scale_Rate{ 12.5f };

	namespace Sceneclass {
		enum class scenes
		{
			NONE_SCENE,
			LOAD_SCENE,
			MAIN_LOOP
		};
		enum class ObjType {
			Plane,
			Ammo,
		};
		enum class SoundEnum {
			Trigger,
			Cocking1_0,
			Cocking1_1,
			Cocking1_2,
			Cocking1_3,

			Cocking2_0,
			Cocking2_1,
			Shot2,
			Unload2,
			Load2,

			Cocking3_0,
			Cocking3_1,
			Shot3,
			Unload3,
			Load3,

			RunFoot,
			SlideFoot,
			StandupFoot,
			Heart,
			Switch,
			GateOpen,

			Engine,
			Propeller,

			Env,

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
