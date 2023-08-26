#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TitleScene : public TEMPSCENE {
		private:
			int select{ 0 };
			std::array<float, 3> SelYadd{};

			float GameFadeIn{ 0.f };
			float GameStart{ 0.f };
			float GameStart2{ 0.f };
			float GameStart3{ 0.f };
		public:
			TitleScene(void) noexcept { }
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(bool* isPause) noexcept override;
			void			Dispose_Sub(void) noexcept override {
				//auto* SE = SoundPool::Instance();
			}
			//
			void			Depth_Draw_Sub(void) noexcept override {}
			void			BG_Draw_Sub(void) noexcept override {}
			void			ShadowDraw_Far_Sub(void) noexcept override {}
			void			ShadowDraw_NearFar_Sub(void) noexcept override {}
			void			ShadowDraw_Sub(void) noexcept override {}
			void			MainDraw_Sub(void) noexcept override {}
			void			MainDrawbyDepth_Sub(void) noexcept override {}
			//UI•\Ž¦
			void			DrawUI_Base_Sub(void) noexcept  override {}
			void			DrawUI_In_Sub(void) noexcept override;
		};
	};
};
