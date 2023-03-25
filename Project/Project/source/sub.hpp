#pragma once
#include	"Header.hpp"

//Box2D拡張
namespace std {
	template <>
	struct default_delete<b2Body> {
		void		operator()(b2Body* body) const noexcept {
			body->GetWorld()->DestroyBody(body);
		}
	};
}; // namespace std
//
namespace FPS_n2 {
	//BOX2D
	static auto* CreateB2Body(std::shared_ptr<b2World>& world, b2BodyType type, float32 x_, float32 y_, float angle = 0) noexcept {
		b2BodyDef f_bodyDef;
		f_bodyDef.type = type;
		f_bodyDef.position.Set(x_, y_);
		f_bodyDef.angle = angle;
		return world->CreateBody(&f_bodyDef);
	}
	class b2Pats {
	private:
		b2FixtureDef				fixtureDef;				//動的ボディフィクスチャを定義します
		std::unique_ptr<b2Body>		body;					//
		b2Fixture*					playerfix{ nullptr };	//
	public:
		VECTOR_ref					pos;//仮
	public:
		void		Set(b2Body* body_ptr, b2Shape* dynamicBox) {
			fixtureDef.shape = dynamicBox;								//
			fixtureDef.density = 1.0f;									//ボックス密度をゼロ以外に設定すると、動的になる
			fixtureDef.friction = 0.3f;									//デフォルトの摩擦をオーバーライド
			this->body.reset(body_ptr);									//
			this->playerfix = this->body->CreateFixture(&fixtureDef);	//シェイプをボディに追加
		}

		void		SetLinearVelocity(const b2Vec2& position) {
			this->body->SetLinearVelocity(position);
		}

		void		Execute(const VECTOR_ref& add, float yradadd) {
			this->body->SetLinearVelocity(b2Vec2(add.x(), add.z()));
			this->body->SetAngularVelocity(yradadd);
		}


		void		Dispose(void) noexcept {
			if (this->playerfix != nullptr) {
				delete this->playerfix->GetUserData();
				this->playerfix->SetUserData(nullptr);
			}
		}

		const auto	Get(void) noexcept { return body.get(); }
		const auto	Pos(void) noexcept { return body->GetPosition(); }
		const auto	Rad(void) noexcept { return body->GetAngle(); }
		const auto	Speed(void) noexcept { return std::hypot(this->body->GetLinearVelocity().x, this->body->GetLinearVelocity().y); }
		void		SetTransform(const b2Vec2& position, float32 angle) {
			body->SetTransform(position, angle);
		}
	};


	//入力
	class InputControl {
	private:
		float			m_AddxRad{ 0.f };
		float			m_AddyRad{ 0.f };
		float			m_xRad{ 0.f };
		float			m_yRad{ 0.f };
		unsigned int	m_Flags{ 0 };
	public:
		void			SetInput(
			float pAddxRad, float pAddyRad,
			bool pGoFrontPress,
			bool pGoBackPress,
			bool pGoLeftPress,
			bool pGoRightPress,

			bool pRunPress,
			bool pQPress,
			bool pEPress,

			bool pRightPress,
			bool pLeftPress,
			bool pUpPress,
			bool pDownPress,

			bool pAction1,
			bool pAction2,
			bool pAction3,
			bool pAction4,
			bool pAction5,
			bool pAction6
		) {
			this->m_AddxRad = pAddxRad;
			this->m_AddyRad = pAddyRad;
			this->m_Flags = 0;
			if (pGoFrontPress) { this->m_Flags |= (1 << 0); }
			if (pGoBackPress) { this->m_Flags |= (1 << 1); }
			if (pGoLeftPress) { this->m_Flags |= (1 << 2); }
			if (pGoRightPress) { this->m_Flags |= (1 << 3); }
			if (pRunPress) { this->m_Flags |= (1 << 4); }
			if (pQPress) { this->m_Flags |= (1 << 5); }
			if (pEPress) { this->m_Flags |= (1 << 6); }
			if (pRightPress) { this->m_Flags |= (1 << 7); }
			if (pLeftPress) { this->m_Flags |= (1 << 8); }
			if (pUpPress) { this->m_Flags |= (1 << 9); }
			if (pDownPress) { this->m_Flags |= (1 << 10); }
			if (pAction1) { this->m_Flags |= (1 << 11); }
			if (pAction2) { this->m_Flags |= (1 << 12); }
			if (pAction3) { this->m_Flags |= (1 << 13); }
			if (pAction4) { this->m_Flags |= (1 << 14); }
			if (pAction5) { this->m_Flags |= (1 << 15); }
			if (pAction6) { this->m_Flags |= (1 << 16); }
		}
		void			SetRadBuf(float pxRad, float pyRad) {
			this->m_xRad = pxRad;
			this->m_yRad = pyRad;
		}
		void			SetRadBuf(const VECTOR_ref& pRad) {
			this->m_xRad = pRad.x();
			this->m_yRad = pRad.y();
		}
		void			SetKeyInput(unsigned int pFlags) { this->m_Flags = pFlags; }
		const auto&		GetKeyInput(void) const noexcept { return this->m_Flags; }


