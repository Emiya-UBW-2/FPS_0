#pragma once
#include	"../../Header.hpp"
#include "../../NetWork.hpp"
#include "../../MainScene/Object/ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class PlayerControl {
		private:
			std::shared_ptr<ObjectBaseClass>	m_Chara{ nullptr };
			int								m_Score{ 0 };							//ƒXƒRƒA
		public:
			PlayerControl(void) noexcept {
				this->m_Score = 0;
			}
			~PlayerControl(void) noexcept {
				this->Dispose();
			}
		public:
			void		SetChara(const std::shared_ptr<ObjectBaseClass>& pChara) noexcept { this->m_Chara = pChara; }
			void		AddScore(int value) noexcept { this->m_Score += value; }
			void		SubScore(int value) noexcept { this->m_Score -= value; }
			void		SetScore(int value) noexcept { this->m_Score = value; }
			auto&		GetChara(void) noexcept { return this->m_Chara; }
			const auto&	GetScore(void) const noexcept { return this->m_Score; }

			const SendInfo	GetNetSendMove(void) const noexcept;

			const auto			GetPos(void) const noexcept { return this->m_Chara->GetMatrix().pos(); }
			//const auto		GetAim(void) const noexcept { return this->m_Chara->GetEyeVector(); }
			const VECTOR_ref	GetRadBuf(void) const noexcept;
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
