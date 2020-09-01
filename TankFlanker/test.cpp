// ���ʌ��ʊ�{

#include "DXLib_ref/DxLib_ref.h"


#define SCREEN_W		640		// ��ʂ̉���
#define SCREEN_H		480		// ��ʂ̏c��

#define MIRROR_SCREEN_W		640		// ���ɉf��f���̎擾�Ɏg�p����X�N���[���̉��𑜓x
#define MIRROR_SCREEN_H		480		// ���ɉf��f���̎擾�Ɏg�p����X�N���[���̏c�𑜓x
#define MIRROR_POINTNUM		64		// ���̕`��Ɏg�p����ʂ̒��_������
#define MIRROR_NUM		2		// ���̐�
#define MIRROR_DEBUG_SCALE	4		// ���̃f�o�b�O�\�����Ɍ��̉����̂P�ɂ��邩�̃T�C�Y

#define CAMERA_SPEED		32.0f		// �J�����̑��x


int CharaModel;		// �L�����N�^�[���f��
int StageModel;		// �X�e�[�W���f��

struct Mirror_mod{
	VECTOR_ref WorldPos[4];		// ���̃��[���h���W
	COLOR_F AmbientColor;		// ���� Ambient Color
	int DiffuseColor[4];		// ���� Diffuse Color
	int BlendParam[2];			// ���̃u�����h���[�h�ƃp�����[�^
	GraphHandle Handle;	// ���ɉf��f���̎擾�Ɏg�p����X�N���[��
	FLOAT4 ScreenPosW[4];	// ���ɉf��f���̎擾�Ɏg�p����N���[���̒��̋��̎l���̍��W( �������W )
};

// �R�c��Ԃ̕`��
void World_RenderProcess(){
	// �X�e�[�W���f���̕`��
	MV1DrawModel(StageModel);
	// �L�����N�^�[���f���̕`��
	MV1DrawModel(CharaModel);
}


class Mirrar_draw {
private:
	Mirror_mod Mirror_obj[MIRROR_NUM];
public:
	auto& get_Mirror_obj(int i) {
		return Mirror_obj[i];
	}
	auto& get_MirrorHandle(int i) {
		return Mirror_obj[i].Handle;
	}

	// ���̏�����
	void Mirror_Initialize(void) {
		for (int i = 0; i < MIRROR_NUM; i++) {
			Mirror_obj[i].Handle = GraphHandle::Make(MIRROR_SCREEN_W, MIRROR_SCREEN_H, FALSE);	// ���ɉf��f���̎擾�Ɏg�p����X�N���[���̍쐬
		}
	}
	//
	Mirrar_draw() {
		Mirror_Initialize();
	}
	~Mirrar_draw() {
	}

	// ���ɉf��f����`�悷�邽�߂̃J�����̐ݒ���s��
	void Mirror_SetupCamera(int MirrorNo, VECTOR_ref CameraEyePos, VECTOR_ref CameraTargetPos) {
		float EyeLength, TargetLength;
		VECTOR_ref MirrorNormal;
		VECTOR_ref *MirrorWorldPosP= Mirror_obj[MirrorNo].WorldPos;
		VECTOR_ref MirrorCameraEyePos, MirrorCameraTargetPos;
		// ���̖ʂ̖@�����Z�o
		MirrorNormal = ((MirrorWorldPosP[1]-MirrorWorldPosP[0]).cross(MirrorWorldPosP[2]-MirrorWorldPosP[0])).Norm();
		// ���̖ʂ���J�����̍��W�܂ł̍ŒZ�����A���̖ʂ���J�����̒����_�܂ł̍ŒZ�������Z�o
		EyeLength = Plane_Point_MinLength(MirrorWorldPosP[0].get(), MirrorNormal.get(), CameraEyePos.get());
		TargetLength = Plane_Point_MinLength(MirrorWorldPosP[0].get(), MirrorNormal.get(), CameraTargetPos.get());
		// ���ɉf��f����`�悷��ۂɎg�p����J�����̍��W�ƃJ�����̒����_���Z�o
		MirrorCameraEyePos = CameraEyePos + MirrorNormal*(-EyeLength * 2.0f);
		MirrorCameraTargetPos = CameraTargetPos + MirrorNormal*(-TargetLength * 2.0f);
		// �v�Z�œ���ꂽ�J�����̍��W�ƃJ�����̒����_�̍��W���J�����̐ݒ肷��
		SetCameraPositionAndTarget_UpVecY(MirrorCameraEyePos.get(), MirrorCameraTargetPos.get());
		// ���ɉf��f���̒��ł̋��̎l���̍��W���Z�o( �������W )
		for (int i = 0; i < 4; i++) {
			Mirror_obj[MirrorNo].ScreenPosW[i] = ConvWorldPosToScreenPosPlusW(MirrorWorldPosP[i].get());
		}
	}

