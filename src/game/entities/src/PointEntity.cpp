#include "game/entities/headers/PointEntity.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>
#include <utility>

namespace {
float GetResolutionDirection(float selfCenter, float otherCenter, float velocity)
{
    if (selfCenter < otherCenter) {
        return -1.0f;
    }

    if (selfCenter > otherCenter) {
        return 1.0f;
    }

    return velocity <= 0.0f ? 1.0f : -1.0f;
}
}

namespace project_playground::game::entities {

PointEntity::PointEntity(float x, float y, float radius, CF_Color color)
    : m_position(x, y), m_velocity(0.0f, 0.0f), m_radius(radius), m_color(color)
{
    // Initialize pseudo-random parameters for smooth motion
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> phaseDist(0.0f, 2.0f * 3.14159265f);
    std::uniform_real_distribution<float> freqDist(0.3f, 1.2f);
    std::uniform_real_distribution<float> ampDist(30.0f, 200.0f); // pixels per second
    std::uniform_real_distribution<float> smoothDist(2.0f, 8.0f);
    std::uniform_real_distribution<float> driftDist(0.15f, 0.8f);
    std::uniform_real_distribution<float> chaosDist(0.25f, 0.7f);
    std::uniform_real_distribution<float> swirlDist(0.12f, 0.45f);
    std::uniform_real_distribution<float> burstFreqDist(0.2f, 0.65f);
    std::uniform_real_distribution<float> burstStrengthDist(0.5f, 1.15f);

    m_phaseX = phaseDist(gen);
    m_phaseY = phaseDist(gen);
    m_freqX = freqDist(gen);
    m_freqY = freqDist(gen);
    m_ampX = ampDist(gen);
    m_ampY = ampDist(gen);
    m_phaseDriftX = driftDist(gen);
    m_phaseDriftY = driftDist(gen);
    m_chaosMix = chaosDist(gen);
    m_swirlStrength = swirlDist(gen);
    m_burstFrequency = burstFreqDist(gen);
    m_burstStrength = burstStrengthDist(gen);
    m_smoothing = smoothDist(gen);
}

std::string_view PointEntity::GetTypeName() const
{
    return "PointEntity";
}

core::interfaces::CollisionBounds PointEntity::GetCollisionBounds() const
{
    return {
        m_position.x - m_radius,
        m_position.x + m_radius,
        m_position.y - m_radius,
        m_position.y + m_radius,
        core::interfaces::CollisionBoundsType::Solid,
    };
}

void PointEntity::SetCollisionProviders(std::vector<const core::interfaces::ICollisionProvider*> collisionProviders)
{
    m_collisionProviders = std::move(collisionProviders);
}

void PointEntity::Update()
{
    float dt = CF_DELTA_TIME;
    float time = CF_SECONDS;

    // Use layered drift for the baseline path, then add brief directional bursts.
    float phaseX = time * m_freqX + m_phaseX + std::sin(time * m_phaseDriftX + m_phaseY) * 1.1f;
    float phaseY = time * m_freqY + m_phaseY + std::cos(time * m_phaseDriftY + m_phaseX) * 1.1f;

    float driftVx =
        (std::sin(phaseX) * 0.65f + std::sin(phaseY * 0.55f + m_phaseX) * 0.35f) * m_ampX;
    float driftVy =
        (std::cos(phaseY) * 0.65f + std::cos(phaseX * 0.6f + m_phaseY) * 0.35f) * m_ampY;

    float wanderVx = std::sin(phaseX * 1.8f + std::cos(phaseY)) * (m_ampX * m_chaosMix * 0.45f);
    float wanderVy = std::cos(phaseY * 1.7f + std::sin(phaseX)) * (m_ampY * m_chaosMix * 0.45f);

    float burstPulse = std::max(0.0f, std::sin(time * m_burstFrequency + m_phaseX));
    burstPulse = burstPulse * burstPulse * burstPulse;
    float burstDirectionX = std::cos(time * (m_burstFrequency * 2.1f) + m_phaseY);
    float burstDirectionY = std::sin(time * (m_burstFrequency * 1.9f) + m_phaseX);
    float burstVx = burstDirectionX * (m_ampX * m_burstStrength * burstPulse);
    float burstVy = burstDirectionY * (m_ampY * m_burstStrength * burstPulse);

    float vx = driftVx + wanderVx + burstVx + driftVy * m_swirlStrength;
    float vy = driftVy + wanderVy + burstVy - driftVx * m_swirlStrength;

    Cute::v2 targetVel = Cute::v2(vx, vy);

    if (dt > 0.0f) {
        float t = 1.0f - std::exp(-m_smoothing * dt);
        m_velocity.x = m_velocity.x + (targetVel.x - m_velocity.x) * t;
        m_velocity.y = m_velocity.y + (targetVel.y - m_velocity.y) * t;

        m_position.x += m_velocity.x * dt;
        m_position.y += m_velocity.y * dt;

        HandleCollisions();
    }
}

void PointEntity::Render() const
{
    Cute::draw_push_color(m_color);
    Cute::draw_circle_fill(m_position, m_radius);
    Cute::draw_pop_color();

    // Optionally render position text for debugging
    // RenderText();
}

void PointEntity::SetPosition(float x, float y)
{
    m_position = Cute::v2(x, y);
}

void PointEntity::SetColor(CF_Color color)
{
    m_color = color;
}

void PointEntity::SetSpeedScale(float scale)
{
    m_ampX *= scale;
    m_ampY *= scale;
}

void PointEntity::SetFontSize(int fontSize)
{
    m_fontSize = fontSize;
}

void PointEntity::HandleCollisions()
{
    ResolveCollisionBounds(m_screenBorderCollisionProvider.GetCollisionBounds());

    for (const auto* collisionProvider : m_collisionProviders) {
        if (collisionProvider == nullptr || collisionProvider == this) {
            continue;
        }

        ResolveCollisionBounds(collisionProvider->GetCollisionBounds());
    }
}

void PointEntity::ResolveCollisionBounds(const core::interfaces::CollisionBounds& collisionBounds)
{
    if (collisionBounds.type == core::interfaces::CollisionBoundsType::Containment) {
        const float minX = collisionBounds.minX + m_radius;
        const float maxX = collisionBounds.maxX - m_radius;
        const float minY = collisionBounds.minY + m_radius;
        const float maxY = collisionBounds.maxY - m_radius;

        if (m_position.x < minX) {
            m_position.x = minX;
            m_velocity.x = -m_velocity.x * m_rebound;
        } else if (m_position.x > maxX) {
            m_position.x = maxX;
            m_velocity.x = -m_velocity.x * m_rebound;
        }

        if (m_position.y < minY) {
            m_position.y = minY;
            m_velocity.y = -m_velocity.y * m_rebound;
        } else if (m_position.y > maxY) {
            m_position.y = maxY;
            m_velocity.y = -m_velocity.y * m_rebound;
        }

        return;
    }

    const core::interfaces::CollisionBounds selfBounds = GetCollisionBounds();
    const float overlapX = std::min(selfBounds.maxX, collisionBounds.maxX) - std::max(selfBounds.minX, collisionBounds.minX);
    const float overlapY = std::min(selfBounds.maxY, collisionBounds.maxY) - std::max(selfBounds.minY, collisionBounds.minY);

    if (overlapX <= 0.0f || overlapY <= 0.0f) {
        return;
    }

    const float selfCenterX = (selfBounds.minX + selfBounds.maxX) * 0.5f;
    const float selfCenterY = (selfBounds.minY + selfBounds.maxY) * 0.5f;
    const float otherCenterX = (collisionBounds.minX + collisionBounds.maxX) * 0.5f;
    const float otherCenterY = (collisionBounds.minY + collisionBounds.maxY) * 0.5f;

    if (overlapX < overlapY) {
        const float direction = GetResolutionDirection(selfCenterX, otherCenterX, m_velocity.x);
        m_position.x += direction * overlapX;
        m_velocity.x = -m_velocity.x * m_rebound;
        return;
    }

    const float direction = GetResolutionDirection(selfCenterY, otherCenterY, m_velocity.y);
    m_position.y += direction * overlapY;
    m_velocity.y = -m_velocity.y * m_rebound;
}

std::string PointEntity::GetPositionText() const
{
    std::ostringstream oss;
    oss << "(" << static_cast<int>(m_position.x) << ", " << static_cast<int>(m_position.y) << ")";
    return oss.str();
}

void PointEntity::RenderText() const
{
    std::string text = GetPositionText();

    Cute::push_font_size(m_fontSize);
    auto textSize = Cute::text_size(text.c_str());

    auto textPos = V2(m_position.x - textSize.x * 0.5f, m_position.y + textSize.y + 10.0f);
    Cute::draw_push_color(m_color);
    Cute::draw_text(text.c_str(), textPos);
    Cute::draw_pop_color();
    Cute::pop_font_size();
}

}
