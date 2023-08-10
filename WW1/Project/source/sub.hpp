#pragma once
#include	"Header.hpp"

//Box2D拡張
namespace std {
	template <>
	struct default_delete<b2Body> {
		void		operator()(b2Body* body) const {
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

	// プレイヤー関係の定義
#define PLAYER_ENUM_MIN_SIZE		(0.1f * Scale_Rate)		// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_ENUM_DEFAULT_SIZE	(1.6f * Scale_Rate)		// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_HIT_WIDTH			(0.45f * Scale_Rate)		// 当たり判定カプセルの半径
#define PLAYER_HIT_HEIGHT			(1.6f * Scale_Rate)		// 当たり判定カプセルの高さ
#define PLAYER_HIT_TRYNUM			(16)					// 壁押し出し処理の最大試行回数
#define PLAYER_HIT_SLIDE_LENGTH		(0.015f * Scale_Rate)	// 一度の壁押し出し処理でスライドさせる距離
	//壁判定ユニバーサル
	static bool col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos, const std::vector<std::pair<MV1*, int>>& col_obj_t) noexcept {
		auto MoveVector = *NowPos - OldPos;
		//MoveVector.y(0);
		// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
		std::vector<MV1_COLL_RESULT_POLY> kabe_;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
		for (const auto& objs : col_obj_t) {
			if ((&objs - &col_obj_t.front()) != 0) {
				if (GetMinLenSegmentToPoint(OldPos, OldPos + VECTOR_ref::up(), objs.first->GetMatrix().pos()) >=
					(20.f*Scale_Rate + PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size())) {
					continue;
				}
			}
			auto HitDim = objs.first->CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), objs.second);
			// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
			for (int i = 0; i < HitDim.HitNum; ++i) {
				auto& h_d = HitDim.Dim[i];
				//壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋PLAYER_ENUM_MIN_SIZEより高いポリゴンのみ当たり判定を行う
				if (
					(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z))) <= deg2rad(30))
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
		Sceneclass::ObjType		CharaType{ Sceneclass::ObjType::Plane };
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

	static VECTOR_ref GetScreenPos(const VECTOR_ref&campos, const VECTOR_ref&camvec, const VECTOR_ref&camup, float fov, const VECTOR_ref&worldpos) noexcept {
		int ScrX = y_r(1920);
		int ScrY = y_r(1080);
		// ビュー行列と射影行列の取得
		MATRIX mat_view;					// ビュー行列
		VECTOR vec_from = campos.get();		// カメラの位置
		VECTOR vec_lookat = camvec.get();   // カメラの注視点
		VECTOR vec_up = camup.get();        // カメラの上方向
		CreateLookAtMatrix(&mat_view, &vec_from, &vec_lookat, &vec_up);
		SetCameraNearFar(0.f, 100.f*Scale_Rate);
		SetupCamera_Perspective(fov);
		MATRIX proj = GetCameraProjectionMatrix();
		// ビューポート行列（スクリーン行列）の作成
		float w = (float)ScrX / 2.0f;
		float h = (float)ScrY / 2.0f;
		MATRIX viewport = {
			w , 0 , 0 , 0 ,
			0 ,-h , 0 , 0 ,
			0 , 0 , 1 , 0 ,
			w , h , 0 , 1
		};
		VECTOR screenPos, tmp = worldpos.get();
		// ビュー変換とプロジェクション変換
		tmp = VTransform(tmp, mat_view);
		tmp = VTransform(tmp, proj);
		// zで割って-1~1の範囲に収める
		tmp.x /= tmp.z; tmp.y /= tmp.z; tmp.z /= tmp.z;
		// スクリーン変換
		screenPos = VTransform(tmp, viewport);
		screenPos.z = -1.f;
		if ((camvec - campos).dot(worldpos - campos) > 0.f) {
			screenPos.z = 0.5f;
		}
		return screenPos;
	}

