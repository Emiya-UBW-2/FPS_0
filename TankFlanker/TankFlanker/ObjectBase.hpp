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
			std::vector<std::pair<int, moves>> Frames;
			std::vector< std::pair<int, float>> Shapes;
			ObjType m_objType;
			std::string m_FilePath;
		public:
			void LoadModel(const char* filepath) {
				this->m_FilePath = filepath;
				FILEINFO FileInfo;
				//model
				{
					std::string Path = this->m_FilePath;
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
					std::string Path = this->m_FilePath;
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
			//
			virtual void Init() {

			}
			void SetFrameNum() {
				int i = 0;
				for (int f = 0; f < this->obj.frame_num(); f++) {
					std::string FName = this->obj.frame_name(f);
					bool compare = false;
					switch (m_objType) {
					case ObjType::Human://human
						if (i == (int)CharaFrame::Max) { break; }
						compare = (FName == CharaFrameName[i]);
						break;
					default:
						break;
					}

					if (compare) {
						this->Frames.resize(this->Frames.size() + 1);
						this->Frames.back().first = f;
						this->Frames.back().second.pos = this->obj.GetFrameLocalMatrix(this->Frames.back().first).pos();
						i++;
						f = 0;
					}
				}

				switch (m_objType) {
				case ObjType::Human://human
					Shapes.resize((int)CharaShape::Max);
					for (int j = 1; j < (int)CharaShape::Max; j++) {
						auto s = MV1SearchShape(this->obj.get(),CharaShapeName[j]);
						if (s >= 0) {
							Shapes[j].first = s;
							Shapes[j].second = 0.f;
						}
					}

					break;
				default:
					break;
				}


			}
			//
			virtual void Execute() {
			}
			virtual void Draw() {
				this->obj.DrawModel();
			}
			//
			virtual void Dispose() {
				this->obj.Dispose();
			}
		public:
			const auto& GetobjType() { return this->m_objType; }
			const auto GetMatrix() { return this->obj.GetMatrix(); }
			const auto* GetCol() { return &this->col; }

			void SetMove(float Yrad, const VECTOR_ref& pos) {
				this->move.mat = MATRIX_ref::RotY(Yrad);
				this->move.pos = pos;
				this->move.vec.clear();
				this->obj.SetMatrix(this->move.MatIn());
				if (this->col.IsActive()) {
					this->col.SetMatrix(this->move.MatIn());
					this->col.RefreshCollInfo();
				}
			}

			void SetShape(CharaShape pShape, float Per) {
				if (m_objType == ObjType::Human) {
					Shapes[(int)pShape].second = Per;
				}
			}

			void ExecuteShape() {
				switch (m_objType) {
				case ObjType::Human://human
					for (int j = 1; j < (int)CharaShape::Max; j++) {
						MV1SetShapeRate(this->obj.get(), Shapes[j].first, (1.f - Shapes[0].second)*Shapes[j].second);
					}
					break;
				default:
					break;
				}
			}
		};
	};
};
