#pragma once

namespace project_playground::core::interfaces {

enum class CollisionBoundsType {
    Solid,
    Containment,
};

struct CollisionBounds {
    float minX;
    float maxX;
    float minY;
    float maxY;
    CollisionBoundsType type = CollisionBoundsType::Solid;
};

class ICollisionProvider {
public:
    virtual ~ICollisionProvider() = default;

    virtual CollisionBounds GetCollisionBounds() const = 0;
};

}