	class LoadScriptClass {
	private:
		struct VARIABLE {
			std::string Base;
			std::string After;
		};
	private:
		std::vector<VARIABLE> m_Variable;
		std::string m_Func;
		std::vector<std::string> m_Args;
	private:
		static void Sub_Func(std::string& func_t, const char& in_str) noexcept {
			size_t str_switch = 0;
			size_t str_in = std::string::npos;
			bool in = false;
			while (true) {
				if (str_switch != std::string::npos) { str_switch = func_t.find('\"', str_switch + 1); in ^= 1; }
				str_in = func_t.find(in_str, str_in + 1);
				if (str_in != std::string::npos) {
					if (str_switch != std::string::npos && str_switch < str_in && in) {
						continue;
					}
					func_t = func_t.erase(str_in, 1);
					continue;
				}
				break;
			}
		}
	public:
		//Getter
		const auto& Getfunc(void) const noexcept { return m_Func; }
		const auto& Getargs(void) const noexcept { return m_Args; }
		//スクリプト読み込み処理
		void LoadScript(std::string_view func_t) noexcept {
			m_Args.clear();
			m_Func = func_t;
			{
				// //を削除
				size_t sls = m_Func.find("//");
				if (sls != std::string::npos) { m_Func = m_Func.substr(0, sls); }
				//いらない要素を排除
				Sub_Func(m_Func, '{');
				Sub_Func(m_Func, '}');
				Sub_Func(m_Func, ' ');
				Sub_Func(m_Func, '\t');
				Sub_Func(m_Func, ';');
				Sub_Func(m_Func, '\"');
			}
			//()と,で囲われた部分から引数を取得
			if (m_Func != "") {
				std::string tmp_func = m_Func;
				size_t left = tmp_func.find("(");
				size_t right = tmp_func.rfind(")");
				if (left != std::string::npos && right != std::string::npos) {
					tmp_func = tmp_func.substr(left + 1, right - 1 - left);
				}
				while (true) {
					size_t in_str = tmp_func.find(",");
					if (in_str == std::string::npos) {
						m_Args.emplace_back(tmp_func);
						break;
					}
					else {
						std::string arg = tmp_func.substr(0, in_str);
						tmp_func = tmp_func.substr(in_str + 1);
						m_Args.emplace_back(arg);
					}
				}
			}
		}
	};

	class TelopClass {
	private:
		class Cut_tex {
			int xpos = 0;
			int ypos = 0;
			int size = 0;
			int LMR = 1;
			std::string str;
			LONGLONG START_TIME = 0;
			LONGLONG END_TIME = 0;
		public:
			Cut_tex(void) noexcept {
				xpos = 0;
				ypos = 0;
				size = 12;
				str = "test";
				START_TIME = (LONGLONG)(1000000.f * 0.01f);
				END_TIME = (LONGLONG)(1000000.f * 1.01f);
			}
			void Set(int xp, int yp, int Fontsize, std::string_view mag, LONGLONG StartF, LONGLONG ContiF, int m_LMR) noexcept {
				this->xpos = xp;
				this->ypos = yp;
				this->size = Fontsize;
				this->str = mag;
				this->START_TIME = StartF;
				this->END_TIME = StartF + ContiF;;
				this->LMR = m_LMR;
			}
			void Draw(LONGLONG nowTimeWait) const noexcept {
				if (this->START_TIME < nowTimeWait && nowTimeWait < this->END_TIME) {
					auto* Fonts = FontPool::Instance();
					switch (this->LMR) {
					case 0:
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(this->size, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
						break;
					case 1:
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(this->size, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
						break;
					case 2:
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(this->size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
						break;
					default:
						break;
					}
				}
			}
		};
	private:
		std::vector<Cut_tex> Texts;
		LONGLONG StartF = 0;
		LONGLONG ContiF = 0;
	public:
		void Init(void) noexcept {
			StartF = 0;
			ContiF = 0;
		}
		void LoadTelop(const std::string &func, const std::vector<std::string>& args) noexcept {
			if (func.find("SetTelopTime") != std::string::npos) {
				StartF = (LONGLONG)(1000000.f * std::stof(args[0]));
				ContiF = (LONGLONG)(1000000.f * std::stof(args[1]));
			}
			else if (func.find("AddTelopTime") != std::string::npos) {
				StartF += (LONGLONG)(1000000.f * std::stof(args[0]));
				ContiF = (LONGLONG)(1000000.f * std::stof(args[1]));
			}
			else if (func.find("SetTelop") != std::string::npos) {
				int t = 0;
				if (args[4].find("LEFT") != std::string::npos) { t = 0; }
				else if (args[4].find("MIDDLE") != std::string::npos) { t = 1; }
				else if (args[4].find("RIGHT") != std::string::npos) { t = 2; }
				Texts.resize(Texts.size() + 1);
				Texts.back().Set(y_r(std::stoi(args[0])), y_r(std::stoi(args[1])), y_r(std::stoi(args[2])), args[3], StartF, ContiF, t);
			}
		}
		void Draw(LONGLONG nowTimeWait) const noexcept {
			for (auto& t : Texts) {
				t.Draw(nowTimeWait);
			}
		}
	};

	class KeyGuideClass : public SingletonBase<KeyGuideClass> {
	private:
		friend class SingletonBase<KeyGuideClass>;
	private:
		class Keys {
		public:
			int xsize{ 0 }, ysize{ 0 };
			GraphHandle GuideImg;
			std::string GuideString;
		public:
			void AddGuide(std::string_view ImgName, std::string_view GuideStr) noexcept {
				if (ImgName != "") {
					std::string Path = "data/key/key_glay/";
					Path += ImgName;
					GuideImg = GraphHandle::Load(Path);
					GuideImg.GetSize(&xsize, &ysize);
					xsize = xsize * y_r(21) / ysize;
					ysize = ysize * y_r(21) / ysize;
				}
				else {
					xsize = 0;
					ysize = 0;
				}
				GuideString = GuideStr;
			}
			void Reset() noexcept {
				GuideImg.Dispose();
				GuideString = "";
			}
			int Draw(int x, int y) const noexcept {
				auto* Fonts = FontPool::Instance();

				int ofs = 0;
				if (xsize > 0) {
					GuideImg.DrawExtendGraph(x + ofs, y, x + ofs + xsize, y + ysize, false);
					ofs += xsize + y_r(6);
				}
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(21), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, x + ofs, y, GetColor(255, 255, 255), GetColor(0, 0, 0), GuideString);
				ofs += Fonts->Get(FontPool::FontType::Nomal_Edge).GetStringWidth(y_r(21), GuideString) + y_r(6);
				return ofs;
			}
		};
	private:
		std::vector<Keys> Key;
	public:
		void Reset(void) noexcept {
			for (auto& k : Key) {
				k.Reset();
			}
			Key.clear();
		}
		void AddGuide(std::string_view ImgName, std::string_view GuideStr) noexcept {
			Key.resize(Key.size() + 1);
			Key.back().AddGuide(ImgName, GuideStr);
		}
		void Draw() const noexcept {
			int x = y_r(32);
			int y = y_r(1080 - 21 - 16);
			for (const auto& k : Key) {
				x += k.Draw(x,y);
			}
		}
	};

