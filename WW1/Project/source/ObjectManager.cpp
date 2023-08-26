#include	"ObjectManager.hpp"
#include "MainScene/Object/Ammo.hpp"
#include "MainScene/Object/Character.hpp"

const FPS_n2::Sceneclass::ObjectManager* SingletonBase<FPS_n2::Sceneclass::ObjectManager>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {
		std::shared_ptr<ObjectBaseClass>*  ObjectManager::AddObject(ObjType ModelType, const char* filepath, const char* objfilename, const char* colfilename) noexcept {
			switch (ModelType) {
			case ObjType::Plane:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<CharacterClass>();
				break;
			case ObjType::Ammo:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<AmmoClass>();
				break;
			case ObjType::Movie:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<ObjectBaseClass>();
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
		std::shared_ptr<ObjectBaseClass>* ObjectManager::AddObject(ObjType ModelType) noexcept {
			switch (ModelType) {
			case ObjType::Plane:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<CharacterClass>();
				break;
			case ObjType::Ammo:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<AmmoClass>();
				break;
			case ObjType::Movie:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<ObjectBaseClass>();
				break;
			default:
				break;
			}
			this->m_Object.back()->SetMapCol(this->m_BackGround);
			this->m_Object.back()->Init();

			return &(this->m_Object[this->m_Object.size() - 1]);
		}
	};
};
