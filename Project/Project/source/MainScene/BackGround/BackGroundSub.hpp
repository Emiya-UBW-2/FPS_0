#pragma once
#include	"../../Header.hpp"


namespace FPS_n2 {
	const int Triangle_Num = 3;

	enum class SideType : char {
		None,
		Triangle,
		Square,
		Mix_OuttoIn,
		Mix_IntoOut,
	};
	struct SideControl {
		//private:
		SideType					Type{ SideType::None };
		int							SquarePoint{ -1 };				//触れている四角の辺
		VECTOR_ref					Pos;
		VECTOR_ref					Pos2D;
		MATRIX_ref					Mat;
		VECTOR_ref					Norm;							// 法線
		COLOR_U8					dif{ 0 };						// ディフューズカラー
		COLOR_U8					spc{ 0 };						// スペキュラカラー
	public:
		void Set(SideType type, const VECTOR_ref& pos, const VECTOR_ref& basepos, const VECTOR_ref& normal,
			COLOR_U8					difcolor,
			COLOR_U8					spccolor,

			int squarepoint = -1) {
			Type = type;
			SquarePoint = squarepoint;//触れている四角の辺
			Pos = pos;

			VECTOR_ref Zvec = VECTOR_ref::up();// (Side[1]->Pos - this->m_BasePos).Norm();
			Mat = MATRIX_ref::Axis1_YZ(normal, Zvec).Inverse();

			Pos2D = MATRIX_ref::Vtrans((Pos - basepos), Mat);
			Pos2D.Set(Pos2D.x(), Pos2D.z(), 0.f);

			Norm = normal;
			dif = difcolor;
			spc = spccolor;
		}
	};

	class LineControl {
		VECTOR_ref m_start;
		VECTOR_ref m_end;
	public:
		void Set(const VECTOR_ref& start, const VECTOR_ref& end) {
			this->m_start = start;
			this->m_end = end;
		}
		const auto& Getstart() const noexcept { return this->m_start; }
		const auto& Getend() const noexcept { return this->m_end; }
	public:
		/**
		 * 与えられた点を含んでいるか
		 * @param {VECTOR_ref} point 調査対象の点
		 * @return {bool} 辺が与えれた点を含んでいたらtrue
		 */
		bool HasPoint(const VECTOR_ref& point) const noexcept { return (this->m_start == point) || (this->m_end == point); }

		bool operator==(const LineControl& tgt) const noexcept {
			return (
				((this->m_start == tgt.m_start) && (this->m_end == tgt.m_end)) ||
				((this->m_start == tgt.m_end) && (this->m_end == tgt.m_start))
				);
		}
	};

	class Triangle {
		std::array<VECTOR_ref, Triangle_Num> m_points;
		std::array<LineControl, Triangle_Num> m_lines;
	public:
		void Set(const VECTOR_ref& point0, const VECTOR_ref& point1, const VECTOR_ref& point2) {
			this->m_points[0] = point0;
			this->m_points[1] = point1;
			this->m_points[2] = point2;
			this->m_lines[0].Set(this->m_points[0], this->m_points[1]);
			this->m_lines[1].Set(this->m_points[1], this->m_points[2]);
			this->m_lines[2].Set(this->m_points[2], this->m_points[0]);
		}
		const auto& Getpoints() const noexcept { return this->m_points; }
		const auto& Getlines() const noexcept { return this->m_lines; }
	public:
		/**
		 * 与えられた辺を含まない点を返す
		 * @param {LineControl} line 調査対象の辺
		 * @return {VECTOR_ref} 与えられた辺に含まれない点
		 */
		const VECTOR_ref* noCommonPointByline(const LineControl& line) const noexcept {
			for (int loop = 0; loop < this->m_points.size(); loop++) {
				if (!line.HasPoint(this->m_points[loop])) {
					return &(this->m_points[loop]);
				}
			}
			return nullptr;
		}

		/**
		 * 与えられた辺以外の辺を返す
		 * @param {LineControl} line 調査対象の辺
		 * @return {Array.<LineControl>} 該当の辺以外の辺の配列
		 */
		std::vector<LineControl> otherlineByline(const LineControl& line) const noexcept {
			std::vector<LineControl> result;
			for (int loop = 0; loop < this->m_lines.size(); loop++) {
				if (!(this->m_lines[loop] == line)) {
					result.emplace_back(this->m_lines[loop]);
				}
			}
			return result;
		}

		/**
		 * 与えられた辺を含んでいるかチェック
		 * @param {LineControl} line 調査対象の辺
		 * @return {bool} 与えられた辺を含んでいたらtrue
		 */
		bool Hasline(const LineControl& line) const noexcept {
			for (int loop = 0; loop < this->m_lines.size(); loop++) {
				if ((this->m_lines[loop] == line)) {
					return true;
				}
			}
			return false;
		}

		/**
		 * 与えられた点の頂点があるか確認
		 * @param {Pointl} point 調査対象の点
		 * @return {bool} 対象の点が頂点にあったらtrue
		 */
		bool HasPoint(const VECTOR_ref& point) const noexcept {
			for (int loop = 0; loop < this->m_points.size(); loop++) {
				if ((this->m_points[loop] == point)) {
					return true;
				}
			}
			return false;
		}

		bool operator==(const Triangle& triangle) const noexcept {
			for (int loop = 0; loop < triangle.m_points.size(); loop++) {
				if (!HasPoint(triangle.m_points[loop])) {
					return false;
				}
			}
			return true;
		}

		//外接円にpointが入っているか
		bool getCircumscribedCircle(const VECTOR_ref& point) {
			VECTOR_ref point1;

			VECTOR_ref p1 = this->m_points[0];
			VECTOR_ref p2 = this->m_points[1];
			VECTOR_ref p3 = this->m_points[2];

			auto x1_p = p1.x() * p1.x();
			auto x2_p = p2.x() * p2.x();
			auto x3_p = p3.x() * p3.x();
			auto y1_p = p1.y() * p1.y();
			auto y2_p = p2.y() * p2.y();
			auto y3_p = p3.y() * p3.y();

			// 外接円の中心座標を計算
			auto c = 2 * ((p2.x() - p1.x()) * (p3.y() - p1.y()) - (p2.y() - p1.y()) * (p3.x() - p1.x()));
			point1.Set(
				((p3.y() - p1.y()) * (x2_p - x1_p + y2_p - y1_p) + (p1.y() - p2.y()) * (x3_p - x1_p + y3_p - y1_p)) / c,
				((p1.x() - p3.x()) * (x2_p - x1_p + y2_p - y1_p) + (p2.x() - p1.x()) * (x3_p - x1_p + y3_p - y1_p)) / c,
				0.f
			);
			return (point - point1).Length() < (p1 - point1).Length();
		}

		//辺に被りがないかどうか
		bool IsOverlap() const noexcept {
			return (
				(this->m_points[0] == this->m_points[1]) ||
				(this->m_points[1] == this->m_points[2]) ||
				(this->m_points[2] == this->m_points[0])
				);
		}
	};

	Triangle GetExternalTriangle(const VECTOR_ref& position, const VECTOR_ref& size);
	//点ABが線の同じ側にいるかどうか
	bool GetSamePoint(const VECTOR_ref& LineA, const VECTOR_ref& LineB, const VECTOR_ref& pointA, const VECTOR_ref& pointB);
	//ドロネー三角形分割を計算
	/*
	points 計算対象の点群
	ExternalTriangle 点を内包する三角形
	 */
	void CalcDelaunay(std::vector<Triangle>* Ans, const std::vector<std::unique_ptr<SideControl>>& points, const Triangle& ExternalTriangle, const std::vector<VECTOR_ref>& GonPoint2D);
};


