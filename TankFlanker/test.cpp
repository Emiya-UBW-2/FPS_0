// 鏡面効果基本

#include "DXLib_ref/DxLib_ref.h"


#define SCREEN_W		640		// 画面の横幅
#define SCREEN_H		480		// 画面の縦幅

#define MIRROR_SCREEN_W		640		// 鏡に映る映像の取得に使用するスクリーンの横解像度
#define MIRROR_SCREEN_H		480		// 鏡に映る映像の取得に使用するスクリーンの縦解像度
#define MIRROR_POINTNUM		64		// 鏡の描画に使用する面の頂点分割数
#define MIRROR_NUM		2		// 鏡の数
#define MIRROR_DEBUG_SCALE	4		// 鏡のデバッグ表示時に元の何分の１にするかのサイズ

#define CAMERA_SPEED		32.0f		// カメラの速度


int CharaModel;		// キャラクターモデル
int StageModel;		// ステージモデル

struct Mirror_mod{
	VECTOR_ref WorldPos[4];		// 鏡のワールド座標
	COLOR_F AmbientColor;		// 鏡の Ambient Color
	int DiffuseColor[4];		// 鏡の Diffuse Color
	int BlendParam[2];			// 鏡のブレンドモードとパラメータ
	GraphHandle Handle;	// 鏡に映る映像の取得に使用するスクリーン
	FLOAT4 ScreenPosW[4];	// 鏡に映る映像の取得に使用するクリーンの中の鏡の四隅の座標( 同次座標 )
};

// ３Ｄ空間の描画
void World_RenderProcess(){
	// ステージモデルの描画
	MV1DrawModel(StageModel);
	// キャラクターモデルの描画
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

	// 鏡の初期化
	void Mirror_Initialize(void) {
		for (int i = 0; i < MIRROR_NUM; i++) {
			Mirror_obj[i].Handle = GraphHandle::Make(MIRROR_SCREEN_W, MIRROR_SCREEN_H, FALSE);	// 鏡に映る映像の取得に使用するスクリーンの作成
		}
	}
	//
	Mirrar_draw() {
		Mirror_Initialize();
	}
	~Mirrar_draw() {
	}

	// 鏡に映る映像を描画するためのカメラの設定を行う
	void Mirror_SetupCamera(int MirrorNo, VECTOR_ref CameraEyePos, VECTOR_ref CameraTargetPos) {
		float EyeLength, TargetLength;
		VECTOR_ref MirrorNormal;
		VECTOR_ref *MirrorWorldPosP= Mirror_obj[MirrorNo].WorldPos;
		VECTOR_ref MirrorCameraEyePos, MirrorCameraTargetPos;
		// 鏡の面の法線を算出
		MirrorNormal = ((MirrorWorldPosP[1]-MirrorWorldPosP[0]).cross(MirrorWorldPosP[2]-MirrorWorldPosP[0])).Norm();
		// 鏡の面からカメラの座標までの最短距離、鏡の面からカメラの注視点までの最短距離を算出
		EyeLength = Plane_Point_MinLength(MirrorWorldPosP[0].get(), MirrorNormal.get(), CameraEyePos.get());
		TargetLength = Plane_Point_MinLength(MirrorWorldPosP[0].get(), MirrorNormal.get(), CameraTargetPos.get());
		// 鏡に映る映像を描画する際に使用するカメラの座標とカメラの注視点を算出
		MirrorCameraEyePos = CameraEyePos + MirrorNormal*(-EyeLength * 2.0f);
		MirrorCameraTargetPos = CameraTargetPos + MirrorNormal*(-TargetLength * 2.0f);
		// 計算で得られたカメラの座標とカメラの注視点の座標をカメラの設定する
		SetCameraPositionAndTarget_UpVecY(MirrorCameraEyePos.get(), MirrorCameraTargetPos.get());
		// 鏡に映る映像の中での鏡の四隅の座標を算出( 同次座標 )
		for (int i = 0; i < 4; i++) {
			Mirror_obj[MirrorNo].ScreenPosW[i] = ConvWorldPosToScreenPosPlusW(MirrorWorldPosP[i].get());
		}
	}

	// 鏡の描画
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
		// 鏡の描画に使用するマテリアルのセットアップ
		Material.Ambient = Mirror_obj[MirrorNo].AmbientColor;
		Material.Diffuse = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
		Material.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
		Material.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
		Material.Power = 1.0f;
		SetMaterialParam(Material);
		// 鏡の面の法線を算出
		MirrorNormal = ((MirrorWorldPosP[1] - MirrorWorldPosP[0]).cross(MirrorWorldPosP[2]- MirrorWorldPosP[0])).Norm();
		// 鏡に映る映像を書き込んだ画像のテクスチャのサイズを取得
		GetGraphTextureSize(Mirror_obj[MirrorNo].Handle.get(), &TextureW, &TextureH);
		// 鏡の描画に使用する頂点のセットアップ
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

		// 鏡の描画に使用する頂点インデックスをセットアップ
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
		// 鏡を描画
		SetDrawMode(DX_DRAWMODE_BILINEAR);
		SetDrawBlendMode(Mirror_obj[MirrorNo].BlendParam[0], Mirror_obj[MirrorNo].BlendParam[1]);
		DrawPolygonIndexed3D(Vert, MIRROR_POINTNUM * MIRROR_POINTNUM, Index, (MIRROR_POINTNUM - 1) * (MIRROR_POINTNUM - 1) * 2, Mirror_obj[MirrorNo].Handle.get(), FALSE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		SetDrawMode(DX_DRAWMODE_NEAREST);
	}
};