	// ���̕`��
	void Mirror_Render(int MirrorNo) {
		int k;
		VERTEX3D Vert[MIRROR_POINTNUM * MIRROR_POINTNUM];
		unsigned short Index[(MIRROR_POINTNUM - 1) * (MIRROR_POINTNUM - 1) * 6];
		MATERIALPARAM Material;
		VECTOR_ref HUnitPos;
		VECTOR_ref VUnitPos[2];
		VECTOR_ref HPos;
		VECTOR_ref VPos[2];
		FLOAT4 HUnitUV;
		FLOAT4 VUnitUV[2];
		FLOAT4 HUV;
		FLOAT4 VUV[2];
		VECTOR_ref MirrorNormal;
		COLOR_U8 DiffuseColor;
		COLOR_U8 SpecularColor;
		int TextureW, TextureH;
		VECTOR_ref *MirrorWorldPosP = Mirror_obj[MirrorNo].WorldPos;
		// ���̕`��Ɏg�p����}�e���A���̃Z�b�g�A�b�v
		Material.Ambient = Mirror_obj[MirrorNo].AmbientColor;
		Material.Diffuse = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
		Material.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
		Material.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
		Material.Power = 1.0f;
		SetMaterialParam(Material);
		// ���̖ʂ̖@�����Z�o
		MirrorNormal = ((MirrorWorldPosP[1] - MirrorWorldPosP[0]).cross(MirrorWorldPosP[2]- MirrorWorldPosP[0])).Norm();
		// ���ɉf��f�����������񂾉摜�̃e�N�X�`���̃T�C�Y���擾
		GetGraphTextureSize(Mirror_obj[MirrorNo].Handle.get(), &TextureW, &TextureH);
		// ���̕`��Ɏg�p���钸�_�̃Z�b�g�A�b�v
		VUnitPos[0] = (MirrorWorldPosP[2]- MirrorWorldPosP[0])*( 1.0f / (MIRROR_POINTNUM - 1));
		VUnitPos[1] = (MirrorWorldPosP[3]- MirrorWorldPosP[1])*( 1.0f / (MIRROR_POINTNUM - 1));
		VUnitUV[0] = F4Scale(F4Sub(Mirror_obj[MirrorNo].ScreenPosW[2], Mirror_obj[MirrorNo].ScreenPosW[0]), 1.0f / (MIRROR_POINTNUM - 1));
		VUnitUV[1] = F4Scale(F4Sub(Mirror_obj[MirrorNo].ScreenPosW[3], Mirror_obj[MirrorNo].ScreenPosW[1]), 1.0f / (MIRROR_POINTNUM - 1));
		DiffuseColor = GetColorU8(Mirror_obj[MirrorNo].DiffuseColor[0], Mirror_obj[MirrorNo].DiffuseColor[1], Mirror_obj[MirrorNo].DiffuseColor[2], Mirror_obj[MirrorNo].DiffuseColor[3]);
		SpecularColor = GetColorU8(0, 0, 0, 0);
		VPos[0] = MirrorWorldPosP[0];
		VPos[1] = MirrorWorldPosP[1];
		VUV[0] = Mirror_obj[MirrorNo].ScreenPosW[0];
		VUV[1] = Mirror_obj[MirrorNo].ScreenPosW[1];
		k = 0;
		for (int i = 0; i < MIRROR_POINTNUM; i++) {
			HUnitPos = (VPos[1]-VPos[0])*(1.0f / (MIRROR_POINTNUM - 1));
			HPos = VPos[0];
			HUnitUV = F4Scale(F4Sub(VUV[1], VUV[0]), 1.0f / (MIRROR_POINTNUM - 1));
			HUV = VUV[0];
			for (int j = 0; j < MIRROR_POINTNUM; j++) {
				Vert[k].pos = HPos.get();
				Vert[k].norm = MirrorNormal.get();
				Vert[k].dif = DiffuseColor;
				Vert[k].spc = SpecularColor;
				Vert[k].u = HUV.x / (HUV.w * TextureW);
				Vert[k].v = HUV.y / (HUV.w * TextureH);
				Vert[k].su = 0.0f;
				Vert[k].sv = 0.0f;
				HUV = F4Add(HUV, HUnitUV);
				HPos += HUnitPos;
				k++;
			}
			VUV[0] = F4Add(VUV[0], VUnitUV[0]);
			VUV[1] = F4Add(VUV[1], VUnitUV[1]);
			VPos[0] += VUnitPos[0];
			VPos[1] += VUnitPos[1];
		}

		// ���̕`��Ɏg�p���钸�_�C���f�b�N�X���Z�b�g�A�b�v
		k = 0;
		for (int i = 0; i < MIRROR_POINTNUM - 1; i++) {
			for (int j = 0; j < MIRROR_POINTNUM - 1; j++) {
				Index[k + 0] = (i + 0) * MIRROR_POINTNUM + j + 0;
				Index[k + 1] = (i + 0) * MIRROR_POINTNUM + j + 1;
				Index[k + 2] = (i + 1) * MIRROR_POINTNUM + j + 0;
				Index[k + 3] = (i + 1) * MIRROR_POINTNUM + j + 1;
				Index[k + 4] = Index[k + 2];
				Index[k + 5] = Index[k + 1];
				k += 6;
			}
		}
		// ����`��
		SetDrawMode(DX_DRAWMODE_BILINEAR);
		SetDrawBlendMode(Mirror_obj[MirrorNo].BlendParam[0], Mirror_obj[MirrorNo].BlendParam[1]);
		DrawPolygonIndexed3D(Vert, MIRROR_POINTNUM * MIRROR_POINTNUM, Index, (MIRROR_POINTNUM - 1) * (MIRROR_POINTNUM - 1) * 2, Mirror_obj[MirrorNo].Handle.get(), FALSE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		SetDrawMode(DX_DRAWMODE_NEAREST);
	}
};


// WinMain�֐�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int CameraAngle;		// �J�����̐����p�x
	VECTOR_ref CameraEyePosition;	// �J�����̍��W
	VECTOR_ref CameraDirection;	// �J�����̌����Ă������
	VECTOR_ref CameraTargetPosition;	// �J�����̒����_
	int i;
	ChangeWindowMode(TRUE);
	SetGraphMode(SCREEN_W, SCREEN_H, 32);
	DxLib_Init();
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	//
	auto Mirrarparts = std::make_unique<Mirrar_draw>();// �������̏�����

	
	Mirrarparts->get_Mirror_obj(0).WorldPos[0] = VGet(2000.0f, 2000.0f, -4498.0f);
	Mirrarparts->get_Mirror_obj(0).WorldPos[1] = VGet(-2000.0f, 2000.0f, -4498.0f);
	Mirrarparts->get_Mirror_obj(0).WorldPos[2] = VGet(2000.0f, 0.0f, -4498.0f);
	Mirrarparts->get_Mirror_obj(0).WorldPos[3] = VGet(-2000.0f, 0.0f, -4498.0f);
	Mirrarparts->get_Mirror_obj(0).AmbientColor = GetColorF(1.0f, 1.0f, 1.0f, 1.0f);
	Mirrarparts->get_Mirror_obj(0).DiffuseColor[0] = 255;
	Mirrarparts->get_Mirror_obj(0).DiffuseColor[1] = 255;
	Mirrarparts->get_Mirror_obj(0).DiffuseColor[2] = 255;
	Mirrarparts->get_Mirror_obj(0).DiffuseColor[3] = 255;
	Mirrarparts->get_Mirror_obj(0).BlendParam[0] = DX_BLENDMODE_NOBLEND;
	Mirrarparts->get_Mirror_obj(0).BlendParam[1] = 255;

