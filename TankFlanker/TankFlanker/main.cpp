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
// ３Ｄアクション基本
// 固定値定義 -------------------------------------------------------------------------------------
// プレイヤー関係の定義
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
PLAYER pl;		// プレイヤー情報の実体宣言
STAGE stg;		// ステージ情報の実体宣言
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
