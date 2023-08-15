#include"Header.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	LPCSTR font_path;

	font_path = "data/x14y24pxHeadUpDaisy.ttf"; // �ǂݍ��ރt�H���g�t�@�C���̃p�X
	DESIGNVECTOR Font1;
	if (AddFontResourceEx(font_path, FR_PRIVATE, &Font1) == 0) {
		MessageBox(NULL, "�t�H���g�Ǎ����s", "", MB_OK);
	}

	font_path = "data/DSFetteGotisch.ttf"; // �ǂݍ��ރt�H���g�t�@�C���̃p�X
	DESIGNVECTOR Font2;
	if (AddFontResourceEx(font_path, FR_PRIVATE, &Font2) == 0) {
		MessageBox(NULL, "�t�H���g�Ǎ����s", "", MB_OK);
	}

	OPTION::Create();
	auto* OptionParts = OPTION::Instance();
	OptionParts->Load();							//�ݒ�ǂݍ���
	DXDraw::Create("FPS_n2");						//�ėp

	FPS_n2::KeyGuideClass::Create();
	FPS_n2::SaveDataClass::Create();
	FPS_n2::PadControl::Create();
	FPS_n2::Sceneclass::OptionWindowClass::Create();
	FPS_n2::Sceneclass::OptionWindowClass::Instance()->Init();
	

	FPS_n2::SaveDataClass::Instance()->Load();
	//MV1SetLoadModelUsePackDraw(TRUE);
#ifdef DEBUG
	DebugClass::Create();
	auto* DebugParts = DebugClass::Instance();		//�f�o�b�O
#endif // DEBUG
	PostPassEffect::Create();						//�V�F�[�_�[
	EffectResource::Create();						//�G�t�F�N�g
	SoundPool::Create();							//�T�E���h
	auto* DrawParts = DXDraw::Instance();
	auto* EffectUseControl = EffectResource::Instance();
	//auto* SE = SoundPool::Instance();
	//�V�[��
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto StartMoviescene = std::make_shared<FPS_n2::Sceneclass::StartMovieScene>();
	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();
	//�V�[���R���g���[��
	auto scene = std::make_unique<SceneControl>(Titlescene);
	//�J�ڐ�w��
	Titlescene->Set_Next(StartMoviescene);
	StartMoviescene->Set_Next(MAINLOOPscene);
	MAINLOOPscene->Set_Next(Titlescene);
	//�J��Ԃ�
	while (true) {
		scene->StartScene();
		while (true) {
			if ((ProcessMessage() != 0) || (CheckHitKeyWithCheck(KEY_INPUT_ESCAPE) != 0)) {
				return 0;
			}
			FPS = GetFPS();
#ifdef DEBUG
			clsDx();
			DebugParts->SetStartPoint();
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("Execute start");
#endif // DEBUG
			if (scene->Execute()) { break; }		//�X�V
			FPS_n2::Sceneclass::OptionWindowClass::Instance()->Execute();

			if (!scene->isPause()) {
				EffectUseControl->Calc();			//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
			}
			else {
				EffectUseControl->Update_effect_f = false;
			}
			scene->Draw();							//�`��
			FPS_n2::KeyGuideClass::Instance()->Draw();
			FPS_n2::Sceneclass::OptionWindowClass::Instance()->Draw();
			//�f�o�b�O
#ifdef DEBUG
			DebugParts->DebugWindow(1920 - 300, 50);
#else
			{
				auto* Fonts = FontPool::Instance();
				Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, y_r(1920 - 8), y_r(8), GetColor(255, 255, 255), GetColor(0, 0, 0), "%5.2f FPS", FPS);
			}
#endif // DEBUG
			DrawParts->Screen_Flip();				//��ʂ̔��f
		}
		FPS_n2::SaveDataClass::Instance()->Save();
		FPS_n2::KeyGuideClass::Instance()->Reset();
		scene->NextScene();							//���̃V�[���ֈڍs
	}

	font_path = "data/x14y24pxHeadUpDaisy.ttf"; // �ǂݍ��ރt�H���g�t�@�C���̃p�X
	if (RemoveFontResourceEx(font_path, FR_PRIVATE, &Font1) == 0) {
		MessageBox(NULL, "�t�H���g�Ǎ����s", "", MB_OK);
	}

	font_path = "data/DSFetteGotisch.ttf"; // �ǂݍ��ރt�H���g�t�@�C���̃p�X
	if (RemoveFontResourceEx(font_path, FR_PRIVATE, &Font2) == 0) {
		MessageBox(NULL, "�t�H���g�Ǎ����s", "", MB_OK);
	}

	return 0;
}