// WinMain関数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int CameraAngle;		// カメラの水平角度
	VECTOR_ref CameraEyePosition;	// カメラの座標
	VECTOR_ref CameraDirection;	// カメラの向いている方向
	VECTOR_ref CameraTargetPosition;	// カメラの注視点
	int i;
	ChangeWindowMode(TRUE);
	SetGraphMode(SCREEN_W, SCREEN_H, 32);
	DxLib_Init();
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	//
	auto Mirrarparts = std::make_unique<Mirrar_draw>();// 鏡処理の初期化

	
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

	// メインループ(何かキーが押されたらループを抜ける)
	while (ProcessMessage() == 0){
		// 左右キーが押されたらカメラを回転する
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
		// カメラの向いている方向を算出
		CameraDirection = VGet(cos(CameraAngle * 3.14159f / 180.0f),0.0f,sin(CameraAngle * 3.14159f / 180.0f));
		// 上下キーが押されたらカメラを前進・後進させる
		if (CheckHitKey(KEY_INPUT_UP)) {
			CameraEyePosition += CameraDirection * CAMERA_SPEED;
		}
		if (CheckHitKey(KEY_INPUT_DOWN)) {
			CameraEyePosition -= CameraDirection * CAMERA_SPEED;
		}
		// カメラの注視点座標を算出
		CameraTargetPosition = CameraEyePosition+CameraDirection;
		// 鏡に映る映像を描画
		for (i = 0; i < MIRROR_NUM; i++){
			Mirrarparts->get_MirrorHandle(i).SetDraw_Screen();
			{
				Mirrarparts->Mirror_SetupCamera(i, CameraEyePosition, CameraTargetPosition);	// 鏡に映る映像を描画する際に使用するカメラの設定を行う

				World_RenderProcess();											// ３Ｄ空間の描画
			}
		}
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		{
			SetCameraPositionAndTarget_UpVecY(CameraEyePosition.get(), CameraTargetPosition.get());

			World_RenderProcess();		// ３Ｄ空間の描画
			for (i = 0; i < MIRROR_NUM; i++) {
				Mirrarparts->Mirror_Render(i);		// 鏡の描画
			}
		}
		ScreenFlip();
	}
	DxLib_End();
	return 0;
}
