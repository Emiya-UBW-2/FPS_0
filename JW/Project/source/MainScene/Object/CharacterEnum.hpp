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
			"�v���y��",
			"���_�[",
			"���⏕��",
			"�E�⏕��",
			"���~��",
			"�@�e1",
			"�@�e2",
			"�Z���^�[",
			"�㔼�g",
			"�㔼�g2",
			"��",
			"��",
			"�E�r",
			"�E�Ђ�",
			"�E���",
			"�E�_�~�[",
			"���r",
			"���Ђ�",
			"�����",
			"���_�~�[",
		};
		enum class CharaShape {
			None,
			Propeller,
			Max,
		};
		static const char* CharaShapeName[(int)CharaShape::Max] = {
			"",
			"�v���y���u���[",
		};

	};
};
