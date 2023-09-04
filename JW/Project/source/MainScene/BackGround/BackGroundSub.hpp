#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	//kusa
	class Grass {
		class grass_t;
		struct GrassPos {
			int X_PosMin = 0;
			int Y_PosMin = 0;
			int X_PosMax = 0;
			int Y_PosMax = 0;
		};
	private:
		class Impl;
		Impl* m_Impl{ nullptr };
	public:
		Grass();
		~Grass();
	public:
		void Init(const MV1* MapCol, int softimage, const char* Graph, const char* Model, float MAPX, float MAPZ, int count);
		void Dispose(void) noexcept;
		void DrawShadow(void) noexcept {}
		void Draw(void) noexcept;
	};
	//
	struct treePats {
		MV1 obj, obj_far;
		MATRIX_ref mat;
		VECTOR_ref pos;
	};
	class TreeControl {
		MV1 tree_model, tree_far;

		std::vector<treePats>		tree;
	public:
		void	Load() noexcept {
			MV1::Load("data/model/tree/model.mv1", &tree_model, true); //木
			MV1::Load("data/model/tree/model2.mv1", &tree_far, true); //木
		}
		void	Init(const MV1* MapCol) noexcept {
			tree.resize(1300);
			for (auto& t : tree) {
				auto scale = 15.f / 10.f*Scale_Rate;
				t.mat = MATRIX_ref::GetScale(VECTOR_ref::vget(scale, scale, scale));
				t.pos = VECTOR_ref::vget(
					GetRandf(150)*10.f*Scale_Rate,
					0.f,
					GetRandf(150)*10.f*Scale_Rate);


				auto res = MapCol->CollCheck_Line(t.pos + VECTOR_ref::up()*300.f*Scale_Rate, t.pos + VECTOR_ref::up()*-300.f*Scale_Rate);
				if (res.HitFlag == TRUE) {
					t.pos = res.HitPosition;
				}
				t.obj = tree_model.Duplicate();
				t.obj_far = tree_far.Duplicate();
				t.obj_far.material_AlphaTestAll(true, DX_CMP_GREATER, 128);

				t.obj.SetMatrix(t.mat * MATRIX_ref::Mtrans(t.pos));
			}
		}
		void	Execute(void) noexcept {
			//木セット
			for (auto& t : tree) {
				t.obj.SetMatrix(t.mat * MATRIX_ref::Mtrans(t.pos));
			}
		}
		void	Dispose(void) noexcept {
			tree_model.Dispose(); //木
			tree_far.Dispose(); //木
			for (auto&t : tree) {
				t.obj.Dispose();
				t.obj_far.Dispose();
			}
			tree.clear();
		}
		void	DrawFar() noexcept {
			for (auto& t : tree) {
				t.obj.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
				t.obj.DrawModel();
			}
		}
		void	Draw(bool isSetFog) noexcept {
			int fog_enable = 0;
			int fog_mode = 0;
			int fog_r = 0, fog_g = 0, fog_b = 0;
			float fog_start = 0.f, fog_end = 0.f;
			float fog_density = 0.f;
			if (isSetFog) {
				fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効  FALSE:無効 )
				fog_mode = GetFogMode();														// フォグモードを取得する
				GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
				GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
				fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )

				SetFogEnable(TRUE);
				SetFogColor(4, 16, 0);
				SetFogStartEnd(Scale_Rate*5.f, Scale_Rate*50.f);
			}
			auto Farlimit = 360.f*Scale_Rate;
			auto Farlimit2 = 240.f*Scale_Rate;
			for (auto& t : tree) {
				auto LengthtoCam = (t.pos - GetCameraPosition());

				if (CheckCameraViewClip_Box(
					(t.pos + VECTOR_ref::vget(-20, 0, -20)*Scale_Rate).get(),
					(t.pos + VECTOR_ref::vget(20, 20, 20)*Scale_Rate).get()) == FALSE
					) {
					if (LengthtoCam.Length() > Farlimit && isSetFog) {
						SetUseLighting(FALSE);
						LengthtoCam.y(0.f); LengthtoCam = LengthtoCam.Norm();
						float rad = std::atan2f(VECTOR_ref::front().cross(LengthtoCam).y(), VECTOR_ref::front().dot(LengthtoCam));
						t.obj_far.SetMatrix(MATRIX_ref::RotY(rad) * t.mat * MATRIX_ref::Mtrans(t.pos));
						t.obj_far.DrawModel();
						SetUseLighting(TRUE);
					}
					else {
						if (isSetFog) {
							t.obj.material_AlphaTestAll(true, DX_CMP_GREATER, (int)(128.f + 127.f*(Farlimit2 - LengthtoCam.Length()) / Farlimit2));
						}
						else {
							t.obj.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
						}
						t.obj.DrawModel();
					}
				}

			}
			if (isSetFog) {
				SetFogEnable(fog_enable);
				SetFogMode(fog_mode);
				SetFogColor(fog_r, fog_g, fog_b);
				SetFogStartEnd(fog_start, fog_end);
				SetFogDensity(fog_density);
			}
		}
	};
};
