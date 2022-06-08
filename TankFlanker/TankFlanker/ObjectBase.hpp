
#pragma once

#include"Header.hpp"
namespace FPS_n2 {
	namespace Sceneclass {
		class ObjectBaseClass {
		protected:
			MV1 obj;
			moves move;
			const MV1* m_MapCol{ nullptr };
		public:
			void LoadModel(const char* filepath) {
				FILEINFO FileInfo;
				std::string Path = filepath;
				if (FileRead_findFirst((Path + ".mv1").c_str(), &FileInfo) != (DWORD_PTR)-1) {
					//MV1::Load(Path + ".pmx", &this->obj, DX_LOADMODEL_PHYSICS_REALTIME);
					MV1::Load((Path + ".mv1").c_str(), &this->obj, DX_LOADMODEL_PHYSICS_REALTIME);
				}
				else {
					MV1::Load(Path + ".pmx", &this->obj, DX_LOADMODEL_PHYSICS_REALTIME);
					MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
					MV1SaveModelToMV1File(this->obj.get(), (Path + ".mv1").c_str());
					MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
				}
				MV1::SetAnime(&this->obj, this->obj);
			}
			void SetCol(const MV1* MapCol) {
				this->m_MapCol = MapCol;
			}

			virtual void Init() {

			}

			virtual void Draw() {
				this->obj.DrawModel();
			}
			virtual void Dispose() {
				this->obj.Dispose();
			}
		public:
			const auto GetMatrix() { return this->obj.GetMatrix(); }
		};
	};
};
