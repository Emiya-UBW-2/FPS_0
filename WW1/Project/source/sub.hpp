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
		std::vector<Keys>	Key;
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

	template <typename... Args>
	static void DrawFetteString(int xp1, int yp1, float per, bool IsSelect, const std::string& String, Args&&... args) noexcept {
		auto* Fonts = FontPool::Instance();
		auto Red = GetColor(255, 0, 0);
		auto Red75 = GetColor(192, 0, 0);
		auto White = GetColor(255, 255, 255);
		auto Gray75 = GetColor(128, 128, 128);
		auto Gray = GetColor(64, 64, 64);

		Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r((int)((float)(48 * 3 / 2 * 3 / 4) * per)), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40.f*per), yp1 + y_r(20.f*per), IsSelect ? Red75 : Gray75, Gray, String, args...);
		Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r((int)((float)(48 * 2 * 3 / 4) * per)), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, IsSelect ? Red : White, Gray, String, args...);
	}

	namespace Sceneclass {
		class OptionWindowClass : public SingletonBase<OptionWindowClass> {
		private:
			friend class SingletonBase<OptionWindowClass>;
		private:
			int select{ 0 };
			static const int selMax{ 9 };
			std::array<float, selMax> SelYadd{};

			bool isActive{ false };
		private:
		public:
			void SetActive(bool value) noexcept { isActive = value; }
			const auto& IsActive() const noexcept { return isActive; }
		public:
			void Init(void) noexcept {
				select = 0;
				isActive = false;
				for (auto& y : SelYadd) {
					y = 0.f;
				}
			}
			void Execute(void)noexcept;
			void Draw() const noexcept {
				if (isActive) {
					auto* OptionParts = OPTION::Instance();
					//
					{
						auto per = std::clamp(0.3f, 0.f, 1.f);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

						DrawBox(y_r(960 - 320), y_r(540 - 320), y_r(960 + 320), y_r(540 + 320), GetColor(128, 128, 128), TRUE);

						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					}

					int xp1, yp1;
					//auto* Fonts = FontPool::Instance();
					//auto Red = GetColor(255, 0, 0);
					//auto Red75 = GetColor(192, 0, 0);
					//auto White = GetColor(255, 255, 255);
					//auto Gray75 = GetColor(128, 128, 128);
					//auto Gray = GetColor(64, 64, 64);

					int Line = 0;

					int height = 54;
					//
					xp1 = y_r(960 + 64);
					yp1 = y_r(356 + height * Line);
					DrawFetteString(xp1, yp1, 1.f, false, "Option");
					Line++;
					//
					auto SelStart = Line;
					//
					{
						xp1 = y_r(960 - 54 * 2);
						//サウンド
						yp1 = y_r(356 + height * Line + (int)SelYadd.at(Line - SelStart));
						DrawFetteString(xp1, yp1, 0.4f, (select == (Line - SelStart)), "BGM");
						DrawFetteString(xp1 + y_r(300), yp1, 0.4f, (select == (Line - SelStart)), "%3d %", (int)(OptionParts->Get_BGM()*100.f + 0.5f));
						Line++;
						yp1 = y_r(356 + height * Line + (int)SelYadd.at(Line - SelStart));
						DrawFetteString(xp1, yp1, 0.4f, (select == (Line - SelStart)), "SE");
						DrawFetteString(xp1 + y_r(300), yp1, 0.4f, (select == (Line - SelStart)), "%3d %", (int)(OptionParts->Get_SE()*100.f + 0.5f));
						Line++;
						//グラフィック
						yp1 = y_r(356 + height * Line + (int)SelYadd.at(Line - SelStart));
						DrawFetteString(xp1, yp1, 0.4f, (select == (Line - SelStart)), "GRASS");
						DrawFetteString(xp1 + y_r(300), yp1, (OptionParts->Get_grass_level() ? 0.4f : 0.35f), (select == (Line - SelStart)), ((OptionParts->Get_grass_level() > 0) ? "Enable" : "DiSable"));
						Line++;
						yp1 = y_r(356 + height * Line + (int)SelYadd.at(Line - SelStart));
						DrawFetteString(xp1, yp1, 0.4f, (select == (Line - SelStart)), "BloomEffect");
						DrawFetteString(xp1 + y_r(300), yp1, (OptionParts->Get_Bloom() ? 0.4f : 0.35f), (select == (Line - SelStart)), (OptionParts->Get_Bloom() ? "Enable" : "DiSable"));
						Line++;
						yp1 = y_r(356 + height * Line + (int)SelYadd.at(Line - SelStart));
						DrawFetteString(xp1, yp1, 0.4f, (select == (Line - SelStart)), "Shadow");
						DrawFetteString(xp1 + y_r(300), yp1, (OptionParts->Get_Shadow() ? 0.4f : 0.35f), (select == (Line - SelStart)), (OptionParts->Get_Shadow() ? "Enable" : "DiSable"));
						Line++;
						yp1 = y_r(356 + height * Line + (int)SelYadd.at(Line - SelStart));
						DrawFetteString(xp1, yp1, 0.4f, (select == (Line - SelStart)), "V Sync");
						DrawFetteString(xp1 + y_r(300), yp1, (OptionParts->Get_Vsync() ? 0.4f : 0.35f), (select == (Line - SelStart)), (OptionParts->Get_Vsync() ? "Enable" : "DiSable"));
						Line++;
						yp1 = y_r(356 + height * Line + (int)SelYadd.at(Line - SelStart));
						DrawFetteString(xp1, yp1, 0.4f, (select == (Line - SelStart)), "ColorAberration");
						DrawFetteString(xp1 + y_r(300), yp1, (OptionParts->Get_aberration() ? 0.4f : 0.35f), (select == (Line - SelStart)), (OptionParts->Get_aberration() ? "Enable" : "DiSable"));
						Line++;
						yp1 = y_r(356 + height * Line + (int)SelYadd.at(Line - SelStart));
						DrawFetteString(xp1, yp1, 0.4f, (select == (Line - SelStart)), "DirectX VerSion");
						switch (OptionParts->Get_DirectXVer()) {
						case  DX_DIRECT3D_9:
							DrawFetteString(xp1 + y_r(300), yp1, 0.4f, (select == (Line - SelStart)), "9.0");
							break;
						case  DX_DIRECT3D_9EX:
							DrawFetteString(xp1 + y_r(300), yp1, 0.4f, (select == (Line - SelStart)), "9.0c");
							break;
						case  DX_DIRECT3D_11:
							DrawFetteString(xp1 + y_r(300), yp1, 0.4f, (select == (Line - SelStart)), "11.0");
							break;
						default:
							break;
						}
						Line++;
					}
					//
					xp1 = y_r(960 + 44);
					yp1 = y_r(356 + height * Line + (int)SelYadd.back());
					DrawFetteString(xp1, yp1, 0.4f, (select == selMax - 1), "Return");
					//
					{
						xp1 = y_r(960 - 320);
						yp1 = y_r(540 + 320);

						auto* Fonts = FontPool::Instance();
						auto White = GetColor(255, 255, 255);
						auto Gray = GetColor(0, 0, 0);

						std::string Info = "";
						switch (select) {
						case 0:
							Info = "BGMボリュームを変更します";
							break;
						case 1:
							Info = "SEボリュームを変更します";
							break;
						case 2:
							Info = "木の表示をするかを指定します(GPU次第では大きくがFPS変動します)";
							break;
						case 3:
							Info = "ブルームエフェクトの有効無効を指定します";
							break;
						case 4:
							Info = "影の有効無効を指定します(GPU次第では大きくFPSが変動します)";
							break;
						case 5:
							Info = "垂直同期の有効無効を指定します(反映は再起動後にされます)";
							break;
						case 6:
							Info = "画面エフェクトの有効無効を指定します";
							break;
						case 7:
							Info = "DirectXのバージョンを変更します(反映は再起動後にされます)";
							break;
						default:
							Info = "オプションを閉じます";
							break;
						}

						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, White, Gray, Info);
					}
				}
			}
		};
	};

	class PadControl : public SingletonBase<PadControl> {
	private:
		friend class SingletonBase<PadControl>;
	private:
		switchs UpKey;
		switchs DownKey;
		switchs LeftKey;
		switchs RightKey;
		switchs OKKey;
		switchs NGKey;

		switchs m_ShotKey;
		switchs m_MouseWheel;
		switchs m_LookKey;

		switchs m_AccelKey;
		switchs m_BrakeKey;

		switchs m_QKey;
		switchs m_EKey;

		float LS_X{ 0.f };
		float LS_Y{ 0.f };
	private:
		bool m_IsPad{ false };
		bool m_IsUpdate{ true };
	private:
		PadControl() {
			m_IsUpdate = true;
		}
		~PadControl() {}
	public:
		void SetGuideUpdate() noexcept { m_IsUpdate = true; }
	public:
		const auto& GetUpKey() const noexcept { return UpKey; }
		const auto& GetDownKey() const noexcept { return DownKey; }
		const auto& GetLeftKey() const noexcept { return LeftKey; }
		const auto& GetRightKey() const noexcept { return RightKey; }
		const auto& GetOKKey() const noexcept { return OKKey; }
		const auto& GetNGKey() const noexcept { return NGKey; }
		const auto& GetShotKey() const noexcept { return m_ShotKey; }
		const auto& GetMouseWheel() const noexcept { return m_MouseWheel; }
		const auto& GetLookKey() const noexcept { return m_LookKey; }

		const auto& GetAccelKey() const noexcept { return m_AccelKey; }
		const auto& GetBrakeKey() const noexcept { return m_BrakeKey; }

		const auto& GetQKey() const noexcept { return m_QKey; }
		const auto& GetEKey() const noexcept { return m_EKey; }

		const auto& GetLS_X() const noexcept { return LS_X; }
		const auto& GetLS_Y() const noexcept { return LS_Y; }
	public:
		void Execute(std::function<void()>Guide_Pad_PS4, std::function<void()>Guide_Key, switchs* MouseActive = nullptr) noexcept {
			//
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
					//左スティック
					{
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
						this->UpKey.Execute(w_key);
						this->DownKey.Execute(s_key);
						this->LeftKey.Execute(a_key);
						this->RightKey.Execute(d_key);
					}
					//十字
					{
						//float deg = (float)(input.POV[0]) / 100.f;
						//bool right_key = (40.f <= deg && deg <= 140.f);
						//bool left_key = (220.f <= deg && deg <= 320.f);
						//bool up_key = (310.f <= deg || deg <= 50.f);
						//bool down_key = (130.f <= deg && deg <= 230.f);
					}
					//ボタン
					{
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
						this->OKKey.Execute((input.Buttons[1] != 0)/*×*/);
						this->NGKey.Execute((input.Buttons[2] != 0)/*〇*/);
						this->m_ShotKey.Execute((input.Buttons[7] != 0)/*R2*/);
						this->m_MouseWheel.Execute((input.Buttons[11] != 0)/*R3*/);
						this->m_LookKey.Execute(input.Buttons[6] != 0);/*L2*/	//ADS
						this->m_AccelKey.Execute((input.Buttons[0] != 0)/*□*/);
						this->m_BrakeKey.Execute((input.Buttons[3] != 0)/*△*/);
						this->m_QKey.Execute((input.Buttons[4] != 0)/*L1*/);
						this->m_EKey.Execute((input.Buttons[5] != 0)/*R1*/);
					}
					//右スティック
					{
						LS_X = std::clamp((float)(input.Z) / 100.f*0.35f, -9.f, 9.f);
						LS_Y = std::clamp(-(float)(input.Rz) / 100.f*0.35f, -9.f, 9.f);
					}
					//
					if (m_IsPad != true) {
						m_IsUpdate = true;
					}
					m_IsPad = true;
					if (m_IsUpdate) {
						m_IsUpdate = false;
						Guide_Pad_PS4();
					}
					return;
					break;
				case DX_PADTYPE_XBOX_360:
				case DX_PADTYPE_XBOX_ONE:
					break;
				default:
					break;
				}
			}
			//PC
			{//キーボード
				//左スティック
				{
					this->UpKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_W) != 0 || CheckHitKeyWithCheck(KEY_INPUT_UP) != 0);
					this->DownKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_S) != 0 || CheckHitKeyWithCheck(KEY_INPUT_DOWN) != 0);
					this->LeftKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_A) != 0 || CheckHitKeyWithCheck(KEY_INPUT_LEFT) != 0);
					this->RightKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_D) != 0 || CheckHitKeyWithCheck(KEY_INPUT_RIGHT) != 0);
				}
				//ボタン
				{
					this->OKKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0);
					this->NGKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_X) != 0);
					this->m_ShotKey.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
					this->m_MouseWheel.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0);
					this->m_LookKey.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0);
					this->m_AccelKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_R) != 0);
					this->m_BrakeKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_F) != 0);
					this->m_QKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_Q) != 0);
					this->m_EKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_E) != 0);
				}
				//右スティック
				{
					int mx = DXDraw::Instance()->m_DispXSize / 2, my = DXDraw::Instance()->m_DispYSize / 2;
					if (MouseActive && MouseActive->on()) {
						if (MouseActive->trigger()) {
							SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
						}
						GetMousePoint(&mx, &my);
						SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
						SetMouseDispFlag(FALSE);
					}
					else {
						SetMouseDispFlag(TRUE);
					}
					LS_X = std::clamp((float)(mx - DXDraw::Instance()->m_DispXSize / 2), -180.f, 180.f);
					LS_Y = std::clamp(-(float)(my - DXDraw::Instance()->m_DispYSize / 2), -180.f, 180.f);
				}
				//
				if (m_IsPad != false) {
					m_IsUpdate = true;
				}
				m_IsPad = false;
				if (m_IsUpdate) {
					m_IsUpdate = false;
					Guide_Key();
				}
			}
			//
		}
	};

};
