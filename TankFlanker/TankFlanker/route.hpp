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
	std::vector<Items> gunitem;	//拾える銃
	std::vector<Items> magitem;	//拾える銃
	//
	bool ending = true;
	int sel_g2 = 0;
	//プレイヤー操作変数群
	switchs TPS, ads, chgun, delgun, usegun;					//
	uint8_t change_gun = 0;										//
	VECTOR_ref gunpos_TPS;										//
	float xrad_p = 0.f;											//マウスエイム
	VECTOR_ref add_pos, add_pos_buf;							//移動
	VECTOR_ref campos, campos_buf, camvec, camup, campos_TPS;	//カメラ
	float fov = 0.f, fov_fps = 0.f;
	bool cansh_gun = true;
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
		MV1::Load("data/model/hand/model_b.mv1", &this->body_obj, true);				//身体
		auto mapparts = std::make_unique<Mapclass>(this->dispx, this->dispy);			//map
		auto tgtparts = std::make_unique<tgttmp>();						//ターゲット
		//GUNデータ
		gun_data.resize(5);
		gun_data[0].mod.set("Knife");
		gun_data[1].mod.set("1911");
		gun_data[2].mod.set("M82A2");
		gun_data[3].mod.set("CAR15_M4");
		gun_data[4].mod.set("AK74");
		UIparts->load_window("銃モデル");				//ロード画面
		//GUNデータ取得
		fill_id(gun_data);
		for (auto& g : gun_data) { g.set_data(); }
		UIparts->load_window("銃モデル");				//ロード画面
		tgtparts->set();								//ターゲット
		auto scoreparts = std::make_unique<scores>();	//スコア
		vrparts->Set_Device();							//VRセット
		do {
			//キャラ設定
			{
				int sel_g = UIparts->select_window(settings->useVR_e, gun_data, vrparts, settings);
				if (sel_g < 0) { break; }
				chara.resize(1);
				chara[0].ready_chara(&gun_data[sel_g], &gun_data[0], gun_data.size(), this->body_obj, &this->ScopeScreen);
				chara[0].set_chara(VGet(0, 0, -0.5f), MGetIdent(), 0);
				chara[0].mat_HMD = MATRIX_ref::RotY(deg2rad(180));
				this->sel_g2 = 0;
				this->usegun.first = true;
				this->usegun.second = 0;
			}
			//マップ読み込み
			mapparts->set_map_pre();
			UIparts->load_window("マップモデル");
			mapparts->set_map("data/maps/set.txt",this->gunitem,this->magitem, this->gun_data);
			//ターゲット
			tgt_pic.resize(5);
			tgt_pic[0].set(tgtparts, VGet(4, 0, 12.f));
			tgt_pic[1].set(tgtparts, VGet(-4, 0, 18.f));
			tgt_pic[2].set(tgtparts, VGet(2, 0, 27.f));
			tgt_pic[3].set(tgtparts, VGet(-2, 0, 36.f));
			tgt_pic[4].set(tgtparts, VGet(0, 0, 45.f));
			fill_id(tgt_pic);
			for (auto& p : tgt_pic) { p.obj.SetPosition(mapparts->map_col_line(p.obj.GetPosition() - VGet(0, -10.f, 0), p.obj.GetPosition() - VGet(0, 10.f, 0), 0).HitPosition); }
			//ライティング
			Drawparts->Set_Light_Shadow(settings->shadow_level_e, VGet(50.f, 20.f, 50.f), VGet(0.05f, -0.5f, 0.75f), [&mapparts] {mapparts->map_get().DrawModel(); });
			//影に描画するものを指定する(仮)
			auto draw_in_shadow = [&] {
				for (auto& p : this->tgt_pic) { p.obj.DrawModel(); }
				for (auto& c : this->chara) { c.draw_chara(this->usegun.first, this->sel_g2); }
				for (auto& g : this->gunitem) { g.draw(); }
				for (auto& g : this->magitem) { g.draw(); }
			};
			auto draw_by_shadow = [&] {
				Drawparts->Draw_by_Shadow([&] {
					mapparts->map_get().DrawModel();
					for (auto& p : this->tgt_pic) { p.obj.DrawModel(); }
					for (auto& c : this->chara) { c.draw_chara(this->usegun.first, this->sel_g2); }
					for (auto& g : this->gunitem) { g.draw(this->chara[0].canget_gunitem, this->chara[0].canget_gun); }
					for (auto& g : this->magitem) { g.draw(this->chara[0].canget_magitem, this->chara[0].canget_mag); }
					//銃弾
					SetFogEnable(FALSE);
					SetUseLighting(FALSE);
					for (auto& c : this->chara) { c.draw_ammo(); }
					SetUseLighting(TRUE);
					SetFogEnable(TRUE);
					for (auto& c : this->chara) {
						VECTOR_ref vec_a1 = MATRIX_ref::Vtrans(((c.pos + c.pos_LHAND) - c.body.frame(c.RIGHTarm1_f.first)).Norm()*0.3f, MATRIX_ref::RotY(DX_PI_F + c.body_yrad).Inverse());//基準
						VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm()*0.3f;//x=0とする
						VECTOR_ref vec_a1L2 = VECTOR_ref(VGet(-1.f, 0.f, vec_a1.x() / vec_a1.z())).Norm()*0.3f;//y=0とする
						float rad_t = cos(deg2rad(30));
						VECTOR_ref vec_t = vec_a1 * rad_t + vec_a1L1 * std::sqrtf(1.f - rad_t * rad_t);
						DrawLine3D(c.body.frame(c.RIGHTarm1_f.first).get(), (c.body.frame(c.RIGHTarm1_f.first) + vec_a1).get(), GetColor(255, 0, 0));
						DrawLine3D(c.body.frame(c.RIGHTarm1_f.first).get(), (c.body.frame(c.RIGHTarm1_f.first) + vec_t).get(), GetColor(255, 0, 0));
					}
				});
			};
			//開始
			{
				auto& mine = chara[0];
				scoreparts->reset();
				//プレイヤー操作変数群
				this->change_gun = 0;								//
				this->xrad_p = 0.f;									//マウスエイム
				this->fov = deg2rad(settings->useVR_e ? 90 : 45);	//
				this->fov_fps = this->fov;							//
				this->TPS.first = true;
				SetMousePoint(deskx / 2, desky / 2);
				//環境
				mapparts->start_map();
				//
				for (auto& tp : tgt_pic) {
					tp.isMOVE = false;
				}
				this->cansh_gun = true;
				while (ProcessMessage() == 0) {
					const auto fps = GetFPS();
					const auto waits = GetNowHiPerformanceCount();
					Debugparts->put_way();
					{
						//プレイヤー操作
						{
							//銃変更
							if (this->usegun.first) {
								if (this->change_gun == 1 || this->cansh_gun == false) {
									++this->sel_g2 %= mine.gun_slot.size();
									if (mine.gun_slot[this->sel_g2].ptr == nullptr) {
										this->sel_g2 = 0;
									}
									auto pos_t = mine.pos;
									auto mat_t = mine.mat;
									mine.delete_chara();
									mine.set_chara(pos_t, mat_t, this->sel_g2);
									this->gunpos_TPS = VGet(0, 0, 0);
								}
								this->cansh_gun = true;
							}
							else if (this->cansh_gun) {
								auto pos_t = mine.pos;
								auto mat_t = mine.mat;
								mine.delete_chara();
								mine.set_chara(pos_t, mat_t, -1);
								this->gunpos_TPS = VGet(0, 0, 0);

								if (this->sel_g2 > 0) {
									--this->sel_g2;
								}
								else {
									this->sel_g2 = int(mine.gun_slot.size() - 1);
									while (true) {
										if (mine.gun_slot[this->sel_g2].ptr == nullptr) {
											this->sel_g2--;
										}
										else {
											break;
										}
									}
								}
								this->cansh_gun = false;
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
								auto pp = mapparts->map_col_line(mine.pos + VGet(0, 1.8f, 0), mine.pos + VGet(0, -0.1f, 0), 0);
								if (mine.add_ypos <= 0.f && pp.HitFlag == 1) {
									mine.pos = pp.HitPosition;
									mine.add_ypos = 0.f;
								}
								else {
									mine.pos.yadd(mine.add_ypos);
									mine.add_ypos -= 9.8f / std::powf(fps, 2.f);
									//復帰
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
									if (mine.ptr_now->frame[4].first != INT_MAX) {
										pv = mine.ptr_now->frame[4].second;
									}
									else if (mine.ptr_now->frame[7].first != INT_MAX) {
										pv = mine.ptr_now->frame[7].second;
									}
									if (this->ads.first) {
										easing_set(&this->gunpos_TPS, mine.pos_HMD + VGet(-0.035f, 0.f - pv.y(), -0.3f), 0.75f, fps);
										easing_set(&this->fov_fps, (this->fov*0.6f) / ((mine.ptr_now->frame[4].first != INT_MAX) ? 4.f : 1.f), 0.9f, fps);
										easing_set(&this->campos_TPS, VGet(-0.35f, 0.15f, 0.5f), 0.9f, fps);
									}
									else {
										easing_set(&this->gunpos_TPS, mine.pos_HMD + VGet(-0.15f, -0.05f - pv.y(), -0.5f), 0.75f, fps);
										easing_set(&this->fov_fps, this->fov, 0.9f, fps);
										easing_set(&this->campos_TPS, VGet(-0.35f, 0.15f, 2.f), 0.95f, fps);
									}
									mine.pos_LHAND = mine.pos_HMD + MATRIX_ref::Vtrans(this->gunpos_TPS - mine.pos_HMD, mine.mat_HMD);
									mine.mat_LHAND = mine.mat_HMD;
								}
								//RHAND
								{
									mine.pos_RHAND = mine.obj.frame(mine.ptr_now->frame[6].first) - mine.pos;
									if (mine.down_mag) {
										mine.pos_RHAND = mine.obj.frame(mine.ptr_now->frame[0].first) + mine.mat_LHAND.yvec()*-0.05f - mine.pos;
									}
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
										easing_set(&mine.obj.get_anime(2).per, float((ptr_.on[0] & BUTTON_TRIGGER) != 0), 0.5f, fps);
										//マグキャッチ
										easing_set(&mine.obj.get_anime(5).per, float((ptr_.on[0] & BUTTON_SIDE) != 0), 0.5f, fps);
										//セレクター
										mine.selkey.get_in(((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_.touch.x() > 0.5f && ptr_.touch.y() < 0.5f&&ptr_.touch.y() > -0.5f));
										//銃の使用
										this->usegun.get_in((CheckHitKey(KEY_INPUT_P) != 0) && (this->sel_g2 != -1));//<---
										if (this->sel_g2 == -1) {
											this->usegun.first = false;
										}
										//武装変更
										this->change_gun = std::clamp<uint8_t>(this->change_gun + 1, 0, (((ptr_.on[0] & BUTTON_TOPBUTTON) != 0)&& this->usegun.first) ? 2 : 0);
									}
								}
								if (vrparts->get_hand2_num() != -1) {
									auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand2_num()];
									if (ptr_.turn && ptr_.now) {
										//マガジン取得
										mine.down_mag |= (((ptr_.on[0] & BUTTON_TRIGGER) != 0) && (mine.gun_stat[mine.ptr_now->id].mag_in.size() >= 1));
										//銃変更
										this->chgun.get_in((ptr_.on[0] & BUTTON_TOPBUTTON) != 0);
										//銃破棄
										this->delgun.get_in((CheckHitKey(KEY_INPUT_G) != 0) && this->usegun.first);
										//タイマーオン
										scoreparts->ready_f |= ((ptr_.on[0] & BUTTON_SIDE) != 0);
										//計測リセット
										if ((ptr_.on[0] & BUTTON_TOUCHPAD) != 0 && scoreparts->end_f) { scoreparts->reset(); }
									}
								}
							}
							else {
								//タイマーオン(Bキー)
								scoreparts->ready_f |= (CheckHitKey(KEY_INPUT_B) != 0);
								//計測リセット(Vキー)
								if (CheckHitKey(KEY_INPUT_V) != 0 && scoreparts->end_f) {
									scoreparts->reset();
								}
								//マガジン取得
								mine.down_mag = true;
								//引き金(左クリック)
								easing_set(&mine.obj.get_anime(2).per, float((GetMouseInput() & MOUSE_INPUT_LEFT) != 0), 0.5f, fps);
								//銃変更
								this->chgun.get_in(CheckHitKey(KEY_INPUT_F) != 0);
								//銃の使用
								this->usegun.get_in((CheckHitKey(KEY_INPUT_P) != 0) && (this->sel_g2 != -1));//<---
								if (this->sel_g2 == -1) {
									this->usegun.first = false;
								}
								//ADS
								this->ads.first = ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) && (this->usegun.first && mine.ptr_now->cate == 1);
								//セレクター(中ボタン)
								mine.selkey.get_in(((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0) && (this->usegun.first));
								//銃破棄
								this->delgun.get_in((CheckHitKey(KEY_INPUT_G) != 0) && this->usegun.first);
								//マグキャッチ(Rキー)
								easing_set(&mine.obj.get_anime(5).per, float((CheckHitKey(KEY_INPUT_R) != 0) && this->usegun.first), 0.5f, fps);
								//武装変更
								this->change_gun = std::clamp<uint8_t>(this->change_gun + 1, 0, ((GetMouseWheelRotVol() != 0) && this->usegun.first) ? 2 : 0);
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
								c.body.SetMatrix(c.mat*MATRIX_ref::Mtrans(c.pos));
								//
								easing_set(&c.vecadd_LHAND, c.vecadd_LHAND_p, 0.9f, fps);
								easing_set(&c.vecadd_LHAND_p, VGet(0, 0, 1.f), 0.975f, fps);
								//リコイル
								if (c.gunf) {
									if (c.ammo_cnt >= 1) {
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
								if (c.ptr_now->cate == 1) {
									//マガジン排出
									if (c.obj.get_anime(5).per >= 0.5f && !c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
										c.audio.mag_down.play_3D(c.pos + c.pos_LHAND, 1.f);
										int dnm = int(c.ammo_cnt) - 1;
										//弾数
										if (c.ammo_cnt >= 1) {
											c.ammo_cnt = 1;
										}
										else {
											dnm = 0;
										}
										c.gun_stat[c.gun_slot[this->sel_g2].ptr->id].in -= dnm;
										//バイブレーション　バッテリー消費が激しいためコメントアウト
										/*
										if (&c == &mine) {
											vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
										}
										*/
										//マガジン排出
										c.reload_cnt = 0.f;
										c.gun_stat[c.ptr_now->id].mag_in.erase(c.gun_stat[c.ptr_now->id].mag_in.begin());
										//マガジン排出
										bool tt = false;
										for (auto& g : this->magitem) {
											if (g.ptr == nullptr) {
												tt = true;
												g.set(c.ptr_now, c.pos_mag,c.mat_mag,1);
												g.add = (c.obj.frame(c.ptr_now->frame[1].first) - c.obj.frame(c.ptr_now->frame[0].first)).Norm()*-1.f / fps;//排莢ベクトル
												g.cap = dnm;
												break;
											}
										}
										if (!tt) {
											this->magitem.resize(this->magitem.size() + 1);
											auto& g = this->magitem.back();
											g.set(c.ptr_now, c.pos_mag,c.mat_mag, 1);
											g.add = (c.obj.frame(c.ptr_now->frame[1].first) - c.obj.frame(c.ptr_now->frame[0].first)).Norm()*-1.f / fps;//排莢ベクトル
											g.cap = dnm;
										}
										//
										c.reloadf = true;
									}
									//セレクター
									if (c.selkey.second == 1) {
										++c.gun_stat[c.gun_slot[this->sel_g2].ptr->id].select %= c.ptr_now->select.size();
									}
								}
								//
								if (c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
									c.reload_cnt += 1.f / fps;
									if (c.reload_cnt < c.ptr_now->reload_time) {
										c.down_mag = false;
									}
								}
								//セフティ
								easing_set(&c.obj.get_anime(4).per, float(0.f), 0.5f, fps);
								//射撃
								if (!c.gunf && c.ammo_cnt >= 1) {
									if (c.ptr_now->select[c.gun_stat[c.gun_slot[this->sel_g2].ptr->id].select] == 2) {//フルオート用
										c.trigger.second = 0;
									}
								}
								c.trigger.get_in(c.obj.get_anime(2).per >= 0.5f);
								if (c.trigger.second == 1) {
									c.audio.trigger.play_3D(c.pos + c.pos_LHAND, 1.f);
								}
								switch (c.ptr_now->cate) {
								case 0:
								{
									//近接
									if (c.trigger.second == 1 && !c.gunf) {
										c.gunf = true;
										/*
										if (&c == &mine) {
											vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
										}
										*/
										//サウンド
										c.audio.shot.play_3D(c.pos + c.pos_LHAND, 1.f);
										c.audio.slide.play_3D(c.pos + c.pos_LHAND, 1.f);
										//スコア
										if (scoreparts->start_f && !scoreparts->end_f) {
											scoreparts->sub(-4);
										}
									}
									break;
								}
								case 1:
								{
									if (c.trigger.second == 1 && !c.gunf && c.ammo_cnt >= 1) {
										c.gunf = true;
										/*
										if (&c == &mine) {
											vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
										}
										*/
										//弾数管理
										c.ammo_cnt--;
										c.gun_stat[c.gun_slot[this->sel_g2].ptr->id].in--;
										if (!c.reloadf && c.gun_stat[c.gun_slot[this->sel_g2].ptr->id].mag_in.size() >= 1 && c.gun_stat[c.gun_slot[this->sel_g2].ptr->id].mag_in.front() > 0) {
											c.gun_stat[c.gun_slot[this->sel_g2].ptr->id].mag_in.front()--;
										}
										//持ち手を持つとココが相殺される
										c.vecadd_LHAND_p = MATRIX_ref::Vtrans(c.vecadd_LHAND_p,
											MATRIX_ref::RotY(deg2rad(float(int(c.ptr_now->recoil_xdn*100.f) + GetRand(int((c.ptr_now->recoil_xup - c.ptr_now->recoil_xdn)*100.f))) / (100.f*(c.LEFT_hand ? 3.f : 1.f))))*
											MATRIX_ref::RotX(deg2rad(float(int(c.ptr_now->recoil_ydn*100.f) + GetRand(int((c.ptr_now->recoil_yup - c.ptr_now->recoil_ydn)*100.f))) / (100.f*(c.LEFT_hand ? 3.f : 1.f)))));
										//弾
										c.bullet[c.use_bullet].set(&c.ptr_now->ammo[0], c.obj.frame(c.ptr_now->frame[3].first), c.mat_LHAND.zvec()*-1.f);
										//薬莢
										c.cart[c.use_bullet].set(&c.ptr_now->ammo[0], c.obj.frame(c.ptr_now->frame[2].first), (c.obj.frame(c.ptr_now->frame[2].first + 1) - c.obj.frame(c.ptr_now->frame[2].first)).Norm()*2.5f / fps, c.mat_LHAND);
										//エフェクト
										set_effect(&c.effcs[ef_fire], c.obj.frame(c.ptr_now->frame[3].first), c.mat_LHAND.zvec()*-1.f, 0.0025f / 0.1f);

										set_effect(&c.effcs_gun[c.use_effcsgun].effect, c.obj.frame(c.ptr_now->frame[3].first), c.mat_LHAND.zvec()*-1.f, 0.11f / 0.1f);
										set_pos_effect(&c.effcs_gun[c.use_effcsgun].effect, Drawparts->get_effHandle(ef_smoke));
										c.effcs_gun[c.use_effcsgun].ptr = &c.bullet[c.use_bullet];
										c.effcs_gun[c.use_effcsgun].cnt = 0.f;
										++c.use_effcsgun %= c.effcs_gun.size();
										//サウンド
										c.audio.shot.play_3D(c.pos + c.pos_LHAND, 1.f);
										c.audio.slide.play_3D(c.pos + c.pos_LHAND, 1.f);
										//次のIDへ
										++c.use_bullet %= c.bullet.size();
										//スコア
										if (scoreparts->start_f && !scoreparts->end_f) {
											scoreparts->sub(-4);
										}
									}
									//マガジン取得
									if (c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
										if (c.down_mag) {
											auto p = MATRIX_ref::RotVec2(c.mat_RHAND.yvec(), (c.obj.frame(c.ptr_now->frame[0].first) - (c.pos_RHAND + c.pos)));
											c.mat_mag = c.mag.GetFrameLocalMatrix(3)* (c.mat_RHAND*p);
											c.pos_mag = c.pos_RHAND + c.pos;
											if ((c.mag.frame(3) - c.obj.frame(c.ptr_now->frame[0].first)).size() <= 0.1f) {
												c.obj.get_anime(1).time = 0.f;
												c.obj.get_anime(0).per = 1.f;
												c.obj.get_anime(1).per = 0.f;
												if (c.ammo_cnt == 0) {
													c.obj.get_anime(3).per = 1.f;
												}
												c.audio.mag_set.play_3D(c.pos + c.pos_LHAND, 1.f);
												c.ammo_cnt += c.gun_stat[c.gun_slot[this->sel_g2].ptr->id].mag_in.front();
												c.reloadf = false;
											}
										}
									}
									else {
										c.down_mag = false;
										c.mat_mag = c.mat_LHAND;
										c.pos_mag = c.obj.frame(c.ptr_now->frame[1].first);
									}
									break;
								}
								}
								{
									//身体
									{
										VECTOR_ref v_ = c.mat_HMD.zvec();
										float x_1 = -sinf(c.body_yrad);
										float y_1 = cosf(c.body_yrad);
										float x_2 = v_.x();
										float y_2 = -v_.z();
										float r_ = std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
										c.body_yrad += r_ * 90.f / fps / 10.f;
										c.body.SetFrameLocalMatrix(c.body_f.first, MATRIX_ref::RotY(DX_PI_F + c.body_yrad)*MATRIX_ref::Mtrans(c.body_f.second));
										//
										for (size_t i = 0; i < c.gun_slot.size(); i++) {
											if (c.gun_slot[i].ptr != nullptr) {
												c.gun_slot[i].obj.SetMatrix(MATRIX_ref::RotY(DX_PI_F)*MATRIX_ref::RotX(DX_PI_F / 2)*MATRIX_ref::RotY(c.body_yrad)* MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(VGet((float(i) - float(c.gun_slot.size()) *0.5f)*0.1f, -0.15f, -0.3f), MATRIX_ref::RotY(c.body_yrad)) + (c.pos_HMD + c.pos)));
											}
										}
									}
									MATRIX_ref m_inv = MATRIX_ref::RotY(DX_PI_F + c.body_yrad).Inverse();
									//右手
									{
										VECTOR_ref vec_a1 = MATRIX_ref::Vtrans(((c.pos + c.pos_LHAND) - c.body.frame(c.RIGHTarm1_f.first)).Norm()*0.3f, MATRIX_ref::RotY(DX_PI_F + c.body_yrad).Inverse());//基準
										VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm()*0.3f;//x=0とする
										VECTOR_ref vec_a1L2 = VECTOR_ref(VGet(-1.f, 0.f, vec_a1.x() / vec_a1.z())).Norm()*0.3f;//y=0とする

										float cos_a1 = getcos_tri(c.RIGHTarm2_f.second.size(), c.RIGHThand_f.second.size(), (c.body.frame(c.RIGHTarm1_f.first) - (c.pos + c.pos_LHAND)).size());

										VECTOR_ref vec_t = vec_a1 * cos_a1 + vec_a1L1 * std::sqrtf(1.f - cos_a1 * cos_a1);
										c.body.SetFrameLocalMatrix(c.RIGHTarm1_f.first, m_inv*MATRIX_ref::Mtrans(c.RIGHTarm1_f.second));

										MATRIX_ref a1_inv = MATRIX_ref::RotVec2(
											c.body.frame(c.RIGHTarm2_f.first) - c.body.frame(c.RIGHTarm1_f.first), 
											vec_t
										);

										c.body.SetFrameLocalMatrix(c.RIGHTarm1_f.first, a1_inv*m_inv*MATRIX_ref::Mtrans(c.RIGHTarm1_f.second));
										MATRIX_ref a2_inv = MGetIdent();
										c.body.SetFrameLocalMatrix(c.RIGHTarm2_f.first, a2_inv*a1_inv.Inverse()*MATRIX_ref::Mtrans(c.RIGHTarm2_f.second));
										MATRIX_ref hand_inv = MGetIdent();
										c.body.SetFrameLocalMatrix(c.RIGHThand_f.first, hand_inv*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.RIGHThand_f.second));
									}
									//左手
									{
										float dist_ = ((c.pos_RHAND + c.pos) - c.obj.frame(c.ptr_now->frame[6].first)).size();
										if (dist_ <= 0.2f && (!c.reloadf || !c.down_mag)) {
											c.LEFT_hand = true;
											MATRIX_ref m4 = MATRIX_ref::RotZ(deg2rad(-90));
											//	c.body.SetFrameLocalMatrix(17, m4*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.obj.frame(c.ptr_now->frame[6].first) - (c.pos_HMD + c.pos), c.mat_HMD.Inverse())));
											c.body.SetFrameLocalMatrix(c.LEFThand_f.first, c.mat_RHAND*m_inv*MATRIX_ref::Mtrans(c.LEFThand_f.second));
										}
										else {
											c.LEFT_hand = false;
											MATRIX_ref m4 = MATRIX_ref::RotZ(deg2rad(-90))*c.mat_RHAND*c.mat_HMD.Inverse();
											//	c.body.SetFrameLocalMatrix(17, m4*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.pos_RHAND - c.pos_HMD, c.mat_HMD.Inverse())));
											c.body.SetFrameLocalMatrix(c.LEFThand_f.first, c.mat_RHAND*m_inv*MATRIX_ref::Mtrans(c.LEFThand_f.second));
										}
									}
									//頭部
									{
										c.body.SetFrameLocalMatrix(c.head_f.first, c.mat_HMD*m_inv*MATRIX_ref::Mtrans(c.head_f.second));
									}
								}
								c.mag.SetMatrix(c.mat_mag* MATRIX_ref::Mtrans(c.pos_mag));
								c.obj.SetMatrix(c.mat_LHAND*MATRIX_ref::Mtrans(c.pos_LHAND + c.pos));
								c.body.SetMatrix(c.mat*MATRIX_ref::Mtrans(c.pos));
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
										for (auto& tp : tgt_pic) {
											auto q = tp.obj.CollCheck_Line(a.repos, a.pos, 0, 1);
											if (q.HitFlag == TRUE) {
												a.pos = q.HitPosition;
												//
												tp.power = (tp.obj.frame(tgtparts->frame.first) - a.pos).y();
												tp.time = 0.f;
												int UI_xpos = 0;
												int UI_ypos = 0;
												//弾痕処理
												{
													tp.pic.SetDraw_Screen(false);
													VECTOR_ref pvecp = (a.pos - tp.obj.frame(tgtparts->frame.first));
													VECTOR_ref xvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.frame_x.first));
													VECTOR_ref yvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.frame_y.first));
													UI_xpos = int(float(tgtparts->x_size)*(xvecp.Norm().dot(pvecp)) / xvecp.size());//X方向
													UI_ypos = int(float(tgtparts->y_size)*(yvecp.Norm().dot(pvecp)) / yvecp.size());//Y方向
													DrawCircle(UI_xpos, UI_ypos, 10, GetColor(255, 0, 0));//弾痕
												}
												//ポイント判定
												if (scoreparts->start_f && !scoreparts->end_f) {
													int r_, g_, b_, a_;
													GetPixelSoftImage(tgtparts->col_tex, UI_xpos, UI_ypos, &r_, &g_, &b_, &a_);
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
												break;
											}
										}
										if (p.HitFlag == TRUE && a.flug) {
											a.flug = false;
											set_effect(&c.effcs_gndhit[c.use_effcsgndhit], a.pos, p.Normal, 0.025f / 0.1f);
											++c.use_effcsgndhit %= c.effcs_gndhit.size();
										}
									}
									//消す(3秒たった、スピードが0以下、貫通が0以下)
									if (a.cnt >= 3.f || a.spec->speed < 0.f || a.spec->pene <= 0.f) {
										a.flug = false;
									}
									//終了
									if (!a.flug) {
										for (auto& b : c.effcs_gun) {
											if (b.ptr == &a) {
												b.cnt = 2.5f;
												b.effect.handle.SetPos(b.ptr->pos);
												break;
											}
										}
									}
									//
								}
							}
							//薬莢の処理
							for (auto& a : c.cart) {
								a.get(mapparts, c);
							}
							for (auto& t : c.effcs) {
								if (t.id != ef_smoke) {
									set_pos_effect(&t, Drawparts->get_effHandle(int(t.id)));
								}
							}
							for (auto& t : c.effcs_gndhit) {
								set_pos_effect(&t, Drawparts->get_gndhitHandle());
							}
							for (auto& a : c.effcs_gun) {
								if (a.ptr != nullptr) {
									if (a.ptr->flug) {
										a.effect.handle.SetPos(a.ptr->pos);
									}
									if (a.cnt >= 0.f) {
										a.cnt += 1.f / fps;
										if (a.cnt >= 3.f) {
											a.effect.handle.Stop();
											a.cnt = -1.f;
										}
									}
								}
							}
						}
						//gunitem
						mine.canget_gunitem = false;
						for (auto& g : this->gunitem) {
							g.get(mine, gunitem, mapparts, chgun, sel_g2, usegun, change_gun, gunpos_TPS, 0);
						}
						//銃を落とす
						if (this->delgun.second == 1) {
							bool tt = false;
							for (auto& g : this->gunitem) {
								if (g.ptr == nullptr) {
									tt = true;
									g.set(mine.ptr_now, mine.pos + mine.pos_LHAND, mine.mat_LHAND, 0);
									break;
								}
							}
							if (!tt) {
								this->gunitem.resize(this->gunitem.size() + 1);
								auto& g = this->gunitem.back();
								g.set(mine.ptr_now, mine.pos + mine.pos_LHAND, mine.mat_LHAND, 0);
							}
							//
							for (size_t i = 0; i < mine.gun_slot.size(); i++) {
								if (this->sel_g2 == i) {
									if ((this->sel_g2 == mine.gun_slot.size() - 1) || mine.gun_slot[std::clamp(this->sel_g2 + 1, 0, int(mine.gun_slot.size()) - 1)].ptr == nullptr) {
										mine.gun_slot[this->sel_g2].ptr = nullptr;
										mine.gun_slot[this->sel_g2].obj.Dispose();
										this->sel_g2--;
										break;
									}
									else {
										for (size_t j = i; j < mine.gun_slot.size() - 1; j++) {
											mine.gun_slot[j].delete_gun();
											mine.gun_slot[j].set(mine.gun_slot[j + 1].ptr);
										}
										mine.gun_slot.back().delete_gun();
										break;
									}
								}
							}
							//
							auto pos_t = mine.pos;
							auto mat_t = mine.mat;
							mine.delete_chara();
							mine.set_chara(pos_t, mat_t, this->sel_g2);
							this->gunpos_TPS = VGet(0, 0, 0);
							//
							if (this->sel_g2 == -1) {

							}
						}
						//magitem
						mine.canget_magitem = false;
						for (auto& g : this->magitem) {
							g.get(mine, gunitem, mapparts, chgun, sel_g2, usegun, change_gun, gunpos_TPS, 1);
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
							tp.obj.SetFrameLocalMatrix(tgtparts->frame.first, MATRIX_ref::RotX(tp.rad)* MATRIX_ref::Mtrans(tgtparts->frame.second));
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
						if (mine.ptr_now->frame[4].first != INT_MAX) {
							VECTOR_ref cam = mine.obj.frame(mine.ptr_now->frame[4].first);
							VECTOR_ref vec = cam - mine.mat_LHAND.zvec();
							VECTOR_ref yvec = mine.mat_LHAND.yvec();
							Hostpassparts->draw(&this->ScopeScreen, mapparts->sky_draw(cam, vec, yvec, (this->fov / 7.5f) / 4.f), draw_by_shadow, cam, vec, yvec, (this->fov / 7.5f) / 4.f, 100.f, 0.1f);
							mine.ptr_now->mod.lenzScreen.DrawExtendGraph(0, 0, 1080, 1080, true);
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
									UIparts->item_draw(this->gunitem, this->campos_buf,0);
									UIparts->item_draw(this->magitem, this->campos_buf, 1);
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
								UIparts->item_draw(this->gunitem, this->campos_buf, 0);
								UIparts->item_draw(this->magitem, this->campos_buf, 1);
							}
						}
						//ディスプレイ描画
						{
							this->TPS.get_in(CheckHitKey(KEY_INPUT_LCONTROL) != 0);

							VECTOR_ref cam = mine.pos + mine.pos_HMD + 
								MATRIX_ref::Vtrans(
								this->campos_TPS
								//, mine.mat_HMD)
								,MATRIX_ref::RotY(deg2rad(-90)))
								;
							VECTOR_ref vec = mine.pos + mine.pos_HMD + 
								MATRIX_ref::Vtrans(
								VGet(-0.35f, 0.15f, 0.f)
								//, mine.mat_HMD)
								, MATRIX_ref::RotY(deg2rad(-90)))
								;
							if (this->TPS.first) {//TPS視点
								Hostpassparts->draw(&this->outScreen[2], mapparts->sky_draw(cam, vec, VGet(0, 1.f, 0), this->fov), draw_by_shadow, cam, vec, VGet(0, 1.f, 0), this->fov, 100.f, 0.1f);
								GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK, 0.1f, 100.f, this->fov, cam, vec, VGet(0, 1.f, 0));
								{
									this->outScreen[2].DrawExtendGraph(0, 0, this->out_dispx, this->out_dispy, false);
									//ターゲットを映す
									UIparts->TGT_drw(tgt_pic, mine.obj.frame(mine.ptr_now->frame[3].first), MATRIX_ref::Vtrans(VGet(0, 0, -1.f), mine.mat_LHAND), tgtparts->x_size, tgtparts->y_size);
									//スコープのエイム
									if (mine.ptr_now->frame[4].first != INT_MAX) {
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
					p.delete_tgt();
				}
				tgt_pic.clear();
				mapparts->delete_map();
				Drawparts->Delete_Shadow();
			}
		} while (ProcessMessage() == 0 && this->ending);
	}
};