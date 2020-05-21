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
	MV1::Load("data/1911/model.mv1", &gun_data.back().obj, true);
	MV1::Load("data/1911/mag.mv1", &gun_data.back().mag, true);
	UIparts->load_window("モデル");					//ロード画面

	gun_data.back().frame.resize(gun_data.back().frame.size() + 1);
	gun_data.back().frame.back().first = 12;
	gun_data.back().frame.back().second = MATRIX_ref::Vtrans(VGet(0, 0, 0), gun_data.back().obj.GetFrameLocalMatrix(gun_data.back().frame.back().first));

	gun_data.back().frame.resize(gun_data.back().frame.size() + 1);
	gun_data.back().frame.back().first = 13;
	gun_data.back().frame.back().second = MATRIX_ref::Vtrans(VGet(0, 0, 0), gun_data.back().obj.GetFrameLocalMatrix(gun_data.back().frame.back().first));
	gun_data.back().ammo_max = 7;

	//VRセット
	vrparts->Set_Device();
	VECTOR_ref HMDpos;
	MATRIX_ref HMDmat;
	bool HMDon;

	VECTOR_ref LHANDpos, LHANDxvec, LHANDyvec, LHANDzvec;
	bool LHANDon;

	VECTOR_ref RHANDpos, RHANDxvec, RHANDyvec, RHANDzvec;
	bool RHANDon;
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
			Drawparts->Set_Shadow(13, VGet(10.f,10.f,10.f), VGet(0.05f, -0.25f, 0.75f), [] {});
		}
		//キャラ設定
		chara[0].pos = VGet(0, 0, 0);
		chara[0].mat = MGetIdent();
		chara[0].gunptr = &gun_data[0];
		for (auto& c : chara) {
			c.gunptr->obj.DuplicateonAnime(&c.obj);
			c.mag = c.gunptr->mag.Duplicate();
			c.guncnt = 0;
			c.ammos = c.gunptr->ammo_max + 1;
			c.gunf = false;
			c.reloadf = false;
			c.down_mag = true;
			for (auto& a : c.obj.get_anime()) {
				a.per = 0.f;
			}
			fill_id(c.effcs);			      //エフェクト
			for (auto& a : c.bullet) {
				a.flug=false;
				a.color = GetColor(255, 255, 172);
			}
		}
		//影に描画するものを指定する(仮)
		auto draw_in_shadow = [&mapparts, &chara] {
			mapparts->map_get().DrawModel();
			for (auto& c : chara) {
				c.mag.DrawModel();
				c.obj.DrawModel();
			}
		};
		auto draw_on_shadow = [&mapparts, &chara, &campos, &vrparts] {
			//マップ
			SetFogStartEnd(0.0f, 3000.f);
			SetFogColor(128, 128, 128);
			{
				mapparts->map_get().DrawModel();
			}
			//機体
			SetFogStartEnd(0.0f, 3000.f);
			SetFogColor(128, 128, 128);
			for (auto& c : chara) {
				c.mag.DrawModel();
				c.obj.DrawModel();
			}
			//銃弾
			SetFogEnable(FALSE);
			SetUseLighting(FALSE);
			for (auto& c : chara) {
				for (auto& a : c.bullet) {
					if (a.flug) {
						DXDraw::Capsule3D( a.pos, a.repos, ((a.spec.caliber_a - 0.00762f) * 0.1f + 0.00762f), a.color, GetColor(255, 255, 255));
					}
				}
			}
			SetUseLighting(TRUE);
			SetFogEnable(TRUE);
			vrparts->Draw_Player();
		};
		//通信開始
		{
		}
		//開始
		auto& mine = chara[0];
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
						if (vrparts->get_hmd_num() != -1) {
							auto& ptr_HMD = (*vrparts->get_device())[vrparts->get_hmd_num()];
							HMDpos = ptr_HMD.pos;
							HMDmat = MATRIX_ref::Axis1(ptr_HMD.xvec, ptr_HMD.yvec, ptr_HMD.zvec);
							eyevec = VGet(ptr_HMD.zvec.x(), ptr_HMD.zvec.y()*-1.f, ptr_HMD.zvec.z());
							HMDon = ptr_HMD.now;
						}
						else {
							HMDpos = VGet(0, 0, 0);
							HMDmat = MATRIX_ref::Axis1(VGet(1, 0, 0), VGet(0, 1, 0), VGet(0, 0, 1));
							eyevec = VGet(0, 0, 1.f);
							HMDon = false;
						}
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
					mine.pos = VGet(0.f, 0.f, 0.f);
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
						mine.mat_LHAND = MATRIX_ref::Axis1(LHANDxvec*-1, LHANDyvec, LHANDzvec*-1);
						mine.pos_LHAND = LHANDpos;

						if (vrparts->get_right_hand_num() != -1) {
							auto& ptr_RHAND = (*vrparts->get_device())[vrparts->get_right_hand_num()];
							RHANDpos = ptr_RHAND.pos;
							RHANDxvec = MATRIX_ref::Vtrans(ptr_RHAND.xvec, MATRIX_ref::RotAxis(ptr_RHAND.xvec, deg2rad(60)));
							RHANDyvec = MATRIX_ref::Vtrans(ptr_RHAND.yvec, MATRIX_ref::RotAxis(ptr_RHAND.xvec, deg2rad(60)));
							RHANDzvec = MATRIX_ref::Vtrans(ptr_RHAND.zvec, MATRIX_ref::RotAxis(ptr_RHAND.xvec, deg2rad(60)));
							RHANDon = ptr_RHAND.now;
						}
						else {
							RHANDpos = VGet(0, 0, 0);
							RHANDxvec = VGet(1, 0, 0);
							RHANDyvec = VGet(0, 1, 0);
							RHANDzvec = VGet(0, 0, 1);
							RHANDon = false;
						}
						mine.mat_RHAND = MATRIX_ref::Axis1(RHANDxvec*-1, RHANDyvec, RHANDzvec*-1);
						mine.pos_RHAND = RHANDpos;


						mine.obj.get_anime(3).per -= (60.f /5.f)/ fps;
						if (mine.obj.get_anime(3).per <= 0.f) {
							mine.obj.get_anime(3).per = 0.f;
						}
						if (vrparts->get_left_hand_num() != -1) {
							auto& ptr_LEFTHAND = (*vrparts->get_device())[vrparts->get_left_hand_num()];
							auto& ptr_RIGHTHAND = (*vrparts->get_device())[vrparts->get_right_hand_num()];
							if (ptr_LEFTHAND.turn && ptr_LEFTHAND.now) {
								//引き金
								easing_set(&mine.obj.get_anime(2).per, float((ptr_LEFTHAND.on[0] & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger)) != 0 && !mine.safety.first), 0.5f, fps);
								//マグキャッチ
								easing_set(&mine.obj.get_anime(5).per, float((ptr_LEFTHAND.on[1] & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_IndexController_A)) != 0), 0.5f, fps);
								//サブ
								mine.safety.second = std::clamp<uint8_t>(mine.safety.second + 1, 0, (((ptr_LEFTHAND.on[0] & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Touchpad)) != 0) && (ptr_LEFTHAND.touch.x() < -0.5f && ptr_LEFTHAND.touch.y() < 0.5f&&ptr_LEFTHAND.touch.y() > -0.5f)) ? 2 : 0);
								if (mine.safety.second == 1) {
									mine.safety.first ^= 1;
								}
								easing_set(&mine.obj.get_anime(4).per, float(mine.safety.first), 0.5f, fps);
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
				}
				//共通
				for (auto& c : chara) {
					{
						if (c.gunf) {
							if (c.ammos >= 1) {
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
							if (c.ammos >= 1) {
								c.ammos = 1;
							}
							c.reloadf = true;
						}
						//射撃
						c.trigger = std::clamp<uint8_t>(c.trigger + 1, 0, (c.obj.get_anime(2).per >= 0.5f) ? 2 : 0);
						if (c.trigger == 1 && !c.gunf && c.ammos >= 1) {
							c.ammos--;
							++c.guncnt;
							c.gunf = true;

							auto& u = c.bullet[c.usebullet];
							++c.usebullet %= c.bullet.size();
							//ココだけ変化
							u.spec.caliber_a = 0.011f;
							u.spec.damage_a = 33;
							u.spec.pene_a = 10.f;
							u.spec.speed_a = 260.f;

							u.pos = c.obj.frame(17);
							u.vec = c.mat_LHAND.zvec()*-1.f;
							u.hit = false;
							u.flug = true;
							u.cnt = 0.f;
							u.yadd = 0.f;
							u.repos = u.pos;


							set_effect(&c.effcs[ef_fire], u.pos, u.vec, 0.0025f / 0.1f);
							{
								set_effect(&c.effcs_gun[c.gun_effcnt].first, u.pos, u.vec, 0.11f / 0.1f);
								set_pos_effect(&c.effcs_gun[c.gun_effcnt].first, Drawparts->get_effHandle(ef_smoke2));
								c.effcs_gun[c.gun_effcnt].second = &u;
								c.effcs_gun[c.gun_effcnt].cnt = 0;
								++c.gun_effcnt %= c.effcs_gun.size();
							}
						}
						//マガジン取得
						if (c.reloadf) {
							if (!c.down_mag) {
								c.mat_mag *= MATRIX_ref::Mtrans(c.mat_addvec*c.mat_add);
								c.mat_add += -9.8f / powf(fps, 2.f);
								easing_set(&c.mat_addvec, VGet(0, 1.f, 0), 0.95f, fps);
							}
							else {
								auto p = MATRIX_ref::RotVec2(c.mat_RHAND.yvec(), (c.obj.frame(12) - (c.pos_RHAND + c.pos)));
								VECTOR_ref xvec = MATRIX_ref::Vtrans(c.mat_RHAND.xvec(), p);
								VECTOR_ref yvec = MATRIX_ref::Vtrans(c.mat_RHAND.yvec(), p);
								VECTOR_ref zvec = MATRIX_ref::Vtrans(c.mat_RHAND.zvec(), p);

								c.mat_mag = c.mag.GetFrameLocalMatrix(3)* MATRIX_ref::Axis1(xvec, yvec, zvec)* MATRIX_ref::Mtrans(c.pos_RHAND + c.pos);

								if ((c.mag.frame(3) - c.obj.frame(12)).size() <= 0.05f) {
									c.obj.get_anime(1).time = 0.f;
									c.obj.get_anime(0).per = 1.f;
									c.obj.get_anime(1).per = 0.f;
									if (c.ammos == 0) {
										c.obj.get_anime(3).per = 1.f;
									}
									c.reloadf = false;
									c.ammos += c.gunptr->ammo_max;
								}
							}
						}
						else {
							c.mat_mag = c.mat_LHAND* MATRIX_ref::Mtrans(c.pos_LHAND + c.pos);
							c.down_mag = false;
							c.mat_add = 0.f;
							c.mat_addvec = (c.obj.frame(13) - c.obj.frame(12)).Norm();
						}
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
							if (a.cnt >= 2.f || a.spec.speed_a < 0.f || a.spec.pene_a <= 0.f) {
								a.flug = false;
							}
							if (!a.flug) {
								for (auto& b : c.effcs_gun) {
									if (b.second == &a) {
										b.first.handle.SetPos(b.second->pos);
										break;
									}
								}
							}
						}
					}

					for (auto& t : c.effcs) {
						if (t.id != ef_smoke1 && t.id != ef_smoke2) {
							set_pos_effect(&t, Drawparts->get_effHandle(int(t.id)));
						}
					}
					for (auto& a : c.effcs_gun) {
						if (a.second != nullptr) {
							if (a.second->flug) {
								a.first.handle.SetPos(a.second->pos);
							}
							if (a.cnt != -1) {
								a.cnt++;
								if (a.cnt >= 3.f * GetFPS()) {
									a.first.handle.Stop();
									a.cnt = -1;
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
				//モデルに反映
				for (auto& c : chara) {
					c.mag.SetMatrix(c.mat_mag);
					c.obj.SetMatrix(c.mat_LHAND*MATRIX_ref::Mtrans(c.pos_LHAND+c.pos));
				}
				//影用意
				if (shadow_e) {
					Drawparts->Ready_Shadow(campos, draw_in_shadow, VGet(5.f, 2.5f, 5.f));
				}
				//VR空間に適用
				vrparts->Move_Player();
				//campos,camvec,camupの指定
				{
					campos = mine.pos;
					if (useVR_e) {
						campos += HMDpos;
						camvec = campos + HMDmat.zvec();
						camup = HMDmat.yvec();
					}
					else {
						camvec = campos + VGet(0, 0, 1.f);
						camup = VGet(0, 1.f, 0);
					}
				}
				//被写体深度描画
				{
					float fardist = 100.f;
					float neardist = 0.1f;

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
					Hostpassparts->bloom(BufScreen, 64);
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

					DrawFormatString(300, 300, GetColor(0, 255, 0), "%d/%d : %d", mine.ammos, mine.gunptr->ammo_max, mine.usebullet);
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
