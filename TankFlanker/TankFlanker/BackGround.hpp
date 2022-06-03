
#pragma once

#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BackGroundClass {
			MV1 Sky;
			MV1 Ground;
			MV1 GroundCol;
		public:
			void Load() {
				MV1::Load("data/model/sky/model.mv1", &Sky);
				MV1::Load("data/model/ground/model.mv1", &Ground);
				MV1::Load("data/model/ground/col.mv1", &GroundCol);
			}
			void BG_Draw(void) noexcept {
				SetUseLighting(FALSE);
				Sky.DrawModel();
				SetUseLighting(TRUE);
			}
			void Shadow_Draw_NearFar(void) noexcept {
				Ground.DrawModel();
			}
			void Draw(void) noexcept {
				Ground.DrawModel();
			}

			const auto& GetGroundCol() { return GroundCol; }
		};
	};
};