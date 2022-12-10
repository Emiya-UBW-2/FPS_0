#include"Header.hpp"
#include <WinUser.h>


bool GetClipBoardGraphHandle(GraphHandle* RetHandle) {
	HWND   hwnd = GetMainWindowHandle();
	// DIBセクションの取得
	BITMAP DDBInfo;
	BITMAPINFO DIBInfo;

	if (IsClipboardFormatAvailable(CF_BITMAP) == FALSE) { return false; }	// 指定した形式のデータがあるかを問い合わせる
	if (OpenClipboard(hwnd) == FALSE) { return false; }						// クリップボードのオープン
	HBITMAP g_hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);				// クリップボードデータの取得
	CloseClipboard();														// クリップボードのクローズ
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

	// ソフトイメージに変換してみる
	int sh = MakeXRGB8ColorSoftImage(DDBInfo.bmWidth, DDBInfo.bmHeight);
	BYTE *Dots = pData;
	for (int y = DDBInfo.bmHeight - 1; y >= 0; y--) { // データは上下さかさまらしい
		for (int x = 0; x < DDBInfo.bmWidth; x++) {
			DrawPixelSoftImage(sh, x, y, *(Dots + 2), *(Dots + 1), *(Dots + 0), *(Dots + 3));
			Dots += 4;
		}
	}
	int gh = CreateGraphFromBmp(&DIBInfo, pData);	// ハンドルに変換してみる
	delete pData;									// ReCreateGraphするときに必要かも。
	*RetHandle = gh;
	return true;
}


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	OPTION::Create();
	SetValidMousePointerWindowOutClientAreaMoveFlag(FALSE);
	DXDraw::Create("FPS_n2");						//汎用
	//MV1SetLoadModelUsePackDraw(TRUE);
	PostPassEffect::Create();						//シェーダー
	EffectResource::Create();						//エフェクト
	SoundPool::Create();							//サウンド
	FontPool::Create();								//フォント
#ifdef DEBUG
	DebugClass::Create();
	auto* DebugParts = DebugClass::Instance();		//デバッグ
#endif // DEBUG
	auto* DrawParts = DXDraw::Instance();
	//
	auto* EffectUseControl = EffectResource::Instance();
	//シーン
	auto MAINLOOPloader = std::make_shared<FPS_n2::MAINLOOPLOADER>();
	
	auto MAINLOOPscene = std::make_shared<FPS_n2::MAINLOOP>();
	//シーンコントロール
	auto scene = std::make_unique<SceneControl>(MAINLOOPloader);
	//遷移先指定
	MAINLOOPloader->Set_Next(MAINLOOPscene);
	MAINLOOPscene->Set_Next(MAINLOOPloader);

	GraphHandle ClipBoadBmp;
	bool IsGetClipBoadBmp = GetClipBoardGraphHandle(&ClipBoadBmp);
	if (IsGetClipBoadBmp) {
		int xx, yy;
		ClipBoadBmp.GetSize(&xx, &yy);
	}

	//繰り返し
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
			if (scene->Execute()) { break; }		//更新
			if (!scene->isPause()) {
				EffectUseControl->Calc();			//エフェクシアのアプデを60FPS相当に変更
			}
			scene->Draw();							//描画
			//デバッグ
#ifdef DEBUG
			DebugParts->DebugWindow(1920 - 300, 50);
#endif // DEBUG
			if (IsGetClipBoadBmp) {
				SetDrawScreen(DX_SCREEN_BACK);
				ClipBoadBmp.DrawGraph(100, 100, false);
			}
			DrawParts->Screen_Flip();				//画面の反映
		}
		scene->NextScene();							//次のシーンへ移行
	}
	return 0;
}