	typedef std::pair<std::string, int> SaveParam;
	class SaveDataClass : public SingletonBase<SaveDataClass> {
	private:
		friend class SingletonBase<SaveDataClass>;
	private:
		std::vector<SaveParam> m_data;
	public:
		SaveParam* GetData(std::string_view Name) noexcept {
			for (auto& d : m_data) {
				if (d.first == Name) {
					return &d;
				}
			}
			return nullptr;
		}
	public:
		void SetParam(std::string_view Name, int value) noexcept {
			auto* Data = GetData(Name);
			if (Data) {
				Data->second = value;
			}
			else {
				m_data.emplace_back(std::make_pair((std::string)Name, value));
			}
		}
		auto GetParam(std::string_view Name) noexcept {
			auto* Data = GetData(Name);
			if (Data) {
				return Data->second;
			}
			return -1;
		}
	public:
		void Save() noexcept {
			std::ofstream outputfile("data/Plane/bokuzyo.ok");
			for (auto& d : m_data) {
				outputfile << d.first + "=" + std::to_string(d.second) + "\n";
			}
			outputfile.close();
		}
		void Load() noexcept {

			m_data.clear();

			std::ifstream inputputfile("data/Plane/bokuzyo.ok");
			std::string line;
			while (std::getline(inputputfile, line)) {
				auto Start = line.find("=");
				if (Start != std::string::npos) {
					m_data.emplace_back(std::make_pair(line.substr(0, Start), std::stoi(line.substr(Start + 1))));
				}
			}
			inputputfile.close();

		}
	};

	namespace Sceneclass {
		class OptionWindowClass : public SingletonBase<OptionWindowClass> {
		private:
			friend class SingletonBase<OptionWindowClass>;
		private:
			switchs UpKey;
			switchs DownKey;
			switchs LeftKey;
			switchs RightKey;
			switchs OKKey;
			switchs NGKey;

