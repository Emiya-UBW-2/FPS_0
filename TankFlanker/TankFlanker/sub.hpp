#pragma once

#define NOMINMAX
#include <D3D11.h>
#include <array>
#include <fstream>
#include <memory>
#include <optional>
#include <vector>
#include<fstream>
#include "DXLib_ref/DXLib_ref.h"
void set_effect(EffectS* efh, VECTOR_ref pos, VECTOR_ref nor, float scale = 1.f) {
	efh->flug = true;
	efh->pos = pos;
	efh->nor = nor;
	efh->scale = scale;
}
void set_pos_effect(EffectS* efh, const EffekseerEffectHandle& handle) {
	if (efh->flug) {
		if (efh->handle.IsPlaying()) {
			efh->handle.Stop();
		}
		efh->handle = handle.Play3D();
		efh->handle.SetPos(efh->pos);
		efh->handle.SetRotation(atan2(efh->nor.y(), std::hypot(efh->nor.x(), efh->nor.z())), atan2(-efh->nor.x(), -efh->nor.z()), 0);
		efh->handle.SetScale(efh->scale);
		efh->flug = false;
	}
	//IsEffekseer3DEffectPlaying(player[0].effcs[i].handle)
}

typedef std::pair<int, VECTOR_ref> frames;

class switchs {
public:
	bool first;
	uint8_t second;

	switchs() {
		first = false;
		second = 0;
	};
	void get_in(bool key) {
		second = std::clamp<uint8_t>(second + 1, 0, (key ? 2 : 0));
		if (second == 1) {
			first ^= 1;
		}
	}
};

//
class Mainclass {
private:

public:
	//tgt
	class tgttmp {
	public:
		MV1 tgt;
		GraphHandle tgt_pic_tmp;
		int tgt_col;
		int tgt_pic_x;
		int tgt_pic_y;
		frames tgt_f;

		tgttmp() {
			MV1::Load("data/model/tgt/model.mv1", &this->tgt, true);
			this->tgt_col = LoadSoftImage("data/model/tgt/point.bmp");
			this->tgt_pic_tmp = GraphHandle::Load("data/model/tgt/Target-A2.png");
		}

		void set() {
			this->tgt_pic_tmp.GetSize(&this->tgt_pic_x, &this->tgt_pic_y);
			this->tgt_f = { 2,this->tgt.frame(2) };
		}
	};
	class tgts {
	public:
		MV1 obj;
		GraphHandle pic;
		int x_frame = 0, y_frame = 0;
		float rad = 0.f;
		float time = 0.f;
		float power = 0.f;
		bool LR = true,isMOVE=true;
		template<class Y, class D>
		void set(std::unique_ptr<Y, D>& tgtparts,const VECTOR_ref& pos) {
			obj = tgtparts->tgt.Duplicate();
			pic = GraphHandle::Make(tgtparts->tgt_pic_x, tgtparts->tgt_pic_y);
			pic.SetDraw_Screen(false);
			tgtparts->tgt_pic_tmp.DrawGraph(0, 0, true);

			obj.SetupCollInfo(8, 8, 8, 0, 1);
			obj.SetTextureGraphHandle(2, pic, false);
			x_frame = 4;
			y_frame = 3;
			LR = true;

			obj.SetPosition(pos);
		}
	};
	//score
	class scores {
	public:
		std::vector<int> score_s;
		int point = 0;
		int p_up = 0;
		int p_down = 0;

		bool c_ready = false;
		float c_readytimer = 4.f;
		bool c_start = false;
		bool c_end = false;
		float c_timer = 30.f;

		scores() {
			SetOutApplicationLogValidFlag(FALSE);  /*log*/
			int mdata = FileRead_open("data/score.txt", FALSE);
			while (FileRead_eof(mdata) == 0) {
				this->score_s.emplace_back(getparams::_int(mdata));
			}
			FileRead_close(mdata);
		}
		~scores() {
			std::ofstream outputfile("data/score.txt");
			for (auto& s : this->score_s) {
				outputfile << s << "\n";
			}
			outputfile.close();
		}
		void ins() {
			int i = 0;
			for (auto& c : this->score_s) {
				if (c < this->point) {
					break;
				}
				i++;
			}
			this->score_s.insert(this->score_s.begin() + i, this->point);
		}

		void reset() {
			this->point = 0;
			this->p_up = 0;
			this->p_down = 0;
			this->c_ready = false;
			this->c_readytimer = 4.f;
			this->c_start = false;
			this->c_end = false;
			this->c_timer = 30.f;
		}

