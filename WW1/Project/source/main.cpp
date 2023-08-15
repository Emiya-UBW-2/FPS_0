#include"Header.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	LPCSTR font_path;

	font_path = "data/x14y24pxHeadUpDaisy.ttf"; // 読み込むフォントファイルのパス
	DESIGNVECTOR Font1;
	if (AddFontResourceEx(font_path, FR_PRIVATE, &Font1) == 0) {
		MessageBox(NULL, "フォント読込失敗", "", MB_OK);
	}

	font_path = "data/DSFetteGotisch.ttf"; // 読み込むフォントファイルのパス
	DESIGNVECTOR Font2;
	if (AddFontResourceEx(font_path, FR_PRIVATE, &Font2) == 0) {
		MessageBox(NULL, "フォント読込失敗", "", MB_OK);
	}

	OPTION::Create();
	auto* OptionParts = OPTION::Instance();
	OptionParts->Load();							//設定読み込み
	DXDraw::Create("FPS_n2");						//汎用

	FPS_n2::KeyGuideClass::Create();
	FPS_n2::SaveDataClass::Create();
	FPS_n2::PadControl::Create();
	FPS_n2::Sceneclass::OptionWindowClass::Create();
	FPS_n2::Sceneclass::OptionWindowClass::Instance()->Init();
	

	FPS_n2::SaveDataClass::Instance()->Load();
	//MV1SetLoadModelUsePackDraw(TRUE);
#ifdef DEBUG
	DebugClass::Create();
	auto* DebugParts = DebugClass::Instance();		//デバッグ
#endif // DEBUG
	PostPassEffect::Create();						//シェーダー
	EffectResource::Create();						//エフェクト
	SoundPool::Create();							//サウンド
	auto* DrawParts = DXDraw::Instance();
	auto* EffectUseControl = EffectResource::Instance();
	//auto* SE = SoundPool::Instance();
	//シーン
	auto Titlescene = std::make_shared<FPS_n2::Sceneclass::TitleScene>();
	auto StartMoviescene = std::make_shared<FPS_n2::Sceneclass::StartMovieScene>();
	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();
	//シーンコントロール
	auto scene = std::make_unique<SceneControl>(Titlescene);
	//遷移先指定
	Titlescene->Set_Next(StartMoviescene);
	StartMoviescene->Set_Next(MAINLOOPscene);
	MAINLOOPscene->Set_Next(Titlescene);
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
#ifdef DEBUG
			DebugParts->SetPoint("Execute start");
#endif // DEBUG
			if (scene->Execute()) { break; }		//更新
			FPS_n2::Sceneclass::OptionWindowClass::Instance()->Execute();

			if (!scene->isPause()) {
				EffectUseControl->Calc();			//エフェクシアのアプデを60FPS相当に変更
			}
			else {
				EffectUseControl->Update_effect_f = false;
			}
			scene->Draw();							//描画
			FPS_n2::KeyGuideClass::Instance()->Draw();
			FPS_n2::Sceneclass::OptionWindowClass::Instance()->Draw();
			//デバッグ
#ifdef DEBUG
			DebugParts->DebugWindow(1920 - 300, 50);
#else
			{
				auto* Fonts = FontPool::Instance();
				Fonts->Get(FontPool::FontType::HUD_Edge).DrawString(y_r(18), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, y_r(1920 - 8), y_r(8), GetColor(255, 255, 255), GetColor(0, 0, 0), "%5.2f FPS", FPS);
			}
#endif // DEBUG
			DrawParts->Screen_Flip();				//画面の反映
		}
		FPS_n2::SaveDataClass::Instance()->Save();
		FPS_n2::KeyGuideClass::Instance()->Reset();
		scene->NextScene();							//次のシーンへ移行
	}

	font_path = "data/x14y24pxHeadUpDaisy.ttf"; // 読み込むフォントファイルのパス
	if (RemoveFontResourceEx(font_path, FR_PRIVATE, &Font1) == 0) {
		MessageBox(NULL, "フォント読込失敗", "", MB_OK);
	}

	font_path = "data/DSFetteGotisch.ttf"; // 読み込むフォントファイルのパス
	if (RemoveFontResourceEx(font_path, FR_PRIVATE, &Font2) == 0) {
		MessageBox(NULL, "フォント読込失敗", "", MB_OK);
	}

	return 0;
}
