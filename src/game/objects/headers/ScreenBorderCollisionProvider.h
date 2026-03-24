#pragma once

#include "core/interfaces/IObject.h"

namespace project_playground::game::objects {

class ScreenBorderCollisionProvider : public core::interfaces::IObject {
public:
    core::interfaces::CollisionBounds GetCollisionBounds() const override;
    void Render() const override;
};

}