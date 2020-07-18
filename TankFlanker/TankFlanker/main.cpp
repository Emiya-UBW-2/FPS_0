#include "sub.hpp"
#include "UI.hpp"
#include "HostPass.hpp"
#include "map.hpp"
#include "VR.hpp"
#include "debug.hpp"
#include "Setting.hpp"

#include"route.hpp"
/*
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	auto Main_c = std::make_unique<main_c>();
	return 0;
}
*/
// �R�c�A�N�V������{
// �Œ�l��` -------------------------------------------------------------------------------------
// �v���C���[�֌W�̒�`
#define PLAYER_PLAY_ANIM_SPEED			250.0f		// �A�j���[�V�������x
#define PLAYER_MOVE_SPEED			30.0f		// �ړ����x
#define PLAYER_ANIM_BLEND_SPEED			0.1f		// �A�j���[�V�����̃u�����h���ω����x
#define PLAYER_ANGLE_SPEED			0.2f		// �p�x�ω����x
#define PLAYER_JUMP_POWER			100.0f		// �W�����v��
#define PLAYER_FALL_UP_POWER			20.0f		// ���𓥂݊O�������̃W�����v��
#define PLAYER_GRAVITY				3.0f		// �d��
#define PLAYER_MAX_HITCOLL			2048		// ��������R���W�����|���S���̍ő吔
#define PLAYER_ENUM_DEFAULT_SIZE		800.0f		// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_HIT_WIDTH			200.0f		// �����蔻��J�v�Z���̔��a
#define PLAYER_HIT_HEIGHT			700.0f		// �����蔻��J�v�Z���̍���
#define PLAYER_HIT_TRYNUM			16		// �ǉ����o�������̍ő厎�s��
#define PLAYER_HIT_SLIDE_LENGTH			5.0f		// ��x�̕ǉ����o�������ŃX���C�h�����鋗��
#define PLAYER_SHADOW_SIZE			200.0f		// �e�̑傫��
#define PLAYER_SHADOW_HEIGHT			700.0f		// �e�������鍂��
// �J�����֌W�̒�`
#define CAMERA_ANGLE_SPEED			0.05f		// ���񑬓x
#define CAMERA_PLAYER_TARGET_HEIGHT		400.0f		// �v���C���[���W����ǂꂾ�������ʒu�𒍎��_�Ƃ��邩
#define CAMERA_PLAYER_LENGTH			1600.0f		// �v���C���[�Ƃ̋���
#define CAMERA_COLLISION_SIZE			50.0f		// �J�����̓����蔻��T�C�Y
// �\���̒�` -------------------------------------------------------------------------------------
// ���͏��\����
struct PADINPUT
{
	int		NowInput;				// ���݂̓���
	int		EdgeInput;				// ���݂̃t���[���ŉ����ꂽ�{�^���̂݃r�b�g�������Ă�����͒l
};
// �v���C���[���\����
struct PLAYER
{
	VECTOR		Position;				// ���W
	VECTOR		TargetMoveDirection;			// ���f���������ׂ������̃x�N�g��
	float		Angle;					// ���f���������Ă�������̊p�x
	float		JumpPower;				// �x�������̑��x
	int		ModelHandle;				// ���f���n���h��
	int		ShadowHandle;				// �e�摜�n���h��
	int		State;					// ���( 0:�����~�܂�  1:����  2:�W�����v )

