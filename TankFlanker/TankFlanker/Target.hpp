#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TargetClass : public ObjectBaseClass {
			std::vector<VECTOR_ref> HitPosRec;
		public:
			TargetClass(void) noexcept {
				this->m_objType = ObjType::Target;
			}
			~TargetClass(void) noexcept {}
		public:
			const auto SetHitPos(const VECTOR_ref& value) noexcept {
				HitPosRec.emplace_back(value);
				this->m_obj.get_anime(0).time = 0.f;

				auto vecx = this->col.frame(3) - this->col.frame(2);
				auto vecy = this->col.frame(4) - this->col.frame(2);
				auto vecsize = (vecx.size() + vecy.size()) / 2;
				auto vec2 = HitPosRec.back() - this->col.frame(2);
				return ((1.f - (vec2.size() / vecsize)) * 10.f);
			}
			void ResetHit(void) noexcept {
				HitPosRec.clear();
			}
			void Execute(void) noexcept override {
				this->m_obj.get_anime(0).per = 1.f;
				if (!this->m_obj.get_anime(0).TimeEnd()) {
					this->m_obj.get_anime(0).time += 30.f / FPS;
				}
				if (this->m_obj.get_anime(0).TimeEnd()) {
					this->m_obj.get_anime(0).time = this->m_obj.get_anime(0).alltime;
				}
				this->m_obj.work_anime();
			}

			void DrawHitCard(int xp, int yp, int size, float AlphaPer) noexcept {
				int xs = size;
				int ys = size;
				int xp2 = xp + ys;
				int yp2 = yp + ys;
				if (AlphaPer > 0.01f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*AlphaPer));
					//îwåi
					DrawCircle(xp + xs / 2, yp + ys / 2, xs / 2, GetColor(255, 0, 0), TRUE);
					DrawCircle(xp + xs / 2, yp + ys / 2, xs / 2, GetColor(255, 255, 255), FALSE);
					DrawLine(xp + xs / 2, yp, xp + xs / 2, yp2, GetColor(255, 255, 255));
					DrawLine(xp, yp + ys / 2, xp2, yp + ys / 2, GetColor(255, 255, 255));
					for (int i = 0; i < 10; i++) {
						DrawCircle(xp + xs / 2, yp + ys / 2, xs / 2 * i / 10, GetColor(255, 255, 255), FALSE);
					}
					//ñΩíÜâ”èä
					auto vecx = this->col.frame(3) - this->col.frame(2);
					auto vecy = this->col.frame(4) - this->col.frame(2);
					auto vecsize = (vecx.size() + vecy.size()) / 2;
					vecx = vecx.Norm();
					vecy = vecy.Norm();
					for (auto& r : HitPosRec) {
						auto vec2 = r - this->col.frame(2);
						float cos_t = -vecx.dot(vec2.Norm());
						float sin_t = -vecy.dot(vec2.Norm());

						DrawCircle(
							xp + xs / 2 + (int)((float)(xs / 2) * (vec2.size() / vecsize) * cos_t),
							yp + ys / 2 + (int)((float)(ys / 2) * (vec2.size() / vecsize) * sin_t),
							2, GetColor(0, 255, 0));
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					//ì_êî
					int ypAdd = 0;
					for (auto& r : HitPosRec) {
						auto vec2 = r - this->col.frame(2);
						DrawFormatString(xp, yp2 + ypAdd, GetColor(255, 255, 255), "[%4.1f]", (1.f - (vec2.size() / vecsize)) * 10.f);
						ypAdd += 18;
					}
				}
			}
		};
	};
};
