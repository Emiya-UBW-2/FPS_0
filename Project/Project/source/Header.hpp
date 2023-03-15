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
#include "MainScene/Object/AmmoEnum.hpp"
#include "MainScene/Object/VehicleEnum.hpp"
#include "MainScene/Object/HindDEnum.hpp"
#include "MainScene/Object/ItemEnum.hpp"
//
#include "sub.hpp"
#include "NetWork.hpp"
//
#include "MainScene/BackGround/BackGroundSub.hpp"
#include "MainScene/BackGround/BackGround.hpp"

#include "MainScene/Object/ObjectBase.hpp"
//
#include "MainScene/Object/AmmoData.hpp"
#include "MainScene/Object/Ammo.hpp"
//
#include "MainScene/Object/HindDData.hpp"
#include "MainScene/Object/HindD.hpp"
//
#include "MainScene/Object/VehicleData.hpp"
#include "MainScene/Object/Vehicle.hpp"
//
#include "MainScene/Object/ItemData.hpp"
#include "MainScene/Object/Item.hpp"

#include "ObjectManager.hpp"
#include "MainScene/Player/Player.hpp"
#include "MainScene/Player/CPU.hpp"

#include "MainScene/UI/MainSceneUIControl.hpp"
//
#include "MainScene/NetWorks.hpp"
#include "MainScene/Inventory.hpp"

#include "Scene/MainScene.hpp"
#include "Scene/MainSceneLoader.hpp"
