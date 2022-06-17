#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GunClass : public ObjectBaseClass {
			class BulletClass {
			private:
				float m_cal{ 0.00762f };
			private:
				bool m_isActive{ false };
				moves m_move;
				float m_yAdd{ 0.f };
				float m_Timer{ 0.f };
				float m_HitTimer{ 0.f };
				std::array<VECTOR_ref, 20> m_Line;
				int m_LineSel = 0;
				moves m_move_Hit;
			public:
				void Set(const VECTOR_ref& pos, const VECTOR_ref& vec) noexcept {
					this->m_isActive = true;
					this->m_move.pos = pos;
					this->m_move.vec = vec;
					this->m_yAdd = 0.f;
					this->m_move.repos = this->m_move.pos;
					this->m_Timer = 0.f;
					this->m_HitTimer = 2.f;
					for (auto& l : this->m_Line) {
						l = this->m_move.pos;
					}
				}
				void Execute(void) noexcept {
					if (this->m_isActive) {
						this->m_move.repos = this->m_move.pos;
						this->m_Line[this->m_LineSel] = this->m_move.pos + VECTOR_ref::vget(GetRandf(12.5f*0.3f*this->m_Timer), GetRandf(12.5f*0.3f*this->m_Timer), GetRandf(12.5f*0.3f*this->m_Timer));
						++this->m_LineSel %= this->m_Line.size();
						this->m_move.pos += this->m_move.vec*60.f / FPS + VECTOR_ref::up()*this->m_yAdd;
						this->m_yAdd += (M_GR / (FPS*FPS));
					}

					if (this->m_Timer > std::min(2.f, this->m_HitTimer)) {
						this->m_isActive = false;
					}
					this->m_Timer += 1.f / FPS;
				}
				bool CheckBullet(const MV1* pCol) noexcept {
					if (this->m_isActive) {
						auto HitResult = pCol->CollCheck_Line(
							this->m_move.repos,
							this->m_move.pos);
						if (HitResult.HitFlag == TRUE) {
							this->m_move_Hit.pos = HitResult.HitPosition;
							this->m_move_Hit.vec = HitResult.Normal;
							this->m_isActive = false;
							this->m_HitTimer = this->m_Timer + 0.5f;
							return true;
						}
					}
					return false;
				}
				void Draw(void) noexcept {
					if (this->m_isActive) {
						SetUseLighting(FALSE);
						for (int i = 1; i < this->m_Line.size(); i++) {
							int LS = (i + this->m_LineSel);
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(128.f*((float)(i) / this->m_Line.size())));
							if (CheckCameraViewClip_Box(
								this->m_Line[(LS - 1) % this->m_Line.size()].get(),
								this->m_Line[LS % this->m_Line.size()].get()) == FALSE
								) {
								DrawCapsule3D(this->m_Line[(LS - 1) % this->m_Line.size()].get(), this->m_Line[LS % this->m_Line.size()].get(), this->m_cal*12.5f*8.f*((float)(i) / this->m_Line.size()), 4, GetColor(64, 64, 64), GetColor(64, 64, 64), TRUE);
							}
						}
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
						if (CheckCameraViewClip_Box(
							this->m_move.repos.get(),
							this->m_move.pos.get()) == FALSE
							) {
							DrawCapsule3D(this->m_move.repos.get(), this->m_move.pos.get(), this->m_cal*12.5f*4.f, 4, GetColor(255, 200, 0), GetColor(255, 255, 255), TRUE);
						}

						SetUseLighting(TRUE);
					}
				}
			public:
				const auto* GetMove(void) noexcept {
					if (this->m_isActive) {
						return &this->m_move;
					}
					else {
						return (const moves*)nullptr;
					}
				}
				const auto* GetMoveHit(void) noexcept {
					if (this->m_isActive) {
						return &this->m_move_Hit;
					}
					else {
						return (const moves*)nullptr;
					}
				}
			};
			class CartClass {
			private:
				float m_cal{ 0.00762f };
			private:
				bool m_isActive{ false };
				moves m_move;
				float m_yAdd{ 0.f };
				float m_Timer{ 0.f };
				std::array<VECTOR_ref, 10> m_Line;
				int m_LineSel = 0;
				MV1 m_obj;
			public:
				void Init(const MV1& baseObj) noexcept {
					this->m_obj = baseObj.Duplicate();
				}
				void Set(const VECTOR_ref& pos, const MATRIX_ref& mat, const VECTOR_ref& vec) noexcept {
					this->m_isActive = true;
					this->m_move.pos = pos;
					this->m_move.vec = vec;
					this->m_yAdd = 0.f;
					this->m_move.repos = this->m_move.pos;
					this->m_move.mat = mat;
					this->m_Timer = 0.f;
					for (auto& l : this->m_Line) {
						l = this->m_move.pos;
					}
				}
				void Execute(void) noexcept {
					this->m_obj.SetMatrix(this->m_move.MatIn());
					if (this->m_isActive) {
						this->m_move.repos = this->m_move.pos;
						this->m_Line[this->m_LineSel] = this->m_obj.frame(2);
						++this->m_LineSel %= this->m_Line.size();
						this->m_move.pos += this->m_move.vec*60.f / FPS + VECTOR_ref::up()*this->m_yAdd;
						this->m_yAdd += (M_GR / (FPS*FPS));
					}
					auto BB = (this->m_move.pos - this->m_move.repos).Norm();
					if ((this->m_move.pos - this->m_move.repos).y() <= 0.f) {
						BB *= -1.f;
					}
					this->m_move.mat = MATRIX_ref::RotAxis(BB.cross(this->m_move.mat.zvec()), deg2rad(-(20.f + GetRandf(30.f))*60.f / FPS))*this->m_move.mat;

					if (this->m_Timer > 2.f) {
						this->m_isActive = false;
					}
					this->m_Timer += 1.f / FPS;
				}
				void CheckBullet(const MV1* pCol) noexcept {
					if (this->m_isActive) {
						if ((this->m_move.pos - this->m_move.repos).y() <= 0.f) {
							auto HitResult = pCol->CollCheck_Line(
								this->m_move.repos,
								this->m_move.pos);
							if (HitResult.HitFlag == FALSE) {
								HitResult = pCol->CollCheck_Line(
									this->m_move.pos + VECTOR_ref::up()*1.f,
									this->m_move.pos);
							}
							if (HitResult.HitFlag == TRUE) {
								this->m_move.pos = HitResult.HitPosition;

								VECTOR_ref Normal = HitResult.Normal;
								this->m_move.vec = (this->m_move.vec + Normal * ((Normal*-1.f).dot(this->m_move.vec.Norm())*1.1f))*0.2f;
								this->m_yAdd = 0.f;
							}
						}
					}
				}
				void Draw(void) noexcept {
					if (this->m_isActive) {
						SetUseLighting(FALSE);
						int min = 1 + (int)(this->m_Timer * this->m_Line.size() / 2.f);
						int max = (int)(this->m_Line.size());
						for (int i = max - 1; i >= min; i--) {
							int LS = (i + this->m_LineSel);
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(192.f*((float)(i - min) / max)));
							if (CheckCameraViewClip_Box(
								this->m_Line[(LS - 1) % this->m_Line.size()].get(),
								this->m_Line[LS % this->m_Line.size()].get()) == FALSE
								) {
								DrawCapsule3D(this->m_Line[(LS - 1) % this->m_Line.size()].get(), this->m_Line[LS % this->m_Line.size()].get(), this->m_cal*12.5f*1.f*((float)(i - min) / max), 4, GetColor(128, 128, 128), GetColor(64, 64, 64), TRUE);
							}
						}
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
						SetUseLighting(TRUE);
						if (CheckCameraViewClip_Box(
							this->m_move.repos.get(),
							this->m_move.pos.get()) == FALSE
							) {
							this->m_obj.DrawModel();
						}
					}
				}
			};
		private:
			MV1 m_Cartobj;
			GraphHandle m_reticle;
			bool m_boltFlag{ false };
			std::array<BulletClass, 3> m_Bullet;
			int m_NowShotBullet{ 0 };
			std::array<CartClass, 3> m_Cart;
			int m_NowShotCart{ 0 };
			moves m_move_Hit;
			bool m_IsHit{ false };
			bool m_IsShot{ false };
			bool m_CartFlag{ false };
			bool m_in_chamber{ true };//チャンバー内に初弾があるか(弾倉最大+1かどうか)
			int m_BoltSel = 0;
			//サウンド
			std::vector<SoundHandle> m_BoltSound;
			SoundHandle m_Trigger;
			SoundHandle m_Shot;
			//銃
			std::shared_ptr<MagazineClass> m_Mag_Ptr{ nullptr };
		public://ゲッター
			void SetMagPtr(std::shared_ptr<MagazineClass>& pMagPtr) noexcept { this->m_Mag_Ptr = pMagPtr; }
			const auto GetScopePos(void) noexcept { return this->obj.frame(8); }
			const auto GetLensPos(void) noexcept { return this->obj.frame(10); }
			const auto GetReticlePos(void) noexcept { return GetLensPos() + (GetLensPos() - GetScopePos()).Norm()*10.f; }
			const auto GetLensPosSize(void) noexcept { return this->obj.frame(11); }
			const auto GetMuzzleMatrix(void) noexcept { return this->obj.GetFrameLocalWorldMatrix(7); }
			const auto GetCartMat(void) noexcept { return this->obj.GetFrameLocalWorldMatrix(5); }
			const auto GetCartVec(void) noexcept { return (this->obj.frame(6) - this->obj.frame(5)).Norm(); }
			const auto GetMagMat(void) noexcept { return this->obj.GetFrameLocalWorldMatrix(12); }
			const auto GetCanshot(void) noexcept { return !this->m_Mag_Ptr->IsEmpty(); }
			const auto& GetReticle(void) noexcept { return this->m_reticle; }
			const auto& GetIsHit(void) noexcept { return this->m_IsHit; }
			const auto& GetHitPos(void) noexcept { return this->m_move_Hit.pos; }
			const auto& GetHitVec(void) noexcept { return this->m_move_Hit.vec; }
			const auto& GetIsShot(void) noexcept { return this->m_IsShot; }
			void SetIsShot(bool value) noexcept { this->m_IsShot = value; }
			const auto GetChamberIn(void) noexcept { return (this->obj.get_anime(0).per == 1.f) && (this->obj.get_anime(0).time >= 25.f); }
			void SetMatrix(const MATRIX_ref& value, bool pBoltFlag) noexcept {
				this->obj.SetMatrix(value);
				this->m_Mag_Ptr->SetMatrix(this->GetMagMat());
				this->m_boltFlag = pBoltFlag;
			}
			void SetBullet(void) noexcept {
				float Spd = 12.5f*800.f / 60.f;
				this->m_Bullet[this->m_NowShotBullet].Set(GetMuzzleMatrix().pos(), GetMuzzleMatrix().GetRot().zvec()*-1.f*Spd);
				++this->m_NowShotBullet %= this->m_Bullet.size();

				this->m_Trigger.play_3D(GetMatrix().pos(), 12.5f*5.f);

				this->m_Shot.play_3D(GetMatrix().pos(), 12.5f*50.f);

				if (!this->m_in_chamber) {
					this->m_Mag_Ptr->SubAmmo();//チャンバーインtodo
				}
				this->m_in_chamber = false;

				this->m_IsShot = true;
				this->m_CartFlag = true;
			}
			void SetCart(void) noexcept {
				if (this->obj.get_anime(0).time >= 19.f) {
					if (this->m_CartFlag) {
						this->m_CartFlag = false;

						float Spd = 12.5f*2.f / 60.f;
						this->m_Cart[this->m_NowShotCart].Set(GetCartMat().pos(), GetMuzzleMatrix().GetRot(),
							GetCartVec()*Spd);
						++this->m_NowShotCart %= this->m_Cart.size();
					}
				}
			}
			const auto* GetLatestAmmoMove(void) noexcept {
				auto Now = this->m_NowShotBullet - 1;
				if (Now < 0) { Now = (int)(this->m_Bullet.size()) - 1; }
				return this->m_Bullet[Now].GetMove();
			}
		public:
			GunClass(void) noexcept {
				this->m_objType = ObjType::Gun;
			}
			~GunClass(void) noexcept {}

			void Init(void) noexcept override {
				ObjectBaseClass::Init();
				SetCreate3DSoundFlag(TRUE);
				for (int i = 0; i < 4; i++) {
					this->m_BoltSound.emplace_back(SoundHandle::Load("data/Sound/SE/gun/slide/bolt/" + std::to_string(i) + ".wav"));
				}
				this->m_Trigger = SoundHandle::Load("data/Sound/SE/gun/trigger.wav");
				this->m_Shot = SoundHandle::Load("data/Sound/SE/gun/shot.wav");
				SetCreate3DSoundFlag(TRUE);

				for (auto& b : this->m_BoltSound) {
					b.vol(128);
					Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, b.get());
				}
				this->m_Trigger.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_Trigger.get());
				this->m_Shot.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_Shot.get());

				MV1::Load(this->m_FilePath + "cart.pmd", &this->m_Cartobj);
				for (auto& b : this->m_Cart) {
					b.Init(this->m_Cartobj);
				}
			}
			void Execute(void) noexcept override {
				if (this->m_boltFlag) {
					this->obj.get_anime(0).per = 1.f;
					this->obj.get_anime(0).time += 1.f*30.f / FPS * 1.5f;

					if ((5.f < this->obj.get_anime(0).time && this->obj.get_anime(0).time < 6.f)) {
						if (!this->m_BoltSound[0].check()) {
							this->m_BoltSound[0].play_3D(GetMatrix().pos(), 12.5f*5.f);
						}
					}
					if ((11.f < this->obj.get_anime(0).time && this->obj.get_anime(0).time < 12.f)) {
						if (!this->m_BoltSound[1].check()) {
							this->m_BoltSound[1].play_3D(GetMatrix().pos(), 12.5f*5.f);
						}
					}
					if ((28.f < this->obj.get_anime(0).time && this->obj.get_anime(0).time < 29.f)) {
						if (!this->m_BoltSound[2].check()) {
							this->m_BoltSound[2].play_3D(GetMatrix().pos(), 12.5f*5.f);
						}
					}
					if ((36.f < this->obj.get_anime(0).time && this->obj.get_anime(0).time < 37.f)) {
						if (!this->m_BoltSound[3].check()) {
							this->m_BoltSound[3].play_3D(GetMatrix().pos(), 12.5f*5.f);
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
					this->m_Cartobj.SetMatrix(this->GetCartMat());
				}
				for (auto& b : this->m_Bullet) {
					b.Execute();
				}
				for (auto& b : this->m_Cart) {
					b.Execute();
				}
				//共通
				ObjectBaseClass::Execute();

				this->m_Mag_Ptr->SetChamberIntime(this->GetChamberIn());
				this->m_Mag_Ptr->SetChamberMatrix(this->GetCartMat());
			}
			void Draw(void) noexcept override {
				for (auto& b : this->m_Bullet) {
					b.Draw();
				}
				if (this->m_CartFlag) {
					this->m_Cartobj.DrawModel();
				}
				ObjectBaseClass::Draw();
				for (auto& b : this->m_Cart) {
					b.Draw();
				}
			}
		public:
			void LoadReticle(void) noexcept {
				SetUseASyncLoadFlag(TRUE);
				this->m_reticle = GraphHandle::Load(this->m_FilePath + "reticle.png");
				SetUseASyncLoadFlag(FALSE);
			}
			bool CheckBullet(const MV1* pCol) noexcept {
				for (auto& b : this->m_Cart) {
					b.CheckBullet(pCol);
				}
				this->m_IsHit = false;
				for (auto& b : this->m_Bullet) {
					if (b.CheckBullet(pCol)) {
						auto* ptr = b.GetMoveHit();
						if (ptr != nullptr) {
							this->m_move_Hit = *ptr;
						}
						this->m_IsHit = true;
					}
				}
				return this->m_IsHit;
			}
		};
	};
};
