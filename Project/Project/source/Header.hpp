#pragma once

#define NOMINMAX

#ifdef _DEBUG

#define DEBUG

#endif // _DEBUG

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

#include "DXLib_ref/DXLib_ref.h"
#include "Box2D/Box2D.h"

using namespace DXLib_ref;

#include "Enums.hpp"
#include "MainScene/Object/CharacterEnum.hpp"
//
#include "sub.hpp"
#include "sequence.hpp"
//
#include "MainScene/BackGround/BackGround.hpp"

#include "MainScene/Object/ObjectBase.hpp"
#include "MainScene/Object/Character.hpp"
#include "MainScene/Object/Circle.hpp"

#include "ObjectManager.hpp"

#include "MainScene/UI/MainSceneUIControl.hpp"
//
#include "Scene/MainScene.hpp"
