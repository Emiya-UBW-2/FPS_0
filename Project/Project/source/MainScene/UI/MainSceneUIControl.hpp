#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class UIClass {
		private:
			std::array<int, 23>				intParam{ 0 };
			std::array<float, 5>			floatParam{ 0 };
			std::array<std::string, 7>		strParam;
			std::array<GraphHandle*, 3>		ItemGraphPtr{ 0 };

			int Blend3Int(int pInt1, int pInt2, int pInt3, float per) {
				int ans;
				ans = (int)(std::clamp<float>(per * 2.f - 1.f, 0.f, 1.f) * (float)pInt3);
				ans += (int)(std::clamp<float>((per < 0.5f) ? (per * 2.f) : ((1.f - per) * 2.f), 0.f, 1.f) * (float)pInt2);
				ans += (int)(std::clamp<float>((1.f - per) * 2.f - 1.f, 0.f, 1.f) * (float)pInt1);
				return ans;
			}

			COLOR_U8 Blend3Color(COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, float per) {
				int r, g, b;
				r = Blend3Int(Color1.r, Color2.r, Color3.r, per);
				g = Blend3Int(Color1.g, Color2.g, Color3.g, per);
				b = Blend3Int(Color1.b, Color2.b, Color3.b, per);
				return GetColorU8(r, g, b, 255);
			}
			void			DrawGauge(int xp1, int yp1, int xp2, int yp2,
				int Param1, int ParamMax, int ParamBuf,
				COLOR_U8 Color1, COLOR_U8 Color2, COLOR_U8 Color3, unsigned int ColorAdd, unsigned int ColorSub) {
				DrawBox(xp1 + 0, yp1 + 0, xp2 - 0, yp2 - 0, GetColor(255, 255, 255), FALSE);
				DrawBox(xp1 + 1, yp1 + 1, xp2 - 1, yp2 - 1, GetColor(255, 255, 255), FALSE);
				int length = (xp2 - 2) - (xp1 + 2);

				COLOR_U8 Color = Blend3Color(Color1, Color2, Color3, (float)Param1 / (float)ParamMax);

				DrawBox(xp1 + 2 + length * std::max(Param1, ParamBuf) / ParamMax, yp1 + 2, xp2 - 2, yp2 - 2, GetColor(0, 0, 0), TRUE);
				DrawBox(xp1 + 2, yp1 + 2, xp1 + 2 + length * Param1 / ParamMax, yp2 - 2, GetColor(Color.r, Color.g, Color.b), TRUE);
				DrawBox(xp1 + 2 + length * ParamBuf / ParamMax, yp1 + 2, xp1 + 2 + length * Param1 / ParamMax, yp2 - 2, (ParamBuf > Param1) ? ColorSub : ColorAdd, TRUE);
			}
		public:
			void			Set(void) noexcept {
				for (int i = 0; i < 3; i++) {
					ItemGraphPtr[i] = nullptr;
				}
			}
			void			Draw(void) noexcept {
				auto* Fonts = FontPool::Instance();
				auto* DrawParts = DXDraw::Instance();
				auto Red = GetColor(255, 0, 0);
				auto Blue = GetColor(50, 50, 255);
				auto Green = GetColor(43, 163, 91);
				auto White = GetColor(255, 255, 255);
				auto Black = GetColor(0, 0, 0);
				//タイム,スコア
				{
					int xp1, yp1;
					xp1 = y_r(10);
					yp1 = y_r(10);
					//Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(20), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
					Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, Green, White, "TIME");
					Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(20), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, xp1 + y_r(240), yp1, Green, White, "%d:%05.2f", intParam[0], floatParam[0]);

					yp1 += y_r(25);
					Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, Green, White, "SCORE");
					Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(20), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, xp1 + y_r(240), yp1, Green, White, "%d", intParam[1]);
				}
				//情報
				{
					int xp1, yp1;
					//名前、体力、魔力
					{
						xp1 = y_r(50);
						yp1 = DrawParts->m_DispYSize - y_r(140);

						Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1 + y_r(50), yp1, Green, White, "%s", strParam[0].c_str());
						yp1 += y_r(25);

						Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, Green, White, "HP");
						Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1 + y_r(50), yp1, Green, White, "%03d / %03d", intParam[3], intParam[4]);
						yp1 += y_r(25);

						DrawGauge(
							xp1, yp1, xp1 + y_r(200), yp1 + y_r(10),
							intParam[3], intParam[4], intParam[5], GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(0, 255, 0, 255),
							GetColor(0, 0, 255), GetColor(255, 0, 0)
						);
						yp1 += y_r(10);

						Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, Green, White, "MP");
						Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1 + y_r(50), yp1, Green, White, "%03d / %03d", intParam[6], intParam[7]);
						yp1 += y_r(25);

						DrawGauge(
							xp1, yp1, xp1 + y_r(200), yp1 + y_r(10),
							intParam[6], intParam[7], intParam[8], GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(0, 255, 0, 255),
							GetColor(0, 0, 255), GetColor(255, 0, 0)
						);
						yp1 += y_r(10);
					}
					//魔法、クールタイム
					{
						xp1 = DrawParts->m_DispXSize - y_r(275);
						yp1 = DrawParts->m_DispYSize - y_r(48) - y_r(84);
						if (strParam[1].length() > 0) {
							auto per = (float)(100 * intParam.at(14) / intParam.at(15)) / 100.f;

							if (ItemGraphPtr[0] != nullptr) {
								int x, y;
								ItemGraphPtr[0]->GetSize(&x, &y);
								ItemGraphPtr[0]->DrawRotaGraph(xp1 + y_r(250) - (int)((float)(y_r(x / 2))*0.5f), yp1 - (int)((float)(y_r(y / 2))*0.5f), (float)(y_r(100)) / 100.f*0.5f, 0.f, true);

								Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
									xp1 + y_r(250) - y_r(x / 2), yp1 - y_r(y / 2), White, Black, "%s", strParam[1].c_str());
							}


							Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(36), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
								xp1 + y_r(50) + y_r(80), yp1 - y_r(12 - 4), (per <= 0.6f) ? Red : White, Black, "%03d", intParam.at(14));

							Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
								xp1 + y_r(50) + y_r(80), yp1, White, Black, " / %03d", intParam.at(15));
						}
						yp1 += y_r(64);
						for (int i = 1; i <= 2; i++) {
							auto& str = strParam.at(1 + i);
							if (str.length() > 0) {
								if (ItemGraphPtr[i] != nullptr) {
									SetDrawBright(192, 192, 192);
									int x, y;
									ItemGraphPtr[i]->GetSize(&x, &y);
									ItemGraphPtr[i]->DrawRotaGraph(xp1 + y_r(250) - (int)((float)(y_r(x / 2))*0.25f), yp1 - (int)((float)(y_r(y / 2))*0.25f), (float)(y_r(100)) / 100.f*0.25f, 0.f, true);

									Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(12), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
										xp1 + y_r(250) - (int)((float)(y_r(x / 2))*0.5f), yp1 - (int)((float)(y_r(y / 2))*0.5f), White, Black, "%s", str.c_str());
									SetDrawBright(255, 255, 255);
								}
							}
							yp1 += y_r(48);
						}
					}
				}
				//セリフ
				{
					std::string Mes = strParam[6];
					if (Mes.length() > 0) {
						unsigned int color = Blue;
						switch ((CharaTypeID)intParam[22]) {
						case CharaTypeID::Team:
							color = Blue;
							break;
						case CharaTypeID::Enemy:
							color = Red;
							break;
						default:
							color = Green;
							break;
						};

						int xp1, yp1;
						int xs1, ys1;
						xp1 = DrawParts->m_DispXSize / 2;
						yp1 = DrawParts->m_DispYSize / 20;
						Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(28), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, xp1, yp1, color, White, "<%s>", Mes.c_str());



						int picx = std::max(DrawParts->m_DispXSize / 8, Fonts->Get(FontPool::FontType::HUD_Edge).GetStringWidth(y_r(28), "<%s>", Mes.c_str()) / 2 + y_r(20));

						xs1 = y_r(40);
						ys1 = y_r(40);
						xp1 = DrawParts->m_DispXSize / 2 + picx + xs1;
						yp1 = DrawParts->m_DispYSize / 20 - y_r(28) + ys1;

						DrawBox((int)(xp1 - xs1), (int)(yp1 - ys1), (int)(xp1 + xs1), (int)(yp1 + ys1), GetColor(0, 0, 0), TRUE);
						Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(20), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, xp1, yp1 + ys1, color, White, "%s", strParam[5].c_str());
					}
				}
			}

			void			SetIntParam(int ID, int value) { intParam[ID] = value; }
			void			SetfloatParam(int ID, float value) { floatParam[ID] = value; }
			void			SetStrParam(int ID, std::string_view value) { strParam[ID] = value; }
			void			SetItemGraph(int ID, const GraphHandle* value) { ItemGraphPtr[ID] = (GraphHandle*)value; }
		};
	};
};