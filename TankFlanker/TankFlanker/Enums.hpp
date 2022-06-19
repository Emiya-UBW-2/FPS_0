#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	//定数
	static const float Frame_Rate{ 60.f };

	enum class Effect {
		ef_fire,	//発砲炎
		ef_reco,	//小口径跳弾
		ef_smoke,	//銃の軌跡
		ef_gndsmoke,//地面の軌跡
		ef_fire2,	//発砲炎
		ef_hitblood,//血しぶき
		ef_greexp,	//グレ爆発
		ef_greexp2,	//グレ爆発2
		effects,	//読み込む
	};
	namespace Sceneclass {
		enum class scenes
		{
			NONE_SCENE,
			LOAD_SCENE,
			MAIN_LOOP
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

			All_PronetoStand,

			Upper_ReloadStart,
			Upper_ReloadOne,
			Upper_ReloadEnd,

			All_Prone_ReloadStart,
			All_Prone_ReloadOne,
			All_Prone_ReloadEnd,

			AnimeIDMax,
		};
		enum class ObjType {
			Human,
			Gun,
			Target,
			Magazine,
		};
		enum class CharaFrame {
			Center,
			Upper,
			Upper2,
			LeftEye,
			RightEye,
			LeftFoot,
			RightFoot,
			RightWrist,
			RightHandJoint,
			RightThumb,
			RightPointer,
			Max,
		};
		const char* CharaFrameName[(int)CharaFrame::Max] = {
			"センター",
			"上半身",
			"上半身2",
			"左目先",
			"右目先",
			"左足首",
			"右足首",
			"右手首",
			"右ダミー",
			"右人指３",
			"右親指２",
		};
		enum class CharaShape {
			None,
			A,
			I,
			U,
			E,
			O,
			MAZIME,
			EYECLOSE,
			Max,
		};
		const char* CharaShapeName[(int)CharaShape::Max] = {
			"",
			"あ",
			"い",
			"う",
			"え",
			"お",
			"真面目",
			"まばたき",
		};
	};
};
