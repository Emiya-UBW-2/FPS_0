#include"Header.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	OPTION::Create();
	auto* OptionParts = OPTION::Instance();
	OptionParts->Load();							//設定読み込み
	DXDraw::Create("FPS_n2");						//汎用

	//MV1SetLoadModelUsePackDraw(TRUE);

#ifdef DEBUG
	DebugClass::Create(FPS_n2::Frame_Rate);
#endif // DEBUG

	auto scene = std::make_unique<FPS_n2::Sceneclass::SceneControl>();
	//シェーダー
	PostPassEffect::Create();
	//シーン
	auto MAINLOOPscene = std::make_shared<FPS_n2::Sceneclass::MAINLOOP>();
	//
	auto* EffectUseControl = EffectResource::Instance();
	EffectUseControl->Init();																						//エフェクト
	//遷移先指定
	MAINLOOPscene->Set_Next(MAINLOOPscene, FPS_n2::Sceneclass::scenes::MAIN_LOOP);
	//開始時遷移先
	scene->ChangeScene(FPS_n2::Sceneclass::scenes::MAIN_LOOP, (std::shared_ptr<FPS_n2::Sceneclass::TEMPSCENE>&)MAINLOOPscene);
	//繰り返し
	do {
		scene->StartScene();
		while (true) {
			if (scene->Execute()) { break; }
			if (!scene->isPause()) {
				EffectUseControl->Calc();			//エフェクシアのアプデを60FPS相当に変更
			}
			scene->Draw();
			//60FPSを待機
			scene->Vsync();
		}
		//終了処理
		scene->NextScene();
	} while (!scene->isEnd());
	EffectUseControl->Dispose();
	return 0;
}
