
#pragma once

#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClass {
			MV1 Sky;
			MV1 Ground;
			MV1 GroundAdd;
			MV1 GroundCol;
			std::vector< MV1> ShootingMat;
		public:
			void Load(void) noexcept {
				MV1::Load("data/model/sky/model.mv1", &Sky);
				MV1::Load("data/model/ground/model.mv1", &Ground);
				MV1::Load("data/model/ground/model_add.mv1", &GroundAdd);
				MV1::Load("data/model/ground/col.mv1", &GroundCol);

				for (int i = 0; i < 3; i++) {
					ShootingMat.resize(ShootingMat.size() + 1);
					if (ShootingMat.size() == 1) {
						MV1::Load("data/model/ShootingMat/model.mv1", &ShootingMat.back());
					}
					else {
						ShootingMat.back() = ShootingMat[0].Duplicate();
					}
				}

				GroundCol.SetupCollInfo();
				for (int i = 0; i < 3; i++) {
					ShootingMat[i].SetMatrix(MATRIX_ref::RotY(deg2rad(-90))*MATRIX_ref::Mtrans(VECTOR_ref::vget(1960.f, 90.0f, -973.72f + (20.f*(i - 1)))));
				}
			}
			void BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				Sky.DrawModel();
				SetUseLighting(TRUE);
			}
			void Shadow_Draw_NearFar(void) noexcept {
				Ground.DrawModel();
				GroundAdd.DrawModel();
			}
			void Shadow_Draw(void) noexcept {
				Ground.DrawModel();
				GroundAdd.DrawModel();
				for (auto& m : ShootingMat) {
					m.DrawModel();
				}
			}
			void Draw(void) noexcept {
				Ground.DrawModel();
				GroundAdd.DrawModel();
				for (auto& m : ShootingMat) {
					m.DrawModel();
				}
			}

			const auto& GetGroundCol(void) noexcept { return GroundCol; }
		};
	};
};