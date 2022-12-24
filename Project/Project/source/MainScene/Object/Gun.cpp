#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void GunClass::SetMagazine(const char* MagName) noexcept {
			if (MagName != nullptr) {
				std::string magName = MagName;
				auto* ObjMngr = ObjectManager::Instance();
				this->m_Mag_Ptr = (std::shared_ptr<MagazineClass>&)(*ObjMngr->AddObject(ObjType::Magazine, ("data/mag/" + magName + "/").c_str()));
				this->m_Mag_Ptr->SetAmmo(GetAmmoAll());
				this->m_in_chamber = false;
			}
			else {
				this->m_Mag_Ptr.reset();
			}
		}
		void GunClass::ExecuteCartInChamber(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();

			const auto& RetMat = GetFrameWorldMat(GunFrame::Cart).GetRot();
			const auto& Pos = GetFrameWorldMat(GunFrame::Cart).pos();
			const auto& PosInMag = this->m_Mag_Ptr->GetObj().GetFrameLocalWorldMatrix(1).pos();

			auto Prev = this->m_IsChamberMove;//GetNowAnime
			this->m_IsChamberMove = false;
			bool IsEject = false;
			switch (GetShotType()) {
			case SHOTTYPE::FULL:
			case SHOTTYPE::SEMI:
				this->m_IsChamberMove |= ((this->m_ShotPhase == 1) && (GetNowAnime().time >= 3.f));
				IsEject |= ((this->m_ShotPhase == 1) && (GetNowAnime().time >= 1.f));

				this->m_IsChamberMove |= ((this->m_ShotPhase == 2) && (GetNowAnime().time >= 25.f));
				IsEject |= ((this->m_ShotPhase == 2) && (GetNowAnime().time >= 19.f));
				break;
			case SHOTTYPE::BOLT:
				this->m_IsChamberMove |= ((this->m_ShotPhase == 5) && (GetNowAnime().time >= 5.f));
				IsEject |= ((this->m_ShotPhase == 3) && (GetNowAnime().time >= 19.f));

				this->m_IsChamberMove |= ((this->m_ShotPhase == 2) && (GetNowAnime().time >= 25.f));
				IsEject |= ((this->m_ShotPhase == 2) && (GetNowAnime().time >= 19.f));
				break;
			default:
				break;
			}

			if (this->m_IsChamberMove) {
				if (this->m_IsChamberMove != Prev) {
					this->m_NowAmmo = this->m_Mag_Ptr->GetAmmoSpec();//マガジンの一番上の弾データを取る
					if (!this->GetIsMagEmpty()) {
						this->m_in_chamber = true;
					}
					this->m_Mag_Ptr->SubAmmo();//チャンバーインtodo
					this->m_CartPtr = (std::shared_ptr<CartClass>&)(*ObjMngr->AddObject(ObjType::Cart, this->m_NowAmmo->GetPath().c_str(), "ammo"));
					this->m_ChamberMovePer = 0.f;
				}
			}
			else {
				if (IsEject) {
					if (this->m_CartPtr.get() != nullptr) {
						this->m_CartPtr->SetEject(Pos, RetMat, ((GetFrameWorldMat(GunFrame::CartVec).pos() - Pos).Norm()) * 2.5f * Scale_Rate / 60.f);
						this->m_CartPtr.reset();
					}
				}
			}
			if (this->m_CartPtr.get() != nullptr) {
				this->m_ChamberMovePer = std::clamp(this->m_ChamberMovePer + 5.f / FPS, 0.f, 1.f);
				this->m_CartPtr->SetMove(RetMat, Lerp(PosInMag, Pos, this->m_ChamberMovePer));
			}
		}
		void GunClass::SetBullet(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();

			SE->Get((int)SoundEnum::Trigger).Play_3D(0, GetMatrix().pos(), Scale_Rate*5.f);
			SE->Get((int)SoundEnum::Shot_Gun).Play_3D(0, GetMatrix().pos(), Scale_Rate*50.f);

			this->m_in_chamber = false;

			this->m_IsShot = true;
			//
			auto& LastAmmo = (std::shared_ptr<AmmoClass>&)(*ObjMngr->AddObject(ObjType::Ammo));
			LastAmmo->Put(this->m_NowAmmo, GetMuzzleMatrix().pos(), GetMuzzleMatrix().GetRot().zvec() * -1.f, this->m_MyID);
			this->m_NowAmmo = nullptr;
			//
			this->m_CartPtr->SetIsEmpty(true);//弾頭をなくす
		}
	};
};
