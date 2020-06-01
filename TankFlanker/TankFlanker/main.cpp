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
	switchs TPS;
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
		dispx = 1080;
		dispy = 1200;
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
	GraphHandle ScopeScreen = GraphHandle::Make(1080,1080);    //描画スクリーン
	GraphHandle bufScreen = GraphHandle::Make(dispx, dispy, true);
	std::array<GraphHandle, 3> outScreen;
	outScreen[0] = GraphHandle::Make(dispx, dispy);    //描画スクリーン
	outScreen[1] = GraphHandle::Make(dispx, dispy);    //描画スクリーン
	outScreen[2] = GraphHandle::Make(dispx, dispy);    //描画スクリーン
	//
	FontHandle font36 = FontHandle::Create(y_r(36, out_dispy), DX_FONTTYPE_EDGE);
	FontHandle font18 = FontHandle::Create(y_r(18, out_dispy), DX_FONTTYPE_EDGE);
	FontHandle font12 = FontHandle::Create(y_r(12, out_dispy), DX_FONTTYPE_EDGE);

	//操作
	VECTOR_ref eyevec;					    //視点
	VECTOR_ref campos, camvec, camup;			    //カメラ
	VECTOR_ref campos_buf;					    //視点
	float fov = deg2rad(useVR_e ? 90 : 45);
	//データ
	MV1 hand;
	MV1::Load("data/model/hand/model_h.mv1", &hand, true);
	auto mapparts = std::make_unique<Mapclass>(dispx, dispy);

	int tgt_col = LoadSoftImage("data/model/tgt/point.bmp");
	MV1 tgt;
	MV1::Load("data/model/tgt/model.mv1", &tgt, true);
	GraphHandle tgt_pic_tmp = GraphHandle::Load("data/model/tgt/Target-A2.png");
	int tgt_pic_x = 0;
	int tgt_pic_y = 0;
	frames tgt_f;
	struct tgts{
		MV1 obj;
		GraphHandle pic;
		int xf, yf;

		float rad = 0.f;
		float radadd = 0.f;
		float time = 0.f;
		float power = 0.f;
		bool LR = true;
	};
	std::vector<tgts> tgt_pic;
	int tgt_pic_sel=-1;
	float tgt_pic_on = 1.f;
	//
	//
	std::vector<Mainclass::Gun> gun_data;
	gun_data.resize(3);
	gun_data[0].name = "1911";
	gun_data[1].name = "M82A2";//"1911";
	gun_data[2].name = "CAR15_M4";
	for (auto& g : gun_data) {
		MV1::Load("data/gun/" + g.name + "/model.mv1", &g.obj, true);
		MV1::Load("data/gun/" + g.name + "/mag.mv1", &g.mag, true);
		MV1::Load("data/gun/" + g.name + "/ammo.mv1", &g.ammo, true);
	}
	UIparts->load_window("モデル");					//ロード画面
	//データ取得
	for (auto& g : gun_data) {
		g.frame.resize(7);
		g.frame[4].first = INT_MAX;
		for (int i = 0; i < g.obj.frame_num(); i++) {
			std::string s = g.obj.frame_name(i);
			if (s.find("mag_fall") != std::string::npos) {
				g.frame[0].first = i;//排莢
				g.frame[0].second = MATRIX_ref::Vtrans(VGet(0, 0, 0), g.obj.GetFrameLocalMatrix(g.frame[0].first));//mag
				g.frame[1].first = i + 1;
				g.frame[1].second = MATRIX_ref::Vtrans(VGet(0, 0, 0), g.obj.GetFrameLocalMatrix(g.frame[1].first));//mag先
			}
			else if (s.find("case") != std::string::npos) {
				g.frame[2].first = i;//排莢
			}
			else if (s.find("mazzule") != std::string::npos) {
				g.frame[3].first = i;//マズル
			}
			else if (s.find("scope") != std::string::npos) {
				g.frame[4].first = i;//スコープ
			}
			else if (s.find("trigger") != std::string::npos) {
				g.frame[5].first = i+1;//スコープ
			}
			else if (s.find("LEFT") != std::string::npos) {
				g.frame[6].first = i;//左手
			}
		}
		int mdata = FileRead_open(("data/gun/" + g.name + "/data.txt").c_str(), FALSE);
		g.ammo_max = getparam_i(mdata);
		while (true) {
			auto p = getparam_str(mdata);
			if (getright(p.c_str()).find("end") != std::string::npos) {
				break;
			}
			else if (getright(p.c_str()).find("semi") != std::string::npos) {
				g.select.emplace_back(1u);					//セミオート=1
			}
			else if (getright(p.c_str()).find("full") != std::string::npos) {
				g.select.emplace_back(2u);					//フルオート=2
			}
			else if (getright(p.c_str()).find("3b") != std::string::npos) {
				g.select.emplace_back(3u);					//3連バースト=3
			}
			else if (getright(p.c_str()).find("2b") != std::string::npos) {
				g.select.emplace_back(4u);					//2連バースト=4
			}
			else {
				g.select.emplace_back(1u);
			}
		}
		SetCreate3DSoundFlag(TRUE);
		g.audio.shot = SoundHandle::Load("data/audio/shot_" + getparam_str(mdata) + ".wav");
		g.audio.slide = SoundHandle::Load("data/audio/slide_" + getparam_str(mdata) + ".wav");
		g.audio.trigger = SoundHandle::Load("data/audio/trigger_" + getparam_str(mdata) + ".wav");
		g.audio.mag_down = SoundHandle::Load("data/audio/mag_down_" + getparam_str(mdata) + ".wav");
		g.audio.mag_set = SoundHandle::Load("data/audio/mag_set_" + getparam_str(mdata) + ".wav");
		g.audio.case_down = SoundHandle::Load("data/audio/case_" + getparam_str(mdata) + ".wav");
		SetCreate3DSoundFlag(FALSE);
		FileRead_close(mdata);
	}
	//
	tgt_f = { 2,tgt.frame(2) };
	GetGraphSize(tgt_pic_tmp.get(), &tgt_pic_x, &tgt_pic_y);
	//VRセット
	vrparts->Set_Device();
	MATRIX_ref HMDmat;
	bool HMDon;
	//
	std::vector<Mainclass::Chara> chara;
	//ココから繰り返し読み込み//-------------------------------------------------------------------
	bool ending = true;
	do {
		//キャラ設定
		chara.resize(1);
		{
			int sel_g=0;
			{
				float start_fl = 0.f;
				camvec = VGet(0.f, 0.f, 0.f);
				uint8_t upct = 0, dnct = 0, rtct = 0, ltct = 0, changecnt = 0;
				bool endp = false;
				bool startp = false;
				float rad = 0.f;
				float yrad_m = deg2rad(90), xrad_m = 0.f;
				int m_x = 0, m_y = 0;
				float ber_r = 0.f;
				GetMousePoint(&m_x, &m_y);
				while (ProcessMessage() == 0) {
					const auto fps = GetFPS();
					const auto waits = GetNowHiPerformanceCount();

					if (useVR_e) {
						if (vrparts->get_hmd_num() != -1) {
							auto& ptr_HMD = (*vrparts->get_device())[vrparts->get_hmd_num()];
							HMDmat = MATRIX_ref::Axis1(ptr_HMD.xvec*-1.f, ptr_HMD.yvec, ptr_HMD.zvec*-1.f);
						}
						else {
							HMDmat = MATRIX_ref::Axis1(VGet(-1, 0, 0), VGet(0, 1, 0), VGet(0, 0, -1));
						}
					}
					else {
						HMDmat = MATRIX_ref::Axis1(VGet(-1, 0, 0), VGet(0, 1, 0), VGet(0, 0, -1));
					}
					if (useVR_e) {
						if (vrparts->get_left_hand_num() != -1) {
							auto& ptr_LEFTHAND = (*vrparts->get_device())[vrparts->get_left_hand_num()];
							//auto& ptr_RIGHTHAND = (*vrparts->get_device())[vrparts->get_right_hand_num()];
							if (ptr_LEFTHAND.turn && ptr_LEFTHAND.now) {
								if (!startp) {
									changecnt = std::clamp<uint8_t>(changecnt + 1, 0, (((ptr_LEFTHAND.on[1] & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_IndexController_A)) != 0) ? 2 : 0));
									if (changecnt == 1) {
										++sel_g %= gun_data.size();
										ber_r = 0.f;
									}
									//引き金
									if ((ptr_LEFTHAND.on[0] & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger)) != 0) {
										startp = true;
									}
								}
							}
						}
					}



					if (!startp) {
						upct = std::clamp<uint8_t>(upct + 1, 0, ((CheckHitKey(KEY_INPUT_D) != 0) ? 2 : 0));
						dnct = std::clamp<uint8_t>(dnct + 1, 0, ((CheckHitKey(KEY_INPUT_A) != 0) ? 2 : 0));
						ltct = std::clamp<uint8_t>(ltct + 1, 0, ((CheckHitKey(KEY_INPUT_S) != 0) ? 2 : 0));
						rtct = std::clamp<uint8_t>(rtct + 1, 0, ((CheckHitKey(KEY_INPUT_W) != 0) ? 2 : 0));
						changecnt = std::clamp<uint8_t>(changecnt + 1, 0, ((CheckHitKey(KEY_INPUT_P) != 0) ? 2 : 0));
						if (changecnt == 1) {
							++sel_g %= gun_data.size();
							ber_r = 0.f;
						}
					}
					else {
						upct = 0;
						dnct = 0;
						ltct = 0;
						rtct = 0;
						changecnt = 0;
					}

					easing_set(&ber_r, float(out_dispy / 4), 0.95f, fps);

					{
						if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
							startp = true;
						}
						if (!startp) {
							//(MATRIX_ref::RotY(yrad_m) * MATRIX_ref::RotX(xrad_m))
							campos = HMDmat.zvec() * 0.6f;
							camvec = VGet(0, 0, 0);
							if (upct == 1) {
							}
							if (dnct == 1) {
							}
							if (ltct == 1) {
							}
							if (rtct == 1) {
							}
						}
						else {
							easing_set(&campos, VGet(1.f, 0.f, 0.f), 0.95f, fps);
							camvec = VGet(0, 0, 0);
							start_fl += 1.f / fps;
							if (start_fl > 3.f) {
								endp = true;
							}
						}
					}
					//VR空間に適用
					vrparts->Move_Player();
					{
						bufScreen.SetDraw_Screen();
						auto& v = gun_data[sel_g];
						{
							int xp = dispx / 2 - dispy / 6;
							int yp = dispy / 2 - dispy / 6;
							font18.DrawStringFormat(xp, yp, GetColor(0, 255, 0), "Name  :%s", v.name.c_str());
						}
						outScreen[0].SetDraw_Screen(0.1f, 10.f, fov, campos, camvec, VGet(0.f, 1.f, 0.f));
						{
							v.obj.DrawModel();
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(255 - int(255.f * start_fl / 1.f), 0, 255));
							bufScreen.DrawExtendGraph(0, 0, dispx, dispy, true);
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f * start_fl / 3.f), 0, 255));
							DrawBox(0, 0, dispx, dispy, GetColor(255, 255, 255), TRUE);
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						}
						if (useVR_e) {
							for (char i = 0; i < 2; i++) {
								GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
								outScreen[0].DrawGraph(0, 0, false);
								vrparts->PutEye((ID3D11Texture2D*)GetUseDirect3D11BackBufferTexture2D(), i);
							}
						}
						GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
						{
							outScreen[0].DrawExtendGraph(0, 0, out_dispx, out_dispy, true);
						}
					}
					Drawparts->Screen_Flip();
					vrparts->Eye_Flip(waits);//フレーム開始の数ミリ秒前にstartするまでブロックし、レンダリングを開始する直前に呼び出す必要があります。
					if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
						sel_g = -1;
						break;
					}
					if (endp) {
						break;
					}
				}
			}
			if (sel_g >= 0) {
				chara[0].set_chara(VGet(0, 0, 0), &gun_data[sel_g], ScopeScreen, hand);
			}
			else {
				ending = false;
				break;
			}
		}
		//マップ読み込み
		mapparts->set_map_pre();
		UIparts->load_window("マップモデル");			   //ロード画面
		//ターゲット
		{
			tgt_pic.resize(4);
			//tgt_pic.resize(tgt_pic.size() + 1);
			for (auto& p : tgt_pic) {
				p.obj = tgt.Duplicate();
				p.pic = GraphHandle::Make(tgt_pic_x, tgt_pic_y);
				p.pic.SetDraw_Screen(false);
				{
					tgt_pic_tmp.DrawGraph(0, 0, true);
				}
				p.obj.SetupCollInfo(8, 8, 8, 0, 1);
				p.obj.SetTextureGraphHandle(2, p.pic, false);
				p.xf = 4;
				p.yf = 3;
				p.LR = true;
			}
			{
				VECTOR_ref pos = VGet(4, 0, 10.f);
				tgt_pic[0].obj.SetPosition(mapparts->map_col_line(pos - VGet(0, -10.f, 0), pos - VGet(0, 10.f, 0), 0).HitPosition);
			}
			{
				VECTOR_ref pos = VGet(-2, 0, 20.f);
				tgt_pic[1].obj.SetPosition(mapparts->map_col_line(pos - VGet(0, -10.f, 0), pos - VGet(0, 10.f, 0), 0).HitPosition);
			}
			{
				VECTOR_ref pos = VGet(0, 0, 30.f);
				tgt_pic[2].obj.SetPosition(mapparts->map_col_line(pos - VGet(0, -10.f, 0), pos - VGet(0, 10.f, 0), 0).HitPosition);
			}
			{
				VECTOR_ref pos = VGet(2, 0, 45.f);
				tgt_pic[3].obj.SetPosition(mapparts->map_col_line(pos - VGet(0, -10.f, 0), pos - VGet(0, 10.f, 0), 0).HitPosition);
			}
		}
		//ライティング
		{
			VECTOR_ref light = VGet(0.05f, -0.5f, 0.75f);
			Drawparts->Set_light(light);
			if (shadow_e) {
				Drawparts->Set_Shadow(14, VGet(50.f, 20.f, 50.f), light, [&mapparts] {mapparts->map_get().DrawModel(); });
			}
		}
		//影に描画するものを指定する(仮)
		auto draw_in_shadow = [&tgt_pic, &chara] {
			for (auto& p : tgt_pic) {
				p.obj.DrawModel();
			}
			for (auto& c : chara) {
				c.hand.DrawModel();
				c.mag.DrawModel();
				c.obj.DrawModel();
				for (auto& a : c.ammo) {
					if (a.cnt >= 0.f) {
						a.second.DrawModel();
					}
				}
			}
		};
		auto draw_on_shadow = [&mapparts, &tgt_pic, &chara, &campos, &vrparts] {
			SetFogStartEnd(0.0f, 3000.f);
			SetFogColor(128, 128, 128);
			//マップ
			{
				mapparts->map_get().DrawModel();
				for (auto& p : tgt_pic) {
					p.obj.DrawModel();
				}
			}
			//機体
			for (auto& c : chara) {
				c.hand.DrawModel();
				c.mag.DrawModel();
				c.obj.DrawModel();
				for (auto& a : c.ammo) {
					if (a.cnt >= 0.f) {
						a.second.DrawModel();
					}
				}
			}
			/*
			{
				auto& c = chara[0];
				VECTOR_ref u_ = c.pos_LHAND - c.hand.frame(c.frame_hand[0].first);
				VECTOR_ref v_ = MATRIX_ref::Vtrans(VGet(0, -1.f, 0), MATRIX_ref::RotVec2(VGet(u_.x(), 0, u_.z()), u_));

				float dist_0 = u_.size();
				float dist_1 = (c.frame_hand[1].second).size();
				float dist_2 = (c.frame_hand[2].second).size();
				float cos_t = -std::clamp(getcos_tri(dist_2, dist_0, dist_1), -1.f, 1.f);
				float sin_t = -std::sqrtf(1.f - std::powf(cos_t, 2.f));

				DrawLine3D(c.hand.frame(c.frame_hand[0].first).get(), (c.hand.frame(c.frame_hand[0].first) + ((u_.Norm()*cos_t + v_ * sin_t)*dist_1)).get(), GetColor(0, 255, 0));


				DrawLine3D(c.hand.frame(c.frame_hand[0].first).get(), (c.hand.frame(c.frame_hand[0].first) + u_.Norm()).get(), GetColor(255, 0, 0));
				DrawLine3D(c.hand.frame(c.frame_hand[0].first).get(), (c.hand.frame(c.frame_hand[0].first) + v_.Norm()).get(), GetColor(0, 0, 255));
			}
			*/
			//銃弾
			SetFogEnable(FALSE);
			SetUseLighting(FALSE);
			for (auto& c : chara) {
				for (auto& a : c.bullet) {
					if (a.flug) {
						DXDraw::Capsule3D(a.pos, a.repos, ((a.spec.caliber_a - 0.00762f) * 0.1f + 0.00762f), a.color, GetColor(255, 255, 255));
					}
				}
			}
			SetUseLighting(TRUE);
			SetFogEnable(TRUE);
		};
		//通信開始
		{
		}
		//開始
		{
			auto& mine = chara[0];
			int point = 0;
			int p_up = 0;
			int p_down = 0;
			while (ProcessMessage() == 0) {
				const auto fps = GetFPS();
				const auto waits = GetNowHiPerformanceCount();
				Debugparts->put_way();
				{
					//プレイヤー操作
					{
						//マウスと視点角度をリンク
						if (useVR_e) {
							if (vrparts->get_hmd_num() != -1) {
								auto& ptr_HMD = (*vrparts->get_device())[vrparts->get_hmd_num()];
								mine.pos_HMD = ptr_HMD.pos;
								HMDmat = MATRIX_ref::Axis1(ptr_HMD.xvec*-1.f, ptr_HMD.yvec, ptr_HMD.zvec*-1.f);
								HMDon = ptr_HMD.now;
							}
							else {
								mine.pos_HMD = VGet(0, 1, 0);
								HMDmat = MATRIX_ref::Axis1(VGet(-1, 0, 0), VGet(0, 1, 0), VGet(0, 0, -1));
								HMDon = false;
							}
						}
						else {
							mine.pos_HMD = VGet(0, 1.f, 0);
							HMDmat = MATRIX_ref::Axis1(VGet(-1, 0, 0), VGet(0, 1, 0), VGet(0, 0, -1));
							HMDon = false;
						}
						mine.pos = VGet(0.f, 0.f, 0.f);
						//
						{
							if (useVR_e) {
								bool LHANDon;
								if (vrparts->get_left_hand_num() != -1) {
									auto& ptr_LHAND = (*vrparts->get_device())[vrparts->get_left_hand_num()];
									mine.pos_LHAND = ptr_LHAND.pos;
									mine.mat_LHAND =
										MATRIX_ref::RotVec2(VGet(0, 0, 1.f), mine.vecadd_LHAND)*
										MATRIX_ref::Axis1(ptr_LHAND.xvec*-1.f, ptr_LHAND.yvec, ptr_LHAND.zvec*-1.f)*MATRIX_ref::RotAxis(ptr_LHAND.xvec, deg2rad(60));
									LHANDon = ptr_LHAND.now;
								}
								else {
									mine.pos_LHAND = VGet(0, 1, 0);
									mine.mat_LHAND = MATRIX_ref::Axis1(VGet(-1, 0, 0), VGet(0, 1, 0), VGet(0, 0, -1));
									LHANDon = false;
								}
							}
							else {
								mine.pos_LHAND = VGet(0.f, 0.95f, 0.75f);
								mine.mat_LHAND = MATRIX_ref::Axis1(VGet(-1, 0, 0), VGet(0, 1, 0), VGet(0, 0, -1));
							}
							{
								MATRIX_ref RHANDmat;
								bool RHANDon;
								if (vrparts->get_right_hand_num() != -1) {
									auto& ptr_RHAND = (*vrparts->get_device())[vrparts->get_right_hand_num()];
									mine.pos_RHAND = ptr_RHAND.pos;
									RHANDmat = MATRIX_ref::Axis1(ptr_RHAND.xvec, ptr_RHAND.yvec, ptr_RHAND.zvec)*MATRIX_ref::RotAxis(ptr_RHAND.xvec, deg2rad(60));
									mine.mat_RHAND = MATRIX_ref::Axis1(RHANDmat.xvec()*-1, RHANDmat.yvec(), RHANDmat.zvec()*-1);
									RHANDon = ptr_RHAND.now;
								}
								else {
									mine.pos_RHAND = VGet(0, 1, 0);
									mine.mat_RHAND = MATRIX_ref::Axis1(VGet(-1, 0, 0), VGet(0, 1, 0), VGet(0, 0, -1));
									RHANDon = false;
								}
							}

							if (mine.obj.get_anime(3).per == 1.f) {
								Set3DPositionSoundMem(mine.pos_LHAND.get(), mine.audio.slide.get());
								Set3DRadiusSoundMem(1.f, mine.audio.slide.get());
								mine.audio.slide.play(DX_PLAYTYPE_BACK, TRUE);
							}
							mine.obj.get_anime(3).per -= (60.f / 5.f) / fps;
							if (mine.obj.get_anime(3).per <= 0.f) {
								mine.obj.get_anime(3).per = 0.f;
							}
							if (useVR_e) {
								if (vrparts->get_left_hand_num() != -1) {
									auto& ptr_LEFTHAND = (*vrparts->get_device())[vrparts->get_left_hand_num()];
									auto& ptr_RIGHTHAND = (*vrparts->get_device())[vrparts->get_right_hand_num()];
									if (ptr_LEFTHAND.turn && ptr_LEFTHAND.now) {
										//引き金
										easing_set(&mine.obj.get_anime(2).per, float((ptr_LEFTHAND.on[0] & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger)) != 0 && !mine.safety.first), 0.5f, fps);
										//マグキャッチ
										easing_set(&mine.obj.get_anime(5).per, float((ptr_LEFTHAND.on[1] & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_IndexController_A)) != 0), 0.5f, fps);
										//セフティ
										mine.safety.second = std::clamp<uint8_t>(mine.safety.second + 1, 0, (((ptr_LEFTHAND.on[0] & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Touchpad)) != 0) && (ptr_LEFTHAND.touch.x() < -0.5f && ptr_LEFTHAND.touch.y() < 0.5f&&ptr_LEFTHAND.touch.y() > -0.5f)) ? 2 : 0);
										if (mine.safety.second == 1) {
											mine.safety.first ^= 1;
										}
										easing_set(&mine.obj.get_anime(4).per, float(mine.safety.first), 0.5f, fps);
										//セレクター
										mine.selkey = std::clamp<uint8_t>(mine.selkey + 1, 0,
											(((ptr_LEFTHAND.on[0] & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Touchpad)) != 0) && (ptr_LEFTHAND.touch.x() > 0.5f && ptr_LEFTHAND.touch.y() < 0.5f&&ptr_LEFTHAND.touch.y() > -0.5f) && !mine.safety.first)
											? 2 : 0);
										//
									}
									if (ptr_RIGHTHAND.turn && ptr_RIGHTHAND.now) {
										//マガジン取得
										if (!mine.down_mag) {
											if ((ptr_RIGHTHAND.on[0] & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger)) != 0) {
												mine.down_mag = true;
											}
										}
									}
								}
							}
							else {
								easing_set(&mine.obj.get_anime(2).per, float((GetMouseInput() & MOUSE_INPUT_LEFT) != 0), 0.5f, fps);
								mine.selkey = std::clamp<uint8_t>(mine.selkey + 1, 0, ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) ? 2 : 0);
							}
						}
					}
					//共通
					for (auto& c : chara) {
						{
							easing_set(&c.vecadd_LHAND, c.vecadd_LHAND_p, 0.9f, fps);
							easing_set(&c.vecadd_LHAND_p, VGet(0, 0, 1.f), 0.975f, fps);
							if (c.gunf) {
								if (c.ammoc >= 1) {
									c.obj.get_anime(0).per = 1.f;
									c.obj.get_anime(1).per = 0.f;
									c.obj.get_anime(0).time += 60.f / fps;
									if (c.obj.get_anime(0).time >= c.obj.get_anime(0).alltime) {
										c.obj.get_anime(0).time = 0.f;
										c.gunf = false;
									}
								}
								else {
									c.obj.get_anime(1).per = 1.f;
									c.obj.get_anime(0).per = 0.f;
									c.obj.get_anime(1).time += 60.f / fps;
									if (c.obj.get_anime(1).time >= c.obj.get_anime(1).alltime) {
										c.obj.get_anime(1).time = c.obj.get_anime(1).alltime;
										c.gunf = false;
									}
								}

							}
							//マガジン排出
							if (c.obj.get_anime(5).per >= 0.5f) {
								if (c.ammoc >= 1) {
									c.ammoc = 1;
								}
								if (!c.reloadf) {
									Set3DPositionSoundMem(c.pos_LHAND.get(), c.audio.mag_down.get());
									Set3DRadiusSoundMem(1.f, c.audio.mag_down.get());
									c.audio.mag_down.play(DX_PLAYTYPE_BACK, TRUE);
									if (&c == &mine) {
										vrparts->Haptic(&(*vrparts->get_device())[vrparts->get_left_hand_num()], unsigned short(60000));
									}
								}
								c.reloadf = true;
							}
							//セレクター
							if (c.selkey == 1) {
								++c.select %= c.gunptr->select.size();
							}
							//射撃
							if (!c.gunf && c.ammoc >= 1) {
								if (c.gunptr->select[c.select] == 2) {//フルオート用
									c.trigger = 0;
								}
							}
							c.trigger = std::clamp<uint8_t>(c.trigger + 1, 0, (c.obj.get_anime(2).per >= 0.5f) ? 2 : 0);
							if (c.trigger == 1) {
								Set3DPositionSoundMem(c.pos_LHAND.get(), c.audio.trigger.get());
								Set3DRadiusSoundMem(1.f, c.audio.trigger.get());
								c.audio.trigger.play(DX_PLAYTYPE_BACK, TRUE);
							}
							if (c.trigger == 1 && !c.gunf && c.ammoc >= 1) {
								if (&c == &mine) {
									vrparts->Haptic(&(*vrparts->get_device())[vrparts->get_left_hand_num()], unsigned short(60000));
								}
								c.ammoc--;
								++c.guncnt;
								c.gunf = true;
								p_down = -5;
								point += p_down;

								//持ち手を持つとココが相殺される
								if (c.LEFT_hand) {
									c.vecadd_LHAND_p = MATRIX_ref::Vtrans(c.vecadd_LHAND_p, MATRIX_ref::RotY(deg2rad(float(-50 + GetRand(100)) / 100.f))*MATRIX_ref::RotX(deg2rad(float(-50 + GetRand(150)) / 100.f)));
								}
								else {
									c.vecadd_LHAND_p = MATRIX_ref::Vtrans(c.vecadd_LHAND_p, MATRIX_ref::RotY(deg2rad(float(-50 + GetRand(100)) / 100.f))*MATRIX_ref::RotX(deg2rad(float(300 + GetRand(700)) / 100.f)));
								}
								Set3DPositionSoundMem(c.pos_LHAND.get(), c.audio.shot.get());
								Set3DRadiusSoundMem(10.f, c.audio.shot.get());
								c.audio.shot.play(DX_PLAYTYPE_BACK, TRUE);

								Set3DPositionSoundMem(c.pos_LHAND.get(), c.audio.slide.get());
								Set3DRadiusSoundMem(1.f, c.audio.slide.get());
								c.audio.slide.play(DX_PLAYTYPE_BACK, TRUE);

								auto& u = c.bullet[c.usebullet];
								auto& a = c.ammo[c.usebullet];
								++c.usebullet %= c.bullet.size();
								//ココだけ変化
								u.spec.caliber_a = 0.011f;
								u.spec.damage_a = 33;
								u.spec.pene_a = 10.f;
								u.spec.speed_a = 260.f;

								u.pos = c.obj.frame(c.gunptr->frame[3].first);
								u.vec = c.mat_LHAND.zvec()*-1.f;
								u.hit = false;
								u.flug = true;
								u.cnt = 0.f;
								u.yadd = 0.f;
								u.repos = u.pos;

								if (c.effcs[ef_fire].handle.IsPlaying()) {
									c.effcs[ef_fire].handle.Stop();
								}
								set_effect(&c.effcs[ef_fire], u.pos, u.vec, 0.0025f / 0.1f);
								{
									if (c.effcs_gun[c.gun_effcnt].cnt > 0.f) {
										c.effcs_gun[c.gun_effcnt].first.handle.Stop();
									}

									set_effect(&c.effcs_gun[c.gun_effcnt].first, u.pos, u.vec, 0.11f / 0.1f);
									set_pos_effect(&c.effcs_gun[c.gun_effcnt].first, Drawparts->get_effHandle(ef_smoke2));
									c.effcs_gun[c.gun_effcnt].second = &u;
									c.effcs_gun[c.gun_effcnt].cnt = 0.f;
									++c.gun_effcnt %= c.effcs_gun.size();
								}


								a.cnt = 0.f;
								a.pos = c.obj.frame(c.gunptr->frame[2].first);//排莢
								a.add = (c.obj.frame(c.gunptr->frame[2].first + 1) - a.pos).Norm()*2.5f / fps;//排莢ベクトル
								a.mat = c.mat_LHAND;
							}
							//マガジン取得
							if (c.reloadf) {
								if (!c.down_mag) {
									c.mat_mag *= MATRIX_ref::Mtrans(c.mat_addvec*c.mat_add);
									c.mat_add += -9.8f / powf(fps, 2.f);
									easing_set(&c.mat_addvec, VGet(0, 1.f, 0), 0.95f, fps);
								}
								else {
									auto p = MATRIX_ref::RotVec2(c.mat_RHAND.yvec(), (c.obj.frame(c.gunptr->frame[0].first) - (c.pos_RHAND + c.pos)));
									VECTOR_ref xvec = MATRIX_ref::Vtrans(c.mat_RHAND.xvec(), p);
									VECTOR_ref yvec = MATRIX_ref::Vtrans(c.mat_RHAND.yvec(), p);
									VECTOR_ref zvec = MATRIX_ref::Vtrans(c.mat_RHAND.zvec(), p);

									c.mat_mag = c.mag.GetFrameLocalMatrix(3)* MATRIX_ref::Axis1(xvec, yvec, zvec)* MATRIX_ref::Mtrans(c.pos_RHAND + c.pos);

									if ((c.mag.frame(3) - c.obj.frame(c.gunptr->frame[0].first)).size() <= 0.05f) {
										c.obj.get_anime(1).time = 0.f;
										c.obj.get_anime(0).per = 1.f;
										c.obj.get_anime(1).per = 0.f;
										if (c.ammoc == 0) {
											c.obj.get_anime(3).per = 1.f;
										}

										Set3DPositionSoundMem(c.pos_LHAND.get(), c.audio.mag_set.get());
										Set3DRadiusSoundMem(1.f, c.audio.mag_set.get());
										c.audio.mag_set.play(DX_PLAYTYPE_BACK, TRUE);

										c.reloadf = false;
										c.ammoc += c.gunptr->ammo_max;
									}
								}
							}
							else {
								c.mat_mag = c.mat_LHAND* MATRIX_ref::Mtrans(c.obj.frame(c.gunptr->frame[1].first));
								c.down_mag = false;
								c.mat_add = 0.f;
								c.mat_addvec = (c.obj.frame(c.gunptr->frame[1].first) - c.obj.frame(c.gunptr->frame[0].first)).Norm();
							}
							{
								/*
								//右手
								VECTOR_ref u_ = c.pos_LHAND - c.hand.frame(c.frame_hand[0].first);
								VECTOR_ref v_ = MATRIX_ref::Vtrans(VGet(0, -1.f, 0), MATRIX_ref::RotVec2(VGet(u_.x(), 0, u_.z()), u_));
								float dist_0 = u_.size();
								float dist_1 = (c.frame_hand[1].second).size();
								float dist_2 = (c.frame_hand[2].second).size();
								float cos_t = -std::clamp(getcos_tri(dist_2, dist_0, dist_1), -1.f, 1.f);
								float sin_t = -std::sqrtf(1.f - std::powf(cos_t, 2.f));

								//u_ = VGet(u_.x(), u_.y(), u_.z());
								//
								c.hand.SetFrameLocalMatrix(c.frame_hand[0].first, MATRIX_ref::Mtrans(c.frame_hand[0].second));
								MATRIX_ref m1 =
									MATRIX_ref::RotVec2((c.hand.frame(c.frame_hand[1].first) - c.hand.frame(c.frame_hand[0].first)), (u_.Norm()*cos_t + v_ * sin_t)*dist_1)
									*
									HMDmat.Inverse()
									;
								c.hand.SetFrameLocalMatrix(c.frame_hand[0].first, m1*MATRIX_ref::Mtrans(c.frame_hand[0].second));
								//
								c.hand.SetFrameLocalMatrix(c.frame_hand[1].first, m1.Inverse()*MATRIX_ref::Mtrans(c.frame_hand[1].second));
								MATRIX_ref m2 = MATRIX_ref::RotVec2(
									(c.hand.frame(c.frame_hand[1].first) - c.hand.frame(c.frame_hand[2].first)),
									MATRIX_ref::Vtrans((c.pos_LHAND - c.hand.frame(c.frame_hand[1].first)), HMDmat.Inverse())
								);
								c.hand.SetFrameLocalMatrix(c.frame_hand[1].first, m2* m1.Inverse()*MATRIX_ref::Mtrans(c.frame_hand[1].second));
								//

								MATRIX_ref m3 = MATRIX_ref::RotZ(deg2rad(90))*c.mat_LHAND*HMDmat.Inverse();
								c.hand.SetFrameLocalMatrix(c.frame_hand[2].first, m3*m2.Inverse()*MATRIX_ref::Mtrans(c.frame_hand[2].second));
								*/
								//右手
								{
									MATRIX_ref m3 = MATRIX_ref::RotZ(deg2rad(90))*c.mat_LHAND*HMDmat.Inverse();
									c.hand.SetFrameLocalMatrix(1, m3*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.pos_LHAND - (c.pos_HMD + c.pos), HMDmat.Inverse())));
								}
								//左手
								{
									float dist_ = (c.pos_RHAND - c.obj.frame(c.gunptr->frame[6].first)).size();
									if (dist_ <= 0.2f && (!c.reloadf || !c.down_mag)) {
										c.LEFT_hand = true;
										MATRIX_ref m4 = MATRIX_ref::RotZ(deg2rad(-90));
										c.hand.SetFrameLocalMatrix(17, m4*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(
											c.obj.frame(c.gunptr->frame[6].first)
											- (c.pos_HMD + c.pos), HMDmat.Inverse())));
									}
									else {
										c.LEFT_hand = false;
										MATRIX_ref m4 = MATRIX_ref::RotZ(deg2rad(-90))*c.mat_RHAND*HMDmat.Inverse();
										c.hand.SetFrameLocalMatrix(17, m4*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.pos_RHAND - (c.pos_HMD + c.pos), HMDmat.Inverse())));
									}
								}
								//身体
								{
									VECTOR_ref v_ = HMDmat.zvec();
									float rad = std::atan2f(-v_.x(), -v_.z());
									c.hand.SetFrameLocalMatrix(34,
										MATRIX_ref::RotY(DX_PI_F + rad / 2.f)*HMDmat.Inverse()*
										MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(VGet(0.f, -0.15f, 0.f), HMDmat.Inverse()))
									);
								}
							}

							c.mag.SetMatrix(c.mat_mag);
							c.obj.SetMatrix(c.mat_LHAND*MATRIX_ref::Mtrans(c.pos_LHAND + c.pos));
							c.hand.SetMatrix(HMDmat*MATRIX_ref::Mtrans(c.pos_HMD + c.pos));
						}
						c.obj.work_anime();
						for (auto& a : c.bullet) {
							if (a.flug) {
								a.repos = a.pos;
								a.pos += a.vec * (a.spec.speed_a / fps);
								//判定
								{
									auto p = mapparts->map_col_line(a.repos, a.pos, 0);
									if (p.HitFlag == TRUE) {
										a.pos = p.HitPosition;
									}
									{
										for (auto& tp : tgt_pic) {
											auto q = tp.obj.CollCheck_Line(a.repos, a.pos, 0, 1);
											if (q.HitFlag == TRUE) {
												a.pos = q.HitPosition;
												//
												tp.power = (tp.obj.frame(tgt_f.first) - a.pos).y();
												tp.time = 0.f;
												{
													//弾痕処理
													tp.pic.SetDraw_Screen(false);
													VECTOR_ref pvecp = (a.pos - tp.obj.frame(tgt_f.first));
													VECTOR_ref xvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.xf));
													VECTOR_ref yvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.yf));
													int UI_xpos = int(float(tgt_pic_x)*(xvecp.Norm().dot(pvecp)) / xvecp.size());//X方向
													int UI_ypos = int(float(tgt_pic_y)*(yvecp.Norm().dot(pvecp)) / yvecp.size());//Y方向
													DrawCircle(UI_xpos, UI_ypos, 10, GetColor(255, 0, 0));//弾痕
													//ポイント判定
													{
														int r_, g_, b_, a_;
														GetPixelSoftImage(tgt_col, UI_xpos, UI_ypos, &r_, &g_, &b_, &a_);

														switch (r_) {
														case 0:
															p_up = 15;
															break;
														case 44:
															p_up = 10;
															break;
														case 86:
															p_up = 9;
															break;
														case 128:
															p_up = 8;
															break;
														case 170:
															p_up = 7;
															break;
														case 212:
															p_up = 5;
															break;
														default:
															p_up = 0;
															break;
														}

														p_up = int(float(p_up) * ((a.pos - c.pos_LHAND).size() / 10.f));

														point += p_up;
													}
												}

												for (auto& b : c.effcs_gun) {
													if (b.second == &a) {
														b.first.handle.SetPos(b.second->pos);
														break;
													}
												}
												a.flug = false;
												break;
											}
										}
									}
									if (p.HitFlag == TRUE && a.flug) {
										for (auto& b : c.effcs_gun) {
											if (b.second == &a) {
												b.first.handle.SetPos(b.second->pos);
												break;
											}
										}
										a.flug = false;
									}
								}
								//消す(2秒たった、スピードが100以下、貫通が0以下)
								if (a.cnt >= 3.f || a.spec.speed_a < 0.f || a.spec.pene_a <= 0.f) {
									a.flug = false;
								}
								if (!a.flug) {
									for (auto& b : c.effcs_gun) {
										if (b.second == &a) {
											b.cnt = 2.5f;
											b.first.handle.SetPos(b.second->pos);
											break;
										}
									}
								}
							}
						}

						for (auto& a : c.ammo) {
							if (a.cnt >= 0.f) {
								a.cnt += 1.f / fps;
								a.pos += a.add*(float(200 + GetRand(1000)) / 1000.f);
								a.add.yadd(-9.8f / powf(fps, 2.f));
								a.mat *= MATRIX_ref::RotY(deg2rad(360 * 10 + GetRand(360 * 20)) / fps);

								auto pp = mapparts->map_col_line(a.pos + VGet(0, 1.f, 0), a.pos, 0);
								if (pp.HitFlag == 1) {
									a.pos = pp.HitPosition;
									a.add += VECTOR_ref(pp.Normal)*(VECTOR_ref(pp.Normal).dot(a.add*-1.f)*1.5f);
									if (!a.down) {
										Set3DPositionSoundMem(c.pos_LHAND.get(), c.audio.case_down.get());
										Set3DRadiusSoundMem(1.f, c.audio.case_down.get());
										c.audio.case_down.play(DX_PLAYTYPE_BACK, TRUE);
									}
									a.down = true;
								}
								if (a.cnt >= 3.f) {
									a.cnt = -1.f;
								}

								a.second.SetMatrix(a.mat*MATRIX_ref::Mtrans(a.pos));
							}
							else {
								a.down = false;
							}
						}
						for (auto& t : c.effcs) {
							if (t.id != ef_smoke1 && t.id != ef_smoke2) {
								set_pos_effect(&t, Drawparts->get_effHandle(int(t.id)));
							}
							if (!t.handle.IsPlaying()) {
								t.handle.Stop();
							}
						}
						for (auto& a : c.effcs_gun) {
							if (a.second != nullptr) {
								if (a.second->flug) {
									a.first.handle.SetPos(a.second->pos);
								}
								if (a.cnt >= 0.f) {
									a.cnt += 1.f / fps;
									if (a.cnt >= 3.f) {
										a.first.handle.Stop();
										a.cnt = -1.f;
									}
								}
							}
						}
					}
					//
					{
						//他の座標をここで出力(ホスト)
					}
					{
						//ホストからの座標をここで入力
					}
					//ターゲットの演算
					for (auto& tp : tgt_pic) {
						//if(false)
						{
							if (std::abs(tp.obj.GetPosition().x()) > 7.5f) {
								tp.LR ^= 1;
							}
							VECTOR_ref pos = VGet(tp.obj.GetPosition().x() + float(tp.LR ? 1 : -1) / fps, 0, tp.obj.GetPosition().z());
							auto pp = mapparts->map_col_line(pos - VGet(0, -10.f, 0), pos - VGet(0, 10.f, 0), 0);
							if (pp.HitFlag == 1) {
								tp.obj.SetPosition(pp.HitPosition);
							}
						}

						tp.radadd = -cos(tp.time)*deg2rad(tp.power);
						tp.rad = std::clamp(tp.rad + tp.radadd, deg2rad(-90), deg2rad(90));
						tp.obj.SetFrameLocalMatrix(tgt_f.first, MATRIX_ref::RotX(tp.rad)* MATRIX_ref::Mtrans(tgt_f.second));
						tp.obj.RefreshCollInfo(0, 1);

						easing_set(&tp.rad, 0.f, 0.995f, fps);
						easing_set(&tp.power, 0.f, 0.995f, fps);
						tp.time += deg2rad(180.f / fps);
					}
					//影用意
					if (shadow_e) {
						Drawparts->Ready_Shadow(campos, draw_in_shadow, VGet(50.f, 2.5f, 50.f));
					}
					//VR空間に適用
					vrparts->Move_Player();
					//campos,camvec,camupの指定
					{
						campos_buf = mine.pos + mine.pos_HMD;
						camvec = HMDmat.zvec()*-1.f;
						camup = HMDmat.yvec();
					}
					Set3DSoundListenerPosAndFrontPosAndUpVec(campos_buf.get(), (campos_buf + camvec).get(), camup.get());
					UpdateEffekseer3D();

					for (char i = 0; i < 2; i++) {
						//被写体深度描画
						{
							float fardist = 100.f;
							float neardist = 0.1f;

							campos = campos_buf + vrparts->GetEyePosition_minVR(i);
							if (shadow_e) {
								Hostpassparts->dof(&BufScreen,
									mapparts->sky_draw(campos, campos + camvec, camup, fov),
									[&Drawparts, &draw_on_shadow] { Drawparts->Draw_by_Shadow(draw_on_shadow); }, campos, campos + camvec, camup, fov, fardist, neardist);
							}
							else {
								Hostpassparts->dof(&BufScreen,
									mapparts->sky_draw(campos, campos + camvec, camup, fov),
									draw_on_shadow, campos, campos + camvec, camup, fov, fardist, neardist);
							}
						}
						//描画
						outScreen[i].SetDraw_Screen();
						{
							//背景
							BufScreen.DrawGraph(0, 0, false);
							//ブルーム
							Hostpassparts->bloom(BufScreen, 64);
							//UI
							UIparts->draw(mine, fps, point, p_up, p_down, useVR_e);
						}
					}
					//スコープ
					if (mine.gunptr->frame[4].first != INT_MAX) {
						float fardist = 100.f;
						float neardist = 1.f;
						VECTOR_ref cam = mine.obj.frame(mine.gunptr->frame[4].first);
						VECTOR_ref vec = cam - mine.mat_LHAND.zvec();
						VECTOR_ref yvec = mine.mat_LHAND.yvec();

						if (shadow_e) {
							Hostpassparts->draw(&ScopeScreen,
								mapparts->sky_draw(cam, vec, yvec, (fov / 7.5f) / 4.f),
								[&Drawparts, &draw_on_shadow] { Drawparts->Draw_by_Shadow(draw_on_shadow); }, cam, vec, yvec, (fov / 7.5f) / 4.f, fardist, neardist);
						}
						else {
							Hostpassparts->draw(&ScopeScreen,
								mapparts->sky_draw(cam, vec, yvec, (fov / 7.5f) / 4.f),
								draw_on_shadow, cam, vec, yvec, (fov / 7.5f) / 4.f, fardist, neardist);
						}

						DrawLine(0, 1080 / 2, 1080 * 45 / 100, 1080 / 2, GetColor(255, 255, 255), 20);
						DrawLine(1080 * 55 / 100, 1080 / 2, 1080, 1080 / 2, GetColor(255, 255, 255), 20);
						DrawLine(1080 / 2, 1080 / 2, 1080 / 2, 1080, GetColor(255, 255, 255), 20);
					}
					//VRに移す
					if (useVR_e) {
						for (char i = 0; i < 2; i++) {
							GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
							outScreen[i].DrawGraph(0, 0, false);
							vrparts->PutEye((ID3D11Texture2D*)GetUseDirect3D11BackBufferTexture2D(), i);
						}
					}
					//映す
					{
						float fardist = 100.f;
						float neardist = 0.1f;
						VECTOR_ref cam = mine.pos + VGet(0.25f, 1.45f, -0.75f);
						VECTOR_ref vec = mine.pos_HMD;
						VECTOR_ref yvec = VGet(0, 1.f, 0);

						if (shadow_e) {
							Hostpassparts->draw(&outScreen[2],
								mapparts->sky_draw(cam, vec, yvec, fov),
								[&Drawparts, &draw_on_shadow] { Drawparts->Draw_by_Shadow(draw_on_shadow); }, cam, vec, yvec, fov, fardist, neardist);
						}
						else {
							Hostpassparts->draw(&outScreen[2],
								mapparts->sky_draw(cam, vec, yvec, fov),
								draw_on_shadow, cam, vec, yvec, fov, fardist, neardist);
						}
						GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK, 0.1f, 100.f, fov, cam, vec, yvec);
					}
					//
					{
						TPS.second = std::clamp<uint8_t>(TPS.second + 1, 0, ((CheckHitKey(KEY_INPUT_SPACE) != 0) ? 2 : 0));
						if (TPS.second == 1) {
							TPS.first ^= 1;
						}
						if (TPS.first) {//TPS視点
							outScreen[2].DrawExtendGraph(0, 0, out_dispx, out_dispy, false);
							//ターゲットを映す
							{
								VECTOR_ref cam = mine.obj.frame(mine.gunptr->frame[3].first);
								VECTOR_ref vec = MATRIX_ref::Vtrans(VGet(0, 0, -1.f), mine.mat_LHAND);
								int i = 0;
								bool fl = false;
								for (auto& tp : tgt_pic) {
									auto q = tp.obj.CollCheck_Line(cam, cam + vec * 100.f, 0, 1);
									if (q.HitFlag == TRUE) {
										tgt_pic_sel = i;
										fl = true;
									}
									i++;
								}
								if (fl) {
									easing_set(&tgt_pic_on, 1.f, 0.9f, fps);
								}
								else {
									easing_set(&tgt_pic_on, 0.f, 0.95f, fps);
								}
							}
							if (tgt_pic_sel >= 0) {
								int xp = 60;
								int yp = 60;
								int xs = 160;
								int ys = 160 * tgt_pic_y / tgt_pic_x;

								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f* tgt_pic_on));
								VECTOR_ref p = ConvWorldPosToScreenPos(((tgt_pic[tgt_pic_sel].obj.frame(tgt_pic[tgt_pic_sel].xf) + tgt_pic[tgt_pic_sel].obj.frame(tgt_pic[tgt_pic_sel].yf)) / 2.f).get());
								if (p.z() >= 0.f&&p.z() <= 1.f) {
									DrawLine(xp + xs / 2, yp + ys / 2, int(p.x()*out_dispx / dispx), int(p.y()*out_dispy / dispy), GetColor(255, 0, 0), 2);
								}
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128 + int(127.f* tgt_pic_on));
								tgt_pic[tgt_pic_sel].pic.DrawExtendGraph(xp, yp, xp + xs, yp + ys, false);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.f* tgt_pic_on));
								DrawBox(xp, yp, xp + xs, yp + ys, GetColor(255, 0, 0), FALSE);
								SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
							}
						}
						else {//FPS視点
							outScreen[1].DrawExtendGraph(0, 0, out_dispx, out_dispy, false);
						}
						if (mine.gunptr->frame[4].first != INT_MAX) {
							ScopeScreen.DrawExtendGraph(out_dispx - 200, 0, out_dispx, 200, true);
						}
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
		}
		//解放
		{
			for (auto& c : chara) {
				c.delete_chara();
			}
			chara.clear();
			for (auto& p : tgt_pic) {
				p.obj.Dispose();
				p.pic.Dispose();
			}
			tgt_pic.clear();
			mapparts->delete_map();
			if (shadow_e) {
				Drawparts->Delete_Shadow();
			}
		}
		//
	} while (ProcessMessage() == 0 && ending);
	return 0; // ソフトの終了
}
