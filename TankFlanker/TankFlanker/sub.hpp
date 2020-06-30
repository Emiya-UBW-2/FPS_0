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
//
class Mainclass {
private:

public:
	//tgt
	class tgttmp {
	public:
		MV1 obj;
		GraphHandle tex;
		int col_tex = 0;
		int x_size = 0;
		int y_size = 0;
		frames frame;
		tgttmp() {
			MV1::Load("data/model/tgt/model.mv1", &this->obj, true);
			this->col_tex = LoadSoftImage("data/model/tgt/point.bmp");
			this->tex = GraphHandle::Load("data/model/tgt/Target-A2.png");
		}
		void set() {
			this->tex.GetSize(&this->x_size, &this->y_size);
			this->frame = { 2,this->obj.frame(2) };
		}
	};
	class tgts {
	public:
		int id;
		MV1 obj;
		GraphHandle pic;
		frames frame_x, frame_y;
		float rad = 0.f;
		float time = 0.f;
		float power = 0.f;
		bool LR = true, isMOVE = true;
		template<class Y, class D>
		void set(std::unique_ptr<Y, D>& tgtparts, const VECTOR_ref& pos) {
			obj = tgtparts->obj.Duplicate();
			pic = GraphHandle::Make(tgtparts->x_size, tgtparts->y_size);
			pic.SetDraw_Screen(false);
			tgtparts->tex.DrawGraph(0, 0, true);

			obj.SetupCollInfo(8, 8, 8, 0, 1);
			obj.SetTextureGraphHandle(2, pic, false);
			frame_x.first = 4;
			frame_y.first = 3;
			LR = true;
			isMOVE = true;
			obj.SetPosition(pos);
		}
	};
	//score
	class scores {
	public:
		std::vector<int> stack;

		int point = 0;
		int pointup = 0;
		int pointdown = 0;
		bool ready_f = false;
		bool start_f = false;
		bool end_f = false;
		float readytimer = 4.f;
		float timer = 30.f;

		scores() {
			SetOutApplicationLogValidFlag(FALSE);  /*log*/
			int mdata = FileRead_open("data/score.txt", FALSE);
			while (FileRead_eof(mdata) == 0) {
				this->stack.emplace_back(getparams::_int(mdata));
			}
			FileRead_close(mdata);
		}
		~scores() {
			std::ofstream outputfile("data/score.txt");
			for (auto& s : this->stack) {
				outputfile << s << "\n";
			}
			outputfile.close();
		}
		void insert() {
			int i = 0;
			for (auto& c : this->stack) {
				if (c < this->point) {
					break;
				}
				i++;
			}
			this->stack.insert(this->stack.begin() + i, this->point);
		}

		void reset() {
			this->point = 0;
			this->pointup = 0;
			this->pointdown = 0;
			this->ready_f = false;
			this->readytimer = 4.f;
			this->start_f = false;
			this->end_f = false;
			this->timer = 30.f;
		}

		void sub(const int& pt) {
			this->pointdown = pt;
			if (this->point + this->pointdown >= 0) {
				this->point += this->pointdown;
			}
		}

		void add(const int& pt) {
			this->pointup = pt;
			this->point += this->pointup;
		}

		void move_timer(void) {
			float fps = GetFPS();
			if (this->ready_f && !this->start_f) {
				this->readytimer -= 1.f / fps;
				if (this->readytimer <= 0.f) {
					this->start_f = true;
				}
			}
			if (this->start_f && !this->end_f) {
				this->timer -= 1.f / fps;
				if (this->timer <= 0.f) {
					this->timer = 0.f;
					this->end_f = true;
					this->insert();
				}
			}
		}
	};
	//銃用オーディオ
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
	//弾データ
	class Ammos {
	public:
		std::string name;
		float caliber = 0.f;
		float speed = 100.f;//弾速
		float pene = 10.f;//貫通
		float damage = 10.f;//ダメージ

		MV1 ammo;
		void set(void) {
			int mdata = FileRead_open(("data/ammo/" + this->name + "/data.txt").c_str(), FALSE);
			this->caliber = getparams::_float(mdata)*0.001f;//口径
			this->speed = getparams::_float(mdata);	//弾速
			this->pene = getparams::_float(mdata);	//貫通
			this->damage = getparams::_float(mdata);//ダメージ
			FileRead_close(mdata);

			MV1::Load("data/ammo/" + this->name + "/ammo.mv1", &ammo, true);
		}
	};
	//マガジンデータ
	class Mags {
	public:
		std::string name;
		int cap = 1;
		std::vector<Ammos> ammo;

