#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClass {
		private:
			MV1							m_ObjSky;
			MV1							m_ObjGround;
			MV1							m_ObjGroundCol;
			MV1							m_ObjGroundCol_Box2D;

			MV1							m_ObjBuildBase;
			MV1							m_ColBuildBase;
			MV1							m_ColBox2DBuildBase;

			std::vector<Builds>			m_ObjBuilds;

			int softimage;

			Grass						m_grass;
			TreeControl					m_Tree;
			Box2DWall					m_Box2DWall;
			//BreakWall					m_BreakWall;
		public://getter
			const auto&		GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }

			const auto&		GetBuildCol(void) noexcept { return this->m_ObjBuilds; }

			const std::shared_ptr<b2World>&	GetBox2Dworld(void) noexcept {
				return this->m_Box2DWall.GetBox2Dworld();
				return nullptr;
			}

			const auto*		GetWallGroundCol(int id) noexcept {
				//return &this->m_BreakWall.GetWallGroundCol(id);
				id = 0;
				return (MV1*)nullptr;
			}
			const auto		GetWallGroundColNum() const noexcept {
				//return this->m_BreakWall.GetWallGroundColNum();
				return 0;
			}
			const auto		GetWallCol(const VECTOR_ref&, VECTOR_ref*, VECTOR_ref*, float) noexcept {
				return false;
			}
			/*
			const auto		GetWallCol(const VECTOR_ref& repos, VECTOR_ref* pos, VECTOR_ref* norm, float radius) noexcept {
				return this->m_BreakWall.GetWallCol(repos, pos, norm, radius);
			}
			//*/
			void			CheckTreetoSquare(
				const VECTOR_ref& cornerLF, const VECTOR_ref& cornerRF, const VECTOR_ref& cornerRR, const VECTOR_ref& cornerLR
				, const VECTOR_ref& center, float speed
			) {
				m_Tree.CheckTreetoSquare(cornerLF, cornerRF, cornerRR, cornerLR, center, speed);
			}
		private:
			void			DrawCommon(bool ischeckDraw) noexcept {
				this->m_ObjGround.DrawModel();
				for (auto& b : m_ObjBuilds) {
					b.Draw(ischeckDraw);
				}
			}
		public://
			void			Load(void) noexcept {
				MV1::Load("data/model/map_old/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map_old/col.mv1", &this->m_ObjGroundCol);
				MV1::Load("data/model/map_old/col_box2D.mv1", &this->m_ObjGroundCol_Box2D);

				MV1::Load("data/model/build/model.mv1", &this->m_ObjBuildBase);
				MV1::Load("data/model/build/col.mv1", &this->m_ColBuildBase);
				MV1::Load("data/model/build/colBox2D.mv1", &this->m_ColBox2DBuildBase);

				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);

				m_Tree.Load();
			}
			//
			void			Init(void) noexcept {
				//地形
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);


				std::vector<Builds*> CrossPoints;
				auto FindCross = [&](float length) {
					int total = 0;
					for (auto& bu : m_ObjBuilds) {
						if (bu.GetFrameSel() == 0) {
							bool Hit = (std::find_if(CrossPoints.begin(), CrossPoints.end(), [&](Builds* tmp) { return tmp == &bu; }) != CrossPoints.end());
							if (!Hit) {
								total++;
							}
						}
					}

					float rad;
					VECTOR_ref BasePos;
					while (true) {
						int rand = GetRand(total - 1) + 1;
						int rbuf = rand;
						Builds* Base{ nullptr };
						for (auto& bu : m_ObjBuilds) {
							if (bu.GetFrameSel() == 0) {
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

						rad = std::atan2f(Base->GetPosition().zvec().x(), Base->GetPosition().zvec().z()) + deg2rad(90.f)*(GetRand(2) > 1 ? 1.f : -1.f);
						BasePos = Base->GetPosition().pos();
						BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(8.f*Scale_Rate);

						if (BasePos.Length() > 200.f*Scale_Rate) {
							continue;
						}

						bool isnear = false;
						for (auto& bu : CrossPoints) {
							auto P2 = (bu->GetPosition().pos() - BasePos).Length();
							if (P2 < length) {
								isnear = true;
								break;
							}
						}
						if (!isnear) {
							CrossPoints.emplace_back(Base);
							break;
						}
					}
					return std::forward_as_tuple(BasePos, rad);
				};
				auto FindLoad = [&]() {
					int total = 0;
					for (auto& bu : m_ObjBuilds) { if (bu.GetFrameSel() == 0) { total++; } }

					float rad;
					VECTOR_ref BasePos;
					while (true) {
						int rand = GetRand(total - 1) + 1;
						int rbuf = rand;
						Builds* Base{ nullptr };
						for (auto& bu : m_ObjBuilds) {
							if (bu.GetFrameSel() == 0) {
								rbuf--;
								if (rbuf == 0) {
									Base = &bu;
									break;
								}
							}
						}

						rad = std::atan2f(Base->GetPosition().zvec().x(), Base->GetPosition().zvec().z()) + deg2rad(90.f)*(GetRand(2) > 1 ? 1.f : -1.f);
						BasePos = Base->GetPosition().pos();
						BasePos += VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(8.f*Scale_Rate);

						if (BasePos.Length() > 200.f*Scale_Rate) {
							continue;
						}

						bool isnear = false;
						for (auto& bu : m_ObjBuilds) {
							if (Base == &bu) { continue; }
							if (bu.GetFrameSel() >= 5) {
								auto P2 = (bu.GetPosition().pos() - BasePos).Length();
								if (P2 < 14.f*Scale_Rate) {
									isnear = true;
									break;
								}
							}
						}
						if (!isnear) {
							for (auto& bu : m_ObjBuilds) {
								if (Base == &bu) { continue; }
								if (bu.GetFrameSel() == 0) {
									auto P2 = (bu.GetPosition().pos() - BasePos).Length();
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
								if (bu.GetFrameSel() == 3) {
									auto P2 = (bu.GetPosition().pos() - BasePos).Length();
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

				m_ObjBuilds.resize((4 * 30 + 1) + (15 * 4) * 5 + 15 * (14 - 5 - 1) + 5);
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
							m_ObjBuilds[i].Set(this->m_ObjBuildBase, this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
							m_ObjBuilds[i].SetPosition(this->m_ObjGroundCol, BasePos, rad, true);

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
							m_ObjBuilds[i].Set(this->m_ObjBuildBase, this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
							m_ObjBuilds[i].SetPosition(this->m_ObjGroundCol, BasePos, rad, true);

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
							m_ObjBuilds[i].Set(this->m_ObjBuildBase, this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
							m_ObjBuilds[i].SetPosition(this->m_ObjGroundCol, BasePos, rad, true);

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
							m_ObjBuilds[i].Set(this->m_ObjBuildBase, this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
							m_ObjBuilds[i].SetPosition(this->m_ObjGroundCol, BasePos, rad, true);

							rad += deg2rad(GetRandf(5.f));
							BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(7.f*Scale_Rate);
						}
						mini += adds;
					}
					rad = radBase + deg2rad(-90.f);
					{
						BasePos.Set(0.f, 0.f, 0.f);
						int i = mini;
						m_ObjBuilds[i].Set(this->m_ObjBuildBase, this->m_ColBuildBase, this->m_ColBox2DBuildBase, 3);
						m_ObjBuilds[i].SetPosition(this->m_ObjGroundCol, BasePos, rad, true);
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
										if (bu.GetFrameSel() == 0) {
											auto P2 = (bu.GetPosition().pos() - BasePos).Length();
											if (P2 < 4.f*Scale_Rate) {
												iscross = true;
												break;
											}
										}
									}
									if (!iscross) {
										m_ObjBuilds[j].Set(this->m_ObjBuildBase, this->m_ColBuildBase, this->m_ColBox2DBuildBase, 0);
										m_ObjBuilds[j].SetPosition(this->m_ObjGroundCol, BasePos, rad, true);
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
							m_ObjBuilds[i].Set(this->m_ObjBuildBase, this->m_ColBuildBase, this->m_ColBox2DBuildBase, 5 + bu);
							m_ObjBuilds[i].SetPosition(this->m_ObjGroundCol, BasePos, rad, false);
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
							m_ObjBuilds[i].Set(this->m_ObjBuildBase, this->m_ColBuildBase, this->m_ColBox2DBuildBase, 5 + 2);
							m_ObjBuilds[i].SetPosition(this->m_ObjGroundCol, BasePos, rad, false);
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
							m_ObjBuilds[i].Set(this->m_ObjBuildBase, this->m_ColBuildBase, this->m_ColBox2DBuildBase, 5 + bu);
							m_ObjBuilds[i].SetPosition(this->m_ObjGroundCol, BasePos, rad, false);
						}
						mini += adds;
					}
				}

				CrossPoints.clear();
				//空
				MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));

				//Box2D壁
				this->m_Box2DWall.Add(MV1GetReferenceMesh(this->m_ObjGroundCol_Box2D.get(), 0, FALSE));
				MV1TerminateReferenceMesh(this->m_ObjGroundCol_Box2D.get(), 0, FALSE);
				for (auto& bu : m_ObjBuilds) {
					if (bu.GetFrameSel() >= 5) {
						this->m_Box2DWall.Add(MV1GetReferenceMesh(bu.GetColBox2D().get(), bu.GetFrameSel(), TRUE));
						MV1TerminateReferenceMesh(bu.GetColBox2D().get(), bu.GetFrameSel(), TRUE);
					}
				}
				this->m_Box2DWall.Init();
				//木
				m_Tree.Init(&this->m_ObjGroundCol, m_ObjBuilds);
				//草
				{
					float MAPX = 300.f*Scale_Rate;
					float MAPZ = 300.f*Scale_Rate;
					float SIZX = 10.f*Scale_Rate;
					float SIZZ = 10.f*Scale_Rate;
					int x, y;
					GraphHandle BaseGrass = GraphHandle::Load("data/grass.png");
					BaseGrass.GetSize(&x, &y);
					softimage = MakeSoftImage(x, y);
					GraphHandle BlackScreen = GraphHandle::Make((int)((float)x*SIZX / MAPX), (int)((float)y*SIZZ / MAPZ));
					BlackScreen.SetDraw_Screen(false);
					{
						DrawBox(0, 0, (int)((float)x*SIZX / MAPX), (int)((float)y*SIZZ / MAPZ), GetColor(0, 0, 0), TRUE);
					}
					GraphHandle BaseScreen = GraphHandle::Make(x, y);
					BaseScreen.SetDraw_Screen(false);
					{
						BaseGrass.DrawGraph(0, 0, false);
						for (auto& b : m_ObjBuilds) {
							if (b.GetFrameSel() >= 0) {
								auto pos = b.GetPosition().pos();
								BlackScreen.DrawRotaGraph(
									(int)((float)x*(pos.x() + MAPX / 2.f) / MAPX),
									(int)((float)y*(pos.z() + MAPZ / 2.f) / MAPZ),
									1.f,
									std::atan2f(b.GetPosition().zvec().x(), b.GetPosition().zvec().z()),
									false
								);
							}
						}
						GetDrawScreenSoftImage(0, 0, x, y, softimage);
					}
					this->m_grass.Init(&this->m_ObjGroundCol, softimage);
					//DeleteSoftImage(softimage);
				}
				//壁
				//this->m_BreakWall.Init();
			}
			//
			void			FirstExecute(void) noexcept {
				this->m_Box2DWall.GetBox2Dworld()->Step(1.f, 1, 1);//物理更新
			}
			//
			void			Execute(void) noexcept {
				//this->m_BreakWall.Execute();
				m_Tree.Execute();
			}
			//
			void			BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				this->m_ObjSky.DrawModel();
				SetUseLighting(TRUE);
			}
			void			Shadow_Draw_Far(void) noexcept {
				DrawCommon(false);
			}
			void			Shadow_Draw_NearFar(void) noexcept {
				m_Tree.Draw(false);
			}
			void			Shadow_Draw(void) noexcept {
			}
			void			Draw(void) noexcept {
				DrawCommon(true);
				m_Tree.Draw(true);
				this->m_grass.Draw();

				//DrawSoftImage(0,0,softimage);
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				this->m_ObjGround.Dispose();
				this->m_ObjBuildBase.Dispose();
				this->m_ObjGroundCol.Dispose();
				this->m_grass.Dispose();
				this->m_Box2DWall.Dispose();
				//this->m_BreakWall.Dispose();
				m_Tree.Dispose();
			}
		};
	};
};
