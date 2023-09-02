#include	"TitleScene.hpp"

#include "../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			TitleScene::Set_Sub(void) noexcept {
			Get_Next()->Load();
			auto* SE = SoundPool::Instance();
			//�T�E���h
			//this->m_BGM = SoundHandle::Load("data/Sound/BGM/Beethoven8_2.wav");
			SE->Add((int)SoundEnum::StandUp, 1, "data/Sound/SE/move/sliding.wav", false);
			SE->Add((int)SoundEnum::Siren, 1, "data/Sound/SE/siren.wav", false);
			//this->m_BGM.vol(128);
			select = 0;
			GameFadeIn = 3.f;
			GameStart = 0.f;
			GameStart2 = 0.f;
			GameStart3 = 0.f;

			for (auto& y : SelYadd) {
				y = 0.f;
			}
		}
		//
		bool			TitleScene::Update_Sub(bool* isPause) noexcept {
			if (*isPause) {
				return true;
			}
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//�f�o�b�O
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("update start");
#endif // DEBUG
			auto* SE = SoundPool::Instance();
			auto* Pad = PadControl::Instance();
			//FirstDoingv
			if (GetIsFirstLoop()) {
			}

			GameFadeIn = std::max(GameFadeIn - 1.f / FPS, 0.f);

			Pad->Execute(
				[&]() {
				auto* KeyGuide = FPS_n2::KeyGuideClass::Instance();
				KeyGuide->Reset();
				KeyGuide->AddGuide("ng.png", "����");
				KeyGuide->AddGuide("ok.png", "�߂�");
				KeyGuide->AddGuide("R_stick.png", "�㉺�I��,����");
			},
				[&]() {
				auto* KeyGuide = FPS_n2::KeyGuideClass::Instance();
				KeyGuide->Reset();
				KeyGuide->AddGuide("none.jpg", "����");
				KeyGuide->AddGuide("X.jpg", "�߂�");
				KeyGuide->AddGuide("W.jpg", "");
				KeyGuide->AddGuide("S.jpg", "�㉺�I��");
				KeyGuide->AddGuide("A.jpg", "");
				KeyGuide->AddGuide("D.jpg", "����");
			}
			);
			if (!OptionWindowClass::Instance()->IsActive()) {
				if (GameStart == 0.f && GameStart2 == 0.f) {
					if (Pad->GetUpKey().trigger()) {
						--select;
						if (select < 0) { select = 2; }
						SelYadd[select] = 10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					if (Pad->GetDownKey().trigger()) {
						++select;
						if (select > 2) { select = 0; }
						SelYadd[select] = -10.f;

						SE->Get((int)SoundEnumCommon::UI_Select).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
				}
				for (int i = 0; i < 3; i++) {
					Easing(&SelYadd[i], 0.f, 0.95f, EasingType::OutExpo);
				}

				if (GameFadeIn == 0.f) {
					switch (select) {
					case 0:
						if (Pad->GetOKKey().trigger()) {
							SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (!(GameStart == 0.f && !Pad->GetOKKey().trigger())) {
							GameStart += 1.f / FPS / 0.5f;
						}
						break;
					case 1:
						if (Pad->GetOKKey().trigger()) {
							SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (!(GameStart2 == 0.f && !Pad->GetOKKey().trigger())) {
							GameStart2 += 1.f / FPS / 0.5f;
						}
						break;
					case 2:
						if (Pad->GetOKKey().trigger()) {
							SE->Get((int)SoundEnumCommon::UI_OK).Play(0, DX_PLAYTYPE_BACK, TRUE);
						}
						if (Pad->GetOKKey().trigger()) {
							OptionWindowClass::Instance()->SetActive();
						}
						break;
					default:
						break;
					}
				}
			}

			if (GameStart >= 1.f || GameStart2 >= 1.f) {
				g_Mode = select;
				return false;
			}
			return true;
		}

		void			TitleScene::DrawUI_In_Sub(void) noexcept {
			auto Red = GetColor(255, 0, 0);
			auto Red75 = GetColor(192, 0, 0);
			auto White = GetColor(255, 255, 255);
			auto Gray75 = GetColor(128, 128, 128);
			auto Gray = GetColor(64, 64, 64);
			auto Black = GetColor(0, 0, 0);

			auto* DrawParts = DXDraw::Instance();
			auto* Fonts = FontPool::Instance();
			auto kill = FPS_n2::SaveDataClass::Instance()->GetParam("KillCount");
			auto killTotal = 80;
			if (kill >= killTotal) {
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(64, 8, 8), TRUE);
			}
			else {
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(16, 8, 8), TRUE);
			}
			int xp1, yp1;

			xp1 = y_r(64 + 600);
			yp1 = y_r(64 + 48 * 2);
			if (kill >= killTotal) {
				auto WhiteR = GetColor(255, 64, 64);
				auto GrayR = GetColor(128, 16, 16);
				Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 3 / 2), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), GrayR, Gray, "Final EpiSode");
				Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48 * 2), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, WhiteR, Gray, "Final EpiSode");
			}
			else {
				DrawFetteString(xp1, yp1, 1.33f, false, "Final EpiSode");
			}
			//
			xp1 = y_r(1920 - 256 - 54 * 2 - (int)(GameStart*100.f));
			yp1 = y_r(1080 - 108 - 108 * 2 + (int)SelYadd[0]);

			{
				auto per = std::clamp((1.f - GameStart), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
				Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 3 / 2 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), (select == 0) ? Red75 : Gray75, Gray, "Start Game");
				Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48 * 2 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, (select == 0) ? Red : White, Gray, "Start Game");
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}

			xp1 = y_r(1920 - 256 - 54 * 1 - (int)(GameStart2*100.f));
			yp1 = y_r(1080 - 108 - 108 * 1 + (int)SelYadd[1]);

			{
				auto per = std::clamp((1.f - GameStart2), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));
				if (kill >= killTotal) {
					auto WhiteR = GetColor(255, 64, 64);
					auto GrayR = GetColor(128, 16, 16);

					Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 - y_r(500), yp1, Gray75, Gray, "%02d / %02d", kill, killTotal);

					Fonts->Get(FontPool::FontType::Nomal_AA).DrawString(y_r(48 * 3 / 2 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 + y_r(40), yp1 + y_r(20), (select == 1) ? Red75 : GrayR, Gray, "Red Baron Mode");
					Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48 * 2 * 3 / 4), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, (select == 1) ? Red : WhiteR, Gray, "Red Baron Mode");
				}
				else {
					DrawFetteString(xp1, yp1, 1.f, (select == 1), "Free Flight");

					Fonts->Get(FontPool::FontType::Fette_AA).DrawString(y_r(48), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, xp1 - y_r(350), yp1, White, Gray, "%02d / %02d", kill, killTotal);
				}
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}

			xp1 = y_r(1920 - 256);
			yp1 = y_r(1080 - 108 + (int)SelYadd[2]);
			DrawFetteString(xp1, yp1, 1.f, (select == 2), "Option");

			//
			{
				xp1 = y_r(32);
				yp1 = y_r(1080 - 32 - 32);

				std::string Info = "";
				switch (select) {
				case 0:
					Info = "3���Ԃ̊ԓG��|�������Ă��������B�݌v���Đ��͂�����ł̂݃J�E���g����܂�";
					break;
				case 1:
					Info = "���Ԑ����̊ɂ����[�h�ł�";
					break;
				case 2:
					Info = "�I�v�V�������J���܂�";
					break;
				default:
					break;
				}

				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(18), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::BOTTOM, xp1, yp1, White, Black, Info);
			}


			{
				auto per = std::clamp((GameFadeIn), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			{
				auto per = std::clamp(std::max(GameStart, GameStart2), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(255, 255, 255), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
		}
	};
};
