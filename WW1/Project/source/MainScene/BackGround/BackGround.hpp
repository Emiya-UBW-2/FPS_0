#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClassBase {
		protected:
			MV1							m_ObjSky;
			MV1							m_ObjGround;
			MV1							m_ObjGroundCol;
		protected:
			virtual void			Init_Sub(void) noexcept {}
			//
			virtual void			FirstExecute_Sub(void) noexcept {}
			//
			virtual void			Execute_Sub(void) noexcept {}
			//
			virtual void			BG_Draw_Sub(void) noexcept {}
			virtual void			Shadow_Draw_Far_Sub(void) noexcept {}
			virtual void			Shadow_Draw_NearFar_Sub(void) noexcept {}
			virtual void			Shadow_Draw_Sub(void) noexcept {}
			virtual void			Draw_Sub(void) noexcept {}
			//
			virtual void			Dispose_Sub(void) noexcept {}
		public://
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
			void			Init(const char* MapPath, const char* SkyPath) noexcept {
				//’nŒ`
				std::string path;

				path = MapPath;
				path += "model.mv1";
				MV1::Load(path, &this->m_ObjGround);
				path = MapPath;
				path += "col.mv1";
				MV1::Load(path, &this->m_ObjGroundCol);
				this->m_ObjGroundCol.SetupCollInfo(64, 16, 64);
				//‹ó
				path = SkyPath;
				path += "model.mv1";
				MV1::Load(path, &this->m_ObjSky);
				MV1SetDifColorScale(this->m_ObjSky.get(), GetColorF(0.9f, 0.9f, 0.9f, 1.0f));
				//
				Init_Sub();
			}
			//
			void			FirstExecute(void) noexcept {
				FirstExecute_Sub();
			}
			//
			void			Execute(void) noexcept {
				Execute_Sub();
			}
			//
			void			BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				this->m_ObjSky.DrawModel();
				SetUseLighting(TRUE);
				BG_Draw_Sub();
			}
			void			Shadow_Draw_Far(void) noexcept {
				this->m_ObjGround.DrawModel();
				Shadow_Draw_Far_Sub();
			}
			void			Shadow_Draw_NearFar(void) noexcept {
				DrawCommon();
				Shadow_Draw_NearFar_Sub();
			}
			void			Shadow_Draw(void) noexcept {
				//DrawCommon();
				Shadow_Draw_Sub();
			}
			void			Draw(void) noexcept {
				DrawCommon();
				Draw_Sub();
			}
			//
			void			Dispose(void) noexcept {
				this->m_ObjSky.Dispose();
				this->m_ObjGround.Dispose();
				this->m_ObjGroundCol.Dispose();
				Dispose_Sub();
			}
		};
		//
		class BackGroundClassMovie : public BackGroundClassBase {
		private:
			int							m_softimage{ -1 };
			Grass						m_grass;
		public://
			//
			void			Init_Sub(void) noexcept override {
				//‘
				{
					float MAPX = 200.f*Scale_Rate;
					float MAPZ = 200.f*Scale_Rate;
					//float SIZX = 10.f*Scale_Rate;
					//float SIZZ = 10.f*Scale_Rate;
					int x, y;
					GraphHandle BaseGrass = GraphHandle::Load("data/grass2.png");
					BaseGrass.GetSize(&x, &y);
					m_softimage = MakeSoftImage(x, y);
					GraphHandle BaseScreen = GraphHandle::Make(x, y);
					BaseScreen.SetDraw_Screen(false);
					{
						BaseGrass.DrawGraph(0, 0, false);
						GetDrawScreenSoftImage(0, 0, x, y, m_softimage);
					}
					this->m_grass.Init(&this->m_ObjGroundCol, m_softimage,
						"data/model/grass/grass.png", "data/model/grass/model.mv1", MAPX, MAPZ,30);
					//DeleteSoftImage(m_softimage);
				}
			}
			//
			void			Execute_Sub(void) noexcept override {
			}
			//
			void			Shadow_Draw_Far_Sub(void) noexcept override {
				this->m_grass.Draw();
			}
			void			Shadow_Draw_Sub(void) noexcept override {
			}
			void			Draw_Sub(void) noexcept override {
				auto* OptionParts = OPTION::Instance();
				if (OptionParts->Get_grass_level() > 0) {
					this->m_grass.Draw();
				}
			}
			//
			void			Dispose_Sub(void) noexcept override {
				this->m_grass.Dispose();
			}
		};
		//
		class BackGroundClassMain : public BackGroundClassBase {
		private:
			int							m_softimage{ -1 };
			Grass						m_grass;
			TreeControl					m_Tree;
		public://
			//
			void			Init_Sub(void) noexcept override {
				//–Ø
				this->m_Tree.Load();
				this->m_Tree.Init(&this->m_ObjGroundCol);
				//‘
				{
					float MAPX = 6000.f*Scale_Rate;
					float MAPZ = 6000.f*Scale_Rate;
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
					this->m_grass.Init(&this->m_ObjGroundCol, m_softimage,
						"data/model/tree/tree.png", "data/model/tree/model3.mv1", MAPX, MAPZ, 6);
					//DeleteSoftImage(m_softimage);
				}
			}
			//
			void			Execute_Sub(void) noexcept override {
				auto* OptionParts = OPTION::Instance();
				if (OptionParts->Get_grass_level() > 0) {
					this->m_Tree.Execute();
				}
			}
			//
			void			Shadow_Draw_Far_Sub(void) noexcept override {
				this->m_Tree.DrawFar();
				this->m_grass.DrawShadow();
			}
			void			Draw_Sub(void) noexcept override {
				auto* OptionParts = OPTION::Instance();
				if (OptionParts->Get_grass_level() > 0) {
					this->m_Tree.Draw(true);
					this->m_grass.Draw();
				}
			}
			//
			void			Dispose_Sub(void) noexcept override {
				this->m_Tree.Dispose();
				this->m_grass.Dispose();
			}
		};
		//
	};
};
