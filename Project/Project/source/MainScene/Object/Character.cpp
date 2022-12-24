#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {


		//��e�`�F�b�N
		const auto		CharacterClass::CheckAmmoHited(const AmmoClass& pAmmo) noexcept {
			bool is_Hit = false;
			for (auto& h : m_HitBox) {
				is_Hit |= h.Colcheck(pAmmo);
			}
			return is_Hit;
		}
		//��e����
		const auto		CharacterClass::CalcAmmoHited(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept {
			//auto* SE = SoundPool::Instance();
			for (auto& h : m_HitBox) {
				if (h.Colcheck(*pAmmo)) {
					h.GetColType();
					pAmmo->Penetrate();	//�ђ�

					//�_���[�W�v�Z
					auto v1 = GetEyeVector();
					auto v2 = (pShooterPos - this->m_move.pos).Norm(); v2.y(0);
					this->m_DamageEvent.SetEvent(this->m_MyID, this->m_objType, pAmmo->GetDamage(), std::atan2f(v1.cross(v2).y(), v1.dot(v2)));
					++this->m_DamageSwitch;// %= 255;//
					//this->SubHP_Parts(pAmmo->GetDamage(), (HitPoint)tt.GetHitMesh());
					//�G�t�F�N�g�Z�b�g
					return true;
				}
			}
			return false;
		}
		const std::pair<bool, bool>		CharacterClass::CheckAmmoHit(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept {
			std::pair<bool, bool> isDamaged{ false,false };
			if (GetMinLenSegmentToPoint(pAmmo->GetMove().repos, pAmmo->GetMove().pos, this->m_move.pos) <= 2.0f*Scale_Rate) {
				if (this->CheckAmmoHited(*pAmmo)) {									//�Ƃ肠���������������ǂ����T��
					isDamaged.first = true;
					if (this->CalcAmmoHited(pAmmo, pShooterPos)) {
						isDamaged.second = true;
					}
				}
			}
			return isDamaged;
		}
	};
};