	Mirrarparts->get_Mirror_obj(1).WorldPos[0] = VGet(-4000.0f, 10.0f, 4500.0f);
	Mirrarparts->get_Mirror_obj(1).WorldPos[1] = VGet(4000.0f, 10.0f, 4500.0f);
	Mirrarparts->get_Mirror_obj(1).WorldPos[2] = VGet(-4000.0f, 10.0f, -4500.0f);
	Mirrarparts->get_Mirror_obj(1).WorldPos[3] = VGet(4000.0f, 10.0f, -4500.0f);
	Mirrarparts->get_Mirror_obj(1).AmbientColor = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
	Mirrarparts->get_Mirror_obj(1).DiffuseColor[0] = 0;
	Mirrarparts->get_Mirror_obj(1).DiffuseColor[1] = 255;
	Mirrarparts->get_Mirror_obj(1).DiffuseColor[2] = 255;
	Mirrarparts->get_Mirror_obj(1).DiffuseColor[3] = 255;
	Mirrarparts->get_Mirror_obj(1).BlendParam[0] = DX_BLENDMODE_ALPHA;
	Mirrarparts->get_Mirror_obj(1).BlendParam[1] = 128;


	CharaModel = MV1LoadModel("DxChara.x");
	StageModel = MV1LoadModel("ColTestStage.mqo");
	MV1SetPosition(CharaModel, VGet(0.0f, 0.0f, -3500.0f));
	MV1SetRotationXYZ(CharaModel, VGet(0.0f, 3.14159f / 2.0f, 0.0f));
	MV1AttachAnim(CharaModel, 7);
	CameraAngle = 325;
	CameraEyePosition = VGet(-2605.0f, 670.0f, -2561.0f);
	SetLightDirection(VGet(1.0f, -1.0f, -1.0f));