		void sub(const int& pt) {
			this->p_down = pt;
			if (this->point + this->p_down >= 0) {
				this->point += this->p_down;
			}
		}

		void add(const int& pt) {
			this->p_up = pt;
			this->point += this->p_up;
		}

		void move_timer() {
			float fps = GetFPS();
			if (this->c_ready && !this->c_start) {
				this->c_readytimer -= 1.f / fps;
				if (this->c_readytimer <= 0.f) {
					this->c_start = true;
				}
			}
			if (this->c_start && !this->c_end) {
				this->c_timer -= 1.f / fps;
				if (this->c_timer <= 0.f) {
					this->c_timer = 0.f;
					this->c_end = true;//
					this->ins();//�f�[�^�x�[�X�Ƀf�[�^��}��
				}
			}
		}
	};
	//�e�p�I�[�f�B�I
	class Audios {
	public:
		SoundHandle shot;
		SoundHandle slide;
		SoundHandle trigger;
		SoundHandle mag_down;
		SoundHandle mag_set;
		SoundHandle case_down;

		void set(int mdata) {
			SetUseASyncLoadFlag(TRUE);
			SetCreate3DSoundFlag(TRUE);
			shot = SoundHandle::Load("data/audio/shot_" + getparams::_str(mdata) + ".wav");
			slide = SoundHandle::Load("data/audio/slide_" + getparams::_str(mdata) + ".wav");
			trigger = SoundHandle::Load("data/audio/trigger_" + getparams::_str(mdata) + ".wav");
			mag_down = SoundHandle::Load("data/audio/mag_down_" + getparams::_str(mdata) + ".wav");
			mag_set = SoundHandle::Load("data/audio/mag_set_" + getparams::_str(mdata) + ".wav");
			case_down = SoundHandle::Load("data/audio/case_" + getparams::_str(mdata) + ".wav");
			SetCreate3DSoundFlag(FALSE);
			SetUseASyncLoadFlag(FALSE);
		}
	};
	//�e�f�[�^
	class Ammos {
	public:
		std::string name;
		float caliber = 0.f;
		float speed = 100.f;//�e��
		float pene = 10.f;//�ђ�
		float damage = 10.f;//�_���[�W

		MV1 ammo;
		void set(void) {
			int mdata = FileRead_open(("data/ammo/" + this->name + "/data.txt").c_str(), FALSE);
			this->caliber = getparams::_float(mdata)*0.001f;//���a
			this->speed = getparams::_float(mdata);	//�e��
			this->pene = getparams::_float(mdata);	//�ђ�
			this->damage = getparams::_float(mdata);//�_���[�W
			FileRead_close(mdata);

			MV1::Load("data/ammo/" + this->name + "/ammo.mv1", &ammo, true);
		}
	};
	//�}�K�W���f�[�^
	class Mags {
	public:
		std::string name;
		int cap = 1;
		std::vector<Ammos> ammo;

		MV1 mag;
		void set(void) {
			if (this->name.find("none") == std::string::npos) {
				int mdata = FileRead_open(("data/mag/" + this->name + "/data.txt").c_str(), FALSE);
				this->cap = getparams::_int(mdata);//���a
				//�e�f�[�^
				while (true) {
					auto p = getparams::_str(mdata);
					if (getright(p.c_str()).find("end") != std::string::npos) {
						break;
					}
					else {
						this->ammo.resize(this->ammo.size() + 1);
						this->ammo.back().name = p;
					}
				}
				FileRead_close(mdata);
				MV1::Load("data/mag/" + this->name + "/mag.mv1", &mag, true);
			}
		}
	};
	//�e�f�[�^
	class Gun {
		class Models {
		public:
			GraphHandle UIScreen;
			GraphHandle lenzScreen;
			MV1 obj;
			std::string name;
			void set(std::string named) {
				name = named;
				MV1::Load("data/gun/" + named + "/model.mv1", &obj, true);
				lenzScreen = GraphHandle::Load("data/gun/" + named + "/lenz.png");
				UIScreen = GraphHandle::Load("data/gun/" + named + "/pic.bmp");
			}
		};
	public:
		uint8_t cate = 0;
		std::string name;
		std::vector<frames> frame;
		std::vector <uint8_t> select;//�Z���N�^�[
		Models mod;
		Audios audio;

