#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	//’è”
	static const float Frame_Rate{ 90.f };
	static const size_t max_bullet{ 64 };

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
	};

	enum class CharaAnimeID : int {
		RightHand,

		Upper_Down,
		Upper_Aim,
		Upper_Shot,
		Upper_Cocking,
		Upper_RunningStart,
		Upper_Running,
		Upper_Sprint,

		Mid_Squat,

		Bottom_Stand,
		Bottom_Walk,
		Bottom_WalkBack,
		Bottom_LeftStep,
		Bottom_RightStep,
		Bottom_Turn,
		Bottom_Run,

		All_Prone,
		All_ProneShot,
		All_ProneCocking,

		All_ProneWalk,

		AnimeIDMax,
	};

}
