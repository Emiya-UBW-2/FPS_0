#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ObjectManager {
			std::vector<std::shared_ptr<ObjectBaseClass>> m_Object;
		public:
			void AddObject(ObjType ModelType) {
				switch (ModelType) {
				case ObjType::Human://human
					m_Object.resize(m_Object.size() + 1);
					m_Object.back() = std::make_shared<CharacterClass>();
					break;
				case ObjType::Magazine://mag
					m_Object.resize(m_Object.size() + 1);
					m_Object.back() = std::make_shared<MagazineClass>();
					break;
				case ObjType::Gun://gun
					m_Object.resize(m_Object.size() + 1);
					m_Object.back() = std::make_shared<GunClass>();
					break;
				case ObjType::Target://human
					m_Object.resize(m_Object.size() + 1);
					m_Object.back() = std::make_shared<TargetClass>();
					break;
				default:
					break;
				}
			}
			void LoadObj(const char* filepath, const char* objfilename = "model", const char* colfilename = "col") {
				m_Object.back()->LoadModel(filepath, objfilename, colfilename);
			}

			auto& GetObj(ObjType ModelType, int num) {
				int cnt = 0;
				for (int i = 0; i < m_Object.size(); i++) {
					auto& o = m_Object[i];
					if (o->GetobjType() == ModelType) {
						if (cnt == num) {
							return m_Object[i];
						}
						cnt++;
					}
				}
				return m_Object[0];
			}
		public:
			void InitObject(const MV1* MapCol) {
				for (auto& o : m_Object) {
					o->Init();
					o->SetFrameNum();
					o->SetCol(MapCol);
				}
			}
			void ExecuteObject() {
				for (auto& o : m_Object) {
					o->Execute();
				}
				for (auto& o : m_Object) {
					o->ExecutePhysics();
				}
			}
			void DrawObject() {
				for (auto& o : m_Object) {
					o->CheckDraw();
					o->Draw();
				}
			}
			void DisposeObject() {
				for (auto& o : m_Object) {
					o->Dispose();
				}
			}
			//
		};
	};
};
