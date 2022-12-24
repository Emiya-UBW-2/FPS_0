#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	/*破壊壁*/
	void	BreakWall::WallObj::SetTri(int ID, bool isback, float offset) noexcept {
		//0を基に別オブジェを作る
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
	void	BreakWall::WallObj::SetFirst(const MV1_REF_POLYGONLIST& PolyList, const VECTOR_ref& pos, float YRad, float YScale) noexcept {
		this->m_Pos = pos;
		this->m_YScale = YScale;
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
			this->m_WallBase[i][0] = this->m_WallVertex[0][this->m_WallIndex[0][index + 0]].pos;
			this->m_WallBase[i][1] = this->m_WallVertex[0][this->m_WallIndex[0][index + 1]].pos;
			this->m_WallBase[i][2] = this->m_WallVertex[0][this->m_WallIndex[0][index + 2]].pos;
		}
		this->m_Wallnorm = this->m_WallVertex[0][0].norm;
	}
	void	BreakWall::WallObj::SetNext() noexcept {
		this->m_WallVertex[0].clear();
		this->m_WallIndex[0].clear();
		for (auto& Side : this->m_Side) {
			size_t index = &Side - &this->m_Side.front();
			//出来たものをリストに再登録
			for (auto& s : this->m_Tri2D[index]) {
				for (auto& p : s.Getpoints()) {
					VECTOR_ref p2; p2.Set(p.x(), 0.f, p.y());
					this->m_WallVertex[0].resize(this->m_WallVertex[0].size() + 1);
					this->m_WallVertex[0].back().pos = (MATRIX_ref::Vtrans(p2, Side[0]->m_Mat.Inverse()) + this->m_BasePos).get();
					this->m_WallVertex[0].back().norm = Side[0]->m_Norm.get();
					this->m_WallVertex[0].back().dif = Side[0]->m_dif;
					this->m_WallVertex[0].back().spc = Side[0]->m_spc;
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
	void	BreakWall::WallObj::SetSide(const VECTOR_ref* basepos) noexcept {
		if (basepos) { this->m_BasePos = *basepos; }
		for (auto& s : this->m_Side) {
			for (auto&s2 : s) { s2.reset(); }
			s.clear();
		}
		this->m_Side.clear();
		this->m_Side.resize(this->m_WallIndex[0].size() / Triangle_Num);
		this->m_Tri2D.resize(this->m_Side.size());
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
	bool	BreakWall::WallObj::GetSamePoint(const VECTOR_ref& LineA, const VECTOR_ref& LineB, const VECTOR_ref& pointA, const VECTOR_ref& pointB) noexcept {
		//点ABが線の同じ側にいるかどうか
		auto vec1 = (LineB - LineA).Norm();
		return vec1.cross((pointA - LineA).Norm()).dot(vec1.cross((pointB - LineA).Norm())) > 0.f;
	};
	void	BreakWall::WallObj::CalcDelaunay(std::vector<Triangle>* Ans, const std::vector<std::unique_ptr<SideControl>>& points, const Triangle& ExternalTriangle, const std::vector<VECTOR_ref>& GonPoint2D) noexcept {
		//ドロネー三角形分割を計算.一番外側の巨大三角形を生成、ここでは画面内の点限定として画面サイズを含む三角形を作る
		/*
		points 計算対象の点群
		ExternalTriangle 点を内包する三角形
		 */
		std::vector<Triangle*> triangles;		// 見つかった三角形を保持する配列
		triangles.emplace_back(new Triangle);
		*triangles.back() = ExternalTriangle;
		Triangle* super_triangle = triangles.back();
		for (auto& p : points) {
			// 外接円に点が含まれる三角形を見つける
			std::vector<Triangle*> hit_triangles;
			for (auto& tris : triangles) {
				if (tris->getCircumscribedCircle(p->m_Pos2D)) {
					hit_triangles.emplace_back(tris);
				}
			}
			std::vector<Triangle::LineControl> line_stack;
			for (auto& tris : hit_triangles) {
				// 見つかった三角形の辺をスタックに積む
				for (int i = 0; i < Triangle_Num; i++) {
					auto startp = tris->Getpoints()[i];
					auto endp = tris->Getpoints()[(i + 1) % Triangle_Num];
					line_stack.emplace_back(std::make_pair(startp, endp));
				}
				// 見つかった三角形を配列から削除
				triangles.erase(std::find(triangles.begin(), triangles.end(), tris));
				// 見つかった三角形を該当の点で分割し、新しく3つの三角形にする
				Triangle new_tri1; new_tri1.Set(line_stack[0].first, line_stack[0].second, p->m_Pos2D);
				Triangle new_tri2; new_tri2.Set(line_stack[1].first, line_stack[1].second, p->m_Pos2D);
				Triangle new_tri3; new_tri3.Set(line_stack[2].first, line_stack[2].second, p->m_Pos2D);
				if (!new_tri1.IsOverlap()) {
					triangles.emplace_back(new Triangle);
					*triangles.back() = new_tri1;
				}
				if (!new_tri2.IsOverlap()) {
					triangles.emplace_back(new Triangle);
					*triangles.back() = new_tri2;
				}
				if (!new_tri3.IsOverlap()) {
					triangles.emplace_back(new Triangle);
					*triangles.back() = new_tri3;
				}
			}
			hit_triangles.clear();
			// スタックが空になるまで繰り返す
			for (int i = 0; i < line_stack.size(); i++) {
				auto& line = line_stack[i];
				// 辺を共有する三角形を見つける
				std::vector<Triangle*> common_line_triangles;
				for (auto& tris : triangles) {
					if (tris->Hasline(line)) {
						common_line_triangles.emplace_back(tris);
					}
				}
				if (common_line_triangles.size() <= 1) { continue; }				// 共有する辺を持つ三角形がふたつ見つからなければスキップ

				// 共有辺（これを辺ABとする）を含む2個の三角形をABC, ABDとする
				// もし、三角形ABCの外接円に点Dが入る場合は、共有辺をflipし、辺AD/DB/BC/CAをスタックにemplace_backする
				// つまり、見つかった三角形をリストから削除し、新しい辺リストをスタックに積む
				// さらに、新しくできた三角形をリストに加える
				auto* triangle_ABC = common_line_triangles[0];
				auto* triangle_ABD = common_line_triangles[1];

				// 選ばれた三角形が同一のものの場合はそれを削除して次へ
				if (triangle_ABC == triangle_ABD) {
					triangles.erase(std::find(triangles.begin(), triangles.end(), triangle_ABC));
					triangles.erase(std::find(triangles.begin(), triangles.end(), triangle_ABD));
					continue;
				}

				auto Cptr = triangle_ABC->noCommonPointByline(line);			// 三角形ABCの頂点のうち、共有辺以外の点を取得（つまり点C）
				auto Dptr = triangle_ABD->noCommonPointByline(line);			// 三角形ABDの頂点のうち、共有辺以外の点を取得（つまり点D）
				VECTOR_ref point_C; if (Cptr) { point_C = *Cptr; }
				VECTOR_ref point_D; if (Dptr) { point_D = *Dptr; }

				// 頂点Dが三角形ABCの外接円に含まれてるか判定
				if (triangle_ABC->getCircumscribedCircle(point_D)) {
					// 三角形リストから三角形を削除
					triangles.erase(std::find(triangles.begin(), triangles.end(), triangle_ABC));
					triangles.erase(std::find(triangles.begin(), triangles.end(), triangle_ABD));

					// 共有辺をflipしてできる三角形を新しく三角形郡に追加
					Triangle tri_ACD; tri_ACD.Set(line.first, point_C, point_D);
					Triangle tri_BCD; tri_BCD.Set(line.second, point_C, point_D);
					if (!tri_ACD.IsOverlap()) {
						triangles.emplace_back(new Triangle);
						*triangles.back() = tri_ACD;
					}
					if (!tri_BCD.IsOverlap()) {
						triangles.emplace_back(new Triangle);
						*triangles.back() = tri_BCD;
					}

					// 上記三角形の辺をline stackに追加
					triangle_ABC->otherlineByline(line, &line_stack);
					triangle_ABD->otherlineByline(line, &line_stack);
				}
			}
			line_stack.clear();
		}

		// 最後に、巨大三角形と頂点を共有している三角形をリストから削除
		for (auto& point : super_triangle->Getpoints()) {
			for (auto& tris : triangles) {
				if (tris && tris->HasPoint(point)) {
					delete tris; tris = nullptr;
				}
			}
		}
		//
		for (auto& tris : triangles) {
			//被り排除
			if (tris) {
				for (auto& tris2 : triangles) {
					if (tris2 == tris) { continue; }
					if (tris2) {
						bool ok = false;
						int OK0[3]{ -1,-1,-1 };
						for (int Lp = 0; Lp < 3; Lp++) {
							for (int L0 = 0; L0 < 3; L0++) {
								if (OK0[0] == L0) { continue; }
								if (OK0[1] == L0) { continue; }
								if (tris->Getpoints()[Lp] == tris2->Getpoints()[L0]) {
									OK0[Lp] = L0;
									break;
								}
							}
							if (OK0[Lp] == -1) {
								ok = true;
								break;//被っていない
							}
						}
						//被りがあるのでtrisを削除
						if (!ok) {
							delete tris; tris = nullptr;
							break;
						}
					}
				}
			}
			//細すぎるものを削除
			if (tris) {
				bool ok = true;
				VECTOR_ref Pos0 = tris->Getpoints()[0];
				VECTOR_ref Pos1 = tris->Getpoints()[1];
				VECTOR_ref Pos2 = tris->Getpoints()[2];
				while (true) {
					for (int loop2 = 0; loop2 < 3; loop2++) {
						if (GetMinLenSegmentToPoint(
							tris->Getpoints()[loop2],
							tris->Getpoints()[(loop2 + 1) % 3],
							tris->Getpoints()[(loop2 + 2) % 3]) <= 0.01f) {
							delete tris; tris = nullptr;
							ok = false;
							break;
						}
					}
					if (!ok) { break; }
					break;
				}
			}
			//辺が四角の点からの内向きベクトルだったら排除
			if (tris) {
				bool ok = true;
				auto N_gon = GonPoint2D.size();
				if (N_gon >= 3) {
					for (int loop2 = 0; loop2 < 3; loop2++) {
						VECTOR_ref Pos1 = tris->Getpoints()[(loop2 + 0) % 3];
						VECTOR_ref Pos2 = tris->Getpoints()[(loop2 + 1) % 3];
						for (int loop3 = 0; loop3 < N_gon; loop3++) {
							VECTOR_ref PosZ = GonPoint2D.at(((size_t)loop3 + 0) % N_gon);
							VECTOR_ref PosA = GonPoint2D.at(((size_t)loop3 + 1) % N_gon);//基準
							VECTOR_ref PosB = GonPoint2D.at(((size_t)loop3 + 2) % N_gon);
							if (Pos1 == PosA) {
								if ((Pos2 != PosB) && (Pos2 != PosZ)) {
									if (
										((PosB - PosA).Norm().cross((Pos2 - PosA).Norm()).z() <= 0.f) &&
										((PosZ - PosA).Norm().cross((Pos2 - PosA).Norm()).z() >= 0.f)
										) {
										ok = false;
										break;
									}
								}
							}
							if (!ok) { break; }
							if (Pos2 == PosA) {
								if ((Pos1 != PosB) && (Pos1 != PosZ)) {
									if (
										((PosB - PosA).Norm().cross((Pos1 - PosA).Norm()).z() <= 0.f) &&
										((PosZ - PosA).Norm().cross((Pos1 - PosA).Norm()).z() > 0.f)
										) {
										ok = false;
										break;
									}
								}
							}
							if (!ok) { break; }
						}
						if (!ok) { break; }
					}
					if (!ok) {
						delete tris; tris = nullptr;
					}
				}
			}
		}
		//
		Ans->clear();
		for (auto& tris : triangles) {
			if (tris) {
				VECTOR_ref Pos0 = tris->Getpoints()[0];
				VECTOR_ref Pos1 = tris->Getpoints()[1];
				VECTOR_ref Pos2 = tris->Getpoints()[2];
				VECTOR_ref cross = (Pos1 - Pos0).cross(Pos2 - Pos0);
				if (cross.z() >= 0.f) {
					tris->Set(Pos0, Pos2, Pos1);
				}
				Ans->emplace_back(*tris);
				delete tris; tris = nullptr;
			}
		}
		triangles.clear();
	}
	//
	void	BreakWall::WallObj::Init(const MV1& obj, const MV1& col, const VECTOR_ref& pos, float YRad, float YScale) noexcept {
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

		MATERIALPARAM							Material_t;
		Material_t.Diffuse = GetLightDifColor();
		Material_t.Specular = GetLightSpcColor();
		Material_t.Ambient = GetLightAmbColor();
		Material_t.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
		Material_t.Power = 20.0f;
		SetMaterialParam(Material_t);
		this->m_IsDraw = true;

		this->m_PlayerCol = col.Duplicate();
		this->m_PlayerCol.SetupCollInfo(2, 2, 2);
		this->m_PlayerCol.SetMatrix(MATRIX_ref::RotY(YRad)*MATRIX_ref::Mtrans(pos));
		this->m_PlayerCol.RefreshCollInfo();
	}
	void	BreakWall::WallObj::Execute(void) noexcept {
		if (this->m_WallCalc.get() != nullptr) {
			if (this->m_isThreadEnd) {
				//m_WallCalc->join();
				this->m_WallCalc->detach();
				this->m_WallCalc.reset();
				this->m_isThreadEnd = false;
			}
		}
		if ((this->m_Bigcount > 3) || (this->m_WallIndex[0].size() > 5000)) {
			this->m_BreakTimer = std::max(this->m_BreakTimer - 1.f / FPS / 2.f, 0.f);
		}
		else {
			this->m_BreakTimer = 1.f;
		}
		//printfDx("スレッド数:%d\n", std::thread::hardware_concurrency());

		//addtri += GetMouseWheelRotVolWithCheck();
	}
	bool	BreakWall::WallObj::Draw() noexcept {

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
	bool	BreakWall::WallObj::CheckHit(const VECTOR_ref& repos, VECTOR_ref* pos, VECTOR_ref* norm, float radius) noexcept {
		VECTOR_ref pos_t = *pos;
		bool ishit = false;
		if (this->m_BreakTimer != 1.f) {
			return false;
		}
		bool isback = ((pos_t - repos).dot(this->m_Wallnorm) >= 0.f);
		VECTOR_ref vec = (isback) ? ((pos_t - repos)*-1.f) : (pos_t - repos);
		VECTOR_ref RePos = isback ? pos_t : repos;
		{
			float Radius = radius * Scale_Rate*1.8f*5.f;
			int N_gon = 5 + std::clamp((int)(radius / 0.015f), 0, 11);
			VECTOR_ref xaxis = vec.Norm().cross(VECTOR_ref::up());
			VECTOR_ref yaxis = vec.Norm().cross(xaxis);
			this->m_GonPoint[0].resize(N_gon);
			this->m_GonPoint[1].resize(N_gon);
			for (int gon = 0; gon < N_gon; gon++) {
				float rad = deg2rad(360.f * (0.5f + (float)gon) / (float)N_gon);
				this->m_GonPoint[0][gon] = RePos + (xaxis * sin(rad) + yaxis * cos(rad))*Radius;
				//平面上のくりぬきポイント取得
				VECTOR_ref TriPos0 = m_WallBase[0][0];
				VECTOR_ref TriPos1 = m_WallBase[0][1];
				VECTOR_ref TriPos2 = m_WallBase[0][2];
				VECTOR_ref TriNorm = ((TriPos1 - TriPos0).cross(TriPos2 - TriPos0)).Norm();

				VECTOR_ref PosN = GetMinPosSegmentToPoint(TriPos0, TriNorm, m_GonPoint[0][gon]);
				float pAN = std::abs((this->m_GonPoint[0][gon] - PosN).dot(TriNorm));
				float pBN = std::abs(((this->m_GonPoint[0][gon] + vec) - PosN).dot(TriNorm));
				this->m_GonPoint[1][gon] = m_GonPoint[0][gon] + vec * (pAN / (pAN + pBN));
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

								VECTOR_ref PosN = GetMinPosSegmentToPoint(TriPos0, TriNorm, RePos);
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
			if (this->m_WallCalc.get() == nullptr) {
				this->m_WallCalc = std::make_shared<std::thread>([&]() {
					this->m_isThreadEnd = false;
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
								if (Side[s]->m_Type == SideType::Triangle) {
									bool isIn = true;
									for (int gon = 0; gon < N_gon; gon++) {
										if (!GetSamePoint(GonPoint2[1][gon], GonPoint2[1][(gon + 1) % N_gon], GonPoint2[1][(gon + 2) % N_gon], Side[s]->m_Pos)) {
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
									if (Side[s]->m_SquarePoint != -1) {
										//IntoOutと番号が同じならOuttoInが優先
										data.resize(data.size() + 1);
										data.back().point = Side[s]->m_Pos2D;
										data.back().SquarePoint = Side[s]->m_SquarePoint;
										data.back().type = Side[s]->m_Type;
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
							CalcDelaunay(&this->m_Tri2D[index], Side, Triangle::GetExternalTriangle(position, Size), Point2D);
							Point2D.clear();
						}
						this->m_IsDraw = false;
						SetNext();
						SetSide();
						this->m_IsDraw = true;
					}
					this->m_isThreadEnd = true;
					if ((GetNowHiPerformanceCount() - OLDTime) > 16 * 2 * 1000) {
						this->m_Bigcount++;
					}
				});
			}
		}
		return ishit;
	}
	//
	void	BreakWall::DrawCommon(void) noexcept {
		for (auto& w : this->m_Walls) {
			w.Draw();
		}
	}
	void	BreakWall::Init(void) noexcept {
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
	void	BreakWall::Execute(void) noexcept {
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
	void	BreakWall::Dispose(void) noexcept {
		this->m_Walls.clear();
	}
};