namespace FPS_n2 {
	//kusa
	class Grass {
		class grass_t {
		public:
			bool			canlook = true;
			Model_Instance	m_Inst;
		public:
			void Init(int total) {
				this->m_Inst.Init("data/model/grass/grass.png", "data/model/grass/model.mv1",-1);
				this->m_Inst.Reset();
				this->m_Inst.Set_start(total);
			}
			void Set_one(const MATRIX_ref& mat) {
				this->m_Inst.Set_one(mat);
			}
			void put(void) {
				canlook = true;
				this->m_Inst.Execute();
			}
			void Dispose(void) {
				this->m_Inst.Dispose();
			}
			/*視界外か否かを判断*/
			void Check_CameraViewClip(const VECTOR_ref& min, const VECTOR_ref& max) {
				this->canlook = true;
				if (CheckCameraViewClip_Box(min.get(), max.get())) {
					this->canlook = false;
					return;
				}
			}
			void Draw(void) {
				if (this->canlook) {
					this->m_Inst.Draw();
				}
			}
		};
		struct GrassPos {
			int X_PosMin = 0;
			int Y_PosMin = 0;
			int X_PosMax = 0;
			int Y_PosMax = 0;
		};
	public:
		static const int grassDiv{ 12 };//^2;
		const float size{ 30.f };
	private:
		const int grasss = 30 * 30;						/*grassの数*/
		std::array<grass_t, grassDiv>grass__;
		std::array<VECTOR_ref, grassDiv>grassPosMin;
		std::array<VECTOR_ref, grassDiv>grassPosMax;
		int Flag = 0;
		std::array<GrassPos, grassDiv> grassPos;
	private:
		int GetColorSoftImage(int softimage, int x_, int y_) {
			int _r_, _g_, _b_;
			int CCC = 0;
			GetPixelSoftImage(softimage, x_, y_, &_r_, &_g_, &_b_, nullptr);
			if (_r_ <= 64) {}
			else if (_r_ <= 192) { CCC |= (1 << 1); }
			else if (_r_ <= 256) { CCC |= (1 << 2); }
			if (_g_ <= 64) {}
			else if (_g_ <= 192) { CCC |= (1 << 4); }
			else if (_g_ <= 256) { CCC |= (1 << 5); }
			if (_b_ <= 64) {}
			else if (_b_ <= 192) { CCC |= (1 << 7); }
			else if (_b_ <= 256) { CCC |= (1 << 8); }
			return CCC;
		}
		//y方向に操作する前提
		void SetMinMax(int CCC, int ID, int softimage, int x_t, int y_t, int sizex, int sizey) {
			int BufC = -1;
			if ((Flag & (1 << ID)) == 0) {
				Flag |= (1 << ID);
				//xmin
				grassPos[ID].X_PosMin = x_t;
				y_t;
				//ymin
				BufC = -1;
				for (int y_ = 0; y_ < sizey; y_++) {
					for (int x_ = grassPos[ID].X_PosMin; x_ < sizex; x_++) {
						BufC = GetColorSoftImage(softimage, x_, y_);
						if (BufC == CCC) {
							grassPos[ID].Y_PosMin = y_;
							break;
						}
						else {
							BufC = -1;
						}
					}
					if (BufC >= 0) { break; }
				}
				//xmax
				BufC = -1;
				for (int x_ = sizex - 1; x_ >= grassPos[ID].X_PosMin; x_--) {
					for (int y_ = sizey - 1; y_ >= grassPos[ID].Y_PosMin; y_--) {
						BufC = GetColorSoftImage(softimage, x_, y_);
						if (BufC == CCC) {
							grassPos[ID].X_PosMax = x_ + 1;
							break;
						}
						else {
							BufC = -1;
						}
					}
					if (BufC >= 0) { break; }
				}
				//ymax
				BufC = -1;
				for (int y_ = sizey - 1; y_ >= grassPos[ID].Y_PosMin; y_--) {
					for (int x_ = grassPos[ID].X_PosMax - 1; x_ >= grassPos[ID].X_PosMin; x_--) {
						BufC = GetColorSoftImage(softimage, x_, y_);
						if (BufC == CCC) {
							grassPos[ID].Y_PosMax = y_ + 1;
							break;
						}
						else {
							BufC = -1;
						}
					}
					if (BufC >= 0) { break; }
				}
				//ok
			}
		}
	public:
		void Init(const MV1* MapCol, int softimage) {
			float MAPX = 300.f*Scale_Rate;
			float MAPZ = 300.f*Scale_Rate;
			float PosX = 0.f;
			float PosZ = 0.f;

			float MINX = -MAPX / 2.f + PosX;
			float MINZ = -MAPZ / 2.f + PosZ;

			float MAXX = MAPX / 2.f + PosX;
			float MAXZ = MAPZ / 2.f + PosZ;

			int sizex = 0, sizey = 0;
			GetSoftImageSize(softimage, &sizex, &sizey);

			Flag = 0;
			for (int x_ = 0; x_ < sizex; x_++) {
				for (int y_ = 0; y_ < sizey; y_++) {
					int CCC = GetColorSoftImage(softimage, x_, y_);
					//255,0,0
					if (CCC == (1 << 2)) {
						SetMinMax(CCC, 0, softimage, x_, y_, sizex, sizey);
					}
					//255,128,0
					else if (CCC == ((1 << 2) | (1 << 4))) {
						SetMinMax(CCC, 1, softimage, x_, y_, sizex, sizey);
					}
					//255,255,0
					else if (CCC == ((1 << 2) | (1 << 5))) {
						SetMinMax(CCC, 2, softimage, x_, y_, sizex, sizey);
					}
					//128,255,0
					else if (CCC == ((1 << 1) | (1 << 5))) {
						SetMinMax(CCC, 3, softimage, x_, y_, sizex, sizey);
					}
					//0,255,0
					else if (CCC == (1 << 5)) {
						SetMinMax(CCC, 4, softimage, x_, y_, sizex, sizey);
					}
					//0,255,128
					else if (CCC == ((1 << 5) | (1 << 7))) {
						SetMinMax(CCC, 5, softimage, x_, y_, sizex, sizey);
					}
					//0,255,255
					else if (CCC == ((1 << 5) | (1 << 8))) {
						SetMinMax(CCC, 6, softimage, x_, y_, sizex, sizey);
					}
					//0,128,255
					else if (CCC == ((1 << 4) | (1 << 8))) {
						SetMinMax(CCC, 7, softimage, x_, y_, sizex, sizey);
					}
					//0,0,255
					else if (CCC == (1 << 8)) {
						SetMinMax(CCC, 8, softimage, x_, y_, sizex, sizey);
					}
					//128,0,255
					else if (CCC == ((1 << 1) | (1 << 8))) {
						SetMinMax(CCC, 9, softimage, x_, y_, sizex, sizey);
					}
					//255,0,255
					else if (CCC == ((1 << 2) | (1 << 8))) {
						SetMinMax(CCC, 10, softimage, x_, y_, sizex, sizey);
					}
					//255,0,128
					else if (CCC == ((1 << 2) | (1 << 7))) {
						SetMinMax(CCC, 11, softimage, x_, y_, sizex, sizey);
					}

					//MINX + (MAXX - MINX) * x_ / sizex = x_t 
					//MINZ + (MAXZ - MINZ) * y_ / sizey = z_t 
				}
			}
			//*/
			for (int ID = 0; ID < grassDiv; ID++) {
				//ポジション決定
				float xp = MINX + (MAXX - MINX) * grassPos[ID].X_PosMin / sizex;
				float zp = MINZ + (MAXZ - MINZ) * grassPos[ID].Y_PosMin / sizey;
				float xp2 = MINX + (MAXX - MINX) * grassPos[ID].X_PosMax / sizex;
				float zp2 = MINZ + (MAXZ - MINZ) * grassPos[ID].Y_PosMax / sizey;
				float xsize = xp2 - xp;
				float zsize = zp2 - zp;
				//
				{
					grassPosMin[ID] = VECTOR_ref::vget(xp, 0.2f, zp);
					grassPosMax[ID] = grassPosMin[ID] + VECTOR_ref::vget(xsize, 1.f, zsize);
					float xmid = xsize / 2.f;
					float zmid = zsize / 2.f;
					if (grasss != 0) {
						auto& tgt_g = grass__[ID];
						tgt_g.Init(grasss);
						for (int i = 0; i < grasss; ++i) {
							float x1 = xmid + GetRandf(xmid);
							float z1 = zmid + GetRandf(zmid);
							while (true) {
								int CCC = GetColorSoftImage(softimage,
									(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
									(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
								);
								if (CCC != 0) {
									break;
								}
								x1 = xmid + GetRandf(xmid);
								z1 = zmid + GetRandf(zmid);
							}

							auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.2f, z1);
							auto tmpscale = VECTOR_ref::vget(size*1.f, (6.0f + GetRandf(6.5f))*size / 12.5f, size*1.f);
							auto res = MapCol->CollCheck_Line(tmpvect + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), tmpvect + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
							if (res.HitFlag == TRUE) { tmpvect = res.HitPosition; }
							tgt_g.Set_one(MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(tmpscale) * MATRIX_ref::RotVec2(VECTOR_ref::up(), ((VECTOR_ref)(res.Normal)).Norm())* MATRIX_ref::Mtrans(tmpvect));
						}
						tgt_g.put();
					}
					{
						auto res = MapCol->CollCheck_Line(grassPosMin[ID] + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), grassPosMin[ID] + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
						if (res.HitFlag == TRUE) { grassPosMin[ID] = res.HitPosition; }
						res = MapCol->CollCheck_Line(grassPosMax[ID] + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), grassPosMax[ID] + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
						if (res.HitFlag == TRUE) { grassPosMax[ID] = res.HitPosition; }
					}
				}
				//
			}
		}
		void Dispose(void) {
			for (int ID = 0; ID < grassDiv; ID++) {
				if (grasss != 0) {
					grass__[ID].Dispose();
				}
			}
		}
		void Draw(void) {
			SetFogEnable(TRUE);
			SetFogColor(184, 187, 118);
			SetDrawAlphaTest(DX_CMP_GREATER, 128);
			//SetUseLighting(FALSE);
			SetUseLightAngleAttenuation(FALSE);
			//auto dir=GetLightDirection();
			//VECTOR_ref vec = (VECTOR_ref)GetCameraPosition() - GetCameraTarget();
			//SetLightDirection(vec.Norm().get());

			for (int ID = 0; ID < grassDiv; ID++) {
#ifdef DEBUG
				//DrawCube3D(grassPosMin[ID].get(), grassPosMax[ID].get(), GetColor(0, 0, 0), GetColor(0, 0, 0), FALSE);
#endif
				if (grasss != 0) {
					this->grass__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
					grass__[ID].Draw();
				}
			}
			//SetLightDirection(dir);

			SetUseLightAngleAttenuation(TRUE);
			//SetUseLighting(TRUE);
			SetDrawAlphaTest(-1, 0);
			SetFogEnable(FALSE);
		}
	};
	//Box2D壁
	class Box2DWall {
	private:
		std::shared_ptr<b2World>	m_b2world;
		std::vector<std::pair<b2Pats, std::array<VECTOR_ref, 2>>>	m_b2wallParts;	//壁をセット
	public://getter
		auto&			GetBox2Dworld(void) { return this->m_b2world; }
	public:
		void			Add(const MV1_REF_POLYGONLIST &p) {
			for (int i = 0; i < p.PolygonNum; i++) {
				this->m_b2wallParts.resize(this->m_b2wallParts.size() + 1);
				this->m_b2wallParts.back().second[0] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
				this->m_b2wallParts.back().second[1] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
				if (b2DistanceSquared(b2Vec2(this->m_b2wallParts.back().second[0].x(), this->m_b2wallParts.back().second[0].z()), b2Vec2(this->m_b2wallParts.back().second[1].x(), this->m_b2wallParts.back().second[1].z())) <= 0.005f * 0.005f) {
					this->m_b2wallParts.pop_back();
				}
				this->m_b2wallParts.resize(this->m_b2wallParts.size() + 1);
				this->m_b2wallParts.back().second[0] = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
				this->m_b2wallParts.back().second[1] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
				if (b2DistanceSquared(b2Vec2(this->m_b2wallParts.back().second[0].x(), this->m_b2wallParts.back().second[0].z()), b2Vec2(this->m_b2wallParts.back().second[1].x(), this->m_b2wallParts.back().second[1].z())) <= 0.005f * 0.005f) {
					this->m_b2wallParts.pop_back();
				}
				this->m_b2wallParts.resize(this->m_b2wallParts.size() + 1);
				this->m_b2wallParts.back().second[0] = p.Vertexs[p.Polygons[i].VIndex[2]].Position;
				this->m_b2wallParts.back().second[1] = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
				if (b2DistanceSquared(b2Vec2(this->m_b2wallParts.back().second[0].x(), this->m_b2wallParts.back().second[0].z()), b2Vec2(this->m_b2wallParts.back().second[1].x(), this->m_b2wallParts.back().second[1].z())) <= 0.005f * 0.005f) {
					this->m_b2wallParts.pop_back();
				}
			}
		}
		void			Init(void) {
			this->m_b2world = std::make_shared<b2World>(b2Vec2(0.0f, 0.0f)); // 剛体を保持およびシミュレートするワールドオブジェクトを構築
			for (auto& w : this->m_b2wallParts) {
				std::array<b2Vec2, 2> vs;								//
				vs[0].Set(w.second[0].x(), w.second[0].z());			//
				vs[1].Set(w.second[1].x(), w.second[1].z());			//
				b2ChainShape chain;										// This a chain shape with isolated vertices
				chain.CreateChain(&vs[0], 2);							//
				b2FixtureDef fixtureDef;								//動的ボディフィクスチャを定義します
				fixtureDef.shape = &chain;								//
				fixtureDef.density = 1.0f;								//ボックス密度をゼロ以外に設定すると、動的になります
				fixtureDef.friction = 0.3f;								//デフォルトの摩擦をオーバーライドします
				b2BodyDef bodyDef;										//ダイナミックボディを定義します。その位置を設定し、ボディファクトリを呼び出します
				bodyDef.type = b2_staticBody;							//
				bodyDef.position.Set(0, 0);								//
				bodyDef.angle = 0.f;									//
				w.first.Set(this->m_b2world->CreateBody(&bodyDef), &chain);	//
			}
		}
		void			Dispose(void) {
			for (auto& w : this->m_b2wallParts) {
				w.first.Dispose();
				w.second[0].clear();
				w.second[1].clear();
			}
			this->m_b2wallParts.clear();
			this->m_b2world.reset();
		}
	};
	//破壊壁
	class BreakWall {
		class WallObj {
		public:
			MV1										m_PlayerCol;
		private:

