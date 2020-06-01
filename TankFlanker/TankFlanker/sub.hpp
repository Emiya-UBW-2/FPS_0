#pragma once

#define NOMINMAX
#include <D3D11.h>
#include <array>
#include <fstream>
#include <memory>
#include <optional>
#include <vector>
#include "Box2D/Box2D.h"
#include "DXLib_ref.h"
constexpr auto veh_all = 2;//車種
void set_effect(EffectS* efh, VECTOR_ref pos, VECTOR_ref nor, float scale = 1.f) {
	efh->flug = true;
	efh->pos = pos;
	efh->nor = nor;
	efh->scale = scale;
}
void set_pos_effect(EffectS* efh, const EffekseerEffectHandle& handle) {
	if (efh->flug) {
		efh->handle = handle.Play3D();
		efh->handle.SetPos(efh->pos);
		efh->handle.SetRotation(atan2(efh->nor.y(), std::hypot(efh->nor.x(), efh->nor.z())), atan2(-efh->nor.x(), -efh->nor.z()), 0);
		efh->handle.SetScale(efh->scale);
		efh->flug = false;
	}
	//IsEffekseer3DEffectPlaying(player[0].effcs[i].handle)
}
namespace std {
	template <>
	struct default_delete<b2Body> {
		void operator()(b2Body* body) const {
			body->GetWorld()->DestroyBody(body);
		}
	};
}; // namespace std

typedef std::pair<int, VECTOR_ref> frames;
typedef std::pair<bool, uint8_t> switchs;

//
class Mainclass {
private:

public:
	//gun
	struct Audios{
		SoundHandle shot;
		SoundHandle slide;
		SoundHandle trigger;
		SoundHandle mag_down;
		SoundHandle mag_set;
		SoundHandle case_down;
	};
	class Gun {
	public:
		std::string name;
		MV1 obj, mag, ammo;
		std::vector<frames> frame;
		size_t ammo_max;//マガジンの装弾数
		std::vector <uint8_t> select;//セレクター
		Audios audio;
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

	class Chara {
	public:
		std::array<ef_guns, 60> effcs_gun;    /*effect*/
		size_t gun_effcnt = 0;
		std::array<ammos, 64> bullet;	      /*確保する弾*/
		size_t usebullet{};		      /*使用弾*/

		std::array<EffectS, efs_user> effcs; /*effect*/
		Gun* gunptr;
		Audios audio;
		MV1 obj, mag,hand;
		std::array<ammo_obj, 64> ammo;	      /*確保する弾*/
		VECTOR_ref pos;
		MATRIX_ref mat;
		switchs safety;
		uint8_t trigger;

		uint8_t selkey;
		uint8_t select;//セレクター
		size_t ammoc;//装弾数カウント

		bool gunf;
		uint8_t guncnt;

		bool LEFT_hand;


		bool reloadf;
		MATRIX_ref mat_mag;
		VECTOR_ref mat_addvec;
		float mat_add;

		bool down_mag;

		VECTOR_ref pos_HMD;
		MATRIX_ref mat_HMD;

		VECTOR_ref pos_LHAND;
		MATRIX_ref mat_LHAND;
		VECTOR_ref vecadd_LHAND;
		VECTOR_ref vecadd_LHAND_p;

		VECTOR_ref pos_RHAND;
		MATRIX_ref mat_RHAND;

		std::vector<frames> frame_hand;

		void set_chara(const VECTOR_ref& pos_, Gun*gundata, const GraphHandle& scope,MV1& hand_) {
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
			//
			this->gunptr = gundata;
			this->gunptr->obj.DuplicateonAnime(&this->obj);
			if (this->gunptr->frame[4].first != INT_MAX) {
				this->obj.SetTextureGraphHandle(1, scope, false);	//スコープ
			}
			this->mag = this->gunptr->mag.Duplicate();
			for (auto& a : this->ammo) {
				a.second = this->gunptr->ammo.Duplicate();
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
	};
};
//