		size_t ammo_max = 0;//�}�K�W���̑��e��
		float recoil_xup = 0.f;
		float recoil_xdn = 0.f;
		float recoil_yup = 0.f;
		float recoil_ydn = 0.f;

		float ammo_speed = 100.f;//�e��
		float ammo_pene = 10.f;//�ђ�
		float ammo_damege = 10.f;//�_���[�W

		std::vector<Ammos> ammo;
		Mags mag;

		void set_data() {
			this->name = this->mod.name;
			this->mod.obj.SetMatrix(MGetIdent());
			{
				//�t���[��
				this->frame.resize(8);
				this->frame[0].first = INT_MAX;
				this->frame[1].first = INT_MAX;
				this->frame[2].first = INT_MAX;
				this->frame[3].first = INT_MAX;
				this->frame[4].first = INT_MAX;
				this->frame[5].first = INT_MAX;
				this->frame[6].first = INT_MAX;
				this->frame[7].first = INT_MAX;
				for (int i = 0; i < this->mod.obj.frame_num(); i++) {
					std::string s = this->mod.obj.frame_name(i);
					if (s.find("mag_fall") != std::string::npos) {
						this->frame[0].first = i;//�r�
						this->frame[0].second = this->mod.obj.frame(i);
						this->frame[1].first = i + 1;
						this->frame[1].second = this->mod.obj.frame(i + 1);
					}
					else if (s.find("case") != std::string::npos) {
						this->frame[2].first = i;//�r�
					}
					else if (s.find("mazzule") != std::string::npos) {
						this->frame[3].first = i;//�}�Y��
					}
					else if (s.find("scope") != std::string::npos) {
						this->frame[4].first = i;//�X�R�[�v
						this->frame[4].second = this->mod.obj.frame(i);
					}
					else if (s.find("trigger") != std::string::npos) {
						this->frame[5].first = i + 1;//�g���K�[
					}
					else if (s.find("LEFT") != std::string::npos) {
						this->frame[6].first = i;//����
					}
					else if (s.find("site") != std::string::npos) {
						this->frame[7].first = i;//�A�C�A���T�C�g
						this->frame[7].second = this->mod.obj.frame(i);
					}
				}
				//�e�L�X�g
				{
					int mdata = FileRead_open(("data/gun/" + this->name + "/data.txt").c_str(), FALSE);
					//�J�e�S��
					{
						auto p = getparams::_str(mdata);
						if (getright(p.c_str()).find("knife") != std::string::npos) {
							this->cate = 0;
						}
						else if (getright(p.c_str()).find("gun") != std::string::npos) {
							this->cate = 1;
						}
					}
					//�Z���N�^�[�ݒ�
					while (true) {
						auto p = getparams::_str(mdata);
						if (getright(p.c_str()).find("end") != std::string::npos) {
							break;
						}
						else if (getright(p.c_str()).find("semi") != std::string::npos) {
							this->select.emplace_back(uint8_t(1));					//�Z�~�I�[�g=1
						}
						else if (getright(p.c_str()).find("full") != std::string::npos) {
							this->select.emplace_back(uint8_t(2));					//�t���I�[�g=2
						}
						else if (getright(p.c_str()).find("3b") != std::string::npos) {
							this->select.emplace_back(uint8_t(3));					//3�A�o�[�X�g=3
						}
						else if (getright(p.c_str()).find("2b") != std::string::npos) {
							this->select.emplace_back(uint8_t(4));					//2�A�o�[�X�g=4
						}
						else {
							this->select.emplace_back(uint8_t(1));
						}
					}
					//�T�E���h
					this->audio.set(mdata);
					//���e��
					this->ammo_max = getparams::_long(mdata);
					//�e�f�[�^
					while (true) {
						auto p = getparams::_str(mdata);
						if (getright(p.c_str()).find("end") != std::string::npos) {
							break;
						}
						else {
							this->ammo.resize(this->ammo.size()+1);
							this->ammo.back().name = p;
						}
					}
					//�}�K�W��
					this->mag.name = getparams::_str(mdata);
					//����
					this->recoil_xup = getparams::_float(mdata);
					this->recoil_xdn = getparams::_float(mdata);
					this->recoil_yup = getparams::_float(mdata);
					this->recoil_ydn = getparams::_float(mdata);
					FileRead_close(mdata);
					//
					for (auto& a : this->ammo) {
						a.set();
					}
					this->mag.set();
				}
			}
		}
	};
	//�A�C�e��
	class Gun_item {
	public:
		VECTOR_ref pos, add;
		MATRIX_ref mat;
		MV1 obj;
		Gun* gunptr = nullptr;