			float									m_BreakTimer{ -1 };
			bool									m_IsDraw{ true };
			std::array<std::vector<VERTEX3D>, 4>		m_WallVertex;		//壁をセット
			std::array<std::vector<WORD>, 4>		m_WallIndex;		//壁をセット

			std::array<std::array<VECTOR_ref, 3>, 2>	m_WallBase;
			VECTOR_ref									m_Wallnorm;

			GraphHandle								m_TexHandle1;
			GraphHandle								m_TexHandle2;

			std::vector<std::vector<std::unique_ptr<SideControl>>>	m_Side;
			std::vector<std::vector<Triangle>>		m_Tri2D;
			VECTOR_ref								m_BasePos;

			VECTOR_ref								m_Pos;

			std::shared_ptr<std::thread>			m_WallCalc;
			bool									isThreadEnd{ false };
			std::array<std::vector<VECTOR_ref>, 2>	m_GonPoint;//辺の数

			int										Bigcount{ 0 };
			//int addtri = 0;

			float									m_YScale{ 1.f };
		public://getter
			bool			GetIsBreak() {
				return (this->m_BreakTimer == 0.f);
			}
		private:
			//0を基に別オブジェを作る
			void SetTri(int ID, bool isback, float offset) {
				auto& vrt = this->m_WallVertex[ID];
				auto& ind = this->m_WallIndex[ID];
				vrt.resize(this->m_WallVertex[0].size());
				ind.resize(this->m_WallIndex[0].size());
				for (int i = 0; i < vrt.size(); i++) {
					vrt[i] = this->m_WallVertex[0][i];
					vrt[i].pos = ((VECTOR_ref)(vrt[i].pos) + (VECTOR_ref)(vrt[i].norm)*offset).get();
					if (isback) {
						vrt[i].norm = ((VECTOR_ref)(vrt[i].norm)*-1.f).get();
					}
				}
				for (size_t i = 0; i < ind.size() / (size_t)Triangle_Num; i++) {
					size_t index = i * (size_t)Triangle_Num;
					ind.at(index) = this->m_WallIndex[0].at(index);
					if (isback) {
						ind.at(index + 1) = this->m_WallIndex[0].at(index + 2);
						ind.at(index + 2) = this->m_WallIndex[0].at(index + 1);
					}
					else {
						ind.at(index + 1) = this->m_WallIndex[0].at(index + 1);
						ind.at(index + 2) = this->m_WallIndex[0].at(index + 2);
					}
				}
			}

