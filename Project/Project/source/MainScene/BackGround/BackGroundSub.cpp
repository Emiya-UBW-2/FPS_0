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

	// ��ԊO���̋���O�p�`�𐶐��A�����ł͉�ʓ��̓_����Ƃ��ĉ�ʃT�C�Y���܂ގO�p�`�����
	void CalcDelaunay(std::vector<Triangle>* Ans, const std::vector<std::unique_ptr<SideControl>>& points, const Triangle& ExternalTriangle, const std::vector<VECTOR_ref>& GonPoint2D) {
		std::vector<Triangle*> triangles;		// ���������O�p�`��ێ�����z��

		Triangle* super_triangle; super_triangle = new Triangle; *super_triangle = ExternalTriangle;

		triangles.emplace_back(super_triangle);		// ������������O�p�`���h���l�[�O�p�`�S�ɒǉ�

		for (auto& p : points) {
			// �O�ډ~�ɓ_���܂܂��O�p�`��������
			std::vector<Triangle*> hit_triangles;
			for (auto& tris : triangles) {
				if (tris->getCircumscribedCircle(p->Pos2D)) {
					hit_triangles.emplace_back(tris);
				}
			}

			std::vector<LineControl> line_stack;
			for (auto& tris : hit_triangles) {
				// ���������O�p�`�̕ӂ��X�^�b�N�ɐς�
				for (const auto& e1 : tris->Getlines()) { line_stack.emplace_back(e1); }
				auto A = tris->Getpoints()[0];
				auto B = tris->Getpoints()[1];
				auto C = tris->Getpoints()[2];
				// ���������O�p�`��z�񂩂�폜
				triangles.erase(std::find(triangles.begin(), triangles.end(), tris));

				// ���������O�p�`���Y���̓_�ŕ������A�V����3�̎O�p�`�ɂ���
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

			// �X�^�b�N����ɂȂ�܂ŌJ��Ԃ�
			for (auto& line : line_stack) {
				// �ӂ����L����O�p�`��������
				std::vector<Triangle*> common_line_triangles;
				for (auto& tris : triangles) {
					if (tris->Hasline(line)) {
						common_line_triangles.emplace_back(tris);
					}
				}
				if (common_line_triangles.size() <= 1) { continue; }				// ���L����ӂ����O�p�`���ӂ�������Ȃ���΃X�L�b�v

				// ���L�Ӂi������AB�Ƃ���j���܂�2�̎O�p�`��ABC, ABD�Ƃ���
				// �����A�O�p�`ABC�̊O�ډ~�ɓ_D������ꍇ�́A���L�ӂ�flip���A��AD/DB/BC/CA���X�^�b�N��emplace_back����
				// �܂�A���������O�p�`�����X�g����폜���A�V�����Ӄ��X�g���X�^�b�N�ɐς�
				// ����ɁA�V�����ł����O�p�`�����X�g�ɉ�����
				auto* triangle_ABC = common_line_triangles[0];
				auto* triangle_ABD = common_line_triangles[1];

				// �I�΂ꂽ�O�p�`������̂��̂̏ꍇ�͂�����폜���Ď���
				if (triangle_ABC == triangle_ABD) {
					triangles.erase(std::find(triangles.begin(), triangles.end(), triangle_ABC));
					triangles.erase(std::find(triangles.begin(), triangles.end(), triangle_ABD));
					continue;
				}

				auto point_AB = line;												// ���ƂŎg�����߁A���_A,B��ێ����Ă���
				auto Cptr = triangle_ABC->noCommonPointByline(line);			// �O�p�`ABC�̒��_�̂����A���L�ӈȊO�̓_���擾�i�܂�_C�j
				auto Dptr = triangle_ABD->noCommonPointByline(line);			// �O�p�`ABD�̒��_�̂����A���L�ӈȊO�̓_���擾�i�܂�_D�j
				VECTOR_ref point_C; if (Cptr) { point_C = *Cptr; }
				VECTOR_ref point_D; if (Dptr) { point_D = *Dptr; }

				// ���_D���O�p�`ABC�̊O�ډ~�Ɋ܂܂�Ă邩����
				if (triangle_ABC->getCircumscribedCircle(point_D)) {
					// �O�p�`���X�g����O�p�`���폜
					triangles.erase(std::find(triangles.begin(), triangles.end(), common_line_triangles[0]));
					triangles.erase(std::find(triangles.begin(), triangles.end(), common_line_triangles[1]));

					// ���L�ӂ�flip���Ăł���O�p�`��V�����O�p�`�S�ɒǉ�
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

					// ��L�O�p�`�̕ӂ�line stack�ɒǉ�
					auto other_line1 = triangle_ABC->otherlineByline(line);
					auto other_line2 = triangle_ABD->otherlineByline(line);
					for (const auto& e1 : other_line1) { line_stack.emplace_back(e1); }
					for (const auto& e1 : other_line2) { line_stack.emplace_back(e1); }
				}
			}
			line_stack.clear();
		}

		// �Ō�ɁA����O�p�`�ƒ��_�����L���Ă���O�p�`�����X�g����폜
		for (auto& point : super_triangle->Getpoints()) {
			for (auto& tris : triangles) {
				if (tris && tris->HasPoint(point)) {
					delete tris; tris = nullptr;
				}
			}
		}
		//
		for (auto& tris : triangles) {
			//���r��
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
								break;//����Ă��Ȃ�
							}
						}
						//��肪����̂�tris���폜
						if (!ok) {
							delete tris; tris = nullptr;
							break;
						}
					}
				}
			}
			//*/
			//�ׂ�������̂��폜
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
			//��O�`�F�b�N
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
			//�ӂ��l�p�̓_����̓������x�N�g����������r��
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
							VECTOR_ref PosA = GonPoint2D.at(((size_t)loop3 + 1) % N_gon);//�
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

			//�|���S���̔��`�F�b�N

			//1�ӂ����L�̏ꍇ�ɂĕӂ�2��������Ă���ꍇ��������
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
						//2�_������
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

			//��_��������3�_�ڂ������������ǂ���
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
						//2�_������
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
