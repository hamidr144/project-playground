#pragma once

#include <vector>

#include "core/interfaces/ICollisionable.h"
#include "core/interfaces/ICollisionHandler.h"
#include "core/interfaces/IEntity.h"
#include "game/objects/headers/ScreenBorderCollisionProvider.h"
#include "cute.h"
#include <string>

namespace project_playground::game::entities {

class PointEntity : public core::interfaces::IEntity,
                    public core::interfaces::ICollisionHandler {
public:
    PointEntity(float x, float y, float radius, CF_Color color);

    std::string_view GetTypeName() const override;
    core::interfaces::CollisionBounds GetCollisionBounds() const override;
    void SetCollisionProviders(
        std::vector<const core::interfaces::ICollisionable*> collisionProviders) override;
    void Update() override;
    void Render() const override;

    void SetPosition(float x, float y);
    void SetColor(CF_Color color);
    void SetSpeedScale(float scale);
    void SetFontSize(int fontSize);

private:
    std::string GetPositionText() const;
    void HandleCollisions();
    void ResolveCollisionBounds(const core::interfaces::CollisionBounds& collisionBounds);
    void ResolvePointCollision(const PointEntity& otherPoint);
    void SetResolvedVelocityX(float resolvedVelocityX);
    void SetResolvedVelocityY(float resolvedVelocityY);
    void RenderText() const;

private:
    Cute::v2 m_position;
    Cute::v2 m_driftVelocity{0.0f, 0.0f};
    Cute::v2 m_velocity{0.0f, 0.0f};
    Cute::v2 m_collisionVelocity{0.0f, 0.0f};
    float m_radius;
    CF_Color m_color;

    // Parameters for smooth random movement
    float m_phaseX = 0.0f;
    float m_phaseY = 0.0f;
    float m_freqX = 0.7f;
    float m_freqY = 1.0f;
    float m_ampX = 80.0f;
    float m_ampY = 80.0f;
    float m_phaseDriftX = 0.0f;
    float m_phaseDriftY = 0.0f;
    float m_chaosMix = 0.35f;
    float m_swirlStrength = 0.2f;
    float m_burstFrequency = 0.4f;
    float m_burstStrength = 0.8f;
    float m_smoothing = 4.0f; // higher = snappier
    float m_rebound = 0.9f;
    float m_collisionDamping = 5.5f;
    float m_collisionKick = 36.0f;
    int m_fontSize = 20;
    objects::ScreenBorderCollisionProvider m_screenBorderCollisionProvider;
    std::vector<const core::interfaces::ICollisionable*> m_collisionProviders;
};

}