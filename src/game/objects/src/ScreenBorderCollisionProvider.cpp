#include "game/objects/headers/ScreenBorderCollisionProvider.h"

#include "cute.h"

namespace project_playground::game::objects {

core::interfaces::CollisionBounds ScreenBorderCollisionProvider::GetCollisionBounds() const
{
    const float halfWidth = static_cast<float>(Cute::app_get_canvas_width()) * 0.5f;
    const float halfHeight = static_cast<float>(Cute::app_get_canvas_height()) * 0.5f;

    return {
        -halfWidth,
        halfWidth,
        -halfHeight,
        halfHeight,
        core::interfaces::CollisionBoundsType::Containment,
    };
}

void ScreenBorderCollisionProvider::Render() const
{
}

}