#include "sub.hpp"
#include "UI.hpp"
#include "HostPass.hpp"
#include "map.hpp"
#include "VR.hpp"
#include "debug.hpp"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	//設定読み込み
	bool dof_e = false;
	bool bloom_e = false;
	bool shadow_e = false;
	bool useVR_e = true;
	int dispx = 1080; /*描画X*/
	int dispy = 1200; /*描画Y*/
	int out_dispx = dispx * 960 / dispy; /*ウィンドウX*/
	int out_dispy = dispy * 960 / dispy; /*ウィンドウY*/
	{
		SetOutApplicationLogValidFlag(FALSE);  /*log*/
		int mdata = FileRead_open("data/setting.txt", FALSE);
		dof_e = getparam_bool(mdata);
		bloom_e = getparam_bool(mdata);
		shadow_e = getparam_bool(mdata);
		useVR_e = getparam_bool(mdata);
		FileRead_close(mdata);
	}
	//DXLib描画
	auto vrparts = std::make_unique<VRDraw>(&useVR_e);
	//画面指定
	if (useVR_e) {
		dispx = 1080*2;
		dispy = 1200*2;
		out_dispx = dispx * 960 / dispy;
		out_dispy = dispy * 960 / dispy;
	}
	else {
		dispx = 1920;
		dispy = 1080;
		out_dispx = dispx;
		out_dispy = dispy;
	}
	//
	auto Drawparts = std::make_unique<DXDraw>("TankFlanker", dispx, dispy, 90.f);		 /*汎用クラス*/
	auto UIparts = std::make_unique<UI>(out_dispx, out_dispy, dispx, dispy);		 /*UI*/
	auto Debugparts = std::make_unique<DeBuG>(90);						 /*デバッグ*/
	auto Hostpassparts = std::make_unique<HostPassEffect>(dof_e, bloom_e, dispx, dispy);	 /*ホストパスエフェクト*/
	if (useVR_e) {
		SetWindowSize(out_dispx, out_dispy);
		SetWindowPosition((deskx - out_dispx) / 2, 0);
	}
	GraphHandle BufScreen = GraphHandle::Make(dispx, dispy);    //描画スクリーン
	GraphHandle outScreen = GraphHandle::Make(dispx, dispy);    //描画スクリーン
	//操作
	VECTOR_ref eyevec;					    //視点
	VECTOR_ref campos, camvec, camup;			    //カメラ
	float fov = deg2rad(useVR_e ? 90 : 45);
	bool ads = false;
	float range = 0.f;
	float range_p = 0.f;
	uint8_t change_vehicle = 0;
	bool chveh = false;
	//データ
	auto mapparts = std::make_unique<Mapclass>(dispx, dispy);
	std::vector<Mainclass::Gun> gun_data;
	gun_data.resize(1);
	MV1::Load("data/1911/model.mv1",&gun_data.back().obj,true);
	gun_data.back().frame.resize(1);
	gun_data.back().frame.back().first = -1;
	gun_data.back().frame.back().second = VGet(0, 0, 0);
	UIparts->load_window("モデル");					//ロード画面
	//VRセット
	vrparts->Set_Device();
	VECTOR_ref HMDpos, HMDxvec, HMDyvec, HMDzvec;
	bool HMDon;

	VECTOR_ref LHANDpos, LHANDxvec, LHANDyvec, LHANDzvec;
	bool LHANDon;

	//
	std::vector<Mainclass::Chara> chara;
	//ココから繰り返し読み込み//-------------------------------------------------------------------
	bool ending = true;
	do {
		//キャラ選択
		chara.resize(1);
		/*
		if (!UIparts->select_window(&chara[0], &Vehicles)) {
			break;
		}
		*/
		//マップ読み込み
		mapparts->set_map_pre();
		UIparts->load_window("マップモデル");			   //ロード画面
		//ライティング
		Drawparts->Set_light(VGet(0.0f, -0.5f, 0.5f));
		if (shadow_e) {
			Drawparts->Set_Shadow(13, VGet(10.f,10.f,10.f), VGet(0.0f, -0.5f, 0.5f), [] {});
		}
		//キャラ設定
		chara[0].gunptr = &gun_data[0];
		chara[0].obj = chara[0].gunptr->obj.Duplicate();
		chara[0].pos = VGet(0, 0, 0);
		chara[0].mat = MGetIdent();
		//影に描画するものを指定する(仮)
		auto draw_in_shadow = [] {};
		auto draw_in_shadow_sky = [] {};
		auto draw_on_shadow = [&mapparts, &chara, &campos, &vrparts] {
			//マップ
			SetFogStartEnd(0.0f, 3000.f);
			SetFogColor(128, 128, 128);
			{
				mapparts->map_get().DrawModel();
			}
			//海
			mapparts->sea_draw(campos);
			//機体
			SetFogStartEnd(0.0f, 3000.f);
			SetFogColor(128, 128, 128);
			{
				chara[0].obj.DrawModel();
			}
			//銃弾
			SetFogEnable(FALSE);
			SetUseLighting(FALSE);
			{

			}
			SetUseLighting(TRUE);
			SetFogEnable(TRUE);
			vrparts->Draw_Player();
		};
		//通信開始
		{
		}
		//開始
		//eyevec = mine.vehicle[mine.mode].mat.zvec() * -1.f;
		//campos = mine.vehicle[mine.mode].pos + VGet(0.f, 3.f, 0.f) + eyevec * (range);

		SetMouseDispFlag(FALSE);
		SetMousePoint(dispx / 2, dispy / 2);
		while (ProcessMessage() == 0) {
			const auto fps = GetFPS();
			const auto waits = GetNowHiPerformanceCount();
			Debugparts->put_way();
			{
				//プレイヤー操作
				{
					//マウスと視点角度をリンク
					if (useVR_e) {
						SetMousePoint(dispx / 2, dispy / 2);
						if (vrparts->get_hmd_num() != -1) {
							auto& ptr_HMD = (*vrparts->get_device())[vrparts->get_hmd_num()];
							HMDpos = ptr_HMD.pos;
							HMDxvec = ptr_HMD.xvec;
							HMDyvec = ptr_HMD.yvec;
							HMDzvec = ptr_HMD.zvec;
							HMDon = ptr_HMD.now;
						}
						else {
							HMDpos = VGet(0, 0, 0);
							HMDxvec = VGet(1, 0, 0);
							HMDyvec = VGet(0, 1, 0);
							HMDzvec = VGet(0, 0, 1);
							HMDon = false;
						}
						eyevec = VGet(HMDzvec.x(), HMDzvec.y()*-1.f, HMDzvec.z());
					}
					else {
						int mousex, mousey;
						GetMousePoint(&mousex, &mousey);
						SetMousePoint(dispx / 2, dispy / 2);
						if (false) {
							if (ads) {
								float y = atan2f(eyevec.x(), eyevec.z()) + deg2rad(float(mousex - dispx / 2) * 0.1f);
								float x = atan2f(eyevec.y(), std::hypotf(eyevec.x(), eyevec.z())) + deg2rad(float(mousey - dispy / 2) * 0.1f);
								x = std::clamp(x, deg2rad(-20), deg2rad(10));
								eyevec = VGet(cos(x) * sin(y), sin(x), cos(x) * cos(y));
							}
							else {
								float y = atan2f(eyevec.x(), eyevec.z()) + deg2rad(float(mousex - dispx / 2) * 0.1f);
								float x = atan2f(eyevec.y(), std::hypotf(eyevec.x(), eyevec.z())) + deg2rad(float(mousey - dispy / 2) * 0.1f);
								x = std::clamp(x, deg2rad(-25), deg2rad(89));
								eyevec = VGet(cos(x) * sin(y), sin(x), cos(x) * cos(y));
							}
						}
					}
					//
					{
						if (vrparts->get_left_hand_num() != -1) {
							auto& ptr_LHAND = (*vrparts->get_device())[vrparts->get_left_hand_num()];
							LHANDpos = ptr_LHAND.pos;
							LHANDxvec = MATRIX_ref::Vtrans(ptr_LHAND.xvec,MATRIX_ref::RotAxis(ptr_LHAND.xvec,deg2rad(60)));
							LHANDyvec = MATRIX_ref::Vtrans(ptr_LHAND.yvec, MATRIX_ref::RotAxis(ptr_LHAND.xvec, deg2rad(60)));
							LHANDzvec = MATRIX_ref::Vtrans(ptr_LHAND.zvec, MATRIX_ref::RotAxis(ptr_LHAND.xvec, deg2rad(60)));
							LHANDon = ptr_LHAND.now;
						}
						else {
							LHANDpos = VGet(0, 0, 0);
							LHANDxvec = VGet(1, 0, 0);
							LHANDyvec = VGet(0, 1, 0);
							LHANDzvec = VGet(0, 0, 1);
							LHANDon = false;
						}
					}
				}
				//共通
				{

				}
				//
				{
					//他の座標をここで出力(ホスト)
				}
				{
					//ホストからの座標をここで入力
				}
				//モデルに反映
				{
					chara[0].obj.SetMatrix(MATRIX_ref::Axis1(LHANDxvec*-1, LHANDyvec, LHANDzvec*-1)*MATRIX_ref::Mtrans(LHANDpos + VGet(100.f, 100.f, 100.f)));
				}
				//影用意
				if (shadow_e) {
					Drawparts->Ready_Shadow(campos, draw_in_shadow, VGet(50.f, 25.f, 50.f));
				}
				//VR空間に適用
				vrparts->Move_Player();
				//campos,camvec,camupの指定
				{
					campos = VGet(100.f, 100.f, 100.f);
					if (useVR_e) {
						campos += HMDpos;
						camvec = campos + HMDzvec;
						camup = HMDyvec;
					}
					else {
						camvec = campos + VGet(0, 0, 1.f);
						camup = VGet(0, 1.f, 0);
					}
				}
				//被写体深度描画
				{
					float fardist = 1000.f;
					float neardist = 1.5f;

					if (shadow_e) {
						Hostpassparts->dof(&BufScreen,
							mapparts->sky_draw(campos, camvec, camup, fov),
							[&Drawparts, &draw_on_shadow] { Drawparts->Draw_by_Shadow(draw_on_shadow); }, campos, camvec, camup, fov, fardist, neardist);
					}
					else {
						Hostpassparts->dof(&BufScreen,
							mapparts->sky_draw(campos, camvec, camup, fov),
							draw_on_shadow, campos, camvec, camup, fov, fardist, neardist);
					}
				}
				//
				GraphHandle::SetDraw_Screen(DX_SCREEN_BACK, 0.01f, 5000.0f, fov, campos, camvec, camup);
				//描画
				outScreen.SetDraw_Screen();
				{
					//背景
					BufScreen.DrawGraph(0, 0, false);
					//ブルーム
					Hostpassparts->bloom(BufScreen, 255);
					//UI
				}
				//VRに移す
				if (useVR_e) {
					GraphHandle::SetDraw_Screen(DX_SCREEN_BACK);
					outScreen.DrawGraph(0, 0, false);
					for (char i = 0; i < 2; i++) {
						vrparts->PutEye((ID3D11Texture2D*)GetUseDirect3D11BackBufferTexture2D(), i);
					}
				}
				GraphHandle::SetDraw_Screen(DX_SCREEN_BACK);
				{
					outScreen.DrawExtendGraph(0, 0, out_dispx, out_dispy, false);
					//デバッグ
					Debugparts->end_way();
					Debugparts->debug(10, 10, fps, float(GetNowHiPerformanceCount() - waits) / 1000.f);
				}

			}
			Drawparts->Screen_Flip();
			vrparts->Eye_Flip(waits);//フレーム開始の数ミリ秒前にstartするまでブロックし、レンダリングを開始する直前に呼び出す必要があります。
			if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
				ending = false;
				break;
			}
			if (CheckHitKey(KEY_INPUT_O) != 0) {
				break;
			}
		}
		//解放

		//
	} while (ProcessMessage() == 0 && ending);
	return 0; // ソフトの終了
}
