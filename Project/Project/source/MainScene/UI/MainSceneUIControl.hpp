#pragma once
#include"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
		private:
			GraphHandle HeartGraph;
			GraphHandle ScoreBoard;

			int intParam[3]{ 0 };
			float floatParam[4]{ 0 };
		public:
			void Set(void) noexcept {
				HeartGraph = GraphHandle::Load("data/UI/Heart.png");
				ScoreBoard = GraphHandle::Load("data/UI/Score.png");
			}
			void Draw(void) noexcept {
				auto* Fonts = FontPool::Instance();
				auto* DrawParts = DXDraw::Instance();
				//心拍数
				{
					int siz = y_r(32);
					int xP = siz, yP = DrawParts->m_DispYSize - siz - y_r(15);
					int xo, yo;
					HeartGraph.GetSize(&xo, &yo);
					HeartGraph.DrawRotaGraph(xP, yP, (float)(siz) / xo * floatParam[0], 0.f, true);
					Fonts->Get(y_r(24), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xP + siz / 2, yP + siz / 2 - y_r(24), GetColor(255, 0, 0), GetColor(0, 0, 0), "%03d", intParam[0]);
				}
				//スタミナ
				{
					float Xsize = (float)(y_r(200));
					int siz = y_r(12);
					int xP = siz, yP = DrawParts->m_DispYSize - siz - y_r(12);
					DrawBox(xP, yP, xP + (int)Xsize, yP + siz, GetColor(0, 0, 0), TRUE);
					DrawBox(xP, yP, xP + (int)(Xsize * floatParam[1]), yP + siz, (floatParam[1] > 0.3f) ? GetColor(0, 255, 0) : GetColor(255, 200, 0), TRUE);
					DrawBox(xP, yP, xP + (int)Xsize, yP + siz, GetColor(128, 128, 128), FALSE);
				}
				//向き
				{
					float Xsize = (float)(y_r(125));
					int siz = y_r(12);
					int xP = siz + (int)Xsize, yP = DrawParts->m_DispYSize - siz - y_r(64);
					int xpt = xP, ypt = yP;
					for (int i = 0; i < 5; i++) {
						float max = Xsize * (i + 1) / 5;
						float rad = floatParam[2] * (i + 1) / 5;
						DrawLine_2D(
							xpt, ypt,
							xP + (int)(max*sin(rad)), yP + (int)(max*-cos(rad)),
							GetColor(255, 0, 0), 5 - i);
						xpt = xP + (int)(max*sin(rad));
						ypt = yP + (int)(max*-cos(rad));
					}
					{
						float max = Xsize;
						float rad = floatParam[2];
						xpt = xP + (int)(max*sin(rad));
						ypt = yP + (int)(max*-cos(rad));

						float yap = Xsize / 5;
						DrawLine_2D(
							xpt - (int)(yap*sin(rad * 2.f - deg2rad(15))), ypt - (int)(yap*-cos(rad * 2.f - deg2rad(15))),
							xpt, ypt,
							GetColor(255, 0, 0), 2);
						DrawLine_2D(
							xpt - (int)(yap*sin(rad * 2.f + deg2rad(15))), ypt - (int)(yap*-cos(rad * 2.f + deg2rad(15))),
							xpt, ypt,
							GetColor(255, 0, 0), 2);
					}

					Fonts->Get(y_r(24), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat_RIGHT(xP - siz / 2, yP - y_r(14), GetColor((int)(192.f - 64.f*floatParam[2] * 2.f), 0, 0), GetColor(0, 0, 0), "Q");
					Fonts->Get(y_r(24), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xP + siz / 2, yP - y_r(14), GetColor((int)(192.f + 64.f*floatParam[2] * 2.f), 0, 0), GetColor(0, 0, 0), "E");
				}
				//弾数
				{
					int xP = DrawParts->m_DispXSize - y_r(24), yP = DrawParts->m_DispYSize - y_r(32);

					Fonts->Get(y_r(32), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat_RIGHT(xP - y_r(48), yP - y_r(18), GetColor(255, 255, 255), GetColor(0, 0, 0), "%02d", intParam[1]);
					Fonts->Get(y_r(24), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat_RIGHT(xP - y_r(30), yP - y_r(6), GetColor(192, 192, 192), GetColor(0, 0, 0), "/");
					Fonts->Get(y_r(18), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat_RIGHT(xP, yP, GetColor(192, 192, 192), GetColor(0, 0, 0), "%02d", intParam[2]);
				}
				//スコア
				{
					int xP = y_r(150), yP = DrawParts->m_DispYSize - y_r(64);
					Fonts->Get(y_r(32), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat_RIGHT(xP + y_r(54), yP, GetColor(255, 255, 255), GetColor(0, 0, 0), "%02d", (int)floatParam[3]);
					Fonts->Get(y_r(18), FontPool::FontType::Nomal_Edge).Get_handle().DrawStringFormat(xP + y_r(54), yP + y_r(14), GetColor(255, 255, 255), GetColor(0, 0, 0), ".%02d", (int)((floatParam[3] - (float)((int)floatParam[3]))*100.f));
				}
			}

			void SetIntParam(int ID, int value) { intParam[ID] = value; }
			void SetfloatParam(int ID, float value) { floatParam[ID] = value; }

			auto& GetScoreBoard(void) { return ScoreBoard; }
		};
	};
};