	int		PlayAnim1;				// �Đ����Ă���A�j���[�V�����P�̃A�^�b�`�ԍ�( -1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ� )
	float		AnimPlayCount1;			// �Đ����Ă���A�j���[�V�����P�̍Đ�����
	int		PlayAnim2;				// �Đ����Ă���A�j���[�V�����Q�̃A�^�b�`�ԍ�( -1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ� )
	float		AnimPlayCount2;			// �Đ����Ă���A�j���[�V�����Q�̍Đ�����
	float		AnimBlendRate;				// �Đ����Ă���A�j���[�V�����P�ƂQ�̃u�����h��
};
// �X�e�[�W���\����
struct STAGE
{
	MV1		ModelHandle;				// ���f���n���h��
};
// �J�������\����
struct CAMERA
{
	float		AngleH;				// �����p�x
	float		AngleV;				// �����p�x
	VECTOR		Eye;					// �J�������W
	VECTOR		Target;				// �����_���W
};
// ���̐錾 ---------------------------------------------------------------------------------------
PADINPUT inp;		// ���͏��̎��̐錾
PLAYER pl;		// �v���C���[���̎��̐錾
STAGE stg;		// �X�e�[�W���̎��̐錾
CAMERA cam;		// �J�������̎��̐錾
void Player_PlayAnim(int PlayAnim);
void Player_AngleProcess(void);
void Player_Move(VECTOR_ref MoveVector);
void Player_AnimProcess(void);
void Player_Terminate(void){// �v���C���[�̌�n��
	MV1DeleteModel(pl.ModelHandle);	// ���f���̍폜
	DeleteGraph(pl.ShadowHandle);	// �e�p�摜�̍폜
}
void Player_Process(void){// �v���C���[�̏���
	VECTOR UpMoveVec;		// �����{�^���u���v����͂������Ƃ��̃v���C���[�̈ړ������x�N�g��
	VECTOR LeftMoveVec;		// �����{�^���u���v����͂������Ƃ��̃v���C���[�̈ړ������x�N�g��
	VECTOR MoveVec;			// ���̃t���[���̈ړ��x�N�g��
	int MoveFlag;			// �ړ��������ǂ����̃t���O( 1:�ړ�����  0:�ړ����Ă��Ȃ� )
	// ���[�g�t���[���̂y�������̈ړ��p�����[�^�𖳌��ɂ���
	{
		MATRIX LocalMatrix;
		MV1ResetFrameUserLocalMatrix(pl.ModelHandle, 2);		// ���[�U�[�s�����������
		LocalMatrix = MV1GetFrameLocalMatrix(pl.ModelHandle, 2);		// ���݂̃��[�g�t���[���̍s����擾����
		LocalMatrix.m[3][2] = 0.0f;		// �y�������̕��s�ړ������𖳌��ɂ���
		MV1SetFrameUserLocalMatrix(pl.ModelHandle, 2, LocalMatrix);		// ���[�U�[�s��Ƃ��ĕ��s�ړ������𖳌��ɂ����s������[�g�t���[���ɃZ�b�g����
	}
	// �v���C���[�̈ړ������̃x�N�g�����Z�o
	{
		UpMoveVec = VSub(cam.Target, cam.Eye);		// �����{�^���u���v���������Ƃ��̃v���C���[�̈ړ��x�N�g���̓J�����̎�����������x�����𔲂�������
		UpMoveVec.y = 0.0f;
		LeftMoveVec = VCross(UpMoveVec, VGet(0.0f, 1.0f, 0.0f));		// �����{�^���u���v���������Ƃ��̃v���C���[�̈ړ��x�N�g���͏���������Ƃ��̕����x�N�g���Ƃx���̃v���X�����̃x�N�g���ɐ����ȕ���
		// ��̃x�N�g���𐳋K��( �x�N�g���̒������P�D�O�ɂ��邱�� )
		UpMoveVec = VNorm(UpMoveVec);
		LeftMoveVec = VNorm(LeftMoveVec);
	}
	MoveVec = VGet(0.0f, 0.0f, 0.0f);	// ���̃t���[���ł̈ړ��x�N�g����������
	MoveFlag = 0;	// �ړ��������ǂ����̃t���O��������Ԃł́u�ړ����Ă��Ȃ��v��\���O�ɂ���
	if (CheckHitKey(KEY_INPUT_LSHIFT) == 0 && (inp.NowInput & PAD_INPUT_C) == 0){	// �p�b�h�̂R�{�^���ƍ��V�t�g���ǂ����������Ă��Ȃ�������v���C���[�̈ړ�����
		if (inp.NowInput & PAD_INPUT_LEFT){		// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă���������猩�č������Ɉړ�����
			MoveVec = VAdd(MoveVec, LeftMoveVec);			// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g�������Z����
			MoveFlag = 1;			// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
		}
		else if (inp.NowInput & PAD_INPUT_RIGHT) {			// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă���������猩�ĉE�����Ɉړ�����
			MoveVec = VAdd(MoveVec, VScale(LeftMoveVec, -1.0f));				// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g���𔽓]�������̂����Z����
			MoveFlag = 1;				// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
		}
		if (inp.NowInput & PAD_INPUT_UP){		// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă�������Ɉړ�����
			MoveVec = VAdd(MoveVec, UpMoveVec);			// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g�������Z����
			MoveFlag = 1;			// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
		}
		else if (inp.NowInput & PAD_INPUT_DOWN) {			// �����{�^���u���v�����͂��ꂽ��J�����̕����Ɉړ�����
			MoveVec = VAdd(MoveVec, VScale(UpMoveVec, -1.0f));				// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g���𔽓]�������̂����Z����
			MoveFlag = 1;			// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
		}
		if (pl.State != 2 && (inp.EdgeInput & PAD_INPUT_A)){		// �v���C���[�̏�Ԃ��u�W�����v�v�ł͂Ȃ��A���{�^���P��������Ă�����W�����v����
			pl.State = 2;			// ��Ԃ��u�W�����v�v�ɂ���
			pl.JumpPower = PLAYER_JUMP_POWER;			// �x�������̑��x���Z�b�g
			Player_PlayAnim(2);			// �W�����v�A�j���[�V�����̍Đ�
		}
	}

	// �ړ��{�^���������ꂽ���ǂ����ŏ����𕪊�
	if (MoveFlag){
		// �ړ��x�N�g���𐳋K���������̂��v���C���[�������ׂ������Ƃ��ĕۑ�
		pl.TargetMoveDirection = VNorm(MoveVec);

		// �v���C���[�������ׂ������x�N�g�����v���C���[�̃X�s�[�h�{�������̂��ړ��x�N�g���Ƃ���
		MoveVec = VScale(pl.TargetMoveDirection, PLAYER_MOVE_SPEED);

		// �������܂Łu�����~�܂�v��Ԃ�������
		if (pl.State == 0)
		{
			// ����A�j���[�V�������Đ�����
			Player_PlayAnim(1);

			// ��Ԃ��u����v�ɂ���
			pl.State = 1;
		}
	}
	else
	{
		// ���̃t���[���ňړ����Ă��Ȃ��āA����Ԃ��u����v��������
		if (pl.State == 1)
		{
			// �����~��A�j���[�V�������Đ�����
			Player_PlayAnim(4);

			// ��Ԃ��u�����~��v�ɂ���
			pl.State = 0;
		}
	}

	// ��Ԃ��u�W�����v�v�̏ꍇ��
	if (pl.State == 2)
	{
		// �x�������̑��x���d�͕����Z����
		pl.JumpPower -= PLAYER_GRAVITY;

		// �����������Ă��Ċ��Đ�����Ă���A�j���[�V�������㏸���p�̂��̂������ꍇ��
		if (pl.JumpPower < 0.0f && MV1GetAttachAnim(pl.ModelHandle, pl.PlayAnim1) == 2)
		{
			// �������悤�̃A�j���[�V�������Đ�����
			Player_PlayAnim(3);
		}

		// �ړ��x�N�g���̂x�������x�������̑��x�ɂ���
		MoveVec.y = pl.JumpPower;
	}

	// �v���C���[�̈ړ������Ƀ��f���̕������߂Â���
	Player_AngleProcess();

	// �ړ��x�N�g�������ɃR���W�������l�����v���C���[���ړ�
	Player_Move(MoveVec);

	// �A�j���[�V��������
	Player_AnimProcess();
}
// �֐��錾 ---------------------------------------------------------------------------
void Input_Process(void) {// ���͏���
	int Old;
	Old = inp.NowInput;	// �ЂƂO�̃t���[���̓��͂�ϐ��ɂƂ��Ă���
	inp.NowInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);	// ���݂̓��͏�Ԃ��擾
	inp.EdgeInput = inp.NowInput & ~Old;	// ���̃t���[���ŐV���ɉ����ꂽ�{�^���̃r�b�g���������Ă���l�� EdgeInput �ɑ������
}
void Player_Initialize(void) {// �v���C���[�̏�����
	pl.Position = VGet(0.0f, 0.0f, 0.0f);	// �������W�͌��_
	pl.Angle = 0.0f;	// ��]�l�͂O
	pl.JumpPower = 0.0f;	// �W�����v�͂͏�����Ԃł͂O
	pl.ModelHandle = MV1LoadModel("DxChara.x");	// ���f���̓ǂݍ���
	pl.ShadowHandle = LoadGraph("Shadow.tga");	// �e�`��p�̉摜�̓ǂݍ���
	pl.State = 0;	// ������Ԃł́u�����~��v���
	pl.TargetMoveDirection = VGet(1.0f, 0.0f, 0.0f);	// ������ԂŃv���C���[�������ׂ������͂w������
	pl.AnimBlendRate = 1.0f;	// �A�j���[�V�����̃u�����h����������
	pl.PlayAnim1 = -1;	// ������Ԃł̓A�j���[�V�����̓A�^�b�`����Ă��Ȃ��ɂ���
	pl.PlayAnim2 = -1;	// ������Ԃł̓A�j���[�V�����̓A�^�b�`����Ă��Ȃ��ɂ���
	Player_PlayAnim(4);	// ���������Ă���A�j���[�V�������Đ�
}
// �v���C���[�̈ړ�����
void Player_Move(VECTOR_ref MoveVector) {
	VECTOR_ref OldPos = pl.Position;			// �ړ��O�̍��W
	VECTOR_ref NowPos = OldPos + MoveVector;	// �ړ���̍��W
	// x����y�������� 0.01f �ȏ�ړ������ꍇ�́u�ړ������v�t���O���P�ɂ���
	bool MoveFlag = (abs(MoveVector.x()) > 0.01f || abs(MoveVector.z()) > 0.01f);	
	// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
	auto HitDim = stg.ModelHandle.CollCheck_Sphere(pl.Position, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), -1);
	std::vector<MV1_COLL_RESULT_POLY*> kabe_;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
	// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
	for (int i = 0; i < HitDim.HitNum; i++) {
		auto& h_d = HitDim.Dim[i];
		// �w�y���ʂɐ������ǂ����̓|���S���̖@���̂x�������O�Ɍ���Ȃ��߂����ǂ����Ŕ��f���� �ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{�P�D�O����荂���|���S���̂ݓ����蔻����s��
		if (
			(h_d.Normal.y < 0.000001f && h_d.Normal.y > -0.000001f) &&
			(h_d.Position[0].y > pl.Position.y + 1.0f || h_d.Position[1].y > pl.Position.y + 1.0f || h_d.Position[2].y > pl.Position.y + 1.0f)
			) {
			kabe_.resize(kabe_.size() + 1);
			kabe_.back() = &h_d;						// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
		}
	}
	// �ǃ|���S���Ƃ̓����蔻�菈��
	if (kabe_.size() > 0) {
		bool HitFlag = false;
		for (auto& k_ : kabe_) {
			if (HitCheck_Capsule_Triangle(NowPos.get(), (NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)).get(), PLAYER_HIT_WIDTH, k_->Position[0], k_->Position[1], k_->Position[2]) == TRUE) {				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
				HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
				if (MoveFlag) {
					// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
					VECTOR_ref SlideVec = k_->Normal;
					NowPos = OldPos + SlideVec.cross(MoveVector.cross(SlideVec));
					bool j = false;
					for (auto& b_ : kabe_) {
						if (HitCheck_Capsule_Triangle(NowPos.get(), (NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)).get(), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2]) == TRUE) {
							j = true;
							break;// �������Ă����烋�[�v���甲����
						}
					}
					if (!j) {
						HitFlag = false;
						break;//�ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂ŕǂɓ��������t���O��|������Ń��[�v���甲����
					}
				}
				else {
					break;
				}
			}
		}
		if (HitFlag) {		// �ǂɓ������Ă�����ǂ��牟���o���������s��
			for (int k = 0; k < PLAYER_HIT_TRYNUM; k++) {			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
				bool i = false;
				for (auto& k_ : kabe_) {
					if (HitCheck_Capsule_Triangle(NowPos.get(), (NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)).get(), PLAYER_HIT_WIDTH, k_->Position[0], k_->Position[1], k_->Position[2]) == TRUE) {// �v���C���[�Ɠ������Ă��邩�𔻒�
						VECTOR_ref SlideVec = k_->Normal;
						NowPos += SlideVec*PLAYER_HIT_SLIDE_LENGTH;					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
						int j = false;
						for (auto& b_ : kabe_) {
							if (HitCheck_Capsule_Triangle(NowPos.get(), (NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)).get(), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2]) == TRUE) {						// �������Ă����烋�[�v�𔲂���
								j = true;
								break;
							}
						}
						if (!j) {// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
							break;
						}
					}
					i=true;
				}
				if (!i) {//�S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
					break;
				}
			}
		}
	}
	MV1CollResultPolyDimTerminate(HitDim);	// ���o�����v���C���[�̎��͂̃|���S�������J������

	pl.Position = NowPos.get();	// �V�������W��ۑ�����
	MV1SetPosition(pl.ModelHandle, pl.Position);	// �v���C���[�̃��f���̍��W���X�V����
	kabe_.clear();
}
// �v���C���[�̌�����ς��鏈��
void Player_AngleProcess(void)
{
	float TargetAngle;			// �ڕW�p�x
	float SaAngle;				// �ڕW�p�x�ƌ��݂̊p�x�Ƃ̍�

	// �ڕW�̕����x�N�g������p�x�l���Z�o����
	TargetAngle = atan2(pl.TargetMoveDirection.x, pl.TargetMoveDirection.z);

	// �ڕW�̊p�x�ƌ��݂̊p�x�Ƃ̍�������o��
	{
		// �ŏ��͒P���Ɉ����Z
		SaAngle = TargetAngle - pl.Angle;

		// ����������炠������̍����P�W�O�x�ȏ�ɂȂ邱�Ƃ͖����̂�
		// ���̒l���P�W�O�x�ȏ�ɂȂ��Ă�����C������
		if (SaAngle < -DX_PI_F)
		{
			SaAngle += DX_TWO_PI_F;
		}
		else
			if (SaAngle > DX_PI_F)
			{
				SaAngle -= DX_TWO_PI_F;
			}
	}

	// �p�x�̍����O�ɋ߂Â���
	if (SaAngle > 0.0f)
	{
		// �����v���X�̏ꍇ�͈���
		SaAngle -= PLAYER_ANGLE_SPEED;
		if (SaAngle < 0.0f)
		{
			SaAngle = 0.0f;
		}
	}
	else
	{
		// �����}�C�i�X�̏ꍇ�͑���
		SaAngle += PLAYER_ANGLE_SPEED;
		if (SaAngle > 0.0f)
		{
			SaAngle = 0.0f;
		}
	}

	// ���f���̊p�x���X�V
	pl.Angle = TargetAngle - SaAngle;
	MV1SetRotationXYZ(pl.ModelHandle, VGet(0.0f, pl.Angle + DX_PI_F, 0.0f));
}
// �v���C���[�ɐV���ȃA�j���[�V�������Đ�����
void Player_PlayAnim(int PlayAnim)
{
	// �Đ����̃��[�V�����Q���L����������f�^�b�`����
	if (pl.PlayAnim2 != -1)
	{
		MV1DetachAnim(pl.ModelHandle, pl.PlayAnim2);
		pl.PlayAnim2 = -1;
	}

	// ���܂ōĐ����̃��[�V�����P���������̂̏����Q�Ɉړ�����
	pl.PlayAnim2 = pl.PlayAnim1;
	pl.AnimPlayCount2 = pl.AnimPlayCount1;

	// �V���Ɏw��̃��[�V���������f���ɃA�^�b�`���āA�A�^�b�`�ԍ���ۑ�����
	pl.PlayAnim1 = MV1AttachAnim(pl.ModelHandle, PlayAnim);
	pl.AnimPlayCount1 = 0.0f;

	// �u�����h���͍Đ����̃��[�V�����Q���L���ł͂Ȃ��ꍇ�͂P�D�O��( �Đ����̃��[�V�����P���P�O�O���̏�� )�ɂ���
	pl.AnimBlendRate = pl.PlayAnim2 == -1 ? 1.0f : 0.0f;
}
// �v���C���[�̃A�j���[�V��������
void Player_AnimProcess(void)
{
	float AnimTotalTime;		// �Đ����Ă���A�j���[�V�����̑�����

	// �u�����h�����P�ȉ��̏ꍇ�͂P�ɋ߂Â���
	if (pl.AnimBlendRate < 1.0f)
	{
		pl.AnimBlendRate += PLAYER_ANIM_BLEND_SPEED;
		if (pl.AnimBlendRate > 1.0f)
		{
			pl.AnimBlendRate = 1.0f;
		}
	}

	// �Đ����Ă���A�j���[�V�����P�̏���
	if (pl.PlayAnim1 != -1)
	{
		// �A�j���[�V�����̑����Ԃ��擾
		AnimTotalTime = MV1GetAttachAnimTotalTime(pl.ModelHandle, pl.PlayAnim1);

		// �Đ����Ԃ�i�߂�
		pl.AnimPlayCount1 += PLAYER_PLAY_ANIM_SPEED;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (pl.AnimPlayCount1 >= AnimTotalTime)
		{
			pl.AnimPlayCount1 = fmod(pl.AnimPlayCount1, AnimTotalTime);
		}

		// �ύX�����Đ����Ԃ����f���ɔ��f������
		MV1SetAttachAnimTime(pl.ModelHandle, pl.PlayAnim1, pl.AnimPlayCount1);

		// �A�j���[�V�����P�̃��f���ɑ΂��锽�f�����Z�b�g
		MV1SetAttachAnimBlendRate(pl.ModelHandle, pl.PlayAnim1, pl.AnimBlendRate);
	}

	// �Đ����Ă���A�j���[�V�����Q�̏���
	if (pl.PlayAnim2 != -1)
	{
		// �A�j���[�V�����̑����Ԃ��擾
		AnimTotalTime = MV1GetAttachAnimTotalTime(pl.ModelHandle, pl.PlayAnim2);

		// �Đ����Ԃ�i�߂�
		pl.AnimPlayCount2 += PLAYER_PLAY_ANIM_SPEED;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (pl.AnimPlayCount2 > AnimTotalTime)
		{
			pl.AnimPlayCount2 = fmod(pl.AnimPlayCount2, AnimTotalTime);
		}

		// �ύX�����Đ����Ԃ����f���ɔ��f������
		MV1SetAttachAnimTime(pl.ModelHandle, pl.PlayAnim2, pl.AnimPlayCount2);

		// �A�j���[�V�����Q�̃��f���ɑ΂��锽�f�����Z�b�g
		MV1SetAttachAnimBlendRate(pl.ModelHandle, pl.PlayAnim2, 1.0f - pl.AnimBlendRate);
	}
}
// �v���C���[�̉e��`��
void Player_ShadowRender(void){
	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY *HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;
	SetUseLighting(FALSE);	// ���C�e�B���O�𖳌��ɂ���
	SetUseZBuffer3D(TRUE);	// �y�o�b�t�@��L���ɂ���
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);	// �e�N�X�`���A�h���X���[�h�� CLAMP �ɂ���( �e�N�X�`���̒[����͒[�̃h�b�g�����X���� )
	// �v���C���[�̒����ɑ��݂���n�ʂ̃|���S�����擾
	HitResDim = MV1CollCheck_Capsule(stg.ModelHandle, -1, pl.Position, VAdd(pl.Position, VGet(0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f)), PLAYER_SHADOW_SIZE);
	// ���_�f�[�^�ŕω��������������Z�b�g
	Vertex[0].dif = GetColorU8(255, 255, 255, 255);
	Vertex[0].spc = GetColorU8(0, 0, 0, 0);
	Vertex[0].su = 0.0f;
	Vertex[0].sv = 0.0f;
	Vertex[1] = Vertex[0];
	Vertex[2] = Vertex[0];
	// ���̒����ɑ��݂���|���S���̐������J��Ԃ�
	HitRes = HitResDim.Dim;
	for (i = 0; i < HitResDim.HitNum; i++, HitRes++){
		// �|���S���̍��W�͒n�ʃ|���S���̍��W
		Vertex[0].pos = HitRes->Position[0];
		Vertex[1].pos = HitRes->Position[1];
		Vertex[2].pos = HitRes->Position[2];
		// ������Ǝ����グ�ďd�Ȃ�Ȃ��悤�ɂ���
		SlideVec = VScale(HitRes->Normal, 0.5f);
		Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
		Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
		Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);
		// �|���S���̕s�����x��ݒ肷��
		Vertex[0].dif.a = 0;
		Vertex[1].dif.a = 0;
		Vertex[2].dif.a = 0;
		if (HitRes->Position[0].y > pl.Position.y - PLAYER_SHADOW_HEIGHT) {
			Vertex[0].dif.a = 128 * BYTE(1.0f - fabs(HitRes->Position[0].y - pl.Position.y) / PLAYER_SHADOW_HEIGHT);
		}
		if (HitRes->Position[1].y > pl.Position.y - PLAYER_SHADOW_HEIGHT) {
			Vertex[1].dif.a = 128 * BYTE(1.0f - fabs(HitRes->Position[1].y - pl.Position.y) / PLAYER_SHADOW_HEIGHT);
		}
		if (HitRes->Position[2].y > pl.Position.y - PLAYER_SHADOW_HEIGHT) {
			Vertex[2].dif.a = 128 * BYTE(1.0f - fabs(HitRes->Position[2].y - pl.Position.y) / PLAYER_SHADOW_HEIGHT);
		}
		// �t�u�l�͒n�ʃ|���S���ƃv���C���[�̑��΍��W���犄��o��
		Vertex[0].u = (HitRes->Position[0].x - pl.Position.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[0].v = (HitRes->Position[0].z - pl.Position.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[1].u = (HitRes->Position[1].x - pl.Position.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[1].v = (HitRes->Position[1].z - pl.Position.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[2].u = (HitRes->Position[2].x - pl.Position.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[2].v = (HitRes->Position[2].z - pl.Position.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		// �e�|���S����`��
		DrawPolygon3D(Vertex, 1, pl.ShadowHandle, TRUE);
	}
	MV1CollResultPolyDimTerminate(HitResDim);	// ���o�����n�ʃ|���S�����̌�n��
	SetUseLighting(TRUE);	// ���C�e�B���O��L���ɂ���
	SetUseZBuffer3D(FALSE);	// �y�o�b�t�@�𖳌��ɂ���
}
// �X�e�[�W�̏���������
void Stage_Initialize(void){
	stg.ModelHandle = MV1LoadModel("ColTestStage.mqo");	// �X�e�[�W���f���̓ǂݍ���
	MV1SetupCollInfo(stg.ModelHandle, -1);	// ���f���S�̂̃R���W�������̃Z�b�g�A�b�v
}
// �X�e�[�W�̌�n������
void Stage_Terminate(void){
	MV1DeleteModel(stg.ModelHandle);	// �X�e�[�W���f���̌�n��
}
// �J�����̏���������
void Camera_Initialize(void){
	cam.AngleH = DX_PI_F;	// �J�����̏��������p�x�͂P�W�O�x
	cam.AngleV = 0.0f;	// �����p�x�͂O�x
}
// �J�����̏���
void Camera_Process(void){
	if (CheckHitKey(KEY_INPUT_LSHIFT) || (inp.NowInput & PAD_INPUT_C)){	// �p�b�h�̂R�{�^�����A�V�t�g�L�[��������Ă���ꍇ�̂݊p�x�ύX������s��
		if (inp.NowInput & PAD_INPUT_LEFT){		// �u���v�{�^����������Ă����琅���p�x���}�C�i�X����
			cam.AngleH -= CAMERA_ANGLE_SPEED;
			if (cam.AngleH < -DX_PI_F){			// �|�P�W�O�x�ȉ��ɂȂ�����p�x�l���傫���Ȃ肷���Ȃ��悤�ɂR�U�O�x�𑫂�
				cam.AngleH += DX_TWO_PI_F;
			}
		}
		if (inp.NowInput & PAD_INPUT_RIGHT){		// �u���v�{�^����������Ă����琅���p�x���v���X����
			cam.AngleH += CAMERA_ANGLE_SPEED;
			if (cam.AngleH > DX_PI_F){			// �P�W�O�x�ȏ�ɂȂ�����p�x�l���傫���Ȃ肷���Ȃ��悤�ɂR�U�O�x������
				cam.AngleH -= DX_TWO_PI_F;
			}
		}
		if (inp.NowInput & PAD_INPUT_UP){		// �u���v�{�^����������Ă����琂���p�x���}�C�i�X����
			cam.AngleV -= CAMERA_ANGLE_SPEED;
			if (cam.AngleV < -DX_PI_F / 2.0f + 0.6f){			// ������p�x�ȉ��ɂ͂Ȃ�Ȃ��悤�ɂ���
				cam.AngleV = -DX_PI_F / 2.0f + 0.6f;
			}
		}
		if (inp.NowInput & PAD_INPUT_DOWN) {		// �u���v�{�^����������Ă����琂���p�x���v���X����
			cam.AngleV += CAMERA_ANGLE_SPEED;
			if (cam.AngleV > DX_PI_F / 2.0f - 0.6f) {			// ������p�x�ȏ�ɂ͂Ȃ�Ȃ��悤�ɂ���
				cam.AngleV = DX_PI_F / 2.0f - 0.6f;
			}
		}
	}

	// �J�����̒����_�̓v���C���[���W����K��l���������W
	cam.Target = VAdd(pl.Position, VGet(0.0f, CAMERA_PLAYER_TARGET_HEIGHT, 0.0f));

	// �J�����̍��W�����肷��
	{
		MATRIX RotZ, RotY;
		float Camera_Player_Length;
		MV1_COLL_RESULT_POLY_DIM HRes;
		int HitNum;
		RotY = MGetRotY(cam.AngleH);		// ���������̉�]�͂x����]
		RotZ = MGetRotZ(cam.AngleV);		// ���������̉�]�͂y����] )
		Camera_Player_Length = CAMERA_PLAYER_LENGTH;		// �J��������v���C���[�܂ł̏����������Z�b�g

		cam.Eye = VAdd(VTransform(VTransform(VGet(-Camera_Player_Length, 0.0f, 0.0f), RotZ), RotY), cam.Target);		// �J�����̍��W���Z�o �w���ɃJ�����ƃv���C���[�Ƃ̋����������L�т��x�N�g���𐂒�������]( �y����] )���������Ɛ���������]( �x����] )���čX�ɒ����_�̍��W�𑫂������̂��J�����̍��W
		HRes = MV1CollCheck_Capsule(stg.ModelHandle, -1, cam.Target, cam.Eye, CAMERA_COLLISION_SIZE);		// �����_����J�����̍��W�܂ł̊ԂɃX�e�[�W�̃|���S�������邩���ׂ�
		HitNum = HRes.HitNum;
		MV1CollResultPolyDimTerminate(HRes);
		if (HitNum != 0){
			float NotHitLength;
			float HitLength;
			float TestLength;
			VECTOR TestPosition;
			// �������疳���ʒu�܂Ńv���C���[�ɋ߂Â�
			NotHitLength = 0.0f;			// �|���S���ɓ�����Ȃ��������Z�b�g
			HitLength = Camera_Player_Length;			// �|���S���ɓ����鋗�����Z�b�g
			do{
				TestLength = NotHitLength + (HitLength - NotHitLength) / 2.0f;				// �����邩�ǂ����e�X�g���鋗�����Z�b�g( ������Ȃ������Ɠ����鋗���̒��� )
				TestPosition = VAdd(VTransform(VTransform(VGet(-TestLength, 0.0f, 0.0f), RotZ), RotY), cam.Target);				// �e�X�g�p�̃J�������W���Z�o
				HRes = MV1CollCheck_Capsule(stg.ModelHandle, -1, cam.Target, TestPosition, CAMERA_COLLISION_SIZE);				// �V�������W�ŕǂɓ����邩�e�X�g
				HitNum = HRes.HitNum;
				MV1CollResultPolyDimTerminate(HRes);
				if (HitNum != 0){
					HitLength = TestLength;					// ���������瓖���鋗���� TestLength �ɕύX����
				}
				else{
					NotHitLength = TestLength;					// ������Ȃ������瓖����Ȃ������� TestLength �ɕύX����
				}
			} while (HitLength - NotHitLength > 0.1f);				// HitLength �� NoHitLength ���\���ɋ߂Â��Ă��Ȃ������烋�[�v
			cam.Eye = TestPosition;			// �J�����̍��W���Z�b�g
		}
	}
	SetCameraPositionAndTarget_UpVecY(cam.Eye, cam.Target);	// �J�����̏������C�u�����̃J�����ɔ��f������
}
// �`�揈��
void Render_Process(void){
	MV1DrawModel(stg.ModelHandle);	// �X�e�[�W���f���̕`��
	MV1DrawModel(pl.ModelHandle);	// �v���C���[���f���̕`��
	Player_ShadowRender();	// �v���C���[�̉e�̕`��
}
// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	ChangeWindowMode(TRUE);
	DxLib_Init();
	Player_Initialize();	// �v���C���[�̏�����
	Stage_Initialize();	// �X�e�[�W�̏�����
	Camera_Initialize();	// �J�����̏�����
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0){
		ClearDrawScreen();
		SetDrawScreen(DX_SCREEN_BACK);
		{
			Input_Process();		// ���͏���
			Player_Process();		// �v���C���[�̏���
			Camera_Process();		// �J�����̏���
			Render_Process();		// �`�揈��
		}
		ScreenFlip();		// ����ʂ̓��e��\��ʂɔ��f
	}
	Player_Terminate();	// �v���C���[�̌�n��
	Stage_Terminate();	// �X�e�[�W�̌�n��
	DxLib_End();	// ���C�u�����̌�n��
	return 0;
}