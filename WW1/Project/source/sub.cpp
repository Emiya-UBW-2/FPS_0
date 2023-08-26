#include	"sub.hpp"


const FPS_n2::KeyGuideClass* SingletonBase<FPS_n2::KeyGuideClass>::m_Singleton = nullptr;
const FPS_n2::SaveDataClass* SingletonBase<FPS_n2::SaveDataClass>::m_Singleton = nullptr;
const FPS_n2::Sceneclass::OptionWindowClass* SingletonBase<FPS_n2::Sceneclass::OptionWindowClass>::m_Singleton = nullptr;
const FPS_n2::PadControl* SingletonBase<FPS_n2::PadControl>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {
		void OptionWindowClass::Execute(void) noexcept {
			auto* SE = SoundPool::Instance();
			if (isActive) {
				auto* OptionParts = OPTION::Instance();
				auto* Pad = FPS_n2::PadControl::Instance();

				/*
				//ŒÄ‚Ño‚µ‚à‚Æ‚ÅŒÄ‚ñ‚Å‚È‚¯‚ê‚Î‚±‚ê‚ð“n‚·
				Pad->Execute(
					[&]() {},
					[&]() {});
				//*/

				if (Pad->GetUpKey().trigger()) {
					--select;
					if (select < 0) { select = selMax - 1; }
					SelYadd[select] = 10.f;

					SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				if (Pad->GetDownKey().trigger()) {
					++select;
					if (select > selMax - 1) { select = 0; }
					SelYadd[select] = -10.f;

					SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
				}
				for (int i = 0; i < selMax; i++) {
					Easing(&SelYadd[i], 0.f, 0.95f, EasingType::OutExpo);
				}
				{
					switch (select) {
					case 0:
						if (Pad->GetLeftKey().trigger()) {
							OptionParts->Set_BGM(std::clamp(OptionParts->Get_BGM() - 0.1f, 0.f, 1.f));
							SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (Pad->GetRightKey().trigger()) {
							OptionParts->Set_BGM(std::clamp(OptionParts->Get_BGM() + 0.1f, 0.f, 1.f));
							SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						break;
					case 1:
						if (Pad->GetLeftKey().trigger()) {
							OptionParts->Set_SE(std::clamp(OptionParts->Get_SE() - 0.1f, 0.f, 1.f));
							SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (Pad->GetRightKey().trigger()) {
							OptionParts->Set_SE(std::clamp(OptionParts->Get_SE() + 0.1f, 0.f, 1.f));
							SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						break;
					case 2:
						if (Pad->GetLeftKey().trigger() || Pad->GetRightKey().trigger()) {
							OptionParts->Set_grass_level(1 - OptionParts->Get_grass_level());
							SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						break;
					case 3:
						if (Pad->GetLeftKey().trigger() || Pad->GetRightKey().trigger()) {
							OptionParts->Set_Bloom(OptionParts->Get_Bloom() ^ 1);
							SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						break;
					case 4:
						if (Pad->GetLeftKey().trigger() || Pad->GetRightKey().trigger()) {
							OptionParts->Set_Shadow(OptionParts->Get_Shadow() ^ 1);
							SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						break;
					case 5:
						if (Pad->GetLeftKey().trigger() || Pad->GetRightKey().trigger()) {
							OptionParts->Set_Vsync(OptionParts->Get_Vsync() ^ 1);
							SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						break;
					case 6:
						if (Pad->GetLeftKey().trigger() || Pad->GetRightKey().trigger()) {
							OptionParts->Set_aberration(OptionParts->Get_aberration() ^ 1);
							SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						break;
					case 7:
						if (Pad->GetLeftKey().trigger()) {
							switch (OptionParts->Get_DirectXVer()) {
							case  DX_DIRECT3D_9:
								OptionParts->Set_DirectXVer(DX_DIRECT3D_11);
								break;
							case  DX_DIRECT3D_9EX:
								OptionParts->Set_DirectXVer(DX_DIRECT3D_9);
								break;
							case  DX_DIRECT3D_11:
								OptionParts->Set_DirectXVer(DX_DIRECT3D_9EX);
								break;
							default:
								break;
							}
							SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (Pad->GetRightKey().trigger()) {
							switch (OptionParts->Get_DirectXVer()) {
							case  DX_DIRECT3D_9:
								OptionParts->Set_DirectXVer(DX_DIRECT3D_9EX);
								break;
							case  DX_DIRECT3D_9EX:
								OptionParts->Set_DirectXVer(DX_DIRECT3D_11);
								break;
							case  DX_DIRECT3D_11:
								OptionParts->Set_DirectXVer(DX_DIRECT3D_9);
								break;
							default:
								break;
							}
							SE->Get((int)SoundEnum::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						break;
					default:
						break;
					}
				}
				if (Pad->GetOKKey().trigger()) {
					switch (select) {
					case 0:
						break;
					case 1:
						break;
					case 2:
						break;
					case 3:
						break;
					case 4:
						break;
					case 5:
						break;
					case 6:
						break;
					case 7:
						break;
					default:
						SE->Get((int)SoundEnum::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						isActive = false;
						break;
					}
				}
				if (Pad->GetNGKey().trigger()) {
					SE->Get((int)SoundEnum::UI_NG).Play(0, DX_PLAYTYPE_BACK, TRUE);
					isActive = false;
				}

				SE->SetVol(OptionParts->Get_SE());

				if (!isActive) {
					OptionParts->Save();
				}
			}
			else {
				select = 0;
			}
		}
	};
};
