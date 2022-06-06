
#pragma once

#include"Header.hpp"
namespace FPS_n2 {
	namespace Sceneclass {
		class GunClass : public ObjectBaseClass {
			GraphHandle reticle;
			bool boltFlag{ false };
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
		};
	};
};
