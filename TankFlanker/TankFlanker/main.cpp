#include "sub.hpp"
#include "UI.hpp"
#include "HostPass.hpp"
#include "map.hpp"
#include "VR.hpp"
#include "debug.hpp"
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	//tex確認
	ID3D11Texture2D* ptr_DX11 = nullptr;
	//設定読み込み
	bool dof_e = false;
	bool bloom_e = false;
	bool shadow_e = false;
	bool useVR_e = true;
	bool getlog_e = true;
	int dispx,dispy; /*描画*/
	int out_dispx,out_dispy; /*ウィンドウ*/
	switchs TPS;
	switchs ads;
	VECTOR_ref viewvec;
	float xrad_p=0.f;
	VECTOR_ref add_pos;
	bool wkey;
	bool skey;
	bool akey;
	bool dkey;
	{
		SetOutApplicationLogValidFlag(FALSE);  /*log*/
		int mdata = FileRead_open("data/setting.txt", FALSE);
		dof_e = getparams::_bool(mdata);
		bloom_e = getparams::_bool(mdata);
		shadow_e = getparams::_bool(mdata);
		useVR_e = getparams::_bool(mdata);
		getlog_e = getparams::_bool(mdata);
		FileRead_close(mdata);
	}
	//DXLib描画
	auto vrparts = std::make_unique<VRDraw>(&useVR_e);
	//画面指定
	if (useVR_e) {
		dispx = 1080*2;
		dispy = 1200*2;
		out_dispx = dispx * (desky * 8 / 9) / dispy;
		out_dispy = dispy * (desky * 8 / 9) / dispy;
	}
	else {
		dispx = deskx;
		dispy = desky;
		out_dispx = dispx;
		out_dispy = dispy;
	}
	//
	auto Drawparts = std::make_unique<DXDraw>("FPS_0", dispx, dispy, 90.f,shadow_e,getlog_e);/*汎用クラス*/
	auto UIparts = std::make_unique<UI>(out_dispx, out_dispy, dispx, dispy);		 /*UI*/
	auto Debugparts = std::make_unique<DeBuG>(90);						 /*デバッグ*/
	auto Hostpassparts = std::make_unique<HostPassEffect>(dof_e, bloom_e, dispx, dispy);	 /*ホストパスエフェクト*/
	if (useVR_e) {
		SetWindowSize(out_dispx, out_dispy);
		SetWindowPosition((deskx - out_dispx) / 2, 0);
	}
	//
	//SetWindowPosition(deskx + (deskx - out_dispx) / 2-12, -64);
	//
	std::array<GraphHandle, 3> outScreen;
	outScreen[0] = GraphHandle::Make(dispx, dispy);//左目
	outScreen[1] = GraphHandle::Make(dispx, dispy);//右目
	outScreen[2] = GraphHandle::Make(dispx, dispy);//TPS
	GraphHandle BufScreen = GraphHandle::Make(dispx, dispy);//
	GraphHandle ScopeScreen = GraphHandle::Make(1080, 1080);//
	//操作
	VECTOR_ref campos, campos_buf, camvec, camup;			    //カメラ
	float fov = deg2rad(useVR_e ? 90 : 45);
	//データ
	MV1 hand;
	MV1::Load("data/model/hand/model_h.mv1", &hand, true);
	auto mapparts = std::make_unique<Mapclass>(dispx, dispy);
	//ターゲット
	MV1 tgt;
	MV1::Load("data/model/tgt/model.mv1", &tgt, true);
	int tgt_col = LoadSoftImage("data/model/tgt/point.bmp");
	GraphHandle tgt_pic_tmp = GraphHandle::Load("data/model/tgt/Target-A2.png");
	//GUNデータ
	std::vector<Mainclass::Gun> gun_data;
	gun_data.resize(3);
	gun_data[0].name = "1911";
	gun_data[1].name = "M82A2";
	gun_data[2].name = "CAR15_M4";
	for (auto& g : gun_data) {
		g.mod.set(g.name);
	}
	//ロード画面
	UIparts->load_window("モデル");	
	//GUNデータ取得
	for (auto& g : gun_data) {
		//フレーム
		g.frame.resize(8);
		g.frame[4].first = INT_MAX;
		g.frame[7].first = INT_MAX;
		g.mod.obj.SetMatrix(MGetIdent());
		for (int i = 0; i < g.mod.obj.frame_num(); i++) {
			std::string s = g.mod.obj.frame_name(i);
			if (s.find("mag_fall") != std::string::npos) {
				g.frame[0].first = i;//排莢
				g.frame[0].second = MATRIX_ref::Vtrans(VGet(0, 0, 0), g.mod.obj.GetFrameLocalMatrix(g.frame[0].first));//mag
				g.frame[1].first = i + 1;
				g.frame[1].second = MATRIX_ref::Vtrans(VGet(0, 0, 0), g.mod.obj.GetFrameLocalMatrix(g.frame[1].first));//mag先
			}
			else if (s.find("case") != std::string::npos) {
				g.frame[2].first = i;//排莢
			}
			else if (s.find("mazzule") != std::string::npos) {
				g.frame[3].first = i;//マズル
			}
			else if (s.find("scope") != std::string::npos) {
				g.frame[4].first = i;//スコープ
				g.frame[4].second = MATRIX_ref::Vtrans(VGet(0, 0, 0), g.mod.obj.GetFrameLocalMatrix(g.frame[4].first));//mag先
			}
			else if (s.find("trigger") != std::string::npos) {
				g.frame[5].first = i+1;//トリガー
			}
			else if (s.find("LEFT") != std::string::npos) {
				g.frame[6].first = i;//左手
			}
			else if (s.find("site") != std::string::npos) {
				g.frame[7].first = i;//アイアンサイト
				g.frame[7].second = g.mod.obj.frame(g.frame[7].first);//mag先
			}
		}
		//テキスト
		{
			int mdata = FileRead_open(("data/gun/" + g.name + "/data.txt").c_str(), FALSE);
			//装弾数
			g.ammo_max = getparams::_long(mdata);
			//セレクター設定
			while (true) {
				auto p = getparams::_str(mdata);
				if (getright(p.c_str()).find("end") != std::string::npos) {
					break;
				}
				else if (getright(p.c_str()).find("semi") != std::string::npos) {
					g.select.emplace_back(uint8_t(1));					//セミオート=1
				}
				else if (getright(p.c_str()).find("full") != std::string::npos) {
					g.select.emplace_back(uint8_t(2));					//フルオート=2
				}
				else if (getright(p.c_str()).find("3b") != std::string::npos) {
					g.select.emplace_back(uint8_t(3));					//3連バースト=3
				}
				else if (getright(p.c_str()).find("2b") != std::string::npos) {
					g.select.emplace_back(uint8_t(4));					//2連バースト=4
				}
				else {
					g.select.emplace_back(uint8_t(1));
				}
			}
			//サウンド
			g.audio.set(mdata);
			FileRead_close(mdata);
		}
	}
	//ターゲット
	frames tgt_f = { 2,tgt.frame(2) };
	int tgt_pic_x,tgt_pic_y;
	tgt_pic_tmp.GetSize(&tgt_pic_x, &tgt_pic_y);
	class tgts {
	public:
		MV1 obj;
		GraphHandle pic;
		int x_frame = 0, y_frame = 0;
		float rad = 0.f;
		float time = 0.f;
		float power = 0.f;
		bool LR = true;

		void set(MV1& tgt,GraphHandle&tex,const int&tgt_picx, const int&tgt_picy) {
			obj = tgt.Duplicate();
			pic = GraphHandle::Make(tgt_picx, tgt_picy);
			pic.SetDraw_Screen(false);
			tex.DrawGraph(0, 0, true);

			obj.SetupCollInfo(8, 8, 8, 0, 1);
			obj.SetTextureGraphHandle(2, pic, false);
			x_frame = 4;
			y_frame = 3;
			LR = true;
		}
	};
	std::vector<tgts> tgt_pic;
	//スコア保存
	std::vector<int> scores;
	{
		SetOutApplicationLogValidFlag(FALSE);  /*log*/
		int mdata = FileRead_open("data/score.txt", FALSE);
		while (FileRead_eof(mdata) == 0) {
			scores.emplace_back(getparams::_int(mdata));
		}
		FileRead_close(mdata);
	}
	//VRセット
	vrparts->Set_Device();
	//キャラ
	std::vector<Mainclass::Chara> chara;
	//ココから繰り返し読み込み//-------------------------------------------------------------------
	bool ending = true;
	do {
		//キャラ設定
		chara.resize(1);
		{
			int sel_g = UIparts->select_window(useVR_e, gun_data, vrparts);
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
		UIparts->load_window("マップモデル");
		//ターゲット
		{
			tgt_pic.resize(4);
			for (auto& p : tgt_pic) {
				p.set(tgt, tgt_pic_tmp, tgt_pic_x, tgt_pic_y);
			}
			tgt_pic[0].obj.SetPosition(VGet(4, 0, 10.f));
			tgt_pic[1].obj.SetPosition(VGet(-2, 0, 20.f));
			tgt_pic[2].obj.SetPosition(VGet(0, 0, 30.f));
			tgt_pic[3].obj.SetPosition(VGet(2, 0, 45.f));
			for (auto& p : tgt_pic) {
				p.obj.SetPosition(mapparts->map_col_line(p.obj.GetPosition() - VGet(0, -10.f, 0), p.obj.GetPosition() - VGet(0, 10.f, 0), 0).HitPosition);
			}
		}
		//ライティング
		Drawparts->Set_Light_Shadow(13, VGet(50.f, 20.f, 50.f), VGet(0.05f, -0.5f, 0.75f), [&mapparts] {mapparts->map_get().DrawModel(); });
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
					if (a.cnt < 0.f) { continue; }
					a.second.DrawModel();
				}
			}
		};
		auto draw_on_shadow = [&mapparts, &tgt_pic, &chara] {
			SetFogStartEnd(0.0f, 3000.f);
			SetFogColor(128, 128, 128);
			mapparts->map_get().DrawModel();
			for (auto& p : tgt_pic) {
				p.obj.DrawModel();
			}
			for (auto& c : chara) {
				c.hand.DrawModel();
				c.mag.DrawModel();
				c.obj.DrawModel();
				for (auto& a : c.ammo) {
					if (a.cnt < 0.f) { continue; }
					a.second.DrawModel();
				}
			}
			//銃弾
			SetFogEnable(FALSE);
			SetUseLighting(FALSE);
			for (auto& c : chara) {
				for (auto& a : c.bullet) {
					if (!a.flug) { continue; }
					DXDraw::Capsule3D(a.pos, a.repos, ((a.spec.caliber_a - 0.00762f) * 0.1f + 0.00762f), a.color, GetColor(255, 255, 255));
				}
			}
			SetUseLighting(TRUE);
			SetFogEnable(TRUE);
		};
		auto draw_by_shadow = [&Drawparts, &draw_on_shadow] {
			Drawparts->Draw_by_Shadow(draw_on_shadow);
		};
		//開始
		{
			auto& mine = chara[0];
			mine.mat_HMD = MATRIX_ref::Axis1(VGet(-1, 0, 0), VGet(0, 1, 0), VGet(0, 0, -1));//改善
			int point = 0;
			int p_up = 0;
			int p_down = 0;
			bool c_ready = false;
			float c_readytimer = 3.f;
			bool c_start = false;
			bool c_end = false;
			float c_timer = 30.f;
			SetMousePoint(deskx / 2, desky / 2);
			while (ProcessMessage() == 0) {
				const auto fps = GetFPS();
				const auto waits = GetNowHiPerformanceCount();
				Debugparts->put_way();
				{
					//プレイヤー操作
					{
						//HMD
						if (useVR_e) {
							vrparts->GetDevicePositionVR(vrparts->get_hmd_num(), &mine.pos_HMD, &mine.mat_HMD);
						}
						else {
							mine.pos_HMD = VGet(0.f, 1.5f, 0.f);
							//マウスエイム
							{
								int x_m, y_m;
								GetMousePoint(&x_m, &y_m);
								mine.mat_HMD = MATRIX_ref::RotX(-xrad_p)*mine.mat_HMD;
								xrad_p = std::clamp(xrad_p - deg2rad(y_m - desky / 2)*0.1f, deg2rad(-45), deg2rad(45));
								mine.mat_HMD *= MATRIX_ref::RotY(deg2rad(x_m - deskx / 2)*0.1f);
								mine.mat_HMD = MATRIX_ref::RotX(xrad_p)*mine.mat_HMD;
								SetMousePoint(deskx / 2, desky / 2);
								SetMouseDispFlag(FALSE);
							}
						}
						//移動
						{
							wkey = (CheckHitKey(KEY_INPUT_W) != 0);
							skey = (CheckHitKey(KEY_INPUT_S) != 0);
							akey = (CheckHitKey(KEY_INPUT_A) != 0);
							dkey = (CheckHitKey(KEY_INPUT_D) != 0);
							if (wkey) {
								easing_set(&add_pos, mine.mat_HMD.zvec()*-4.f / fps, 0.95f, fps);
							}
							if (skey) {
								easing_set(&add_pos, mine.mat_HMD.zvec()*4.f / fps, 0.95f, fps);
							}
							if (akey) {
								easing_set(&add_pos, mine.mat_HMD.xvec()*4.f / fps, 0.95f, fps);
							}
							if (dkey) {
								easing_set(&add_pos, mine.mat_HMD.xvec()*-4.f / fps, 0.95f, fps);
							}
							if (!wkey && !skey && !akey && !dkey) {
								easing_set(&add_pos, VGet(0, 0, 0), 0.95f, fps);
							}
							mine.pos += add_pos;
							auto pp = mapparts->map_col_line(mine.pos + VGet(0, 1.f, 0), mine.pos, 0);
							if (pp.HitFlag == 1) {
								mine.pos = pp.HitPosition;
								mine.add_ypos = 0.f;
							}
							else {
								mine.pos.yadd(mine.add_ypos);
								mine.add_ypos -= 9.8f / std::powf(fps, 2.f);
							}
						}
						//LHAND
						vrparts->GetDevicePositionVR(vrparts->get_hand1_num(), &mine.pos_LHAND, &mine.mat_LHAND);
						if (!useVR_e) {
							VECTOR_ref pv = VGet(0, 0, 0);
							if (mine.gunptr->frame[4].first != INT_MAX) {
								pv = mine.gunptr->frame[4].second;
							}
							else if (mine.gunptr->frame[7].first != INT_MAX) {
								pv = mine.gunptr->frame[7].second;
							}
							if (ads.first) {
								easing_set(&viewvec, mine.pos_HMD + VGet(0.f, 0.f - pv.y(), -0.25f), 0.75f, fps);
							}
							else {
								easing_set(&viewvec, mine.pos_HMD + VGet(-0.15f, -0.05f - pv.y(), -0.5f), 0.75f, fps);
							}
							mine.pos_LHAND = mine.pos_HMD + MATRIX_ref::Vtrans(viewvec - mine.pos_HMD, mine.mat_HMD);
							mine.mat_LHAND = mine.mat_HMD;
						}
						else {
							mine.mat_LHAND = mine.mat_LHAND*MATRIX_ref::RotAxis(mine.mat_LHAND.xvec(), deg2rad(-60));
						}
						mine.mat_LHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), mine.vecadd_LHAND)*mine.mat_LHAND;//リコイル
						//RHAND
						vrparts->GetDevicePositionVR(vrparts->get_hand2_num(), &mine.pos_RHAND, &mine.mat_RHAND);
						if (!useVR_e) {
							mine.pos_RHAND = mine.obj.frame(mine.gunptr->frame[6].first);
							mine.mat_RHAND = mine.mat_HMD;
						}
						else {
							mine.mat_RHAND = mine.mat_RHAND*MATRIX_ref::RotAxis(mine.mat_RHAND.xvec(), deg2rad(-60));
						}
						//銃
						if (mine.obj.get_anime(3).per == 1.f) {
							mine.audio.slide.play_3D(mine.pos + mine.pos_LHAND, 1.f);
						}
						mine.obj.get_anime(3).per = std::max(mine.obj.get_anime(3).per - 12.f / fps, 0.f);
						if (useVR_e) {
							if (vrparts->get_hand1_num() != -1) {
								auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand1_num()];
								if (ptr_.turn && ptr_.now) {
									//引き金
									easing_set(&mine.obj.get_anime(2).per, float((ptr_.on[0] & BUTTON_TRIGGER) != 0 && !mine.safety.first), 0.5f, fps);
									//マグキャッチ
									easing_set(&mine.obj.get_anime(5).per, float((ptr_.on[1] & BUTTON_SIDE) != 0), 0.5f, fps);
									//セフティ
									mine.safety.get_in(((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_.touch.x() < -0.5f && ptr_.touch.y() < 0.5f&&ptr_.touch.y() > -0.5f));
									//セレクター
									mine.selkey = std::clamp<uint8_t>(mine.selkey + 1, 0, (((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_.touch.x() > 0.5f && ptr_.touch.y() < 0.5f&&ptr_.touch.y() > -0.5f) && !mine.safety.first) ? 2 : 0);
								}
							}
							if (vrparts->get_hand2_num() != -1) {
								auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand2_num()];
								if (ptr_.turn && ptr_.now) {
									//マガジン取得
									mine.down_mag |= ((ptr_.on[0] & BUTTON_TRIGGER) != 0);
									//タイマーオン
									c_ready |= ((ptr_.on[1] & BUTTON_SIDE) != 0);
									//計測リセット
									if (c_end) {
										if ((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) {
											point = 0;
											p_up = 0;
											p_down = 0;
											c_ready = false;
											c_readytimer = 3.f;
											c_start = false;
											c_end = false;
											c_timer = 30.f;
										}
									}
								}
							}
						}
						else {
							//引き金(左クリック)
							easing_set(&mine.obj.get_anime(2).per, float((GetMouseInput() & MOUSE_INPUT_LEFT) != 0), 0.5f, fps);
							//ADS
							ads.first = (GetMouseInput() & MOUSE_INPUT_RIGHT) != 0;
							//マグキャッチ(Rキー)
							easing_set(&mine.obj.get_anime(5).per, float(CheckHitKey(KEY_INPUT_R) != 0), 0.5f, fps);
							//セフティ
							mine.safety.get_in(false);
							//セレクター(中ボタン)
							mine.selkey = std::clamp<uint8_t>(mine.selkey + 1, 0, ((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0) ? 2 : 0);
							//タイマーオン
							c_ready |= (CheckHitKey(KEY_INPUT_B) != 0);
							//計測リセット
							if (c_end) {
								if (CheckHitKey(KEY_INPUT_V) != 0) {
									point = 0;
									p_up = 0;
									p_down = 0;
									c_ready = false;
									c_readytimer = 3.f;
									c_start = false;
									c_end = false;
									c_timer = 30.f;
								}
							}
						}
						//タイマー開始
						if (c_ready&&!c_start) {
							c_readytimer -= 1.f / fps;
							if (c_readytimer <= 0.f) {
								c_start = true;
							}
						}
					}
					//共通
					for (auto& c : chara) {
						{
							//1フレームずれへの暫定対処
							c.mag.SetMatrix(c.mat_mag);
							c.obj.SetMatrix(c.mat_LHAND*MATRIX_ref::Mtrans(c.pos_LHAND + c.pos));
							c.hand.SetMatrix(c.mat_HMD*MATRIX_ref::Mtrans(c.pos_HMD + c.pos));
							//
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
									c.audio.mag_down.play_3D(c.pos + c.pos_LHAND, 1.f);
									if (&c == &mine) {
										vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
									}
								}
								c.reloadf = true;
							}
							//セレクター
							if (c.selkey == 1) {
								++c.select %= c.gunptr->select.size();
							}
							//セフティ
							easing_set(&c.obj.get_anime(4).per, float(c.safety.first), 0.5f, fps);
							//射撃
							if (!c.gunf && c.ammoc >= 1) {
								if (c.gunptr->select[c.select] == 2) {//フルオート用
									c.trigger = 0;
								}
							}
							c.trigger = std::clamp<uint8_t>(c.trigger + 1, 0, (c.obj.get_anime(2).per >= 0.5f) ? 2 : 0);
							if (c.trigger == 1) {
								c.audio.trigger.play_3D(c.pos + c.pos_LHAND, 1.f);
							}
							if (c.trigger == 1 && !c.gunf && c.ammoc >= 1) {
								if (&c == &mine) {
									vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
								}
								c.ammoc--;
								++c.guncnt;
								c.gunf = true;
								if (c_start && !c_end) {
									p_down = -4;
									if (point + p_down >= 0) {
										point += p_down;
									}
								}
								//持ち手を持つとココが相殺される
								if (c.LEFT_hand) {
									c.vecadd_LHAND_p = MATRIX_ref::Vtrans(c.vecadd_LHAND_p, MATRIX_ref::RotY(deg2rad(float(-50 + GetRand(100)) / 100.f))*MATRIX_ref::RotX(deg2rad(float(-50 + GetRand(150)) / 100.f)));
								}
								else {
									c.vecadd_LHAND_p = MATRIX_ref::Vtrans(c.vecadd_LHAND_p, MATRIX_ref::RotY(deg2rad(float(-50 + GetRand(100)) / 100.f))*MATRIX_ref::RotX(deg2rad(float(300 + GetRand(700)) / 100.f)));
								}
								c.audio.shot.play_3D(c.pos + c.pos_LHAND, 1.f);
								c.audio.slide.play_3D(c.pos + c.pos_LHAND, 1.f);
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

								set_effect(&c.effcs[ef_fire], u.pos, u.vec, 0.0025f / 0.1f);
								{
									set_effect(&c.effcs_gun[c.gun_effcnt].first, u.pos, u.vec, 0.11f / 0.1f);
									set_pos_effect(&c.effcs_gun[c.gun_effcnt].first, Drawparts->get_effHandle(ef_smoke));
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

										c.audio.mag_set.play_3D(c.pos + c.pos_LHAND, 1.f);

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
								//右手
								{
									MATRIX_ref m3 = MATRIX_ref::RotZ(deg2rad(90))*c.mat_LHAND*c.mat_HMD.Inverse();
									c.hand.SetFrameLocalMatrix(1, m3*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.pos_LHAND - c.pos_HMD, c.mat_HMD.Inverse())));
								}
								//左手
								{
									float dist_ = (c.pos_RHAND - c.obj.frame(c.gunptr->frame[6].first)).size();
									if (dist_ <= 0.2f && (!c.reloadf || !c.down_mag)) {
										c.LEFT_hand = true;
										MATRIX_ref m4 = MATRIX_ref::RotZ(deg2rad(-90));
										c.hand.SetFrameLocalMatrix(17, m4*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.obj.frame(c.gunptr->frame[6].first) - (c.pos_HMD + c.pos), c.mat_HMD.Inverse())));
									}
									else {
										c.LEFT_hand = false;
										MATRIX_ref m4 = MATRIX_ref::RotZ(deg2rad(-90))*c.mat_RHAND*c.mat_HMD.Inverse();
										c.hand.SetFrameLocalMatrix(17, m4*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.pos_RHAND - (c.pos_HMD + c.pos), c.mat_HMD.Inverse())));
									}
								}
								//身体
								{
									VECTOR_ref v_ = c.mat_HMD.zvec();
									float x_1 = -sinf(c.body_yrad);
									float y_1 = cosf(c.body_yrad);
									float x_2 = v_.x();
									float y_2 = -v_.z();

									float r_ = std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
									c.body_yrad += r_ * 9.f / fps;
									//easing_set(&c.body_yrad, r_, 0.7f, fps);
									c.hand.SetFrameLocalMatrix(34,
										MATRIX_ref::RotY(DX_PI_F + c.body_yrad)*c.mat_HMD.Inverse()*
										MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(VGet(0.f, -0.15f, 0.f), c.mat_HMD.Inverse()))
									);
								}
							}

							c.mag.SetMatrix(c.mat_mag);
							c.obj.SetMatrix(c.mat_LHAND*MATRIX_ref::Mtrans(c.pos_LHAND + c.pos));
							c.hand.SetMatrix(c.mat_HMD*MATRIX_ref::Mtrans(c.pos_HMD + c.pos));
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
									for (auto& tp : tgt_pic) {
										auto q = tp.obj.CollCheck_Line(a.repos, a.pos, 0, 1);
										if (q.HitFlag == TRUE) {
											a.pos = q.HitPosition;
											//
											tp.power = (tp.obj.frame(tgt_f.first) - a.pos).y();
											tp.time = 0.f;
											if (c_start && !c_end) {
												//弾痕処理
												tp.pic.SetDraw_Screen(false);
												VECTOR_ref pvecp = (a.pos - tp.obj.frame(tgt_f.first));
												VECTOR_ref xvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.x_frame));
												VECTOR_ref yvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.y_frame));
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
														p_up = 6;
														break;
													default:
														p_up = 5;
														break;
													}
													p_up = int(float(p_up) * ((a.pos - (c.pos+c.pos_LHAND)).size() / 9.144f));
													point += p_up;
												}
											}
											//
											set_effect(&c.effcs[ef_reco], a.pos, q.Normal, 0.011f / 0.1f);
											//
											a.flug = false;
											for (auto& b : c.effcs_gun) {
												if (b.second == &a) {
													b.cnt = 2.5f;
													b.first.handle.SetPos(b.second->pos);
													break;
												}
											}
											break;
										}
									}
									if (p.HitFlag == TRUE && a.flug) {
										a.flug = false;
										for (auto& b : c.effcs_gun) {
											if (b.second == &a) {
												b.cnt = 2.5f;
												b.first.handle.SetPos(b.second->pos);
												break;
											}
										}
										set_effect(&c.gndsmkeffcs[c.gndsmk_use], a.pos, p.Normal, 0.025f / 0.1f);
										++c.gndsmk_use %= c.gndsmkeffcs.size();
									}
								}
								//消す(2秒たった、スピードが100以下、貫通が0以下)
								if (a.cnt >= 3.f || a.spec.speed_a < 0.f || a.spec.pene_a <= 0.f) {
									a.flug = false;
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
										c.audio.case_down.play_3D(c.pos + c.pos_LHAND, 1.f);
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
							if (t.id != ef_smoke) {
								set_pos_effect(&t, Drawparts->get_effHandle(int(t.id)));
							}
						}
						for (auto& t : c.gndsmkeffcs) {
							set_pos_effect(&t, Drawparts->get_gndhitHandle());
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
					//ターゲットの演算
					for (auto& tp : tgt_pic) {
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
						tp.rad = std::clamp(tp.rad + (-cos(tp.time)*deg2rad(tp.power)), deg2rad(-90), deg2rad(90));
						tp.obj.SetFrameLocalMatrix(tgt_f.first, MATRIX_ref::RotX(tp.rad)* MATRIX_ref::Mtrans(tgt_f.second));
						tp.obj.RefreshCollInfo(0, 1);

						easing_set(&tp.rad, 0.f, 0.995f, fps);
						easing_set(&tp.power, 0.f, 0.995f, fps);
						tp.time += deg2rad(180.f / fps);
					}
					//影用意
					Drawparts->Ready_Shadow(campos_buf, draw_in_shadow, VGet(10.f, 2.5f, 10.f));
					//VR空間に適用
					vrparts->Move_Player();
					//campos,camvec,camupの指定
					{
						campos_buf = mine.pos + mine.pos_HMD;
						camvec = mine.mat_HMD.zvec()*-1.f;
						camup = mine.mat_HMD.yvec();
					}
					Set3DSoundListenerPosAndFrontPosAndUpVec(campos_buf.get(), (campos_buf + camvec).get(), camup.get());
					UpdateEffekseer3D();
					//スコープ
					if (mine.gunptr->frame[4].first != INT_MAX) {
						VECTOR_ref cam = mine.obj.frame(mine.gunptr->frame[4].first);
						VECTOR_ref vec = cam - mine.mat_LHAND.zvec();
						VECTOR_ref yvec = mine.mat_LHAND.yvec();
						Hostpassparts->draw(&ScopeScreen, mapparts->sky_draw(cam, vec, yvec, (fov / 7.5f) / 4.f), draw_by_shadow, cam, vec, yvec, (fov / 7.5f) / 4.f, 100.f, 0.1f);
						mine.gunptr->mod.lenzScreen.DrawExtendGraph(0, 0, 1080, 1080, true);
					}
					UIparts->set_draw(mine, scores, c_ready, c_readytimer, c_start, c_end, c_timer, point, p_up, p_down, useVR_e);
					//タイマー処理（ほかのところに置け）
					if (c_start && !c_end) {
						c_timer -= 1.f / fps;
						if (c_timer <= 0.f) {
							c_timer = 0.f;
							c_end=true;//
							scores;
							int i = 0;
							for (auto& c : scores) {
								if (c < point) {
									break;
								}
								i++;
							}
							scores.insert(scores.begin() + i, point);
						}
					}
					if (useVR_e) {
						//VRに移す
						for (char i = 0; i < 2; i++) {
							//被写体深度描画
							campos = campos_buf + vrparts->GetEyePosition_minVR(i);
							Hostpassparts->dof(&BufScreen, mapparts->sky_draw(campos, campos + camvec, camup, fov), draw_by_shadow, campos, campos + camvec, camup, fov, 100.f, 0.1f);
							//描画
							outScreen[i].SetDraw_Screen();
							{
								Hostpassparts->bloom(BufScreen, 64);//ブルーム付き描画
								UIparts->draw();//UI
							}
							GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
							outScreen[i].DrawGraph(0, 0, false);
							ptr_DX11 = (ID3D11Texture2D*)GetUseDirect3D11BackBufferTexture2D();
							vrparts->PutEye(ptr_DX11, i);
						}
					}
					else {
						//被写体深度描画
						Hostpassparts->dof(&BufScreen, mapparts->sky_draw(campos_buf, campos_buf + camvec, camup, fov), draw_by_shadow, campos_buf, campos_buf + camvec, camup, fov, 100.f, 0.1f);
						//描画
						outScreen[1].SetDraw_Screen();
						{
							Hostpassparts->bloom(BufScreen, 64);//ブルーム付き描画
							UIparts->draw();//UI
						}
					}
					//映す
					{
						VECTOR_ref cam = mine.pos + mine.pos_HMD + MATRIX_ref::Vtrans(VGet(0.35f, 0.15f, 1.f),mine.mat_HMD);
						VECTOR_ref vec = mine.pos + mine.pos_HMD + MATRIX_ref::Vtrans(VGet(0.35f, 0.15f, 0.f), mine.mat_HMD);
						if (TPS.first) {//TPS視点
							Hostpassparts->draw(&outScreen[2], mapparts->sky_draw(cam, vec, VGet(0, 1.f, 0), fov), draw_by_shadow, cam, vec, VGet(0, 1.f, 0), fov, 100.f, 0.1f);
						}
						GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK, 0.1f, 100.f, fov, cam, vec, VGet(0, 1.f, 0));
						{
							TPS.get_in(CheckHitKey(KEY_INPUT_SPACE) != 0);
							if (TPS.first) {//TPS視点
								outScreen[2].DrawExtendGraph(0, 0, out_dispx, out_dispy, false);
								//ターゲットを映す
								UIparts->TGT_drw(tgt_pic, mine.obj.frame(mine.gunptr->frame[3].first), MATRIX_ref::Vtrans(VGet(0, 0, -1.f), mine.mat_LHAND), tgt_pic_x, tgt_pic_y);
							}
							else {//FPS視点
								outScreen[1].DrawExtendGraph(0, 0, out_dispx, out_dispy, false);
							}
							if (mine.gunptr->frame[4].first != INT_MAX) {
								ScopeScreen.DrawExtendGraph(out_dispx - 200, 0, out_dispx, 200, true);
							}
							//デバッグ
							DrawFormatString(0, 200, GetColor(0, 255, 0), " %x", ptr_DX11);
							Debugparts->end_way();
							Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
						}
					}
				}
				DXDraw::Screen_Flip();
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
			Drawparts->Delete_Shadow();
		}
	} while (ProcessMessage() == 0 && ending);
	{
		std::ofstream outputfile("data/score.txt");
		for (auto& s : scores) {
			outputfile << s << "\n";
		}
		outputfile.close();
	}
	return 0; // ソフトの終了
}
