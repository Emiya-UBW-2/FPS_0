#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class CharaAnimeID : int {
			Bottom_Stand,
			Bottom_Stand_Walk,
			Bottom_Stand_WalkBack,
			Bottom_Stand_LeftStep,
			Bottom_Stand_RightStep,
			Bottom_Stand_Turn,
			Bottom_Stand_Run,

			Bottom_Squat,
			Bottom_Squat_Walk,
			Bottom_Squat_WalkBack,
			Bottom_Squat_LeftStep,
			Bottom_Squat_RightStep,
			Bottom_Squat_Turn,//12

			//AR15
			Upper_Reload1Start,
			Upper_Reload1One,
			Upper_Reload1End,

			Upper_Down1,
			Upper_Ready1,
			Upper_ADS1,
			Upper_Cocking1,
			//ハンドガン
			Upper_Reload2Start,
			Upper_Reload2One,
			Upper_Reload2End,

			Upper_Down2,
			Upper_Ready2,
			Upper_ADS2,
			Upper_Cocking2,
			//AK
			Upper_Reload3Start,
			Upper_Reload3One,
			Upper_Reload3End,

			Upper_Down3,
			Upper_Ready3,
			Upper_ADS3,
			Upper_Cocking3,

			AnimeIDMax,
		};
		enum class CharaFrame {
			Center,
			Upper,
			Upper2,
			Head,
			LeftEye,
			RightEye,
			LeftFoot1,
			LeftFoot2,
			LeftFoot,
			RightFoot1,
			RightFoot2,
			RightFoot,
			RightArm,
			RightArm2,
			RightWrist,
			RightHandJoint,
			RightThumb,
			RightPointer,
			LeftArm,
			LeftArm2,
			LeftWrist,
			LeftHandJoint,
			Max,
		};
		static const char* CharaFrameName[(int)CharaFrame::Max] = {
			"センター",
			"上半身",
			"上半身2",
			"頭",
			"左目",
			"右目",
			"左足",
			"左ひざ",
			"左足首",
			"右足",
			"右ひざ",
			"右足首",
			"右腕",
			"右ひじ",
			"右手首",
			"右ダミー",
			"右人指３",
			"右親指２",
			"左腕",
			"左ひじ",
			"左手首",
			"左ダミー",
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
		static const char* CharaShapeName[(int)CharaShape::Max] = {
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