		const auto&		GetAddxRad(void) const noexcept { return this->m_AddxRad; }
		const auto&		GetAddyRad(void) const noexcept { return this->m_AddyRad; }
		const auto&		GetxRad(void) const noexcept { return this->m_xRad; }
		const auto&		GetyRad(void) const noexcept { return this->m_yRad; }
		const auto	GetGoFrontPress(void) const noexcept { return (this->m_Flags & (1 << 0)) != 0; }
		const auto	GetGoBackPress(void) const noexcept { return (this->m_Flags & (1 << 1)) != 0; }
		const auto	GetGoLeftPress(void) const noexcept { return (this->m_Flags & (1 << 2)) != 0; }
		const auto	GetGoRightPress(void) const noexcept { return (this->m_Flags & (1 << 3)) != 0; }
		const auto	GetRunPress(void) const noexcept { return (this->m_Flags & (1 << 4)) != 0; }
		const auto	GetQPress(void) const noexcept { return (this->m_Flags & (1 << 5)) != 0; }
		const auto	GetEPress(void) const noexcept { return (this->m_Flags & (1 << 6)) != 0; }
		const auto	GetRightPress(void) const noexcept { return (this->m_Flags & (1 << 7)) != 0; }
		const auto	GetLeftPress(void) const noexcept { return (this->m_Flags & (1 << 8)) != 0; }
		const auto	GetUpPress(void) const noexcept { return (this->m_Flags & (1 << 9)) != 0; }
		const auto	GetDownPress(void) const noexcept { return (this->m_Flags & (1 << 10)) != 0; }
		const auto	GetAction1(void) const noexcept { return (this->m_Flags & (1 << 11)) != 0; }
		const auto	GetAction2(void) const noexcept { return (this->m_Flags & (1 << 12)) != 0; }
		const auto	GetAction3(void) const noexcept { return (this->m_Flags & (1 << 13)) != 0; }
		const auto	GetAction4(void) const noexcept { return (this->m_Flags & (1 << 14)) != 0; }
		const auto	GetAction5(void) const noexcept { return (this->m_Flags & (1 << 15)) != 0; }
		const auto	GetAction6(void) const noexcept { return (this->m_Flags & (1 << 16)) != 0; }

