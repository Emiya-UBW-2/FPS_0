#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {

	Triangle GetExternalTriangle(const VECTOR_ref& position, const VECTOR_ref& size) {
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

	bool GetSamePoint(const VECTOR_ref& LineA, const VECTOR_ref& LineB, const VECTOR_ref& pointA, const VECTOR_ref& pointB) {
		auto vec1 = (LineB - LineA).Norm();
		return vec1.cross((pointA - LineA).Norm()).dot(vec1.cross((pointB - LineA).Norm())) > 0.f;
	};

	// 一番外側の巨大三角形を生成、ここでは画面内の点限定として画面サイズを含む三角形を作る
	void CalcDelaunay(std::vector<Triangle>* Ans, const std::vector<std::unique_ptr<SideControl>>& points, const Triangle& ExternalTriangle, const std::vector<VECTOR_ref>& GonPoint2D) {
		std::vector<Triangle*> triangles;		// 見つかった三角形を保持する配列

		Triangle* super_triangle; super_triangle = new Triangle; *super_triangle = ExternalTriangle;

		triangles.emplace_back(super_triangle);		// 生成した巨大三角形をドロネー三角形郡に追加

		for (auto& p : points) {
			// 外接円に点が含まれる三角形を見つける
			std::vector<Triangle*> hit_triangles;
			for (auto& tris : triangles) {
				if (tris->getCircumscribedCircle(p->Pos2D)) {
					hit_triangles.emplace_back(tris);
				}
			}

			std::vector<LineControl> line_stack;
			for (auto& tris : hit_triangles) {
				// 見つかった三角形の辺をスタックに積む
				for (const auto& e1 : tris->Getlines()) { line_stack.emplace_back(e1); }
				auto A = tris->Getpoints()[0];
				auto B = tris->Getpoints()[1];
				auto C = tris->Getpoints()[2];
				// 見つかった三角形を配列から削除
				triangles.erase(std::find(triangles.begin(), triangles.end(), tris));

				// 見つかった三角形を該当の点で分割し、新しく3つの三角形にする
				Triangle new_tri1; new_tri1.Set(A, B, p->Pos2D);
				Triangle new_tri2; new_tri2.Set(B, C, p->Pos2D);
				Triangle new_tri3; new_tri3.Set(C, A, p->Pos2D);
				if (!new_tri1.IsOverlap()) {
					Triangle* new_triangle1; new_triangle1 = new Triangle; *new_triangle1 = new_tri1;
					triangles.emplace_back(new_triangle1);
				}
				if (!new_tri2.IsOverlap()) {
					Triangle* new_triangle2; new_triangle2 = new Triangle; *new_triangle2 = new_tri2;
					triangles.emplace_back(new_triangle2);
				}
				if (!new_tri3.IsOverlap()) {
					Triangle* new_triangle3; new_triangle3 = new Triangle; *new_triangle3 = new_tri3;
					triangles.emplace_back(new_triangle3);
				}
			}
			hit_triangles.clear();

			// スタックが空になるまで繰り返す
			for (auto& line : line_stack) {
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

				auto point_AB = line;												// あとで使うため、頂点A,Bを保持しておく
				auto Cptr = triangle_ABC->noCommonPointByline(line);			// 三角形ABCの頂点のうち、共有辺以外の点を取得（つまり点C）
				auto Dptr = triangle_ABD->noCommonPointByline(line);			// 三角形ABDの頂点のうち、共有辺以外の点を取得（つまり点D）
				VECTOR_ref point_C; if (Cptr) { point_C = *Cptr; }
				VECTOR_ref point_D; if (Dptr) { point_D = *Dptr; }

				// 頂点Dが三角形ABCの外接円に含まれてるか判定
				if (triangle_ABC->getCircumscribedCircle(point_D)) {
					// 三角形リストから三角形を削除
					triangles.erase(std::find(triangles.begin(), triangles.end(), common_line_triangles[0]));
					triangles.erase(std::find(triangles.begin(), triangles.end(), common_line_triangles[1]));

					// 共有辺をflipしてできる三角形を新しく三角形郡に追加
					Triangle tri_ACD; tri_ACD.Set(point_AB.Getstart(), point_C, point_D);
					Triangle tri_BCD; tri_BCD.Set(point_AB.Getend(), point_C, point_D);
					if (!tri_ACD.IsOverlap()) {
						Triangle* triangle_ACD; triangle_ACD = new Triangle; *triangle_ACD = tri_ACD;
						triangles.emplace_back(triangle_ACD);
					}
					if (!tri_BCD.IsOverlap()) {
						Triangle* triangle_BCD; triangle_BCD = new Triangle; *triangle_BCD = tri_BCD;
						triangles.emplace_back(triangle_BCD);
					}

					// 上記三角形の辺をline stackに追加
					auto other_line1 = triangle_ABC->otherlineByline(line);
					auto other_line2 = triangle_ABD->otherlineByline(line);
					for (const auto& e1 : other_line1) { line_stack.emplace_back(e1); }
					for (const auto& e1 : other_line2) { line_stack.emplace_back(e1); }
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
			//*
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
			//*/
			//細すぎるものを削除
			//*
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
			//*/
			//大外チェック
			/*
			if (tris) {
				bool ok = false;
				for (auto& point : tris->Getpoints()) {
					for (const auto& p : points) {
						if ((point == p->Pos2D) && (p->Type != SideType::Triangle)) {
							ok = true;
							break;
						}
					}
					if (ok) { break; }
				}
				if (!ok) {
					delete tris; tris = nullptr;
				}
			}
			//*/
			//辺が四角の点からの内向きベクトルだったら排除
			//*
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
			//*/

			//ポリゴンの被りチェック

			//1辺が共有の場合にて辺が2回交差している場合おかしい
			/*
			if (tris) {
				bool ok = true;
				int counter[3] = { 0,0,0 };
				for (int loop2 = 0; loop2 < triangles.size(); loop2++) {
					if (tris == triangles[loop2]) { continue; }
					if (triangles[loop2]) {
						int OK0[3]{ -1,-1,-1 };
						for (int Lp = 0; Lp < 3; Lp++) {
							for (int L0 = 0; L0 < 3; L0++) {
								if (OK0[0] == L0) { continue; }
								if (OK0[1] == L0) { continue; }
								if (tris->Getpoints()[Lp] == triangles[loop2]->Getpoints()[L0]) {
									OK0[Lp] = L0;
									break;
								}
							}
						}
						int count = 0;
						int ID = -1;
						int ID2 = -1;
						{
							int ID_t[3]{ 0,1,2 };
							for (int Lp = 0; Lp < 3; Lp++) {
								if (OK0[Lp] != -1) {
									ID_t[OK0[Lp]] = -1;
									count++;
								}
								else {
									ID = Lp;
								}
							}
							if (count == 2) {
								for (int L0 = 0; L0 < 3; L0++) {
									if (ID_t[L0] != -1) {
										ID2 = ID_t[L0];
										break;
									}
								}
							}
						}
						//2点が同じ
						if (count == 2) {
							for (int Lp = 1; Lp < 3; Lp++) {
								bool ishit = false;
								VECTOR_ref Pos1t = tris->Getpoints()[ID];
								VECTOR_ref Pos2t = tris->Getpoints()[(ID + Lp) % 3];
								for (int L0 = 1; L0 < 3; L0++) {
									VECTOR_ref PosAt = triangles[loop2]->Getpoints()[ID2];
									VECTOR_ref PosBt = triangles[loop2]->Getpoints()[(ID2 + L0) % 3];
									SEGMENT_SEGMENT_RESULT Res;
									if (
										GetSegmenttoSegment(Pos1t, Pos2t, PosAt, PosBt, &Res) &&
										(
										(0.01f < Res.SegA_MinDist_Pos1_Pos2_t && Res.SegA_MinDist_Pos1_Pos2_t < 0.99f) &&
											(0.01f < Res.SegB_MinDist_Pos1_Pos2_t && Res.SegB_MinDist_Pos1_Pos2_t < 0.99f)
											)
										) {
										ishit = true;
										int id = (Lp == 1) ? ID : (ID + Lp) % 3;
										counter[id]++;
										if (counter[id] == 2) {
											ok = false;
										}
										break;
									}
								}
								if (ishit) { break; }
								if (!ok) { break; }
							}
						}
						if (!ok) { break; }
					}
				}
				if (!ok) {
					delete tris; tris = nullptr;
				}
			}
			//*/

			//二点が同じで3点目が同じ向きかどうか
			/*
			if (tris) {
				bool ok = true;
				int counter = 0;
				for (int loop2 = 0; loop2 < triangles.size(); loop2++) {
					if (tris == triangles[loop2]) { continue; }
					if (triangles[loop2]) {
						int OK0[3]{ -1,-1,-1 };
						for (int Lp = 0; Lp < 3; Lp++) {
							for (int L0 = 0; L0 < 3; L0++) {
								if (OK0[0] == L0) { continue; }
								if (OK0[1] == L0) { continue; }
								if (tris->Getpoints()[Lp] == triangles[loop2]->Getpoints()[L0]) {
									OK0[Lp] = L0;
									break;
								}
							}
						}
						int count = 0;
						int ID = -1;
						int ID2 = -1;
						{
							int ID_t[3]{ 0,1,2 };
							for (int Lp = 0; Lp < 3; Lp++) {
								if (OK0[Lp] != -1) {
									ID_t[OK0[Lp]] = -1;
									count++;
								}
								else {
									ID = Lp;
								}
							}
							if (count == 2) {
								for (int L0 = 0; L0 < 3; L0++) {
									if (ID_t[L0] != -1) {
										ID2 = ID_t[L0];
										break;
									}
								}
							}
						}
						//2点が同じ
						if (count == 2) {
							auto a = tris->Getpoints()[(ID + 1) % 3];
							auto b = tris->Getpoints()[(ID + 2) % 3];
							auto c = tris->Getpoints()[ID];
							auto d = triangles[loop2]->Getpoints()[ID2];
							if (GetSamePoint(a, b, c, d)) {
								counter++;
								if (counter == 2) {
									ok = false;
									break;
								}
							}
						}
					}
				}
				if (!ok) {
					delete tris; tris = nullptr;
				}
			}
			//*/
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
};
namespace FPS_n2 {
};
