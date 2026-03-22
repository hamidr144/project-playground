#pragma once

#include "core/interfaces/ICollisionProvider.h"

namespace project_playground::game::entities {

class ScreenBorderCollisionProvider : public core::interfaces::ICollisionProvider {
public:
    core::interfaces::CollisionBounds GetCollisionBounds() const override;
};

}