		void set(Gun*gundata, const VECTOR_ref& pos_) {
			this->pos = pos_;
			this->add = VGet(0, 0, 0);
			this->mat = MGetIdent();
			//��
			this->gunptr = gundata;
			this->obj = this->gunptr->mod.obj.Duplicate();
		}
		void draw() {
			if (this->gunptr != nullptr) {
				this->obj.DrawModel();
			}
		}
		void delete_chara() {
			this->gunptr = nullptr;
			this->obj.Dispose();
		}
	};
	//���ۂɔ��˂����e
	struct ammos {
		bool hit{ false };
		bool flug{ false };
		float cnt = 0.f;
		unsigned int color = 0;
		Ammos* spec;
		float yadd = 0.f;
		VECTOR_ref pos, repos, vec;
	};
	//player
	class Chara {
		struct ef_guns {
			EffectS first;
			ammos* second = nullptr;
			float cnt = -1.f;
		};
		struct ammo_obj {
			MV1 second;
			float cnt = -1.f;
			bool down = false;
			VECTOR_ref pos;
			VECTOR_ref add;
			MATRIX_ref mat;
		};
		struct mag_obj {
			MV1 second;
			float cnt = -1.f;
			bool down = false;
			VECTOR_ref pos;
			VECTOR_ref add;
			MATRIX_ref mat;
		};
	public:
		std::array<ef_guns, 60> effcs_gun;    /*effect*/
		size_t gun_effcnt = 0;
		std::array<ammos, 64> bullet;	      /*�m�ۂ���e*/
		size_t usebullet = 0;		      /*�g�p�e*/

		std::array<EffectS, effects> effcs; /*effect*/
		std::array<EffectS, 12> gndsmkeffcs; /*effect*/
		size_t gndsmk_use = 0;
		Gun* gunptr=nullptr;

		Gun* gunptr_backup;
		std::array<Gun*, 3> gunptr_have;
		Audios audio;
		MV1 obj, mag,hand;
		std::array<ammo_obj, 64> ammo;		/*�m�ۂ���e*/
		std::array<mag_obj, 8> magazine;	/*�m�ۂ���e*/
		size_t usemag = 0;		      /*�g�p�e*/

		VECTOR_ref pos;
		MATRIX_ref mat;
		switchs safety;
		uint8_t trigger=0;

		uint8_t selkey=0;
		uint8_t select = 0;//�Z���N�^�[
		size_t ammoc = 0;//���e���J�E���g

		bool gunf=false;
		uint8_t guncnt = 0;

		bool LEFT_hand=false;


		bool reloadf=false;
		VECTOR_ref pos_mag;
		VECTOR_ref add_mag;
		MATRIX_ref mat_mag;
		bool down_mag=false;

		VECTOR_ref pos_HMD;
		MATRIX_ref mat_HMD;
		float add_ypos = 0.f;
		float body_yrad = 0.f;

		VECTOR_ref pos_LHAND;
		MATRIX_ref mat_LHAND;
		VECTOR_ref vecadd_LHAND;
		VECTOR_ref vecadd_LHAND_p;

		VECTOR_ref pos_RHAND;
		MATRIX_ref mat_RHAND;

		std::vector<frames> frame_hand;