			void			SetFirst(const MV1_REF_POLYGONLIST& PolyList, const VECTOR_ref& pos, float YRad, float YScale) {
				m_Pos = pos;
				m_YScale = YScale;
				auto matrix = MATRIX_ref::RotY(YRad)*MATRIX_ref::Mtrans(pos);
				this->m_WallVertex[0].resize(PolyList.VertexNum);
				VECTOR_ref BasePos = PolyList.Vertexs[0].Position;
				if (BasePos.y() > 0.f) { BasePos.y(BasePos.y()*m_YScale); }
				for (int i = 0; i < this->m_WallVertex[0].size(); i++) {
					VECTOR_ref Pos = PolyList.Vertexs[i].Position;
					if (Pos.y() > 0.f) { Pos.y(Pos.y()*m_YScale); }
					this->m_WallVertex[0][i].pos = MATRIX_ref::Vtrans(Pos, matrix).get();
					this->m_WallVertex[0][i].norm = MATRIX_ref::Vtrans(PolyList.Vertexs[i].Normal, matrix.GetRot()).Norm().get();
					this->m_WallVertex[0][i].dif = PolyList.Vertexs[i].DiffuseColor;
					this->m_WallVertex[0][i].spc = PolyList.Vertexs[i].SpecularColor;

					auto uv = Pos - BasePos;

					this->m_WallVertex[0][i].u = uv.x() / Scale_Rate;
					this->m_WallVertex[0][i].v = -uv.y() / Scale_Rate / m_YScale;
					this->m_WallVertex[0][i].su = uv.x() / Scale_Rate;
					this->m_WallVertex[0][i].sv = -uv.y() / Scale_Rate / m_YScale;
				}
				this->m_WallIndex[0].resize((size_t)PolyList.PolygonNum * (size_t)Triangle_Num);
				for (size_t i = 0; i < this->m_WallIndex[0].size() / (size_t)Triangle_Num; i++) {
					size_t index = i * (size_t)Triangle_Num;
					this->m_WallIndex[0][index + 0] = (WORD)PolyList.Polygons[i].VIndex[0];
					this->m_WallIndex[0][index + 1] = (WORD)PolyList.Polygons[i].VIndex[1];
					this->m_WallIndex[0][index + 2] = (WORD)PolyList.Polygons[i].VIndex[2];
				}
				//反転
				SetTri(1, true, 0.f);
				//正転
				SetTri(2, false, -0.7f);
				//反転
				SetTri(3, true, -0.7f);

				for (size_t i = 0; i < 2; i++) {
					size_t index = i * (size_t)Triangle_Num;
					m_WallBase[i][0] = this->m_WallVertex[0][this->m_WallIndex[0][index + 0]].pos;
					m_WallBase[i][1] = this->m_WallVertex[0][this->m_WallIndex[0][index + 1]].pos;
					m_WallBase[i][2] = this->m_WallVertex[0][this->m_WallIndex[0][index + 2]].pos;
				}
				m_Wallnorm = this->m_WallVertex[0][0].norm;
			}
			void			SetNext() {
				this->m_WallVertex[0].clear();
				this->m_WallIndex[0].clear();
				for (auto& Side : this->m_Side) {
					size_t index = &Side - &this->m_Side.front();
					//出来たものをリストに再登録
					for (auto& s : this->m_Tri2D[index]) {
						for (auto& p : s.Getpoints()) {
							VECTOR_ref p2; p2.Set(p.x(), 0.f, p.y());
							this->m_WallVertex[0].resize(this->m_WallVertex[0].size() + 1);
							this->m_WallVertex[0].back().pos = (MATRIX_ref::Vtrans(p2, Side[0]->Mat.Inverse()) + this->m_BasePos).get();
							this->m_WallVertex[0].back().norm = Side[0]->Norm.get();
							this->m_WallVertex[0].back().dif = Side[0]->dif;
							this->m_WallVertex[0].back().spc = Side[0]->spc;
							this->m_WallVertex[0].back().u = p.x() / Scale_Rate;
							this->m_WallVertex[0].back().v = -p.y() / Scale_Rate / m_YScale;
							this->m_WallVertex[0].back().su = p.x() / Scale_Rate;
							this->m_WallVertex[0].back().sv = -p.y() / Scale_Rate / m_YScale;

							this->m_WallIndex[0].emplace_back((WORD)(this->m_WallVertex[0].size() - 1));
						}
					}
				}
				//反転
				SetTri(1, true, 0.f);
				//正転
				SetTri(2, false, -0.7f);
				//反転
				SetTri(3, true, -0.7f);
			}
			void			SetSide(const VECTOR_ref* basepos = nullptr) {
				if (basepos) { this->m_BasePos = *basepos; }
				for (auto& s : this->m_Side) {
					for (auto&s2 : s) { s2.reset(); }
					s.clear();
				}
				this->m_Side.clear();
				this->m_Side.resize(this->m_WallIndex[0].size() / Triangle_Num);
				this->m_Tri2D.resize(m_Side.size());
				for (auto& Side : this->m_Side) {
					size_t index = &Side - &this->m_Side.front();
					auto GetVertex = [&](int ID) {return &(this->m_WallVertex[0][this->m_WallIndex[0][index * (size_t)Triangle_Num + ID]]); };
					auto GetVertexPos = [&](int ID) {return &(GetVertex(ID)->pos); };
					VECTOR_ref TriPos0 = *GetVertexPos(0);
					VECTOR_ref TriPos1 = *GetVertexPos(1);
					VECTOR_ref TriPos2 = *GetVertexPos(2);
					VECTOR_ref TriNorm = ((TriPos1 - TriPos0).cross(TriPos2 - TriPos0)).Norm();

					Side.resize(Side.size() + Triangle_Num);
					Side[0] = std::make_unique<SideControl>();
					Side[0]->Set(SideType::Triangle, *GetVertexPos(0), this->m_BasePos, TriNorm, GetVertex(0)->dif, GetVertex(0)->spc);

					Side[1] = std::make_unique<SideControl>();
					Side[1]->Set(SideType::Triangle, *GetVertexPos(1), this->m_BasePos, TriNorm, GetVertex(1)->dif, GetVertex(1)->spc);

					Side[2] = std::make_unique<SideControl>();
					Side[2]->Set(SideType::Triangle, *GetVertexPos(2), this->m_BasePos, TriNorm, GetVertex(2)->dif, GetVertex(2)->spc);
				}
			}
		public:
			void			Init(const MV1& obj, const MV1& col, const VECTOR_ref& pos, float YRad, float YScale) {
				this->m_TexHandle1 = GraphHandle::Make(512, (int)(512.f* YScale));
				{
					auto Handle1 = GraphHandle::Load("data/model/wall/tex.dds");//tex.dds//tex.bmp

					this->m_TexHandle1.SetDraw_Screen(false);
					{
						Handle1.DrawExtendGraph(0, 0, 512, (int)(512.f* YScale), false);
					}
					Handle1.Dispose();
				}


				this->m_TexHandle2 = GraphHandle::Load("data/model/wall/plathome.dds");

				SetFirst(MV1GetReferenceMesh(obj.get(), 0, FALSE), pos, YRad, YScale);
				MV1TerminateReferenceMesh(obj.get(), 0, FALSE);

				VECTOR_ref basepos = this->m_WallVertex[0][this->m_WallIndex[0][0]].pos;
				SetSide(&basepos);

				MATERIALPARAM							m_Material;
				m_Material.Diffuse = GetLightDifColor();
				m_Material.Specular = GetLightSpcColor();
				m_Material.Ambient = GetLightAmbColor();
				m_Material.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
				m_Material.Power = 20.0f;
				SetMaterialParam(m_Material);
				this->m_IsDraw = true;

				m_PlayerCol = col.Duplicate();
				m_PlayerCol.SetupCollInfo(2, 2, 2);
				m_PlayerCol.SetMatrix(MATRIX_ref::RotY(YRad)*MATRIX_ref::Mtrans(pos));
				m_PlayerCol.RefreshCollInfo();
			}

			void			Execute(void) {
				if (m_WallCalc.get() != nullptr) {
					if (isThreadEnd) {
						//m_WallCalc->join();
						m_WallCalc->detach();
						m_WallCalc.reset();
						isThreadEnd = false;
					}
				}
				if ((Bigcount > 3) || (this->m_WallIndex[0].size() > 5000)) {
					this->m_BreakTimer = std::max(this->m_BreakTimer - 1.f / FPS / 2.f, 0.f);
				}
				else {
					this->m_BreakTimer = 1.f;
				}
				//printfDx("スレッド数:%d\n", std::thread::hardware_concurrency());

				//addtri += GetMouseWheelRotVolWithCheck();
			}

			bool			Draw() {

				//m_PlayerCol.DrawModel();
				//return true;

				if (
					(
						CheckCameraViewClip_Box(
						(this->m_Pos + VECTOR_ref::vget(-6.25f, 0, -6.25f)).get(),
							(this->m_Pos + VECTOR_ref::vget(6.25f, 40, 6.25f)).get()) == FALSE
						)
					&& this->m_IsDraw
					) {

					SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*this->m_BreakTimer), 0, 255));

					auto vec_tmp = this->m_Pos - GetCameraPosition();
					int sel = (this->m_Wallnorm.dot(vec_tmp.Norm()*-1.f) > 0.f) ? 0 : 1;
					for (int i = 0; i < 4; i++) {
						if ((i % 2) == sel) {
							if (1 <= i && i <= 2) {
								if ((this->m_BreakTimer == 1.f) && (vec_tmp.Length() < 5.f*Scale_Rate)) {
									DrawPolygonIndexed3D(this->m_WallVertex[i].data(), (int)this->m_WallVertex[i].size(), this->m_WallIndex[i].data(), (int)(this->m_WallIndex[i].size()) / Triangle_Num, this->m_TexHandle2.get(), TRUE);
								}
							}
							else {
								DrawPolygonIndexed3D(this->m_WallVertex[i].data(), (int)this->m_WallVertex[i].size(), this->m_WallIndex[i].data(), (int)(this->m_WallIndex[i].size()) / Triangle_Num, this->m_TexHandle1.get(), TRUE);
							}
						}
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					return true;
				}
				return false;
#if false
				for (auto& s : this->m_Side) {
					for (int i = 0; i < s.size(); i++) {
						DrawLine3D(s[i]->Pos.get(), s[(i + 1) % s.size()]->Pos.get(), GetColor(255, 0, 0));
					}
					for (auto& s2 : s) {
						DrawSphere_3D(s2->Pos, Scale_Rate*0.05f, GetColor(255, 0, 0), GetColor(255, 255, 255));
					}
				}

