#pragma once

#include "core/interfaces/IObject.h"

namespace project_playground::game::objects {

class MazeBoundary : public core::interfaces::IObject {
public:
    MazeBoundary(float width, float height);

    core::interfaces::CollisionBounds GetCollisionBounds() const override;
    void Render() const override;

private:
    float m_halfWidth;
    float m_halfHeight;
};

}
