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
			//�n���h�K��
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
			"�Z���^�[",
			"�㔼�g",
			"�㔼�g2",
			"��",
			"����",
			"�E��",
			"����",
			"���Ђ�",
			"������",
			"�E��",
			"�E�Ђ�",
			"�E����",
			"�E�r",
			"�E�Ђ�",
			"�E���",
			"�E�_�~�[",
			"�E�l�w�R",
			"�E�e�w�Q",
			"���r",
			"���Ђ�",
			"�����",
			"���_�~�[",
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
			"��",
			"��",
			"��",
			"��",
			"��",
			"�^�ʖ�",
			"�܂΂���",
		};

	};
};