	// ���C�����[�v(�����L�[�������ꂽ�烋�[�v�𔲂���)
	while (ProcessMessage() == 0){
		// ���E�L�[�������ꂽ��J��������]����
		if (CheckHitKey(KEY_INPUT_LEFT)) {
			CameraAngle += 3;
			if (CameraAngle > 360) {
				CameraAngle -= 360;
			}
		}
		if (CheckHitKey(KEY_INPUT_RIGHT)) {
			CameraAngle -= 3;
			if (CameraAngle < 0) {
				CameraAngle += 360;
			}
		}
		//
		// �J�����̌����Ă���������Z�o
		CameraDirection = VGet(cos(CameraAngle * 3.14159f / 180.0f),0.0f,sin(CameraAngle * 3.14159f / 180.0f));
		// �㉺�L�[�������ꂽ��J������O�i�E��i������
		if (CheckHitKey(KEY_INPUT_UP)) {
			CameraEyePosition += CameraDirection * CAMERA_SPEED;
		}
		if (CheckHitKey(KEY_INPUT_DOWN)) {
			CameraEyePosition -= CameraDirection * CAMERA_SPEED;
		}
		// �J�����̒����_���W���Z�o
		CameraTargetPosition = CameraEyePosition+CameraDirection;
		// ���ɉf��f����`��
		for (i = 0; i < MIRROR_NUM; i++){
			Mirrarparts->get_MirrorHandle(i).SetDraw_Screen();
			{
				Mirrarparts->Mirror_SetupCamera(i, CameraEyePosition, CameraTargetPosition);	// ���ɉf��f����`�悷��ۂɎg�p����J�����̐ݒ���s��

				World_RenderProcess();											// �R�c��Ԃ̕`��
			}
		}
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		{
			SetCameraPositionAndTarget_UpVecY(CameraEyePosition.get(), CameraTargetPosition.get());

			World_RenderProcess();		// �R�c��Ԃ̕`��
			for (i = 0; i < MIRROR_NUM; i++) {
				Mirrarparts->Mirror_Render(i);		// ���̕`��
			}
		}
		ScreenFlip();
	}
	DxLib_End();
	return 0;
}
