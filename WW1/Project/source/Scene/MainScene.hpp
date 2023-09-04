#pragma once
#include	"../Header.hpp"
#include "../NetWork.hpp"
#include "../MainScene/UI/MainSceneUIControl.hpp"
#include "../MainScene/BackGround/BackGround.hpp"
#include "../MainScene/NetworkBrowser.hpp"
#include "../MainScene/Object/Character.hpp"
#include "../MainScene/Player/CPU.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MAINLOOP : public TEMPSCENE, public EffectControl {
		private:
			static const int		Chara_num = Player_num;
		private:
			//リソース関連
			std::shared_ptr<BackGroundClassMain>			m_BackGround;				//BG
			SoundHandle				m_BGM;
			SoundHandle				m_AimOn;
			//人
			std::vector<std::shared_ptr<CharacterClass>> character_Pool;	//ポインター別持ち
			std::vector<std::shared_ptr<AIControl>>		m_AICtrl;						//AI
			//操作関連
			float					m_EyeRunPer{ 0.f };
			int						m_LookMode{ 0 };
			int						m_LookOn{ -1 };
			//UI関連
			UIClass					m_UIclass;
			float					m_HPBuf{ 0.f };
			GraphHandle				autoaimpoint_Graph;
			GraphHandle				Enemyaimpoint_Graph;
			GraphHandle				hit_Graph;
			GraphHandle				aim_Graph;
			GraphHandle				Gauge_Graph;
			GraphHandle				OIL_Graph;
			GraphHandle				Enemy_Graph;
			int						stand_sel{ 0 };
			float					stand_selAnim{ 0.f };
			float					stand_selAnimR{ 0.f };
			float					stand_AnimTime{ 0.f };
			//
			float					m_CamShake{ 0.f };
			VECTOR_ref				m_CamShake1;
			VECTOR_ref				m_CamShake2;
			//
			VECTOR_ref				m_FreeLookVec;
			MATRIX_ref				m_TPSLookMat;
			float					m_TPS_Xrad{ 0.f };
			float					m_TPS_Yrad{ 0.f };
			float					m_TPS_XradR{ 0.f };
			float					m_TPS_YradR{ 0.f };
			float					m_FreeLook_Per{ 1.f };
			float					m_Aim_Per{ 1.f };
			//
			std::vector<DamageEvent>	m_DamageEvents;

			NetWorkBrowser			m_NetWorkBrowser;
			//共通
			double					m_ClientFrame{ 0.0 };

			float					fov_base{ 0.f };

			float					StartTimer{ 5.f };
			float					Timer{ 0.f };
			float					EndTimer{ 3.f };
			float					TotalTime{ 180.f };

			float					SpeedUpdateTime{ 0.f };
			int						WatchSelect{ 0 };

			float					m_AutoAimTimer{ -1 };
			int						m_AutoAimSel{ -1 };
			VECTOR_ref				m_AimPoint;
			float					m_AimRot{ 0.f };
			
			float					m_ResultXofs{ 0.f };
			float					m_ResultColor{ 0.f };

			bool					m_GameEnd{ false };

			float					m_DrawSpeed{ 0.f };
		private:
			int select{ 0 };
			std::array<float, 3> SelYadd{};
		private:
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_NetWorkBrowser.GetMyPlayerID(); }
		public:
			MAINLOOP(void) noexcept { }
			void			Load_Sub(void) noexcept override {
				auto* SE = SoundPool::Instance();

				SetCreate3DSoundFlag(FALSE);
				this->m_BGM = SoundHandle::Load("data/Sound/BGM/Beethoven8_2.wav");
				//this->m_AimOn = SoundHandle::Load("data/Sound/SE/aim_on.wav");
				SE->Add((int)SoundEnum::Env, 1, "data/Sound/SE/envi.wav", false);
				SetCreate3DSoundFlag(FALSE);
				SE->Add((int)SoundEnum::Engine, Chara_num, "data/Sound/SE/engine.wav");
				SE->Add((int)SoundEnum::Propeller, Chara_num, "data/Sound/SE/Propeller.wav");
				SE->Add((int)SoundEnum::Shot2, Chara_num * 3, "data/Sound/SE/hit.wav");

				autoaimpoint_Graph = GraphHandle::Load("data/UI/battle_autoaimpoint.bmp");
				Enemyaimpoint_Graph = GraphHandle::Load("data/UI/battle_enemyaimpoint.bmp");
				hit_Graph = GraphHandle::Load("data/UI/battle_hit.bmp");
				aim_Graph = GraphHandle::Load("data/UI/battle_aim.bmp");
				Gauge_Graph = GraphHandle::Load("data/UI/Gauge.png");
				OIL_Graph = GraphHandle::Load("data/UI/back.png");
				Enemy_Graph = GraphHandle::Load("data/UI/enemyrad.png");

				//BG
				this->m_BackGround = std::make_shared<BackGroundClassMain>();
				this->m_BackGround->Init("data/model/map/", "data/model/sky/");

				for (auto& y : SelYadd) {
					y = 0.f;
				}
			}
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			Depth_Draw_Sub(void) noexcept override {
				//auto* ObjMngr = ObjectManager::Instance();
				//auto* PlayerMngr = PlayerManager::Instance();

				this->m_BackGround->Draw();
				//ObjMngr->DrawDepthObject();
			}
			void			BG_Draw_Sub(void) noexcept override {
				this->m_BackGround->BG_Draw();
			}
			void			ShadowDraw_Far_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw_Far();
			}
			void			ShadowDraw_NearFar_Sub(void) noexcept override;
			void			ShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			void			MainDrawbyDepth_Sub(void) noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept override;
			void			DrawUI_In_Sub(void) noexcept override;
		};
	};
};
