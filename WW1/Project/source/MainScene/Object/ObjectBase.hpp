#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ObjectBaseClass {
		protected:
			bool										m_Use_RealTimePhysics{ false };
			MV1											m_obj_REALTIME;
			MV1											m_obj_LOADCALC;
			bool										m_objActive{ false };							//

			MV1											m_col;
			bool										m_ColActive{ false };							//

			moves										m_move;
			MATRIX_ref									m_PrevMat;//物理更新のため
			std::shared_ptr<BackGroundClass>			m_BackGround;				//BG
			std::vector<std::pair<int, MATRIX_ref>>		m_Frames;
			std::vector<std::pair<int, float>>			m_Shapes;
			ObjType										m_objType{ (ObjType)0 };
			std::string									m_FilePath;
			std::string									m_ObjFileName;
			std::string									m_ColFileName;

			bool										m_IsActive{ true };
			bool										m_IsDelete{ false };

			bool										m_IsResetPhysics{ true };
			bool										m_IsFirstLoop{ true };
			bool										m_IsDraw{ true };
			float										m_DistanceToCam{ 0.f };
			bool										m_IsBaseModel{ false };
			VECTOR_ref									m_CameraPosition;
			float										m_CameraSize{ 0.f };
			PlayerID									m_MyID{ 0 };									//

			shaders*									m_UseShader{ nullptr };
			std::array<int,2>							m_ShaderTex{ -1 , -1 };
		public:
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
			void			SetUseRealTimePhysics(bool value) noexcept { this->m_Use_RealTimePhysics = value; }
			void			SetActive(bool value) noexcept { this->m_IsActive = value; }
			void			SetIsDelete(bool value) noexcept { this->m_IsDelete = value; }
			void			SetMapCol(const std::shared_ptr<BackGroundClass>& backGround) noexcept { this->m_BackGround = backGround; }
			void			SetResetP(bool value) { this->m_IsResetPhysics = value; }
			void			SetCameraPosition(const VECTOR_ref& value) { this->m_CameraPosition = value; }
			void			SetCameraSize(float value) { this->m_CameraSize = value; }
			void			SetUseShader(shaders* value) noexcept { this->m_UseShader = value; }
			void			SetShaderTexHandle(int id, int value) noexcept { this->m_ShaderTex[id] = value; }

			auto&			GetObj(void) noexcept { return this->m_Use_RealTimePhysics ? this->m_obj_REALTIME : this->m_obj_LOADCALC; }
			const auto&		GetObj_const(void) const noexcept { return this->m_Use_RealTimePhysics ? this->m_obj_REALTIME : this->m_obj_LOADCALC; }
			const auto		GetMatrix(void) const noexcept { return this->GetObj_const().GetMatrix(); }
			const auto		GetIsBaseModel(const char* filepath, const char* objfilename, const char* colfilename) const noexcept {
				return (
					this->m_IsBaseModel &&
					(this->m_FilePath == filepath) &&
					(this->m_ObjFileName == objfilename) &&
					(this->m_ColFileName == colfilename));
			}
			const auto&		GetobjType(void) const noexcept { return this->m_objType; }
			const auto&		GetCameraPosition(void) const noexcept { return this->m_CameraPosition; }
			const auto&		GetCameraSize(void) const noexcept { return this->m_CameraSize; }
			const auto&		GetMove(void) const noexcept { return this->m_move; }
			const auto&		IsActive(void) const noexcept { return this->m_IsActive; }
			const auto&		GetIsDelete(void) const noexcept { return this->m_IsDelete; }
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			//
			void			SetAnimOnce(int ID, float speed) {
				this->GetObj().get_anime(ID).time += 30.f / FPS * speed;
				if (this->GetObj().get_anime(ID).TimeEnd()) { this->GetObj().get_anime(ID).GoEnd(); }
			}
			void			SetAnimLoop(int ID, float speed) {
				this->GetObj().get_anime(ID).time += 30.f / FPS * speed;
				if (this->GetObj().get_anime(ID).TimeEnd()) { this->GetObj().get_anime(ID).GoStart(); }
			}
			void			ResetMove(const MATRIX_ref& mat, const VECTOR_ref& pos) noexcept {
				this->m_move.vec.clear();
				SetMove(mat, pos);
			}
			void			SetMove(const MATRIX_ref& mat, const VECTOR_ref& pos) noexcept {
				this->m_move.mat = mat;
				this->m_move.pos = pos;
				UpdateMove();
			}
			void			UpdateMove(void) noexcept {
				this->m_PrevMat = this->GetObj().GetMatrix();
				this->GetObj().SetMatrix(this->m_move.MatIn());
				if (this->m_col.IsActive()) {
					this->m_col.SetMatrix(this->m_move.MatIn());
					//this->m_col.RefreshCollInfo();
				}
			}

			const auto		GetMapColNearest(const VECTOR_ref& StartPos, VECTOR_ref* EndPos) {
				bool ans = false;
				while (true) {
					auto colres = this->m_BackGround->GetGroundCol().CollCheck_Line(StartPos, *EndPos);
					if (colres.HitFlag == TRUE) {
						ans = true;
						if (*EndPos == colres.HitPosition) { break; }
						*EndPos = colres.HitPosition;
					}
					else {
						break;
					}
				}
				return ans;
			}

			//判定起動
			const auto		RefreshCol(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos, float pRange) noexcept {
				if (this->m_ColActive) { return true; }				//すでに起動しているなら無視
				if (GetMinLenSegmentToPoint(StartPos, EndPos, m_move.pos) <= pRange) {
					//判定起動
					this->m_ColActive = true;
					for (int i = 0; i < this->m_col.mesh_num(); i++) {
						this->m_col.RefreshCollInfo(-1, i);
					}
					return true;
				}
				return false;
			}
			//判定取得
			const auto		GetColLine(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos, const int sel = 0) const noexcept { return this->m_col.CollCheck_Line(StartPos, EndPos, -1, sel); }
			void			GetColNearestInAllMesh(const VECTOR_ref& StartPos, VECTOR_ref* EndPos) const noexcept {
				MV1_COLL_RESULT_POLY colres;
				for (int i = 0; i < this->m_col.mesh_num(); ++i) {
					colres = GetColLine(StartPos, *EndPos, i);
					if (colres.HitFlag == TRUE) {
						*EndPos = colres.HitPosition;
					}
				}
			}
		public:
			void			LoadModel(const char* filepath, const char* objfilename = "model", const char* colfilename = "col") noexcept {
				this->m_FilePath = filepath;
				this->m_ObjFileName = objfilename;
				this->m_ColFileName = colfilename;
				FILEINFO FileInfo;
				//model
				{
					std::string Path = this->m_FilePath;
					Path += this->m_ObjFileName;
					if (FileRead_findFirst((Path + "_REALTIME.mv1").c_str(), &FileInfo) != (DWORD_PTR)-1) {
						//MV1::Load(Path + ".pmx", &this->m_obj_REALTIME, DX_LOADMODEL_PHYSICS_REALTIME);
						MV1::Load((Path + "_REALTIME.mv1").c_str(), &this->m_obj_REALTIME, DX_LOADMODEL_PHYSICS_REALTIME);
					}
					else {
						MV1::Load(Path + ".pmx", &this->m_obj_REALTIME, DX_LOADMODEL_PHYSICS_REALTIME);
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_REALTIME);
						MV1SaveModelToMV1File(this->m_obj_REALTIME.get(), (Path + "_REALTIME.mv1").c_str());
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
					}
					if (FileRead_findFirst((Path + "_LOADCALC.mv1").c_str(), &FileInfo) != (DWORD_PTR)-1) {
						//MV1::Load(Path + ".pmx", &this->m_obj_LOADCALC, DX_LOADMODEL_PHYSICS_LOADCALC);
						MV1::Load((Path + "_LOADCALC.mv1").c_str(), &this->m_obj_LOADCALC, DX_LOADMODEL_PHYSICS_LOADCALC);
					}
					else {
						MV1::Load(Path + ".pmx", &this->m_obj_LOADCALC, DX_LOADMODEL_PHYSICS_LOADCALC);
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
						MV1SaveModelToMV1File(this->m_obj_LOADCALC.get(), (Path + "_LOADCALC.mv1").c_str());
						MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
					}
					MV1::SetAnime(&this->m_obj_REALTIME, m_obj_REALTIME);
					MV1::SetAnime(&this->m_obj_LOADCALC, m_obj_LOADCALC);
				}
				//col
				{
					std::string Path = this->m_FilePath;
					Path += this->m_ColFileName;
					if (FileRead_findFirst((Path + ".mv1").c_str(), &FileInfo) != (DWORD_PTR)-1) {
						MV1::Load(Path + ".pmx", &this->m_col, DX_LOADMODEL_PHYSICS_DISABLE);
						//MV1::Load((Path + ".mv1").c_str(), &this->m_col, DX_LOADMODEL_PHYSICS_DISABLE);
					}
					else {
						if (FileRead_findFirst((Path + ".pmx").c_str(), &FileInfo) != (DWORD_PTR)-1) {
							MV1::Load(Path + ".pmx", &this->m_col, DX_LOADMODEL_PHYSICS_DISABLE);
							MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_DISABLE);
							MV1SaveModelToMV1File(this->m_col.get(), (Path + ".mv1").c_str());
							MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
						}
					}
					if (this->m_col.IsActive()) {
						for (int i = 0; i < this->m_col.mesh_num(); ++i) { this->m_col.SetupCollInfo(8, 8, 8, -1, i); }
					}

				}
				this->m_IsBaseModel = true;
				this->m_objActive = true;
			}
			void			CopyModel(const std::shared_ptr<ObjectBaseClass>& pBase) noexcept {
				this->m_FilePath = pBase->m_FilePath;
				this->m_ObjFileName = pBase->m_ObjFileName;
				this->m_ColFileName = pBase->m_ColFileName;
				this->m_obj_REALTIME = pBase->m_obj_REALTIME.Duplicate();
				this->m_obj_LOADCALC = pBase->m_obj_LOADCALC.Duplicate();
				MV1::SetAnime(&this->m_obj_REALTIME, pBase->m_obj_REALTIME);
				MV1::SetAnime(&this->m_obj_LOADCALC, pBase->m_obj_LOADCALC);
				//col
				if (pBase->m_col.IsActive()) {
					this->m_col = pBase->m_col.Duplicate();
					if (this->m_col.IsActive()) {
						for (int i = 0; i < this->m_col.mesh_num(); ++i) { this->m_col.SetupCollInfo(8, 8, 8, -1, i); }
					}
				}
				this->m_IsBaseModel = false;
				this->m_objActive = true;
			}
			//
			virtual void	Init(void) noexcept {
				this->m_IsActive = true;
				this->m_IsResetPhysics = true;
				this->m_IsFirstLoop = true;
				this->m_IsDraw = false;
			}
			//
			void			SetFrameNum(void) noexcept {
				if (this->m_objActive) {
					int i = 0;
					bool isEnd = false;
					auto fNum = this->GetObj().frame_num();
					for (int f = 0; f < fNum; f++) {
						std::string FName = this->GetObj().frame_name(f);
						bool compare = false;
						switch (this->m_objType) {
						case ObjType::Plane:
							compare = (FName == CharaFrameName[i]);
							if (!compare) {
								//compare = (FName.find(CharaFrameName[i]) != std::string::npos);
							}
							break;
						default:
							break;
						}

						if (compare) {
							this->m_Frames.resize(this->m_Frames.size() + 1);
							this->m_Frames.back().first = f;
							this->m_Frames.back().second = MATRIX_ref::Mtrans(this->GetObj().GetFrameLocalMatrix(this->m_Frames.back().first).pos());
							i++;
							f = 0;
						}
						switch (this->m_objType) {
						case ObjType::Plane:
							if (i >= (int)CharaFrame::Max) { isEnd = true; }
							break;
						default:
							isEnd = true;
							break;
						}
						if (f == fNum - 1) {
							if (!isEnd) {
								this->m_Frames.resize(this->m_Frames.size() + 1);
								this->m_Frames.back().first = -1;
								i++;
								f = 0;
							}
						}
						switch (this->m_objType) {
						case ObjType::Plane:
							if (i >= (int)CharaFrame::Max) { isEnd = true; }
							break;
						default:
							isEnd = true;
							break;
						}
						if (isEnd) {
							break;
						}
					}
					switch (this->m_objType) {
					case ObjType::Plane:
						this->m_Shapes.resize((int)CharaShape::Max);
						for (int j = 1; j < (int)CharaShape::Max; j++) {
							auto s = MV1SearchShape(this->GetObj().get(), CharaShapeName[j]);
							if (s >= 0) {
								this->m_Shapes[j].first = s;
								this->m_Shapes[j].second = 0.f;
							}
						}
						break;
					default:
						break;
					}
				}
			}
			//
			virtual void	FirstExecute(void) noexcept { }
			void			ExecuteCommon(void) noexcept {
				if (this->m_IsFirstLoop) {
					this->m_PrevMat = this->GetObj().GetMatrix();
				}
				//シェイプ更新
				switch (this->m_objType) {
				case ObjType::Plane:
					for (int j = 1; j < (int)CharaShape::Max; j++) {
						MV1SetShapeRate(this->GetObj().get(), m_Shapes[j].first, (1.f - this->m_Shapes[0].second)*this->m_Shapes[j].second);
					}
					break;
				default:
					break;
				}
				//物理更新
				if (this->m_Use_RealTimePhysics) {
					if (this->m_IsResetPhysics) {
						this->m_IsResetPhysics = false;
						this->GetObj().PhysicsResetState();
					}
					else {
						auto NowMat = this->GetObj().GetMatrix();
						int Max = 2;
						for (int i = 0; i < Max; i++) {
							this->GetObj().SetMatrix(
								Lerp_Matrix(this->m_PrevMat.GetRot(), NowMat.GetRot(), (float)(i + 1) / (float)Max)
								* MATRIX_ref::Mtrans(Lerp(this->m_PrevMat.pos(), NowMat.pos(), (float)(i + 1) / (float)Max)));
							this->GetObj().PhysicsCalculation(1000.0f / FPS * 60.f);
						}
					}
				}
				//最初のループ終わり
				this->m_IsFirstLoop = false;
			}
			virtual void	LateExecute(void) noexcept { }
			//
			virtual void	Depth_Draw(void) noexcept { }
			virtual void	DrawShadow(void) noexcept {
				if (this->m_IsActive && this->m_IsDraw) {
					this->GetObj().DrawModel();
				}
			}
			virtual void	CheckDraw(void) noexcept {
				this->m_IsDraw = false;
				this->m_DistanceToCam = (this->GetObj().GetMatrix().pos() - GetCameraPosition()).size();
				if (CheckCameraViewClip_Box(
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-20, 0, -20)).get(),
					(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(20, 20, 20)).get()) == FALSE
					) {
					this->m_IsDraw |= true;
				}
			}
			virtual void	Draw(bool isDrawSemiTrans) noexcept {
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-20, 0, -20)).get(),
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(20, 20, 20)).get()) == FALSE
						) {
						for (int i = 0; i < this->GetObj().mesh_num(); i++) {
							if ((MV1GetMeshSemiTransState(this->GetObj().get(), i) == TRUE) == isDrawSemiTrans) {
								this->GetObj().DrawMesh(i);
							}
						}
					}
				}
			}
			//
			virtual void	Dispose(void) noexcept {
				this->m_BackGround.reset();
				this->GetObj().Dispose();
				this->m_col.Dispose();
			}
		};
	};
};
