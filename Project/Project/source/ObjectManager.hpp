#pragma once
#include	"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ObjectManager {
			std::vector<std::shared_ptr<ObjectBaseClass>>	m_Object;
			switchs											m_ResetP;
			std::shared_ptr<BackGroundClass>			m_BackGround;				//BGコピー
			//シングルトン化
#if true

		private:
			static inline  ObjectManager*	m_Singleton = nullptr;
		public:
			static void Create() {
				if (m_Singleton == nullptr) {
					m_Singleton = new ObjectManager();
				}
			}
			static ObjectManager* Instance(void) {
				if (m_Singleton == nullptr) {
					m_Singleton = new ObjectManager();
				}
				return m_Singleton;
			}
		private:

#endif
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
				case ObjType::Human:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<CharacterClass>();
					break;
				case ObjType::Vehicle:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<VehicleClass>();
					break;
				case ObjType::Ammo:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<AmmoClass>();
					break;
				case ObjType::Magazine:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<MagazineClass>();
					break;
				case ObjType::Gun:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<GunClass>();
					break;
				case ObjType::Cart:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<CartClass>();
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
				case ObjType::Human:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<CharacterClass>();
					break;
				case ObjType::Vehicle:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<VehicleClass>();
					break;
				case ObjType::Ammo:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<AmmoClass>();
					break;
				case ObjType::Magazine:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<MagazineClass>();
					break;
				case ObjType::Gun:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<GunClass>();
					break;
				case ObjType::Cart:
					this->m_Object.resize(this->m_Object.size() + 1);
					this->m_Object.back() = std::make_shared<CartClass>();
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
			void			DeleteCheck(void) {
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
			void			ExecuteObject(void) {
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
			void			LateExecuteObject(void) {
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					o->LateExecute();
				}
			}
			void			DrawDepthObject(void) {
				for (auto& o : this->m_Object) {
					o->Depth_Draw();
				}
			}
			void			DrawObject(void) {
				for (auto& o : this->m_Object) {
					o->CheckDraw();
					o->Draw();
				}
			}
			void			DrawObject_Shadow(void) {
				for (auto& o : this->m_Object) {
					o->DrawShadow();
				}
			}
			void			DisposeObject(void) {
				for (auto& o : this->m_Object) {
					o->Dispose();
				}
				this->m_Object.clear();
			}
			//
		};
	};
};
