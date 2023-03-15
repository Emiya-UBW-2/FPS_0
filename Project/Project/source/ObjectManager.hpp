#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ObjectManager :public SingletonBase<ObjectManager> {
		private:
			friend class SingletonBase<ObjectManager>;
		private:
			std::vector<std::shared_ptr<ObjectBaseClass>>	m_Object;
			switchs											m_ResetP;
			std::shared_ptr<BackGroundClass>				m_BackGround;				//BGコピー
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

			std::shared_ptr<ObjectBaseClass>* AddObject(ObjType ModelType, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") {
				switch (ModelType) {
				case ObjType::Vehicle:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<VehicleClass>();
					break;
				case ObjType::Ammo:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<AmmoClass>();
					break;
				case ObjType::HindD:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<HindDClass>();
					break;
				case ObjType::Item:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<ItemClass>();
					break;
				default:
					break;
				}
				LoadModel(this->m_Object.back().get(), filepath, objfilename, colfilename);

				auto back = this->m_Object.size();
				this->m_Object.back()->SetMapCol(this->m_BackGround);
				this->m_Object.back()->Init();

				return &(this->m_Object[back - 1]);
			}
			std::shared_ptr<ObjectBaseClass>* AddObject(ObjType ModelType) {
				switch (ModelType) {
				case ObjType::Vehicle:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<VehicleClass>();
					break;
				case ObjType::Ammo:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<AmmoClass>();
					break;
				case ObjType::HindD:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<HindDClass>();
					break;
				case ObjType::Item:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<ItemClass>();
					break;
				default:
					break;
				}
				this->m_Object.back()->SetMapCol(this->m_BackGround);
				this->m_Object.back()->Init();

				return &(this->m_Object[this->m_Object.size() - 1]);
			}
			std::shared_ptr<ObjectBaseClass>* GetObj(ObjType ModelType, int num) {
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
			void			DelObj(ObjType ModelType, int num) {
				int cnt = 0;
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (o->GetobjType() == ModelType) {
						if (cnt == num) {
							//順番の維持のためここはerase
							o->Dispose();
							this->m_Object.erase(this->m_Object.begin() + i);
							break;
						}
						cnt++;
					}
				}
			}
			//オブジェクトの排除チェック
			void			DeleteCheck(void) noexcept {
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (o->GetIsDelete()) {
						//順番の維持のためここはerase
						o->Dispose();
						this->m_Object.erase(this->m_Object.begin() + i);
						i--;
					}
				}
			}
		public:
			void			Init(const std::shared_ptr<BackGroundClass>& backGround) {
				this->m_BackGround = backGround;
			}
			void			ExecuteObject(void) noexcept {
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					o->FirstExecute();
				}
				//物理アップデート
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
			void			DrawObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->CheckDraw();
					o->Draw();
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
