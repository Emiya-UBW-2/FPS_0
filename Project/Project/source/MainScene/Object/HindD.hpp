#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class HindDClass : public ObjectBaseClass {
		private:
		public://ゲッター
		public:
			HindDClass(void) noexcept { this->m_objType = ObjType::HindD; }
			~HindDClass(void) noexcept { }
		public:
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
			}
			void			FirstExecute(void) noexcept override {
				//共通
				ObjectBaseClass::FirstExecute();
			}
			void			Dispose(void) noexcept override {
			}
		public:
		};
	};
};
