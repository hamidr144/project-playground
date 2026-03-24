#include "game/objects/headers/MazeBoundary.h"

namespace project_playground::game::objects {

MazeBoundary::MazeBoundary(float width, float height)
    : m_halfWidth(width * 0.5f), m_halfHeight(height * 0.5f)
{
}

core::interfaces::CollisionBounds MazeBoundary::GetCollisionBounds() const
{
    return {
        -m_halfWidth,
        m_halfWidth,
        -m_halfHeight,
        m_halfHeight,
        core::interfaces::CollisionBoundsType::Containment,
    };
}

void MazeBoundary::Render() const
{
}

}
