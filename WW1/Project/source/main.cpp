#include"Header.hpp"

#include "Enums.hpp"
#include "MainScene/Object/ObjectBaseEnum.hpp"
#include "MainScene/Object/AmmoEnum.hpp"
#include "MainScene/Object/CharacterEnum.hpp"
//
#include "sub.hpp"
#include "NetWork.hpp"
//
#include "MainScene/BackGround/BackGroundSub.hpp"
#include "MainScene/BackGround/BackGround.hpp"

#include "MainScene/Object/AmmoData.hpp"

#include "MainScene/Object/ObjectBase.hpp"
#include "MainScene/Object/Ammo.hpp"
#include "MainScene/Object/Character_before.hpp"
#include "MainScene/Object/Character.hpp"

#include "ObjectManager.hpp"
#include "MainScene/Player/Player.hpp"
#include "MainScene/Player/CPU.hpp"

#include "MainScene/UI/MainSceneUIControl.hpp"
//
#include "MainScene/NetworkBrowser.hpp"
#include "Scene/TitleScene.hpp"
#include "Scene/StartMovieScene.hpp"
#include "Scene/MainScene.hpp"


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	DXDraw::Create("FPS_n2");						//汎用
	PostPassEffect::Create();						//シェーダー
#ifdef DEBUG
	auto* DebugParts = DebugClass::Instance();		//デバッグ
#endif // DEBUG
	auto* DrawParts = DXDraw::Instance();
	auto* EffectUseControl = EffectResource::Instance();
	//auto* SE = SoundPool::Instance();
	//
	FPS_n2::KeyGuideClass::Create();
	FPS_n2::SaveDataClass::Create();
	FPS_n2::PadControl::Create();
	FPS_n2::Sceneclass::OptionWindowClass::Create();
	FPS_n2::Sceneclass::OptionWindowClass::Instance()->Init();
	FPS_n2::SaveDataClass::Instance()->Load();

	auto* Pad = FPS_n2::PadControl::Instance();
	//MV1SetLoadModelUsePackDraw(TRUE);
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
		Pad->SetGuideUpdate();
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
			EffectUseControl->Calc(scene->isPause());//エフェクシアのアプデを60FPS相当に変更
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
	return 0;
}
