#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class CharaAnimeID : int {
			AnimeIDMax,
		};
		enum class CharaFrame {
			Propeller,
			Yaw,
			RollLeft,
			RollRight,
			Pitch,
			Gun1,
			Gun2,

			Center,
			Upper,
			Upper2,
			Head,
			Neck,
			RightArm,
			RightArm2,
			RightWrist,
			RightHandJoint,
			LeftArm,
			LeftArm2,
			LeftWrist,
			LeftHandJoint,
			Max,
		};
		static const char* CharaFrameName[(int)CharaFrame::Max] = {
			"プロペラ",
			"ラダー",
			"左補助翼",
			"右補助翼",
			"昇降舵",
			"機銃1",
			"機銃2",
			"センター",
			"上半身",
			"上半身2",
			"頭",
			"首",
			"右腕",
			"右ひじ",
			"右手首",
			"右ダミー",
			"左腕",
			"左ひじ",
			"左手首",
			"左ダミー",
		};
		enum class CharaShape {
			None,
			Propeller,
			Max,
		};
		static const char* CharaShapeName[(int)CharaShape::Max] = {
			"",
			"プロペラブラー",
		};

	};
};
