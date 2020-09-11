#pragma once
#define FRAME_RATE 90.f
class main_c : Mainclass {
	const size_t id_mine = 0;			//���@ID
	std::array<GraphHandle, 3> outScreen;	//�X�N���[���o�b�t�@
	GraphHandle BufScreen;					//�X�N���[���o�b�t�@
	GraphHandle ScopeScreen;				//�X�R�[�v�p�X�N���[��
	std::vector<Gun> gun_data;				//GUN�f�[�^
	std::vector<Chara> chara;				//�L����
	std::vector<Items> item_data;			//�E����A�C�e��
	bool ending = true;						//
	int sel_gun = 0;						//
	//�v���C���[����ϐ��Q
	switchs TPS, ads, chgun, delgun, usegun, change_gun;		//����X�C�b�`
	VECTOR_ref gunpos_TPS;										//�}�E�X�G�C���p�e���W
	float xrad_p = 0.f;											//�}�E�X�G�C���p�ϐ��m��
	bool running = false;										//���邩�ۂ�
	switchs squat;
	VECTOR_ref add_pos, add_pos_buf;							//�ړ�
	VECTOR_ref campos, campos_buf, camvec, camup, campos_TPS;	//�J����
	float fov = 0.f, fov_fps = 0.f;								//�J����
	//
	MV1 body_obj;												//�g�̃��f��
public:
	main_c() {
		auto settings = std::make_unique<Setting>();								/*�ݒ�ǂݍ���*/
		auto vrparts = std::make_unique<VRDraw>(settings);							/*VR�`��*/
		settings->useVR_e = vrparts->use_vr;
		settings->set();
		settings->set_dispsize();													/*��ʎw��*/
		auto Drawparts = std::make_unique<DXDraw>("FPS_0", settings, FRAME_RATE);	/*�ėp�N���X*/
		auto UIparts = std::make_unique<UI>(settings);								/*UI*/
		auto Debugparts = std::make_unique<DeBuG>(FRAME_RATE);						/*�f�o�b�O*/
		auto Hostpassparts = std::make_unique<HostPassEffect>(settings);			/*�z�X�g�p�X�G�t�F�N�g*/
		this->outScreen[0] = GraphHandle::Make(settings->dispx, settings->dispy);	/*����*/
		this->outScreen[1] = GraphHandle::Make(settings->dispx, settings->dispy);	/*�E��*/
		this->outScreen[2] = GraphHandle::Make(settings->dispx, settings->dispy);	/*TPS�p*/
		this->BufScreen = GraphHandle::Make(settings->dispx, settings->dispy);		/*�o�b�t�@�X�N���[��*/
		this->ScopeScreen = GraphHandle::Make(1080, 1080);							/*�X�R�[�v*/
		settings->ready_draw_setting();												/*�Z�b�e�B���O*/
		MV1::Load("data/model/body/model.mv1", &this->body_obj, true);				/*�g��*/
		auto mapparts = std::make_unique<Mapclass>(settings);						/*map*/
		//GUN�f�[�^
		{
			this->gun_data.resize(5);
			this->gun_data[0].mod.set("Knife");
			this->gun_data[1].mod.set("1911");
			this->gun_data[2].mod.set("M82A2");
			this->gun_data[3].mod.set("CAR15_M4");
			this->gun_data[4].mod.set("AK74");
		}
		UIparts->load_window("�e���f��");						//���[�h���1
		{
			fill_id(this->gun_data);							//GUN�f�[�^�擾1
			for (auto& g : this->gun_data) { g.set_data(); }	//GUN�f�[�^�擾2
		}
		UIparts->load_window("�e�f�[�^");						//���[�h���2
		vrparts->Set_Device();									//VR�Z�b�g
		do {
			this->fov = deg2rad(settings->useVR_e ? 90 : 45);	//
			//�L�����ݒ�
			int sel_g = 0;
			{
				float gun_yrad = 90.f, ber_r = 0.f, start_fl = 0.f, sets = 0.f;
				VECTOR_ref pos_HMD;
				MATRIX_ref mat_HMD;
				switchs changecnt, setf;
				bool endp = false, startp = false;
				unsigned char restart = 0;
				//
				while (ProcessMessage() == 0) {
					const auto fps = GetFPS();
					const auto waits = GetNowHiPerformanceCount();
					if (!startp) {
						//VR�p
						if (!setf.first) {
							if (settings->useVR_e) {
								vrparts->GetDevicePositionVR(vrparts->get_hmd_num(), &pos_HMD, &mat_HMD);
								if (vrparts->get_hand1_num() != -1) {
									auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand1_num()];
									if (ptr_.turn && ptr_.now) {
										changecnt.get_in((ptr_.on[1] & BUTTON_SIDE) != 0);
										if ((ptr_.on[0] & BUTTON_TRIGGER) != 0) { startp = true; }
									}
								}
							}
							{
								changecnt.get_in(CheckHitKey(KEY_INPUT_P) != 0);
								if (CheckHitKey(KEY_INPUT_SPACE) != 0) { startp = true; }
							}
						}
						//
						if (changecnt.second == 1) {
							++sel_g %= this->gun_data.size();
							ber_r = 0.f;
							UIparts->cursor.play(DX_PLAYTYPE_BACK, TRUE);
						}
						if (startp) {
							UIparts->decision.play(DX_PLAYTYPE_BACK, TRUE);
						}
					}
					else {
						changecnt.second = 0;
					}

					easing_set(&ber_r, float(settings->dispy / 4), 0.95f, fps);

					if (!startp) {
						campos = mat_HMD.zvec() * 0.6f;
						gun_yrad += 10.f / fps;
						if (gun_yrad >= 180.f) {
							gun_yrad = -180.f;
						}
					}
					else {
						easing_set(&campos, VGet(0.f, 0.f, 1.f), 0.95f, fps);
						easing_set(&gun_yrad, 90.f, 0.95f, fps);
						start_fl += 1.f / fps;
						if (start_fl > 3.f) {
							endp = true;
						}
					}
					//VR��ԂɓK�p
					vrparts->Move_Player();
					{
						setf.get_in(CheckHitKey(KEY_INPUT_O) != 0);
						if (setf.first) {
							restart = settings->set_draw_setting();
							if (restart >= 2) {
								setf.first = false;
							}
							easing_set(&sets, 1.f, 0.9f, fps);
						}
						else {
							settings->reset();
							easing_set(&sets, 0.f, 0.9f, fps);
						}
						BufScreen.SetDraw_Screen();
						auto& v = this->gun_data[sel_g];
						{
							int xp = settings->dispx / 2 - settings->dispy / 6;
							int yp = settings->dispy / 2 - settings->dispy / 6;
							UIparts->font18.DrawStringFormat(xp, yp, GetColor(0, 255, 0), "Name  :%s", v.name.c_str());
						}
						outScreen[0].SetDraw_Screen(0.1f, 10.f, fov, campos, VGet(0, 0, 0), VGet(0.f, 1.f, 0.f));
						{
							v.mod.obj.SetMatrix(MATRIX_ref::RotY(deg2rad(gun_yrad)));
							v.mod.obj.DrawModel();
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(255 - int(255.f * start_fl / 3.f), 0, 255));
							BufScreen.DrawExtendGraph(0, 0, settings->dispx, settings->dispy, true);
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(255.f * start_fl / 3.f), 0, 255));
							DrawBox(0, 0, settings->dispx, settings->dispy, GetColor(255, 255, 255), TRUE);
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						}
						if (settings->useVR_e) {
							for (char i = 0; i < 2; i++) {
								GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
								outScreen[0].DrawGraph(0, 0, false);
								vrparts->PutEye((ID3D11Texture2D*)GetUseDirect3D11BackBufferTexture2D(), i);
							}
						}
						GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
						{
							DrawBox(0, 0, settings->out_dispx, settings->out_dispy, GetColor(64, 64, 64), TRUE);
							outScreen[0].DrawExtendGraph(0, 0, settings->out_dispx, settings->out_dispy, true);

							SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(int(215.f*sets), 0, 255));
							settings->settinggraphs().DrawExtendGraph(
								settings->out_dispx / 2 - int(float(settings->out_dispx / 6)*sets), settings->out_dispy / 2 - int(float(settings->out_dispx / 6 * 480 / 640)*sets),
								settings->out_dispx / 2 + int(float(settings->out_dispx / 6)*sets), settings->out_dispy / 2 + int(float(settings->out_dispx / 6 * 480 / 640)*sets),
								true);
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
						}
					}
					DXDraw::Screen_Flip();
					vrparts->Eye_Flip(waits, 90.f);
					if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
						sel_g = -1;
						break;
					}
					//�ݒ�K����ċN��������
					if (restart == 3) {
						//
						settings->save();
						start_me();
						sel_g = -1;
						break;
					}
					//
					if (endp) {
						break;
					}
				}
			}
			if (sel_g < 0) { break; }
			chara.resize(2);
			auto& mine = chara[id_mine];
			//���@�Z�b�g
			mine.Ready_chara(&this->gun_data[sel_g], &this->gun_data[0], this->gun_data.size(), this->body_obj, &this->ScopeScreen);
			mine.Set_chara_Position(VGet(0.0f, 9.0f, 0.f), MGetIdent(), MATRIX_ref::RotY(DX_PI_F));
			mine.Set_chara(0);
			//���̑�
			chara[1].Ready_chara(&this->gun_data[sel_g], &this->gun_data[0], this->gun_data.size(), this->body_obj, &this->ScopeScreen);
			chara[1].Set_chara_Position(VGet(0.0f, 11.0f, 0.f), MGetIdent(), MATRIX_ref::RotY(DX_PI_F));
			chara[1].Set_chara(0);

			this->sel_gun = 0;
			this->usegun.ready(true);
			//�}�b�v�ǂݍ���
			mapparts->Ready_map("data/map");			//mapparts->Ready_map("data/new");
			UIparts->load_window("�}�b�v");
			mapparts->Set_map("data/maps/set.txt", this->item_data, this->gun_data);
			//�`��֘A
			{
				//���C�e�B���O
				Drawparts->Set_Light_Shadow(mapparts->map_col_get().mesh_maxpos(0), mapparts->map_col_get().mesh_minpos(0), VGet(0.5f, -0.5f, 0.5f), [&] {mapparts->map_get().DrawModel(); });
				//��
				Drawparts->Mirror_init(1);
				Drawparts->set_Mirror_obj(0, VGet(-1.0f, 2.0f, 5.f), VGet(1.0f, 2.0f, 5.f), VGet(-1.0f, 0.5f, 5.f), VGet(1.0f, 0.5f, 5.f), GetColorF(1.0f, 1.0f, 1.0f, 1.0f), GetColorU8(255, 255, 255, 255), DX_BLENDMODE_NOBLEND, 255);
			}
			//�`�悷����̂��w�肷��(��)
			auto draw_by_shadow = [&] {
				Drawparts->Draw_by_Shadow([&] {
					mapparts->map_get().DrawModel();
					for (auto& c : this->chara) { c.Draw_chara(this->usegun.first, this->sel_gun); }
					for (auto& g : this->item_data) { g.Draw_item(this->chara[id_mine]); }
					//�e�e
					SetFogEnable(FALSE);
					SetUseLighting(FALSE);
					for (auto& c : this->chara) { c.Draw_ammo(); }
					SetUseLighting(TRUE);
					SetFogEnable(TRUE);
				});
			};
			auto draw_by_shadow_2 = [&] {
				draw_by_shadow();
				Drawparts->Mirror_Render();		// ���̕`��
			};
			//�J�n
			{
				//��
				mapparts->Start_map();
				//�v���C���[����ϐ��Q
				this->xrad_p = 0.f;									//�}�E�X�G�C��
				this->fov_fps = this->fov;							//
				this->TPS.ready(true);
				this->ads.ready(false);
				bool cansh_gun = true;
				mine.start_c = true;
				SetMousePoint(deskx / 2, desky / 2);
				//
				while (ProcessMessage() == 0) {
					const auto fps = GetFPS();
					const auto waits = GetNowHiPerformanceCount();
					Debugparts->put_way();
					{
						//�e�ύX
						{
							if (this->usegun.first) {
								if (this->change_gun.first == true || cansh_gun == false) {
									++this->sel_gun %= mine.gun_slot.size();
									if (mine.gun_slot[this->sel_gun].ptr == nullptr) {
										this->sel_gun = 0;
									}
									mine.Delete_chara();
									mine.Set_chara(this->sel_gun);
									this->gunpos_TPS = VGet(0, 0, 0);
									this->change_gun.first = false;
								}
								cansh_gun = true;
							}
							else if (cansh_gun) {
								mine.Delete_chara();
								mine.Set_chara(-1);
								this->gunpos_TPS = VGet(0, 0, 0);

								if (this->sel_gun > 0) {
									--this->sel_gun;
								}
								else {
									this->sel_gun = int(mine.gun_slot.size() - 1);
									while (true) {
										if (this->sel_gun == -1) {
											break;
										}
										if (mine.gun_slot[this->sel_gun].ptr == nullptr) {
											this->sel_gun--;
										}
										else {
											break;
										}
									}
								}
								cansh_gun = false;
							}
						}
						//�v���C���[����
						{
							//HMD_mat
							if (settings->useVR_e) {
								//+���_�擾
								vrparts->GetDevicePositionVR(vrparts->get_hmd_num(), &mine.pos_HMD, &mine.mat_HMD);
							}
							else {
								auto qkey = (CheckHitKey(KEY_INPUT_Q) != 0);
								auto ekey = (CheckHitKey(KEY_INPUT_E) != 0);
								mine.mat_HMD *= MATRIX_ref::RotAxis(mine.mat_HMD.zvec(), mine.body_zrad).Inverse();
								if (qkey) {
									easing_set(&mine.body_zrad, deg2rad(-30), 0.9f, fps);
								}
								else if (ekey) {
									easing_set(&mine.body_zrad, deg2rad(30), 0.9f, fps);
								}
								else {
									easing_set(&mine.body_zrad, 0.f, 0.9f, fps);
								}
								mine.mat_HMD *= MATRIX_ref::RotAxis(mine.mat_HMD.zvec(), mine.body_zrad);
								int x_m, y_m;
								GetMousePoint(&x_m, &y_m);
								mine.mat_HMD = MATRIX_ref::RotX(-this->xrad_p)*mine.mat_HMD;
								this->xrad_p = std::clamp(this->xrad_p - deg2rad(std::clamp(y_m - desky / 2, -120, 120))*0.1f*this->fov_fps / this->fov, deg2rad(-45), deg2rad(45));
								mine.mat_HMD *= MATRIX_ref::RotY(deg2rad(std::clamp(x_m - deskx / 2, -120, 120))*0.1f*this->fov_fps / this->fov);
								mine.mat_HMD = MATRIX_ref::RotX(this->xrad_p)*mine.mat_HMD;
								SetMousePoint(deskx / 2, desky / 2);
								SetMouseDispFlag(FALSE);
							}
							//�ړ�
							if (settings->useVR_e) {
								if (vrparts->get_hand2_num() != -1) {
									auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand2_num()];
									if (ptr_.turn && ptr_.now) {
										if ((ptr_.on[1] & BUTTON_TOUCHPAD) != 0) {
											//running
											running = (ptr_.on[0] & BUTTON_TOUCHPAD) != 0;
											auto speed = (running ? 4.f : 2.f);
											easing_set(&this->add_pos_buf, (mine.mat_HMD.zvec()*ptr_.touch.y() + mine.mat_HMD.xvec()*ptr_.touch.x())*-speed / fps, 0.95f, fps);
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
								auto wkey = (CheckHitKey(KEY_INPUT_W) != 0);
								auto skey = (CheckHitKey(KEY_INPUT_S) != 0);
								auto akey = (CheckHitKey(KEY_INPUT_A) != 0);
								auto dkey = (CheckHitKey(KEY_INPUT_D) != 0);
								auto jampkey = (CheckHitKey(KEY_INPUT_SPACE) != 0);
								running = (CheckHitKey(KEY_INPUT_LSHIFT) != 0);
								squat.get_in(CheckHitKey(KEY_INPUT_C) != 0);
								if (running) {
									squat.first = false;
								}
								if (this->ads.first) {
									running = false;
								}
								if (!wkey && !skey && !akey && !dkey) {
									running = false;
								}
								auto speed = (running ? 6.f : ((this->ads.first ? 2.f : 4.f)*(squat.first ? 0.4f : 1.f)));
								VECTOR_ref zv_t = mine.mat_HMD.zvec();
								zv_t.y(0.f);
								zv_t = zv_t.Norm();

								VECTOR_ref xv_t = mine.mat_HMD.xvec();
								xv_t.y(0.f);
								xv_t = xv_t.Norm();

								if (wkey) {
									easing_set(&this->add_pos_buf, zv_t*-speed / fps, 0.95f, fps);
								}
								if (skey) {
									easing_set(&this->add_pos_buf, zv_t*speed / fps, 0.95f, fps);
								}
								if (akey) {
									easing_set(&this->add_pos_buf, xv_t*speed / fps, 0.95f, fps);
								}
								if (dkey) {
									easing_set(&this->add_pos_buf, xv_t*-speed / fps, 0.95f, fps);
								}
								if (!wkey && !skey && !akey && !dkey) {
									easing_set(&this->add_pos_buf, VGet(0, 0, 0), 0.95f, fps);
								}
								if (mine.add_ypos == 0.f) {
									if (jampkey && !squat.first) {
										mine.add_ypos = 0.05f;
									}
									this->add_pos = this->add_pos_buf;
								}
								else {
									easing_set(&this->add_pos, VGet(0, 0, 0), 0.995f, fps);
								}
							}
							//�ǂ��̑��̔���
							{
								VECTOR_ref pos_t = mine.pos;
								pos_t += this->add_pos;
								//��
								{
									mapparts->map_col_wall(mine.pos, &pos_t);
									if ((this->add_pos - (pos_t - mine.pos)).size() != 0.f) {
										this->add_pos = pos_t - mine.pos;
										if (mine.add_ypos == 0.f) {
											this->add_pos_buf = this->add_pos;
										}
									}
								}
								//����
								{
									auto pp = mapparts->map_col_line(pos_t + VGet(0, 1.6f, 0), pos_t, 0);
									if (mine.add_ypos <= 0.f && pp.HitFlag == 1) {
										if (VECTOR_ref(VGet(0, 1.f, 0.f)).dot(pp.Normal) >= cos(deg2rad(30))) {
											pos_t = pp.HitPosition;
										}
										else {
											//�u���b�N����x�N�g��
											auto v_t = VECTOR_ref(pp.Normal);
											v_t.y(0);
											v_t = v_t.Norm();
											//
											pos_t -= this->add_pos;
											this->add_pos += v_t * this->add_pos.cross(v_t.cross(this->add_pos).Norm()).size();
											if (mine.add_ypos == 0.f) {
												this->add_pos_buf = this->add_pos;
											}
											pos_t += this->add_pos;
										}
										mine.add_ypos = 0.f;
									}
									else {
										pos_t.yadd(mine.add_ypos);
										mine.add_ypos -= 9.8f / std::powf(fps, 2.f);
										//���A
										if (pos_t.y() <= -5.f) {
											pos_t = VGet(0.f, 9.f, 0.f);
											mine.add_ypos = 0.f;
											mine.body.SetMatrix(mine.mat*MATRIX_ref::Mtrans(pos_t));
											mine.body.PhysicsResetState();
										}
									}
								}
								//���f
								mine.pos = pos_t;
							}
							//pos
							{
								VECTOR_ref v_ = mine.mat_HMD.zvec();
								float x_1 = -sinf(mine.body_yrad);
								float y_1 = cosf(mine.body_yrad);
								float x_2 = v_.x();
								float y_2 = -v_.z();
								float r_ = std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
								mine.body_yrad += r_ * FRAME_RATE / fps / 2.f;
							}
							{
								VECTOR_ref v_ = mine.mat_HMD.zvec();
								float x_1 = sinf(mine.body_xrad);
								float y_1 = -cosf(mine.body_xrad);
								float x_2 = -v_.y();
								float y_2 = -std::hypotf(v_.x(), v_.z());
								mine.body_xrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
							}
							MATRIX_ref t_inv = MATRIX_ref::RotY(DX_PI_F + mine.body_yrad);
							if (settings->useVR_e) {
								//�g��
								MATRIX_ref m_inv = t_inv;
								{
									mine.body.SetMatrix(mine.mat*m_inv);
									mine.body.SetMatrix(mine.mat*m_inv*MATRIX_ref::Mtrans(mine.pos - (mine.body.frame(mine.RIGHTeye_f.first) + (mine.body.frame(mine.LEFTeye_f.first) - mine.body.frame(mine.RIGHTeye_f.first))*0.5f) + mine.pos_HMD));
									//mine.body.SetFrameLocalMatrix(mine.bodyc_f.first, m_inv*MATRIX_ref::Mtrans(mine.bodyc_f.second));
									//mine.body.SetFrameLocalMatrix(mine.body_f.first, m_inv*MATRIX_ref::Mtrans(mine.body_f.second));
									for (size_t i = 0; i < mine.gun_slot.size(); i++) {
										if (mine.gun_slot[i].ptr != nullptr) {
											mine.gun_slot[i].obj.SetMatrix(MATRIX_ref::RotY(DX_PI_F)*MATRIX_ref::RotX(DX_PI_F / 2) *m_inv * MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(VGet((float(i) - float(mine.gun_slot.size()) *0.5f)*0.1f, -0.15f, 0.3f), m_inv) + (mine.pos_HMD + mine.pos)));
										}
									}
								}
								//����
								mine.body.SetFrameLocalMatrix(mine.head_f.first, mine.mat_HMD*m_inv.Inverse()*MATRIX_ref::Mtrans(mine.head_f.second));
								//��
								{

								}
								//��
								{
									mine.body.get_anime(3).per = 0.f;
									mine.body.get_anime(3).time = 0.f;
									//�E��
									{
										vrparts->GetDevicePositionVR(vrparts->get_hand1_num(), &mine.pos_RIGHTHAND, &mine.mat_RIGHTHAND);
										mine.mat_RIGHTHAND = mine.mat_RIGHTHAND*MATRIX_ref::RotAxis(mine.mat_RIGHTHAND.xvec(), deg2rad(-60));
										//easing_set(&this->campos_TPS, VGet(-0.35f, 0.15f, 1.f), 0.95f, fps);
										mine.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), mine.vecadd_RIGHTHAND)*mine.mat_RIGHTHAND;//���R�C��
										//�e��
										mine.obj.SetMatrix(mine.mat_RIGHTHAND*MATRIX_ref::Mtrans(mine.pos_RIGHTHAND + mine.pos));
										VECTOR_ref tgt_pt = mine.obj.frame(mine.ptr_now->frame[8].first);
										//
										VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - mine.body.frame(mine.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());//�
										VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
										float cos_t = getcos_tri((mine.body.frame(mine.RIGHThand_f.first) - mine.body.frame(mine.RIGHTarm2_f.first)).size(), (mine.body.frame(mine.RIGHTarm2_f.first) - mine.body.frame(mine.RIGHTarm1_f.first)).size(), (mine.body.frame(mine.RIGHTarm1_f.first) - tgt_pt).size());
										VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
										//��r
										mine.body.SetFrameLocalMatrix(mine.RIGHTarm1_f.first, MATRIX_ref::Mtrans(mine.RIGHTarm1_f.second));
										MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(mine.body.frame(mine.RIGHTarm2_f.first) - mine.body.frame(mine.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
										mine.body.SetFrameLocalMatrix(mine.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(mine.RIGHTarm1_f.second));
										//���r
										mine.body.SetFrameLocalMatrix(mine.RIGHTarm2_f.first, MATRIX_ref::Mtrans(mine.RIGHTarm2_f.second));
										MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(mine.body.frame(mine.RIGHThand_f.first) - mine.body.frame(mine.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - mine.body.frame(mine.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
										mine.body.SetFrameLocalMatrix(mine.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(mine.RIGHTarm2_f.second));
										//��
										mine.body.SetFrameLocalMatrix(mine.RIGHThand_f.first,
											MATRIX_ref::RotY(deg2rad(-10))*
											MATRIX_ref::RotZ(deg2rad(50))*
											MATRIX_ref::RotX(deg2rad(90))*
											mine.mat_RIGHTHAND*
											m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(mine.RIGHThand_f.second));
										//�E�l�����w
										mine.body.get_anime(0).per = 1.f;
										mine.body.get_anime(5).per = mine.obj.get_anime(2).per;
									}
									//����
									{
										vrparts->GetDevicePositionVR(vrparts->get_hand2_num(), &mine.pos_LEFTHAND, &mine.mat_LEFTHAND);
										mine.mat_LEFTHAND = mine.mat_LEFTHAND*MATRIX_ref::RotAxis(mine.mat_LEFTHAND.xvec(), deg2rad(-60));

										/*
										mine.pos_LEFTHAND = mine.obj.frame(mine.ptr_now->frame[6].first) - mine.pos;
										if (mine.down_mag) {
											mine.pos_LEFTHAND = mine.obj.frame(mine.ptr_now->frame[0].first) + mine.mat_RIGHTHAND.yvec()*-0.05f - mine.pos;
										}
										mine.mat_LEFTHAND = mine.mat_HMD;
										*/
										float dist_ = ((mine.pos_LEFTHAND + mine.pos) - mine.obj.frame(mine.ptr_now->frame[6].first)).size();
										if (dist_ <= 0.1f && (!mine.reloadf || !mine.down_mag)) {
											mine.LEFT_hand = true;
											mine.pos_LEFTHAND = mine.obj.frame(mine.ptr_now->frame[6].first) - mine.pos;
										}
										else {
											mine.LEFT_hand = false;
										}
										{
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans(((mine.pos + mine.pos_LEFTHAND) - mine.body.frame(mine.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//�
											VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
											float cos_t = getcos_tri((mine.body.frame(mine.LEFThand_f.first) - mine.body.frame(mine.LEFTarm2_f.first)).size(), (mine.body.frame(mine.LEFTarm2_f.first) - mine.body.frame(mine.LEFTarm1_f.first)).size(), (mine.body.frame(mine.LEFTarm1_f.first) - (mine.pos + mine.pos_LEFTHAND)).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//��r
											mine.body.SetFrameLocalMatrix(mine.LEFTarm1_f.first, MATRIX_ref::Mtrans(mine.LEFTarm1_f.second));
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(
												MATRIX_ref::Vtrans(mine.body.frame(mine.LEFTarm2_f.first) - mine.body.frame(mine.LEFTarm1_f.first), m_inv.Inverse()),
												vec_t
											);
											mine.body.SetFrameLocalMatrix(mine.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(mine.LEFTarm1_f.second));
											//���r
											mine.body.SetFrameLocalMatrix(mine.LEFTarm2_f.first, MATRIX_ref::Mtrans(mine.LEFTarm2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
												MATRIX_ref::Vtrans(mine.body.frame(mine.LEFThand_f.first) - mine.body.frame(mine.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()),
												MATRIX_ref::Vtrans((mine.pos + mine.pos_LEFTHAND) - mine.body.frame(mine.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse())
											);
											mine.body.SetFrameLocalMatrix(mine.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(mine.LEFTarm2_f.second));
											//��
											mine.body.SetFrameLocalMatrix(mine.LEFThand_f.first,
												MATRIX_ref::RotZ(deg2rad(-60))*
												MATRIX_ref::RotX(deg2rad(80))*
												mine.mat_LEFTHAND*
												m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(mine.LEFThand_f.second));
										}
									}
								}
							}
							else {
								//�g��
								MATRIX_ref m_inv = MATRIX_ref::RotY(deg2rad(30))*MATRIX_ref::RotZ(mine.body_zrad)*MATRIX_ref::RotX(mine.body_xrad)*t_inv;
								MATRIX_ref mb_inv = MATRIX_ref::RotY(deg2rad(15))*t_inv;
								MATRIX_ref mg_inv = t_inv;
								{
									if (mine.reloadf) {
										m_inv = MATRIX_ref::RotZ(mine.body_zrad)*MATRIX_ref::RotX(mine.body_xrad)*t_inv;
										mb_inv = t_inv;
									}
									mine.body.SetMatrix(mine.mat*MATRIX_ref::Mtrans(mine.pos));
									//
									mine.body.SetFrameLocalMatrix(mine.bodyg_f.first, mg_inv*MATRIX_ref::Mtrans(mine.bodyg_f.second));
									mine.body.SetFrameLocalMatrix(mine.bodyb_f.first, mb_inv*mg_inv.Inverse()*MATRIX_ref::Mtrans(mine.bodyb_f.second));
									mine.body.SetFrameLocalMatrix(mine.body_f.first, m_inv*mb_inv.Inverse()*MATRIX_ref::Mtrans(mine.body_f.second));
									//
									for (size_t i = 0; i < mine.gun_slot.size(); i++) {
										if (mine.gun_slot[i].ptr != nullptr) {
											mine.gun_slot[i].obj.SetMatrix(MATRIX_ref::RotY(DX_PI_F)*MATRIX_ref::RotX(DX_PI_F / 2) *m_inv * MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(VGet((float(i) - float(mine.gun_slot.size()) *0.5f)*0.1f, -0.15f, 0.3f), m_inv) + (mine.pos_HMD + mine.pos)));
										}
									}
								}
								//����
								mine.body.SetFrameLocalMatrix(mine.head_f.first, mine.mat_HMD*m_inv.Inverse()*MATRIX_ref::Mtrans(mine.head_f.second));
								if (mine.reloadf) {
									mine.body.frame_reset(mine.head_f.first);
								}
								//��
								{
									auto speed = (running ? 6.f : ((this->ads.first ? 2.f : 4.f)*(squat.first ? 0.4f : 1.f)));
									auto ratio_t = this->add_pos.size() / (speed / fps);
									if (running) {
										easing_set(&mine.body.get_anime(8).per, 0.f, 0.95f, fps);
										easing_set(&mine.body.get_anime(7).per, 0.f, 0.95f, fps);
										easing_set(&mine.body.get_anime(2).per, 1.f*ratio_t, 0.95f, fps);
										easing_set(&mine.body.get_anime(1).per, 0.f, 0.95f, fps);
									}
									else if (this->ads.first) {
										easing_set(&mine.body.get_anime(2).per, 0.f, 0.95f, fps);
										if (!squat.first) {
											easing_set(&mine.body.get_anime(1).per, 0.5f*ratio_t, 0.95f, fps);
											easing_set(&mine.body.get_anime(8).per, 0.f, 0.9f, fps);
											easing_set(&mine.body.get_anime(7).per, 0.f, 0.9f, fps);
										}
										else {
											easing_set(&mine.body.get_anime(1).per, 0.f, 0.95f, fps);
											easing_set(&mine.body.get_anime(8).per, 0.5f*ratio_t, 0.9f, fps);
											easing_set(&mine.body.get_anime(7).per, 1.f - 1.f*ratio_t, 0.9f, fps);
										}
									}
									else {
										easing_set(&mine.body.get_anime(2).per, 0.f, 0.95f, fps);
										if (!squat.first) {
											easing_set(&mine.body.get_anime(1).per, 1.f*ratio_t, 0.95f, fps);
											easing_set(&mine.body.get_anime(8).per, 0.f, 0.9f, fps);
											easing_set(&mine.body.get_anime(7).per, 0.f, 0.9f, fps);
										}
										else {
											easing_set(&mine.body.get_anime(1).per, 0.f, 0.95f, fps);
											easing_set(&mine.body.get_anime(8).per, 1.f*ratio_t, 0.9f, fps);
											easing_set(&mine.body.get_anime(7).per, 1.f - 1.f*ratio_t, 0.9f, fps);
										}
										easing_set(&mine.body.get_anime(0).per, 0.f, 0.95f, fps);
									}
									mine.body.get_anime(1).time += 30.f / fps;
									if (mine.body.get_anime(1).time >= mine.body.get_anime(1).alltime) {
										mine.body.get_anime(1).time = 0.f;
									}
									mine.body.get_anime(2).time += 30.f / fps;
									if (mine.body.get_anime(2).time >= mine.body.get_anime(2).alltime) {
										mine.body.get_anime(2).time = 0.f;
									}
									mine.body.get_anime(8).time += 30.f / fps * ((mine.body.get_anime(8).alltime / 30.f) / mine.ptr_now->reload_time);
									if (mine.body.get_anime(8).time >= mine.body.get_anime(8).alltime) {
										mine.body.get_anime(8).time = 0.f;
									}
								}
								//���_
								{
									VECTOR_ref pv = VGet(0, 0, 0);
									if (mine.ptr_now->frame[4].first != INT_MAX) {
										pv = mine.ptr_now->frame[4].second;
									}
									else if (mine.ptr_now->frame[7].first != INT_MAX) {
										pv = mine.ptr_now->frame[7].second;
									}
									if (this->ads.first) {
										easing_set(&this->gunpos_TPS, VGet(-0.035f, 0.f - pv.y(), -0.225f), 0.75f, fps);
										easing_set(&this->fov_fps, (this->fov*0.6f) / ((mine.ptr_now->frame[4].first != INT_MAX) ? 4.f : 1.f), 0.9f, fps);
										easing_set(&this->campos_TPS, VGet(-0.35f, 0.125f, 1.f), 0.9f, fps);
									}
									else {
										if (running) {
											easing_set(&this->gunpos_TPS, VGet(-0.1f, -0.1f - pv.y(), -0.25f), 0.9f, fps);
											easing_set(&this->fov_fps, (this->fov*1.2f), 0.9f, fps);
											easing_set(&this->campos_TPS, VGet(-0.35f, 0.125f, 3.f), 0.95f, fps);
										}
										else {
											easing_set(&this->gunpos_TPS, VGet(-0.1f, -0.05f - pv.y(), -0.3f), 0.75f, fps);
											easing_set(&this->fov_fps, this->fov, 0.9f, fps);
											easing_set(&this->campos_TPS, VGet(-0.35f, 0.125f, 1.75f), 0.95f, fps);
										}
									}
								}
								//��
								{
									mine.body.frame_reset(mine.RIGHTarm1_f.first);
									mine.body.frame_reset(mine.RIGHTarm2_f.first);
									mine.body.frame_reset(mine.RIGHThand_f.first);
									mine.body.frame_reset(mine.LEFTarm1_f.first);
									mine.body.frame_reset(mine.LEFTarm2_f.first);
									mine.body.frame_reset(mine.LEFThand_f.first);


									if (running) {
										mine.body.get_anime(6).per = 1.f;
										mine.body.get_anime(6).time += 30.f / fps;
										if (mine.body.get_anime(6).time >= mine.body.get_anime(6).alltime) {
											mine.body.get_anime(6).time = 0.f;
										}
										easing_set(&this->fov_fps, this->fov, 0.9f, fps);
									}
									else {
										mine.body.get_anime(6).per = 0.f;
										mine.body.get_anime(6).time = 0.f;
										if (mine.reloadf && mine.gun_stat[mine.ptr_now->id].mag_in.size() >= 1) {
											mine.body.get_anime(3).per = 1.f;
											mine.body.get_anime(3).time += 30.f / fps * ((mine.body.get_anime(3).alltime / 30.f) / mine.ptr_now->reload_time);
											if (mine.body.get_anime(3).time >= mine.body.get_anime(3).alltime) {
												mine.body.get_anime(3).time = 0.f;
											}
											easing_set(&this->fov_fps, this->fov, 0.9f, fps);
										}
										else {
											mine.body.get_anime(3).per = 0.f;
											mine.body.get_anime(3).time = 0.f;
											//�E��
											{
												//���_���ꎞ�擾
												mine.pos_HMD = (mine.body.frame(mine.RIGHTeye_f.first) + (mine.body.frame(mine.LEFTeye_f.first) - mine.body.frame(mine.RIGHTeye_f.first))*0.5f) - mine.pos;
												//�e��
												mine.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), mine.vecadd_RIGHTHAND)*mine.mat_HMD;//���R�C��
												mine.pos_RIGHTHAND = mine.pos_HMD + MATRIX_ref::Vtrans(this->gunpos_TPS, mine.mat_RIGHTHAND);
												mine.obj.SetMatrix(mine.mat_RIGHTHAND*MATRIX_ref::Mtrans(mine.pos_RIGHTHAND + mine.pos));
												VECTOR_ref tgt_pt = mine.obj.frame(mine.ptr_now->frame[8].first);
												//�
												VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - mine.body.frame(mine.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());
												VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
												float cos_t = getcos_tri((mine.body.frame(mine.RIGHThand_f.first) - mine.body.frame(mine.RIGHTarm2_f.first)).size(), (mine.body.frame(mine.RIGHTarm2_f.first) - mine.body.frame(mine.RIGHTarm1_f.first)).size(), (mine.body.frame(mine.RIGHTarm1_f.first) - tgt_pt).size());
												VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
												//��r
												mine.body.SetFrameLocalMatrix(mine.RIGHTarm1_f.first, MATRIX_ref::Mtrans(mine.RIGHTarm1_f.second));
												MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(mine.body.frame(mine.RIGHTarm2_f.first) - mine.body.frame(mine.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
												mine.body.SetFrameLocalMatrix(mine.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(mine.RIGHTarm1_f.second));
												//���r
												mine.body.SetFrameLocalMatrix(mine.RIGHTarm2_f.first, MATRIX_ref::Mtrans(mine.RIGHTarm2_f.second));
												MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(mine.body.frame(mine.RIGHThand_f.first) - mine.body.frame(mine.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - mine.body.frame(mine.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
												mine.body.SetFrameLocalMatrix(mine.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(mine.RIGHTarm2_f.second));
												//��
												mine.body.SetFrameLocalMatrix(mine.RIGHThand_f.first, MATRIX_ref::RotY(deg2rad(-10))* MATRIX_ref::RotZ(deg2rad(50))* MATRIX_ref::RotX(deg2rad(90))* mine.mat_RIGHTHAND* m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(mine.RIGHThand_f.second));
											}
											//����
											{
												mine.pos_LEFTHAND = mine.obj.frame(mine.ptr_now->frame[6].first) - mine.pos;
												if (mine.down_mag) {
													mine.pos_LEFTHAND = mine.obj.frame(mine.ptr_now->frame[0].first) + mine.mat_RIGHTHAND.yvec()*-0.05f - mine.pos;
												}
												mine.mat_LEFTHAND = mine.mat_HMD;

												float dist_ = ((mine.pos_LEFTHAND + mine.pos) - mine.obj.frame(mine.ptr_now->frame[6].first)).size();
												if (dist_ <= 0.2f && (!mine.reloadf || !mine.down_mag)) {
													mine.LEFT_hand = true;
													mine.pos_LEFTHAND = mine.obj.frame(mine.ptr_now->frame[6].first) - mine.pos;
												}
												else {
													mine.LEFT_hand = false;
												}
												{
													VECTOR_ref vec_a1 = MATRIX_ref::Vtrans(((mine.pos + mine.pos_LEFTHAND) - mine.body.frame(mine.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//�
													VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
													float cos_t = getcos_tri((mine.body.frame(mine.LEFThand_f.first) - mine.body.frame(mine.LEFTarm2_f.first)).size(), (mine.body.frame(mine.LEFTarm2_f.first) - mine.body.frame(mine.LEFTarm1_f.first)).size(), (mine.body.frame(mine.LEFTarm1_f.first) - (mine.pos + mine.pos_LEFTHAND)).size());
													VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
													//��r
													mine.body.SetFrameLocalMatrix(mine.LEFTarm1_f.first, MATRIX_ref::Mtrans(mine.LEFTarm1_f.second));
													MATRIX_ref a1_inv = MATRIX_ref::RotVec2(
														MATRIX_ref::Vtrans(mine.body.frame(mine.LEFTarm2_f.first) - mine.body.frame(mine.LEFTarm1_f.first), m_inv.Inverse()),
														vec_t
													);
													mine.body.SetFrameLocalMatrix(mine.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(mine.LEFTarm1_f.second));
													//���r
													mine.body.SetFrameLocalMatrix(mine.LEFTarm2_f.first, MATRIX_ref::Mtrans(mine.LEFTarm2_f.second));
													MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
														MATRIX_ref::Vtrans(mine.body.frame(mine.LEFThand_f.first) - mine.body.frame(mine.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()),
														MATRIX_ref::Vtrans((mine.pos + mine.pos_LEFTHAND) - mine.body.frame(mine.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse())
													);
													mine.body.SetFrameLocalMatrix(mine.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(mine.LEFTarm2_f.second));
													//��
													mine.body.SetFrameLocalMatrix(mine.LEFThand_f.first,
														MATRIX_ref::RotZ(deg2rad(-60))*
														MATRIX_ref::RotX(deg2rad(80))*
														mine.mat_LEFTHAND*
														m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(mine.LEFThand_f.second));
												}
											}
										}
									}
									//�E�l�����w
									{
										mine.body.get_anime(0).per = 1.f;
										mine.body.get_anime(5).per = mine.obj.get_anime(2).per;
									}
								}
							}
							mine.body.work_anime();
							if (!settings->useVR_e) {
								//���_�擾
								mine.pos_HMD = (mine.body.frame(mine.RIGHTeye_f.first) + (mine.body.frame(mine.LEFTeye_f.first) - mine.body.frame(mine.RIGHTeye_f.first))*0.5f) - mine.pos;
								//
								if (running) {
									//�e��
									mine.mat_RIGHTHAND = MATRIX_ref::RotY(deg2rad(45))* MATRIX_ref::RotX(deg2rad(-90))* mine.body.GetFrameLocalWorldMatrix(mine.RIGHThand2_f.first);
									mine.pos_RIGHTHAND = mine.body.frame(mine.RIGHThand_f.first) - mine.pos;
									mine.obj.SetMatrix(mine.mat_RIGHTHAND*MATRIX_ref::Mtrans(mine.pos_RIGHTHAND + mine.pos));
									mine.pos_RIGHTHAND -= mine.obj.frame(mine.ptr_now->frame[8].first) - (mine.pos_RIGHTHAND + mine.pos);
									mine.obj.SetMatrix(mine.mat_RIGHTHAND*MATRIX_ref::Mtrans(mine.pos_RIGHTHAND + mine.pos));
									//
									mine.mat_LEFTHAND = MATRIX_ref::RotY(deg2rad(-90 + 45))* MATRIX_ref::RotX(deg2rad(-90))*  (mine.body.GetFrameLocalWorldMatrix(mine.LEFThand2_f.first)*MATRIX_ref::Mtrans(mine.body.frame(mine.LEFThand2_f.first)).Inverse());
									mine.pos_LEFTHAND = mine.body.frame(mine.LEFThand_f.first) - mine.pos + mine.mat_LEFTHAND.yvec()*0.1f;
								}
								else {
									if (mine.reloadf && mine.gun_stat[mine.ptr_now->id].mag_in.size() >= 1) {
										//�e��
										mine.mat_RIGHTHAND = MATRIX_ref::RotY(deg2rad(45))* MATRIX_ref::RotX(deg2rad(-90))* mine.body.GetFrameLocalWorldMatrix(mine.RIGHThand2_f.first);
										mine.pos_RIGHTHAND = mine.body.frame(mine.RIGHThand_f.first) - mine.pos;
										mine.obj.SetMatrix(mine.mat_RIGHTHAND*MATRIX_ref::Mtrans(mine.pos_RIGHTHAND + mine.pos));
										mine.pos_RIGHTHAND -= mine.obj.frame(mine.ptr_now->frame[8].first) - (mine.pos_RIGHTHAND + mine.pos);
										mine.obj.SetMatrix(mine.mat_RIGHTHAND*MATRIX_ref::Mtrans(mine.pos_RIGHTHAND + mine.pos));
										//
										mine.mat_LEFTHAND = MATRIX_ref::RotY(deg2rad(-90 + 45))* MATRIX_ref::RotX(deg2rad(-90))*  (mine.body.GetFrameLocalWorldMatrix(mine.LEFThand2_f.first)*MATRIX_ref::Mtrans(mine.body.frame(mine.LEFThand2_f.first)).Inverse());
										mine.pos_LEFTHAND = mine.body.frame(mine.LEFThand_f.first) - mine.pos + mine.mat_LEFTHAND.yvec()*0.1f;
									}
									else {
										mine.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), mine.vecadd_RIGHTHAND)*mine.mat_HMD;//���R�C��
										mine.pos_RIGHTHAND = mine.pos_HMD + MATRIX_ref::Vtrans(this->gunpos_TPS, mine.mat_RIGHTHAND);
										mine.obj.SetMatrix(mine.mat_RIGHTHAND*MATRIX_ref::Mtrans(mine.pos_RIGHTHAND + mine.pos));
									}
								}
							}
							//�e����
							{
								if (mine.obj.get_anime(3).per == 1.f) {
									mine.audio.slide.play_3D(mine.pos + mine.pos_RIGHTHAND, 1.f);
								}
								mine.obj.get_anime(3).per = std::max(mine.obj.get_anime(3).per - 12.f / fps, 0.f);
							}
							//����
							if (settings->useVR_e) {
								if (vrparts->get_hand1_num() != -1) {
									auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand1_num()];
									if (ptr_.turn && ptr_.now) {
										//������
										easing_set(&mine.obj.get_anime(2).per, float((ptr_.on[0] & BUTTON_TRIGGER) != 0), 0.5f, fps);
										//�}�O�L���b�`
										easing_set(&mine.obj.get_anime(5).per, float((ptr_.on[0] & BUTTON_SIDE) != 0), 0.5f, fps);
										//�Z���N�^�[
										mine.selkey.get_in(((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_.touch.x() > 0.5f && ptr_.touch.y() < 0.5f&&ptr_.touch.y() > -0.5f));
										//�e�̎g�p
										this->usegun.get_in((CheckHitKey(KEY_INPUT_P) != 0) && (this->sel_gun != -1));//<---
										if (this->sel_gun == -1) {
											this->usegun.first = false;
										}
										//�����ύX
										this->change_gun.get_in(((ptr_.on[0] & BUTTON_TOPBUTTON) != 0) && this->usegun.first);
									}
								}
								if (vrparts->get_hand2_num() != -1) {
									auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand2_num()];
									if (ptr_.turn && ptr_.now) {
										//�}�K�W���擾
										mine.down_mag |= (((ptr_.on[0] & BUTTON_TRIGGER) != 0) && (mine.gun_stat[mine.ptr_now->id].mag_in.size() >= 1));
										//�e�ύX
										this->chgun.get_in((ptr_.on[0] & BUTTON_TOPBUTTON) != 0);
										//�e�j��
										this->delgun.get_in((CheckHitKey(KEY_INPUT_G) != 0) && this->usegun.first);
									}
								}
							}
							else {
								//�}�K�W���擾
								mine.down_mag = true;
								//������(���N���b�N)
								easing_set(&mine.obj.get_anime(2).per, float(((GetMouseInput() & MOUSE_INPUT_LEFT) != 0) && !running), 0.5f, fps);
								if (!this->ads.first) {
									//�e�擾
									this->chgun.get_in(CheckHitKey(KEY_INPUT_F) != 0);
									//�e�̎g�p
									this->usegun.get_in((CheckHitKey(KEY_INPUT_P) != 0) && (this->sel_gun != -1));//<---
									if (this->sel_gun == -1) {
										this->usegun.first = false;
									}
								}
								//ADS
								this->ads.first = ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) && (this->usegun.first && mine.ptr_now->cate == 1) && (!mine.reloadf);
								//�Z���N�^�[(���{�^��)
								mine.selkey.get_in(((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0) && (this->usegun.first));
								//�e�j��
								this->delgun.get_in((CheckHitKey(KEY_INPUT_G) != 0) && this->usegun.first);
								//�}�O�L���b�`(R�L�[)
								easing_set(&mine.obj.get_anime(5).per, float((CheckHitKey(KEY_INPUT_R) != 0) && this->usegun.first), 0.5f, fps);
								if (!this->ads.first) {
									//�����ύX
									this->change_gun.get_in((GetMouseWheelRotVol() != 0) && this->usegun.first);
								}
							}
							//�ˌ��֘A
							{
								{
									//����
									easing_set(&mine.vecadd_RIGHTHAND, mine.vecadd_RIGHTHAND_p, 0.9f, fps);
									easing_set(&mine.vecadd_RIGHTHAND_p, VGet(0, 0, 1.f), 0.975f, fps);
									//���R�C��
									if (mine.gunf) {
										if (mine.ammo_cnt >= 1) {
											mine.obj.get_anime(0).per = 1.f;
											mine.obj.get_anime(1).per = 0.f;
											mine.obj.get_anime(0).time += 60.f / fps;
											if (mine.obj.get_anime(0).time >= mine.obj.get_anime(0).alltime) {
												mine.obj.get_anime(0).time = 0.f;
												mine.gunf = false;
											}
										}
										else {
											mine.obj.get_anime(1).per = 1.f;
											mine.obj.get_anime(0).per = 0.f;
											mine.obj.get_anime(1).time += 60.f / fps;
											if (mine.obj.get_anime(1).time >= mine.obj.get_anime(1).alltime) {
												mine.obj.get_anime(1).time = mine.obj.get_anime(1).alltime;
												mine.gunf = false;
											}
										}

									}
									if (mine.ptr_now->cate == 1) {
										//�}�K�W���r�o
										if (mine.obj.get_anime(5).per >= 0.5f && !mine.reloadf && mine.gun_stat[mine.ptr_now->id].mag_in.size() >= 1) {
											mine.audio.mag_down.play_3D(mine.pos + mine.pos_RIGHTHAND, 1.f);
											int dnm = int(mine.ammo_cnt) - 1;
											//�e��
											if (mine.ammo_cnt >= 1) {
												mine.ammo_cnt = 1;
											}
											else {
												dnm = 0;
											}
											mine.gun_stat[mine.gun_slot[this->sel_gun].ptr->id].in -= dnm;
											//�o�C�u���[�V�����@�o�b�e���[������������߃R�����g�A�E�g
											/*
												vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
											*/
											//�}�K�W���r�o
											mine.reload_cnt = 0.f;
											mine.gun_stat[mine.ptr_now->id].mag_in.erase(mine.gun_stat[mine.ptr_now->id].mag_in.begin());
											//�}�K�W���r�o
											bool tt = false;
											for (auto& g : this->item_data) {
												if (g.ptr == nullptr && g.cate == 1) {
													tt = true;
													g.Set_item(mine.ptr_now, mine.pos_mag, mine.mat_mag, 1);
													g.add = (mine.obj.frame(mine.ptr_now->frame[1].first) - mine.obj.frame(mine.ptr_now->frame[0].first)).Norm()*-1.f / fps;//�r䰃x�N�g��
													g.cap = dnm;
													break;
												}
											}
											if (!tt) {
												this->item_data.resize(this->item_data.size() + 1);
												auto& g = this->item_data.back();
												g.Set_item(mine.ptr_now, mine.pos_mag, mine.mat_mag, 1);
												g.add = (mine.obj.frame(mine.ptr_now->frame[1].first) - mine.obj.frame(mine.ptr_now->frame[0].first)).Norm()*-1.f / fps;//�r䰃x�N�g��
												g.cap = dnm;
											}
											//
											mine.reloadf = true;
										}
										//�Z���N�^�[
										if (mine.selkey.second == 1) {
											++mine.gun_stat[mine.gun_slot[this->sel_gun].ptr->id].select %= mine.ptr_now->select.size();
										}
									}
									//
									if (mine.reloadf && mine.gun_stat[mine.ptr_now->id].mag_in.size() >= 1) {
										mine.reload_cnt += 1.f / fps;
										if (settings->useVR_e) {
											if (mine.reload_cnt < mine.ptr_now->reload_time) {
												mine.down_mag = false;
											}
										}
									}
									//�Z�t�e�B
									easing_set(&mine.obj.get_anime(4).per, float(0.f), 0.5f, fps);
									//�ˌ�
									if (!mine.gunf && mine.ammo_cnt >= 1) {
										if (mine.ptr_now->select[mine.gun_stat[mine.gun_slot[this->sel_gun].ptr->id].select] == 2) {//�t���I�[�g�p
											mine.trigger.second = 0;
										}
									}
									mine.trigger.get_in(mine.obj.get_anime(2).per >= 0.5f);
									if (mine.trigger.second == 1) {
										mine.audio.trigger.play_3D(mine.pos + mine.pos_RIGHTHAND, 1.f);
									}
									switch (mine.ptr_now->cate) {
									case 0:
									{
										//�ߐ�
										if (mine.trigger.second == 1 && !mine.gunf) {
											mine.gunf = true;
											//�o�C�u���[�V�����@�o�b�e���[������������߃R�����g�A�E�g
											/*
												vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
											*/
											//�T�E���h
											mine.audio.shot.play_3D(mine.pos + mine.pos_RIGHTHAND, 1.f);
											mine.audio.slide.play_3D(mine.pos + mine.pos_RIGHTHAND, 1.f);
										}
										break;
									}
									case 1:
									{
										if (mine.trigger.second == 1 && !mine.gunf && mine.ammo_cnt >= 1) {
											mine.gunf = true;
											//�o�C�u���[�V�����@�o�b�e���[������������߃R�����g�A�E�g
											/*
												vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
											*/
											if (!settings->useVR_e && this->ads.first) {
												this->fov_fps *= 0.95f;
											}
											//�e���Ǘ�
											mine.ammo_cnt--;
											mine.gun_stat[mine.gun_slot[this->sel_gun].ptr->id].in--;
											if (!mine.reloadf && mine.gun_stat[mine.gun_slot[this->sel_gun].ptr->id].mag_in.size() >= 1 && mine.gun_stat[mine.gun_slot[this->sel_gun].ptr->id].mag_in.front() > 0) {
												mine.gun_stat[mine.gun_slot[this->sel_gun].ptr->id].mag_in.front()--;
											}
											//����������ƃR�R�����E�����
											mine.vecadd_RIGHTHAND_p = MATRIX_ref::Vtrans(mine.vecadd_RIGHTHAND_p,
												MATRIX_ref::RotY(deg2rad(float(int(mine.ptr_now->recoil_xdn*100.f) + GetRand(int((mine.ptr_now->recoil_xup - mine.ptr_now->recoil_xdn)*100.f))) / (100.f*(mine.LEFT_hand ? 3.f : 1.f))))*
												MATRIX_ref::RotX(deg2rad(float(int(mine.ptr_now->recoil_ydn*100.f) + GetRand(int((mine.ptr_now->recoil_yup - mine.ptr_now->recoil_ydn)*100.f))) / (100.f*(mine.LEFT_hand ? 3.f : 1.f)))));
											//�e
											mine.bullet[mine.use_bullet].set(&mine.ptr_now->ammo[0], mine.obj.frame(mine.ptr_now->frame[3].first), mine.mat_RIGHTHAND.zvec()*-1.f);
											//���
											mine.cart[mine.use_bullet].set(&mine.ptr_now->ammo[0], mine.obj.frame(mine.ptr_now->frame[2].first), (mine.obj.frame(mine.ptr_now->frame[2].first + 1) - mine.obj.frame(mine.ptr_now->frame[2].first)).Norm()*2.5f / fps, mine.mat_RIGHTHAND);
											//�G�t�F�N�g
											set_effect(&mine.effcs[ef_fire], mine.obj.frame(mine.ptr_now->frame[3].first), mine.mat_RIGHTHAND.zvec()*-1.f, 0.0025f / 0.1f);

											set_effect(&mine.effcs_gun[mine.use_effcsgun].effect, mine.obj.frame(mine.ptr_now->frame[3].first), mine.mat_RIGHTHAND.zvec()*-1.f, 0.11f / 0.1f);
											set_pos_effect(&mine.effcs_gun[mine.use_effcsgun].effect, Drawparts->get_effHandle(ef_smoke));
											mine.effcs_gun[mine.use_effcsgun].ptr = &mine.bullet[mine.use_bullet];
											mine.effcs_gun[mine.use_effcsgun].cnt = 0.f;
											++mine.use_effcsgun %= mine.effcs_gun.size();
											//�T�E���h
											mine.audio.shot.play_3D(mine.pos + mine.pos_RIGHTHAND, 1.f);
											mine.audio.slide.play_3D(mine.pos + mine.pos_RIGHTHAND, 1.f);
											//����ID��
											++mine.use_bullet %= mine.bullet.size();
										}
										//�}�K�W���擾
										if (mine.reloadf && mine.gun_stat[mine.ptr_now->id].mag_in.size() >= 1) {
											if (mine.down_mag) {
												if (settings->useVR_e) {
													auto p = MATRIX_ref::RotVec2(mine.mat_LEFTHAND.yvec(), (mine.obj.frame(mine.ptr_now->frame[0].first) - (mine.pos_LEFTHAND + mine.pos)));
													mine.mat_mag = mine.mag.GetFrameLocalMatrix(3)* (mine.mat_LEFTHAND*p);
												}
												else {
													mine.mat_mag = mine.mat_LEFTHAND;
												}
												mine.pos_mag = mine.pos_LEFTHAND + mine.pos;
												if ((settings->useVR_e) ? ((mine.mag.frame(3) - mine.obj.frame(mine.ptr_now->frame[0].first)).size() <= 0.05f) : (mine.reload_cnt > mine.ptr_now->reload_time)) {
													mine.obj.get_anime(1).time = 0.f;
													mine.obj.get_anime(0).per = 1.f;
													mine.obj.get_anime(1).per = 0.f;
													if (mine.ammo_cnt == 0) {
														mine.obj.get_anime(3).per = 1.f;
													}
													mine.audio.mag_set.play_3D(mine.pos + mine.pos_RIGHTHAND, 1.f);
													if (1) {
														mine.ammo_cnt += mine.gun_stat[mine.gun_slot[this->sel_gun].ptr->id].mag_in.front();
													}
													mine.reloadf = false;
												}
											}
										}
										else {
											mine.down_mag = false;
											mine.mat_mag = mine.mat_RIGHTHAND;
											mine.pos_mag = mine.obj.frame(mine.ptr_now->frame[1].first);
										}
										break;
									}
									}
								}
								mine.mag.SetMatrix(mine.mat_mag* MATRIX_ref::Mtrans(mine.pos_mag));
								mine.obj.work_anime();
								for (auto& a : mine.bullet) {
									if (a.flug) {
										a.repos = a.pos;
										a.pos += a.vec * (a.spec->speed / fps);
										//����
										{
											auto p = mapparts->map_col_line(a.repos, a.pos, 0);
											if (p.HitFlag == TRUE) {
												a.pos = p.HitPosition;
											}
											/*
											for (auto& tp : tgt_pic) {
												auto q = tp.obj.CollCheck_Line(a.repos, a.pos, 0, 1);
												if (q.HitFlag == TRUE) {
													a.pos = q.HitPosition;
													//
													tp.power = (tp.obj.frame(tgtparts->frame.first) - a.pos).y();
													tp.time = 0.f;
													int UI_xpos = 0;
													int UI_ypos = 0;
													//�e������
													{
														tp.pic.SetDraw_Screen(false);
														VECTOR_ref pvecp = (a.pos - tp.obj.frame(tgtparts->frame.first));
														VECTOR_ref xvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.frame_x.first));
														VECTOR_ref yvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.frame_y.first));
														UI_xpos = int(float(tgtparts->x_size)*(xvecp.Norm().dot(pvecp)) / xvecp.size());//X����
														UI_ypos = int(float(tgtparts->y_size)*(yvecp.Norm().dot(pvecp)) / yvecp.size());//Y����
														DrawCircle(UI_xpos, UI_ypos, 10, GetColor(255, 0, 0));//�e��
													}
													//
													set_effect(&mine.effcs[ef_reco], a.pos, q.Normal, 0.011f / 0.1f);
													//
													a.hit = true;
													a.flug = false;
													break;
												}
											}
											if (p.HitFlag == TRUE && a.flug) {
												a.flug = false;
												set_effect(&mine.effcs_gndhit[mine.use_effcsgndhit], a.pos, p.Normal, 0.025f / 0.1f);
												++mine.use_effcsgndhit %= mine.effcs_gndhit.size();
											}
											*/
										}
										//����(3�b�������A�X�s�[�h��0�ȉ��A�ђʂ�0�ȉ�)
										if (a.cnt >= 3.f || a.spec->speed < 0.f || a.spec->pene <= 0.f) {
											a.flug = false;
										}
										//�I��
										if (!a.flug) {
											for (auto& b : mine.effcs_gun) {
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
								//��䰂̏���
								for (auto& a : mine.cart) {
									a.get(mapparts, mine);
								}
								for (auto& t : mine.effcs) {
									if (t.id != ef_smoke) {
										set_pos_effect(&t, Drawparts->get_effHandle(int(t.id)));
									}
								}
								for (auto& t : mine.effcs_gndhit) {
									set_pos_effect(&t, Drawparts->get_gndhitHandle());
								}
								for (auto& a : mine.effcs_gun) {
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
							//�A�C�e���֘A
							{
								//�E��
								mine.canget_gunitem = false;
								mine.canget_magitem = false;
								for (auto& g : this->item_data) {
									g.Get_item(mine, item_data, mapparts, chgun, sel_gun, usegun, change_gun, gunpos_TPS);
								}
								//�e�𗎂Ƃ�
								if (this->delgun.second == 1) {
									//�A�C�e����
									{
										bool tt = true;
										for (auto& g : this->item_data) {
											if (g.ptr == nullptr && g.cate == 0) {
												tt = false;
												g.Set_item(mine.ptr_now, mine.pos + mine.pos_RIGHTHAND, mine.mat_RIGHTHAND, 0);
												break;
											}
										}
										if (tt) {
											this->item_data.resize(this->item_data.size() + 1);
											this->item_data.back().Set_item(mine.ptr_now, mine.pos + mine.pos_RIGHTHAND, mine.mat_RIGHTHAND, 0);
										}
									}
									//�e�����g�̃X���b�g�ɃZ�b�g���Ȃ���
									{
										for (size_t i = 0; i < mine.gun_slot.size(); i++) {
											if (this->sel_gun == i) {
												if ((this->sel_gun == mine.gun_slot.size() - 1) || mine.gun_slot[std::clamp(this->sel_gun + 1, 0, int(mine.gun_slot.size()) - 1)].ptr == nullptr) {
													mine.gun_slot[this->sel_gun].delete_gun();
													this->sel_gun--;
												}
												else {
													for (size_t j = i; j < mine.gun_slot.size() - 1; j++) {
														mine.gun_slot[j].delete_gun();
														mine.gun_slot[j].set(mine.gun_slot[j + 1].ptr);
													}
													mine.gun_slot.back().delete_gun();
												}
												break;
											}
										}
										mine.Delete_chara();
										mine.Set_chara(this->sel_gun);
										this->gunpos_TPS = VGet(0, 0, 0);
									}
								}
							}
						}
						//�ʐM
						{
							auto& cd = mine.senddata;
							cd.get_data(mine);//�ۑ�
							/*send cd*///���M
							/*get cd*///��M
							auto& td = mine.senddata;
							td.put_data(chara[1]);//���f
						}
						//�������Z�A�A�j���[�V����
						{
							for (auto& c_ : chara) {
								if (&c_ != &mine) {
									c_.body.work_anime();
								}
								if (c_.start_c) {
									c_.body.PhysicsResetState();
									c_.start_c = false;
								}
								else {
									c_.body.PhysicsCalculation(1000.f / fps);
								}
							}
						}
						//campos,camvec,camup�̎w��
						{
							this->campos_buf = mine.pos + mine.pos_HMD;
							this->camvec = mine.mat_HMD.zvec()*-1.f;
							this->camup = mine.mat_HMD.yvec();
						}
						Set3DSoundListenerPosAndFrontPosAndUpVec(this->campos_buf.get(), (this->campos_buf + this->camvec).get(), this->camup.get());
						UpdateEffekseer3D();
						//�e�p��
						Drawparts->Ready_Shadow(this->campos_buf,
							[&] {
							for (auto& mine : this->chara) { mine.Draw_chara(this->usegun.first, this->sel_gun); }
							for (auto& g : this->item_data) { g.Draw_item(); }
						}, VGet(5.f, 2.5f, 5.f));
						//VR��ԂɓK�p
						vrparts->Move_Player();
						//�X�R�[�v
						{
							if (mine.ptr_now->frame[4].first != INT_MAX) {
								VECTOR_ref cam = mine.obj.frame(mine.ptr_now->frame[4].first);
								VECTOR_ref vec = cam - mine.mat_RIGHTHAND.zvec();
								VECTOR_ref yvec = mine.mat_RIGHTHAND.yvec();

								// ���ɉf��f����`��
								for (auto& i : Drawparts->get_Mirror_obj()) {
									Drawparts->Mirror_SetupCamera(i, cam, vec, yvec, (this->fov / 7.5f) / 4.f, 100.f, 0.1f);	// ���ɉf��f����`�悷��ۂɎg�p����J�����̐ݒ���s��
									if (i.canlook) {
										Hostpassparts->dof(&i.Handle, mapparts->sky_draw(Drawparts->Mirrorcampos, Drawparts->Mirrorcamtgt, VGet(0, 1.f, 0), (this->fov / 7.5f) / 4.f), draw_by_shadow, Drawparts->Mirrorcampos, Drawparts->Mirrorcamtgt, VGet(0, 1.f, 0), this->fov_fps, 100.f, 1.0f, 0.1f);
									}
								}

								Hostpassparts->draw(&this->ScopeScreen, mapparts->sky_draw(cam, vec, yvec, (this->fov / 7.5f) / 4.f), draw_by_shadow_2, cam, vec, yvec, (this->fov / 7.5f) / 4.f, 100.f, 0.1f);
								mine.ptr_now->mod.lenzScreen.DrawExtendGraph(0, 0, 1080, 1080, true);
							}
						}
						//�`��
						if (settings->useVR_e) {
							UIparts->set_draw_vr(mine, this->usegun.first, this->sel_gun);
						}
						else {
							UIparts->set_draw_nomal(mine, this->usegun.first, this->sel_gun);
						}
						if (settings->useVR_e) {
							for (char eye = 0; eye < 2; eye++) {
								this->campos = this->campos_buf + vrparts->GetEyePosition_minVR(eye);

								// ���ɉf��f����`��
								for (auto& i : Drawparts->get_Mirror_obj()) {
									Drawparts->Mirror_SetupCamera(i, this->campos, this->campos + this->camvec, this->camup, this->fov, 100.f, 0.1f);	// ���ɉf��f����`�悷��ۂɎg�p����J�����̐ݒ���s��
									if (i.canlook) {
										Hostpassparts->dof(&i.Handle, mapparts->sky_draw(Drawparts->Mirrorcampos, Drawparts->Mirrorcamtgt, VGet(0, 1.f, 0), this->fov), draw_by_shadow, Drawparts->Mirrorcampos, Drawparts->Mirrorcamtgt, VGet(0, 1.f, 0), this->fov, 100.f, 1.0f, 0.1f);
									}
								}
								//��ʑ̐[�x�`��
								Hostpassparts->dof(&this->BufScreen, mapparts->sky_draw(this->campos, this->campos + this->camvec, this->camup, this->fov), draw_by_shadow_2, this->campos, this->campos + this->camvec, this->camup, this->fov, 100.f, 0.2f, 0.1f);
								//�`��
								this->outScreen[eye].SetDraw_Screen(0.1f, 100.f, this->fov, this->campos, this->campos + this->camvec, this->camup);
								{
									Hostpassparts->bloom(this->BufScreen, 64);//�u���[���t���`��
									UIparts->draw();//UI
									UIparts->item_draw(this->item_data, this->campos_buf);
								}
								//VR�Ɉڂ�
								GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
								{
									this->outScreen[eye].DrawGraph(0, 0, false);
									ID3D11Texture2D* ptr_DX11 = (ID3D11Texture2D*)GetUseDirect3D11BackBufferTexture2D();
									vrparts->PutEye(ptr_DX11, eye);
								}
							}
						}
						else {
							this->campos = this->campos_buf + MATRIX_ref::Vtrans(VGet(-0.035f, 0.f, 0.f), mine.mat_HMD);
							// ���ɉf��f����`��
							for (auto& i : Drawparts->get_Mirror_obj()) {
								Drawparts->Mirror_SetupCamera(i, this->campos, this->campos + this->camvec, this->camup, this->fov_fps, 100.f, 0.1f);	// ���ɉf��f����`�悷��ۂɎg�p����J�����̐ݒ���s��
								if (i.canlook) {
									Hostpassparts->dof(&i.Handle, mapparts->sky_draw(Drawparts->Mirrorcampos, Drawparts->Mirrorcamtgt, VGet(0, 1.f, 0), this->fov_fps), draw_by_shadow, Drawparts->Mirrorcampos, Drawparts->Mirrorcamtgt, VGet(0, 1.f, 0), this->fov_fps, 100.f, 1.0f, 0.1f);
								}
							}
							//��ʑ̐[�x�`��
							Hostpassparts->dof(&this->BufScreen, mapparts->sky_draw(this->campos, this->campos + this->camvec, this->camup, this->fov_fps), draw_by_shadow_2, this->campos, this->campos + this->camvec, this->camup, this->fov_fps, 100.f, 0.2f, 0.1f);
							//�`��
							this->outScreen[1].SetDraw_Screen(0.1f, 100.f, this->fov_fps, this->campos, this->campos + this->camvec, this->camup);
							{
								Hostpassparts->bloom(this->BufScreen, 64);//�u���[���t���`��
								UIparts->draw();//UI
								UIparts->item_draw(this->item_data, this->campos_buf);
							}
						}
						//�f�B�X�v���C�`��
						{
							this->TPS.get_in(CheckHitKey(KEY_INPUT_LCONTROL) != 0);
							VECTOR_ref cam = mine.pos + mine.pos_HMD + MATRIX_ref::Vtrans(this->campos_TPS, mine.mat_HMD);
							VECTOR_ref vec = mine.pos + mine.pos_HMD + MATRIX_ref::Vtrans(VGet(-0.35f, 0.125f, 0.f), mine.mat_HMD);
							if (this->TPS.first) {//TPS���_
								// ���ɉf��f����`��
								for (auto& i : Drawparts->get_Mirror_obj()) {
									Drawparts->Mirror_SetupCamera(i, cam, vec, VGet(0, 1.f, 0), this->fov, 100.f, 0.1f);	// ���ɉf��f����`�悷��ۂɎg�p����J�����̐ݒ���s��
									if (i.canlook) {
										Hostpassparts->draw(&i.Handle, mapparts->sky_draw(Drawparts->Mirrorcampos, Drawparts->Mirrorcamtgt, VGet(0, 1.f, 0), this->fov), draw_by_shadow, Drawparts->Mirrorcampos, Drawparts->Mirrorcamtgt, VGet(0, 1.f, 0), this->fov, 100.f, 0.1f);
									}
								}
								//��ʑ̐[�x�`��
								Hostpassparts->draw(&this->outScreen[2], mapparts->sky_draw(cam, vec, VGet(0, 1.f, 0), this->fov), draw_by_shadow_2, cam, vec, VGet(0, 1.f, 0), this->fov, 100.f, 0.1f);
								GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK, 0.1f, 100.f, this->fov, cam, vec, VGet(0, 1.f, 0));
								{
									this->outScreen[2].DrawExtendGraph(0, 0, settings->out_dispx, settings->out_dispy, false);
									//�X�R�[�v�̃G�C��
									if (mine.ptr_now->frame[4].first != INT_MAX) {
										this->ScopeScreen.DrawExtendGraph(settings->out_dispx - 200, 0, settings->out_dispx, 200, true);
									}
								}
							}
							else {//FPS���_
								GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
								this->outScreen[1].DrawExtendGraph(0, 0, settings->out_dispx, settings->out_dispy, false);
							}
							{
								//mine.gun_stat[mine.ptr_now->id]
							}
							//�f�o�b�O
							Debugparts->end_way();
							Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
						}
					}
					//��ʂ̔��f
					DXDraw::Screen_Flip();
					vrparts->Eye_Flip(waits, FRAME_RATE);
					//�I������
					if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
						this->ending = false;
						break;
					}
					if (CheckHitKey(KEY_INPUT_O) != 0) {
						break;
					}
				}
			}
			//���
			{
				for (auto& c : chara) {
					c.Delete_chara();
				}
				chara.clear();
				mapparts->Delete_map();
				Drawparts->Delete_Shadow();
			}
		} while (ProcessMessage() == 0 && this->ending);
	}
};