#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class TargetClass : public ObjectBaseClass {
			std::vector<VECTOR_ref> HitPosRec;
		public:
			TargetClass(void) noexcept { this->m_objType = ObjType::Target; }
			~TargetClass(void) noexcept {}
		public:
			const auto& GetHitPosRec() const noexcept { return this->HitPosRec; }
			const auto GetCenterPos() const noexcept { return this->m_col.frame(2); }
			const auto GetHitPoint(const VECTOR_ref& value, float* x = nullptr, float* y = nullptr) noexcept {
				auto vecx = this->m_col.frame(3) - GetCenterPos();
				auto vecy = this->m_col.frame(4) - GetCenterPos();
				auto vecsize = (vecx.size() + vecy.size()) / 2;
				auto vec2 = value - GetCenterPos();

				if (x != nullptr) {
					*x = (vec2.size() / vecsize) * (-vecx.Norm().dot(vec2.Norm()));
				}
				if (y != nullptr) {
					*y = (vec2.size() / vecsize) * (-vecy.Norm().dot(vec2.Norm()));
				}
				return ((1.f - (vec2.size() / vecsize)) * 11.f);
			}
			const auto SetHitPos(const VECTOR_ref& value) noexcept {
				this->m_obj.get_anime(0).GoStart();
				HitPosRec.emplace_back(value);
				return GetHitPoint(HitPosRec.back());
			}
			void ResetHit(void) noexcept {
				HitPosRec.clear();
			}
			void Execute(void) noexcept override {
				this->m_obj.get_anime(0).per = 1.f;
				SetAnimOnce(0, 1.f);
				this->m_obj.work_anime();
			}
		};
	};
};