		MV1 mag;
		void set(void) {
			if (this->name.find("none") == std::string::npos) {
				int mdata = FileRead_open(("data/mag/" + this->name + "/data.txt").c_str(), FALSE);
				this->cap = getparams::_int(mdata);//口径
				//弾データ
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
	//銃データ
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
		int id = 0;
		uint8_t cate = 0;
		std::string name;
		std::vector<frames> frame;
		std::vector <uint8_t> select;//セレクター
		Models mod;
		Audios audio;

		size_t ammo_max = 0;//マガジンの装弾数
		float recoil_xup = 0.f;
		float recoil_xdn = 0.f;
		float recoil_yup = 0.f;
		float recoil_ydn = 0.f;

		float reload_time = 1.f;//再装填時間

		float ammo_speed = 100.f;//弾速
		float ammo_pene = 10.f;//貫通
		float ammo_damege = 10.f;//ダメージ

		std::vector<Ammos> ammo;
		Mags mag;

		void set_data() {
			this->name = this->mod.name;
			this->mod.obj.SetMatrix(MGetIdent());
			{
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
				for (int i = 0; i < this->mod.obj.frame_num(); i++) {
					std::string s = this->mod.obj.frame_name(i);
					if (s.find("mag_fall") != std::string::npos) {
						this->frame[0].first = i;//マガジン
						this->frame[0].second = this->mod.obj.frame(i);
						this->frame[1].first = i + 1;
						this->frame[1].second = this->mod.obj.frame(i + 1);
					}
					else if (s.find("case") != std::string::npos) {
						this->frame[2].first = i;//排莢
					}
					else if (s.find("mazzule") != std::string::npos) {
						this->frame[3].first = i;//マズル
					}
					else if (s.find("scope") != std::string::npos) {
						this->frame[4].first = i;//スコープ
						this->frame[4].second = this->mod.obj.frame(i);
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
					//カテゴリ
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
					//装弾数
					this->ammo_max = getparams::_long(mdata);
					//弾データ
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
					//マガジン
					this->mag.name = getparams::_str(mdata);
					//反動
					this->recoil_xup = getparams::_float(mdata);
					this->recoil_xdn = getparams::_float(mdata);
					this->recoil_yup = getparams::_float(mdata);
					this->recoil_ydn = getparams::_float(mdata);
					//リロードタイム
					this->reload_time = getparams::_float(mdata);
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
	//アイテム
	class Gun_item {
	public:
		VECTOR_ref pos, add;
		MATRIX_ref mat;
		MV1 obj;
		Gun* ptr = nullptr;

		void set(Gun*gundata, const VECTOR_ref& pos_, const MATRIX_ref& mat_) {
			this->pos = pos_;
			this->add = VGet(0, 0, 0);
			this->mat = mat_;
			//手
			this->ptr = gundata;
			this->obj = this->ptr->mod.obj.Duplicate();
		}
		void draw() {
			if (this->ptr != nullptr) {
				this->obj.DrawModel();
			}
		}
		void draw(const bool& canget, std::string& name) {
			if (this->ptr != nullptr) {
				if (canget && name == this->ptr->name) {
					DrawLine3D(this->pos.get(), (this->pos + VGet(0, 0.1f, 0)).get(), GetColor(255, 0, 0));
					auto c = MV1GetDifColorScale(this->obj.get());
					MV1SetDifColorScale(this->obj.get(), GetColorF(0.f, 1.f, 0.f, 1.f));
					this->obj.DrawModel();
					MV1SetDifColorScale(this->obj.get(), c);
				}
				else {
					this->obj.DrawModel();
				}
			}
		}
		void delete_item() {
			this->ptr = nullptr;
			this->obj.Dispose();
		}
	};
	class Mag_item {
	public:
		VECTOR_ref pos, add;
		MATRIX_ref mat;
		MV1 obj;
		Gun* ptr = nullptr;

		size_t cap = 0;

		void set(Gun*gundata, const VECTOR_ref& pos_,const MATRIX_ref& mat_) {
			if (gundata->mag.name.find("none") == std::string::npos) {
				this->pos = pos_;
				this->add = VGet(0, 0, 0);
				this->mat = mat_;
				//手
				this->ptr = gundata;
				this->obj = this->ptr->mag.mag.Duplicate();
			}
			else {
				this->delete_item();
			}
		}
		void draw() {
			if (this->ptr != nullptr) {
				this->obj.DrawModel();
			}
		}
		void draw(const bool& canget, std::string& name) {
			if (this->ptr != nullptr) {
				if (canget && name == this->ptr->name) {
					DrawLine3D(this->pos.get(), (this->pos + VGet(0, 0.1f, 0)).get(), GetColor(255, 0, 0));
					auto c = MV1GetDifColorScale(this->obj.get());
					MV1SetDifColorScale(this->obj.get(), GetColorF(0.f, 1.f, 0.f, 1.f));
					this->obj.DrawModel();
					MV1SetDifColorScale(this->obj.get(), c);
				}
				else {
					this->obj.DrawModel();
				}
			}
		}
		void delete_item() {
			this->ptr = nullptr;
			this->obj.Dispose();
		}
	};
	//実際に発射される弾
	class ammos {
	public:
		bool hit{ false };
		bool flug{ false };
		float cnt = 0.f;
		unsigned int color = 0;
		Ammos* spec = nullptr;
		float yadd = 0.f;
		VECTOR_ref pos, repos, vec;

		void set(Ammos* spec_t, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t) {
			this->spec = spec_t;
			this->pos = pos_t;
			this->vec = vec_t;
			this->hit = false;
			this->flug = true;
			this->cnt = 0.f;
			this->yadd = 0.f;
			this->repos = this->pos;
		}
	};
	//player
	class Chara {
		struct ef_guns {
			EffectS effect;
			ammos* ptr = nullptr;
			float cnt = -1.f;
		};
		class ammo_obj {
		public:
			MV1 second;
			float cnt = -1.f;
			bool down = false;
			VECTOR_ref pos;
			VECTOR_ref add;
			MATRIX_ref mat;
			void set(Ammos* spec_t, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t, const MATRIX_ref& mat_t) {
				this->second.Dispose();
				this->second = spec_t->ammo.Duplicate();
				this->cnt = 0.f;
				this->pos = pos_t;//排莢
				this->add = vec_t;//排莢ベクトル
				this->mat = mat_t;
			}
		};
		struct gun_state {
			size_t in = 0;				//所持弾数
			std::vector<size_t> mag_in;	//マガジン内
			uint8_t select = 0;			//セレクター
		};
		class gun_have {
		public:
			int id = 0;
			Gun* ptr = nullptr;
			MV1 obj;
			void set(Gun* ptr_) {
				this->ptr = ptr_;
				if (this->ptr != nullptr) {
					this->obj = this->ptr->mod.obj.Duplicate();
				}
			}
			void delete_gun() {
				this->ptr = nullptr;
				this->obj.Dispose();
			}
		};
	public:
		/*エフェクト*/
		std::array<EffectS, effects> effcs;
		std::array<ef_guns, 60> effcs_gun;
		size_t use_effcsgun = 0;
		std::array<EffectS, 12> effcs_gndhit;
		size_t use_effcsgndhit = 0;
		/*確保する弾*/
		std::array<ammos, 64> bullet;
		size_t use_bullet = 0;
		std::array<ammo_obj, 64> cart;
		//所持弾数などのデータ
		std::vector<gun_state> gun_stat;
		/*武器ポインタ*/
		Gun* ptr_now = nullptr;				//現在使用中の武装
		size_t ammo_cnt = 0;				//装弾数カウント
		std::array<gun_have, 3> gun_slot;	//銃スロット
		Gun* gun_slot_backup = nullptr;		//バックアップ武器スロット
		/*モデル、音声*/
		Audios audio;
		MV1 obj, mag, body;
		/**/
		float reload_cnt = 0.f;//リロード開始までのカウント
		switchs trigger;//トリガー
		bool gunf = false;//射撃フラグ
		switchs selkey;//トリガー
		bool LEFT_hand = false;//左手を添えているか
		bool reloadf = false;
		bool down_mag = false;
		//プレイヤー座標系
		VECTOR_ref pos;
		MATRIX_ref mat;
		//マガジン座標系
		VECTOR_ref pos_mag;
		MATRIX_ref mat_mag;
		//頭部座標系
		VECTOR_ref pos_HMD;
		MATRIX_ref mat_HMD;
		float add_ypos = 0.f;//垂直加速度
		float body_yrad = 0.f;//胴体角度
		//左手座標系
		VECTOR_ref pos_LHAND;
		MATRIX_ref mat_LHAND;
		VECTOR_ref vecadd_LHAND, vecadd_LHAND_p;//
		//右手座標系
		VECTOR_ref pos_RHAND;
		MATRIX_ref mat_RHAND;
		//
		bool canget_gunitem = false;
		std::string canget_gun;
		//
		bool canget_magitem = false;
		std::string canget_mag;
		//
		void ready_chara(Gun*gundata, Gun*gundata_backup, size_t state_s) {
			this->gun_slot_backup = gundata_backup;
			this->gun_slot[0].set(gundata);
			this->gun_slot[1].delete_gun();
			this->gun_slot[2].delete_gun();
			fill_id(this->gun_slot);
			this->gun_stat.resize(state_s);
			for (auto& s : this->gun_stat) {
				s.in = 0;
				s.select = 0;
			}
		}
		void set_chara(const VECTOR_ref& pos_, const MATRIX_ref& mat_, int gunid, const GraphHandle& scope, MV1& hand_) {
			this->pos = pos_;
			this->mat = mat_;
			//手
			this->body = hand_.Duplicate();// .Duplicate();
			if (gunid >= 0) {
				this->ptr_now = this->gun_slot[gunid].ptr;
			}
			else {
				this->ptr_now = this->gun_slot_backup;
			}
			this->ptr_now->mod.obj.DuplicateonAnime(&this->obj);
			if (this->ptr_now->frame[4].first != INT_MAX) {
				this->obj.SetTextureGraphHandle(1, scope, false);	//スコープ
			}
			this->mag = this->ptr_now->mag.mag.Duplicate();
			for (auto& a : this->cart) {
				a.cnt = -1.f;
				a.pos = VGet(0, 0, 0);
				a.mat = MGetIdent();
			}
			this->LEFT_hand = false;
			if (this->gun_stat[this->ptr_now->id].mag_in.size() >= 1) {
				this->ammo_cnt = this->gun_stat[this->ptr_now->id].mag_in.front();//改良
			}
			else {
				this->ammo_cnt = std::clamp<size_t>(this->gun_stat[this->ptr_now->id].in, 0, this->ptr_now->ammo_max);//改良
			}


			this->gunf = false;
			this->vecadd_LHAND = VGet(0, 0, 1.f);
			this->vecadd_LHAND_p = this->vecadd_LHAND;
			this->reloadf = false;
			this->down_mag = true;
			this->selkey.second = 0;
			for (auto& a : this->obj.get_anime()) {
				a.per = 0.f;
			}
			fill_id(this->effcs);			      //エフェクト
			for (auto& a : this->bullet) {
				a.flug = false;
				a.color = GetColor(255, 255, 172);
			}
			SetCreate3DSoundFlag(TRUE);
			this->audio.shot = this->ptr_now->audio.shot.Duplicate();
			this->audio.slide = this->ptr_now->audio.slide.Duplicate();
			this->audio.trigger = this->ptr_now->audio.trigger.Duplicate();
			this->audio.mag_down = this->ptr_now->audio.mag_down.Duplicate();
			this->audio.mag_set = this->ptr_now->audio.mag_set.Duplicate();
			this->audio.case_down = this->ptr_now->audio.case_down.Duplicate();
			SetCreate3DSoundFlag(FALSE);
		}
		void draw(const bool& usegun, const int& sel_gun) {
			this->body.DrawModel();
			this->obj.DrawModel();
			if (this->ptr_now->cate == 1) {
				if ((!this->reloadf || this->down_mag) && this->gun_stat[this->ptr_now->id].mag_in.size() >= 1) {
					this->mag.DrawModel();
				}
				for (auto& a : this->cart) {
					if (a.cnt < 0.f) { continue; }
					a.second.DrawModel();
				}
			}
			for (auto& s : this->gun_slot) {
				if ((!usegun || s.id != sel_gun) && s.ptr != nullptr) {
					s.obj.DrawModel();
				}
			}
		}
		void draw_ammo() {
			if (this->ptr_now->cate == 1) {
				for (auto& a : this->bullet) {
					if (!a.flug) { continue; }
					DXDraw::Capsule3D(a.pos, a.repos, ((a.spec->caliber - 0.00762f) * 0.1f + 0.00762f), a.color, GetColor(255, 255, 255));
				}
			}
		}
		void delete_chara() {
			this->ptr_now = nullptr;
			this->obj.Dispose();
			this->mag.Dispose();
			for (auto& a : this->cart) {
				a.second.Dispose();
			}
			this->audio.shot.Dispose();
			this->audio.slide.Dispose();
			this->audio.trigger.Dispose();
			this->audio.mag_down.Dispose();
			this->audio.mag_set.Dispose();
			this->audio.case_down.Dispose();
			for (auto& t : this->effcs_gun) {
				t.effect.handle.Dispose();
			}
			for (auto& t : this->effcs) {
				t.handle.Dispose();
			}
		}
		void init() {
			delete_chara();
			for (auto& h : gun_slot) {
				h.delete_gun();
			}
		}
	};
};
//