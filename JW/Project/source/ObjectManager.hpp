#pragma once
#include	"Header.hpp"
#include "MainScene/Object/ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ObjectManager : public SingletonBase<ObjectManager> {
		private:
			friend class SingletonBase<ObjectManager>;
		private:
			std::vector<std::shared_ptr<ObjectBaseClass>>	m_Object;
			switchs											m_ResetP;
			std::shared_ptr<BackGroundClassBase>				m_BackGround;				//BG�R�s�[
		public:
			void	LoadModel(ObjectBaseClass* pObj, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") const noexcept {
				bool iscopy = false;
				for (auto& o : this->m_Object) {
					if (o->GetIsBaseModel(filepath, objfilename, colfilename)) {
						pObj->CopyModel(o);
						iscopy = true;
						break;
					}
				}
				if (!iscopy) {
					pObj->LoadModel(filepath, objfilename, colfilename);
				}
				pObj->SetFrameNum();
			}

			std::shared_ptr<ObjectBaseClass>* AddObject(ObjType ModelType, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") noexcept;
			std::shared_ptr<ObjectBaseClass>* AddObject(ObjType ModelType) noexcept;
			std::shared_ptr<ObjectBaseClass>* GetObj(ObjType ModelType, int num) noexcept {
				int cnt = 0;
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (o->GetobjType() == ModelType) {
						if (cnt == num) {
							return &this->m_Object[i];
						}
						cnt++;
					}
				}
				return nullptr;
			}
			void			DelObj(ObjType ModelType, int num) noexcept {
				int cnt = 0;
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (o->GetobjType() == ModelType) {
						if (cnt == num) {
							//���Ԃ̈ێ��̂��߂�����erase
							o->Dispose();
							this->m_Object.erase(this->m_Object.begin() + i);
							break;
						}
						cnt++;
					}
				}
			}
			//�I�u�W�F�N�g�̔r���`�F�b�N
			void			DeleteCheck(void) noexcept {
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (o->GetIsDelete()) {
						//���Ԃ̈ێ��̂��߂�����erase
						o->Dispose();
						this->m_Object.erase(this->m_Object.begin() + i);
						i--;
					}
				}
			}
		public:
			void			Init(const std::shared_ptr<BackGroundClassBase>& backGround) noexcept {
				this->m_BackGround = backGround;
			}
			void			ExecuteObject(void) noexcept {
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					o->FirstExecute();
				}
				//�����A�b�v�f�[�g
				this->m_ResetP.Execute(CheckHitKeyWithCheck(KEY_INPUT_P) != 0);

				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (this->m_ResetP.trigger()) { o->SetResetP(true); }
					o->ExecuteCommon();
				}
			}
			void			LateExecuteObject(void) noexcept {
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					o->LateExecute();
				}
			}
			void			DrawDepthObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->Depth_Draw();
				}
			}
			void			DrawObject() noexcept {
				for (auto& o : this->m_Object) {
					o->CheckDraw();
					o->Draw(false);
				}
				for (auto& o : this->m_Object) {
					o->Draw(true);
				}
			}
			void			DrawObject_Shadow(void) noexcept {
				for (auto& o : this->m_Object) {
					o->DrawShadow();
				}
			}
			void			DisposeObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->Dispose();
				}
				this->m_Object.clear();
			}
			//
		};
	};
};
