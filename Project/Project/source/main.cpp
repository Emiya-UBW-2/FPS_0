#include"Header.hpp"
#include <WinUser.h>


bool GetClipBoardGraphHandle(GraphHandle* RetHandle) {
	HWND   hwnd = GetMainWindowHandle();
	// DIB�Z�N�V�����̎擾
	BITMAP DDBInfo;
	BITMAPINFO DIBInfo;

	if (IsClipboardFormatAvailable(CF_BITMAP) == FALSE) { return false; }	// �w�肵���`���̃f�[�^�����邩��₢���킹��
	if (OpenClipboard(hwnd) == FALSE) { return false; }						// �N���b�v�{�[�h�̃I�[�v��
	HBITMAP g_hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);				// �N���b�v�{�[�h�f�[�^�̎擾
	CloseClipboard();														// �N���b�v�{�[�h�̃N���[�Y
	if (g_hBitmap == NULL) { return false; }
	GetObject(g_hBitmap, sizeof(BITMAP), &DDBInfo);
	DIBInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	DIBInfo.bmiHeader.biWidth = DDBInfo.bmWidth;
	DIBInfo.bmiHeader.biHeight = DDBInfo.bmHeight;
	DIBInfo.bmiHeader.biPlanes = 1;
	DIBInfo.bmiHeader.biBitCount = 32;
	DIBInfo.bmiHeader.biCompression = BI_RGB;

	BYTE *pData = new BYTE[DDBInfo.bmWidth * DDBInfo.bmHeight * 4];
	HDC hDC = GetDC(hwnd);
	GetDIBits(hDC, g_hBitmap, 0, DDBInfo.bmHeight, (void*)pData, &DIBInfo, DIB_RGB_COLORS);
	ReleaseDC(hwnd, hDC);
	DeleteObject(g_hBitmap);

	// �\�t�g�C���[�W�ɕϊ����Ă݂�
	int sh = MakeXRGB8ColorSoftImage(DDBInfo.bmWidth, DDBInfo.bmHeight);
	BYTE *Dots = pData;
	for (int y = DDBInfo.bmHeight - 1; y >= 0; y--) { // �f�[�^�͏㉺�������܂炵��
		for (int x = 0; x < DDBInfo.bmWidth; x++) {
			DrawPixelSoftImage(sh, x, y, *(Dots + 2), *(Dots + 1), *(Dots + 0), *(Dots + 3));
			Dots += 4;
		}
	}
	int gh = CreateGraphFromBmp(&DIBInfo, pData);	// �n���h���ɕϊ����Ă݂�
	delete pData;									// ReCreateGraph����Ƃ��ɕK�v�����B
	*RetHandle = gh;
	return true;
}


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	OPTION::Create();
	SetValidMousePointerWindowOutClientAreaMoveFlag(FALSE);
	DXDraw::Create("FPS_n2");						//�ėp
	//MV1SetLoadModelUsePackDraw(TRUE);
	PostPassEffect::Create();						//�V�F�[�_�[
	EffectResource::Create();						//�G�t�F�N�g
	SoundPool::Create();							//�T�E���h
	FontPool::Create();								//�t�H���g
#ifdef DEBUG
	DebugClass::Create();
	auto* DebugParts = DebugClass::Instance();		//�f�o�b�O
#endif // DEBUG
	auto* DrawParts = DXDraw::Instance();
	//
	auto* EffectUseControl = EffectResource::Instance();
	//�V�[��
	auto MAINLOOPloader = std::make_shared<FPS_n2::MAINLOOPLOADER>();
	
	auto MAINLOOPscene = std::make_shared<FPS_n2::MAINLOOP>();
	//�V�[���R���g���[��
	auto scene = std::make_unique<SceneControl>(MAINLOOPloader);
	//�J�ڐ�w��
	MAINLOOPloader->Set_Next(MAINLOOPscene);
	MAINLOOPscene->Set_Next(MAINLOOPloader);

	GraphHandle ClipBoadBmp;
	bool IsGetClipBoadBmp = GetClipBoardGraphHandle(&ClipBoadBmp);
	if (IsGetClipBoadBmp) {
		int xx, yy;
		ClipBoadBmp.GetSize(&xx, &yy);
	}

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
			if (scene->Execute()) { break; }		//�X�V
			if (!scene->isPause()) {
				EffectUseControl->Calc();			//�G�t�F�N�V�A�̃A�v�f��60FPS�����ɕύX
			}
			scene->Draw();							//�`��
			//�f�o�b�O
#ifdef DEBUG
			DebugParts->DebugWindow(1920 - 300, 50);
#endif // DEBUG
			if (IsGetClipBoadBmp) {
				SetDrawScreen(DX_SCREEN_BACK);
				ClipBoadBmp.DrawGraph(100, 100, false);
			}
			DrawParts->Screen_Flip();				//��ʂ̔��f
		}
		scene->NextScene();							//���̃V�[���ֈڍs
	}
	return 0;
}
