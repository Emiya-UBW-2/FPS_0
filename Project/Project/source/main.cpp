#include"Header.hpp"

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
	auto* EffectUseControl = EffectResource::Instance();
	//シーン
	auto MAINLOOPloader = std::make_shared<FPS_n2::MAINLOOPLOADER>();
	
	auto MAINLOOPscene = std::make_shared<FPS_n2::MAINLOOP>();
	//シーンコントロール
	auto scene = std::make_unique<SceneControl>(MAINLOOPloader);
	//遷移先指定
	MAINLOOPloader->Set_Next(MAINLOOPscene);
	MAINLOOPscene->Set_Next(MAINLOOPloader);
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
			printfDx("AsyncCount :%d\n", GetASyncLoadNum());
			printfDx("Drawcall   :%d\n", GetDrawCallCount());
			printfDx("FPS        :%5.2f fps\n", FPS);
#endif // DEBUG
			DrawParts->Screen_Flip();				//画面の反映
		}
		scene->NextScene();							//次のシーンへ移行
	}
	return 0;
}
