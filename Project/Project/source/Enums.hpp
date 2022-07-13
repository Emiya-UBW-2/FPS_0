#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	//’è”
	static const float Frame_Rate{ 60.f };

	enum class Effect {
		ef_fire,	//”­–C‰Š
		ef_reco,	//¬ŒûŒa’µ’e
		ef_smoke,	//e‚Ì‹OÕ
		ef_gndsmoke,//’n–Ê‚Ì‹OÕ
		ef_fire2,	//”­–C‰Š
		ef_hitblood,//ŒŒ‚µ‚Ô‚«
		ef_greexp,	//ƒOƒŒ”š”­
		ef_greexp2,	//ƒOƒŒ”š”­2
		effects,	//“Ç‚İ‚Ş
	};
	namespace Sceneclass {
		enum class scenes
		{
			NONE_SCENE,
			LOAD_SCENE,
			MAIN_LOOP
		};
		enum class ObjType {
			Human,
			Circle,
		};
		enum class SoundEnum {
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
	};
};
