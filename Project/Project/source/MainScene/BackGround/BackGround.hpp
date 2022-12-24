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

			std::vector<VECTOR_ref>		m_WayPoint;
			std::vector<MV1*>			m_MapCols;

			int							m_softimage{ -1 };

			BuildControl				m_BuildControl;
			Grass						m_grass;
			TreeControl					m_Tree;
			Box2DWall					m_Box2DWall;
			//BreakWall					m_BreakWall;
		public://getter
			const auto&		GetGroundCols(void) noexcept { return this->m_MapCols; }
			//const auto&		GetBuildCol(void) noexcept { return this->m_BuildControl.GetBuildCol(); }
			const auto&		GetWayPoint(void) noexcept { return this->m_WayPoint; }
			const std::shared_ptr<b2World>&	GetBox2Dworld(void) noexcept {
				return this->m_Box2DWall.GetBox2Dworld();
				return nullptr;
			}

			const auto		GetWallCol(const VECTOR_ref&, VECTOR_ref*, VECTOR_ref*, float) noexcept {//const VECTOR_ref& repos, VECTOR_ref* pos, VECTOR_ref* norm, float radius
				//return this->m_BreakWall.GetWallCol(repos, pos, norm, radius);
				return false;
			}
			void			CheckTreetoSquare(const VECTOR_ref& cornerLF, const VECTOR_ref& cornerRF, const VECTOR_ref& cornerRR, const VECTOR_ref& cornerLR, const VECTOR_ref& center, float speed) {
				this->m_Tree.CheckTreetoSquare(cornerLF, cornerRF, cornerRR, cornerLR, center, speed);
			}
			const auto		CheckLinetoMap(const VECTOR_ref& StartPos, VECTOR_ref* EndPos, bool isNearest, bool isOnlyGround, VECTOR_ref* Normal = nullptr) {
				bool isHit = false;
				{
					auto col_p = this->m_ObjGroundCol.CollCheck_Line(StartPos, *EndPos);
					if (col_p.HitFlag == TRUE) {
						isHit = true;
						if (isNearest) {
							*EndPos = col_p.HitPosition;
							if (Normal) { *Normal = col_p.Normal; }
						}
						else {
							return isHit;
						}
					}
				}
				if (isOnlyGround) {
					return isHit;
				}
				for (auto& bu : this->m_BuildControl.GetBuildCol()) {
					if (bu.GetMeshSel() < 0) { continue; }
					if (GetMinLenSegmentToPoint(StartPos, *EndPos, bu.GetMatrix().pos()) >= 20.f*Scale_Rate) { continue; }
					auto col_p = bu.GetCol(StartPos, *EndPos);
					if (col_p.HitFlag == TRUE) {
						isHit = true;
						if (isNearest) {
							*EndPos = col_p.HitPosition;
							if (Normal) { *Normal = col_p.Normal; }
						}
						else {
							return isHit;
						}
					}
				}
				return isHit;
			}
			const auto		CheckCapsuletoMap(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos, float radius) {
				for (const auto& c : m_MapCols) {
					if (c->CollCheck_Capsule(StartPos, EndPos, radius).HitNum > 0) {
						return true;
					}
				}
				return false;
			}

			const auto		GetRoadPointNum() const noexcept {
				int count = 0;
				for (const auto& bu : this->m_BuildControl.GetBuildCol()) {
					if (bu.GetMeshSel() == 0) {
						count++;
					}
				}
				return count;
			}
			const MATRIX_ref*GetRoadPoint(int ID) const noexcept {
				int count = 0;
				for (const auto& bu : this->m_BuildControl.GetBuildCol()) {
					if (bu.GetMeshSel() == 0) {
						if (ID == count) {
							return &bu.GetMatrix();
						}
						count++;
					}
				}
				return nullptr;
			}
		public://
			void			Load(void) noexcept {
				MV1::Load("data/model/map_old/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map_old/col.mv1", &this->m_ObjGroundCol);
				MV1::Load("data/model/map_old/col_box2D.mv1", &this->m_ObjGroundCol_Box2D);

				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);

				this->m_BuildControl.Load();
				this->m_Tree.Load();
			}
			//
			void			Init(void) noexcept {
				//地形
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);
				this->m_BuildControl.Init(&this->m_ObjGroundCol);
				for (auto& bu : this->m_BuildControl.GetBuildCol()) {
					if (bu.GetMeshSel() == 3 || bu.GetMeshSel() == 0) {
						VECTOR_ref pos_t = bu.GetMatrix().pos();
						float offset = 5.f*Scale_Rate;
						if (
							(-300.f*Scale_Rate / 2.f + offset < pos_t.x() && pos_t.x() < 300.f*Scale_Rate / 2.f - offset) &&
							(-300.f*Scale_Rate / 2.f + offset < pos_t.z() && pos_t.z() < 300.f*Scale_Rate / 2.f - offset)
							) {
							this->m_WayPoint.emplace_back(bu.GetMatrix().pos() + VECTOR_ref::up()*(1.f*Scale_Rate));
						}
					}
				}
				//空
				MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				//Box2D壁
				{
					this->m_Box2DWall.Add(MV1GetReferenceMesh(this->m_ObjGroundCol_Box2D.get(), 0, FALSE));
					MV1TerminateReferenceMesh(this->m_ObjGroundCol_Box2D.get(), 0, FALSE);
					for (auto& bu : this->m_BuildControl.GetBuildCol()) {
						if (bu.GetMeshSel() >= 5) {
							this->m_Box2DWall.Add(MV1GetReferenceMesh(bu.GetColBox2D().get(), bu.GetMeshSel(), TRUE));
							MV1TerminateReferenceMesh(bu.GetColBox2D().get(), bu.GetMeshSel(), TRUE);
						}
					}
					this->m_Box2DWall.Init();
				}
				//木
				this->m_Tree.Init(&this->m_ObjGroundCol, this->m_BuildControl.GetBuildCol());
				//草
				{
					float MAPX = 300.f*Scale_Rate;
					float MAPZ = 300.f*Scale_Rate;
					float SIZX = 10.f*Scale_Rate;
					float SIZZ = 10.f*Scale_Rate;
					int x, y;
					GraphHandle BaseGrass = GraphHandle::Load("data/grass.png");
					BaseGrass.GetSize(&x, &y);
					m_softimage = MakeSoftImage(x, y);
					GraphHandle BlackScreen = GraphHandle::Make((int)((float)x*SIZX / MAPX), (int)((float)y*SIZZ / MAPZ));
					BlackScreen.SetDraw_Screen(false);
					{
						DrawBox(0, 0, (int)((float)x*SIZX / MAPX), (int)((float)y*SIZZ / MAPZ), GetColor(0, 0, 0), TRUE);
					}
					GraphHandle BaseScreen = GraphHandle::Make(x, y);
					BaseScreen.SetDraw_Screen(false);
					{
						BaseGrass.DrawGraph(0, 0, false);
						for (auto& b : this->m_BuildControl.GetBuildCol()) {
							if (b.GetMeshSel() >= 0) {
								auto pos = b.GetMatrix().pos();
								BlackScreen.DrawRotaGraph(
									(int)((float)x*(pos.x() + MAPX / 2.f) / MAPX),
									(int)((float)y*(pos.z() + MAPZ / 2.f) / MAPZ),
									1.f,
									std::atan2f(b.GetMatrix().zvec().x(), b.GetMatrix().zvec().z()),
									false
								);
							}
						}
						GetDrawScreenSoftImage(0, 0, x, y, m_softimage);
					}
					this->m_grass.Init(&this->m_ObjGroundCol, m_softimage);
					//DeleteSoftImage(m_softimage);
				}
				//壁
				{
					//this->m_BreakWall.Init();
				}
				//コリジョンども
				{
					this->m_MapCols.emplace_back((MV1*)(&this->m_ObjGroundCol));
					//for (int i = 0; i < this->m_BreakWall.GetWallGroundColNum(); i++) {
					//	this->m_MapCols.emplace_back((MV1*)(this->m_BreakWall.GetWallGroundCol(i)));
					//}
				}
			}
			//
			void			FirstExecute(void) noexcept {
				this->m_Box2DWall.GetBox2Dworld()->Step(1.f, 1, 1);//物理更新
			}
			//
			void			Execute(void) noexcept {
				//this->m_BreakWall.Execute();
				this->m_Tree.Execute();
			}
			//
			void			BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				this->m_ObjSky.DrawModel();
				SetUseLighting(TRUE);
			}
			void			Shadow_Draw_Far(void) noexcept {
				this->m_ObjGround.DrawModel();
				this->m_BuildControl.Draw();
			}
			void			Shadow_Draw_NearFar(void) noexcept {
				this->m_Tree.Draw(false);
			}
			void			Shadow_Draw(void) noexcept {
			}
			void			Draw(void) noexcept {
				this->m_ObjGround.DrawModel();
				this->m_BuildControl.Draw();
				this->m_Tree.Draw(true);
				this->m_grass.Draw();

				//DrawSoftImage(0,0,m_softimage);
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				this->m_ObjGround.Dispose();
				this->m_ObjGroundCol.Dispose();

				this->m_BuildControl.Dispose();
				this->m_grass.Dispose();
				this->m_Box2DWall.Dispose();
				//this->m_BreakWall.Dispose();
				this->m_Tree.Dispose();
			}
		};
	};
};
