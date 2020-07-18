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
// ３Ｄアクション基本
// 固定値定義 -------------------------------------------------------------------------------------
// プレイヤー関係の定義
#define PLAYER_PLAY_ANIM_SPEED			250.0f		// アニメーション速度
#define PLAYER_MOVE_SPEED			30.0f		// 移動速度
#define PLAYER_ANIM_BLEND_SPEED			0.1f		// アニメーションのブレンド率変化速度
#define PLAYER_ANGLE_SPEED			0.2f		// 角度変化速度
#define PLAYER_JUMP_POWER			100.0f		// ジャンプ力
#define PLAYER_FALL_UP_POWER			20.0f		// 足を踏み外した時のジャンプ力
#define PLAYER_GRAVITY				3.0f		// 重力
#define PLAYER_MAX_HITCOLL			2048		// 処理するコリジョンポリゴンの最大数
#define PLAYER_ENUM_DEFAULT_SIZE		800.0f		// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_HIT_WIDTH			200.0f		// 当たり判定カプセルの半径
#define PLAYER_HIT_HEIGHT			700.0f		// 当たり判定カプセルの高さ
#define PLAYER_HIT_TRYNUM			16		// 壁押し出し処理の最大試行回数
#define PLAYER_HIT_SLIDE_LENGTH			5.0f		// 一度の壁押し出し処理でスライドさせる距離
#define PLAYER_SHADOW_SIZE			200.0f		// 影の大きさ
#define PLAYER_SHADOW_HEIGHT			700.0f		// 影が落ちる高さ
// カメラ関係の定義
#define CAMERA_ANGLE_SPEED			0.05f		// 旋回速度
#define CAMERA_PLAYER_TARGET_HEIGHT		400.0f		// プレイヤー座標からどれだけ高い位置を注視点とするか
#define CAMERA_PLAYER_LENGTH			1600.0f		// プレイヤーとの距離
#define CAMERA_COLLISION_SIZE			50.0f		// カメラの当たり判定サイズ
// 構造体定義 -------------------------------------------------------------------------------------
// 入力情報構造体
struct PADINPUT
{
	int		NowInput;				// 現在の入力
	int		EdgeInput;				// 現在のフレームで押されたボタンのみビットが立っている入力値
};
// プレイヤー情報構造体
struct PLAYER
{
	VECTOR		Position;				// 座標
	VECTOR		TargetMoveDirection;			// モデルが向くべき方向のベクトル
	float		Angle;					// モデルが向いている方向の角度
	float		JumpPower;				// Ｙ軸方向の速度
	int		ModelHandle;				// モデルハンドル
	int		ShadowHandle;				// 影画像ハンドル
	int		State;					// 状態( 0:立ち止まり  1:走り  2:ジャンプ )

