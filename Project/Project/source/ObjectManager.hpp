#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ObjectManager {
			std::vector<std::shared_ptr<ObjectBaseClass>> m_Object;
			switchs m_ResetP;
		public:
			void AddObject(ObjType ModelType) {
				switch (ModelType) {
				case ObjType::Human://human
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<CharacterClass>();
					break;
				case ObjType::Circle://human
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<CircleClass>();
					break;
				default:
					break;
				}
			}
			void LoadObj(const char* filepath, const char* objfilename = "model", const char* colfilename = "col") {
				for (auto& o : this->m_Object) {
					if (o->GetIsBaseModel(filepath, objfilename, colfilename)) {
						this->m_Object.back()->CopyModel(o);
						return;
					}
				}
				this->m_Object.back()->LoadModel(filepath, objfilename, colfilename);
			}

			auto& GetObj(ObjType ModelType, int num) {
				int cnt = 0;
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (o->GetobjType() == ModelType) {
						if (cnt == num) {
							return this->m_Object[i];
						}
						cnt++;
					}
				}
				return this->m_Object[0];
			}
		public:
			void InitObject(const MV1* MapCol) {
				for (auto& o : this->m_Object) {
					o->Init();
					o->SetFrameNum();
					o->SetMapCol(MapCol);
				}
			}
			void ExecuteObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->Execute();
				}
				//物理アップデート
				this->m_ResetP.GetInput(CheckHitKeyWithCheck(KEY_INPUT_P) != 0);

				for (auto& o : this->m_Object) {
					if (this->m_ResetP.trigger()) { o->SetResetP(true); }
					o->ExecuteCommon();
				}
			}
			void DrawDepthObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->Depth_Draw();
				}
			}
			void DrawObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->CheckDraw();
					o->Draw();
				}
			}
			void DrawObject_Shadow(void) noexcept {
				for (auto& o : this->m_Object) {
					o->DrawShadow();
				}
			}
			void DisposeObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->Dispose();
				}
			}
			//
		};
	};
};
