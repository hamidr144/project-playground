#include "game/objects/headers/WallObject.h"

namespace project_playground::game::objects {

WallObject::WallObject(float x, float y, float width, float height, CF_Color color)
    : m_position(x, y), m_size(width, height), m_color(color)
{
}

std::string_view WallObject::GetTypeName() const
{
    return "WallObject";
}

core::interfaces::CollisionBounds WallObject::GetCollisionBounds() const
{
    const float halfWidth = m_size.x * 0.5f;
    const float halfHeight = m_size.y * 0.5f;

    return {
        m_position.x - halfWidth,
        m_position.x + halfWidth,
        m_position.y - halfHeight,
        m_position.y + halfHeight,
        core::interfaces::CollisionBoundsType::Solid,
    };
}

void WallObject::Render() const
{
    Cute::draw_push_color(m_color);
    Cute::draw_box_fill(m_position, m_size.x, m_size.y, 0.0f);
    Cute::draw_pop_color();
}

}