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
			std::vector<std::pair<int, MATRIX_ref>> Frames;
			std::vector< std::pair<int, float>> Shapes;
			ObjType m_objType{ ObjType::Human };
			std::string m_FilePath;
			std::string m_ObjFileName;
			std::string m_ColFileName;
			bool m_IsActive{ true };
			bool m_IsResetPhysics{ true };
			bool m_IsFirstLoop{ true };
			bool m_IsDraw{ true };
			float m_DistanceToCam{ 0.f };
			bool m_IsBaseModel{ false };
		public:
			void SetActive(bool value) noexcept { this->m_IsActive = value; }
			void SetMapCol(const MV1* MapCol) noexcept { this->m_MapCol = MapCol; }
			void SetResetP(bool value) { this->m_IsResetPhysics = value; }
			void SetShape(CharaShape pShape, float Per) noexcept { if (this->m_objType == ObjType::Human) { Shapes[(int)pShape].second = Per; } }

			const auto GetFrameLocalMat(CharaFrame frame) const noexcept { return this->m_obj.GetFrameLocalMatrix(Frames[(int)frame].first); }
			const auto GetFrameLocalMat(GunFrame frame) const noexcept { return this->m_obj.GetFrameLocalMatrix(Frames[(int)frame].first); }
			const auto GetParentFrameLocalMat(CharaFrame frame) const noexcept { return this->m_obj.GetFrameLocalMatrix(this->m_obj.frame_parent(Frames[(int)frame].first)); }
			const auto GetParentFrameLocalMat(GunFrame frame) const noexcept { return this->m_obj.GetFrameLocalMatrix(this->m_obj.frame_parent(Frames[(int)frame].first)); }

			const auto GetFrameWorldMat(CharaFrame frame) const noexcept { return this->m_obj.GetFrameLocalWorldMatrix(Frames[(int)frame].first); }
			const auto GetFrameWorldMat(GunFrame frame) const noexcept { return this->m_obj.GetFrameLocalWorldMatrix(Frames[(int)frame].first); }
			const auto GetParentFrameWorldMat(CharaFrame frame) const noexcept { return this->m_obj.GetFrameLocalWorldMatrix(this->m_obj.frame_parent(Frames[(int)frame].first)); }
			const auto GetParentFrameWorldMat(GunFrame frame) const noexcept { return this->m_obj.GetFrameLocalWorldMatrix(this->m_obj.frame_parent(Frames[(int)frame].first)); }

			void ResetFrameLocalMat(CharaFrame frame) noexcept { this->m_obj.frame_Reset(Frames[(int)frame].first); }
			void ResetFrameLocalMat(GunFrame frame) noexcept { this->m_obj.frame_Reset(Frames[(int)frame].first); }

			void SetFrameLocalMat(CharaFrame frame, const MATRIX_ref&value) noexcept { this->m_obj.SetFrameLocalMatrix(Frames[(int)frame].first, value * Frames[(int)frame].second); }
			void SetFrameLocalMat(GunFrame frame, const MATRIX_ref&value) noexcept { this->m_obj.SetFrameLocalMatrix(Frames[(int)frame].first, value * Frames[(int)frame].second); }

			const auto GetMatrix(void) const noexcept { return this->m_obj.GetMatrix(); }
			const auto& GetobjType(void) const noexcept { return this->m_objType; }
			const auto* GetCol(void) const noexcept { return &this->col; }

			//const auto GetAnime(CharaAnimeID anim) noexcept { return this->m_obj.get_anime((int)anim); }
			const auto GetAnime(GunAnimeID anim) noexcept { return this->m_obj.get_anime((int)anim); }

			void SetAnimOnce(int ID, float speed) {
				this->m_obj.get_anime(ID).time += 30.f / FPS * speed;
				if (this->m_obj.get_anime(ID).TimeEnd()) { this->m_obj.get_anime(ID).GoEnd(); }
			}
			void SetAnimLoop(int ID, float speed) {
				this->m_obj.get_anime(ID).time += 30.f / FPS * speed;
				if (this->m_obj.get_anime(ID).TimeEnd()) { this->m_obj.get_anime(ID).GoStart(); }
			}
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
				this->m_IsBaseModel = true;
			}
			void CopyModel(std::shared_ptr<ObjectBaseClass>& pBase) noexcept {
				this->m_FilePath = pBase->m_FilePath;
				this->m_ObjFileName = pBase->m_ObjFileName;
				this->m_ColFileName = pBase->m_ColFileName;
				this->m_obj = pBase->m_obj.Duplicate();
				MV1::SetAnime(&this->m_obj, pBase->m_obj);
				//col
				if(pBase->col.IsActive()){
					this->col = pBase->col.Duplicate();
					this->col.SetupCollInfo(1, 1, 1);
				}
				this->m_IsBaseModel = false;
			}
			const auto GetIsBaseModel(const char* filepath, const char* objfilename, const char* colfilename) const noexcept {
				return (
					this->m_IsBaseModel && 
					(this->m_FilePath == filepath) && 
					(this->m_ObjFileName == objfilename) &&
					(this->m_ColFileName == colfilename) );
			}
			//
			virtual void Init(void) noexcept {
				this->m_IsActive = true;
				this->m_IsResetPhysics = true;
				this->m_IsFirstLoop = true;
				this->m_IsDraw = false;
			}
			//
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
					case ObjType::Gun://human
						if (i == (int)GunFrame::Max) { break; }
						compare = (FName == GunFrameName[i]);
						break;
					default:
						break;
					}

					if (compare) {
						this->Frames.resize(this->Frames.size() + 1);
						this->Frames.back().first = f;
						this->Frames.back().second = MATRIX_ref::Mtrans(this->m_obj.GetFrameLocalMatrix(this->Frames.back().first).pos());
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
			virtual void Execute(void) noexcept { }
			void ExecuteCommon(void) noexcept {
				//シェイプ更新
				switch (this->m_objType) {
				case ObjType::Human://human
					for (int j = 1; j < (int)CharaShape::Max; j++) {
						MV1SetShapeRate(this->m_obj.get(), Shapes[j].first, (1.f - Shapes[0].second)*Shapes[j].second);
					}
					break;
				default:
					break;
				}
				//物理更新
				if (this->m_IsResetPhysics) {
					this->m_IsResetPhysics = false;
					this->m_obj.PhysicsResetState();
				}
				else {
					this->m_obj.PhysicsCalculation(1000.0f / FPS * 240.f);
				}
				//最初のループ終わり
				this->m_IsFirstLoop = false;
			}
			//
			virtual void Depth_Draw(void) noexcept { }
			virtual void DrawShadow(void) noexcept {
				if (this->m_IsActive) {
					this->m_obj.DrawModel();
				}
			}
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
			virtual void Draw(void) noexcept {
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(m_obj.GetMatrix().pos() + VECTOR_ref::vget(-20, 0, -20)).get(),
						(m_obj.GetMatrix().pos() + VECTOR_ref::vget(20, 20, 20)).get()) == FALSE
						) {
						this->m_obj.DrawModel();
					}
				}
			}
			//
			virtual void Dispose(void) noexcept {
				this->m_obj.Dispose();
			}
		public:
			void SetMove(const MATRIX_ref& mat, const VECTOR_ref& pos) noexcept {
				this->m_move.mat = mat;
				this->m_move.pos = pos;
				UpdateMove();
			}
			void UpdateMove(void) noexcept {
				this->m_obj.SetMatrix(this->m_move.MatIn());
				if (this->col.IsActive()) {
					this->col.SetMatrix(this->m_move.MatIn());
					this->col.RefreshCollInfo();
				}
			}
			//
		};
	};
};
