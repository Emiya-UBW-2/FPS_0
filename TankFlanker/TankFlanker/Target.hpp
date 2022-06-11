#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TargetClass : public ObjectBaseClass {
			bool hitSwitch = false;
			VECTOR_ref hitpos;
			std::vector<VECTOR_ref> HitPosRec;
		public:
			void Init() override {
				ObjectBaseClass::Init();

				MV1SetMaterialOutLineWidth(this->obj.get(), 0, 0.f);
				MV1SetMaterialOutLineDotWidth(this->obj.get(), 0, 0.f);
				MV1SetMaterialOutLineWidth(this->obj.get(), 1, 0.f);
				MV1SetMaterialOutLineDotWidth(this->obj.get(), 1, 0.f);
				MV1SetMaterialOutLineWidth(this->obj.get(), 2, 0.f);
				MV1SetMaterialOutLineDotWidth(this->obj.get(), 2, 0.f);
			}

			void SetMat(float Yrad, const VECTOR_ref& pos) {
				this->move.mat = MATRIX_ref::RotY(Yrad);
				this->move.pos = pos;
				this->obj.SetMatrix(this->move.MatIn());

				this->col.SetMatrix(this->move.MatIn());
				this->col.RefreshCollInfo();
			}

			void ResetHit() {
				HitPosRec.clear();
			}

			void Execute() {
				if (hitSwitch) {
					//hitpos - col.frame(2);
					HitPosRec.emplace_back(hitpos);
				}
				hitSwitch = false;
			}
			void Draw() override {
				ObjectBaseClass::Draw();

				//for (auto& r : HitPosRec) {
				//	DrawSphere3D(r.get(), 2.f, 8, GetColor(0, 255, 0), GetColor(255, 255, 255), TRUE);
				//}
			}
		public:
			void SetHitPos(const VECTOR_ref& value) {
				hitSwitch = true;
				hitpos = value;
			}

			void DrawHitCard(int xp, int yp, int size, float AlphaPer) {
				int xs = size;
				int ys = size;
				int xp2 = xp + ys;
				int yp2 = yp + ys;
				if (AlphaPer > 0.01f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*AlphaPer));
					DrawCircle(xp + xs / 2, yp + ys / 2, xs / 2, GetColor(255, 0, 0), TRUE);
					DrawCircle(xp + xs / 2, yp + ys / 2, xs / 2, GetColor(255, 255, 255), FALSE);
					DrawLine(xp + xs / 2, yp, xp + xs / 2, yp2, GetColor(255, 255, 255));
					DrawLine(xp, yp + ys / 2, xp2, yp + ys / 2, GetColor(255, 255, 255));
					for (int i = 0; i < 10; i++) {
						DrawCircle(xp + xs / 2, yp + ys / 2, xs / 2 * i / 10, GetColor(255, 255, 255), FALSE);
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

					auto vecx = col.frame(3) - col.frame(2);
					auto vecy = col.frame(4) - col.frame(2);
					auto vecsize = (vecx.size() + vecy.size()) / 2;
					vecx = vecx.Norm();
					vecy = vecy.Norm();
					int ypAdd = 0;
					for (auto& r : HitPosRec) {
						auto vec2 = r - col.frame(2);
						float cos_t = vecx.dot(vec2.Norm());
						float sin_t = -vecy.dot(vec2.Norm());

						float xp_t = (float)xs / 2.f * (vec2.size() / vecsize) * cos_t;
						float yp_t = (float)ys / 2.f * (vec2.size() / vecsize) * sin_t;

						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255.f*AlphaPer));
						DrawCircle(
							xp + xs / 2 + (int)xp_t,
							yp + ys / 2 + (int)yp_t,
							2,
							GetColor(0, 255, 0)
						);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

						DrawFormatString(xp, yp2 + ypAdd, GetColor(255, 255, 255), "[%4.1f]", (vecsize - vec2.size()) / vecsize * 10.f);
						ypAdd += 18;
					}
				}
			}
		};
	};
};
