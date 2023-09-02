#pragma once
#include	"BackGroundSub.hpp"

#include	"../../sub.hpp"

namespace FPS_n2 {
	class Grass::grass_t {
	public:
		bool			canlook = true;
		Model_Instance	m_Inst;
	public:
		void Init(int total, const char* Graph, const char* Model) {
			this->m_Inst.Init(Graph, Model, -1);
			this->m_Inst.Reset();
			this->m_Inst.Set_start(total);
		}
		void Set_one(const MATRIX_ref& mat) {
			this->m_Inst.Set_one(mat);
		}
		void put(void) noexcept {
			canlook = true;
			this->m_Inst.Execute();
		}
		void Dispose(void) noexcept {
			this->m_Inst.Dispose();
		}
		/*視界外か否かを判断*/
		void Check_CameraViewClip(const VECTOR_ref& min, const VECTOR_ref& max) {
			this->canlook = true;
			if (CheckCameraViewClip_Box(min.get(), max.get())) {
				this->canlook = false;
				return;
			}
		}
		void Draw(void) noexcept {
			if (this->canlook) {
				this->m_Inst.Draw();
			}
		}
	};

	class Grass::Impl {
	public:
		static const int grassDiv{ 12 };//^2;
	private:
		int grasss = 6 * 6;						/*grassの数*/
		std::array<std::pair<float, int>, grassDiv>grassLen;
		std::array<grass_t, grassDiv>grass__;
		std::array<VECTOR_ref, grassDiv>grassPosMin;
		std::array<VECTOR_ref, grassDiv>grassPosMax;
		int Flag = 0;
		std::array<GrassPos, grassDiv> grassPos;
	public:
		Impl() {}
		~Impl() {}
	public:
		int GetColorSoftImage(int softimage, int x_, int y_) {
			int _r_, _g_, _b_;
			int CCC = 0;
			GetPixelSoftImage(softimage, x_, y_, &_r_, &_g_, &_b_, nullptr);
			if (_r_ <= 64) {}
			else if (_r_ <= 192) { CCC |= (1 << 1); }
			else if (_r_ <= 256) { CCC |= (1 << 2); }
			if (_g_ <= 64) {}
			else if (_g_ <= 192) { CCC |= (1 << 4); }
			else if (_g_ <= 256) { CCC |= (1 << 5); }
			if (_b_ <= 64) {}
			else if (_b_ <= 192) { CCC |= (1 << 7); }
			else if (_b_ <= 256) { CCC |= (1 << 8); }
			return CCC;
		}
		//y方向に操作する前提
		void SetMinMax(int CCC, int ID, int softimage, int x_t, int y_t, int sizex, int sizey) {
			int BufC = -1;
			if ((Flag & (1 << ID)) == 0) {
				Flag |= (1 << ID);
				//xmin
				grassPos[ID].X_PosMin = x_t;
				y_t;
				//ymin
				BufC = -1;
				for (int y_ = 0; y_ < sizey; y_++) {
					for (int x_ = grassPos[ID].X_PosMin; x_ < sizex; x_++) {
						BufC = GetColorSoftImage(softimage, x_, y_);
						if (BufC == CCC) {
							grassPos[ID].Y_PosMin = y_;
							break;
						}
						else {
							BufC = -1;
						}
					}
					if (BufC >= 0) { break; }
				}
				//xmax
				BufC = -1;
				for (int x_ = sizex - 1; x_ >= grassPos[ID].X_PosMin; x_--) {
					for (int y_ = sizey - 1; y_ >= grassPos[ID].Y_PosMin; y_--) {
						BufC = GetColorSoftImage(softimage, x_, y_);
						if (BufC == CCC) {
							grassPos[ID].X_PosMax = x_ + 1;
							break;
						}
						else {
							BufC = -1;
						}
					}
					if (BufC >= 0) { break; }
				}
				//ymax
				BufC = -1;
				for (int y_ = sizey - 1; y_ >= grassPos[ID].Y_PosMin; y_--) {
					for (int x_ = grassPos[ID].X_PosMax - 1; x_ >= grassPos[ID].X_PosMin; x_--) {
						BufC = GetColorSoftImage(softimage, x_, y_);
						if (BufC == CCC) {
							grassPos[ID].Y_PosMax = y_ + 1;
							break;
						}
						else {
							BufC = -1;
						}
					}
					if (BufC >= 0) { break; }
				}
				//ok
			}
		}
		void Init(const MV1* MapCol, int softimage, const char* Graph, const char* Model, float MAPX, float MAPZ, int count) {
			grasss = count * count;
			//float MAPX = 6000.f*Scale_Rate;
			//float MAPZ = 6000.f*Scale_Rate;
			float PosX = 0.f;
			float PosZ = 0.f;

			float MINX = -MAPX / 2.f + PosX;
			float MINZ = -MAPZ / 2.f + PosZ;

			float MAXX = MAPX / 2.f + PosX;
			float MAXZ = MAPZ / 2.f + PosZ;

			int sizex = 0, sizey = 0;
			GetSoftImageSize(softimage, &sizex, &sizey);

			Flag = 0;
			for (int x_ = 0; x_ < sizex; x_++) {
				for (int y_ = 0; y_ < sizey; y_++) {
					int CCC = GetColorSoftImage(softimage, x_, y_);
					//255,0,0
					if (CCC == (1 << 2)) {
						SetMinMax(CCC, 0, softimage, x_, y_, sizex, sizey);
					}
					//255,128,0
					else if (CCC == ((1 << 2) | (1 << 4))) {
						SetMinMax(CCC, 1, softimage, x_, y_, sizex, sizey);
					}
					//255,255,0
					else if (CCC == ((1 << 2) | (1 << 5))) {
						SetMinMax(CCC, 2, softimage, x_, y_, sizex, sizey);
					}
					//128,255,0
					else if (CCC == ((1 << 1) | (1 << 5))) {
						SetMinMax(CCC, 3, softimage, x_, y_, sizex, sizey);
					}
					//0,255,0
					else if (CCC == (1 << 5)) {
						SetMinMax(CCC, 4, softimage, x_, y_, sizex, sizey);
					}
					//0,255,128
					else if (CCC == ((1 << 5) | (1 << 7))) {
						SetMinMax(CCC, 5, softimage, x_, y_, sizex, sizey);
					}
					//0,255,255
					else if (CCC == ((1 << 5) | (1 << 8))) {
						SetMinMax(CCC, 6, softimage, x_, y_, sizex, sizey);
					}
					//0,128,255
					else if (CCC == ((1 << 4) | (1 << 8))) {
						SetMinMax(CCC, 7, softimage, x_, y_, sizex, sizey);
					}
					//0,0,255
					else if (CCC == (1 << 8)) {
						SetMinMax(CCC, 8, softimage, x_, y_, sizex, sizey);
					}
					//128,0,255
					else if (CCC == ((1 << 1) | (1 << 8))) {
						SetMinMax(CCC, 9, softimage, x_, y_, sizex, sizey);
					}
					//255,0,255
					else if (CCC == ((1 << 2) | (1 << 8))) {
						SetMinMax(CCC, 10, softimage, x_, y_, sizex, sizey);
					}
					//255,0,128
					else if (CCC == ((1 << 2) | (1 << 7))) {
						SetMinMax(CCC, 11, softimage, x_, y_, sizex, sizey);
					}

					//MINX + (MAXX - MINX) * x_ / sizex = x_t 
					//MINZ + (MAXZ - MINZ) * y_ / sizey = z_t 
				}
			}
			//*/
			for (int ID = 0; ID < grassDiv; ID++) {
				//ポジション決定
				float xp = MINX + (MAXX - MINX) * grassPos[ID].X_PosMin / sizex;
				float zp = MINZ + (MAXZ - MINZ) * grassPos[ID].Y_PosMin / sizey;
				float xp2 = MINX + (MAXX - MINX) * grassPos[ID].X_PosMax / sizex;
				float zp2 = MINZ + (MAXZ - MINZ) * grassPos[ID].Y_PosMax / sizey;
				float xsize = xp2 - xp;
				float zsize = zp2 - zp;
				//
				{
					grassPosMin[ID] = VECTOR_ref::vget(xp, 0.2f, zp);
					grassPosMax[ID] = grassPosMin[ID] + VECTOR_ref::vget(xsize, 1.f, zsize);
					float xmid = xsize / 2.f;
					float zmid = zsize / 2.f;
					if (grasss != 0) {
						auto& tgt_g = grass__[ID];
						tgt_g.Init(grasss, Graph, Model);
						for (int i = 0; i < grasss; ++i) {
							float x1 = xmid + GetRandf(xmid);
							float z1 = zmid + GetRandf(zmid);
							int count_buf = 0;
							while (true) {
								int CCC = GetColorSoftImage(softimage,
									(int)(((grassPosMin[ID].x() + x1) - MINX) / (MAXX - MINX) * float(sizex)),
									(int)(((grassPosMin[ID].z() + z1) - MINZ) / (MAXZ - MINZ) * float(sizey))
								);
								if (CCC != 0) {
									break;
								}
								x1 = xmid + GetRandf(xmid);
								z1 = zmid + GetRandf(zmid);
								count_buf++;
								if (count_buf > 10) { break; }
							}

							auto tmpvect = grassPosMin[ID] + VECTOR_ref::vget(x1, 0.2f, z1);
							auto scale = 15.f / 10.f*Scale_Rate;
							auto res = MapCol->CollCheck_Line(tmpvect + VECTOR_ref::up()*300.f*Scale_Rate, tmpvect + VECTOR_ref::up()*-300.f*Scale_Rate);
							if (res.HitFlag == TRUE) { tmpvect = res.HitPosition; }
							tgt_g.Set_one(MATRIX_ref::RotY(deg2rad(GetRand(90))) * MATRIX_ref::GetScale(VECTOR_ref::vget(scale, scale, scale)) *  MATRIX_ref::Mtrans(tmpvect));
						}
						tgt_g.put();
					}
					{
						auto res = MapCol->CollCheck_Line(grassPosMin[ID] + VECTOR_ref::up()*300.f*Scale_Rate, grassPosMin[ID] + VECTOR_ref::up()*-300.f*Scale_Rate);
						if (res.HitFlag == TRUE) { grassPosMin[ID] = res.HitPosition; }
						res = MapCol->CollCheck_Line(grassPosMax[ID] + VECTOR_ref::up()*300.f*Scale_Rate, grassPosMax[ID] + VECTOR_ref::up()*-300.f*Scale_Rate);
						if (res.HitFlag == TRUE) { grassPosMax[ID] = res.HitPosition; }
					}
				}
				//
			}
		}
		void Dispose(void) noexcept {
			for (int ID = 0; ID < grassDiv; ID++) {
				if (grasss != 0) {
					grass__[ID].Dispose();
				}
			}
		}
		void Draw(void) noexcept {
			SetFogEnable(TRUE);
			SetFogColor(4, 16, 0);
			SetDrawAlphaTest(DX_CMP_GREATER, 128);
			//SetUseLighting(FALSE);
			SetUseLightAngleAttenuation(FALSE);
			//auto dir=GetLightDirection();
			//VECTOR_ref vec = (VECTOR_ref)GetCameraPosition() - GetCameraTarget();
			//SetLightDirection(vec.Norm().get());


			for (int ID = 0; ID < grassDiv; ID++) {
				this->grassLen[ID].first = 10000.f*Scale_Rate;
				this->grassLen[ID].second = ID;
				if (grasss != 0) {
					this->grass__[ID].Check_CameraViewClip(grassPosMin[ID], grassPosMax[ID]);
					if (this->grass__[ID].canlook) {
						this->grassLen[ID].first = ((grassPosMin[ID] + grassPosMax[ID]) / 2.f - GetCameraPosition()).Length();
						this->grassLen[ID].second = ID;
					}
				}
			}
			std::sort(this->grassLen.begin(), this->grassLen.end(), [&](const std::pair<float, int>&A, const std::pair<float, int>&B) {return A.first < B.first; });

			for (int i = 0; i < grassDiv / 2; i++) {
				int ID = this->grassLen[i].second;
#ifdef DEBUG
				//DrawCube3D(grassPosMin[ID].get(), grassPosMax[ID].get(), GetColor(0, 0, 0), GetColor(0, 0, 0), FALSE);
#endif
				if (grasss != 0) {
					if (this->grassLen[i].first < 4000.f*Scale_Rate) {
						grass__[ID].Draw();
					}
				}
			}
			//SetLightDirection(dir);

			SetUseLightAngleAttenuation(TRUE);
			//SetUseLighting(TRUE);
			SetDrawAlphaTest(-1, 0);
			SetFogEnable(FALSE);
		}
	};


	Grass::Grass() {
		m_Impl = new Impl();
	}
	Grass::~Grass() {}
	void Grass::Init(const MV1* MapCol, int softimage, const char* Graph, const char* Model, float MAPX, float MAPZ, int count) {
		m_Impl->Init(MapCol, softimage, Graph, Model, MAPX, MAPZ, count);
	}
	void Grass::Dispose(void) noexcept {
		m_Impl->Dispose();
	}
	void Grass::Draw(void) noexcept {
		m_Impl->Draw();
	}

};