				int x = 400;
				int y = 400;
				int xsize = 0;
				float scale = 20.f;
				x = 400;
				for (auto& s : this->m_Tri2D) {
					for (int loop = 0; loop < (int)s.size(); loop++) {
						int xadd = loop * addtri;
						int yadd = 0;
						{
							int x1 = x + xadd + (int)(s[loop].Getpoints()[0].x()*scale);
							int y1 = y + yadd - (int)(s[loop].Getpoints()[0].y()*scale);
							int x2 = x + xadd + (int)(s[loop].Getpoints()[1].x()*scale);
							int y2 = y + yadd - (int)(s[loop].Getpoints()[1].y()*scale);
							int x3 = x + xadd + (int)(s[loop].Getpoints()[2].x()*scale);
							int y3 = y + yadd - (int)(s[loop].Getpoints()[2].y()*scale);
							DrawTriangle(x1, y1, x2, y2, x3, y3, GetColor(0, 255 * (loop + 1) / (int)s.size(), 0), TRUE);
						}
					}
					x += xsize;
				}
#endif
			}

			bool			CheckHit(const VECTOR_ref& repos, VECTOR_ref* pos, VECTOR_ref* norm, float radius) {
				VECTOR_ref pos_t = *pos;
				bool ishit = false;
				if (this->m_BreakTimer != 1.f) {
					return false;
				}
				bool isback = ((pos_t - repos).dot(m_Wallnorm) >= 0.f);
				VECTOR_ref vec = (isback) ? ((pos_t - repos)*-1.f) : (pos_t - repos);
				VECTOR_ref RePos = isback ? pos_t : repos;
				{
					float Radius = radius * Scale_Rate*1.8f*5.f;
					int N_gon = 5 + std::clamp((int)(radius / 0.015f), 0, 11);
					VECTOR_ref xaxis = vec.Norm().cross(VECTOR_ref::up());
					VECTOR_ref yaxis = vec.Norm().cross(xaxis);
					m_GonPoint[0].resize(N_gon);
					m_GonPoint[1].resize(N_gon);
					for (int gon = 0; gon < N_gon; gon++) {
						float rad = deg2rad(360.f * (0.5f + (float)gon) / (float)N_gon);
						m_GonPoint[0][gon] = RePos + (xaxis * sin(rad) + yaxis * cos(rad))*Radius;
						//平面上のくりぬきポイント取得
						VECTOR_ref TriPos0 = m_WallBase[0][0];
						VECTOR_ref TriPos1 = m_WallBase[0][1];
						VECTOR_ref TriPos2 = m_WallBase[0][2];
						VECTOR_ref TriNorm = ((TriPos1 - TriPos0).cross(TriPos2 - TriPos0)).Norm();

						VECTOR_ref PosN = Plane_Point_MinLength_Position(TriPos0.get(), TriNorm.get(), m_GonPoint[0][gon].get());
						float pAN = std::abs((m_GonPoint[0][gon] - PosN).dot(TriNorm));
						float pBN = std::abs(((m_GonPoint[0][gon] + vec) - PosN).dot(TriNorm));
						m_GonPoint[1][gon] = m_GonPoint[0][gon] + vec * (pAN / (pAN + pBN));
					}

					for (int gon = 0; gon < N_gon; gon++) {
						VECTOR_ref repos_tmp, pos_tmp;
						if (gon == N_gon) {
							repos_tmp = repos;
							pos_tmp = pos_t;
						}
						else {
							repos_tmp = m_GonPoint[0][gon];
							pos_tmp = m_GonPoint[0][gon] + vec;
						}
						for (int i = 0; i < 2; i++) {
							auto res1 = HitCheck_Line_Triangle(repos_tmp.get(), pos_tmp.get(), m_WallBase[i][0].get(), m_WallBase[i][1].get(), m_WallBase[i][2].get());
							if (res1.HitFlag == TRUE) {
								//元の4角にて当たったら
								for (int index = 0; index < this->m_WallIndex[0].size() / Triangle_Num; index++) {
									auto GetVertexPos = [&](int ID) {return &(this->m_WallVertex[0][this->m_WallIndex[0].at(index * (size_t)Triangle_Num + ID)].pos); };
									auto res2 = HitCheck_Line_Triangle(repos_tmp.get(), pos_tmp.get(), *GetVertexPos(0), *GetVertexPos(1), *GetVertexPos(2));
									if (res2.HitFlag == TRUE) {
										ishit = true;
										*pos = repos + (*pos - repos).Norm()*(((VECTOR_ref)res1.Position - repos).size());
										if (norm) {
											*norm = this->m_Wallnorm;
										}
										break;
									}
								}
								break;
							}
						}
						if (ishit) { break; }
					}
				}
				{
					VECTOR_ref repos_tmp = RePos, pos_tmp = RePos + vec;
					for (int i = 0; i < 2; i++) {
						auto res1 = HitCheck_Line_Triangle(repos_tmp.get(), pos_tmp.get(), m_WallBase[i][0].get(), m_WallBase[i][1].get(), m_WallBase[i][2].get());
						if (res1.HitFlag == TRUE) {
							//元の4角にて当たったら
							for (int index = 0; index < this->m_WallIndex[0].size() / Triangle_Num; index++) {
								auto GetVertexPos = [&](int ID) {return &(this->m_WallVertex[0][this->m_WallIndex[0].at(index * (size_t)Triangle_Num + ID)].pos); };
								auto res2 = HitCheck_Line_Triangle(repos_tmp.get(), pos_tmp.get(), *GetVertexPos(0), *GetVertexPos(1), *GetVertexPos(2));
								if (res2.HitFlag == TRUE) {
									this->m_TexHandle1.SetDraw_Screen(false);
									{
										VECTOR_ref TriPos0 = m_WallBase[0][0];
										VECTOR_ref TriPos1 = m_WallBase[0][1];
										VECTOR_ref TriPos2 = m_WallBase[0][2];
										VECTOR_ref TriNorm = ((TriPos1 - TriPos0).cross(TriPos2 - TriPos0)).Norm();

										VECTOR_ref Zvec = VECTOR_ref::up();// (Side[1]->Pos - this->m_BasePos).Norm();
										MATRIX_ref Mat = MATRIX_ref::Axis1_YZ(TriNorm, Zvec).Inverse();

										VECTOR_ref PosN = Plane_Point_MinLength_Position(TriPos0.get(), TriNorm.get(), RePos.get());
										float pAN = std::abs((RePos - PosN).dot(TriNorm));
										float pBN = std::abs(((RePos + vec) - PosN).dot(TriNorm));
										VECTOR_ref Pos = RePos + vec * (pAN / (pAN + pBN));

										VECTOR_ref Pos2D = MATRIX_ref::Vtrans((Pos - this->m_BasePos), Mat) / Scale_Rate;
										Pos2D.Set(Pos2D.x(), -Pos2D.z(), 0.f);

										DrawCircle((int)(Pos2D.x() * 512), (int)(Pos2D.y() * 512), (int)(512.f * radius*12.5f), GetColor(0, 0, 0));
									}
									break;
								}
							}
							break;
						}
					}
				}
				if (ishit) {
					if (m_WallCalc.get() == nullptr) {
						m_WallCalc = std::make_shared<std::thread>([&]() {
							isThreadEnd = false;
							LONGLONG OLDTime = GetNowHiPerformanceCount();
							auto GonPoint2 = m_GonPoint;//辺の数
							int N_gon = (int)GonPoint2[1].size();
							{
								for (auto& Side : this->m_Side) {
									int index = (int)(&Side - &this->m_Side.front());
									auto GetVertex = [&](int ID) {return &(this->m_WallVertex[0][this->m_WallIndex[0].at(index * (size_t)Triangle_Num + ID)]); };
									auto GetVertexPos = [&](int ID) {return &(GetVertex(ID)->pos); };
									VECTOR_ref TriPos0 = *GetVertexPos(0);
									VECTOR_ref TriPos1 = *GetVertexPos(1);
									VECTOR_ref TriPos2 = *GetVertexPos(2);
									VECTOR_ref TriNorm = ((TriPos1 - TriPos0).cross(TriPos2 - TriPos0)).Norm();

									for (int gon = 0; gon < N_gon; gon++) {
										//直に入っている部分
										auto res2 = HitCheck_Line_Triangle(GonPoint2[0][gon].get(), (GonPoint2[0][gon] + (GonPoint2[1][gon] - GonPoint2[0][gon])*2.f).get(), TriPos0.get(), TriPos1.get(), TriPos2.get());
										if ((res2.HitFlag == TRUE)) {
											Side.resize(Side.size() + 1);
											Side.back() = std::make_unique<SideControl>();
											Side.back()->Set(SideType::Square, res2.Position, this->m_BasePos, TriNorm, GetVertex(0)->dif, GetVertex(0)->spc, gon);
										}
									}
									//n_sideの中にある点の削除(外周としては不要なもののため)
									for (int s = 0; s < Side.size(); s++) {
										if (Side[s]->Type == SideType::Triangle) {
											bool isIn = true;
											for (int gon = 0; gon < N_gon; gon++) {
												if (!GetSamePoint(GonPoint2[1][gon], GonPoint2[1][(gon + 1) % N_gon], GonPoint2[1][(gon + 2) % N_gon], Side[s]->Pos)) {
													isIn = false;
													break;
												}
											}
											if (isIn) {
												Side[s].reset();
												Side.erase(Side.begin() + s);
												s--;
											}
										}
									}
									//三角と辺の交点を追加
									for (int gon = 0; gon < N_gon; gon++) {
										VECTOR_ref pos1 = GonPoint2[1][gon];
										VECTOR_ref pos2 = GonPoint2[1][(gon + 1) % N_gon];
										for (int tri = 0; tri < Triangle_Num; tri++) {
											VECTOR_ref TriPost0 = *GetVertexPos(tri);
											VECTOR_ref TriPost1 = *GetVertexPos((tri + 1) % Triangle_Num);
											VECTOR_ref TriPost2 = *GetVertexPos((tri + 2) % Triangle_Num);
											SEGMENT_SEGMENT_RESULT Res;
											if (GetSegmenttoSegment(pos1, pos2, TriPost0, TriPost1, &Res)) {
												//pos2が三角の辺のどちらにいるか
												if (GetSamePoint(TriPost0, TriPost1, TriPost2, pos2)) {
													Side.resize(Side.size() + 1);
													Side.back() = std::make_unique<SideControl>();
													Side.back()->Set(SideType::Mix_OuttoIn, Res.SegA_MinDist_Pos, this->m_BasePos, TriNorm, GetVertex(tri)->dif, GetVertex(tri)->spc, gon);
												}
												else {
													Side.resize(Side.size() + 1);
													Side.back() = std::make_unique<SideControl>();
													Side.back()->Set(SideType::Mix_IntoOut, Res.SegA_MinDist_Pos, this->m_BasePos, TriNorm, GetVertex(tri)->dif, GetVertex(tri)->spc, (gon + 1) % N_gon);
												}
											}
										}
									}
									std::vector<VECTOR_ref>	Point2D;//辺の数
									{
										struct DATA {
											VECTOR_ref point;//辺の数
											int SquarePoint{ 0 };//辺の数
											SideType type{ SideType::None };//辺の数
										};
										std::vector<DATA> data;//辺の数
										for (int s = 0; s < Side.size(); s++) {
											if (Side[s]->SquarePoint != -1) {
												//IntoOutと番号が同じならOuttoInが優先
												data.resize(data.size() + 1);
												data.back().point = Side[s]->Pos2D;
												data.back().SquarePoint = Side[s]->SquarePoint;
												data.back().type = Side[s]->Type;
											}
										}
										std::sort(data.begin(), data.end(), [](const DATA& a, const DATA& b) {
											return (a.SquarePoint == b.SquarePoint) ? ((a.type == SideType::Mix_IntoOut) && (b.type == SideType::Mix_OuttoIn)) : (a.SquarePoint < b.SquarePoint);
										});
										for (int loop = 0; loop < data.size(); loop++) {
											Point2D.emplace_back(data[loop].point);
										}
									}
									// 一番外側の巨大三角形を生成、ここでは画面内の点限定として画面サイズを含む三角形を作る
									VECTOR_ref position; position.Set(0, 0, 0.f);
									VECTOR_ref Size; Size.Set(200.f, 200.f, 0.f);
									CalcDelaunay(&this->m_Tri2D[index], Side, GetExternalTriangle(position, Size), Point2D);
									Point2D.clear();
								}
								this->m_IsDraw = false;
								SetNext();
								SetSide();
								this->m_IsDraw = true;
							}
							isThreadEnd = true;
							if ((GetNowHiPerformanceCount() - OLDTime) > 16 * 2 * 1000) {
								Bigcount++;
							}
						});
					}
				}
				return ishit;
			}
		};
	private:
		MV1							m_ObjGround_Wallpoint;
		MV1							m_objWall;
		MV1							m_objWallCol;
		std::vector<WallObj>		m_Walls;
		std::vector<std::array<VECTOR_ref, 2>>	m_WallParts;	//box2d壁をセット
	public://getter
		const auto&		GetWallGroundCol(int id) { return this->m_Walls.at(id).m_PlayerCol; }
		const auto		GetWallGroundColNum() const noexcept { return this->m_Walls.size(); }
		const auto		GetWallCol(const VECTOR_ref& repos, VECTOR_ref* pos, VECTOR_ref* norm, float radius) {
			bool res = false;
			for (auto& w : this->m_Walls) {
				if (w.CheckHit(repos, pos, norm, radius)) {
					res = true;
				}
			}
			return res;
		}
	public:
		void			DrawCommon() {
			for (auto& w : this->m_Walls) {
				w.Draw();
			}
		}
	public:
		void			Init(void) {
			MV1::Load("data/model/map/wallpoint.mv1", &this->m_ObjGround_Wallpoint);
			MV1::Load("data/model/wall/model.mqoz", &this->m_objWall);
			MV1::Load("data/model/wallcol/model.mv1", &this->m_objWallCol);
			{
				float lim = 0.5f;
				MV1_REF_POLYGONLIST p = MV1GetReferenceMesh(this->m_ObjGround_Wallpoint.get(), 0, FALSE);
				for (int i = 0; i < p.PolygonNum; i++) {
					VECTOR_ref pos0 = p.Vertexs[p.Polygons[i].VIndex[0]].Position;
					VECTOR_ref pos1 = p.Vertexs[p.Polygons[i].VIndex[1]].Position;
					VECTOR_ref pos2 = p.Vertexs[p.Polygons[i].VIndex[2]].Position;

					auto p0 = (pos0 - pos1); auto py0 = p0.y(); p0.y(0);
					auto p1 = (pos1 - pos2); auto py1 = p1.y();  p1.y(0);
					auto p2 = (pos2 - pos0); auto py2 = p2.y();  p2.y(0);

					if (p0.Length() > lim && py0 <= lim) {
						this->m_WallParts.resize(this->m_WallParts.size() + 1);
						this->m_WallParts.back()[0] = pos0;
						this->m_WallParts.back()[1] = pos1;
						continue;
					}
					if (p1.Length() > lim && py1 <= lim) {
						this->m_WallParts.resize(this->m_WallParts.size() + 1);
						this->m_WallParts.back()[0] = pos1;
						this->m_WallParts.back()[1] = pos2;
						continue;
					}
					if (p2.Length() > lim && py2 <= lim) {
						this->m_WallParts.resize(this->m_WallParts.size() + 1);
						this->m_WallParts.back()[0] = pos2;
						this->m_WallParts.back()[1] = pos0;
						continue;
					}
				}
				MV1TerminateReferenceMesh(this->m_ObjGround_Wallpoint.get(), 0, FALSE);
				this->m_Walls.resize(this->m_WallParts.size());
				for (int i = 0; i < this->m_WallParts.size(); i++) {
					auto pos = (this->m_WallParts[i][0] + this->m_WallParts[i][1]) / 2.f;
					auto vec = (this->m_WallParts[i][0] - this->m_WallParts[i][1]); vec.y(0);

					this->m_Walls[i].Init(this->m_objWall, this->m_objWallCol, pos, std::atan2f(vec.z(), vec.x()), 2.7f);
				}
				//this->m_Walls.clear();
			}
			SetUseBackCulling(TRUE);
			SetTextureAddressModeUV(DX_TEXADDRESS_WRAP, DX_TEXADDRESS_WRAP);
		}
		void			Execute(void) {
			for (auto& w : this->m_Walls) {
				w.Execute();
			}
			//破壊
			for (int i = 0; i < this->m_Walls.size(); i++) {
				if (this->m_Walls[i].GetIsBreak()) {
					std::swap(this->m_Walls.back(), this->m_Walls[i]);
					this->m_Walls.pop_back();
					i--;
				}
			}
		}
		void			Dispose(void) {
			this->m_Walls.clear();
		}
	};
	//
	class Builds {
		int						m_mesh{ -1 };
		MV1						m_Col;
		MV1						m_ColBox2D;
		MATRIX_ref				m_mat;
	public:
		const auto&		GetMeshSel(void) const noexcept { return m_mesh; }
		const auto&		GetColBox2D(void) const noexcept { return m_ColBox2D; }
		const auto&		GetMatrix(void) const noexcept { return m_mat; }
		const auto		GetCol(const VECTOR_ref& repos, const VECTOR_ref& pos) const noexcept { return this->m_Col.CollCheck_Line(repos, pos, m_mesh); }
	public:
		void		Set(const MV1& ColModel, const MV1& Box2DModel, int frame) {
			this->m_Col = ColModel.Duplicate();
			this->m_ColBox2D = Box2DModel.Duplicate();
			m_mesh = frame;
			this->m_Col.SetupCollInfo(1, 1, 1, m_mesh);
		}
		void		ChangeSel(int frame) {
			if (this->m_Col.IsActive()) {
				MV1TerminateCollInfo(this->m_Col.get(), m_mesh);
				m_mesh = frame;
				this->m_Col.SetupCollInfo(1, 1, 1, m_mesh);
			}
		}
		void		SetPosition(const MV1& colModel, VECTOR_ref pos, float rad, bool isTilt) {
			m_mat.clear();
			auto res = colModel.CollCheck_Line(pos + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), pos + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
			if (res.HitFlag == TRUE) {
				pos = res.HitPosition;
				pos += VECTOR_ref::up()*(0.1f*Scale_Rate);
				if (isTilt) {
					m_mat = MATRIX_ref::RotVec2(VECTOR_ref::up(), res.Normal);
				}
			}
			m_mat = MATRIX_ref::RotY(rad)*m_mat*MATRIX_ref::Mtrans(pos);
			this->m_Col.SetMatrix(m_mat);
			this->m_Col.RefreshCollInfo(m_mesh);
			this->m_ColBox2D.SetMatrix(m_mat);
		}
	};
	class BuildControl {
		MV1							m_ObjBuildBase;
		MV1							m_ColBuildBase;
		MV1							m_ColBox2DBuildBase;

		std::vector<Builds>			m_ObjBuilds;

		std::vector<Model_Instance>	m_Inst;
	public:
		const auto&		GetBuildCol(void) const noexcept { return this->m_ObjBuilds; }
		const auto&		GetBuildCol(void) noexcept { return this->m_ObjBuilds; }
	public:
		void			Load(void) noexcept {
			MV1::Load("data/model/build/model.mv1", &this->m_ObjBuildBase);
			MV1::Load("data/model/build/col.mv1", &this->m_ColBuildBase);
			MV1::Load("data/model/build/colBox2D.mv1", &this->m_ColBox2DBuildBase);
		}
		void			Init(const MV1* MapCol) noexcept {

			m_ObjBuilds.resize((4 * 30 + 1) + (15 * 4) * 5 + 15 * (14 - 5 - 1) + 5);
			m_Inst.resize(15);
			{
				std::vector<Builds*> CrossPoints;
				auto FindCross = [&](float length) {
					int total = 0;
					for (auto& bu : m_ObjBuilds) {
						if (bu.GetMeshSel() == 0) {
							bool Hit = (std::find_if(CrossPoints.begin(), CrossPoints.end(), [&](Builds* tmp) { return tmp == &bu; }) != CrossPoints.end());
							if (!Hit) {
								total++;
							}
						}
					}

					float rad;
					VECTOR_ref BasePos;
					int count = 0;
					while (true) {
						//count++;
						int rand = GetRand(total - 1) + 1;
						int rbuf = rand;
						Builds* Base{ nullptr };
						for (auto& bu : m_ObjBuilds) {
							if (bu.GetMeshSel() == 0) {
								bool Hit = (std::find_if(CrossPoints.begin(), CrossPoints.end(), [&](Builds* tmp) { return tmp == &bu; }) != CrossPoints.end());
								if (!Hit) {
									rbuf--;
									if (rbuf == 0) {
										Base = &bu;
										break;
									}
								}
							}
						}

						rad = std::atan2f(Base->GetMatrix().zvec().x(), Base->GetMatrix().zvec().z()) + deg2rad(90.f)*(GetRand(2) > 1 ? 1.f : -1.f);
						BasePos = Base->GetMatrix().pos();
						BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(8.f*Scale_Rate);

						if (BasePos.Length() > 200.f*Scale_Rate) {
							continue;
						}

						bool isnear = false;
						for (auto& bu : CrossPoints) {
							auto P2 = (bu->GetMatrix().pos() - BasePos).Length();
							if (P2 < length) {
								isnear = true;
								break;
							}
						}
						if (!isnear || count>100) {
							CrossPoints.emplace_back(Base);
							break;
						}
					}
					return std::forward_as_tuple(BasePos, rad);
				};
				auto FindLoad = [&]() {
					int total = 0;
					for (auto& bu : m_ObjBuilds) { if (bu.GetMeshSel() == 0) { total++; } }

					float rad;
					VECTOR_ref BasePos;
					int count = 0;
					while (true) {
						//count++;
						int rand = GetRand(total - 1) + 1;
						int rbuf = rand;
						Builds* Base{ nullptr };
						for (auto& bu : m_ObjBuilds) {
							if (bu.GetMeshSel() == 0) {
								rbuf--;
								if (rbuf == 0) {
									Base = &bu;
									break;
								}
							}
						}

						rad = std::atan2f(Base->GetMatrix().zvec().x(), Base->GetMatrix().zvec().z()) + deg2rad(90.f)*(GetRand(2) > 1 ? 1.f : -1.f);
						BasePos = Base->GetMatrix().pos();
						BasePos += VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(8.f*Scale_Rate);

						if (count > 100) {
							break;
						}

						if (BasePos.Length() > 200.f*Scale_Rate) {
							continue;
						}

						bool isnear = false;
						for (auto& bu : m_ObjBuilds) {
							if (Base == &bu) { continue; }
							if (bu.GetMeshSel() >= 5) {
								auto P2 = (bu.GetMatrix().pos() - BasePos).Length();
								if (P2 < 14.f*Scale_Rate) {
									isnear = true;
									break;
								}
							}
						}
						if (!isnear) {
							for (auto& bu : m_ObjBuilds) {
								if (Base == &bu) { continue; }
								if (bu.GetMeshSel() == 0) {
									auto P2 = (bu.GetMatrix().pos() - BasePos).Length();
									if (P2 < 4.f*Scale_Rate) {
										isnear = true;
										break;
									}
								}
							}
						}
						if (!isnear) {
							for (auto& bu : m_ObjBuilds) {
								if (Base == &bu) { continue; }
								if (bu.GetMeshSel() == 3) {
									auto P2 = (bu.GetMatrix().pos() - BasePos).Length();
									if (P2 < 14.f*Scale_Rate) {
										isnear = true;
										break;
									}
								}
							}
						}
						if (!isnear) {
							break;
						}
					}
					return std::forward_as_tuple(BasePos, rad);
				};
				int mini = 0;
				{
					int adds = 30;
					float radBase = deg2rad(GetRandf(180.f));
					float rad = radBase;
					VECTOR_ref BasePos;
					rad = radBase + deg2rad(0.f);
					{
						BasePos.Set(0.f, 0.f, 0.f);
						BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(7.f*Scale_Rate);
						for (int i = mini; i < mini + adds; i++) {
							m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
							m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, true);

							rad += deg2rad(GetRandf(5.f));
							BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(7.f*Scale_Rate);
						}
						mini += adds;
					}
					rad = radBase + deg2rad(90.f);
					{
						BasePos.Set(0.f, 0.f, 0.f);
						BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(7.f*Scale_Rate);
						for (int i = mini; i < mini + adds; i++) {
							m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
							m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, true);

							rad += deg2rad(GetRandf(5.f));
							BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(7.f*Scale_Rate);
						}
						mini += adds;
					}
					rad = radBase + deg2rad(180.f);
					{
						BasePos.Set(0.f, 0.f, 0.f);
						BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(7.f*Scale_Rate);
						for (int i = mini; i < mini + adds; i++) {
							m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
							m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, true);

							rad += deg2rad(GetRandf(5.f));
							BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(7.f*Scale_Rate);
						}
						mini += adds;
					}
					rad = radBase + deg2rad(-90.f);
					{
						BasePos.Set(0.f, 0.f, 0.f);
						BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(7.f*Scale_Rate);
						for (int i = mini; i < mini + adds; i++) {
							m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
							m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, true);

							rad += deg2rad(GetRandf(5.f));
							BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(7.f*Scale_Rate);
						}
						mini += adds;
					}
					rad = radBase + deg2rad(-90.f);
					{
						BasePos.Set(0.f, 0.f, 0.f);
						int i = mini;
						m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 3);
						m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, true);
						CrossPoints.emplace_back(&m_ObjBuilds[i]);
						mini += 1;
					}
				}
				for (int Z = 0; Z < 5; Z++) {
					{
						float rad = 0;
						VECTOR_ref BasePos;
						int adds = 15;
						for (int i = 0; i < 4; i++) {
							std::tie(BasePos, rad) = FindCross((60.f*Scale_Rate) / (float)(Z + 1));
							{
								for (int j = mini; j < mini + adds; j++) {
									bool iscross = false;
									for (auto& bu : m_ObjBuilds) {
										if (bu.GetMeshSel() == 0) {
											auto P2 = (bu.GetMatrix().pos() - BasePos).Length();
											if (P2 < 4.f*Scale_Rate) {
												iscross = true;
												break;
											}
										}
									}
									if (!iscross) {
										m_ObjBuilds[j].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
										m_ObjBuilds[j].SetPosition(*MapCol, BasePos, rad, true);
									}
									rad += deg2rad(GetRandf(5.f));
									BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(7.f*Scale_Rate);
								}
							}
							mini += adds;
						}
					}
					for (auto& bu : CrossPoints) {
						bu->ChangeSel(3);
					}
				}
				{
					VECTOR_ref BasePos;
					float rad = 0.f;
					int adds = 15;
					for (int bu = 0; bu < 7 - 5; bu++) {
						for (int i = mini; i < mini + adds; i++) {
							std::tie(BasePos, rad) = FindLoad();
							m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 5 + bu);
							m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, false);
						}
						mini += adds;
					}
				}
				{
					VECTOR_ref BasePos;
					float rad = 0.f;
					int adds = 5;
					{
						for (int i = mini; i < mini + adds; i++) {
							std::tie(BasePos, rad) = FindLoad();
							m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 5 + 2);
							m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, false);
						}
						mini += adds;
					}
				}
				{
					VECTOR_ref BasePos;
					float rad = 0.f;
					int adds = 15;
					for (int bu = 8 - 5; bu < 14 - 5; bu++) {
						for (int i = mini; i < mini + adds; i++) {
							std::tie(BasePos, rad) = FindLoad();
							m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 5 + bu);
							m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, false);
						}
						mini += adds;
					}
				}
				CrossPoints.clear();
			}
			for (int i = 0; i < 15; i++) {
				int total = 0;
				for (auto& b : m_ObjBuilds) {
					if (b.GetMeshSel() == i) {
						total++;
					}
				}
				this->m_Inst[i].Init(this->m_ObjBuildBase, i);
				this->m_Inst[i].Reset();
				this->m_Inst[i].Set_start(total);
				for (auto& b : m_ObjBuilds) {
					if (b.GetMeshSel() == i) {
						this->m_Inst[i].Set_one(b.GetMatrix());
					}
				}
				this->m_Inst[i].Execute();
			}

			MATERIALPARAM							m_Material;
			m_Material.Diffuse = GetLightDifColor();
			m_Material.Specular = GetLightSpcColor();
			m_Material.Ambient = GetLightAmbColor();
			m_Material.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
			m_Material.Power = 20.0f;
			SetMaterialParam(m_Material);
		}
		void			Draw() noexcept {
			for (auto& b : this->m_Inst) {
				b.Draw();
			}
		}
		void			Dispose(void) noexcept {
			this->m_ObjBuildBase.Dispose();
			this->m_ColBuildBase.Dispose();
			this->m_ColBox2DBuildBase.Dispose();
			for (auto& b : this->m_Inst) {
				b.Dispose();
			}
			this->m_Inst.clear();
		}

	};
	//
	struct treePats {
		MV1 obj, obj_far;
		MATRIX_ref mat;
		VECTOR_ref pos;
		bool fall_flag = false;
		VECTOR_ref fall_vec;
		float fall_rad = 0.f;
		float fall_speed = 0.f;

		treePats() {
			fall_flag = false;
			fall_vec = VGet(0.f, 0.f, 1.f);
			fall_rad = 0.f;
			fall_speed = 0.f;
		}
	};
	class TreeControl {
		MV1 tree_model, tree_far;

		std::vector<treePats>		tree;
	public:
		void			CheckTreetoSquare(
			const VECTOR_ref& cornerLF, const VECTOR_ref& cornerRF, const VECTOR_ref& cornerRR, const VECTOR_ref& cornerLR
			, const VECTOR_ref& center, float speed
		) {
			for (auto& l : tree) {
				if (!l.fall_flag) {
					auto p0 = cornerLF;
					auto p1 = cornerRF;
					auto p2 = cornerRR;
					auto p3 = cornerLR;
					p0.y(l.pos.y());
					p1.y(l.pos.y());
					p2.y(l.pos.y());
					p3.y(l.pos.y());

					size_t cnt = 0;
					cnt += (((p0 - p1).cross(l.pos - p1)).y() >= 0);
					cnt += (((p1 - p2).cross(l.pos - p2)).y() >= 0);
					cnt += (((p2 - p3).cross(l.pos - p3)).y() >= 0);
					cnt += (((p3 - p0).cross(l.pos - p0)).y() >= 0);
					if (cnt == 4) {
						l.fall_vec = VGet((l.pos - center).z(), 0.f, -(l.pos - center).x());
						l.fall_flag = true;
						l.fall_speed = std::clamp(speed*2.f, 0.1f, 6.f) * deg2rad(30.f / FPS);
					}
				}
			}
		}
	public:
		void	Load() noexcept {
			MV1::Load("data/model/tree/model.mv1", &tree_model, true); //木
			MV1::Load("data/model/tree/model2.mv1", &tree_far, true); //木
		}
		void	Init(const MV1* MapCol, const std::vector<Builds>& BGBuild) noexcept {
			tree.resize(300);
			for (auto& t : tree) {
				auto scale = 15.f / 10.f*Scale_Rate;
				t.mat = MATRIX_ref::GetScale(VECTOR_ref::vget(scale, scale, scale));
				while (true) {
					t.pos = VECTOR_ref::vget(
						(float)(GetRand(300) - 150)*1.f*Scale_Rate,
						0.f,
						(float)(GetRand(300) - 150)*1.f*Scale_Rate);
					bool isnear = false;
					for (const auto& bu : BGBuild) {
						if (bu.GetMeshSel() >= 0) {
							auto pos_p = (t.pos - bu.GetMatrix().pos()); pos_p.y(0);
							if (pos_p.size() < 20.f*Scale_Rate) {
								isnear = true;
								break;
							}
						}
					}
					if (!isnear) { break; }
				}
				auto res = MapCol->CollCheck_Line(t.pos + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), t.pos + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
				if (res.HitFlag == TRUE) {
					t.pos = res.HitPosition;
				}
				t.obj = tree_model.Duplicate();
				t.obj_far = tree_far.Duplicate();
				t.obj_far.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
			}
		}
		void	Execute(void) noexcept {
			//木セット
			for (auto& l : tree) {
				if (l.fall_flag) {
					l.fall_rad = std::clamp(l.fall_rad + l.fall_speed, deg2rad(0.f), deg2rad(80.f));
					if (l.fall_rad == deg2rad(80.f)) {
						l.fall_speed = -l.fall_speed / 5.f;
					}
					l.fall_speed += deg2rad(1.f / FPS);
				}
				l.obj.SetMatrix(MATRIX_ref::RotAxis(l.fall_vec, l.fall_rad) * l.mat * MATRIX_ref::Mtrans(l.pos));
			}
		}
		void	Dispose(void) noexcept {
			tree_model.Dispose(); //木
			tree_far.Dispose(); //木
			for (auto&t : tree) {
				t.obj.Dispose();
				t.obj_far.Dispose();
			}
			tree.clear();
		}

		void	Draw(bool isSetFog) noexcept {
			int fog_enable = 0;
			int fog_mode = 0;
			int fog_r = 0, fog_g = 0, fog_b = 0;
			float fog_start = 0.f, fog_end = 0.f;
			float fog_density = 0.f;
			if (isSetFog) {
				fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効  FALSE:無効 )
				fog_mode = GetFogMode();														// フォグモードを取得する
				GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
				GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
				fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )

				SetFogEnable(TRUE);
				SetFogColor(0, 12, 0);
				SetFogStartEnd(Scale_Rate*5.f, Scale_Rate*50.f);
			}
			auto Farlimit = 60.f*Scale_Rate;
			auto Farlimit2 = 40.f*Scale_Rate;
			for (auto& l : tree) {
				auto LengthtoCam = (l.pos - GetCameraPosition());

				if (CheckCameraViewClip_Box(
					(l.pos + VECTOR_ref::vget(-20, 0, -20)*Scale_Rate).get(),
					(l.pos + VECTOR_ref::vget(20, 20, 20)*Scale_Rate).get()) == FALSE
					) {
					if (LengthtoCam.Length() > Farlimit && isSetFog) {
						SetUseLighting(FALSE);
						LengthtoCam.y(0.f); LengthtoCam = LengthtoCam.Norm();
						float rad = std::atan2f(VECTOR_ref::front().cross(LengthtoCam).y(), VECTOR_ref::front().dot(LengthtoCam));
						l.obj_far.SetMatrix(MATRIX_ref::RotY(rad) * l.mat * MATRIX_ref::Mtrans(l.pos));
						l.obj_far.DrawModel();
						SetUseLighting(TRUE);
					}
					else {
						if (isSetFog) {
							l.obj.material_AlphaTestAll(true, DX_CMP_GREATER, (int)(128.f + 127.f*(Farlimit2 - LengthtoCam.Length()) / Farlimit2));
						}
						else {
							l.obj.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
						}
						l.obj.DrawModel();
					}
				}

			}
			if (isSetFog) {
				SetFogEnable(fog_enable);
				SetFogMode(fog_mode);
				SetFogColor(fog_r, fog_g, fog_b);
				SetFogStartEnd(fog_start, fog_end);
				SetFogDensity(fog_density);
			}
		}
	};
};
