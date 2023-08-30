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

			int prevScore{ 0 };
			std::vector<std::pair<int, float>> ScoreAdd;
		private:
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
				//return;
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
				prevScore = 0;
				ScoreAdd.clear();
			}
			void			Draw(void) noexcept {
				auto* Fonts = FontPool::Instance();
				auto* DrawParts = DXDraw::Instance();
				auto Red = GetColor(255, 0, 0);
				//auto Blue = GetColor(50, 50, 255);
				auto Green = GetColor(64, 192, 48);
				auto White = GetColor(255, 255, 255);
				auto Gray = GetColor(64, 64, 64);
				//auto Black = GetColor(0, 0, 0);
				//タイム,スコア
				{
					int xp1, yp1;
					xp1 = y_r(30);
					yp1 = y_r(10);
					//Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP,
					Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Gray, "TIME");

					Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, xp1 + y_r(240), yp1, White, Gray, "%d:%05.2f",
						(int)(floatParam[0] / 60.f),
						(float)((int)(floatParam[0]) % 60) + (floatParam[0] - (float)((int)(floatParam[0])))
					);

					yp1 += y_r(25);
					Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Gray, "SCORE");
					Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, xp1 + y_r(240), yp1, White, Gray, "%d", intParam[6]);
				}
				{
					int xp1, yp1;
					xp1 = DrawParts->m_DispXSize / 2;
					yp1 = DrawParts->m_DispYSize / 2;

					if (intParam[6] != prevScore) {
						if (intParam[6] > prevScore) {
							ScoreAdd.emplace_back(std::make_pair(intParam[6] - prevScore, 2.f));
						}
						prevScore = intParam[6];
					}
					for (int i = 0; i < ScoreAdd.size(); i++) {
						auto& s = ScoreAdd[i];
						if (s.second > 0.f) {
							float per = std::powf(2.f - s.second, 2.f);
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*(1.f- per)), 0, 255));
							Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(32), FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::BOTTOM, xp1, yp1 - y_r(per*96.f), Green, Gray, "+%d", s.first);
						}
						else {
							std::swap(s, ScoreAdd.back());
							ScoreAdd.pop_back();
							i--;
						}
						s.second = std::max(s.second - 1.f / FPS, 0.f);
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				//情報
				{
					int xp1, yp1;
					//名前、体力
					{
						xp1 = DrawParts->m_DispXSize - y_r(260);
						yp1 = DrawParts->m_DispYSize - y_r(80);

						Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1 + y_r(50), yp1, White, Gray, "%s", strParam[0].c_str());
						yp1 += y_r(25);

						Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Gray, "HP");
						Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1 + y_r(50), yp1, White, Gray, "%03d / %03d", intParam[3], intParam[4]);
						yp1 += y_r(32);

						DrawGauge(
							xp1, yp1, xp1 + y_r(230), yp1 + y_r(12),
							intParam[3], intParam[4], intParam[5], GetColorU8(255, 0, 0, 255), GetColorU8(255, 255, 0, 255), GetColorU8(0, 255, 0, 255),
							GetColor(0, 0, 255), GetColor(255, 0, 0)
						);
						yp1 += y_r(10);
					}
				}
				//高度、速度
				{
					float rad = deg2rad(intParam[2])/60.f;
					int xp1, yp1;
					if (ItemGraphPtr[0]) {
						float per = std::clamp((floatParam[3] - 60.f) / (220.f - 60.f), 0.f, 1.f);

						xp1 = DrawParts->m_DispXSize / 2 + intParam[0] - y_r(300.f*std::cos(rad));
						yp1 = DrawParts->m_DispYSize / 2 + intParam[1] - y_r(300.f*std::sin(rad)) - y_r(18) / 2;


						SetDrawBlendMode(DX_BLENDMODE_ADD, 32);
						DrawCircleGauge(xp1 + y_r(256), yp1, 50.0 + (double)((50.f - 15.0f*2.f)*1.f) + 15.0, ItemGraphPtr[0]->get(), 50.0 + 15.0, (double)((float)(DrawParts->m_DispYSize) / 1080.f));
						DrawCircleGauge(xp1 + y_r(256), yp1, 50.0 + (double)((50.f - 15.0f*2.f)*per) + 15.0, ItemGraphPtr[0]->get(), 50.0 + 15.0, (double)((float)(DrawParts->m_DispYSize) / 1080.f));
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

						Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, xp1, yp1, (per > 0.f) ? White : Red, Gray, "SPEED %5.2f km/h", floatParam[1]);
					}
					if (ItemGraphPtr[1]) {
						float per = (floatParam[4]) / (500.f);

						xp1 = DrawParts->m_DispXSize / 2 + intParam[0] + y_r(300.f*std::cos(rad));
						yp1 = DrawParts->m_DispYSize / 2 + intParam[1] + y_r(300.f*std::sin(rad)) - y_r(18) / 2;

						SetDrawBlendMode(DX_BLENDMODE_ADD, 32);
						DrawCircleGauge(xp1 - y_r(256), yp1, (double)(50.f - 15.0f*2.f) + 15.0, ItemGraphPtr[1]->get(), (double)((50.f - 15.0f*2.f)*(1.f - 1.f)) + 15.0, (double)((float)(DrawParts->m_DispYSize) / 1080.f));
						DrawCircleGauge(xp1 - y_r(256), yp1, (double)(50.f - 15.0f*2.f) + 15.0, ItemGraphPtr[1]->get(), (double)((50.f - 15.0f*2.f)*(1.f - per)) + 15.0, (double)((float)(DrawParts->m_DispYSize) / 1080.f));
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

						Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(18), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, (per > 0.f) ? White : Red, Gray, "ALT %4d m", (int)floatParam[2]);
					}
				}
				//
			}

			void			SetIntParam(int ID, int value) { intParam[ID] = value; }
			void			SetfloatParam(int ID, float value) { floatParam[ID] = value; }
			void			SetStrParam(int ID, std::string_view value) { strParam[ID] = value; }
			void			SetItemGraph(int ID, const GraphHandle* value) { ItemGraphPtr[ID] = (GraphHandle*)value; }
		};
	};
};