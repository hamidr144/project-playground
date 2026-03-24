#pragma once

#include <string_view>

#include "core/interfaces/IObject.h"
#include "cute.h"

namespace project_playground::game::objects {

class WallObject : public core::interfaces::IObject {
public:
    WallObject(float x, float y, float width, float height, CF_Color color);

    core::interfaces::CollisionBounds GetCollisionBounds() const override;
    void Render() const override;

    std::string_view GetTypeName() const;

private:
    Cute::v2 m_position;
    Cute::v2 m_size;
    CF_Color m_color;
};

}