#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClass {
		private:
			MV1							m_ObjSky;
			MV1							m_ObjGround;
			MV1							m_ObjGroundCol;

			int							m_softimage{ -1 };
			Grass						m_grass;
			TreeControl					m_Tree;
		public://getter
			const auto&		GetGroundCol(void) noexcept { return this->m_ObjGroundCol; }
			const auto		CheckLinetoMap(const VECTOR_ref& StartPos, VECTOR_ref* EndPos, bool isNearest, VECTOR_ref* Normal = nullptr, MV1_COLL_RESULT_POLY* Ret = nullptr) {
				bool isHit = false;
				{
					auto col_p = this->m_ObjGroundCol.CollCheck_Line(StartPos, *EndPos);
					if (col_p.HitFlag == TRUE) {
						isHit = true;
						if (Ret) { *Ret = col_p; }
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
		private:
			void			DrawCommon() noexcept {
				SetFogEnable(TRUE);
				//SetFogColor(54, 59, 50);
				SetFogColor(102, 114, 80);
				//SetFogDensity(0.5f);

				this->m_ObjGround.DrawModel();

				SetUseBackCulling(TRUE);
				SetFogEnable(FALSE);
			}
		public://
			//
			void			Init(void) noexcept {
				//’nŒ`
				MV1::Load("data/model/map/model.mv1", &this->m_ObjGround);
				MV1::Load("data/model/map/col.mv1", &this->m_ObjGroundCol);
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);
				//‹ó
				MV1::Load("data/model/sky/model.mv1", &this->m_ObjSky);
				MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				//
				this->m_Tree.Load();
				//–Ø
				this->m_Tree.Init(&this->m_ObjGroundCol);
				//‘
				{
					//float MAPX = 300.f*Scale_Rate;
					//float MAPZ = 300.f*Scale_Rate;
					//float SIZX = 10.f*Scale_Rate;
					//float SIZZ = 10.f*Scale_Rate;
					int x, y;
					GraphHandle BaseGrass = GraphHandle::Load("data/grass.png");
					BaseGrass.GetSize(&x, &y);
					m_softimage = MakeSoftImage(x, y);
					GraphHandle BaseScreen = GraphHandle::Make(x, y);
					BaseScreen.SetDraw_Screen(false);
					{
						BaseGrass.DrawGraph(0, 0, false);
						GetDrawScreenSoftImage(0, 0, x, y, m_softimage);
					}
					this->m_grass.Init(&this->m_ObjGroundCol, m_softimage);
					//DeleteSoftImage(m_softimage);
				}
			}
			//
			void			FirstExecute(void) noexcept {
			}
			//
			void			Execute(void) noexcept {
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
				this->m_Tree.DrawFar();
				this->m_grass.DrawShadow();
			}
			void			Shadow_Draw_NearFar(void) noexcept {
				DrawCommon();
			}
			void			Shadow_Draw(void) noexcept {
				//DrawCommon();
			}
			void			Draw(void) noexcept {
				DrawCommon();
				this->m_Tree.Draw(true);
				this->m_grass.Draw();
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				this->m_ObjGround.Dispose();
				this->m_ObjGroundCol.Dispose();
				this->m_Tree.Dispose();
				this->m_grass.Dispose();
			}
		};
	};
};
