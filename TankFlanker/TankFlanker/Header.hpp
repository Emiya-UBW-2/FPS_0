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

#include "Enums.hpp"
#include "sub.hpp"
#include "sequence.hpp"
//
#include "BackGround.hpp"

#include "ObjectBase.hpp"
#include "Cart.hpp"
#include "Magazine.hpp"
#include "Gun.hpp"
#include "Target.hpp"
#include "Character.hpp"
#include "Circle.hpp"
#include "Gate.hpp"
#include "ShootingMat.hpp"

#include "ObjectManager.hpp"
//
#include "MainScene.hpp"