		const InputControl operator+(const InputControl& o) const noexcept {
			InputControl tmp;

			tmp.m_AddxRad = this->m_AddxRad + o.m_AddxRad;
			tmp.m_AddyRad = this->m_AddyRad + o.m_AddyRad;
			tmp.m_xRad = this->m_xRad + o.m_xRad;
			tmp.m_yRad = this->m_yRad + o.m_yRad;
			tmp.m_Flags = this->m_Flags;

			return tmp;
		}
		const InputControl operator-(const InputControl& o) const noexcept {
			InputControl tmp;

			tmp.m_AddxRad = this->m_AddxRad - o.m_AddxRad;
			tmp.m_AddyRad = this->m_AddyRad - o.m_AddyRad;
			tmp.m_xRad = this->m_xRad - o.m_xRad;
			tmp.m_yRad = this->m_yRad - o.m_yRad;
			tmp.m_Flags = this->m_Flags;

			return tmp;
		}
		const InputControl operator*(float per) const noexcept {
			InputControl tmp;

			tmp.m_AddxRad = this->m_AddxRad *per;
			tmp.m_AddyRad = this->m_AddyRad *per;
			tmp.m_xRad = this->m_xRad *per;
			tmp.m_yRad = this->m_yRad *per;
			tmp.m_Flags = this->m_Flags;

			return tmp;
		}

	};
	//キャラ入力
	class CharacterMoveGroundControl {
	private:
		std::array<float, 4>								m_Vec{ 0,0,0,0 };
		bool												m_Press_GoFront{ false };
		bool												m_Press_GoRear{ false };
		bool												m_Press_GoLeft{ false };
		bool												m_Press_GoRight{ false };
		switchs												m_Squat;
		switchs												m_QKey;
		switchs												m_EKey;
		bool												m_IsRun{ false };
		float												m_RunPer{ 0.f };
		float												m_RunTimer{ 0.f };
		float												m_SquatPer{ 0.f };
		VECTOR_ref											m_rad_Buf, m_rad, m_radAdd;
		int													m_TurnRate{ 0 };
		float												m_TurnRatePer{ 0.f };
		int													m_LeanRate{ 0 };
		float												m_LeanRatePer{ 0.f };
	private: //内部
		void			SetVec(int pDir, bool Press) {
			this->m_Vec[pDir] += (Press ? 1.f : -1.f)*2.f / FPS;
			this->m_Vec[pDir] = std::clamp(this->m_Vec[pDir], 0.f, 1.f);
		}
	public:
		const auto		GetRadBuf(void) const noexcept { return  this->m_rad_Buf; }
		const auto		GetTurnRatePer(void) const noexcept { return  this->m_TurnRatePer; }
		const auto		GetLeanRatePer(void) const noexcept { return this->m_LeanRatePer; }
		const auto		GetRad(void) const noexcept { return  this->m_rad; }

		const auto		GetIsSquat(void) const noexcept { return this->m_Squat.on(); }
		const auto		GetSquatPer(void) const noexcept { return this->m_SquatPer; }

		const auto		GetVecFront(void) const noexcept { return  this->m_Vec[0]; }
		const auto		GetVecRear(void) const noexcept { return this->m_Vec[2]; }
		const auto		GetVecLeft(void) const noexcept { return this->m_Vec[1]; }
		const auto		GetVecRight(void) const noexcept { return this->m_Vec[3]; }
		const auto		GetPressFront(void) const noexcept { return this->m_Press_GoFront; }
		const auto		GetPressRear(void) const noexcept { return this->m_Press_GoRear; }
		const auto		GetPressLeft(void) const noexcept { return this->m_Press_GoLeft; }
		const auto		GetPressRight(void) const noexcept { return this->m_Press_GoRight; }
		const auto		GetRun(void) const noexcept { return this->m_IsRun; }
		const auto		GetRunPer(void) const noexcept { return  this->m_RunPer; }
		const auto		GetVec(void) const noexcept { return VECTOR_ref::vget(GetVecLeft() - GetVecRight(), 0, GetVecRear() - GetVecFront()); }
		const auto		GetFrontP(void) const noexcept {
			auto FrontP = ((GetPressFront() && !GetPressRear())) ? (std::atan2f(GetVec().x(), -GetVec().z()) * GetVecFront()) : 0.f;
			FrontP += (!GetPressFront() && GetPressRear()) ? (std::atan2f(-GetVec().x(), GetVec().z()) * GetVecRear()) : 0.f;
			return FrontP;
		}

