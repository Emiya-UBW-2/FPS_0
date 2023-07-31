#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PlayerControl {
		private:
			std::shared_ptr<CharacterClass>	m_Chara{ nullptr };
			float							m_Score{ 0.f };							//ƒXƒRƒA
		public:
			PlayerControl(void) noexcept {
				this->m_Score = 0.f;
			}
			~PlayerControl(void) noexcept {
				this->Dispose();
			}
		public:
			void		SetChara(const std::shared_ptr<CharacterClass>& pChara) noexcept { this->m_Chara = pChara; }
			void		AddScore(float value) noexcept { this->m_Score += value; }
			void		SubScore(float value) noexcept { this->m_Score -= value; }
			void		SetScore(float value) noexcept { this->m_Score = value; }
			auto&		GetChara(void) noexcept { return this->m_Chara; }
			const auto&	GetScore(void) const noexcept { return this->m_Score; }

			const auto		GetNetSendMove(void) const noexcept {
				SendInfo ans;
				ans.m_Pos = this->m_Chara->GetMove().pos;
				ans.m_Vec = this->m_Chara->GetMove().vec;
				ans.m_Vec.y(0);
				ans.m_rad = this->m_Chara->GetRadBuf();
				ans.m_Damage = &m_Chara->GetDamageEvent();
				ans.m_DamageSwitch = this->m_Chara->GetDamageSwitch();
				return ans;
			}

			const auto		GetPos(void) const noexcept { return this->m_Chara->GetMatrix().pos(); }
			//const auto		GetAim(void) const noexcept { return this->m_Chara->GetEyeVector(); }
			const auto		GetRadBuf(void) const noexcept { return this->m_Chara->GetRadBuf(); }
		public:
			void Init(void) noexcept {
				m_Chara = nullptr;
			}

			void Dispose(void) noexcept {
				m_Chara = nullptr;
			}
		};

		class PlayerManager : public SingletonBase<PlayerManager> {
		private:
			friend class SingletonBase<PlayerManager>;
		private:
			std::vector<PlayerControl>	m_Player;
		public:
			auto&		GetPlayer(int ID) noexcept { return this->m_Player[ID]; }
		public:
			void Init(int playerNum) noexcept {
				m_Player.resize(playerNum);
			}
			void Dispose(void) noexcept {
				m_Player.clear();
			}
		};

	};
};
