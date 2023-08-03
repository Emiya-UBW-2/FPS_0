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
			Movie,
		};
		enum class SoundEnum {
			Shot2,
			RunFoot,
			Engine,
			Propeller,
			Env,

			EngineStart,
			Engine2,
			Propeller2,

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
