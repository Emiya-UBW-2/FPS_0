
#pragma once

#include"Header.hpp"
namespace FPS_n2 {
	namespace Sceneclass {
		class BulletClass {
		public:
			float m_cal{ 0.00762f };
		public:
			bool isActive{ false };
			moves move;
		public:
			moves move_Hit;
		public:
			void Set(const VECTOR_ref& pos, const VECTOR_ref& vec) {
				isActive = true;
				move.pos = pos;
				move.vec = vec;
				move.repos = move.pos;
			}
			void Execute() {
				if (isActive) {
					move.repos = move.pos;
					move.pos += move.vec;
					move.vec.yadd(M_GR / (FPS*FPS));
				}
			}
			bool SetCol(const MV1* pMapCol) {
				if (isActive) {
					auto HitResult = pMapCol->CollCheck_Line(
						this->move.repos,
						this->move.pos);
					if (HitResult.HitFlag == TRUE) {
						this->move_Hit.pos = HitResult.HitPosition;
						this->move_Hit.vec = HitResult.Normal;
						this->isActive = false;
						return true;
					}
				}
				return false;
			}
			void Draw() {
				if (isActive) {
					SetUseLighting(FALSE);
					DrawCapsule3D(move.repos.get(), move.pos.get(), m_cal*12.5f*4.f, 8, GetColor(255, 255, 172), GetColor(255, 255, 172), TRUE);
					SetUseLighting(TRUE);
				}
			}
		};
		class GunClass : public ObjectBaseClass {
			GraphHandle reticle;
			bool boltFlag{ false };

			std::array<BulletClass, 16> m_Bullet;
			int m_NowShotBullet{ 0 };

			moves move_Hit;
			bool m_IsHit{ false };
		public:
			void LoadReticle(const char* reticle_filepath) {
				reticle = GraphHandle::Load(reticle_filepath);
			}

			void Init() override {
				ObjectBaseClass::Init();
			}
			void Execute() {
				if (this->boltFlag) {
					this->obj.get_anime(0).per = 1.f;
					this->obj.get_anime(0).time += 1.f*30.f / FPS * 1.5f;
				}
				else {
					this->obj.get_anime(0).per = 0.f;
					this->obj.get_anime(0).time = 0.f;
				}
				this->obj.frame_Reset(1);
				this->obj.work_anime();
				this->obj.SetFrameLocalMatrix(1, this->obj.GetFrameLocalMatrix(1).GetRot());

				for (auto& b : m_Bullet) {
					b.Execute();
				}
				m_IsHit = false;
				for (auto& b : m_Bullet) {
					if (b.SetCol(this->m_MapCol)) {
						move_Hit = b.move_Hit;
						m_IsHit = true;
					}
				}
			}
			void Draw() override {
				for (auto& b : m_Bullet) {
					b.Draw();
				}
				ObjectBaseClass::Draw();
			}
		public:
			void SetMatrix(const MATRIX_ref& value, bool pBoltFlag) {
				obj.SetMatrix(value);
				boltFlag = pBoltFlag;
			}
			const auto GetScopePos() { return obj.frame(6); }
			const auto GetLensPos() { return obj.frame(8); }
			const auto GetReticlePos() { return GetLensPos() + (GetLensPos() - GetScopePos()).Norm()*10.f; }
			const auto GetLensPosSize() { return obj.frame(9); }
			const auto GetMuzzleMatrix() { return obj.GetFrameLocalWorldMatrix(5); }
			const auto& GetReticle() { return reticle; }

			const auto& GetIsHit() { return m_IsHit; }
			const auto& GetHitPos() { return move_Hit.pos; }
			const auto& GetHitVec() { return move_Hit.vec; }


			void SetBullet() {
				float Spd = 12.5f*10.f*60.f / FPS;
				m_Bullet[m_NowShotBullet].Set(GetMuzzleMatrix().pos(), GetMuzzleMatrix().GetRot().zvec()*-1.f*Spd);
				++m_NowShotBullet %= m_Bullet.size();
			}

		};
	};
};