	int		PlayAnim1;				// 再生しているアニメーション１のアタッチ番号( -1:何もアニメーションがアタッチされていない )
	float		AnimPlayCount1;			// 再生しているアニメーション１の再生時間
	int		PlayAnim2;				// 再生しているアニメーション２のアタッチ番号( -1:何もアニメーションがアタッチされていない )
	float		AnimPlayCount2;			// 再生しているアニメーション２の再生時間
	float		AnimBlendRate;				// 再生しているアニメーション１と２のブレンド率
};
// ステージ情報構造体
struct STAGE
{
	MV1		ModelHandle;				// モデルハンドル
};
// カメラ情報構造体
struct CAMERA
{
	float		AngleH;				// 水平角度
	float		AngleV;				// 垂直角度
	VECTOR		Eye;					// カメラ座標
	VECTOR		Target;				// 注視点座標
};
// 実体宣言 ---------------------------------------------------------------------------------------
PADINPUT inp;		// 入力情報の実体宣言
PLAYER pl;		// プレイヤー情報の実体宣言
STAGE stg;		// ステージ情報の実体宣言
CAMERA cam;		// カメラ情報の実体宣言
void Player_PlayAnim(int PlayAnim);
void Player_AngleProcess(void);
void Player_Move(VECTOR_ref MoveVector);
void Player_AnimProcess(void);
void Player_Terminate(void){// プレイヤーの後始末
	MV1DeleteModel(pl.ModelHandle);	// モデルの削除
	DeleteGraph(pl.ShadowHandle);	// 影用画像の削除
}
void Player_Process(void){// プレイヤーの処理
	VECTOR UpMoveVec;		// 方向ボタン「↑」を入力をしたときのプレイヤーの移動方向ベクトル
	VECTOR LeftMoveVec;		// 方向ボタン「←」を入力をしたときのプレイヤーの移動方向ベクトル
	VECTOR MoveVec;			// このフレームの移動ベクトル
	int MoveFlag;			// 移動したかどうかのフラグ( 1:移動した  0:移動していない )
	// ルートフレームのＺ軸方向の移動パラメータを無効にする
	{
		MATRIX LocalMatrix;
		MV1ResetFrameUserLocalMatrix(pl.ModelHandle, 2);		// ユーザー行列を解除する
		LocalMatrix = MV1GetFrameLocalMatrix(pl.ModelHandle, 2);		// 現在のルートフレームの行列を取得する
		LocalMatrix.m[3][2] = 0.0f;		// Ｚ軸方向の平行移動成分を無効にする
		MV1SetFrameUserLocalMatrix(pl.ModelHandle, 2, LocalMatrix);		// ユーザー行列として平行移動成分を無効にした行列をルートフレームにセットする
	}
	// プレイヤーの移動方向のベクトルを算出
	{
		UpMoveVec = VSub(cam.Target, cam.Eye);		// 方向ボタン「↑」を押したときのプレイヤーの移動ベクトルはカメラの視線方向からＹ成分を抜いたもの
		UpMoveVec.y = 0.0f;
		LeftMoveVec = VCross(UpMoveVec, VGet(0.0f, 1.0f, 0.0f));		// 方向ボタン「←」を押したときのプレイヤーの移動ベクトルは上を押したときの方向ベクトルとＹ軸のプラス方向のベクトルに垂直な方向
		// 二つのベクトルを正規化( ベクトルの長さを１．０にすること )
		UpMoveVec = VNorm(UpMoveVec);
		LeftMoveVec = VNorm(LeftMoveVec);
	}
	MoveVec = VGet(0.0f, 0.0f, 0.0f);	// このフレームでの移動ベクトルを初期化
	MoveFlag = 0;	// 移動したかどうかのフラグを初期状態では「移動していない」を表す０にする
	if (CheckHitKey(KEY_INPUT_LSHIFT) == 0 && (inp.NowInput & PAD_INPUT_C) == 0){	// パッドの３ボタンと左シフトがどちらも押されていなかったらプレイヤーの移動処理
		if (inp.NowInput & PAD_INPUT_LEFT){		// 方向ボタン「←」が入力されたらカメラの見ている方向から見て左方向に移動する
			MoveVec = VAdd(MoveVec, LeftMoveVec);			// 移動ベクトルに「←」が入力された時の移動ベクトルを加算する
			MoveFlag = 1;			// 移動したかどうかのフラグを「移動した」にする
		}
		else if (inp.NowInput & PAD_INPUT_RIGHT) {			// 方向ボタン「→」が入力されたらカメラの見ている方向から見て右方向に移動する
			MoveVec = VAdd(MoveVec, VScale(LeftMoveVec, -1.0f));				// 移動ベクトルに「←」が入力された時の移動ベクトルを反転したものを加算する
			MoveFlag = 1;				// 移動したかどうかのフラグを「移動した」にする
		}
		if (inp.NowInput & PAD_INPUT_UP){		// 方向ボタン「↑」が入力されたらカメラの見ている方向に移動する
			MoveVec = VAdd(MoveVec, UpMoveVec);			// 移動ベクトルに「↑」が入力された時の移動ベクトルを加算する
			MoveFlag = 1;			// 移動したかどうかのフラグを「移動した」にする
		}
		else if (inp.NowInput & PAD_INPUT_DOWN) {			// 方向ボタン「↓」が入力されたらカメラの方向に移動する
			MoveVec = VAdd(MoveVec, VScale(UpMoveVec, -1.0f));				// 移動ベクトルに「↑」が入力された時の移動ベクトルを反転したものを加算する
			MoveFlag = 1;			// 移動したかどうかのフラグを「移動した」にする
		}
		if (pl.State != 2 && (inp.EdgeInput & PAD_INPUT_A)){		// プレイヤーの状態が「ジャンプ」ではなく、且つボタン１が押されていたらジャンプする
			pl.State = 2;			// 状態を「ジャンプ」にする
			pl.JumpPower = PLAYER_JUMP_POWER;			// Ｙ軸方向の速度をセット
			Player_PlayAnim(2);			// ジャンプアニメーションの再生
		}
	}

	// 移動ボタンが押されたかどうかで処理を分岐
	if (MoveFlag){
		// 移動ベクトルを正規化したものをプレイヤーが向くべき方向として保存
		pl.TargetMoveDirection = VNorm(MoveVec);

		// プレイヤーが向くべき方向ベクトルをプレイヤーのスピード倍したものを移動ベクトルとする
		MoveVec = VScale(pl.TargetMoveDirection, PLAYER_MOVE_SPEED);

		// もし今まで「立ち止まり」状態だったら
		if (pl.State == 0)
		{
			// 走りアニメーションを再生する
			Player_PlayAnim(1);

			// 状態を「走り」にする
			pl.State = 1;
		}
	}
	else
	{
		// このフレームで移動していなくて、且つ状態が「走り」だったら
		if (pl.State == 1)
		{
			// 立ち止りアニメーションを再生する
			Player_PlayAnim(4);

			// 状態を「立ち止り」にする
			pl.State = 0;
		}
	}

	// 状態が「ジャンプ」の場合は
	if (pl.State == 2)
	{
		// Ｙ軸方向の速度を重力分減算する
		pl.JumpPower -= PLAYER_GRAVITY;

		// もし落下していて且つ再生されているアニメーションが上昇中用のものだった場合は
		if (pl.JumpPower < 0.0f && MV1GetAttachAnim(pl.ModelHandle, pl.PlayAnim1) == 2)
		{
			// 落下中ようのアニメーションを再生する
			Player_PlayAnim(3);
		}

		// 移動ベクトルのＹ成分をＹ軸方向の速度にする
		MoveVec.y = pl.JumpPower;
	}

	// プレイヤーの移動方向にモデルの方向を近づける
	Player_AngleProcess();

	// 移動ベクトルを元にコリジョンを考慮しつつプレイヤーを移動
	Player_Move(MoveVec);

	// アニメーション処理
	Player_AnimProcess();
}
// 関数宣言 ---------------------------------------------------------------------------
void Input_Process(void) {// 入力処理
	int Old;
	Old = inp.NowInput;	// ひとつ前のフレームの入力を変数にとっておく
	inp.NowInput = GetJoypadInputState(DX_INPUT_KEY_PAD1);	// 現在の入力状態を取得
	inp.EdgeInput = inp.NowInput & ~Old;	// 今のフレームで新たに押されたボタンのビットだけ立っている値を EdgeInput に代入する
}
void Player_Initialize(void) {// プレイヤーの初期化
	pl.Position = VGet(0.0f, 0.0f, 0.0f);	// 初期座標は原点
	pl.Angle = 0.0f;	// 回転値は０
	pl.JumpPower = 0.0f;	// ジャンプ力は初期状態では０
	pl.ModelHandle = MV1LoadModel("DxChara.x");	// モデルの読み込み
	pl.ShadowHandle = LoadGraph("Shadow.tga");	// 影描画用の画像の読み込み
	pl.State = 0;	// 初期状態では「立ち止り」状態
	pl.TargetMoveDirection = VGet(1.0f, 0.0f, 0.0f);	// 初期状態でプレイヤーが向くべき方向はＸ軸方向
	pl.AnimBlendRate = 1.0f;	// アニメーションのブレンド率を初期化
	pl.PlayAnim1 = -1;	// 初期状態ではアニメーションはアタッチされていないにする
	pl.PlayAnim2 = -1;	// 初期状態ではアニメーションはアタッチされていないにする
	Player_PlayAnim(4);	// ただ立っているアニメーションを再生
}
// プレイヤーの移動処理
void Player_Move(VECTOR_ref MoveVector) {
	VECTOR_ref OldPos = pl.Position;			// 移動前の座標
	VECTOR_ref NowPos = OldPos + MoveVector;	// 移動後の座標
	// x軸かy軸方向に 0.01f 以上移動した場合は「移動した」フラグを１にする
	bool MoveFlag = (abs(MoveVector.x()) > 0.01f || abs(MoveVector.z()) > 0.01f);	
	// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
	auto HitDim = stg.ModelHandle.CollCheck_Sphere(pl.Position, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), -1);
	std::vector<MV1_COLL_RESULT_POLY*> kabe_;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
	// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
	for (int i = 0; i < HitDim.HitNum; i++) {
		auto& h_d = HitDim.Dim[i];
		// ＸＺ平面に垂直かどうかはポリゴンの法線のＹ成分が０に限りなく近いかどうかで判断する 壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋１．０ｆより高いポリゴンのみ当たり判定を行う
		if (
			(h_d.Normal.y < 0.000001f && h_d.Normal.y > -0.000001f) &&
			(h_d.Position[0].y > pl.Position.y + 1.0f || h_d.Position[1].y > pl.Position.y + 1.0f || h_d.Position[2].y > pl.Position.y + 1.0f)
			) {
			kabe_.resize(kabe_.size() + 1);
			kabe_.back() = &h_d;						// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
		}
	}
	// 壁ポリゴンとの当たり判定処理
	if (kabe_.size() > 0) {
		bool HitFlag = false;
		for (auto& k_ : kabe_) {
			if (HitCheck_Capsule_Triangle(NowPos.get(), (NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)).get(), PLAYER_HIT_WIDTH, k_->Position[0], k_->Position[1], k_->Position[2]) == TRUE) {				// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
				HitFlag = true;// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
				if (MoveFlag) {
					// 壁に当たったら壁に遮られない移動成分分だけ移動する
					VECTOR_ref SlideVec = k_->Normal;
					NowPos = OldPos + SlideVec.cross(MoveVector.cross(SlideVec));
					bool j = false;
					for (auto& b_ : kabe_) {
						if (HitCheck_Capsule_Triangle(NowPos.get(), (NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)).get(), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2]) == TRUE) {
							j = true;
							break;// 当たっていたらループから抜ける
						}
					}
					if (!j) {
						HitFlag = false;
						break;//どのポリゴンとも当たらなかったということなので壁に当たったフラグを倒した上でループから抜ける
					}
				}
				else {
					break;
				}
			}
		}
		if (HitFlag) {		// 壁に当たっていたら壁から押し出す処理を行う
			for (int k = 0; k < PLAYER_HIT_TRYNUM; k++) {			// 壁からの押し出し処理を試みる最大数だけ繰り返し
				bool i = false;
				for (auto& k_ : kabe_) {
					if (HitCheck_Capsule_Triangle(NowPos.get(), (NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)).get(), PLAYER_HIT_WIDTH, k_->Position[0], k_->Position[1], k_->Position[2]) == TRUE) {// プレイヤーと当たっているかを判定
						VECTOR_ref SlideVec = k_->Normal;
						NowPos += SlideVec*PLAYER_HIT_SLIDE_LENGTH;					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
						int j = false;
						for (auto& b_ : kabe_) {
							if (HitCheck_Capsule_Triangle(NowPos.get(), (NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f)).get(), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2]) == TRUE) {						// 当たっていたらループを抜ける
								j = true;
								break;
							}
						}
						if (!j) {// 全てのポリゴンと当たっていなかったらここでループ終了
							break;
						}
					}
					i=true;
				}
				if (!i) {//全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
					break;
				}
			}
		}
	}
	MV1CollResultPolyDimTerminate(HitDim);	// 検出したプレイヤーの周囲のポリゴン情報を開放する

	pl.Position = NowPos.get();	// 新しい座標を保存する
	MV1SetPosition(pl.ModelHandle, pl.Position);	// プレイヤーのモデルの座標を更新する
	kabe_.clear();
}
// プレイヤーの向きを変える処理
void Player_AngleProcess(void)
{
	float TargetAngle;			// 目標角度
	float SaAngle;				// 目標角度と現在の角度との差

	// 目標の方向ベクトルから角度値を算出する
	TargetAngle = atan2(pl.TargetMoveDirection.x, pl.TargetMoveDirection.z);

	// 目標の角度と現在の角度との差を割り出す
	{
		// 最初は単純に引き算
		SaAngle = TargetAngle - pl.Angle;

		// ある方向からある方向の差が１８０度以上になることは無いので
		// 差の値が１８０度以上になっていたら修正する
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

	// 角度の差が０に近づける
	if (SaAngle > 0.0f)
	{
		// 差がプラスの場合は引く
		SaAngle -= PLAYER_ANGLE_SPEED;
		if (SaAngle < 0.0f)
		{
			SaAngle = 0.0f;
		}
	}
	else
	{
		// 差がマイナスの場合は足す
		SaAngle += PLAYER_ANGLE_SPEED;
		if (SaAngle > 0.0f)
		{
			SaAngle = 0.0f;
		}
	}

	// モデルの角度を更新
	pl.Angle = TargetAngle - SaAngle;
	MV1SetRotationXYZ(pl.ModelHandle, VGet(0.0f, pl.Angle + DX_PI_F, 0.0f));
}
// プレイヤーに新たなアニメーションを再生する
void Player_PlayAnim(int PlayAnim)
{
	// 再生中のモーション２が有効だったらデタッチする
	if (pl.PlayAnim2 != -1)
	{
		MV1DetachAnim(pl.ModelHandle, pl.PlayAnim2);
		pl.PlayAnim2 = -1;
	}

	// 今まで再生中のモーション１だったものの情報を２に移動する
	pl.PlayAnim2 = pl.PlayAnim1;
	pl.AnimPlayCount2 = pl.AnimPlayCount1;

	// 新たに指定のモーションをモデルにアタッチして、アタッチ番号を保存する
	pl.PlayAnim1 = MV1AttachAnim(pl.ModelHandle, PlayAnim);
	pl.AnimPlayCount1 = 0.0f;

	// ブレンド率は再生中のモーション２が有効ではない場合は１．０ｆ( 再生中のモーション１が１００％の状態 )にする
	pl.AnimBlendRate = pl.PlayAnim2 == -1 ? 1.0f : 0.0f;
}
// プレイヤーのアニメーション処理
void Player_AnimProcess(void)
{
	float AnimTotalTime;		// 再生しているアニメーションの総時間

	// ブレンド率が１以下の場合は１に近づける
	if (pl.AnimBlendRate < 1.0f)
	{
		pl.AnimBlendRate += PLAYER_ANIM_BLEND_SPEED;
		if (pl.AnimBlendRate > 1.0f)
		{
			pl.AnimBlendRate = 1.0f;
		}
	}

	// 再生しているアニメーション１の処理
	if (pl.PlayAnim1 != -1)
	{
		// アニメーションの総時間を取得
		AnimTotalTime = MV1GetAttachAnimTotalTime(pl.ModelHandle, pl.PlayAnim1);

		// 再生時間を進める
		pl.AnimPlayCount1 += PLAYER_PLAY_ANIM_SPEED;

		// 再生時間が総時間に到達していたら再生時間をループさせる
		if (pl.AnimPlayCount1 >= AnimTotalTime)
		{
			pl.AnimPlayCount1 = fmod(pl.AnimPlayCount1, AnimTotalTime);
		}

		// 変更した再生時間をモデルに反映させる
		MV1SetAttachAnimTime(pl.ModelHandle, pl.PlayAnim1, pl.AnimPlayCount1);

		// アニメーション１のモデルに対する反映率をセット
		MV1SetAttachAnimBlendRate(pl.ModelHandle, pl.PlayAnim1, pl.AnimBlendRate);
	}

	// 再生しているアニメーション２の処理
	if (pl.PlayAnim2 != -1)
	{
		// アニメーションの総時間を取得
		AnimTotalTime = MV1GetAttachAnimTotalTime(pl.ModelHandle, pl.PlayAnim2);

		// 再生時間を進める
		pl.AnimPlayCount2 += PLAYER_PLAY_ANIM_SPEED;

		// 再生時間が総時間に到達していたら再生時間をループさせる
		if (pl.AnimPlayCount2 > AnimTotalTime)
		{
			pl.AnimPlayCount2 = fmod(pl.AnimPlayCount2, AnimTotalTime);
		}

		// 変更した再生時間をモデルに反映させる
		MV1SetAttachAnimTime(pl.ModelHandle, pl.PlayAnim2, pl.AnimPlayCount2);

		// アニメーション２のモデルに対する反映率をセット
		MV1SetAttachAnimBlendRate(pl.ModelHandle, pl.PlayAnim2, 1.0f - pl.AnimBlendRate);
	}
}
// プレイヤーの影を描画
void Player_ShadowRender(void){
	int i;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY *HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;
	SetUseLighting(FALSE);	// ライティングを無効にする
	SetUseZBuffer3D(TRUE);	// Ｚバッファを有効にする
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);	// テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	// プレイヤーの直下に存在する地面のポリゴンを取得
	HitResDim = MV1CollCheck_Capsule(stg.ModelHandle, -1, pl.Position, VAdd(pl.Position, VGet(0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f)), PLAYER_SHADOW_SIZE);
	// 頂点データで変化が無い部分をセット
	Vertex[0].dif = GetColorU8(255, 255, 255, 255);
	Vertex[0].spc = GetColorU8(0, 0, 0, 0);
	Vertex[0].su = 0.0f;
	Vertex[0].sv = 0.0f;
	Vertex[1] = Vertex[0];
	Vertex[2] = Vertex[0];
	// 球の直下に存在するポリゴンの数だけ繰り返し
	HitRes = HitResDim.Dim;
	for (i = 0; i < HitResDim.HitNum; i++, HitRes++){
		// ポリゴンの座標は地面ポリゴンの座標
		Vertex[0].pos = HitRes->Position[0];
		Vertex[1].pos = HitRes->Position[1];
		Vertex[2].pos = HitRes->Position[2];
		// ちょっと持ち上げて重ならないようにする
		SlideVec = VScale(HitRes->Normal, 0.5f);
		Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
		Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
		Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);
		// ポリゴンの不透明度を設定する
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
		// ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
		Vertex[0].u = (HitRes->Position[0].x - pl.Position.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[0].v = (HitRes->Position[0].z - pl.Position.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[1].u = (HitRes->Position[1].x - pl.Position.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[1].v = (HitRes->Position[1].z - pl.Position.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[2].u = (HitRes->Position[2].x - pl.Position.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		Vertex[2].v = (HitRes->Position[2].z - pl.Position.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
		// 影ポリゴンを描画
		DrawPolygon3D(Vertex, 1, pl.ShadowHandle, TRUE);
	}
	MV1CollResultPolyDimTerminate(HitResDim);	// 検出した地面ポリゴン情報の後始末
	SetUseLighting(TRUE);	// ライティングを有効にする
	SetUseZBuffer3D(FALSE);	// Ｚバッファを無効にする
}
// ステージの初期化処理
void Stage_Initialize(void){
	stg.ModelHandle = MV1LoadModel("ColTestStage.mqo");	// ステージモデルの読み込み
	MV1SetupCollInfo(stg.ModelHandle, -1);	// モデル全体のコリジョン情報のセットアップ
}
// ステージの後始末処理
void Stage_Terminate(void){
	MV1DeleteModel(stg.ModelHandle);	// ステージモデルの後始末
}
// カメラの初期化処理
void Camera_Initialize(void){
	cam.AngleH = DX_PI_F;	// カメラの初期水平角度は１８０度
	cam.AngleV = 0.0f;	// 垂直角度は０度
}
// カメラの処理
void Camera_Process(void){
	if (CheckHitKey(KEY_INPUT_LSHIFT) || (inp.NowInput & PAD_INPUT_C)){	// パッドの３ボタンか、シフトキーが押されている場合のみ角度変更操作を行う
		if (inp.NowInput & PAD_INPUT_LEFT){		// 「←」ボタンが押されていたら水平角度をマイナスする
			cam.AngleH -= CAMERA_ANGLE_SPEED;
			if (cam.AngleH < -DX_PI_F){			// －１８０度以下になったら角度値が大きくなりすぎないように３６０度を足す
				cam.AngleH += DX_TWO_PI_F;
			}
		}
		if (inp.NowInput & PAD_INPUT_RIGHT){		// 「→」ボタンが押されていたら水平角度をプラスする
			cam.AngleH += CAMERA_ANGLE_SPEED;
			if (cam.AngleH > DX_PI_F){			// １８０度以上になったら角度値が大きくなりすぎないように３６０度を引く
				cam.AngleH -= DX_TWO_PI_F;
			}
		}
		if (inp.NowInput & PAD_INPUT_UP){		// 「↑」ボタンが押されていたら垂直角度をマイナスする
			cam.AngleV -= CAMERA_ANGLE_SPEED;
			if (cam.AngleV < -DX_PI_F / 2.0f + 0.6f){			// ある一定角度以下にはならないようにする
				cam.AngleV = -DX_PI_F / 2.0f + 0.6f;
			}
		}
		if (inp.NowInput & PAD_INPUT_DOWN) {		// 「↓」ボタンが押されていたら垂直角度をプラスする
			cam.AngleV += CAMERA_ANGLE_SPEED;
			if (cam.AngleV > DX_PI_F / 2.0f - 0.6f) {			// ある一定角度以上にはならないようにする
				cam.AngleV = DX_PI_F / 2.0f - 0.6f;
			}
		}
	}

	// カメラの注視点はプレイヤー座標から規定値分高い座標
	cam.Target = VAdd(pl.Position, VGet(0.0f, CAMERA_PLAYER_TARGET_HEIGHT, 0.0f));

	// カメラの座標を決定する
	{
		MATRIX RotZ, RotY;
		float Camera_Player_Length;
		MV1_COLL_RESULT_POLY_DIM HRes;
		int HitNum;
		RotY = MGetRotY(cam.AngleH);		// 水平方向の回転はＹ軸回転
		RotZ = MGetRotZ(cam.AngleV);		// 垂直方向の回転はＺ軸回転 )
		Camera_Player_Length = CAMERA_PLAYER_LENGTH;		// カメラからプレイヤーまでの初期距離をセット

		cam.Eye = VAdd(VTransform(VTransform(VGet(-Camera_Player_Length, 0.0f, 0.0f), RotZ), RotY), cam.Target);		// カメラの座標を算出 Ｘ軸にカメラとプレイヤーとの距離分だけ伸びたベクトルを垂直方向回転( Ｚ軸回転 )させたあと水平方向回転( Ｙ軸回転 )して更に注視点の座標を足したものがカメラの座標
		HRes = MV1CollCheck_Capsule(stg.ModelHandle, -1, cam.Target, cam.Eye, CAMERA_COLLISION_SIZE);		// 注視点からカメラの座標までの間にステージのポリゴンがあるか調べる
		HitNum = HRes.HitNum;
		MV1CollResultPolyDimTerminate(HRes);
		if (HitNum != 0){
			float NotHitLength;
			float HitLength;
			float TestLength;
			VECTOR TestPosition;
			// あったら無い位置までプレイヤーに近づく
			NotHitLength = 0.0f;			// ポリゴンに当たらない距離をセット
			HitLength = Camera_Player_Length;			// ポリゴンに当たる距離をセット
			do{
				TestLength = NotHitLength + (HitLength - NotHitLength) / 2.0f;				// 当たるかどうかテストする距離をセット( 当たらない距離と当たる距離の中間 )
				TestPosition = VAdd(VTransform(VTransform(VGet(-TestLength, 0.0f, 0.0f), RotZ), RotY), cam.Target);				// テスト用のカメラ座標を算出
				HRes = MV1CollCheck_Capsule(stg.ModelHandle, -1, cam.Target, TestPosition, CAMERA_COLLISION_SIZE);				// 新しい座標で壁に当たるかテスト
				HitNum = HRes.HitNum;
				MV1CollResultPolyDimTerminate(HRes);
				if (HitNum != 0){
					HitLength = TestLength;					// 当たったら当たる距離を TestLength に変更する
				}
				else{
					NotHitLength = TestLength;					// 当たらなかったら当たらない距離を TestLength に変更する
				}
			} while (HitLength - NotHitLength > 0.1f);				// HitLength と NoHitLength が十分に近づいていなかったらループ
			cam.Eye = TestPosition;			// カメラの座標をセット
		}
	}
	SetCameraPositionAndTarget_UpVecY(cam.Eye, cam.Target);	// カメラの情報をライブラリのカメラに反映させる
}
// 描画処理
void Render_Process(void){
	MV1DrawModel(stg.ModelHandle);	// ステージモデルの描画
	MV1DrawModel(pl.ModelHandle);	// プレイヤーモデルの描画
	Player_ShadowRender();	// プレイヤーの影の描画
}
// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	ChangeWindowMode(TRUE);
	DxLib_Init();
	Player_Initialize();	// プレイヤーの初期化
	Stage_Initialize();	// ステージの初期化
	Camera_Initialize();	// カメラの初期化
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0){
		ClearDrawScreen();
		SetDrawScreen(DX_SCREEN_BACK);
		{
			Input_Process();		// 入力処理
			Player_Process();		// プレイヤーの処理
			Camera_Process();		// カメラの処理
			Render_Process();		// 描画処理
		}
		ScreenFlip();		// 裏画面の内容を表画面に反映
	}
	Player_Terminate();	// プレイヤーの後始末
	Stage_Terminate();	// ステージの後始末
	DxLib_End();	// ライブラリの後始末
	return 0;
}