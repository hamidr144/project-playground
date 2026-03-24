#pragma once

#include <vector>

#include "core/interfaces/ICollisionHandler.h"
#include "core/interfaces/IEntity.h"
#include "cute.h"

namespace project_playground::game::entities {

class MazeRunner : public core::interfaces::IEntity,
                   public core::interfaces::ICollisionHandler {
public:
    MazeRunner(Cute::v2 startPosition, Cute::v2 goalPosition, float radius, CF_Color color, float speed);

    std::string_view GetTypeName() const override;
    bool IsAlive() const override;
    core::interfaces::CollisionBounds GetCollisionBounds() const override;
    void SetCollisionProviders(std::vector<const core::interfaces::ICollisionable*> collisionProviders) override;
    void Update() override;
    void Render() const override;

    Cute::v2 GetVelocity() const;

private:
    void RebuildPath();
    void ResolveCollisions();
    bool IsPathNodeReached(const Cute::v2& node) const;
    bool HasReachedGoal() const;

private:
    std::vector<const core::interfaces::ICollisionable*> m_collisionProviders;
    std::vector<Cute::v2> m_navigationPath;
    Cute::v2 m_position{0.0f, 0.0f};
    Cute::v2 m_goalPosition{0.0f, 0.0f};
    Cute::v2 m_heading{0.0f, 1.0f};
    Cute::v2 m_velocity{0.0f, 0.0f};
    float m_radius;
    CF_Color m_color;
    float m_speed;
    float m_navigationStep = 24.0f;
    float m_repathInterval = 0.35f;
    float m_repathTimer = 0.0f;
    bool m_alive = true;
};

}