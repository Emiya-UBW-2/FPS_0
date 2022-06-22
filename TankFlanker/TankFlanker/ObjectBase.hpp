#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ObjectBaseClass {
		protected:
			MV1 m_obj;
			MV1 col;
			moves m_move;
			const MV1* m_MapCol{ nullptr };
			std::vector<std::pair<int, moves>> Frames;
			std::vector< std::pair<int, float>> Shapes;
			ObjType m_objType;
			std::string m_FilePath;
			std::string m_ObjFileName;
			std::string m_ColFileName;

			bool m_SetReset{ true };

			bool m_IsDraw{ true };
			float m_DistanceToCam{ 0.f };
		public:
			void LoadModel(const char* filepath, const char* objfilename = "model", const char* colfilename = "col") noexcept {
				this->m_FilePath = filepath;
				this->m_ObjFileName = objfilename;
				this->m_ColFileName = colfilename;
				FILEINFO FileInfo;
				//model
				{
					std::string Path = this->m_FilePath;
					Path += this->m_ObjFileName;
					if (FileRead_findFirst((Path + ".mv1").c_str(), &FileInfo) != (DWORD_PTR)-1) {
						//MV1::Load(Path + ".pmx", &this->m_obj, DX_LOADMODEL_PHYSICS_REALTIME);
						MV1::Load((Path + ".mv1").c_str(), &this->m_obj, DX_LOADMODEL_PHYSICS_REALTIME);
					}
					else {
						MV1::Load(Path + ".pmx", &this->m_obj, DX_LOADMODEL_PHYSICS_REALTIME);
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
						MV1SaveModelToMV1File(this->m_obj.get(), (Path + ".mv1").c_str());
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
					}
					MV1::SetAnime(&this->m_obj, this->m_obj);
				}
				//col
				{
					std::string Path = this->m_FilePath;
					Path += this->m_ColFileName;
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
			void SetCol(const MV1* MapCol) noexcept {
				this->m_MapCol = MapCol;
			}
			//
			virtual void Init(void) noexcept {

			}
			void SetFrameNum(void) noexcept {
				int i = 0;
				for (int f = 0; f < this->m_obj.frame_num(); f++) {
					std::string FName = this->m_obj.frame_name(f);
					bool compare = false;
					switch (this->m_objType) {
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
						this->Frames.back().second.pos = this->m_obj.GetFrameLocalMatrix(this->Frames.back().first).pos();
						i++;
						f = 0;
					}
				}
				switch (this->m_objType) {
				case ObjType::Human://human
					Shapes.resize((int)CharaShape::Max);
					for (int j = 1; j < (int)CharaShape::Max; j++) {
						auto s = MV1SearchShape(this->m_obj.get(), CharaShapeName[j]);
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
			virtual void Execute(void) noexcept {

			}
			virtual void Depth_Draw(void) noexcept {
			}
			virtual void Draw(void) noexcept {
				if (this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(m_obj.GetMatrix().pos() + VECTOR_ref::vget(-20, 0, -20)).get(),
						(m_obj.GetMatrix().pos() + VECTOR_ref::vget(20, 20, 20)).get()) == FALSE
						) {
						this->m_obj.DrawModel();
					}
				}
			}
			virtual void DrawShadow(void) noexcept {
				this->m_obj.DrawModel();
			}
			//
			virtual void Dispose(void) noexcept {
				this->m_obj.Dispose();
			}
		public:
			void SetResetP(bool value) { this->m_SetReset = value; }
			const auto& GetobjType(void) noexcept { return this->m_objType; }
			const auto GetMatrix(void) noexcept { return this->m_obj.GetMatrix(); }
			const auto* GetCol(void) noexcept { return &this->col; }
			void SetMove(float Yrad, const VECTOR_ref& pos) noexcept {
				this->m_move.mat = MATRIX_ref::RotY(Yrad);
				this->m_move.pos = pos;
				this->m_move.vec.clear();
				this->m_obj.SetMatrix(this->m_move.MatIn());
				if (this->col.IsActive()) {
					this->col.SetMatrix(this->m_move.MatIn());
					this->col.RefreshCollInfo();
				}
			}
			void SetShape(CharaShape pShape, float Per) noexcept {
				if (this->m_objType == ObjType::Human) {
					Shapes[(int)pShape].second = Per;
				}
			}
			//
			void ExecuteShape(void) noexcept {
				switch (this->m_objType) {
				case ObjType::Human://human
					for (int j = 1; j < (int)CharaShape::Max; j++) {
						MV1SetShapeRate(this->m_obj.get(), Shapes[j].first, (1.f - Shapes[0].second)*Shapes[j].second);
					}
					break;
				default:
					break;
				}
			}
			void ExecutePhysics(void) noexcept {
				{
					if (this->m_SetReset) {
						this->m_SetReset = false;
						this->m_obj.PhysicsResetState();
					}
					else {
						//if (this->m_DistanceToCam <= 10.f*12.5f) {
							this->m_obj.PhysicsCalculation(1000.0f / FPS * 240.f);
						//}
					}
				}
			}
			//
			void CheckDraw(void) noexcept {
				this->m_IsDraw = false;
				this->m_DistanceToCam = (m_obj.GetMatrix().pos() - GetCameraPosition()).size();
				if (CheckCameraViewClip_Box(
					(m_obj.GetMatrix().pos() + VECTOR_ref::vget(-20, 0, -20)).get(),
					(m_obj.GetMatrix().pos() + VECTOR_ref::vget(20, 20, 20)).get()) == FALSE
					) {
					this->m_IsDraw |= true;
				}
			}
		};
	};
};
