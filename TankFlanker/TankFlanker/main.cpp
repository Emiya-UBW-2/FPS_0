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
	std::array<GraphHandle, 2> outScreen;
	outScreen[0] = GraphHandle::Make(dispx, dispy);    //描画スクリーン
	outScreen[1] = GraphHandle::Make(dispx, dispy);    //描画スクリーン
	//操作
	VECTOR_ref eyevec;					    //視点
	VECTOR_ref campos, camvec, camup;			    //カメラ
	VECTOR_ref campos_buf;					    //視点
	float fov = deg2rad(useVR_e ? 90 : 45);
	bool ads = false;
	float range = 0.f;
	float range_p = 0.f;
	uint8_t change_vehicle = 0;
	bool chveh = false;
	//データ
	auto mapparts = std::make_unique<Mapclass>(dispx, dispy);
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
	//
	//
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

	gun_data.back().ammo_max = 700;
	SetCreate3DSoundFlag(TRUE);
	gun_data.back().shot = SoundHandle::Load("data/audio/shot.wav");
	SetCreate3DSoundFlag(FALSE);
	//
	tgt_f = { 2,tgt.frame(2) };
	GetGraphSize(tgt_pic_tmp.get(), &tgt_pic_x, &tgt_pic_y);
	//VRセット
	vrparts->Set_Device();
	VECTOR_ref HMDpos;
	MATRIX_ref HMDmat;
	bool HMDon;
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
		//
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
				Drawparts->Set_Shadow(13, VGet(10.f, 10.f, 10.f), light, [] {});
			}
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
			c.vecadd_LHAND = VGet(0, 0, 1.f);
			c.vecadd_LHAND_p = c.vecadd_LHAND;
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
			SetCreate3DSoundFlag(TRUE);
			c.shot = c.gunptr->shot.Duplicate();
			SetCreate3DSoundFlag(FALSE);
		}
		//影に描画するものを指定する(仮)
		auto draw_in_shadow = [&mapparts, &chara] {
			mapparts->map_get().DrawModel();
			for (auto& c : chara) {
				c.mag.DrawModel();
				c.obj.DrawModel();
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
				c.mag.DrawModel();
				c.obj.DrawModel();
			}
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
							HMDon = ptr_HMD.now;
						}
						else {
							HMDpos = VGet(0, 0, 0);
							HMDmat = MATRIX_ref::Axis1(VGet(1, 0, 0), VGet(0, 1, 0), VGet(0, 0, 1));
							HMDon = false;
						}
					}
					else {
						HMDpos = VGet(0, 1.f, 0);
						HMDmat = MATRIX_ref::Axis1(VGet(1, 0, 0), VGet(0, 1, 0), VGet(0, 0, 1));
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
								mine.pos_LHAND = VGet(0, 0, 0);
								mine.mat_LHAND = MATRIX_ref::Axis1(VGet(-1, 0, 0), VGet(0, 1, 0), VGet(0, 0, -1));
								LHANDon = false;
							}
						}
						else {
							mine.pos_LHAND = VGet(0.25f, 0.95f, 0.75f);
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
								mine.pos_RHAND = VGet(0, 0, 0);
								mine.mat_RHAND = MATRIX_ref::Axis1(VGet(-1, 0, 0), VGet(0, 1, 0), VGet(0, 0, -1));
								RHANDon = false;
							}
						}

						mine.obj.get_anime(3).per -= (60.f /5.f)/ fps;
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
						else {
							easing_set(&mine.obj.get_anime(2).per, float((GetMouseInput() & MOUSE_INPUT_LEFT) != 0), 0.5f, fps);
						}
					}
				}
				//共通
				for (auto& c : chara) {
					{
						easing_set(&c.vecadd_LHAND, c.vecadd_LHAND_p, 0.9f, fps);
						easing_set(&c.vecadd_LHAND_p, VGet(0, 0, 1.f), 0.95f, fps);
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
							c.vecadd_LHAND_p = MATRIX_ref::Vtrans(c.vecadd_LHAND_p, MATRIX_ref::RotY(deg2rad(float(-50 + GetRand(100)) / 100.f))*MATRIX_ref::RotX(deg2rad(float(300 + GetRand(700)) / 100.f)));
							Set3DPositionSoundMem(c.pos_LHAND.get(), c.shot.get());
							Set3DRadiusSoundMem(1.f, c.shot.get());
							c.shot.play(DX_PLAYTYPE_BACK, TRUE);
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
								c.effcs_gun[c.gun_effcnt].cnt = 0.f;
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
							if (a.cnt >= 0.f) {
								a.cnt+=1.f/fps;
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
					{
						if (std::abs(tp.obj.GetPosition().x())>7.5f) {
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
					campos_buf = mine.pos;
					if (useVR_e) {
						campos_buf += HMDpos;
						camvec = HMDmat.zvec();
						camup = HMDmat.yvec();
					}
					else {
						campos_buf += HMDpos;
						camvec = HMDmat.zvec();
						camup = HMDmat.yvec();
					}
				}
				Set3DSoundListenerPosAndFrontPosAndUpVec(campos_buf.get(), (campos_buf + camvec).get(), camup.get());
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
					}
				}
				//VRに移す
				if (useVR_e) {
					for (char i = 0; i < 2; i++) {
						GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
						outScreen[i].DrawGraph(0, 0, false);
						vrparts->PutEye((ID3D11Texture2D*)GetUseDirect3D11BackBufferTexture2D(), i);
					}
				}
				GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
				{
					outScreen[0].DrawExtendGraph(0, 0, out_dispx, out_dispy, false);
					tgt_pic[0].pic.DrawGraph(0, 0, true);
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
