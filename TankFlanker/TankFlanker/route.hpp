#pragma once

class main_c {
	int dispx, dispy;			//描画
	int out_dispx, out_dispy;		//ウィンドウ
	//スクリーン
	std::array<GraphHandle, 3> outScreen;
	GraphHandle BufScreen;
	GraphHandle ScopeScreen;
	//データ
	MV1 hand;
	std::vector<Mainclass::Gun> gun_data;		//GUNデータ
	std::vector<Mainclass::Chara> chara;		//キャラ
	std::vector<Mainclass::tgts> tgt_pic;		//ターゲット
	std::vector<Mainclass::Gun_item> gunitem;	//拾える銃
	//
	bool ending = true;
	int sel_g = 0;
public:
	main_c() {
		//ラムダ式のいざこざが治ったら戻す
		//
		auto settings = std::make_unique<Setting>();			//設定読み込み
		auto vrparts = std::make_unique<VRDraw>(&settings->useVR_e);	//DXLib描画
		//画面指定
		if (settings->useVR_e) {
			this->dispx = 1080 * 2;
			this->dispy = 1200 * 2;
			this->out_dispx = this->dispx * (desky * 8 / 9) / this->dispy;
			this->out_dispy = this->dispy * (desky * 8 / 9) / this->dispy;
		}
		else {
			this->dispx = deskx;
			this->dispy = desky;
			this->out_dispx = this->dispx;
			this->out_dispy = this->dispy;
		}
		auto Drawparts = std::make_unique<DXDraw>("FPS_0", this->dispx, this->dispy, this->out_dispx, this->out_dispy, 90.f, settings->shadow_e, settings->getlog_e);	/*汎用クラス*/
		auto UIparts = std::make_unique<UI>(this->out_dispx, this->out_dispy, this->dispx, this->dispy);								/*UI*/
		auto Debugparts = std::make_unique<DeBuG>(90);															/*デバッグ*/
		auto Hostpassparts = std::make_unique<HostPassEffect>(settings->dof_e, settings->bloom_e, this->dispx, this->dispy);						/*ホストパスエフェクト*/
		this->outScreen[0] = GraphHandle::Make(this->dispx, this->dispy);			//左目
		this->outScreen[1] = GraphHandle::Make(this->dispx, this->dispy);			//右目
		this->outScreen[2] = GraphHandle::Make(this->dispx, this->dispy);			//TPS用
		this->BufScreen = GraphHandle::Make(this->dispx, this->dispy);				//
		this->ScopeScreen = GraphHandle::Make(1080, 1080);					//
		settings->ready_draw_setting();								//セッティング
		MV1::Load("data/model/hand/model_h.mv1", &hand, true);					//身体
		auto mapparts = std::make_unique<Mapclass>(this->dispx, this->dispy);			//map
		auto tgtparts = std::make_unique<Mainclass::tgttmp>();					//ターゲット
		//GUNデータ
		gun_data.resize(4);
		gun_data[0].name = "Knife";
		gun_data[1].name = "1911";
		gun_data[2].name = "M82A2";
		gun_data[3].name = "CAR15_M4";
		for (auto& g : gun_data) {
			g.mod.set(g.name);
		}
		//ロード画面
		UIparts->load_window("銃モデル");
		//GUNデータ取得
		for (auto& g : gun_data) {
			g.set_data();
		}
		tgtparts->set();						//ターゲット
		auto scoreparts = std::make_unique<Mainclass::scores>();	//スコア
		vrparts->Set_Device();						//VRセット
		do {
			//キャラ設定
			this->sel_g = UIparts->select_window(settings->useVR_e, gun_data, vrparts, settings);
			chara.resize(1);
			if (this->sel_g >= 0) {
				chara[0].set_list(&gun_data[this->sel_g]);
				chara[0].set_chara(VGet(0, 0, -0.5f), 0, this->ScopeScreen, hand);
				chara[0].mat_HMD = MATRIX_ref::RotY(deg2rad(180));
				this->sel_g = 0;
			}
			else {
				break;
			}
			gunitem.resize(4);
			gunitem[0].set_chara(VGet( 2.f, 1.f, 0.0f), &gun_data[0]);
			gunitem[1].set_chara(VGet( 0.f, 1.f, 0.0f), &gun_data[1]);
			gunitem[2].set_chara(VGet(-2.f, 1.f, 0.0f), &gun_data[2]);
			gunitem[3].set_chara(VGet(-4.f, 1.f, 0.0f), &gun_data[3]);
			//マップ読み込み
			mapparts->set_map_pre();
			UIparts->load_window("マップモデル");
			mapparts->set_map();
			//ターゲット
			{
				tgt_pic.resize(5);
				for (auto& p : tgt_pic) {
					p.set(tgtparts);
				}
				tgt_pic[0].obj.SetPosition(VGet(4, 0, 12.f));
				tgt_pic[1].obj.SetPosition(VGet(-4, 0, 18.f));
				tgt_pic[2].obj.SetPosition(VGet(2, 0, 27.f));
				tgt_pic[3].obj.SetPosition(VGet(-2, 0, 36.f));
				tgt_pic[4].obj.SetPosition(VGet(0, 0, 45.f));
				for (auto& p : tgt_pic) {
					p.obj.SetPosition(mapparts->map_col_line(p.obj.GetPosition() - VGet(0, -10.f, 0), p.obj.GetPosition() - VGet(0, 10.f, 0), 0).HitPosition);
				}
			}
			//ライティング
			Drawparts->Set_Light_Shadow(settings->shadow_level_e, VGet(50.f, 20.f, 50.f), VGet(0.05f, -0.5f, 0.75f), [&mapparts] {mapparts->map_get().DrawModel(); });
			//影に描画するものを指定する(仮)
			auto draw_in_shadow = [&] {
				for (auto& p : this->tgt_pic) {
					p.obj.DrawModel();
				}
				for (auto& c : this->chara) {
					c.hand.DrawModel();
					if (c.gunptr->cate == 1) {
						c.mag.DrawModel();
					}
					c.obj.DrawModel();
					if (c.gunptr->cate == 1) {
						for (auto& a : c.ammo) {
							if (a.cnt < 0.f) { continue; }
							a.second.DrawModel();
						}
					}
				}
				for (auto& g : this->gunitem) {
					if (g.gunptr != nullptr) {
						g.obj.DrawModel();
					}
				}
			};
			auto draw_by_shadow = [&] {
				Drawparts->Draw_by_Shadow([&] {
					SetFogStartEnd(0.0f, 300.f);
					SetFogColor(128, 128, 128);
					mapparts->map_get().DrawModel();
					for (auto& p : this->tgt_pic) {
						p.obj.DrawModel();
					}
					for (auto& c : this->chara) {
						c.hand.DrawModel();
						if (c.gunptr->cate == 1) {
							c.mag.DrawModel();
						}
						c.obj.DrawModel();
						if (c.gunptr->cate == 1) {
							for (auto& a : c.ammo) {
								if (a.cnt < 0.f) { continue; }
								a.second.DrawModel();
							}
						}
					}
					for (auto& g : this->gunitem) {
						if (g.gunptr != nullptr) {
							g.obj.DrawModel();
						}
					}
					//銃弾
					SetFogEnable(FALSE);
					SetUseLighting(FALSE);
					for (auto& c : this->chara) {
						if (c.gunptr->cate == 1) {
							for (auto& a : c.bullet) {
								if (!a.flug) { continue; }
								DXDraw::Capsule3D(a.pos, a.repos, ((a.spec.caliber_a - 0.00762f) * 0.1f + 0.00762f), a.color, GetColor(255, 255, 255));
							}
						}
					}
					SetUseLighting(TRUE);
					SetFogEnable(TRUE);
				});
			};
			//開始
			{
				auto& mine = chara[0];
				mine.safety.first = false;
				mine.safety.second = 0;
				scoreparts->reset();
				//プレイヤー操作変数群
				switchs TPS, ads,chgun;						//
				uint8_t change_gun = 0;						//
				VECTOR_ref gunpos_TPS;						//
				float xrad_p = 0.f;						//マウスエイム
				VECTOR_ref add_pos, add_pos_buf;				//移動
				VECTOR_ref campos, campos_buf, camvec, camup, campos_TPS;	//カメラ
				float fov = deg2rad(settings->useVR_e ? 90 : 45);		//
				float fov_fps = fov;						//

				TPS.first = true;
				SetMousePoint(deskx / 2, desky / 2);
				//envi
				mapparts->start_map();
				//
				for (auto& tp : tgt_pic) {
					//tp.isMOVE=false;
				}
				while (ProcessMessage() == 0) {
					const auto fps = GetFPS();
					const auto waits = GetNowHiPerformanceCount();
					Debugparts->put_way();
					{
						//プレイヤー操作
						{
							//銃変更
							{
								if (change_gun == 1) {
									auto pos = mine.pos;
									++this->sel_g%=mine.gunptr_have.size();
									if (mine.gunptr_have[this->sel_g] == nullptr) {
										this->sel_g = 0;
									}
									mine.delete_chara();
									mine.set_chara(VGet(0, 0, 0), this->sel_g, this->ScopeScreen, hand);
									mine.pos = pos;
									gunpos_TPS = VGet(0, 0, 0);
								}
							}
							if (settings->useVR_e) {
								//HMD
								vrparts->GetDevicePositionVR(vrparts->get_hmd_num(), &mine.pos_HMD, &mine.mat_HMD);
								//移動
								{
									if (vrparts->get_hand2_num() != -1) {
										auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand2_num()];
										if (ptr_.turn && ptr_.now) {
											if ((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) {
												easing_set(&add_pos_buf,
													(
													mine.mat_HMD.zvec()*ptr_.touch.y() +
													mine.mat_HMD.xvec()*ptr_.touch.x()
													)*-4.f / fps, 0.95f, fps);
											}
											else {
												easing_set(&add_pos_buf, VGet(0, 0, 0), 0.95f, fps);
											}
											if (mine.add_ypos == 0.f) {
												if ((ptr_.on[0] & BUTTON_SIDE) != 0) {
													mine.add_ypos = 0.05f;
												}
												add_pos = add_pos_buf;
											}
											else {
												easing_set(&add_pos, VGet(0, 0, 0), 0.995f, fps);
											}
										}
									}
								}
							}
							else {
								//HMD
								{
									mine.pos_HMD = VGet(0.f, 1.5f, 0.f);
									int x_m, y_m;
									GetMousePoint(&x_m, &y_m);
									mine.mat_HMD = MATRIX_ref::RotX(-xrad_p)*mine.mat_HMD;
									xrad_p = std::clamp(xrad_p - deg2rad(y_m - desky / 2)*0.1f*fov_fps / fov, deg2rad(-45), deg2rad(45));
									mine.mat_HMD *= MATRIX_ref::RotY(deg2rad(x_m - deskx / 2)*0.1f*fov_fps / fov);
									mine.mat_HMD = MATRIX_ref::RotX(xrad_p)*mine.mat_HMD;
									SetMousePoint(deskx / 2, desky / 2);
									SetMouseDispFlag(FALSE);
								}
								//移動
								{
									auto wkey = (CheckHitKey(KEY_INPUT_W) != 0);
									auto skey = (CheckHitKey(KEY_INPUT_S) != 0);
									auto akey = (CheckHitKey(KEY_INPUT_A) != 0);
									auto dkey = (CheckHitKey(KEY_INPUT_D) != 0);
									auto jampkey = (CheckHitKey(KEY_INPUT_SPACE) != 0);
									auto runkey = (CheckHitKey(KEY_INPUT_LSHIFT) != 0);
									if (wkey) {
										easing_set(&add_pos_buf, mine.mat_HMD.zvec()*-4.f / fps, 0.95f, fps);
									}
									if (skey) {
										easing_set(&add_pos_buf, mine.mat_HMD.zvec()*4.f / fps, 0.95f, fps);
									}
									if (akey) {
										easing_set(&add_pos_buf, mine.mat_HMD.xvec()*4.f / fps, 0.95f, fps);
									}
									if (dkey) {
										easing_set(&add_pos_buf, mine.mat_HMD.xvec()*-4.f / fps, 0.95f, fps);
									}
									if (!wkey && !skey && !akey && !dkey) {
										easing_set(&add_pos_buf, VGet(0, 0, 0), 0.95f, fps);
									}
									if (mine.add_ypos == 0.f) {
										if (jampkey) {
											mine.add_ypos = 0.05f;
										}
										add_pos = add_pos_buf;
									}
									else {
										easing_set(&add_pos, VGet(0, 0, 0), 0.995f, fps);
									}
								}
							}
							//移動共通
							{
								mine.pos += add_pos;
								auto pp = mapparts->map_col_line(mine.pos + VGet(0, 1.f, 0), mine.pos + VGet(0, -0.1f, 0), 0);
								if (mine.add_ypos <= 0.f && pp.HitFlag == 1) {
									mine.pos = pp.HitPosition;
									mine.add_ypos = 0.f;
								}
								else {
									mine.pos.yadd(mine.add_ypos);
									mine.add_ypos -= 9.8f / std::powf(fps, 2.f);
									if (mine.pos.y() <= -5.f) {
										mine.pos = VGet(0.f, 5.f, 0.f);
										mine.add_ypos = 0.f;
									}
								}
							}
							//
							if (settings->useVR_e) {
								//LHAND
								{
									vrparts->GetDevicePositionVR(vrparts->get_hand1_num(), &mine.pos_LHAND, &mine.mat_LHAND);
									mine.mat_LHAND = mine.mat_LHAND*MATRIX_ref::RotAxis(mine.mat_LHAND.xvec(), deg2rad(-60));
									easing_set(&campos_TPS, VGet(-0.35f, 0.15f, 1.f), 0.95f, fps);
								}
								//RHAND
								{
									vrparts->GetDevicePositionVR(vrparts->get_hand2_num(), &mine.pos_RHAND, &mine.mat_RHAND);
									mine.mat_RHAND = mine.mat_RHAND*MATRIX_ref::RotAxis(mine.mat_RHAND.xvec(), deg2rad(-60));
								}
							}
							else {
								//LHAND
								{
									VECTOR_ref pv = VGet(0, 0, 0);
									if (mine.gunptr->frame[4].first != INT_MAX) {
										pv = mine.gunptr->frame[4].second;
									}
									else if (mine.gunptr->frame[7].first != INT_MAX) {
										pv = mine.gunptr->frame[7].second;
									}
									if (ads.first) {
										easing_set(&gunpos_TPS, mine.pos_HMD + VGet(-0.035f, 0.f - pv.y(), -0.3f), 0.75f, fps);
										easing_set(&fov_fps, (fov*0.6f) / ((mine.gunptr->frame[4].first != INT_MAX) ? 4.f : 1.f), 0.9f, fps);
										easing_set(&campos_TPS, VGet(-0.35f, 0.15f, 0.5f), 0.9f, fps);
									}
									else {
										easing_set(&gunpos_TPS, mine.pos_HMD + VGet(-0.15f, -0.05f - pv.y(), -0.5f), 0.75f, fps);
										easing_set(&fov_fps, fov, 0.9f, fps);
										easing_set(&campos_TPS, VGet(-0.35f, 0.15f, 1.f), 0.95f, fps);
									}
									mine.pos_LHAND = mine.pos_HMD + MATRIX_ref::Vtrans(gunpos_TPS - mine.pos_HMD, mine.mat_HMD);
									mine.mat_LHAND = mine.mat_HMD;
								}
								//RHAND
								{
									mine.pos_RHAND = mine.obj.frame(mine.gunptr->frame[6].first) - mine.pos;
									mine.mat_RHAND = mine.mat_HMD;
								}
							}
							//LHAND共通
							mine.mat_LHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), mine.vecadd_LHAND)*mine.mat_LHAND;//リコイル
							//銃共通
							if (mine.obj.get_anime(3).per == 1.f) {
								mine.audio.slide.play_3D(mine.pos + mine.pos_LHAND, 1.f);
							}
							mine.obj.get_anime(3).per = std::max(mine.obj.get_anime(3).per - 12.f / fps, 0.f);
							if (settings->useVR_e) {
								if (vrparts->get_hand1_num() != -1) {
									auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand1_num()];
									if (ptr_.turn && ptr_.now) {
										//引き金
										easing_set(&mine.obj.get_anime(2).per, float((ptr_.on[0] & BUTTON_TRIGGER) != 0 && !mine.safety.first), 0.5f, fps);
										//マグキャッチ
										easing_set(&mine.obj.get_anime(5).per, float((ptr_.on[0] & BUTTON_SIDE) != 0), 0.5f, fps);
										//セフティ
										mine.safety.get_in(((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_.touch.x() < -0.5f && ptr_.touch.y() < 0.5f&&ptr_.touch.y() > -0.5f));
										//セレクター
										mine.selkey = std::clamp<uint8_t>(mine.selkey + 1, 0, (((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_.touch.x() > 0.5f && ptr_.touch.y() < 0.5f&&ptr_.touch.y() > -0.5f) && !mine.safety.first) ? 2 : 0);
										//武装変更
										change_gun = std::clamp<uint8_t>(change_gun + 1, 0, ((ptr_.on[0] & BUTTON_TOPBUTTON) != 0) ? 2 : 0);
									}
								}
								if (vrparts->get_hand2_num() != -1) {
									auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand2_num()];
									if (ptr_.turn && ptr_.now) {
										//マガジン取得
										mine.down_mag |= ((ptr_.on[0] & BUTTON_TRIGGER) != 0);
										//タイマーオン
										scoreparts->c_ready |= ((ptr_.on[0] & BUTTON_SIDE) != 0);
										//計測リセット
										if ((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) {
											if (scoreparts->c_end) {
												scoreparts->reset();
											}
										}
										//銃変更
										chgun.get_in((ptr_.on[0] & BUTTON_TOPBUTTON) != 0);
									}
								}
							}
							else {
								//引き金(左クリック)
								easing_set(&mine.obj.get_anime(2).per, float((GetMouseInput() & MOUSE_INPUT_LEFT) != 0 && !mine.safety.first), 0.5f, fps);
								//ADS
								if (mine.gunptr->cate == 1) {
									ads.first = (GetMouseInput() & MOUSE_INPUT_RIGHT) != 0;
								}
								//マグキャッチ(Rキー)
								easing_set(&mine.obj.get_anime(5).per, float(CheckHitKey(KEY_INPUT_R) != 0), 0.5f, fps);
								//セフティ(Tキー)
								if (mine.gunptr->cate == 1) {
									mine.safety.get_in(CheckHitKey(KEY_INPUT_T) != 0);
								}
								else {
									mine.safety.first = false;
									mine.safety.second = 0;
								}
								//セレクター(中ボタン)
								mine.selkey = std::clamp<uint8_t>(mine.selkey + 1, 0, ((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0) ? 2 : 0);
								//タイマーオン(Bキー)
								scoreparts->c_ready |= (CheckHitKey(KEY_INPUT_B) != 0);
								//計測リセット(Vキー)
								if (CheckHitKey(KEY_INPUT_V) != 0) {
									if (scoreparts->c_end) {
										scoreparts->reset();
									}
								}
								//銃変更
								chgun.get_in(CheckHitKey(KEY_INPUT_F) != 0);
								//武装変更
								change_gun = std::clamp<uint8_t>(change_gun + 1, 0, (CheckHitKey(KEY_INPUT_P) != 0) ? 2 : 0);
							}
							//タイマー処理
							scoreparts->move_timer();
						}
						//共通
						for (auto& c : chara) {
							{
								//1フレームずれへの暫定対処
								c.mag.SetMatrix(c.mat_mag* MATRIX_ref::Mtrans(c.pos_mag));
								c.obj.SetMatrix(c.mat_LHAND*MATRIX_ref::Mtrans(c.pos_LHAND + c.pos));
								c.hand.SetMatrix(c.mat_HMD*MATRIX_ref::Mtrans(c.pos_HMD + c.pos));
								//
								easing_set(&c.vecadd_LHAND, c.vecadd_LHAND_p, 0.9f, fps);
								easing_set(&c.vecadd_LHAND_p, VGet(0, 0, 1.f), 0.975f, fps);
								//リコイル
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
								if (c.gunptr->cate == 1) {
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
								switch (c.gunptr->cate) {
								case 0:
									{
										//近接
										if (c.trigger == 1 && !c.gunf) {
											if (&c == &mine) {
												vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
											}
											c.gunf = true;
											if (scoreparts->c_start && !scoreparts->c_end) {
												scoreparts->sub(-4);
											}
											c.audio.shot.play_3D(c.pos + c.pos_LHAND, 1.f);
											c.audio.slide.play_3D(c.pos + c.pos_LHAND, 1.f);
										}
										break;
									}
								case 1:
									{
										if (c.trigger == 1 && !c.gunf && c.ammoc >= 1) {
											if (&c == &mine) {
												vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
											}
											c.ammoc--;
											++c.guncnt;
											c.gunf = true;
											if (scoreparts->c_start && !scoreparts->c_end) {
												scoreparts->sub(-4);
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
												c.pos_mag += c.add_mag;
												c.add_mag.yadd(-9.8f / powf(fps, 2.f));
												auto pp = mapparts->map_col_line(c.pos_mag + VGet(0, 1.f, 0), c.pos_mag- VGet(0, 0.05f, 0) , 0);
												if (pp.HitFlag == 1) {
													c.pos_mag = VECTOR_ref(pp.HitPosition) + VGet(0, 0.05f, 0);
													c.add_mag += VECTOR_ref(pp.Normal)*(VECTOR_ref(pp.Normal).dot(c.add_mag*-1.f)*1.2f);
													easing_set(&c.add_mag, VGet(0, 0, 0), 0.9f, fps);
												}
											}
											else {
												auto p = MATRIX_ref::RotVec2(c.mat_RHAND.yvec(), (c.obj.frame(c.gunptr->frame[0].first) - (c.pos_RHAND + c.pos)));
												VECTOR_ref xvec = MATRIX_ref::Vtrans(c.mat_RHAND.xvec(), p);
												VECTOR_ref yvec = MATRIX_ref::Vtrans(c.mat_RHAND.yvec(), p);
												VECTOR_ref zvec = MATRIX_ref::Vtrans(c.mat_RHAND.zvec(), p);

												c.mat_mag = c.mag.GetFrameLocalMatrix(3)* MATRIX_ref::Axis1(xvec, yvec, zvec);
												c.pos_mag = c.pos_RHAND + c.pos;
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
											c.mat_mag = c.mat_LHAND;
											c.down_mag = false;
											c.add_mag = (c.obj.frame(c.gunptr->frame[1].first) - c.obj.frame(c.gunptr->frame[0].first)).Norm()*-1.f / fps;
											c.pos_mag = c.obj.frame(c.gunptr->frame[1].first);
										}
										break;
									}
								default:
									break;
								}
								{
									//右手
									{
										MATRIX_ref m3 = MATRIX_ref::RotZ(deg2rad(90))*c.mat_LHAND*c.mat_HMD.Inverse();
										c.hand.SetFrameLocalMatrix(1, m3*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.pos_LHAND - c.pos_HMD, c.mat_HMD.Inverse())));
									}
									//左手
									{
										float dist_ = ((c.pos_RHAND + c.pos) - c.obj.frame(c.gunptr->frame[6].first)).size();
										if (dist_ <= 0.2f && (!c.reloadf || !c.down_mag)) {
											c.LEFT_hand = true;
											MATRIX_ref m4 = MATRIX_ref::RotZ(deg2rad(-90));
											c.hand.SetFrameLocalMatrix(17, m4*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.obj.frame(c.gunptr->frame[6].first) - (c.pos_HMD + c.pos), c.mat_HMD.Inverse())));
										}
										else {
											c.LEFT_hand = false;
											MATRIX_ref m4 = MATRIX_ref::RotZ(deg2rad(-90))*c.mat_RHAND*c.mat_HMD.Inverse();
											c.hand.SetFrameLocalMatrix(17, m4*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.pos_RHAND - c.pos_HMD, c.mat_HMD.Inverse())));
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
										c.body_yrad += r_ * 90.f / fps / 10.f;
										c.hand.SetFrameLocalMatrix(34,
											MATRIX_ref::RotY(DX_PI_F + c.body_yrad)*c.mat_HMD.Inverse()*
											MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(VGet(0.f, -0.15f, 0.f), c.mat_HMD.Inverse()))
										);
									}
								}
								c.mag.SetMatrix(c.mat_mag* MATRIX_ref::Mtrans(c.pos_mag));
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
										/*
										auto p2 = mapparts->map_col_line(a.repos, a.pos, 1);
										if (p2.HitFlag == TRUE) {
											a.pos = p2.HitPosition;
											p = p2;
										}
										*/
										for (auto& tp : tgt_pic) {
											auto q = tp.obj.CollCheck_Line(a.repos, a.pos, 0, 1);
											if (q.HitFlag == TRUE) {
												a.pos = q.HitPosition;
												//
												tp.power = (tp.obj.frame(tgtparts->tgt_f.first) - a.pos).y();
												tp.time = 0.f;
												int UI_xpos = 0;
												int UI_ypos = 0;
												//弾痕処理
												{
													tp.pic.SetDraw_Screen(false);
													VECTOR_ref pvecp = (a.pos - tp.obj.frame(tgtparts->tgt_f.first));
													VECTOR_ref xvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.x_frame));
													VECTOR_ref yvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.y_frame));
													UI_xpos = int(float(tgtparts->tgt_pic_x)*(xvecp.Norm().dot(pvecp)) / xvecp.size());//X方向
													UI_ypos = int(float(tgtparts->tgt_pic_y)*(yvecp.Norm().dot(pvecp)) / yvecp.size());//Y方向
													DrawCircle(UI_xpos, UI_ypos, 10, GetColor(255, 0, 0));//弾痕
												}
												//ポイント判定
												if (scoreparts->c_start && !scoreparts->c_end) {
													int r_, g_, b_, a_;
													GetPixelSoftImage(tgtparts->tgt_col, UI_xpos, UI_ypos, &r_, &g_, &b_, &a_);
													int pt = 0;
													switch (r_) {
													case 0:
														pt = 15;
														break;
													case 44:
														pt = 10;
														break;
													case 86:
														pt = 9;
														break;
													case 128:
														pt = 8;
														break;
													case 170:
														pt = 7;
														break;
													case 212:
														pt = 6;
														break;
													default:
														pt = 5;
														break;
													}
													scoreparts->add(int(float(pt) * ((a.pos - (c.pos + c.pos_LHAND)).size() / 9.144f)));
												}
												//
												set_effect(&c.effcs[ef_reco], a.pos, q.Normal, 0.011f / 0.1f);
												//
												a.hit = true;
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
										easing_set(&a.add, VGet(0, 0, 0), 0.95f, fps);
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
						//gunitem
						for (auto& c : chara) {
							c.canget = false;
						}
						for (auto& g : this->gunitem) {
							//
							if (g.gunptr != nullptr) {
								g.obj.SetMatrix(MATRIX_ref::RotZ(deg2rad(90))*g.mat*MATRIX_ref::Mtrans(g.pos));
								g.pos += g.add;
								g.add.yadd(-9.8f / powf(fps, 2.f));
								auto pp = mapparts->map_col_line(g.pos + VGet(0, 1.f, 0), g.pos - VGet(0, 0.05f, 0), 0);
								if (pp.HitFlag == 1) {
									g.pos = VECTOR_ref(pp.HitPosition) + VGet(0, 0.05f, 0);
									g.mat *= MATRIX_ref::RotVec2(g.mat.yvec(), VECTOR_ref(pp.Normal));
									easing_set(&g.add, VGet(0, 0, 0), 0.5f, fps);
								}
								//
								for (auto& c : chara) {
									c.canget |= ((c.pos + c.pos_RHAND) - g.pos).size() <= 1.5f;
									if (((c.pos + c.pos_RHAND) - g.pos).size() <= 1.5f) {
										c.canget_gun = g.gunptr->name;
										if (&c == &mine) {
											if (chgun.second == 1) {
												for (size_t i = 0; i < mine.gunptr_have.size(); i++) {
													if (mine.gunptr_have[i] == nullptr) {
														VECTOR_ref pos_t = c.pos;
														c.gunptr_have[i] = g.gunptr;
														g.delete_chara();//

														c.delete_chara();
														c.set_chara(pos_t, i, this->ScopeScreen, hand);
														gunpos_TPS = VGet(0, 0, 0);
														this->sel_g = i;
														break;
													}
													if (i == mine.gunptr_have.size() - 1) {
														//
														Mainclass::Gun*ptr = g.gunptr;
														g.delete_chara();
														g.set_chara(c.pos + c.pos_LHAND, c.gunptr);
														g.mat = c.mat_LHAND;
														//
														VECTOR_ref pos_t = c.pos;
														MATRIX_ref mat_t = c.mat_LHAND;
														c.delete_chara();
														c.gunptr_have[this->sel_g] = ptr;
														c.set_chara(pos_t, this->sel_g, this->ScopeScreen, hand);
														gunpos_TPS = VGet(0, 0, 0);
													}
												}
											}
										}
									}
								}
							}
						}
						//ターゲットの演算
						for (auto& tp : tgt_pic) {
							if (tp.isMOVE) {
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
							tp.obj.SetFrameLocalMatrix(tgtparts->tgt_f.first, MATRIX_ref::RotX(tp.rad)* MATRIX_ref::Mtrans(tgtparts->tgt_f.second));
							tp.obj.RefreshCollInfo(0, 1);

							easing_set(&tp.rad, 0.f, 0.995f, fps);
							easing_set(&tp.power, 0.f, 0.995f, fps);
							tp.time += deg2rad(180.f / fps);
						}
						//campos,camvec,camupの指定
						campos_buf = mine.pos + mine.pos_HMD;
						camvec = mine.mat_HMD.zvec()*-1.f;
						camup = mine.mat_HMD.yvec();
						Set3DSoundListenerPosAndFrontPosAndUpVec(campos_buf.get(), (campos_buf + camvec).get(), camup.get());
						UpdateEffekseer3D();
						//影用意
						Drawparts->Ready_Shadow(campos_buf, draw_in_shadow, VGet(5.f, 2.5f, 5.f));
						//VR空間に適用
						vrparts->Move_Player();
						//スコープ
						if (mine.gunptr->frame[4].first != INT_MAX) {
							VECTOR_ref cam = mine.obj.frame(mine.gunptr->frame[4].first);
							VECTOR_ref vec = cam - mine.mat_LHAND.zvec();
							VECTOR_ref yvec = mine.mat_LHAND.yvec();
							Hostpassparts->draw(&this->ScopeScreen, mapparts->sky_draw(cam, vec, yvec, (fov / 7.5f) / 4.f), draw_by_shadow, cam, vec, yvec, (fov / 7.5f) / 4.f, 100.f, 0.1f);
							mine.gunptr->mod.lenzScreen.DrawExtendGraph(0, 0, 1080, 1080, true);
						}
						//描画
						UIparts->set_draw(mine, scoreparts, settings->useVR_e);
						if (settings->useVR_e) {
							for (char i = 0; i < 2; i++) {
								campos = campos_buf + vrparts->GetEyePosition_minVR(i);
								//被写体深度描画
								Hostpassparts->dof(&this->BufScreen, mapparts->sky_draw(campos, campos + camvec, camup, fov), draw_by_shadow, campos, campos + camvec, camup, fov, 100.f, 0.1f);
								//描画
								this->outScreen[i].SetDraw_Screen(0.1f, 100.f, fov_fps, campos, campos + camvec, camup);
								{
									Hostpassparts->bloom(this->BufScreen, 64);//ブルーム付き描画
									UIparts->draw();//UI
									UIparts->Gunitem_draw(this->gunitem, campos_buf);
								}
								//VRに移す
								GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
								{
									this->outScreen[i].DrawGraph(0, 0, false);
									ID3D11Texture2D* ptr_DX11 = (ID3D11Texture2D*)GetUseDirect3D11BackBufferTexture2D();
									vrparts->PutEye(ptr_DX11, i);
								}
							}
						}
						else {
							campos = campos_buf + MATRIX_ref::Vtrans(VGet(-0.035f, 0.f, 0.f), mine.mat_HMD);
							//被写体深度描画
							Hostpassparts->dof(&this->BufScreen, mapparts->sky_draw(campos, campos + camvec, camup, fov_fps), draw_by_shadow, campos, campos + camvec, camup, fov_fps, 100.f, 0.1f);
							//描画
							this->outScreen[1].SetDraw_Screen(0.1f, 100.f, fov_fps, campos, campos + camvec, camup);
							{
								Hostpassparts->bloom(this->BufScreen, 64);//ブルーム付き描画
								UIparts->draw();//UI
								UIparts->Gunitem_draw(this->gunitem,campos_buf);
							}
						}
						//ディスプレイ描画
						{
							TPS.get_in(CheckHitKey(KEY_INPUT_LCONTROL) != 0);

							VECTOR_ref cam = mine.pos + mine.pos_HMD + MATRIX_ref::Vtrans(campos_TPS, mine.mat_HMD);
							VECTOR_ref vec = mine.pos + mine.pos_HMD + MATRIX_ref::Vtrans(VGet(-0.35f, 0.15f, 0.f), mine.mat_HMD);
							if (TPS.first) {//TPS視点
								Hostpassparts->draw(&this->outScreen[2], mapparts->sky_draw(cam, vec, VGet(0, 1.f, 0), fov), draw_by_shadow, cam, vec, VGet(0, 1.f, 0), fov, 100.f, 0.1f);
								GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK, 0.1f, 100.f, fov, cam, vec, VGet(0, 1.f, 0));
								{
									this->outScreen[2].DrawExtendGraph(0, 0, this->out_dispx, this->out_dispy, false);
									//ターゲットを映す
									UIparts->TGT_drw(tgt_pic, mine.obj.frame(mine.gunptr->frame[3].first), MATRIX_ref::Vtrans(VGet(0, 0, -1.f), mine.mat_LHAND), tgtparts->tgt_pic_x, tgtparts->tgt_pic_y);
									//スコープのエイム
									if (mine.gunptr->frame[4].first != INT_MAX) {
										this->ScopeScreen.DrawExtendGraph(this->out_dispx - 200, 0, this->out_dispx, 200, true);
									}
								}
							}
							else {//FPS視点
								GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
								{
									this->outScreen[1].DrawExtendGraph(0, 0, this->out_dispx, this->out_dispy, false);
								}
							}
							//デバッグ
							Debugparts->end_way();
							Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
						}
					}
					DXDraw::Screen_Flip();
					vrparts->Eye_Flip(waits);//フレーム開始の数ミリ秒前にstartするまでブロックし、レンダリングを開始する直前に呼び出す必要があります。
					if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
						this->ending = false;
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
		} while (ProcessMessage() == 0 && this->ending);
	}
};