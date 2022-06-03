#pragma once
#include"Header.hpp"

//Box2D拡張
namespace std {
	template <>
	struct default_delete<b2Body> {
		void operator()(b2Body* body) const {
			body->GetWorld()->DestroyBody(body);
		}
	};
}; // namespace std
//
namespace FPS_n2 {

	float GameSpeed = 1.0f;

	static const auto GetMocroSec(void) noexcept {
		return GetNowHiPerformanceCount() * (int)(GameSpeed*1000.f) / 1000;
	}

	template <class T>
	static void easing_set_SetSpeed(T* first, const T& aim, const float& ratio) noexcept {
		if (ratio == 0.f) {
			*first = aim;
		}
		else {
			*first = *first + (aim - *first) * (1.f - std::powf(ratio, 60.f / FPS * GameSpeed));
		}
	};

	//ファイル走査
	std::vector<WIN32_FIND_DATA> data_t;
	void GetFileNames(std::string path_t) noexcept {
		data_t.clear();
		WIN32_FIND_DATA win32fdt;
		HANDLE hFind = FindFirstFile((path_t + "*").c_str(), &win32fdt);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (win32fdt.cFileName[0] != '.') {
					data_t.resize(data_t.size() + 1);
					data_t.back() = win32fdt;
				}

			} while (FindNextFile(hFind, &win32fdt));
		} //else{ return false; }
		FindClose(hFind);
	}
	//
	static float GetRandf(float m_arg) noexcept { return -m_arg + (float)(GetRand((int)(m_arg * 2.f * 10000.f))) / 10000.f; }
	//フォントプール
	class FontPool {
	public:
		class Fonthave {
			int size = 0;
			FontHandle handle;
		public:
			const auto& Get_size(void)const noexcept { return size; }
			const auto& Get_handle(void)const noexcept { return handle; }
			void Set(int siz_t) noexcept {
				this->size = siz_t;
				this->handle = FontHandle::Create("x14y24pxHeadUpDaisy",siz_t, DX_FONTTYPE_EDGE, -1, 2);
			}
		};
	private:
		std::vector<Fonthave> havehandle;
		size_t Add(int siz_t) noexcept {
			for (auto& h : this->havehandle) {
				if (h.Get_size() == siz_t) {
					return &h - &this->havehandle.front();
				}
			}
			this->havehandle.resize(this->havehandle.size() + 1);
			this->havehandle.back().Set(siz_t);
			return this->havehandle.size() - 1;
		}
	public:
		Fonthave& Get(int siz_t) noexcept { return this->havehandle[Add(siz_t)]; }
	};
	FontPool Fonts;
	//エフェクトリソース
	class EffectControl {
		LONGLONG Update_effect_was = 0;					//エフェクトのアップデートタイミングタイマー
	public:
		bool Update_effect_f{ true };					//エフェクトのアップデートタイミングフラグ
		std::vector<EffekseerEffectHandle> effsorce;	/*エフェクトリソース*/
		void Init(void) noexcept {
			GetFileNames("data/effect/");
			for (auto& d : data_t) {
				std::string p = d.cFileName;
				if (p.find(".efk") != std::string::npos) {
					effsorce.resize(effsorce.size() + 1);
					effsorce.back() = EffekseerEffectHandle::load("data/effect/" + p);
				}
			}
			effsorce.resize(effsorce.size() + 1);
			effsorce.back() = EffekseerEffectHandle::load("data/effect/gndsmk.efk");								//戦車用エフェクト
			Update_effect_was = GetMocroSec();
		}
		void Calc(void) noexcept {
			Update_effect_f = ((GetMocroSec() - Update_effect_was) >= 1000000 / 60);
			if (Update_effect_f) {
				Update_effect_was = GetMocroSec();
			}
		}
		void Dispose(void) noexcept {
			for (auto& e : effsorce) {
				e.Dispose();
			}
		}
	};
	EffectControl effectControl;
	//ライト
	class LightPool {
		class Lights {
		public:
			LightHandle handle;
			LONGLONG time{ 0 };
		};
		std::array<Lights, 2> handles;
		int now = 0;
		VECTOR_ref campos;
	public:
		void Put(const VECTOR_ref& pos) noexcept {
			if ((pos - campos).size() >= 10.f) { return; }
			if (handles[now].handle.get() != -1) {
				handles[now].handle.Dispose();
			}
			handles[now].time = GetMocroSec();
			handles[now].handle = LightHandle::Create(pos, 2.5f, 0.5f, 1.5f, 0.5f);
			SetLightDifColorHandle(handles[now].handle.get(), GetColorF(1.f, 1.f, 0.f, 1.f));
			++now %= handles.size();
		}
		void Update(const VECTOR_ref& campos_t) noexcept {
			campos = campos_t;
			for (auto& h : handles) {
				if (h.handle.get() != -1) {
					if ((GetMocroSec() - h.time) >= 1000000 / 30) {
						h.handle.Dispose();
					}
				}
			}
		}
	};
	LightPool Light_pool;
	//エフェクト利用コントロール
	class Effect_UseControl {
		std::array<EffectS, int(Effect::effects)> effcs;	/*エフェクト*/
		std::array<EffectS, 256> effcs_G;					/*エフェクト*/
		int G_cnt = 0;
	public:
		void Set_FootEffect(const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept {
			this->effcs_G[this->G_cnt].Stop();
			this->effcs_G[this->G_cnt].Set(pos_t, nomal_t, scale);
			++this->G_cnt %= 256;
		}
		const auto Check_FootEffectCnt(void) noexcept {
			int cnt = 0;
			for (auto& t : this->effcs_G) {
				if (t.GetStart()) { cnt++; }
			}
			return cnt;
		}
		void Set_Effect(Effect ef_, const VECTOR_ref& pos_t, const VECTOR_ref& nomal_t, float scale = 1.f) noexcept { this->effcs[(int)ef_].Set(pos_t, nomal_t, scale); }
		void Stop_Effect(Effect ef_) noexcept { this->effcs[(int)ef_].Stop(); }
		void SetSpeed_Effect(Effect ef_, float value) noexcept { this->effcs[(int)ef_].Set_Speed(value); }
		void SetScale_Effect(Effect ef_, float value) noexcept { this->effcs[(int)ef_].Set_Scale(value); }
		//エフェクトの更新
		void Update_Effect(void) noexcept {
			for (auto& t : this->effcs) {
				const size_t index = &t - &this->effcs.front();
				if (index != (int)Effect::ef_smoke) {
					t.put(effectControl.effsorce[index]);
				}
			}
			for (auto& t : this->effcs_G) {
				t.put(effectControl.effsorce[(int)Effect::ef_gndsmoke]);
			}
		}
		/*おわり*/
		void Dispose_Effect(void) noexcept {
			for (auto& t : this->effcs) { t.handle.Dispose(); }
		}
	};
};