		bool canget;
		std::string canget_gun;
		void set_list(Gun*gundata, Gun*gundata_backup) {
			this->gunptr_backup = gundata_backup;
			this->gunptr_have[0] = gundata;
			this->gunptr_have[1] = nullptr;
			this->gunptr_have[2] = nullptr;
		}
		void set_chara(const VECTOR_ref& pos_, int gunid, const GraphHandle& scope,MV1& hand_) {
			this->pos = pos_;
			this->mat = MGetIdent();
			//��
			this->hand = hand_.Duplicate();// .Duplicate();
			/*
			hand.DuplicateonAnime(&this->hand);
			this->frame_hand.resize(3);
			for (int i = 0; i < this->hand.frame_num(); i++) {
				std::string s = this->hand.frame_name(i);
				if (s.find("RIGHT") != std::string::npos) {
					this->frame_hand[0].first = i + 1;
					this->frame_hand[0].second = MATRIX_ref::Vtrans(VGet(0, 0, 0), this->hand.GetFrameLocalMatrix(this->frame_hand[0].first));//mag

					this->frame_hand[1].first = i + 2;
					this->frame_hand[1].second = MATRIX_ref::Vtrans(VGet(0, 0, 0), this->hand.GetFrameLocalMatrix(this->frame_hand[1].first));//mag

					this->frame_hand[2].first = i + 3;
					this->frame_hand[2].second = MATRIX_ref::Vtrans(VGet(0, 0, 0), this->hand.GetFrameLocalMatrix(this->frame_hand[2].first));//mag
				}
				else if (s.find("LEFT") != std::string::npos) {
				}
			}
			*/
			if (gunid >= 0) {
				this->gunptr = gunptr_have[gunid];
			}
			else {
				this->gunptr = gunptr_backup;
			}
			this->gunptr->mod.obj.DuplicateonAnime(&this->obj);
			if (this->gunptr->frame[4].first != INT_MAX) {
				this->obj.SetTextureGraphHandle(1, scope, false);	//�X�R�[�v
			}
			this->mag = this->gunptr->mag.mag.Duplicate();
			for (auto& a : this->ammo) {
				a.cnt = -1.f;
				a.pos = VGet(0, 0, 0);
				a.mat = MGetIdent();
			}

			for (auto& a : this->magazine) {
				a.cnt = -1.f;
				a.pos = VGet(0, 0, 0);
				a.mat = MGetIdent();
			}
			this->LEFT_hand = false;

			this->guncnt = 0;
			this->ammoc = this->gunptr->ammo_max + 1;
			this->gunf = false;
			this->vecadd_LHAND = VGet(0, 0, 1.f);
			this->vecadd_LHAND_p = this->vecadd_LHAND;
			this->reloadf = false;
			this->down_mag = true;
			this->select = 0;
			this->selkey = 0;
			for (auto& a : this->obj.get_anime()) {
				a.per = 0.f;
			}
			fill_id(this->effcs);			      //�G�t�F�N�g
			for (auto& a : this->bullet) {
				a.flug = false;
				a.color = GetColor(255, 255, 172);
			}
			SetCreate3DSoundFlag(TRUE);
			this->audio.shot = this->gunptr->audio.shot.Duplicate();
			this->audio.slide = this->gunptr->audio.slide.Duplicate();
			this->audio.trigger = this->gunptr->audio.trigger.Duplicate();
			this->audio.mag_down = this->gunptr->audio.mag_down.Duplicate();
			this->audio.mag_set = this->gunptr->audio.mag_set.Duplicate();
			this->audio.case_down = this->gunptr->audio.case_down.Duplicate();
			SetCreate3DSoundFlag(FALSE);
		}

		void draw() {
			this->hand.DrawModel();
			if (this->gunptr->cate == 1) {
				if (!this->reloadf || this->down_mag) {
					this->mag.DrawModel();
				}
				for (auto& a : this->magazine) {
					if (a.cnt < 0.f) { continue; }
					a.second.DrawModel();
				}
			}

			this->obj.DrawModel();
			if (this->gunptr->cate == 1) {
				for (auto& a : this->ammo) {
					if (a.cnt < 0.f) { continue; }
					a.second.DrawModel();
				}
			}
		}
		void draw_ammo() {
			if (this->gunptr->cate == 1) {
				for (auto& a : this->bullet) {
					if (!a.flug) { continue; }
					DXDraw::Capsule3D(a.pos, a.repos, ((a.spec->caliber - 0.00762f) * 0.1f + 0.00762f), a.color, GetColor(255, 255, 255));
				}
			}
		}
		void delete_chara() {
			this->gunptr = nullptr;
			this->obj.Dispose();
			this->mag.Dispose();
			for (auto& a : this->ammo) {
				a.second.Dispose();
			}
			for (auto& a : this->magazine) {
				a.second.Dispose();
			}
			this->audio.shot.Dispose();
			this->audio.slide.Dispose();
			this->audio.trigger.Dispose();
			this->audio.mag_down.Dispose();
			this->audio.mag_set.Dispose();
			this->audio.case_down.Dispose();
			for (auto& t : this->effcs_gun) {
				t.first.handle.Dispose();
			}
			for (auto& t : this->effcs) {
				t.handle.Dispose();
			}
		}
		void init() {
			delete_chara();
			std::fill(gunptr_have.begin(), gunptr_have.end(), nullptr);
		}
	};
};
//