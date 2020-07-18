#include "sub.hpp"
#include "UI.hpp"
#include "HostPass.hpp"
#include "map.hpp"
#include "VR.hpp"
#include "debug.hpp"
#include "Setting.hpp"

#include"route.hpp"
//*
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	auto Main_c = std::make_unique<main_c>();
	return 0;
}
//*/
// �R�c�A�N�V������{
// �Œ�l��` -------------------------------------------------------------------------------------
// �v���C���[�֌W�̒�`
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
PLAYER pl;		// �v���C���[���̎��̐錾
STAGE stg;		// �X�e�[�W���̎��̐錾
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
