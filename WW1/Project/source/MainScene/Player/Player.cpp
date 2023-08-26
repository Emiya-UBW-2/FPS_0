#include	"Player.hpp"
#include "../../sub.hpp"
#include "../../MainScene/Object/Character.hpp"

const FPS_n2::Sceneclass::PlayerManager* SingletonBase<FPS_n2::Sceneclass::PlayerManager>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {


		const SendInfo		 PlayerControl::GetNetSendMove(void) const noexcept {
			auto& CharaPtr = (std::shared_ptr<CharacterClass>&)this->m_Chara;

			SendInfo ans;
			ans.m_Pos = CharaPtr->GetMove().pos;
			ans.m_Vec = CharaPtr->GetMove().vec;
			ans.m_Vec.y(0);
			ans.m_rad = CharaPtr->GetRadBuf();
			ans.m_Damage = &CharaPtr->GetDamageEvent();
			ans.m_DamageSwitch = CharaPtr->GetDamageSwitch();
			return ans;
		}
		const VECTOR_ref	PlayerControl::GetRadBuf(void) const noexcept {
			auto& CharaPtr = (std::shared_ptr<CharacterClass>&)this->m_Chara;

			return CharaPtr->GetRadBuf();
		}
	};
};
