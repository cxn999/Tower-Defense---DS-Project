#pragma once

#include <memory>
#include "Vec2.h"
#include "Entity.h"

Vec2 GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
Vec2 GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);