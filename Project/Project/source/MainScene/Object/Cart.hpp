#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class CartClass : public ObjectBaseClass {
		private:
			float m_cal{ 0.00762f };
		private:
			bool		m_IsInMag{ false };
			bool		m_IsEmpty{ false };

			float		m_yAdd{ 0.f };
			float		m_Timer{ 0.f };
			std::array<VECTOR_ref, 10> m_Line;
			int			m_LineSel = 0;
		public:
			CartClass(void) { this->m_objType = ObjType::Cart; }
			~CartClass(void) { }
		public:
			void	SetIsEmpty(bool value) { this->m_IsEmpty = value; }
			void	SetEject(const VECTOR_ref& pos, const MATRIX_ref& mat, const VECTOR_ref& vec) {
				this->m_IsInMag = false;
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
			void	CheckBullet(void) {
				if (IsActive()) {
					if ((this->m_move.pos - this->m_move.repos).y() <= 0.f) {
						auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(this->m_move.repos + VECTOR_ref::up()*1.f, this->m_move.pos);
						if (HitResult.HitFlag == TRUE) {
							this->m_move.pos = HitResult.HitPosition;

							VECTOR_ref Normal = HitResult.Normal;
							this->m_move.vec = (this->m_move.vec + Normal * ((Normal*-1.f).dot(this->m_move.vec.Norm())*2.f))*0.1f;
							this->m_yAdd = 0.f;
						}
					}
				}
			}
		public:
			void	Init(void) override {
				ObjectBaseClass::Init();
				this->m_IsInMag = true;//チェンバー内にあり
				this->m_IsEmpty = false;//弾頭付き
			}

			void	FirstExecute(void) override {
				if (!this->m_IsInMag) {
					if (IsActive()) {
						GetObj().SetMatrix(this->m_move.MatIn());

						this->m_move.repos = this->m_move.pos;
						this->m_Line[this->m_LineSel] = GetObj().frame(2);
						++this->m_LineSel %= this->m_Line.size();
						this->m_move.pos += this->m_move.vec*60.f / FPS + VECTOR_ref::up()*this->m_yAdd;
						this->m_yAdd += (M_GR / (FPS*FPS));

						auto BB = (this->m_move.pos - this->m_move.repos).Norm();
						if ((this->m_move.pos - this->m_move.repos).y() <= 0.f) {
							BB *= -1.f;
						}
						this->m_move.mat = MATRIX_ref::RotAxis(BB.cross(this->m_move.mat.zvec()).Norm(), deg2rad((20.f + GetRandf(30.f)) / 2.f*60.f / FPS))*this->m_move.mat;

						if (this->m_Timer > 5.f) {
							SetIsDelete(true);
						}
						this->m_Timer += 1.f / FPS;
					}
				}
				CheckBullet();
			}

			void	DrawShadow(void) override {
				if (IsActive() && this->m_IsDraw) {
					if (this->m_IsEmpty) {
						this->GetObj().DrawMesh(0);
					}
					else {
						this->GetObj().DrawModel();
					}
				}
			}
			void	Draw(void) override {
				if (IsActive() && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-20, 0, -20)).get(),
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(20, 20, 20)).get()) == FALSE
						) {
						if (!this->m_IsInMag && this->m_IsEmpty) {
							SetUseLighting(FALSE);
							int max = (int)(this->m_Line.size());
							int min = 1 + (int)(this->m_Timer * max / 2.f);
							for (int i = max - 1; i >= min; i--) {
								int LS = (i + this->m_LineSel);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(192.f*((float)(i - min) / max)));
								auto p1 = (LS - 1) % max;
								auto p2 = LS % max;
								if (CheckCameraViewClip_Box(
									this->m_Line[p1].get(),
									this->m_Line[p2].get()) == FALSE
									) {
									DrawCapsule3D(this->m_Line[p1].get(), this->m_Line[p2].get(), this->m_cal*Scale_Rate*1.f*((float)(i - min) / max), 3, GetColor(128, 128, 128), GetColor(64, 64, 64), TRUE);
								}
							}
							SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
							SetUseLighting(TRUE);
						}
						if (this->m_IsEmpty) {
							this->GetObj().DrawMesh(0);
						}
						else {
							this->GetObj().DrawModel();
						}
					}
				}
			}
		};
	};
};
