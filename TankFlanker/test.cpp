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
// ���̃��[���h���W
VECTOR MirrorWorldPos[MIRROR_NUM][4] =
{
	{
		{  2000.0f, 2000.0f, -4498.0f },
		{ -2000.0f, 2000.0f, -4498.0f },
		{  2000.0f,    0.0f, -4498.0f },
		{ -2000.0f,    0.0f, -4498.0f },
	},

	{
		{ -4000.0f, 10.0f,  4500.0f },
		{  4000.0f, 10.0f,  4500.0f },
		{ -4000.0f, 10.0f, -4500.0f },
		{  4000.0f, 10.0f, -4500.0f },
	}
};

// ���� Ambient Color
COLOR_F MirrorAmbientColor[MIRROR_NUM] =
{
	{ 1.0f, 1.0f, 1.0f, 1.0f },
	{ 0.0f, 0.0f, 0.0f, 0.0f },
};

// ���� Diffuse Color
int MirrorDiffuseColor[MIRROR_NUM][4] =
{
	{ 255, 255, 255, 255 },
	{   0, 255, 255, 255 },
};

// ���̃u�����h���[�h�ƃp�����[�^
int MirrorBlendParam[MIRROR_NUM][2] =
{
	{ DX_BLENDMODE_NOBLEND, 255 },
	{ DX_BLENDMODE_ALPHA,   128 },
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
	GraphHandle MirrorHandle[MIRROR_NUM];		// ���ɉf��f���̎擾�Ɏg�p����X�N���[��
	FLOAT4 MirrorScreenPosW[MIRROR_NUM][4];	// ���ɉf��f���̎擾�Ɏg�p����N���[���̒��̋��̎l���̍��W( �������W )
public:
	auto& get_MirrorHandle(int i) {
		return MirrorHandle[i];
	}

	// ���̏�����
	void Mirror_Initialize(void) {
		for (int i = 0; i < MIRROR_NUM; i++) {
			MirrorHandle[i] = GraphHandle::Make(MIRROR_SCREEN_W, MIRROR_SCREEN_H, FALSE);	// ���ɉf��f���̎擾�Ɏg�p����X�N���[���̍쐬
		}
	}
	//
	Mirrar_draw() {
		Mirror_Initialize();
	}
	~Mirrar_draw() {
	}

	// ���ɉf��f����`�悷�邽�߂̃J�����̐ݒ���s��
	void Mirror_SetupCamera(int MirrorNo, VECTOR CameraEyePos, VECTOR CameraTargetPos) {
		float EyeLength, TargetLength;
		VECTOR MirrorNormal;
		VECTOR *MirrorWorldPosP;
		VECTOR MirrorCameraEyePos, MirrorCameraTargetPos;
		MirrorWorldPosP = MirrorWorldPos[MirrorNo];
		// ���̖ʂ̖@�����Z�o
		MirrorNormal = VNorm(VCross(VSub(MirrorWorldPosP[1], MirrorWorldPosP[0]), VSub(MirrorWorldPosP[2], MirrorWorldPosP[0])));
		// ���̖ʂ���J�����̍��W�܂ł̍ŒZ�����A���̖ʂ���J�����̒����_�܂ł̍ŒZ�������Z�o
		EyeLength = Plane_Point_MinLength(MirrorWorldPosP[0], MirrorNormal, CameraEyePos);
		TargetLength = Plane_Point_MinLength(MirrorWorldPosP[0], MirrorNormal, CameraTargetPos);
		// ���ɉf��f����`�悷��ۂɎg�p����J�����̍��W�ƃJ�����̒����_���Z�o
		MirrorCameraEyePos = VAdd(CameraEyePos, VScale(MirrorNormal, -EyeLength * 2.0f));
		MirrorCameraTargetPos = VAdd(CameraTargetPos, VScale(MirrorNormal, -TargetLength * 2.0f));
		// �v�Z�œ���ꂽ�J�����̍��W�ƃJ�����̒����_�̍��W���J�����̐ݒ肷��
		SetCameraPositionAndTarget_UpVecY(MirrorCameraEyePos, MirrorCameraTargetPos);
		// ���ɉf��f���̒��ł̋��̎l���̍��W���Z�o( �������W )
		for (int i = 0; i < 4; i++) {
			MirrorScreenPosW[MirrorNo][i] = ConvWorldPosToScreenPosPlusW(MirrorWorldPosP[i]);
		}
	}

	// ���̕`��
	void Mirror_Render(int MirrorNo) {
		int k;
		VERTEX3D Vert[MIRROR_POINTNUM * MIRROR_POINTNUM];
		unsigned short Index[(MIRROR_POINTNUM - 1) * (MIRROR_POINTNUM - 1) * 6];
		MATERIALPARAM Material;
		VECTOR HUnitPos;
		VECTOR VUnitPos[2];
		VECTOR HPos;
		VECTOR VPos[2];
		FLOAT4 HUnitUV;
		FLOAT4 VUnitUV[2];
		FLOAT4 HUV;
		FLOAT4 VUV[2];
		VECTOR MirrorNormal;
		COLOR_U8 DiffuseColor;
		COLOR_U8 SpecularColor;
		int TextureW, TextureH;
		VECTOR *MirrorWorldPosP;

		MirrorWorldPosP = MirrorWorldPos[MirrorNo];
		// ���̕`��Ɏg�p����}�e���A���̃Z�b�g�A�b�v
		Material.Ambient = MirrorAmbientColor[MirrorNo];
		Material.Diffuse = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
		Material.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
		Material.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
		Material.Power = 1.0f;
		SetMaterialParam(Material);
		// ���̖ʂ̖@�����Z�o
		MirrorNormal = VNorm(VCross(VSub(MirrorWorldPosP[1], MirrorWorldPosP[0]), VSub(MirrorWorldPosP[2], MirrorWorldPosP[0])));
		// ���ɉf��f�����������񂾉摜�̃e�N�X�`���̃T�C�Y���擾
		GetGraphTextureSize(MirrorHandle[MirrorNo].get(), &TextureW, &TextureH);
		// ���̕`��Ɏg�p���钸�_�̃Z�b�g�A�b�v
		VUnitPos[0] = VScale(VSub(MirrorWorldPosP[2], MirrorWorldPosP[0]), 1.0f / (MIRROR_POINTNUM - 1));
		VUnitPos[1] = VScale(VSub(MirrorWorldPosP[3], MirrorWorldPosP[1]), 1.0f / (MIRROR_POINTNUM - 1));
		VUnitUV[0] = F4Scale(F4Sub(MirrorScreenPosW[MirrorNo][2], MirrorScreenPosW[MirrorNo][0]), 1.0f / (MIRROR_POINTNUM - 1));
		VUnitUV[1] = F4Scale(F4Sub(MirrorScreenPosW[MirrorNo][3], MirrorScreenPosW[MirrorNo][1]), 1.0f / (MIRROR_POINTNUM - 1));
		DiffuseColor = GetColorU8(MirrorDiffuseColor[MirrorNo][0], MirrorDiffuseColor[MirrorNo][1], MirrorDiffuseColor[MirrorNo][2], MirrorDiffuseColor[MirrorNo][3]);
		SpecularColor = GetColorU8(0, 0, 0, 0);
		VPos[0] = MirrorWorldPosP[0];
		VPos[1] = MirrorWorldPosP[1];
		VUV[0] = MirrorScreenPosW[MirrorNo][0];
		VUV[1] = MirrorScreenPosW[MirrorNo][1];
		k = 0;
		for (int i = 0; i < MIRROR_POINTNUM; i++) {
			HUnitPos = VScale(VSub(VPos[1], VPos[0]), 1.0f / (MIRROR_POINTNUM - 1));
			HPos = VPos[0];
			HUnitUV = F4Scale(F4Sub(VUV[1], VUV[0]), 1.0f / (MIRROR_POINTNUM - 1));
			HUV = VUV[0];
			for (int j = 0; j < MIRROR_POINTNUM; j++) {
				Vert[k].pos = HPos;
				Vert[k].norm = MirrorNormal;
				Vert[k].dif = DiffuseColor;
				Vert[k].spc = SpecularColor;
				Vert[k].u = HUV.x / (HUV.w * TextureW);
				Vert[k].v = HUV.y / (HUV.w * TextureH);
				Vert[k].su = 0.0f;
				Vert[k].sv = 0.0f;
				HUV = F4Add(HUV, HUnitUV);
				HPos = VAdd(HPos, HUnitPos);
				k++;
			}
			VUV[0] = F4Add(VUV[0], VUnitUV[0]);
			VUV[1] = F4Add(VUV[1], VUnitUV[1]);
			VPos[0] = VAdd(VPos[0], VUnitPos[0]);
			VPos[1] = VAdd(VPos[1], VUnitPos[1]);
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
		SetDrawBlendMode(MirrorBlendParam[MirrorNo][0], MirrorBlendParam[MirrorNo][1]);
		DrawPolygonIndexed3D(Vert, MIRROR_POINTNUM * MIRROR_POINTNUM, Index, (MIRROR_POINTNUM - 1) * (MIRROR_POINTNUM - 1) * 2, MirrorHandle[MirrorNo].get(), FALSE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		SetDrawMode(DX_DRAWMODE_NEAREST);
	}
};


// WinMain�֐�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int CameraAngle;		// �J�����̐����p�x
	VECTOR CameraEyePosition;	// �J�����̍��W
	VECTOR CameraDirection;	// �J�����̌����Ă������
	VECTOR CameraTargetPosition;	// �J�����̒����_
	int i;
	ChangeWindowMode(TRUE);
	SetGraphMode(SCREEN_W, SCREEN_H, 32);

	// �c�w���C�u�����̏�����
	DxLib_Init();

	auto Mirrarparts = std::make_unique<Mirrar_draw>();// �������̏�����

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
		if (CheckHitKey(KEY_INPUT_UP)){
			CameraEyePosition = VAdd(CameraEyePosition, VScale(CameraDirection, CAMERA_SPEED));
		}
		if (CheckHitKey(KEY_INPUT_DOWN)){
			CameraEyePosition = VSub(CameraEyePosition, VScale(CameraDirection, CAMERA_SPEED));
		}
		// �J�����̒����_���W���Z�o
		CameraTargetPosition = VAdd(CameraEyePosition, CameraDirection);
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
			SetCameraPositionAndTarget_UpVecY(CameraEyePosition, CameraTargetPosition);

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
