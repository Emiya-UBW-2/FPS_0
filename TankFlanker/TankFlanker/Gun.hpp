#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunClass : public ObjectBaseClass {
			class BulletClass {
			public:
				float m_cal{ 0.00762f };
			public:
				bool isActive{ false };
				moves move;
				float yAdd{ 0.f };
				float Timer{ 0.f };
				float HitTimer{ 0.f };
				std::array<VECTOR_ref, 20> Line;
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
					HitTimer = 2.f;
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

					if (Timer > std::min(2.f, HitTimer)) {
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
							HitTimer = Timer + 0.5f;
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
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(128.f*((float)(i) / Line.size())));
							if (CheckCameraViewClip_Box(
								Line[(LS - 1) % Line.size()].get(),
								Line[LS % Line.size()].get()) == FALSE
								) {
								DrawCapsule3D(Line[(LS - 1) % Line.size()].get(), Line[LS % Line.size()].get(), m_cal*12.5f*8.f*((float)(i) / Line.size()), 4, GetColor(64, 64, 64), GetColor(64, 64, 64), TRUE);
							}
						}
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
						if (CheckCameraViewClip_Box(
							move.repos.get(),
							move.pos.get()) == FALSE
							) {
							DrawCapsule3D(move.repos.get(), move.pos.get(), m_cal*12.5f*4.f, 4, GetColor(255, 200, 0), GetColor(255, 255, 255), TRUE);
						}

						SetUseLighting(TRUE);
					}
				}
			};
			class CartClass {
			public:
				float m_cal{ 0.00762f };
			public:
				bool isActive{ false };
				moves move;
				float yAdd{ 0.f };
				float Timer{ 0.f };
				std::array<VECTOR_ref, 10> Line;
				int LineSel = 0;
				MV1 obj;
			public:
				void Init(const MV1& baseObj) {
					this->obj = baseObj.Duplicate();
				}

				void Set(const VECTOR_ref& pos, const MATRIX_ref& mat, const VECTOR_ref& vec) {
					isActive = true;
					move.pos = pos;
					move.vec = vec;
					yAdd = 0.f;
					move.repos = move.pos;
					move.mat = mat;
					Timer = 0.f;
					for (auto& l : Line) {
						l = move.pos;
					}
				}
				void Execute() {
					this->obj.SetMatrix(move.MatIn());
					if (isActive) {
						move.repos = move.pos;
						Line[LineSel] = this->obj.frame(2);
						++LineSel %= Line.size();
						move.pos += move.vec*60.f / FPS + VECTOR_ref::up()*yAdd;
						yAdd += (M_GR / (FPS*FPS));
					}
					auto BB = (move.pos - move.repos).Norm();
					if ((this->move.pos - this->move.repos).y() <= 0.f) {
						BB *= -1.f;
					}
					this->move.mat = MATRIX_ref::RotAxis(BB.cross(this->move.mat.zvec()), deg2rad(-(20.f + GetRandf(30.f))*60.f / FPS))*this->move.mat;

					if (Timer > 2.f) {
						this->isActive = false;
					}
					Timer += 1.f / FPS;
				}
				void CheckBullet(const MV1* pCol) {
					if (isActive) {
						if ((this->move.pos - this->move.repos).y() <= 0.f) {
							auto HitResult = pCol->CollCheck_Line(
								this->move.repos,
								this->move.pos);
							if (HitResult.HitFlag == FALSE) {
								HitResult = pCol->CollCheck_Line(
									this->move.pos + VECTOR_ref::up()*1.f,
									this->move.pos);
							}
							if (HitResult.HitFlag == TRUE) {
								this->move.pos = HitResult.HitPosition;

								VECTOR_ref Normal = HitResult.Normal;
								this->move.vec = (this->move.vec + Normal * ((Normal*-1.f).dot(this->move.vec.Norm())*1.1f))*0.2f;
								this->yAdd = 0.f;
							}
						}
					}
				}
				void Draw() {
					if (isActive) {
						SetUseLighting(FALSE);
						int min = 1 + (int)(Timer * Line.size() / 2.f);
						int max = (int)(Line.size());
						for (int i = max - 1; i >= min; i--) {
							int LS = (i + LineSel);
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(192.f*((float)(i - min) / max)));
							if (CheckCameraViewClip_Box(
								Line[(LS - 1) % Line.size()].get(),
								Line[LS % Line.size()].get()) == FALSE
								) {
								DrawCapsule3D(Line[(LS - 1) % Line.size()].get(), Line[LS % Line.size()].get(), m_cal*12.5f*1.f*((float)(i - min) / max), 4, GetColor(128, 128, 128), GetColor(64, 64, 64), TRUE);
							}
						}
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
						SetUseLighting(TRUE);
						if (CheckCameraViewClip_Box(
							move.repos.get(),
							move.pos.get()) == FALSE
							) {
							this->obj.DrawModel();
						}
					}
				}
			};
		private:
			MV1 Cartobj;

			GraphHandle reticle;
			bool boltFlag{ false };

			std::array<BulletClass, 3> m_Bullet;
			int m_NowShotBullet{ 0 };
			std::array<CartClass, 3> m_Cart;
			int m_NowShotCart{ 0 };


			moves move_Hit;
			bool m_IsHit{ false };

			bool m_IsShot{ false };
			bool m_CartFlag{ false };

			bool in_chamber{ true };//チャンバー内に初弾があるか(弾倉最大+1かどうか)
			int BoltSel = 0;
			std::vector<SoundHandle> BoltSound;
			SoundHandle Trigger;
			SoundHandle Shot;
			//銃
			std::shared_ptr<MagazineClass> m_Mag_Ptr{ nullptr };
		public://ゲッター
			void SetMagPtr(std::shared_ptr<MagazineClass>& pMagPtr) noexcept { this->m_Mag_Ptr = pMagPtr; }
			const auto GetScopePos() { return this->obj.frame(8); }
			const auto GetLensPos() { return this->obj.frame(10); }
			const auto GetReticlePos() { return GetLensPos() + (GetLensPos() - GetScopePos()).Norm()*10.f; }
			const auto GetLensPosSize() { return this->obj.frame(11); }
			const auto GetMuzzleMatrix() { return this->obj.GetFrameLocalWorldMatrix(7); }

			const auto GetCartMat() { return this->obj.GetFrameLocalWorldMatrix(5); }
			const auto GetCartVec() { return (this->obj.frame(6) - this->obj.frame(5)).Norm(); }

			const auto GetMagMat() { return this->obj.GetFrameLocalWorldMatrix(12); }

			void SetMatrix(const MATRIX_ref& value, bool pBoltFlag) {
				this->obj.SetMatrix(value);
				this->m_Mag_Ptr->SetMatrix(this->GetMagMat());
				boltFlag = pBoltFlag;
			}

			const auto GetCanshot() { return !this->m_Mag_Ptr->IsEmpty(); }

			const auto& GetReticle() { return reticle; }
			const auto& GetIsHit() { return m_IsHit; }
			const auto& GetHitPos() { return move_Hit.pos; }
			const auto& GetHitVec() { return move_Hit.vec; }
			const auto& GetIsShot() { return this->m_IsShot; }
			void SetIsShot(bool value) { this->m_IsShot = value; }
			const auto GetChamberIn() {
				return
					(this->obj.get_anime(0).per == 1.f) &&
					(this->obj.get_anime(0).time >= 25.f);
			}
			void SetBullet() {
				float Spd = 12.5f*800.f / 60.f;
				m_Bullet[m_NowShotBullet].Set(GetMuzzleMatrix().pos(), GetMuzzleMatrix().GetRot().zvec()*-1.f*Spd);
				++m_NowShotBullet %= m_Bullet.size();

				Trigger.play_3D(GetMatrix().pos(), 12.5f*5.f);

				Shot.play_3D(GetMatrix().pos(), 12.5f*50.f);

				if (!this->in_chamber) {
					this->m_Mag_Ptr->SubAmmo();//チャンバーインtodo
				}
				this->in_chamber = false;

				this->m_IsShot = true;
				this->m_CartFlag = true;
			}
			void SetCart() {
				if (this->obj.get_anime(0).time >= 19.f) {
					if (this->m_CartFlag) {
						this->m_CartFlag = false;

						float Spd = 12.5f*2.f / 60.f;
						m_Cart[m_NowShotCart].Set(GetCartMat().pos(), GetMuzzleMatrix().GetRot(),
							GetCartVec()*Spd);
						++m_NowShotCart %= m_Cart.size();
					}
				}
			}
			const auto* GetLatestAmmoMove() {
				auto Now = m_NowShotBullet - 1;
				if (Now < 0) { Now = m_Bullet.size() - 1; }
				if (m_Bullet[Now].isActive) {
					return &m_Bullet[Now].move;
				}
				return (const moves*)nullptr;
			}
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

				MV1::Load(this->m_FilePath + "cart.pmd", &Cartobj);
				for (auto& b : m_Cart) {
					b.Init(Cartobj);
				}
			}
			void Execute() override {
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
				if (this->m_CartFlag) {
					Cartobj.SetMatrix(this->GetCartMat());
				}
				for (auto& b : m_Bullet) {
					b.Execute();
				}
				for (auto& b : m_Cart) {
					b.Execute();
				}
				//共通
				ObjectBaseClass::Execute();

				this->m_Mag_Ptr->SetChamberIntime(this->GetChamberIn());
				this->m_Mag_Ptr->SetChamberMatrix(this->GetCartMat());
			}

			bool CheckBullet(const MV1* pCol) {
				for (auto& b : m_Cart) {
					b.CheckBullet(pCol);
				}
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
				if (this->m_CartFlag) {
					Cartobj.DrawModel();
				}
				ObjectBaseClass::Draw();
				for (auto& b : m_Cart) {
					b.Draw();
				}
			}
		public:

		};
	};
};