		void			SetRadBufX(float x) noexcept {
			auto xbuf = this->m_rad_Buf.x();
			Easing(&xbuf, x, 0.9f, EasingType::OutExpo);
			this->m_rad_Buf.x(xbuf);
		}
		void			SetRadBufY(float y) noexcept {
			this->m_rad_Buf.y(y);
			this->m_rad.y(y);
		}
		void			SetRadBufZ(float z) noexcept {
			auto zbuf = this->m_rad_Buf.z();
			Easing(&zbuf, z, 0.9f, EasingType::OutExpo);
			this->m_rad_Buf.z(zbuf);
		}
	public:
		void			ValueSet(float pxRad, float pyRad, bool SquatOn) {
			for (int i = 0; i < 4; i++) {
				this->m_Vec[i] = 0.f;
			}
			this->m_Press_GoFront = false;
			this->m_Press_GoRear = false;
			this->m_Press_GoLeft = false;
			this->m_Press_GoRight = false;
			this->m_radAdd.clear();
			this->m_IsRun = false;
			this->m_RunPer = 0.f;
			this->m_RunTimer = 0.f;
			//動作にかかわる操作
			this->m_rad_Buf.x(pxRad);
			this->m_rad_Buf.y(pyRad);
			this->m_Squat.Set(SquatOn);
			//上記を反映するもの
			this->m_rad = this->m_rad_Buf;
			this->m_SquatPer = SquatOn ? 1.f : 0.f;
		}
		void			SetInput(
			float pAddxRad, float pAddyRad,
			const VECTOR_ref& pAddRadvec,
			bool pGoFrontPress,
			bool pGoBackPress,
			bool pGoLeftPress,
			bool pGoRightPress,
			bool pSquatPress,
			bool pRunPress,
			bool pIsNotActive,
			bool pQPress,
			bool pEPress,
			bool pCannotSprint
		) {
			this->m_Press_GoFront = pGoFrontPress && !pIsNotActive;
			this->m_Press_GoRear = pGoBackPress && !pIsNotActive;
			this->m_Press_GoLeft = pGoLeftPress && !pIsNotActive;
			this->m_Press_GoRight = pGoRightPress && !pIsNotActive;

			if (!this->m_IsRun && (pRunPress && !pIsNotActive)) {
				this->m_RunTimer = 1.f;
			}
			if (this->m_RunTimer > 0.f) {
				this->m_RunTimer -= 1.f / FPS;
				this->m_Press_GoFront = true;
				this->m_IsRun = true;
			}
			else {
				this->m_RunTimer = 0.f;
				this->m_IsRun = (pRunPress && !pIsNotActive);
			}
			if (GetPressRear() || (!GetPressFront() && (GetPressLeft() || GetPressRight()))) {
				this->m_IsRun = false;
			}
			if (pCannotSprint) {
				this->m_IsRun = false;
			}

			this->m_Squat.Execute(pSquatPress && !pIsNotActive);
			if (this->m_IsRun) { this->m_Squat.Set(false); }

			{
				this->m_QKey.Execute(pQPress && !pIsNotActive);
				this->m_EKey.Execute(pEPress && !pIsNotActive);
				if (this->m_EKey.trigger()) {
					if (this->m_LeanRate == 1) {
						this->m_LeanRate = -1;
					}
					else {
						if (this->m_LeanRate > -1) {
							this->m_LeanRate--;
						}
						else {
							this->m_LeanRate++;
						}
					}

					if (this->m_TurnRate > -1) {
						this->m_TurnRate--;
					}
					else {
						this->m_TurnRate++;
					}
				}
				if (this->m_QKey.trigger()) {
					if (this->m_LeanRate == -1) {
						this->m_LeanRate = 1;
					}
					else {
						if (this->m_LeanRate < 1) {
							this->m_LeanRate++;
						}
						else {
							this->m_LeanRate--;
						}
					}

					if (this->m_TurnRate < 1) {
						this->m_TurnRate++;
					}
					else {
						this->m_TurnRate--;
					}
				}
				if (!GetRun()) {
					this->m_TurnRate = 0;
				}
				else {
					this->m_LeanRate = 0;
				}

				this->m_TurnRate = std::clamp(this->m_TurnRate, -1, 1);
				this->m_LeanRate = std::clamp(this->m_LeanRate, -1, 1);
				float xadd = 0.f;
				if (GetRun()) {
					xadd = 0.2f*(-this->m_TurnRate);//走り
				}
				Easing(&this->m_TurnRatePer, xadd, 0.9f, EasingType::OutExpo);

				Easing(&this->m_LeanRatePer, (float)(-this->m_LeanRate), 0.9f, EasingType::OutExpo);
			}
			//回転
			{
				auto limchange = Lerp(1.f, powf(1.f - this->GetVecFront(), 0.5f), this->m_RunPer * 0.8f);
				auto tmp = 1.f;
				Easing(&this->m_radAdd, pAddRadvec, 0.95f, EasingType::OutExpo);

				this->m_rad_Buf.x(
					std::clamp(this->m_rad_Buf.x() + pAddxRad * tmp, -deg2rad(80.f) * limchange, deg2rad(80.f) * limchange)
					+ this->m_radAdd.x()
				);
				this->m_rad_Buf.y(
					this->m_rad_Buf.y() + (pAddyRad + this->m_TurnRatePer / 100.f)*tmp
					+ this->m_radAdd.y()
				);

				Easing(&this->m_rad, this->m_rad_Buf, 0.5f, EasingType::OutExpo);
			}
		}
		void			Execute(void) noexcept {
			//移動ベクトル取得
			{
				SetVec(0, GetPressFront());
				SetVec(1, GetPressLeft());
				SetVec(2, GetPressRear());
				SetVec(3, GetPressRight());
			}
			//
			Easing(&this->m_RunPer, this->m_IsRun ? 1.f : 0.f, 0.975f, EasingType::OutExpo);
			Easing(&this->m_SquatPer, GetIsSquat() ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
		}
	};

