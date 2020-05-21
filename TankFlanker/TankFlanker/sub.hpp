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
constexpr auto veh_all = 2;//é‘éÌ
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
	class Gun {
	public:
		MV1 obj,mag;
		std::vector<frames> frame;
		size_t ammo_max;//É}ÉKÉWÉìÇÃëïíeêî
	};
	//player
		//íeñÚ
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
		int cnt = -1;
	};

	class Chara {
	public:
		std::array<ef_guns, 12> effcs_gun;    /*effect*/
		size_t gun_effcnt = 0;
		std::array<ammos, 64> bullet;	      /*ämï€Ç∑ÇÈíe*/
		size_t usebullet{};		      /*égópíe*/

		std::array<EffectS, efs_user> effcs; /*effect*/
		Gun* gunptr;
		MV1 obj, mag;
		VECTOR_ref pos;
		MATRIX_ref mat;
		switchs safety;
		uint8_t trigger;

		size_t ammos;//ëïíeêîÉJÉEÉìÉg

		bool gunf;
		uint8_t guncnt;

		bool reloadf;
		MATRIX_ref mat_mag;
		VECTOR_ref mat_addvec;
		float mat_add;

		bool down_mag;

		VECTOR_ref pos_LHAND;
		MATRIX_ref mat_LHAND;

		VECTOR_ref pos_RHAND;
		MATRIX_ref mat_RHAND;
	};
};
//