			int select{ 0 };
			float SelYadd[3] = { 0.f,0.f,0.f };

			bool isActive{ false };
		private:
		public:
			void SetActive(bool value) noexcept { isActive = value; }
			const auto& IsActive() const noexcept { return isActive; }
		public:
			void Init(void) noexcept {
				select = 0;
				isActive = false;
			}
			void Execute(void) noexcept {
				auto SE = SoundPool::Instance();
				if (isActive) {
					auto* OptionParts = OPTION::Instance();

					if (GetJoypadNum() > 0) {
						DINPUT_JOYSTATE input;
						int padID = DX_INPUT_PAD1;
						GetJoypadInputState(padID);
						switch (GetJoypadType(padID)) {
						case DX_PADTYPE_OTHER:
						case DX_PADTYPE_DUAL_SHOCK_4:
						case DX_PADTYPE_DUAL_SENSE:
						case DX_PADTYPE_SWITCH_JOY_CON_L:
						case DX_PADTYPE_SWITCH_JOY_CON_R:
						case DX_PADTYPE_SWITCH_PRO_CTRL:
						case DX_PADTYPE_SWITCH_HORI_PAD:
							GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
							{
								//pp_x = std::clamp(-(float)(input.Rz) / 100.f*0.35f, -9.f, 9.f) * cam_per;
								//pp_y = std::clamp((float)(input.Z) / 100.f*0.35f, -9.f, 9.f) * cam_per;
								float deg = rad2deg(atan2f((float)input.X, -(float)input.Y));
								bool w_key = false;
								bool s_key = false;
								bool a_key = false;
								bool d_key = false;
								if (!(input.X == 0 && input.Y == 0)) {
									w_key = (-50.f <= deg && deg <= 50.f);
									a_key = (-140.f <= deg && deg <= -40.f);
									s_key = (130.f <= deg || deg <= -130.f);
									d_key = (40.f <= deg && deg <= 140.f);
								}
								//十字
								//deg = (float)(input.POV[0]) / 100.f;
								//bool right_key = (40.f <= deg && deg <= 140.f);
								//bool left_key = (220.f <= deg && deg <= 320.f);
								//bool up_key = (310.f <= deg || deg <= 50.f);
								//bool down_key = (130.f <= deg && deg <= 230.f);

								//ボタン
								//(input.Buttons[0] != 0)/*□*/
								//(input.Buttons[1] != 0)/*×*/
								//(input.Buttons[2] != 0)/*〇*/
								//(input.Buttons[3] != 0)/*△*/
								//(input.Buttons[4] != 0)/*L1*/
								//(input.Buttons[5] != 0)/*R1*/
								//(input.Buttons[6] != 0)/*L2*/
								//(input.Buttons[7] != 0)/*R2*/
								//(input.Buttons[8] != 0)/**/
								//(input.Buttons[9] != 0)/**/
								//(input.Buttons[10] != 0)/*L3*/
								//(input.Buttons[11] != 0)/*R3*/
								UpKey.Execute(w_key);
								DownKey.Execute(s_key);
								LeftKey.Execute(a_key);
								RightKey.Execute(d_key);
								OKKey.Execute((input.Buttons[1] != 0)/*×*/);
								NGKey.Execute((input.Buttons[2] != 0)/*〇*/);

							}
							break;
						case DX_PADTYPE_XBOX_360:
						case DX_PADTYPE_XBOX_ONE:
							break;
						default:
							break;
						}
					}
					else {//キーボード
						UpKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_W) != 0 || CheckHitKeyWithCheck(KEY_INPUT_UP) != 0);
						DownKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_S) != 0 || CheckHitKeyWithCheck(KEY_INPUT_DOWN) != 0);
						LeftKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_A) != 0 || CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0);
						RightKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_D) != 0 || CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0);
						OKKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0);
						NGKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_X) != 0);
					}

					if (UpKey.trigger()) {
						--select;
						if (select < 0) { select = 2; }
						SelYadd[select] = 10.f;

						SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (DownKey.trigger()) {
						++select;
						if (select > 2) { select = 0; }
						SelYadd[select] = -10.f;

						SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					for (int i = 0; i < 3; i++) {
						Easing(&SelYadd[i], 0.f, 0.95f, EasingType::OutExpo);
					}
					{
						switch (select) {
						case 0:
							if (LeftKey.trigger()) {
								OptionParts->Set_BGM(std::clamp(OptionParts->Get_BGM() - 0.1f, 0.f, 1.f));
								SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
							}
							if (RightKey.trigger()) {
								OptionParts->Set_BGM(std::clamp(OptionParts->Get_BGM() + 0.1f, 0.f, 1.f));
								SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
							}
							break;
						case 1:
							if (LeftKey.trigger()) {
								OptionParts->Set_SE(std::clamp(OptionParts->Get_SE() - 0.1f, 0.f, 1.f));
								SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
							}
							if (RightKey.trigger()) {
								OptionParts->Set_SE(std::clamp(OptionParts->Get_SE() + 0.1f, 0.f, 1.f));
								SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
							}
							break;
						default:
							break;
						}
					}
					if (OKKey.trigger()) {
						switch (select) {
						case 0:
							break;
						case 1:
							break;
						case 2:
							SE->Get((int)SoundEnum::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
							isActive = false;
							break;
						default:
							SE->Get((int)SoundEnum::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
							isActive = false;
							break;
						}
					}
					if (NGKey.trigger()) {
						SE->Get((int)SoundEnum::UI_NG).Play(0, DX_PLAYTYPE_BACK, TRUE);
						isActive = false;
					}

					SE->SetVol(OptionParts->Get_SE());

					if (!isActive) {
						OptionParts->Save();
					}
				}
				else {
					select = 0;
				}
			}
			void Draw() const noexcept {
				if (isActive) {
					auto* OptionParts = OPTION::Instance();
					//
					{
						auto per = std::clamp(0.3f, 0.f, 1.f);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

						DrawBox(y_r(960 - 320), y_r(540 - 320), y_r(960 + 320), y_r(540 + 320), GetColor(255, 255, 255), TRUE);

						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					}

					int xp1, yp1;
					auto* Fonts = FontPool::Instance();
					auto Red = GetColor(255, 0, 0);
					auto Red75 = GetColor(192, 0, 0);
					auto White = GetColor(255, 255, 255);
					auto Gray75 = GetColor(128, 128, 128);
					auto Gray = GetColor(64, 64, 64);

					xp1 = y_r(960 + 44);
					yp1 = y_r(1080 - 400 - 108 * 3);
					Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 3 / 2 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), Gray75, Gray, "Option");
					Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48 * 2 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, White, Gray, "Option");

					xp1 = y_r(960 - 54 * 2);
					yp1 = y_r(1080 - 400 - 108 * 2 + (int)SelYadd[0]);
					Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 3 / 2 * 3 / 4 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), (select == 0) ? Red75 : Gray75, Gray, "BGM");
					Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48 * 2 * 3 / 4 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, (select == 0) ? Red : White, Gray, "BGM");

					xp1 = y_r(960 - 54 * 2 + 208);
					Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 3 / 2 * 3 / 4 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), (select == 1) ? Red75 : Gray75, Gray, "%3d %", (int)(OptionParts->Get_BGM()*100.f + 0.5f));
					Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48 * 2 * 3 / 4 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, (select == 1) ? Red : White, Gray, "%3d %", (int)(OptionParts->Get_BGM()*100.f + 0.5f));

					xp1 = y_r(960 - 54 * 2);
					yp1 = y_r(1080 - 400 - 108 * 1 + (int)SelYadd[1]);
					Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 3 / 2 * 3 / 4 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), (select == 1) ? Red75 : Gray75, Gray, "SE");
					Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48 * 2 * 3 / 4 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, (select == 1) ? Red : White, Gray, "SE");

					xp1 = y_r(960 - 54 * 2 + 208);
					Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 3 / 2 * 3 / 4 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), (select == 1) ? Red75 : Gray75, Gray, "%3d %", (int)(OptionParts->Get_SE()*100.f + 0.5f));
					Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48 * 2 * 3 / 4 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, (select == 1) ? Red : White, Gray, "%3d %", (int)(OptionParts->Get_SE()*100.f + 0.5f));

					xp1 = y_r(960 + 44);
					yp1 = y_r(1080 - 400 - 108 * -1 + (int)SelYadd[2]);
					Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 3 / 2 * 3 / 4 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), (select == 2) ? Red75 : Gray75, Gray, "Return");
					Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48 * 2 * 3 / 4 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, (select == 2) ? Red : White, Gray, "Return");

				}
			}
		};
	};

};
