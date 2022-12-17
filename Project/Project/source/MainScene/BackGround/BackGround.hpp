#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClass {
		private:
			MV1							m_ObjSky;
			MV1							m_ObjGround;
			MV1							m_ObjGroundCol;

			MV1							m_ObjBuildBase;
			class Builds {
				int						m_frame{ -1 };
				MV1						m_Obj;
				float					m_rad{ 0.f };
				VECTOR_ref				m_pos;
			public:
				const auto		GetPosition() { return MATRIX_ref::RotY(m_rad) * MATRIX_ref::Mtrans(m_pos); }
				const auto&		GetFrameSel() { return m_frame; }
			public:
				void		Set(const MV1& baseModel, int frame) {
					m_Obj = baseModel.Duplicate();
					m_frame = frame;
				}
				void		SetPosition(const MV1& colModel,const VECTOR_ref& pos,float rad, bool isTilt) {
					VECTOR_ref pos_t = pos;
					MATRIX_ref mat_t;
					auto res = colModel.CollCheck_Line(pos_t + VECTOR_ref::vget(0.f, 10.f*Scale_Rate, 0.f), pos_t + VECTOR_ref::vget(0.f, -10.f*Scale_Rate, 0.f));
					if (res.HitFlag == TRUE) {
						pos_t = res.HitPosition;

						pos_t += VECTOR_ref::up()*(0.1f*Scale_Rate);
						if (isTilt) {
							mat_t = MATRIX_ref::RotVec2(VECTOR_ref::up(), res.Normal);
						}
					}
					m_rad = rad;
					m_pos = pos_t;
					m_Obj.SetMatrix(MATRIX_ref::RotY(rad)*mat_t*MATRIX_ref::Mtrans(pos_t));
				}
				void		Draw() {
					if (m_frame >= 0) {
						m_Obj.DrawFrame(m_frame);
					}
				}
			};
			std::vector< Builds>		m_ObjBuilds;

			int softimage;

			Grass						m_grass;
			Box2DWall					m_Box2DWall;
			//BreakWall					m_BreakWall;
		public://getter
			const auto&		GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }

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
		private:
			void			DrawCommon() noexcept {
#ifdef DEBUG
				//auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
				//DebugParts->SetPoint();
#endif // DEBUG

				//SetFogEnable(TRUE);
				//SetFogColor(0, 0, 0);
				//SetFogDensity(0.5f);

				this->m_ObjGround.DrawModel();
				for (auto& b : m_ObjBuilds) {
					b.Draw();
				}
				//this->m_BreakWall.DrawCommon();

				//SetUseBackCulling(TRUE);
				//SetFogEnable(FALSE);

#ifdef DEBUG
				//DebugParts->SetPoint();
#endif // DEBUG
			}
		public://
			//
			void			Init(void) noexcept {
				//地形
				MV1::Load("data/model/map_old/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map_old/col.mv1", &this->m_ObjGroundCol);
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);

				MV1::Load("data/model/build/model.mv1", &this->m_ObjBuildBase);
				
				auto FindLoad = [&](int sel) {
					int total = 0;
					for (auto& bu : m_ObjBuilds) { if (bu.GetFrameSel() == sel) { total++; } }

					float rad;
					VECTOR_ref BasePos;
					while (true) {
						int rand = GetRand(total - 1) + 1;
						int rbuf = rand;
						Builds* Base{ nullptr };
						for (auto& bu : m_ObjBuilds) {
							if (bu.GetFrameSel() == sel) {
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
							if (bu.GetFrameSel() > sel) {
								auto P2 = (bu.GetPosition().pos() - BasePos).Length();
								if (P2 < 14.f*Scale_Rate) {
									isnear = true;
									break;
								}
							}
						}
						if (!isnear) {
							break;
						}
					}
					return std::forward_as_tuple(BasePos, rad);
				};

				m_ObjBuilds.resize((4 * 30 + 1) + (3 * (14 - 5) + 30 * 5) * 5);
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
							m_ObjBuilds[i].Set(this->m_ObjBuildBase, 0);
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
							m_ObjBuilds[i].Set(this->m_ObjBuildBase, 0);
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
							m_ObjBuilds[i].Set(this->m_ObjBuildBase, 0);
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
							m_ObjBuilds[i].Set(this->m_ObjBuildBase, 0);
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
						m_ObjBuilds[i].Set(this->m_ObjBuildBase, 3);
						m_ObjBuilds[i].SetPosition(this->m_ObjGroundCol, BasePos, rad, true);
						mini += 1;
					}
				}
				for (int Z = 0; Z < 5; Z++) {
					{
						VECTOR_ref BasePos;
						float rad = 0.f;
						int adds = 3;
						for (int bu = 0; bu < 14 - 5; bu++) {
							for (int i = mini; i < mini + adds; i++) {
								std::tie(BasePos, rad) = FindLoad(0);
								m_ObjBuilds[i].Set(this->m_ObjBuildBase, 5 + bu);
								m_ObjBuilds[i].SetPosition(this->m_ObjGroundCol, BasePos, rad, false);
							}
							mini += adds;
						}
					}
					{
						float rad = 0;
						VECTOR_ref BasePos;
						int adds = 30;
						for (int i = 0; i < 5; i++) {
							std::tie(BasePos, rad) = FindLoad(0);
							BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(15.f*Scale_Rate);
							{
								for (int j = mini; j < mini + adds; j++) {
									m_ObjBuilds[j].Set(this->m_ObjBuildBase, 0);
									m_ObjBuilds[j].SetPosition(this->m_ObjGroundCol, BasePos, rad, true);

									rad += deg2rad(GetRandf(5.f));
									BasePos -= VECTOR_ref::vget(sin(rad), 0.f, cos(rad))*(7.f*Scale_Rate);
								}
							}
							mini += adds;
						}
					}
				}
				//空
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
				MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));

				//Box2D壁
				this->m_Box2DWall.Init();
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
					GraphHandle BaseScreen = GraphHandle::Make(x,y);
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
			}
			//
			void			BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				this->m_ObjSky.DrawModel();
				SetUseLighting(TRUE);
			}
			void			Shadow_Draw_NearFar(void) noexcept {
				DrawCommon();
			}
			void			Shadow_Draw(void) noexcept {
				//DrawCommon();
			}
			void			Draw(void) noexcept {
				DrawCommon();
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
			}
		};
	};
};
