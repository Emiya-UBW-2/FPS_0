#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	//kusa
	class Grass {
		class grass_t {
		public:
			bool			canlook = true;
			Model_Instance	m_Inst;
		public:
			void Init(int total) {
				this->m_Inst.Init("data/model/grass/grass.png", "data/model/grass/model.mv1", -1);
				this->m_Inst.Reset();
				this->m_Inst.Set_start(total);
			}
			void Set_one(const MATRIX_ref& mat) {
				this->m_Inst.Set_one(mat);
			}
			void put(void) noexcept {
				canlook = true;
				this->m_Inst.Execute();
			}
			void Dispose(void) noexcept {
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
			void Draw(void) noexcept {
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
		void Dispose(void) noexcept {
			for (int ID = 0; ID < grassDiv; ID++) {
				if (grasss != 0) {
					grass__[ID].Dispose();
				}
			}
		}
		void Draw(void) noexcept {
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
		auto&			GetBox2Dworld(void) noexcept { return this->m_b2world; }
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
		void			Init(void) noexcept {
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
		void			Dispose(void) noexcept {
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
		static const int Triangle_Num = 3;
		class Triangle {
		public:
			typedef std::pair<VECTOR_ref, VECTOR_ref> LineControl;
		private:
			std::array<VECTOR_ref, Triangle_Num> m_points;
		public:
			void Set(const VECTOR_ref& point0, const VECTOR_ref& point1, const VECTOR_ref& point2) {
				this->m_points[0] = point0;
				this->m_points[1] = point1;
				this->m_points[2] = point2;
			}
			const auto& Getpoints() const noexcept { return this->m_points; }
		public:
			// 与えられた辺の端を含まない点を返す
			const VECTOR_ref* noCommonPointByline(const LineControl& line) const noexcept {
				for (auto& p : this->m_points) {
					if ((line.first != p) && (line.second != p)) {
						return &p;
					}
				}
				return nullptr;
			}

			// 与えられた辺以外の辺を返す
			void otherlineByline(const LineControl& line, std::vector<LineControl>* result) const noexcept {
				for (int i = 0; i < Triangle_Num; i++) {
					auto startp = this->m_points[i];
					auto endp = this->m_points[(i + 1) % Triangle_Num];
					if (!(((startp == line.first) && (endp == line.second)) || ((startp == line.second) && (endp == line.first)))) {
						result->emplace_back(std::make_pair(startp, endp));
					}
				}
			}

			// 与えられた辺を含んでいるかチェック
			bool Hasline(const LineControl& line) const noexcept {
				for (int i = 0; i < Triangle_Num; i++) {
					auto startp = this->m_points[i];
					auto endp = this->m_points[(i + 1) % Triangle_Num];
					if (((startp == line.first) && (endp == line.second)) || ((startp == line.second) && (endp == line.first))) {
						return true;
					}
				}
				return false;
			}

			// 与えられた点の頂点があるか確認
			bool HasPoint(const VECTOR_ref& point) const noexcept {
				for (auto& p : this->m_points) {
					if (p == point) {
						return true;
					}
				}
				return false;
			}

			bool operator==(const Triangle& triangle) const noexcept {
				for (auto& p : triangle.m_points) {
					if (!HasPoint(p)) {
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

			static const Triangle GetExternalTriangle(const VECTOR_ref& position, const VECTOR_ref& size) {
				VECTOR_ref center; center.Set(position.x() + (size.x() / 2.f), position.y() + (size.y() / 2.f), 0.f);
				auto r = size.Length() / 2.f;
				auto _2r = 2.f * r;
				auto _r3r = std::sqrtf(3.f) * r;

				std::array<VECTOR_ref, 3> tri;
				tri[0].Set((center.x() - _r3r), (center.y() - r), 0.f);
				tri[1].Set((center.x() + _r3r), (center.y() - r), 0.f);
				tri[2].Set(center.x(), (center.y() + _2r), 0.f);
				Triangle ans; ans.Set(tri[0], tri[1], tri[2]);
				return ans;
			}
		};
		class WallObj {
			enum class SideType : char {
				None,
				Triangle,
				Square,
				Mix_OuttoIn,
				Mix_IntoOut,
			};
			struct SideControl {
				//private:
				SideType					m_Type{ SideType::None };
				int							m_SquarePoint{ -1 };			//触れている四角の辺
				VECTOR_ref					m_Pos;
				VECTOR_ref					m_Pos2D;
				MATRIX_ref					m_Mat;
				VECTOR_ref					m_Norm;							// 法線
				COLOR_U8					m_dif{ 0 };						// ディフューズカラー
				COLOR_U8					m_spc{ 0 };						// スペキュラカラー
			public:
				void Set(SideType type, const VECTOR_ref& pos, const VECTOR_ref& basepos, const VECTOR_ref& normal, COLOR_U8 difcolor, COLOR_U8 spccolor, int squarepoint = -1) {
					this->m_Type = type;
					this->m_SquarePoint = squarepoint;//触れている四角の辺
					this->m_Pos = pos;

					VECTOR_ref Zvec = VECTOR_ref::up();// (Side[1]->Pos - this->m_BasePos).Norm();
					this->m_Mat = MATRIX_ref::Axis1_YZ(normal, Zvec).Inverse();

					this->m_Pos2D = MATRIX_ref::Vtrans((this->m_Pos - basepos), m_Mat);
					this->m_Pos2D.Set(this->m_Pos2D.x(), m_Pos2D.z(), 0.f);

					this->m_Norm = normal;
					this->m_dif = difcolor;
					this->m_spc = spccolor;
				}
			};
		public:
			MV1										m_PlayerCol;
		private:
			float									m_BreakTimer{ -1 };
			bool									m_IsDraw{ true };
			std::array<std::vector<VERTEX3D>, 4>	m_WallVertex;		//壁をセット
			std::array<std::vector<WORD>, 4>		m_WallIndex;		//壁をセット
			std::array<std::array<VECTOR_ref, 3>, 2>m_WallBase;
			VECTOR_ref								m_Wallnorm;
			GraphHandle								m_TexHandle1;
			GraphHandle								m_TexHandle2;
			std::vector<std::vector<std::unique_ptr<SideControl>>>	m_Side;
			std::vector<std::vector<Triangle>>		m_Tri2D;
			VECTOR_ref								m_BasePos;
			VECTOR_ref								m_Pos;
			std::shared_ptr<std::thread>			m_WallCalc;
			bool									m_isThreadEnd{ false };
			std::array<std::vector<VECTOR_ref>, 2>	m_GonPoint;//辺の数
			int										m_Bigcount{ 0 };
			float									m_YScale{ 1.f };
		public://getter
			bool			GetIsBreak() const noexcept { return this->m_BreakTimer == 0.f; }
		private:
			//0を基に別オブジェを作る
			void			SetTri(int ID, bool isback, float offset) noexcept;
			void			SetFirst(const MV1_REF_POLYGONLIST& PolyList, const VECTOR_ref& pos, float YRad, float YScale) noexcept;
			void			SetNext() noexcept;
			void			SetSide(const VECTOR_ref* basepos = nullptr) noexcept;
			bool			GetSamePoint(const VECTOR_ref& LineA, const VECTOR_ref& LineB, const VECTOR_ref& pointA, const VECTOR_ref& pointB) noexcept;
			void			CalcDelaunay(std::vector<Triangle>* Ans, const std::vector<std::unique_ptr<SideControl>>& points, const Triangle& ExternalTriangle, const std::vector<VECTOR_ref>& GonPoint2D) noexcept;
		public:
			void			Init(const MV1& obj, const MV1& col, const VECTOR_ref& pos, float YRad, float YScale) noexcept;
			void			Execute(void) noexcept;
			bool			Draw() noexcept;
			bool			CheckHit(const VECTOR_ref& repos, VECTOR_ref* pos, VECTOR_ref* norm, float radius) noexcept;
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
		void			DrawCommon(void) noexcept;
	public:
		void			Init(void) noexcept;
		void			Execute(void) noexcept;
		void			Dispose(void) noexcept;
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
			this->m_mesh = frame;
			this->m_Col.SetupCollInfo(1, 1, 1, m_mesh);
		}
		void		ChangeSel(int frame) {
			if (this->m_Col.IsActive()) {
				MV1TerminateCollInfo(this->m_Col.get(), m_mesh);
				this->m_mesh = frame;
				this->m_Col.SetupCollInfo(1, 1, 1, m_mesh);
			}
		}
		void		SetPosition(const MV1& colModel, VECTOR_ref pos, float rad, bool isTilt) {
			this->m_mat.clear();
			auto res = colModel.CollCheck_Line(pos + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), pos + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
			if (res.HitFlag == TRUE) {
				pos = res.HitPosition;
				pos += VECTOR_ref::up()*(0.1f*Scale_Rate);
				if (isTilt) {
					this->m_mat = MATRIX_ref::RotVec2(VECTOR_ref::up(), res.Normal);
				}
			}
			this->m_mat = MATRIX_ref::RotY(rad)*m_mat*MATRIX_ref::Mtrans(pos);
			this->m_Col.SetMatrix(this->m_mat);
			this->m_Col.RefreshCollInfo(this->m_mesh);
			this->m_ColBox2D.SetMatrix(this->m_mat);
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

			this->m_ObjBuilds.resize((4 * 30 + 1) + (15 * 4) * 5 + 15 * (14 - 5 - 1) + 5);
			this->m_Inst.resize(15);
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
						Builds* Base = nullptr;
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
						if (Base == nullptr) { continue; }

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
						if (!isnear || count > 100) {
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
						Builds* Base = nullptr;
						for (auto& bu : m_ObjBuilds) {
							if (bu.GetMeshSel() == 0) {
								rbuf--;
								if (rbuf == 0) {
									Base = &bu;
									break;
								}
							}
						}
						if (Base == nullptr) { continue; }

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
							this->m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
							this->m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, true);

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
							this->m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
							this->m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, true);

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
							this->m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
							this->m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, true);

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
							this->m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
							this->m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, true);

							rad += deg2rad(GetRandf(5.f));
							BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(7.f*Scale_Rate);
						}
						mini += adds;
					}
					rad = radBase + deg2rad(-90.f);
					{
						BasePos.Set(0.f, 0.f, 0.f);
						int i = mini;
						this->m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 3);
						this->m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, true);
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
										this->m_ObjBuilds[j].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
										this->m_ObjBuilds[j].SetPosition(*MapCol, BasePos, rad, true);
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
							this->m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 5 + bu);
							this->m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, false);
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
							this->m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 5 + 2);
							this->m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, false);
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
							this->m_ObjBuilds[i].Set(this->m_ColBuildBase, this->m_ColBox2DBuildBase, 5 + bu);
							this->m_ObjBuilds[i].SetPosition(*MapCol, BasePos, rad, false);
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

			MATERIALPARAM							Material_t;
			Material_t.Diffuse = GetLightDifColor();
			Material_t.Specular = GetLightSpcColor();
			Material_t.Ambient = GetLightAmbColor();
			Material_t.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
			Material_t.Power = 20.0f;
			SetMaterialParam(Material_t);
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