	// プレイヤー関係の定義
#define PLAYER_ENUM_MIN_SIZE		(0.1f * Scale_Rate)		// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_ENUM_DEFAULT_SIZE	(1.6f * Scale_Rate)		// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_HIT_WIDTH			(0.4f * Scale_Rate)		// 当たり判定カプセルの半径
#define PLAYER_HIT_HEIGHT			(1.6f * Scale_Rate)		// 当たり判定カプセルの高さ
#define PLAYER_HIT_TRYNUM			(16)					// 壁押し出し処理の最大試行回数
#define PLAYER_HIT_SLIDE_LENGTH		(0.015f * Scale_Rate)	// 一度の壁押し出し処理でスライドさせる距離
	//壁判定ユニバーサル
	static bool col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos, const std::vector<MV1*>& col_obj_t) noexcept {
		auto MoveVector = *NowPos - OldPos;
		//MoveVector.y(0);
		// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
		std::vector<MV1_COLL_RESULT_POLY> kabe_;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
		for (const auto& objs : col_obj_t) {
			auto HitDim = objs->CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size());
			// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
			for (int i = 0; i < HitDim.HitNum; ++i) {
				auto& h_d = HitDim.Dim[i];
				//壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋PLAYER_ENUM_MIN_SIZEより高いポリゴンのみ当たり判定を行う
				if (
					(abs(std::atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
					&& (h_d.Position[0].y > OldPos.y() + PLAYER_ENUM_MIN_SIZE || h_d.Position[1].y > OldPos.y() + PLAYER_ENUM_MIN_SIZE || h_d.Position[2].y > OldPos.y() + PLAYER_ENUM_MIN_SIZE)
					&& (h_d.Position[0].y < OldPos.y() + PLAYER_ENUM_DEFAULT_SIZE || h_d.Position[1].y < OldPos.y() + PLAYER_ENUM_DEFAULT_SIZE || h_d.Position[2].y < OldPos.y() + PLAYER_ENUM_DEFAULT_SIZE)
					) {
					kabe_.emplace_back(h_d);// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
				}
			}
			MV1CollResultPolyDimTerminate(HitDim);	// 検出したプレイヤーの周囲のポリゴン情報を開放する
		}
		bool HitFlag = false;
		// 壁ポリゴンとの当たり判定処理
		if (kabe_.size() > 0) {
			HitFlag = false;
			for (auto& h_d : kabe_) {
				if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + VECTOR_ref::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {				// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
					HitFlag = true;// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
					if (MoveVector.size() >= 0.001f) {	// x軸かz軸方向に 0.001f 以上移動した場合は移動したと判定
						// 壁に当たったら壁に遮られない移動成分分だけ移動する
						*NowPos = VECTOR_ref(h_d.Normal).cross(MoveVector.cross(h_d.Normal)) + OldPos;
						//NowPos->y(OldPos.y());
						bool j = false;
						for (auto& h_d2 : kabe_) {
							if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + VECTOR_ref::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {
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
				for (int k = 0; k < PLAYER_HIT_TRYNUM; ++k) {			// 壁からの押し出し処理を試みる最大数だけ繰り返し
					bool HitF = false;
					for (auto& h_d : kabe_) {
						if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + VECTOR_ref::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d.Position[0], h_d.Position[1], h_d.Position[2])) {// プレイヤーと当たっているかを判定
							*NowPos += VECTOR_ref(h_d.Normal) * PLAYER_HIT_SLIDE_LENGTH;					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
							//NowPos->y(OldPos.y());
							bool j = false;
							for (auto& h_d2 : kabe_) {
								if (GetHitCapsuleToTriangle(*NowPos + VECTOR_ref::up()*(PLAYER_HIT_WIDTH + 0.1f), *NowPos + VECTOR_ref::up()*(PLAYER_HIT_HEIGHT), PLAYER_HIT_WIDTH, h_d2.Position[0], h_d2.Position[1], h_d2.Position[2])) {// 当たっていたらループを抜ける
									j = true;
									break;
								}
							}
							if (!j) {// 全てのポリゴンと当たっていなかったらここでループ終了
								break;
							}
							HitF = true;
						}
					}
					if (!HitF) {//全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
						break;
					}
				}
			}
			kabe_.clear();
		}
		return HitFlag;
	}

	//ダメージイベント
	struct DamageEvent {
		PlayerID				ID{ 127 };
		Sceneclass::ObjType		CharaType{ Sceneclass::ObjType::Vehicle };
		HitPoint				Damage{ 0 };
		float					rad{ 0.f };
		void SetEvent(PlayerID pID, Sceneclass::ObjType pCharaType, HitPoint pDamage, float pRad) {
			this->ID = pID;
			this->CharaType = pCharaType;
			this->Damage = pDamage;
			this->rad = pRad;
		}
	};


	//インスタシング
	class Model_Instance {
	private:
		int						m_Count{ 0 };			//数
		std::vector<VERTEX3D>	m_Vertex;				//
		std::vector<DWORD>		m_Index;				//
		int						m_VerBuf{ -1 };			//
		int						m_IndexBuf{ -1 };		//
		MV1						m_obj;					//モデル
		GraphHandle				m_pic;					//画像ハンドル
		int						m_vnum{ -1 };			//
		int						m_pnum{ -1 };			//
		MV1_REF_POLYGONLIST		m_RefMesh{};			//
		int						m_Mesh{ 0 };			//
	private:
		void			Init_one(void) noexcept {
			MV1RefreshReferenceMesh(this->m_obj.get(), -1, TRUE, FALSE, m_Mesh);				//参照用メッシュの更新
			this->m_RefMesh = MV1GetReferenceMesh(this->m_obj.get(), -1, TRUE, FALSE, m_Mesh);	//参照用メッシュの取得
		}
	public:
		//リセット
		void			Reset(void) noexcept {
			this->m_Count = 0;
			this->m_vnum = 0;
			this->m_pnum = 0;
			this->m_Vertex.clear();								//頂点データとインデックスデータを格納するメモリ領域の確保
			this->m_Vertex.reserve(2000);						//頂点データとインデックスデータを格納するメモリ領域の確保
			this->m_Index.clear();								//頂点データとインデックスデータを格納するメモリ領域の確保
			this->m_Index.reserve(2000);						//頂点データとインデックスデータを格納するメモリ領域の確保
		}
		void			Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			this->m_Count++;
			Set_start(this->m_Count);
			float asize = 200.f * caliber;
			const auto& y_vec = Normal;
			auto z_vec = y_vec.cross(Zvec).Norm();
			auto scale = VECTOR_ref::vget(asize / std::abs(y_vec.dot(Zvec)), asize, asize);
			MATRIX_ref mat = MATRIX_ref::GetScale(scale) * MATRIX_ref::Axis1_YZ(y_vec, z_vec) * MATRIX_ref::Mtrans(Position + y_vec * 0.02f);
			Set_one(mat);
		}
		void			Set_start(int value) noexcept {
			this->m_Count = value;
			int Num = this->m_RefMesh.VertexNum * this->m_Count;
			this->m_Vertex.resize(Num);			//頂点データとインデックスデータを格納するメモリ領域の確保
			Num = this->m_RefMesh.PolygonNum * 3 * this->m_Count;
			this->m_Index.resize(Num);		//頂点データとインデックスデータを格納するメモリ領域の確保
		}
		void			Set_one(const MATRIX_ref& mat) noexcept {
			this->m_obj.SetMatrix(mat);
			Init_one();
			for (size_t j = 0; j < size_t(this->m_RefMesh.VertexNum); ++j) {
				auto& g = this->m_Vertex[j + this->m_vnum];
				const auto& r = this->m_RefMesh.Vertexs[j];
				g.pos = r.Position;
				g.norm = r.Normal;
				g.dif = r.DiffuseColor;
				g.spc = r.SpecularColor;
				g.u = r.TexCoord[0].u;
				g.v = r.TexCoord[0].v;
				g.su = r.TexCoord[1].u;
				g.sv = r.TexCoord[1].v;
			}
			for (size_t j = 0; j < size_t(this->m_RefMesh.PolygonNum); ++j) {
				for (size_t k = 0; k < std::size(this->m_RefMesh.Polygons[j].VIndex); ++k) {
					this->m_Index[j * 3 + k + this->m_pnum] = WORD(this->m_RefMesh.Polygons[j].VIndex[k] + this->m_vnum);
				}
			}
			this->m_vnum += this->m_RefMesh.VertexNum;
			this->m_pnum += this->m_RefMesh.PolygonNum * 3;
		}
	public:
		void			Init(MV1& mv1path, int MeshNum) noexcept {
			SetUseASyncLoadFlag(FALSE);
			this->m_Mesh = MeshNum;
			auto path = MV1GetTextureGraphHandle(mv1path.get(), MV1GetMaterialDifMapTexture(mv1path.get(), MV1GetMeshMaterial(mv1path.get(), m_Mesh)));
			this->m_pic = path;								 //grass
			this->m_obj = mv1path.Duplicate();				//弾痕
			Init_one();
		}
		void			Init(std::string pngpath, std::string mv1path, int MeshNum) noexcept {
			SetUseASyncLoadFlag(FALSE);
			this->m_Mesh = MeshNum;
			this->m_pic = GraphHandle::Load(pngpath);		 //grass
			MV1::Load(mv1path, &this->m_obj);				//弾痕
			Init_one();
		}
		void			Execute(void) noexcept {
			this->m_VerBuf = CreateVertexBuffer((int)this->m_Vertex.size(), DX_VERTEX_TYPE_NORMAL_3D);
			this->m_IndexBuf = CreateIndexBuffer((int)this->m_Index.size(), DX_INDEX_TYPE_32BIT);
			SetVertexBufferData(0, this->m_Vertex.data(), (int)this->m_Vertex.size(), this->m_VerBuf);
			SetIndexBufferData(0, this->m_Index.data(), (int)this->m_Index.size(), this->m_IndexBuf);
		}
		void			Draw(void) noexcept {
			//SetDrawAlphaTest(DX_CMP_GREATER, 128);
			DrawPolygonIndexed3D_UseVertexBuffer(this->m_VerBuf, this->m_IndexBuf, this->m_pic.get(), TRUE);
			//SetDrawAlphaTest(-1, 0);
		}
		void			Dispose(void) noexcept {
			this->m_Vertex.clear();
			this->m_Index.clear();
			this->m_obj.Dispose();
			this->m_pic.Dispose();
		}
	};
	//命中根
	class HIT_PASSIVE {
	private:
		Model_Instance	m_inst;
		bool			m_IsUpdate{ true };
	public:
		//初期化
		void			Init(void) noexcept {
			this->m_inst.Init("data/m_obj/hit/hit.png", "data/m_obj/hit/m_obj.mv1", -1);
		}
		//毎回のリセット
		void			Clear(void) noexcept {
			this->m_inst.Reset();
		}

		void			Set(const float& caliber, const VECTOR_ref& Position, const VECTOR_ref& Normal, const VECTOR_ref& Zvec) {
			this->m_inst.Set(caliber, Position, Normal, Zvec);
			this->m_IsUpdate = true;
		}
		void			Execute(void) noexcept {
			if (this->m_IsUpdate) {
				this->m_IsUpdate = false;
				this->m_inst.Execute();
			}
		}
		void			Draw(void) noexcept {
			this->m_inst.Draw();
		}
	};
};
