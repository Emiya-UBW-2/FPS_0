#include "DXLib_ref/DXLib_ref.h"
const int sock_send = 9851;
const int sock_get = 9850;
const char* name = "get";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	IPDATA Ip;        // 送信用ＩＰアドレスデータ
	struct stat {
		int xpos = 100;
		int ypos = 100;
	};
	stat mine;
	stat yours;
	SetMainWindowText(name);
	ChangeWindowMode(TRUE);
	SetAlwaysRunFlag(TRUE);
	if (DxLib_Init() == -1)    // ＤＸライブラリ初期化処理
	{
		return -1;    // エラーが起きたら直ちに終了
	}


	int SendUDPHandle = MakeUDPSocket(-1);	// ＵＤＰ通信用のソケットハンドルを作成
	int GetUDPHandle = MakeUDPSocket(sock_get);	// ＵＤＰ通信用のソケットハンドルを作成
	// ＩＰアドレスを設定( ここにある４つのＩＰ値は仮です )
	Ip.d1 = 10;
	Ip.d2 = 19;
	Ip.d3 = 52;
	Ip.d4 = 125;
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	DrawString(0, 0, "受信待ち", GetColor(255, 255, 255));	// 受信待ちだよと表示
	int i = 18;
	while (ProcessMessage() == 0) {
		SetDrawScreen(DX_SCREEN_BACK);
		{
			if (CheckNetWorkRecvUDP(GetUDPHandle) == TRUE) {	// 文字列の受信を待つ
				NetWorkRecvUDP(GetUDPHandle, NULL, NULL, &yours, sizeof(yours), FALSE);	// 受信
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

	DeleteUDPSocket(SendUDPHandle);	// ＵＤＰソケットハンドルの削除
	DeleteUDPSocket(GetUDPHandle);	// ＵＤＰソケットハンドルの削除

	DxLib_End();    // ＤＸライブラリ使用の終了処理
	return 0;    // ソフトの終了
}