
#pragma once

#include"Header.hpp"
namespace FPS_n2 {
	namespace Sceneclass {
		class GunClass {
			MV1 obj;
			GraphHandle reticle;
			bool boltFlag{ false };
		public:
			void LoadModel(const char* filepath, const char* reticle_filepath) {
				std::string Path = filepath;
				MV1::Load(Path + ".pmx", &this->obj, DX_LOADMODEL_PHYSICS_REALTIME);
				MV1::SetAnime(&this->obj, this->obj);

				reticle = GraphHandle::Load(reticle_filepath);
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
			}
			void Draw() {
				this->obj.DrawModel();
			}
			void Dispose() {
				this->obj.Dispose();
			}
		public:
			void SetMatrix(const MATRIX_ref& value, bool pBoltFlag) {
				obj.SetMatrix(value);
				boltFlag = pBoltFlag;
			}
			const auto GetMatrix() { return obj.GetMatrix(); }
			const auto GetScopePos() { return obj.frame(6); }
			const auto GetLensPos() { return obj.frame(8); }

			const auto GetReticlePos() {
				return GetLensPos() + (GetLensPos() - GetScopePos()).Norm()*10.f;
			}
			
			const auto GetLensPosSize() { return obj.frame(9); }
			const auto GetMuzzleMatrix() { return obj.GetFrameLocalWorldMatrix(5); }
			const auto& GetReticle() { return reticle; }
		};
	};
};
