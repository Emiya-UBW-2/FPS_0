#include "DXLib_ref/DXLib_ref.h"
const int sock_send = 9851;
const int sock_get = 9850;
const char* name = "get";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	IPDATA Ip;        // ���M�p�h�o�A�h���X�f�[�^
	struct stat {
		int xpos = 100;
		int ypos = 100;
	};
	stat mine;
	stat yours;
	SetMainWindowText(name);
	ChangeWindowMode(TRUE);
	SetAlwaysRunFlag(TRUE);
	if (DxLib_Init() == -1)    // �c�w���C�u��������������
	{
		return -1;    // �G���[���N�����璼���ɏI��
	}


	int SendUDPHandle = MakeUDPSocket(-1);	// �t�c�o�ʐM�p�̃\�P�b�g�n���h�����쐬
	int GetUDPHandle = MakeUDPSocket(sock_get);	// �t�c�o�ʐM�p�̃\�P�b�g�n���h�����쐬
	// �h�o�A�h���X��ݒ�( �����ɂ���S�̂h�o�l�͉��ł� )
	Ip.d1 = 10;
	Ip.d2 = 19;
	Ip.d3 = 52;
	Ip.d4 = 125;
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	DrawString(0, 0, "��M�҂�", GetColor(255, 255, 255));	// ��M�҂�����ƕ\��
	int i = 18;
	while (ProcessMessage() == 0) {
		SetDrawScreen(DX_SCREEN_BACK);
		{
			if (CheckNetWorkRecvUDP(GetUDPHandle) == TRUE) {	// ������̎�M��҂�
				NetWorkRecvUDP(GetUDPHandle, NULL, NULL, &yours, sizeof(yours), FALSE);	// ��M
				DrawString(0, i, (std::to_string(yours.xpos) + "," + std::to_string(yours.ypos)).c_str(), GetColor(255, 255, 255));
				i += 18;
			}
			mine.xpos += 1;
			mine.ypos += 1;
		}
		ScreenFlip();

		NetWorkSendUDP(SendUDPHandle, Ip, sock_send, &mine, sizeof(mine));
		if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
			break;
		}
	}

	DeleteUDPSocket(SendUDPHandle);	// �t�c�o�\�P�b�g�n���h���̍폜
	DeleteUDPSocket(GetUDPHandle);	// �t�c�o�\�P�b�g�n���h���̍폜

	DxLib_End();    // �c�w���C�u�����g�p�̏I������
	return 0;    // �\�t�g�̏I��
}