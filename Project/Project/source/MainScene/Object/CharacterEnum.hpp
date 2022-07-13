#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class CharaAnimeID : int {
			Bottom_Stand,				//0
			Bottom_Walk,				//1
			Bottom_WalkBack,			//2
			Bottom_LeftStep,			//3
			Bottom_RightStep,			//4
			Bottom_Turn,				//5
			Bottom_Run,					//6

			Upper_Normal,				//7
			Upper_Running,				//8

			Bottom_Flight,				//9
			Upper_Flight,				//10

			AnimeIDMax,
		};
		enum class CharaFrame {
			Center,
			Upper,
			Upper2,
			Head,
			LeftEye,
			RightEye,
			LeftFoot,
			RightFoot,
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
		const char* CharaFrameName[(int)CharaFrame::Max] = {
			"センター",
			"上半身",
			"上半身2",
			"頭",
			"左目先",
			"右目先",
			"左足首",
			"右足首",
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
