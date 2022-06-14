#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class BulletClass : public Effect_UseControl {
		public:
			float m_cal{ 0.00762f };
		public:
			bool isActive{ false };
			moves move;
			float yAdd{ 0.f };
			float Timer{ 0.f };
			float HitTimer{ 0.f };
			std::array<VECTOR_ref, 30> Line;
			int LineSel = 0;
		public:
			moves move_Hit;
		public:
			void Set(const VECTOR_ref& pos, const VECTOR_ref& vec) {
				isActive = true;
				move.pos = pos;
				move.vec = vec;
				yAdd = 0.f;
				move.repos = move.pos;
				Timer = 0.f;
				HitTimer = 3.f;
				for (auto& l : Line) {
					l = move.pos;
				}
			}
			void Execute() {
				if (isActive) {
					move.repos = move.pos;
					Line[LineSel] = move.pos + VECTOR_ref::vget(GetRandf(12.5f*0.3f*Timer), GetRandf(12.5f*0.3f*Timer), GetRandf(12.5f*0.3f*Timer));
					++LineSel %= Line.size();
					move.pos += move.vec*60.f / FPS + VECTOR_ref::up()*yAdd;
					yAdd += (M_GR / (FPS*FPS));
				}

				if (Timer > std::min(3.f, HitTimer)) {
					this->isActive = false;
				}
				Timer += 1.f / FPS;
			}
			bool CheckBullet(const MV1* pCol) {
				if (isActive) {
					auto HitResult = pCol->CollCheck_Line(
						this->move.repos,
						this->move.pos);
					if (HitResult.HitFlag == TRUE) {
						this->move_Hit.pos = HitResult.HitPosition;
						this->move_Hit.vec = HitResult.Normal;
						this->isActive = false;
						HitTimer = Timer+0.5f;
						return true;
					}
				}
				return false;
			}
			void Draw() {
				if (isActive) {
					SetUseLighting(FALSE);
					for (int i = 1; i < Line.size(); i++) {
						int LS = (i + LineSel);
						SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(128.f*((float)(i) / Line.size())) );
						DrawCapsule3D(Line[(LS - 1) % Line.size()].get(), Line[LS % Line.size()].get(), m_cal*12.5f*8.f, 8, GetColor(64, 64, 64), GetColor(64, 64, 64), TRUE);
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

					DrawCapsule3D(move.repos.get(), move.pos.get(), m_cal*12.5f*4.f, 8, GetColor(255, 200, 0), GetColor(255, 255, 255), TRUE);
					SetUseLighting(TRUE);
				}
			}
		};
		class GunClass : public ObjectBaseClass, public Effect_UseControl{
			GraphHandle reticle;
			bool boltFlag{ false };

			std::array<BulletClass, 8> m_Bullet;
			int m_NowShotBullet{ 0 };

			moves move_Hit;
			bool m_IsHit{ false };

			bool m_IsShot{ false };

			int BoltSel = 0;
			std::vector<SoundHandle> BoltSound;
			SoundHandle Trigger;
			SoundHandle Shot;
		public:
			GunClass() {
				m_objType = ObjType::Gun;
			}
			~GunClass() {

			}

			void LoadReticle() {
				reticle = GraphHandle::Load(this->m_FilePath + "reticle.png");
			}

			void Init() override {
				ObjectBaseClass::Init();
				SetCreate3DSoundFlag(TRUE);
				for (int i = 0; i < 4; i++) {
					BoltSound.emplace_back(SoundHandle::Load("data/Sound/SE/gun/slide/bolt/" + std::to_string(i) + ".wav"));
				}
				Trigger = SoundHandle::Load("data/Sound/SE/gun/trigger.wav");
				Shot = SoundHandle::Load("data/Sound/SE/gun/shot.wav");
				SetCreate3DSoundFlag(TRUE);

				for (auto& b : BoltSound) {
					b.vol(128);
					Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, b.get());
				}
				Trigger.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, Trigger.get());
				Shot.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, Shot.get());

			}
			void Execute() override {
				this->m_IsShot = false;
				if (this->boltFlag) {
					this->obj.get_anime(0).per = 1.f;
					this->obj.get_anime(0).time += 1.f*30.f / FPS * 1.5f;

					if ((5.f < this->obj.get_anime(0).time && this->obj.get_anime(0).time < 6.f)) {
						if (!BoltSound[0].check()) {
							BoltSound[0].play_3D(GetMatrix().pos(), 12.5f*5.f);
						}
					}
					if ((11.f < this->obj.get_anime(0).time && this->obj.get_anime(0).time < 12.f)) {
						if (!BoltSound[1].check()) {
							BoltSound[1].play_3D(GetMatrix().pos(), 12.5f*5.f);
						}
					}
					if ((28.f < this->obj.get_anime(0).time && this->obj.get_anime(0).time < 29.f)) {
						if (!BoltSound[2].check()) {
							BoltSound[2].play_3D(GetMatrix().pos(), 12.5f*5.f);
						}
					}
					if ((36.f < this->obj.get_anime(0).time && this->obj.get_anime(0).time < 37.f)) {
						if (!BoltSound[3].check()) {
							BoltSound[3].play_3D(GetMatrix().pos(), 12.5f*5.f);
						}
					}
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
			}

			bool CheckBullet(const MV1* pCol) {
				m_IsHit = false;
				for (auto& b : m_Bullet) {
					if (b.CheckBullet(pCol)) {
						move_Hit = b.move_Hit;
						m_IsHit = true;
					}
				}
				return m_IsHit;
			}

			void Draw() override {
				for (auto& b : m_Bullet) {
					b.Draw();
				}
				ObjectBaseClass::Draw();
			}
		public:
			void SetMatrix(const MATRIX_ref& value, bool pBoltFlag) {
				this->obj.SetMatrix(value);
				boltFlag = pBoltFlag;
			}
			const auto GetScopePos() { return this->obj.frame(6); }
			const auto GetLensPos() { return this->obj.frame(8); }
			const auto GetReticlePos() { return GetLensPos() + (GetLensPos() - GetScopePos()).Norm()*10.f; }
			const auto GetLensPosSize() { return this->obj.frame(9); }
			const auto GetMuzzleMatrix() { return this->obj.GetFrameLocalWorldMatrix(5); }
			const auto& GetReticle() { return reticle; }

			const auto& GetIsHit() { return m_IsHit; }
			const auto& GetHitPos() { return move_Hit.pos; }
			const auto& GetHitVec() { return move_Hit.vec; }

			const auto& GetIsShot() { return this->m_IsShot; }

			void SetBullet() {
				float Spd = 12.5f*800.f / 60.f;
				m_Bullet[m_NowShotBullet].Set(GetMuzzleMatrix().pos(), GetMuzzleMatrix().GetRot().zvec()*-1.f*Spd);
				++m_NowShotBullet %= m_Bullet.size();

				Trigger.play_3D(GetMatrix().pos(), 12.5f*5.f);

				Shot.play_3D(GetMatrix().pos(), 12.5f*50.f);

				this->m_IsShot = true;
			}

			const auto* GetLatestAmmoMove() {
				auto Now = m_NowShotBullet-1;
				if (Now < 0) { Now = m_Bullet.size() - 1; }
				if (m_Bullet[Now].isActive) {
					return &m_Bullet[Now].move;
				}
				return (const moves*)nullptr;
			}

		};
	};
};
