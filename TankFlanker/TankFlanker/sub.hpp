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
		bool LR = true;
		template<class Y, class D>
		void set(std::unique_ptr<Y, D>& tgtparts) {
			obj = tgtparts->tgt.Duplicate();
			pic = GraphHandle::Make(tgtparts->tgt_pic_x, tgtparts->tgt_pic_y);
			pic.SetDraw_Screen(false);
			tgtparts->tgt_pic_tmp.DrawGraph(0, 0, true);

			obj.SetupCollInfo(8, 8, 8, 0, 1);
			obj.SetTextureGraphHandle(2, pic, false);
			x_frame = 4;
			y_frame = 3;
			LR = true;
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
					this->ins();//データベースにデータを挿入
				}
			}
		}
	};
	//gun
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
	class Gun {
		class models {
		public:
			GraphHandle lenzScreen;
			MV1 obj, mag, ammo;
			void set(std::string name) {
				MV1::Load("data/gun/" + name + "/model.mv1", &obj, true);
				MV1::Load("data/gun/" + name + "/mag.mv1", &mag, true);
				MV1::Load("data/gun/" + name + "/ammo.mv1", &ammo, true);
				lenzScreen = GraphHandle::Load("data/gun/" + name + "/lenz.png");
			}
		};
	public:
		uint8_t cate = 0;
		std::string name;
		std::vector<frames> frame;
		size_t ammo_max=0;//マガジンの装弾数
		std::vector <uint8_t> select;//セレクター
		models mod;
		Audios audio;

		void set_data() {
			//フレーム
			this->frame.resize(8);
			this->frame[0].first = INT_MAX;
			this->frame[1].first = INT_MAX;
			this->frame[2].first = INT_MAX;
			this->frame[3].first = INT_MAX;
			this->frame[4].first = INT_MAX;
			this->frame[5].first = INT_MAX;
			this->frame[6].first = INT_MAX;
			this->frame[7].first = INT_MAX;

			this->mod.obj.SetMatrix(MGetIdent());
			{
				for (int i = 0; i < this->mod.obj.frame_num(); i++) {
					std::string s = this->mod.obj.frame_name(i);
					if (s.find("mag_fall") != std::string::npos) {
						this->frame[0].first = i;//排莢
						this->frame[0].second = MATRIX_ref::Vtrans(VGet(0, 0, 0), this->mod.obj.GetFrameLocalMatrix(this->frame[0].first));//mag
						this->frame[1].first = i + 1;
						this->frame[1].second = MATRIX_ref::Vtrans(VGet(0, 0, 0), this->mod.obj.GetFrameLocalMatrix(this->frame[1].first));//mag先
					}
					else if (s.find("case") != std::string::npos) {
						this->frame[2].first = i;//排莢
					}
					else if (s.find("mazzule") != std::string::npos) {
						this->frame[3].first = i;//マズル
					}
					else if (s.find("scope") != std::string::npos) {
						this->frame[4].first = i;//スコープ
						this->frame[4].second = MATRIX_ref::Vtrans(VGet(0, 0, 0), this->mod.obj.GetFrameLocalMatrix(this->frame[4].first));
					}
					else if (s.find("trigger") != std::string::npos) {
						this->frame[5].first = i + 1;//トリガー
					}
					else if (s.find("LEFT") != std::string::npos) {
						this->frame[6].first = i;//左手
					}
					else if (s.find("site") != std::string::npos) {
						this->frame[7].first = i;//アイアンサイト
						this->frame[7].second = this->mod.obj.frame(i);
					}
				}
				//テキスト
				{
					int mdata = FileRead_open(("data/gun/" + this->name + "/data.txt").c_str(), FALSE);
					//装弾数
					this->ammo_max = getparams::_long(mdata);
					{
						auto p = getparams::_str(mdata);
						if (getright(p.c_str()).find("knife") != std::string::npos) {
							this->cate = 0;
						}
						else if (getright(p.c_str()).find("gun") != std::string::npos) {
							this->cate = 1;
						}
					}
					//セレクター設定
					while (true) {
						auto p = getparams::_str(mdata);
						if (getright(p.c_str()).find("end") != std::string::npos) {
							break;
						}
						else if (getright(p.c_str()).find("semi") != std::string::npos) {
							this->select.emplace_back(uint8_t(1));					//セミオート=1
						}
						else if (getright(p.c_str()).find("full") != std::string::npos) {
							this->select.emplace_back(uint8_t(2));					//フルオート=2
						}
						else if (getright(p.c_str()).find("3b") != std::string::npos) {
							this->select.emplace_back(uint8_t(3));					//3連バースト=3
						}
						else if (getright(p.c_str()).find("2b") != std::string::npos) {
							this->select.emplace_back(uint8_t(4));					//2連バースト=4
						}
						else {
							this->select.emplace_back(uint8_t(1));
						}
					}
					//サウンド
					this->audio.set(mdata);
					FileRead_close(mdata);
				}
			}
		}
	};
	//player
		//弾薬
	struct Ammos {
		float caliber_a = 0.f;
		float pene_a = 0.f;
		float speed_a = 0.f;
		uint16_t damage_a = 0;
	};
	struct ammos {
		bool hit{ false };
		bool flug{ false };
		float cnt = 0.f;
		unsigned int color = 0;
		Ammos spec;
		float yadd = 0.f;
		VECTOR_ref pos, repos, vec;
	};
	struct ef_guns {
		EffectS first;
		ammos* second = nullptr;
		float cnt = -1.f;
	};

	struct ammo_obj {
		EffectS first;
		MV1 second;
		float cnt = -1.f;
		bool down = false;
		VECTOR_ref pos;
		VECTOR_ref add;
		MATRIX_ref mat;
	};
	class Gun_item {
	public:
		VECTOR_ref pos, add;
		MATRIX_ref mat;
		MV1 obj, mag;
		Gun* gunptr = nullptr;

		void set_chara(const VECTOR_ref& pos_, Gun*gundata) {
			this->pos = pos_;
			this->add = VGet(0, 0, 0);
			this->mat = MGetIdent();
			//手
			this->gunptr = gundata;
			this->obj = this->gunptr->mod.obj.Duplicate();
			this->mag = this->gunptr->mod.mag.Duplicate();
		}

		void delete_chara() {
			this->gunptr = nullptr;
			this->obj.Dispose();
			this->mag.Dispose();
		}
	};

	class Chara {
	public:
		std::array<ef_guns, 60> effcs_gun;    /*effect*/
		size_t gun_effcnt = 0;
		std::array<ammos, 64> bullet;	      /*確保する弾*/
		size_t usebullet{};		      /*使用弾*/

		std::array<EffectS, effects> effcs; /*effect*/
		std::array<EffectS, 12> gndsmkeffcs; /*effect*/
		size_t gndsmk_use = 0;
		Gun* gunptr=nullptr;

		std::array<Gun*, 3> gunptr_have;
		Audios audio;
		MV1 obj, mag,hand;
		std::array<ammo_obj, 64> ammo;	      /*確保する弾*/
		VECTOR_ref pos;
		MATRIX_ref mat;
		switchs safety;
		uint8_t trigger=0;

		uint8_t selkey=0;
		uint8_t select = 0;//セレクター
		size_t ammoc = 0;//装弾数カウント

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
		void set_list(Gun*gundata) {
			this->gunptr_have[0] = gundata;
			this->gunptr_have[1] = nullptr;
			this->gunptr_have[2] = nullptr;
		}
		void set_chara(const VECTOR_ref& pos_, int gunid, const GraphHandle& scope,MV1& hand_) {
			this->pos = pos_;
			this->mat = MGetIdent();
			//手
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
			this->gunptr = gunptr_have[gunid];

			this->gunptr->mod.obj.DuplicateonAnime(&this->obj);
			if (this->gunptr->frame[4].first != INT_MAX) {
				this->obj.SetTextureGraphHandle(1, scope, false);	//スコープ
			}
			this->mag = this->gunptr->mod.mag.Duplicate();
			for (auto& a : this->ammo) {
				a.second = this->gunptr->mod.ammo.Duplicate();
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
			fill_id(this->effcs);			      //エフェクト
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

		void delete_chara() {
			this->gunptr = nullptr;
			this->obj.Dispose();
			this->mag.Dispose();
			for (auto& a : this->ammo) {
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