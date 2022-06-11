#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ObjectBaseClass {
		protected:
			MV1 obj;
			MV1 col;
			moves move;
			const MV1* m_MapCol{ nullptr };
		public:
			void LoadModel(const char* filepath) {
				FILEINFO FileInfo;
				//model
				{
					std::string Path = filepath;
					Path += "model";
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
				//col
				{
					std::string Path = filepath;
					Path += "col";
					if (FileRead_findFirst((Path + ".mv1").c_str(), &FileInfo) != (DWORD_PTR)-1) {
						MV1::Load(Path + ".pmx", &this->col, DX_LOADMODEL_PHYSICS_REALTIME);
						//MV1::Load((Path + ".mv1").c_str(), &this->col, DX_LOADMODEL_PHYSICS_REALTIME);
					}
					else {
						if (FileRead_findFirst((Path + ".pmx").c_str(), &FileInfo) != (DWORD_PTR)-1) {
							MV1::Load(Path + ".pmx", &this->col, DX_LOADMODEL_PHYSICS_REALTIME);
							MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
							MV1SaveModelToMV1File(this->col.get(), (Path + ".mv1").c_str());
							MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
						}
						else {
						}
					}

					this->col.SetupCollInfo(1, 1, 1);
				}
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

			const auto* GetCol() { return &this->col; }
		};
	};
};
