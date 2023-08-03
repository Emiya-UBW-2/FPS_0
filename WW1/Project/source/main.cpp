#include"Header.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	OPTION::Create();
	auto* OptionParts = OPTION::Instance();
	OptionParts->Load();							//�ݒ�ǂݍ���
	DXDraw::Create("FPS_n2");						//�ėp
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
	auto StartMoviescene = std::make_shared<FPS_n2::Sceneclass::StartMovieScene>();
	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();
	//�V�[���R���g���[��
	auto scene = std::make_unique<SceneControl>(StartMoviescene);
	//�J�ڐ�w��
	StartMoviescene->Set_Next(MAINLOOPscene);
	MAINLOOPscene->Set_Next(StartMoviescene);
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

			if (!scene->isPause()) {
				EffectUseControl->Calc();			//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
			}
			scene->Draw();							//�`��
			//�f�o�b�O
#ifdef DEBUG
			DebugParts->DebugWindow(1920 - 300, 50);
#endif // DEBUG
			DrawParts->Screen_Flip();				//��ʂ̔��f
		}
		scene->NextScene();							//���̃V�[���ֈڍs
	}
	return 0;
}
