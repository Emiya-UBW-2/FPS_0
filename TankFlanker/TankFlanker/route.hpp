#pragma once

class main_c : Mainclass {
	int dispx, dispy;			//描画
	int out_dispx, out_dispy;		//ウィンドウ
	//スクリーン
	std::array<GraphHandle, 3> outScreen;
	GraphHandle BufScreen;
	GraphHandle ScopeScreen;
	//データ
	MV1 body_obj;
	std::vector<Gun> gun_data;		//GUNデータ
	std::vector<Chara> chara;		//キャラ
	std::vector<tgts> tgt_pic;		//ターゲット
	std::vector<Gun_item> gunitem;	//拾える銃
	std::vector<Mag_item> magitem;	//拾える銃
	//
	bool ending = true;
	int sel_g2 = 0;
	//プレイヤー操作変数群
	switchs TPS, ads, chgun,delgun, usegun;				//
	uint8_t change_gun = 0;						//
	VECTOR_ref gunpos_TPS;						//
	float xrad_p = 0.f;						//マウスエイム
	VECTOR_ref add_pos, add_pos_buf;				//移動
	VECTOR_ref campos, campos_buf, camvec, camup, campos_TPS;	//カメラ
	float fov, fov_fps;
public:
	main_c() {
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
		MV1::Load("data/model/hand/model_h.mv1", &body_obj, true);				//身体
		auto mapparts = std::make_unique<Mapclass>(this->dispx, this->dispy);			//map
		auto tgtparts = std::make_unique<tgttmp>();						//ターゲット
		//GUNデータ
		gun_data.resize(5);
		gun_data[0].mod.set("Knife");
		gun_data[1].mod.set("1911");
		gun_data[2].mod.set("M82A2");
		gun_data[3].mod.set("CAR15_M4");
		gun_data[4].mod.set("AK74");
		//ロード画面
		UIparts->load_window("銃モデル");
		//GUNデータ取得
		fill_id(gun_data);
		for (auto& g : gun_data) { g.set_data(); }
		//ロード画面
		UIparts->load_window("銃モデル");
		tgtparts->set();				//ターゲット
		auto scoreparts = std::make_unique<scores>();	//スコア
		vrparts->Set_Device();				//VRセット
		do {
			//キャラ設定
			{
				int sel_g = UIparts->select_window(settings->useVR_e, gun_data, vrparts, settings);
				if (sel_g < 0) { break; }
				chara.resize(1);
				chara[0].set_list(&gun_data[sel_g], &gun_data[0]);
				chara[0].gun_have_state.resize(gun_data.size());
				
				for (auto& s : chara[0].gun_have_state) {
					s.in = 0;
				}

				chara[0].set_chara(VGet(0, 0, -0.5f),MGetIdent(), 0, this->ScopeScreen, body_obj);
				chara[0].mat_HMD = MATRIX_ref::RotY(deg2rad(180));
				this->sel_g2 = 0;
				this->usegun.first = true;
				this->usegun.second = 0;
			}
			//マップ読み込み
			mapparts->set_map_pre();
			UIparts->load_window("マップモデル");
			mapparts->set_map();
			//銃アイテム配置
			gunitem.resize(5);
			gunitem[0].set(&gun_data[0], VGet(4.f, 1.f, 0.0f));
			gunitem[1].set(&gun_data[1], VGet(2.f, 1.f, 0.0f));
			gunitem[2].set(&gun_data[2], VGet(0.f, 1.f, 0.0f));
			gunitem[3].set(&gun_data[3], VGet(-2.f, 1.f, 0.0f));
			gunitem[4].set(&gun_data[4], VGet(-4.f, 1.f, 0.0f));
			//マガジン
			magitem.resize(10);
			magitem[0].set(&gun_data[3], VGet(4.f, 1.f, 4.0f));
			magitem[1].set(&gun_data[3], VGet(2.f, 1.f, 4.0f));
			magitem[2].set(&gun_data[3], VGet(0.f, 1.f, 4.0f));
			magitem[3].set(&gun_data[3], VGet(-2.f, 1.f, 4.0f));
			magitem[4].set(&gun_data[3], VGet(-4.f, 1.f, 4.0f));

			magitem[5].set(&gun_data[3], VGet(4.f, 2.f, 6.0f));
			magitem[6].set(&gun_data[3], VGet(2.f, 2.f, 6.0f));
			magitem[7].set(&gun_data[3], VGet(0.f, 2.f, 6.0f));
			magitem[8].set(&gun_data[3], VGet(-2.f, 2.f, 6.0f));
			magitem[9].set(&gun_data[3], VGet(-4.f, 2.f, 6.0f));


			for (auto& p : magitem) {
				p.cap = p.gunptr->ammo_max;
			}

			//ターゲット
			tgt_pic.resize(5);
			tgt_pic[0].set(tgtparts, VGet(4, 0, 12.f));
			tgt_pic[1].set(tgtparts, VGet(-4, 0, 18.f));
			tgt_pic[2].set(tgtparts, VGet(2, 0, 27.f));
			tgt_pic[3].set(tgtparts, VGet(-2, 0, 36.f));
			tgt_pic[4].set(tgtparts, VGet(0, 0, 45.f));
			for (auto& p : tgt_pic) {
				p.obj.SetPosition(mapparts->map_col_line(p.obj.GetPosition() - VGet(0, -10.f, 0), p.obj.GetPosition() - VGet(0, 10.f, 0), 0).HitPosition);
			}
			//ライティング
			Drawparts->Set_Light_Shadow(settings->shadow_level_e, VGet(50.f, 20.f, 50.f), VGet(0.05f, -0.5f, 0.75f), [&mapparts] {mapparts->map_get().DrawModel(); });
			//影に描画するものを指定する(仮)
			auto draw_in_shadow = [&] {
				for (auto& p : this->tgt_pic) { p.obj.DrawModel(); }
				for (auto& c : this->chara) { c.draw(); 
				
					for (size_t i = 0; i < c.gunptr_have.size(); i++) {
						if (
							(this->usegun.first || int(i) != this->sel_g2)
							&&
							c.gunptr_have[i].ptr != nullptr

							) {
							c.gunptr_have[i].obj.SetMatrix(

								MATRIX_ref::RotY(DX_PI_F)*
								MATRIX_ref::RotX(DX_PI_F / 2)*
								MATRIX_ref::RotY(c.body_yrad)*

								MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(VGet((float(i) - float(c.gunptr_have.size()) *0.5f)*0.1f, -0.15f, -0.3f), MATRIX_ref::RotY(c.body_yrad)))*
								MATRIX_ref::Mtrans(c.pos_HMD + c.pos)
							);
							c.gunptr_have[i].obj.DrawModel();
						}
					}

				}
				for (auto& g : this->gunitem) { g.draw(); }
				for (auto& g : this->magitem) { g.draw(); }
			};
			auto draw_by_shadow = [&] {
				Drawparts->Draw_by_Shadow([&] {
					mapparts->map_get().DrawModel();
					for (auto& p : this->tgt_pic) { p.obj.DrawModel(); }
					for (auto& c : this->chara) { c.draw(); 

						for (size_t i = 0; i < c.gunptr_have.size(); i++) {
							if (
								(!this->usegun.first || int(i) != this->sel_g2)
								&&
								c.gunptr_have[i].ptr != nullptr

								) {
								c.gunptr_have[i].obj.SetMatrix(

									MATRIX_ref::RotY(DX_PI_F)*
									MATRIX_ref::RotX(DX_PI_F / 2)*
									MATRIX_ref::RotY(c.body_yrad)*

									MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(VGet((float(i) - float(c.gunptr_have.size()) *0.5f)*0.1f, -0.15f, -0.3f), MATRIX_ref::RotY(c.body_yrad)))*
									MATRIX_ref::Mtrans(c.pos_HMD + c.pos)
								);
								c.gunptr_have[i].obj.DrawModel();
							}
						}

					}
					for (auto& g : this->gunitem) { g.draw(this->chara[0].canget, this->chara[0].canget_gun); }
					for (auto& g : this->magitem) { g.draw(this->chara[0].cangetm, this->chara[0].canget_mag); }
					//銃弾
					SetFogEnable(FALSE);
					SetUseLighting(FALSE);
					for (auto& c : this->chara) { c.draw_ammo(); }
					SetUseLighting(TRUE);
					SetFogEnable(TRUE);
				});
			};
			//開始
			{
				auto& mine = chara[0];
				scoreparts->reset();
				//プレイヤー操作変数群
				this->change_gun = 0;						//
				this->xrad_p = 0.f;						//マウスエイム
				this->fov = deg2rad(settings->useVR_e ? 90 : 45);		//
				this->fov_fps = this->fov;						//
				this->TPS.first = true;
				SetMousePoint(deskx / 2, desky / 2);
				//envi
				mapparts->start_map();
				//
				/*
				for (auto& tp : tgt_pic) {
					//tp.isMOVE=false;
				}
				*/
				bool bee = true;
				while (ProcessMessage() == 0) {
					const auto fps = GetFPS();
					const auto waits = GetNowHiPerformanceCount();
					Debugparts->put_way();
					{
						//プレイヤー操作
						{
							//銃変更
							if (this->usegun.first) {
								if (this->change_gun == 1 || bee == false) {
									++this->sel_g2%=mine.gunptr_have.size();
									if (mine.gunptr_have[this->sel_g2].ptr == nullptr) {
										this->sel_g2 = 0;
									}
									auto pos_t = mine.pos;
									auto mat_t = mine.mat;
									mine.delete_chara();
									mine.set_chara(pos_t, mat_t, this->sel_g2, this->ScopeScreen, body_obj);
									this->gunpos_TPS = VGet(0, 0, 0);
								}
								bee = true;
							}
							else {
								if(bee){
									auto pos_t = mine.pos;
									auto mat_t = mine.mat;
									mine.delete_chara();
									mine.set_chara(pos_t, mat_t, -1, this->ScopeScreen, body_obj);
									this->gunpos_TPS = VGet(0, 0, 0);

									if (this->sel_g2 > 0) {
										--this->sel_g2;
									}
									else {
										this->sel_g2 = int(mine.gunptr_have.size() - 1);
										while (true){
											if (mine.gunptr_have[this->sel_g2].ptr == nullptr) {
												this->sel_g2--;
											}
											else {
												break;
											}
										}

									}

									bee = false;
								}
							}
							if (settings->useVR_e) {
								//HMD
								vrparts->GetDevicePositionVR(vrparts->get_hmd_num(), &mine.pos_HMD, &mine.mat_HMD);
								//移動
								if (vrparts->get_hand2_num() != -1) {
									auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand2_num()];
									if (ptr_.turn && ptr_.now) {
										if ((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) {
											easing_set(&this->add_pos_buf,
												(
													mine.mat_HMD.zvec()*ptr_.touch.y() +
													mine.mat_HMD.xvec()*ptr_.touch.x()
													)*-4.f / fps, 0.95f, fps);
										}
										else {
											easing_set(&this->add_pos_buf, VGet(0, 0, 0), 0.95f, fps);
										}
										if (mine.add_ypos == 0.f) {
											if ((ptr_.on[0] & BUTTON_SIDE) != 0) {
												mine.add_ypos = 0.05f;
											}
											this->add_pos = this->add_pos_buf;
										}
										else {
											easing_set(&this->add_pos, VGet(0, 0, 0), 0.995f, fps);
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
									mine.mat_HMD = MATRIX_ref::RotX(-this->xrad_p)*mine.mat_HMD;
									this->xrad_p = std::clamp(this->xrad_p - deg2rad(y_m - desky / 2)*0.1f*this->fov_fps / this->fov, deg2rad(-45), deg2rad(45));
									mine.mat_HMD *= MATRIX_ref::RotY(deg2rad(x_m - deskx / 2)*0.1f*this->fov_fps / this->fov);
									mine.mat_HMD = MATRIX_ref::RotX(this->xrad_p)*mine.mat_HMD;
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
										easing_set(&this->add_pos_buf, mine.mat_HMD.zvec()*-(runkey ? 8.f : 4.f) / fps, 0.95f, fps);
									}
									if (skey) {
										easing_set(&this->add_pos_buf, mine.mat_HMD.zvec()*(runkey ? 8.f : 4.f) / fps, 0.95f, fps);
									}
									if (akey) {
										easing_set(&this->add_pos_buf, mine.mat_HMD.xvec()*(runkey ? 8.f : 4.f) / fps, 0.95f, fps);
									}
									if (dkey) {
										easing_set(&this->add_pos_buf, mine.mat_HMD.xvec()*-(runkey ? 8.f : 4.f) / fps, 0.95f, fps);
									}
									if (!wkey && !skey && !akey && !dkey) {
										easing_set(&this->add_pos_buf, VGet(0, 0, 0), 0.95f, fps);
									}
									if (mine.add_ypos == 0.f) {
										if (jampkey) {
											mine.add_ypos = 0.05f;
										}
										this->add_pos = this->add_pos_buf;
									}
									else {
										easing_set(&this->add_pos, VGet(0, 0, 0), 0.995f, fps);
									}
								}
							}
							//移動共通
							{
								mine.pos += this->add_pos;
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
									easing_set(&this->campos_TPS, VGet(-0.35f, 0.15f, 1.f), 0.95f, fps);
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
									if (this->ads.first) {
										easing_set(&this->gunpos_TPS, mine.pos_HMD + VGet(-0.035f, 0.f - pv.y(), -0.3f), 0.75f, fps);
										easing_set(&this->fov_fps, (this->fov*0.6f) / ((mine.gunptr->frame[4].first != INT_MAX) ? 4.f : 1.f), 0.9f, fps);
										easing_set(&this->campos_TPS, VGet(-0.35f, 0.15f, 0.5f), 0.9f, fps);
									}
									else {
										easing_set(&this->gunpos_TPS, mine.pos_HMD + VGet(-0.15f, -0.05f - pv.y(), -0.5f), 0.75f, fps);
										easing_set(&this->fov_fps, this->fov, 0.9f, fps);
										easing_set(&this->campos_TPS, VGet(-0.35f, 0.15f, 1.f), 0.95f, fps);
									}
									mine.pos_LHAND = mine.pos_HMD + MATRIX_ref::Vtrans(this->gunpos_TPS - mine.pos_HMD, mine.mat_HMD);
									mine.mat_LHAND = mine.mat_HMD;
								}
								//RHAND
								{
									mine.pos_RHAND = mine.obj.frame(mine.gunptr->frame[6].first) - mine.pos;
									mine.mat_RHAND = mine.mat_HMD;
								}

								if (mine.down_mag) {
									mine.pos_RHAND = mine.obj.frame(mine.gunptr->frame[0].first) + mine.mat_LHAND.yvec()*-0.05f - mine.pos;
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
										easing_set(&mine.obj.get_anime(2).per, float((ptr_.on[0] & BUTTON_TRIGGER) != 0), 0.5f, fps);
										//マグキャッチ
										easing_set(&mine.obj.get_anime(5).per, float((ptr_.on[0] & BUTTON_SIDE) != 0), 0.5f, fps);
										//セレクター
										mine.selkey = std::clamp<uint8_t>(mine.selkey + 1, 0, (((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_.touch.x() > 0.5f && ptr_.touch.y() < 0.5f&&ptr_.touch.y() > -0.5f)) ? 2 : 0);
										//武装変更
										if (this->sel_g2 != -1) {
											this->usegun.get_in(CheckHitKey(KEY_INPUT_P) != 0);//<---
										}
										else {
											this->usegun.first = false;
											this->usegun.second = 0;
										}
										if (this->usegun.first) {
											this->change_gun = std::clamp<uint8_t>(this->change_gun + 1, 0, ((ptr_.on[0] & BUTTON_TOPBUTTON) != 0) ? 2 : 0);
										}
									}
								}
								if (vrparts->get_hand2_num() != -1) {
									auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand2_num()];
									if (ptr_.turn && ptr_.now) {
										//マガジン取得
										if (mine.gun_have_state[mine.gunptr->id].mag_in.size() >= 1) {
											mine.down_mag |= ((ptr_.on[0] & BUTTON_TRIGGER) != 0);
										}
										//銃変更
										this->chgun.get_in((ptr_.on[0] & BUTTON_TOPBUTTON) != 0);

										if (this->usegun.first) {
											this->delgun.get_in(CheckHitKey(KEY_INPUT_G) != 0);
										}
										else {
											this->delgun.second = 0;
										}

										//タイマーオン
										scoreparts->c_ready |= ((ptr_.on[0] & BUTTON_SIDE) != 0);
										//計測リセット
										if ((ptr_.on[0] & BUTTON_TOUCHPAD) != 0 && scoreparts->c_end) {
											scoreparts->reset();
										}
									}
								}
							}
							else {
								//タイマーオン(Bキー)
								scoreparts->c_ready |= (CheckHitKey(KEY_INPUT_B) != 0);
								//計測リセット(Vキー)
								if (CheckHitKey(KEY_INPUT_V) != 0 && scoreparts->c_end) {
									scoreparts->reset();
								}
								//マガジン取得
								mine.down_mag = true;
								//引き金(左クリック)
								easing_set(&mine.obj.get_anime(2).per, float((GetMouseInput() & MOUSE_INPUT_LEFT) != 0), 0.5f, fps);
								//銃変更
								this->chgun.get_in(CheckHitKey(KEY_INPUT_F) != 0);

								if (this->sel_g2 != -1) {
									this->usegun.get_in(CheckHitKey(KEY_INPUT_P) != 0);//<---
								}
								else {
									this->usegun.first = false;
									this->usegun.second = 0;
								}

								if (this->usegun.first) {
									if (mine.gunptr->cate == 1) {
										//ADS
										this->ads.first = (GetMouseInput() & MOUSE_INPUT_RIGHT) != 0;
									}
									else {
										//ADS
										this->ads.first = false;
										/*なし*/
									}
									//マグキャッチ(Rキー)
									easing_set(&mine.obj.get_anime(5).per, float(CheckHitKey(KEY_INPUT_R) != 0), 0.5f, fps);
									//セレクター(中ボタン)
									mine.selkey = std::clamp<uint8_t>(mine.selkey + 1, 0, ((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0) ? 2 : 0);
									//銃変更
									this->delgun.get_in(CheckHitKey(KEY_INPUT_G) != 0);
									//武装変更
									this->change_gun = std::clamp<uint8_t>(this->change_gun + 1, 0, (GetMouseWheelRotVol() != 0) ? 2 : 0);
								}
								else {
									//ADS
									this->ads.first = false;
									//マグキャッチ(Rキー)
									mine.obj.get_anime(5).per=0.f;
									//セレクター(中ボタン)
									mine.selkey = 0;
									//銃変更
									this->delgun.second = 0;
								}
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
								c.body.SetMatrix(c.mat_HMD*MATRIX_ref::Mtrans(c.pos_HMD + c.pos));
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
									if (c.obj.get_anime(5).per >= 0.5f && !c.reloadf && c.gun_have_state[c.gunptr->id].mag_in.size()>=1) {
										c.audio.mag_down.play_3D(c.pos + c.pos_LHAND, 1.f);
										int dnm = int(c.ammoc) - 1;
										//弾数
										if (c.ammoc >= 1) {
											c.ammoc = 1;
										}
										else {
											dnm = 0;
										}
										c.gun_have_state[c.gunptr_have[this->sel_g2].ptr->id].in -= dnm;
										//バイブレーション　バッテリー消費が激しいためコメントアウト
										/*
										if (&c == &mine) {
											vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
										}
										*/
										//マガジン排出
										c.reload_cnt = 0.f;
										c.gun_have_state[c.gunptr->id].mag_in.erase(c.gun_have_state[c.gunptr->id].mag_in.begin());
										//マガジン排出
										bool tt = false;
										for (auto& g : this->magitem) {
											if (g.gunptr == nullptr) {
												tt = true;
												g.set(c.gunptr, c.pos_mag);
												g.add = (c.obj.frame(c.gunptr->frame[1].first) - c.obj.frame(c.gunptr->frame[0].first)).Norm()*-1.f / fps;//排莢ベクトル
												g.mat = c.mat_mag;
												g.cap = dnm;
												break;
											}
										}
										if (!tt) {
											this->magitem.resize(this->magitem.size() + 1);
											auto& g = this->magitem.back();
											g.set(c.gunptr, c.pos_mag);
											g.add = (c.obj.frame(c.gunptr->frame[1].first) - c.obj.frame(c.gunptr->frame[0].first)).Norm()*-1.f / fps;//排莢ベクトル
											g.mat = c.mat_mag;
											g.cap = dnm;
										}
										//
										c.reloadf = true;
									}
									//セレクター
									if (c.selkey == 1) {
										++c.select %= c.gunptr->select.size();
									}
								}
								//
								if (c.reloadf && c.gun_have_state[c.gunptr->id].mag_in.size() >= 1) {
									c.reload_cnt += 1.f / fps;
									if (c.reload_cnt<c.gunptr->reload_time) {
										c.down_mag = false;
									}
								}
								//セフティ
								easing_set(&c.obj.get_anime(4).per, float(0.f), 0.5f, fps);
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
											c.gun_have_state[c.gunptr_have[this->sel_g2].ptr->id].in--;
											if (!c.reloadf && c.gun_have_state[c.gunptr_have[this->sel_g2].ptr->id].mag_in.size()>=1) {
												if (c.gun_have_state[c.gunptr_have[this->sel_g2].ptr->id].mag_in.front() > 0) {
													c.gun_have_state[c.gunptr_have[this->sel_g2].ptr->id].mag_in.front()--;
												}
											}
											c.gunf = true;
											if (scoreparts->c_start && !scoreparts->c_end) {
												scoreparts->sub(-4);
											}
											//持ち手を持つとココが相殺される
											c.vecadd_LHAND_p = MATRIX_ref::Vtrans(c.vecadd_LHAND_p,
												MATRIX_ref::RotY(deg2rad(float(int(c.gunptr->recoil_xdn*100.f) + GetRand(int((c.gunptr->recoil_xup - c.gunptr->recoil_xdn)*100.f))) / (100.f*(c.LEFT_hand ? 3.f : 1.f))))*
												MATRIX_ref::RotX(deg2rad(float(int(c.gunptr->recoil_ydn*100.f) + GetRand(int((c.gunptr->recoil_yup - c.gunptr->recoil_ydn)*100.f))) / (100.f*(c.LEFT_hand ? 3.f : 1.f)))));
											c.audio.shot.play_3D(c.pos + c.pos_LHAND, 1.f);
											c.audio.slide.play_3D(c.pos + c.pos_LHAND, 1.f);
											auto& u = c.bullet[c.usebullet];
											auto& a = c.ammo[c.usebullet];
											++c.usebullet %= c.bullet.size();
											//ココだけ変化
											u.spec = &c.gunptr->ammo[0];
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
											a.second.Dispose();
											a.second = u.spec->ammo.Duplicate();
											a.cnt = 0.f;
											a.pos = c.obj.frame(c.gunptr->frame[2].first);//排莢
											a.add = (c.obj.frame(c.gunptr->frame[2].first + 1) - a.pos).Norm()*2.5f / fps;//排莢ベクトル
											a.mat = c.mat_LHAND;
										}
										//マガジン取得
										if (c.reloadf && c.gun_have_state[c.gunptr->id].mag_in.size() >= 1) {
											if (c.down_mag) {
												auto p = MATRIX_ref::RotVec2(c.mat_RHAND.yvec(), (c.obj.frame(c.gunptr->frame[0].first) - (c.pos_RHAND + c.pos)));
												VECTOR_ref xvec = MATRIX_ref::Vtrans(c.mat_RHAND.xvec(), p);
												VECTOR_ref yvec = MATRIX_ref::Vtrans(c.mat_RHAND.yvec(), p);
												VECTOR_ref zvec = MATRIX_ref::Vtrans(c.mat_RHAND.zvec(), p);

												c.mat_mag = c.mag.GetFrameLocalMatrix(3)* MATRIX_ref::Axis1(xvec, yvec, zvec);
												c.pos_mag = c.pos_RHAND + c.pos;
												if ((c.mag.frame(3) - c.obj.frame(c.gunptr->frame[0].first)).size() <= 0.1f) {
													c.obj.get_anime(1).time = 0.f;
													c.obj.get_anime(0).per = 1.f;
													c.obj.get_anime(1).per = 0.f;
													if (c.ammoc == 0) {
														c.obj.get_anime(3).per = 1.f;
													}
													c.audio.mag_set.play_3D(c.pos + c.pos_LHAND, 1.f);

													c.ammoc += c.gun_have_state[c.gunptr_have[this->sel_g2].ptr->id].mag_in.front();


													c.reloadf = false;
												}
											}
										}
										else {
											c.down_mag = false;
											c.mat_mag = c.mat_LHAND;
											c.pos_mag = c.obj.frame(c.gunptr->frame[1].first);
										}
										break;
									}
								}
								{
									//右手
									{
										MATRIX_ref m3 = MATRIX_ref::RotZ(deg2rad(90))*c.mat_LHAND*c.mat_HMD.Inverse();
										c.body.SetFrameLocalMatrix(1, m3*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.pos_LHAND - c.pos_HMD, c.mat_HMD.Inverse())));
									}
									//左手
									{
										float dist_ = ((c.pos_RHAND + c.pos) - c.obj.frame(c.gunptr->frame[6].first)).size();
										if (dist_ <= 0.2f && (!c.reloadf || !c.down_mag)) {
											c.LEFT_hand = true;
											MATRIX_ref m4 = MATRIX_ref::RotZ(deg2rad(-90));
											c.body.SetFrameLocalMatrix(17, m4*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.obj.frame(c.gunptr->frame[6].first) - (c.pos_HMD + c.pos), c.mat_HMD.Inverse())));
										}
										else {
											c.LEFT_hand = false;
											MATRIX_ref m4 = MATRIX_ref::RotZ(deg2rad(-90))*c.mat_RHAND*c.mat_HMD.Inverse();
											c.body.SetFrameLocalMatrix(17, m4*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.pos_RHAND - c.pos_HMD, c.mat_HMD.Inverse())));
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
										c.body.SetFrameLocalMatrix(34,
											MATRIX_ref::RotY(DX_PI_F + c.body_yrad)*c.mat_HMD.Inverse()*
											MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(VGet(0.f, -0.15f, 0.f), c.mat_HMD.Inverse()))
										);
										/*
										for (size_t i = 0; i < c.gunptr_have.size(); i++) {
											if (int(i) != this->sel_g2 && c.gunptr_have[i]!=nullptr) {
												c.gunptr_have[i]->mod.obj.SetMatrix(
													MATRIX_ref::RotY(DX_PI_F + c.body_yrad)*
													MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(VGet(0.f, -0.15f, 0.f), c.mat_HMD.Inverse()))*
													MATRIX_ref::Mtrans(c.pos_HMD + c.pos)
												);
												c.gunptr_have[i]->mod.obj.DrawModel();
											}
										}
										*/
									}
								}
								c.mag.SetMatrix(c.mat_mag* MATRIX_ref::Mtrans(c.pos_mag));
								c.obj.SetMatrix(c.mat_LHAND*MATRIX_ref::Mtrans(c.pos_LHAND + c.pos));
								c.body.SetMatrix(c.mat_HMD*MATRIX_ref::Mtrans(c.pos_HMD + c.pos));
							}
							c.obj.work_anime();
							for (auto& a : c.bullet) {
								if (a.flug) {
									a.repos = a.pos;
									a.pos += a.vec * (a.spec->speed / fps);
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
													scoreparts->add(int(float(pt) * ((a.pos - (c.pos + c.pos_LHAND)).size() / 9.144f)*(a.spec->damage / 10.f)));
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
									if (a.cnt >= 3.f || a.spec->speed < 0.f || a.spec->pene <= 0.f) {
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

									auto pp = mapparts->map_col_line(a.pos + VGet(0, 1.f, 0), a.pos - VGet(0, 0.008f, 0), 0);
									if (pp.HitFlag == 1) {
										a.pos = VECTOR_ref(pp.HitPosition) + VGet(0, 0.008f, 0);
										a.add += VECTOR_ref(pp.Normal)*(VECTOR_ref(pp.Normal).dot(a.add*-1.f)*1.25f);
										easing_set(&a.add, VGet(0, 0, 0), 0.95f, fps);
										if (!a.down) {
											c.audio.case_down.play_3D(c.pos + c.pos_LHAND, 1.f);
										}

										a.mat *= MATRIX_ref::RotVec2(a.mat.yvec(), VECTOR_ref(pp.Normal));

										a.down = true;
									}
									else {
										a.mat *= MATRIX_ref::RotY(deg2rad(360 * 10 + GetRand(360 * 20)) / fps);
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
								g.obj.SetMatrix(g.mat*MATRIX_ref::Mtrans(g.pos));
								g.pos += g.add;
								g.add.yadd(-9.8f / powf(fps, 2.f));

								for (auto& p : this->gunitem) {
									if (p.gunptr != nullptr && &p != &g) {
										if ((p.pos - g.pos).size() <= 0.1f) {
											p.add.xadd((p.pos - g.pos).x()*10.f / fps);
											p.add.zadd((p.pos - g.pos).z()*10.f / fps);
											g.add.xadd((g.pos - p.pos).x()*10.f / fps);
											g.add.zadd((g.pos - p.pos).z()*10.f / fps);
										}
									}
								}
								auto pp = mapparts->map_col_line(g.pos + VGet(0, 1.f, 0), g.pos - VGet(0, 0.05f, 0), 0);
								if (pp.HitFlag == 1) {
									g.pos = VECTOR_ref(pp.HitPosition) + VGet(0, 0.05f, 0);
									g.mat *= MATRIX_ref::RotVec2(g.mat.xvec(), VECTOR_ref(pp.Normal));
									easing_set(&g.add, VGet(0, 0, 0), 0.8f, fps);
								}
								//
								{
									bool cng = false;

									VECTOR_ref startpos = mine.obj.frame(mine.gunptr->frame[3].first);
									VECTOR_ref endpos = mine.obj.frame(mine.gunptr->frame[3].first) + mine.mat_RHAND.zvec()*-3.f;
									auto p = mapparts->map_col_line(startpos, endpos, 0);
									if (p.HitFlag == 1) {
										endpos = p.HitPosition;
									}
									cng = (Segment_Point_MinLength(startpos.get(), endpos.get(), g.pos.get()) <= 0.3f);

									mine.canget |= cng;
									if (cng) {
										mine.canget_gun = g.gunptr->name;
										if (this->chgun.second == 1) {
											for (size_t i = 0; i < mine.gunptr_have.size(); i++) {
												if (mine.gunptr_have[i].ptr == nullptr) {
													this->sel_g2 = int(i);
													if (!this->usegun.first) {
														this->usegun.first = true;
														this->change_gun = 1;
													}
													//
													mine.gunptr_have[this->sel_g2].ptr = g.gunptr;
													mine.gunptr_have[this->sel_g2].obj = mine.gunptr_have[this->sel_g2].ptr->mod.obj.Duplicate();
													g.delete_chara();
													//

													//
													auto pos_t = mine.pos;
													auto mat_t = mine.mat;
													mine.delete_chara();
													mine.set_chara(pos_t, mat_t, this->sel_g2, this->ScopeScreen, body_obj);
													this->gunpos_TPS = VGet(0, 0, 0);
													//
													break;
												}
												if (i == mine.gunptr_have.size() - 1) {
													int ii = this->sel_g2;
													if (!this->usegun.first) {
														++ii %= mine.gunptr_have.size();
														if (mine.gunptr_have[ii].ptr == nullptr) {
															ii = 0;
														}
														this->usegun.first = true;
														this->change_gun = 1;
													}
													auto pt = mine.gunptr_have[ii].ptr;
													mine.gunptr_have[ii].obj.Dispose();
													//
													mine.gunptr_have[ii].ptr = g.gunptr;
													mine.gunptr_have[ii].obj = mine.gunptr_have[ii].ptr->mod.obj.Duplicate();
													g.delete_chara();
													//
													g.set(pt, mine.pos + mine.pos_LHAND);
													g.mat = mine.mat_LHAND;
													//
													auto pos_t = mine.pos;
													auto mat_t = mine.mat;
													mine.delete_chara();
													mine.set_chara(pos_t, mat_t, ii, this->ScopeScreen, body_obj);
													this->gunpos_TPS = VGet(0, 0, 0);
													//
												}
											}
										}
									}
								}
							}
						}
						for (auto& c : chara) {
							if (&c == &mine) {
								if (this->delgun.second == 1) {
									//
									bool tt = false;
									for (auto& g : this->gunitem) {
										if (g.gunptr == nullptr) {
											tt = true;
											g.set(c.gunptr, c.pos + c.pos_LHAND);
											g.mat = c.mat_LHAND;
											break;
										}
									}
									if (!tt) {
										this->gunitem.resize(this->gunitem.size() + 1);
										auto& g = this->gunitem.back();
										g.set(c.gunptr, c.pos + c.pos_LHAND);
										g.mat = c.mat_LHAND;
									}
									//
									for (size_t i = 0; i < c.gunptr_have.size(); i++) {
										if (this->sel_g2 == i) {
											if (
												(this->sel_g2 == c.gunptr_have.size() - 1) ||
												c.gunptr_have[std::clamp<size_t>(this->sel_g2 + 1, 0, c.gunptr_have.size() - 1)].ptr == nullptr
											) {
												c.gunptr_have[this->sel_g2].ptr = nullptr;
												c.gunptr_have[this->sel_g2].obj.Dispose();
												this->sel_g2--;
												break;
											}
											else {
												for (size_t j = i; j < c.gunptr_have.size() - 1; j++) {
													c.gunptr_have[j].ptr = c.gunptr_have[j + 1].ptr;
													c.gunptr_have[j].obj.Dispose();
													if (c.gunptr_have[j].ptr != nullptr) {
														c.gunptr_have[j].obj = c.gunptr_have[j].ptr->mod.obj.Duplicate();
													}
												}
												c.gunptr_have[c.gunptr_have.size() - 1].ptr = nullptr;
												c.gunptr_have[c.gunptr_have.size() - 1].obj.Dispose();
												break;
											}
										}
									}
									//
									auto pos_t = c.pos;
									auto mat_t = c.mat;
									c.delete_chara();
									c.set_chara(pos_t, mat_t, this->sel_g2, this->ScopeScreen, body_obj);
									this->gunpos_TPS = VGet(0, 0, 0);
									//
									if (this->sel_g2 == -1) {

									}
								}
							}
						}
						//magitem
						for (auto& c : chara) {
							c.cangetm = false;
						}
						for (auto& g : this->magitem) {
							//
							if (g.gunptr != nullptr) {
								g.obj.SetMatrix(g.mat*MATRIX_ref::Mtrans(g.pos));
								g.pos += g.add;
								g.add.yadd(-9.8f / powf(fps, 2.f));

								for (auto& p : this->magitem) {
									if (p.gunptr != nullptr && &p != &g) {
										if ((p.pos - g.pos).size() <= 0.1f) {
											p.add.xadd((p.pos - g.pos).x()*10.f / fps);
											p.add.zadd((p.pos - g.pos).z()*10.f / fps);
											g.add.xadd((g.pos - p.pos).x()*10.f / fps);
											g.add.zadd((g.pos - p.pos).z()*10.f / fps);
										}
									}
								}

								auto pp = mapparts->map_col_line(g.pos + VGet(0, 1.f, 0), g.pos - VGet(0, 0.05f, 0), 0);
								if (pp.HitFlag == 1) {
									g.pos = VECTOR_ref(pp.HitPosition) + VGet(0, 0.05f, 0);
									g.mat *= MATRIX_ref::RotVec2(g.mat.xvec(), VECTOR_ref(pp.Normal));
									easing_set(&g.add, VGet(0, 0, 0), 0.5f, fps);
								}
								//
								for (auto& c : chara) {
									bool cng = false;
									if (&c == &mine) {

										VECTOR_ref startpos = mine.obj.frame(mine.gunptr->frame[3].first);
										VECTOR_ref endpos = mine.obj.frame(mine.gunptr->frame[3].first) + mine.mat_RHAND.zvec()*-3.f;
										auto p = mapparts->map_col_line(startpos, endpos, 0);
										if (p.HitFlag == 1) {
											endpos = p.HitPosition;
										}
										cng = (Segment_Point_MinLength(startpos.get(), endpos.get(), g.pos.get()) <= 0.3f);


										c.cangetm |= cng;
										if (cng) {
											c.canget_mag = g.gunptr->mag.name;
											if (this->chgun.second == 1) {
												c.gun_have_state[g.gunptr->id].in += g.cap;
												c.gun_have_state[g.gunptr->id].mag_in.insert(c.gun_have_state[g.gunptr->id].mag_in.end(), g.cap);
												if (c.gun_have_state[g.gunptr->id].mag_in.size() == 1) {
													c.reloadf = true;
												}
												g.delete_chara();
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
						this->campos_buf = mine.pos + mine.pos_HMD;
						this->camvec = mine.mat_HMD.zvec()*-1.f;
						this->camup = mine.mat_HMD.yvec();
						Set3DSoundListenerPosAndFrontPosAndUpVec(this->campos_buf.get(), (this->campos_buf + this->camvec).get(), this->camup.get());
						UpdateEffekseer3D();
						//影用意
						Drawparts->Ready_Shadow(this->campos_buf, draw_in_shadow, VGet(5.f, 2.5f, 5.f));
						//VR空間に適用
						vrparts->Move_Player();
						//スコープ
						if (mine.gunptr->frame[4].first != INT_MAX) {
							VECTOR_ref cam = mine.obj.frame(mine.gunptr->frame[4].first);
							VECTOR_ref vec = cam - mine.mat_LHAND.zvec();
							VECTOR_ref yvec = mine.mat_LHAND.yvec();
							Hostpassparts->draw(&this->ScopeScreen, mapparts->sky_draw(cam, vec, yvec, (this->fov / 7.5f) / 4.f), draw_by_shadow, cam, vec, yvec, (this->fov / 7.5f) / 4.f, 100.f, 0.1f);
							mine.gunptr->mod.lenzScreen.DrawExtendGraph(0, 0, 1080, 1080, true);
						}
						//描画
						UIparts->set_draw(mine, scoreparts, settings->useVR_e, this->usegun.first, this->sel_g2);
						if (settings->useVR_e) {
							for (char i = 0; i < 2; i++) {
								this->campos = this->campos_buf + vrparts->GetEyePosition_minVR(i);
								//被写体深度描画
								Hostpassparts->dof(&this->BufScreen, mapparts->sky_draw(this->campos, this->campos + this->camvec, this->camup, this->fov), draw_by_shadow, this->campos, this->campos + this->camvec, this->camup, this->fov, 100.f, 0.1f);
								//描画
								this->outScreen[i].SetDraw_Screen(0.1f, 100.f, this->fov_fps, this->campos, this->campos + this->camvec, this->camup);
								{
									Hostpassparts->bloom(this->BufScreen, 64);//ブルーム付き描画
									UIparts->draw();//UI
									UIparts->Gunitem_draw(this->gunitem, this->campos_buf);
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
							this->campos = this->campos_buf + MATRIX_ref::Vtrans(VGet(-0.035f, 0.f, 0.f), mine.mat_HMD);
							//被写体深度描画
							Hostpassparts->dof(&this->BufScreen, mapparts->sky_draw(this->campos, this->campos + this->camvec, this->camup, this->fov_fps), draw_by_shadow, this->campos, this->campos + this->camvec, this->camup, this->fov_fps, 100.f, 0.1f);
							//描画
							this->outScreen[1].SetDraw_Screen(0.1f, 100.f, this->fov_fps, this->campos, this->campos + this->camvec, this->camup);
							{
								Hostpassparts->bloom(this->BufScreen, 64);//ブルーム付き描画
								UIparts->draw();//UI
								UIparts->Gunitem_draw(this->gunitem, this->campos_buf);
								UIparts->Magitem_draw(this->magitem, this->campos_buf);
							}
						}
						//ディスプレイ描画
						{
							this->TPS.get_in(CheckHitKey(KEY_INPUT_LCONTROL) != 0);

							VECTOR_ref cam = mine.pos + mine.pos_HMD + MATRIX_ref::Vtrans(this->campos_TPS, mine.mat_HMD);
							VECTOR_ref vec = mine.pos + mine.pos_HMD + MATRIX_ref::Vtrans(VGet(-0.35f, 0.15f, 0.f), mine.mat_HMD);
							if (this->TPS.first) {//TPS視点
								Hostpassparts->draw(&this->outScreen[2], mapparts->sky_draw(cam, vec, VGet(0, 1.f, 0), this->fov), draw_by_shadow, cam, vec, VGet(0, 1.f, 0), this->fov, 100.f, 0.1f);
								GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK, 0.1f, 100.f, this->fov, cam, vec, VGet(0, 1.f, 0));
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