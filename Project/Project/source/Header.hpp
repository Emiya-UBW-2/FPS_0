#pragma once

#define NOMINMAX

#ifdef _DEBUG

#define DEBUG

#endif // _DEBUG

//ここをコメントアウト
#define DEBUG


#include <D3D11.h>
#include <array>
#include <fstream>
#include <memory>
#include <optional>
#include <vector>
#include <fstream>
#include <cassert>
#include <filesystem>
#include <algorithm>
#include <iostream>

#include "DXLib_ref/DXLib_ref.h"
#include "Box2D/Box2D.h"

using namespace DXLib_ref;


#include "Enums.hpp"
#include "MainScene/Object/ObjectBaseEnum.hpp"
#include "MainScene/Object/CartEnum.hpp"
#include "MainScene/Object/AmmoEnum.hpp"
#include "MainScene/Object/MagazineEnum.hpp"
#include "MainScene/Object/GunEnum.hpp"
#include "MainScene/Object/CharacterEnum.hpp"
#include "MainScene/Object/VehicleEnum.hpp"
//
#include "sub.hpp"
#include "NetWork.hpp"
//
#include "MainScene/BackGround/BackGroundSub.hpp"
#include "MainScene/BackGround/BackGround.hpp"

#include "MainScene/Object/ObjectBase.hpp"
//
#include "MainScene/Object/CartData.hpp"
#include "MainScene/Object/Cart.hpp"
//
#include "MainScene/Object/AmmoData.hpp"
#include "MainScene/Object/Ammo.hpp"
//
#include "MainScene/Object/MagazineData.hpp"
#include "MainScene/Object/Magazine.hpp"
//
#include "MainScene/Object/GunData.hpp"
#include "MainScene/Object/Gun.hpp"
//
#include "MainScene/Object/CharacterData.hpp"
#include "MainScene/Object/Character.hpp"
//
#include "MainScene/Object/VehicleData.hpp"
#include "MainScene/Object/Vehicle.hpp"

#include "ObjectManager.hpp"
#include "MainScene/Player/Player.hpp"
#include "MainScene/Player/CPU.hpp"

#include "MainScene/UI/MainSceneUIControl.hpp"
//
#include "Scene/MainScene.hpp"
#include "Scene/MainSceneLoader.hpp"
