#include "game/entities/headers/MazeRunner.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <utility>

namespace {

using CollisionBounds = project_playground::core::interfaces::CollisionBounds;
using CollisionBoundsType = project_playground::core::interfaces::CollisionBoundsType;
using ICollisionable = project_playground::core::interfaces::ICollisionable;

struct NavigationGrid {
    float minX = 0.0f;
    float maxX = 0.0f;
    float minY = 0.0f;
    float maxY = 0.0f;
    float step = 24.0f;
    int columnCount = 0;
    int rowCount = 0;
    std::vector<unsigned char> walkable;

    int GetIndex(int column, int row) const
    {
        return row * columnCount + column;
    }

    Cute::v2 GetPosition(int index) const
    {
        const int column = index % columnCount;
        const int row = index / columnCount;
        return Cute::v2(minX + static_cast<float>(column) * step, minY + static_cast<float>(row) * step);
    }
};

bool CircleIntersectsBounds(const Cute::v2& center, float radius, const CollisionBounds& bounds)
{
    const float closestX = std::clamp(center.x, bounds.minX, bounds.maxX);
    const float closestY = std::clamp(center.y, bounds.minY, bounds.maxY);
    const float deltaX = center.x - closestX;
    const float deltaY = center.y - closestY;
    return deltaX * deltaX + deltaY * deltaY < radius * radius;
}

bool IsCircleWalkable(
    const Cute::v2& center,
    float radius,
    const std::vector<const ICollisionable*>& collisionProviders)
{
    for (const ICollisionable* collisionProvider : collisionProviders) {
        if (collisionProvider == nullptr) {
            continue;
        }

        const CollisionBounds bounds = collisionProvider->GetCollisionBounds();
        if (bounds.type == CollisionBoundsType::Containment) {
            if (center.x - radius < bounds.minX || center.x + radius > bounds.maxX ||
                center.y - radius < bounds.minY || center.y + radius > bounds.maxY) {
                return false;
            }

            continue;
        }

        if (CircleIntersectsBounds(center, radius, bounds)) {
            return false;
        }
    }

    return true;
}

NavigationGrid BuildNavigationGrid(
    const Cute::v2& position,
    const Cute::v2& goalPosition,
    float radius,
    float step,
    const std::vector<const ICollisionable*>& collisionProviders)
{
    NavigationGrid grid;
    grid.step = step;

    bool hasContainmentBounds = false;
    for (const ICollisionable* collisionProvider : collisionProviders) {
        if (collisionProvider == nullptr) {
            continue;
        }

        const CollisionBounds bounds = collisionProvider->GetCollisionBounds();
        if (bounds.type != CollisionBoundsType::Containment) {
            continue;
        }

        grid.minX = bounds.minX + radius;
        grid.maxX = bounds.maxX - radius;
        grid.minY = bounds.minY + radius;
        grid.maxY = bounds.maxY - radius;
        hasContainmentBounds = true;
        break;
    }

    if (!hasContainmentBounds) {
        const float padding = step * 4.0f;
        grid.minX = std::min(position.x, goalPosition.x) - padding;
        grid.maxX = std::max(position.x, goalPosition.x) + padding;
        grid.minY = std::min(position.y, goalPosition.y) - padding;
        grid.maxY = std::max(position.y, goalPosition.y) + padding;
    }

    grid.columnCount = std::max(2, static_cast<int>(std::floor((grid.maxX - grid.minX) / step)) + 1);
    grid.rowCount = std::max(2, static_cast<int>(std::floor((grid.maxY - grid.minY) / step)) + 1);
    grid.walkable.assign(static_cast<size_t>(grid.columnCount * grid.rowCount), 0);

    for (int row = 0; row < grid.rowCount; ++row) {
        for (int column = 0; column < grid.columnCount; ++column) {
            const Cute::v2 samplePosition(
                grid.minX + static_cast<float>(column) * step,
                grid.minY + static_cast<float>(row) * step);
            if (IsCircleWalkable(samplePosition, radius, collisionProviders)) {
                grid.walkable[static_cast<size_t>(grid.GetIndex(column, row))] = 1;
            }
        }
    }

    return grid;
}

int FindClosestWalkableNode(const NavigationGrid& grid, const Cute::v2& target)
{
    int bestIndex = -1;
    float bestDistanceSquared = std::numeric_limits<float>::max();

    for (int index = 0; index < static_cast<int>(grid.walkable.size()); ++index) {
        if (grid.walkable[static_cast<size_t>(index)] == 0) {
            continue;
        }

        const Cute::v2 samplePosition = grid.GetPosition(index);
        const float deltaX = samplePosition.x - target.x;
        const float deltaY = samplePosition.y - target.y;
        const float distanceSquared = deltaX * deltaX + deltaY * deltaY;
        if (distanceSquared < bestDistanceSquared) {
            bestDistanceSquared = distanceSquared;
            bestIndex = index;
        }
    }

    return bestIndex;
}

std::vector<Cute::v2> BuildPath(
    const NavigationGrid& grid,
    int startIndex,
    int goalIndex)
{
    if (startIndex < 0 || goalIndex < 0) {
        return {};
    }

    struct OpenNode {
        float score;
        int index;

        bool operator<(const OpenNode& other) const
        {
            return score > other.score;
        }
    };

    const int nodeCount = static_cast<int>(grid.walkable.size());
    std::vector<float> costToStart(static_cast<size_t>(nodeCount), std::numeric_limits<float>::max());
    std::vector<int> previousIndex(static_cast<size_t>(nodeCount), -1);
    std::vector<unsigned char> isClosed(static_cast<size_t>(nodeCount), 0);
    std::priority_queue<OpenNode> openSet;

    auto heuristic = [&](int fromIndex) {
        const Cute::v2 from = grid.GetPosition(fromIndex);
        const Cute::v2 goal = grid.GetPosition(goalIndex);
        return std::abs(from.x - goal.x) + std::abs(from.y - goal.y);
    };

    costToStart[static_cast<size_t>(startIndex)] = 0.0f;
    openSet.push(OpenNode{heuristic(startIndex), startIndex});

    constexpr int kNeighborColumns[4] = {1, -1, 0, 0};
    constexpr int kNeighborRows[4] = {0, 0, 1, -1};

    while (!openSet.empty()) {
        const int currentIndex = openSet.top().index;
        openSet.pop();

        if (isClosed[static_cast<size_t>(currentIndex)] != 0) {
            continue;
        }

        if (currentIndex == goalIndex) {
            std::vector<Cute::v2> path;
            for (int index = goalIndex; index >= 0; index = previousIndex[static_cast<size_t>(index)]) {
                path.push_back(grid.GetPosition(index));
                if (index == startIndex) {
                    break;
                }
            }

            std::reverse(path.begin(), path.end());
            if (path.size() <= 2) {
                return path;
            }

            std::vector<Cute::v2> simplifiedPath;
            simplifiedPath.push_back(path.front());
            Cute::v2 previousDirection = Cute::v2(0.0f, 0.0f);

            for (size_t index = 1; index < path.size(); ++index) {
                const Cute::v2 segment = path[index] - path[index - 1];
                const Cute::v2 direction(
                    segment.x == 0.0f ? 0.0f : segment.x / std::abs(segment.x),
                    segment.y == 0.0f ? 0.0f : segment.y / std::abs(segment.y));

                if (index == 1) {
                    previousDirection = direction;
                    continue;
                }

                if (direction.x != previousDirection.x || direction.y != previousDirection.y) {
                    simplifiedPath.push_back(path[index - 1]);
                    previousDirection = direction;
                }
            }

            simplifiedPath.push_back(path.back());
            return simplifiedPath;
        }

        isClosed[static_cast<size_t>(currentIndex)] = 1;
        const int currentColumn = currentIndex % grid.columnCount;
        const int currentRow = currentIndex / grid.columnCount;

        for (int neighborOffset = 0; neighborOffset < 4; ++neighborOffset) {
            const int neighborColumn = currentColumn + kNeighborColumns[neighborOffset];
            const int neighborRow = currentRow + kNeighborRows[neighborOffset];
            if (neighborColumn < 0 || neighborColumn >= grid.columnCount ||
                neighborRow < 0 || neighborRow >= grid.rowCount) {
                continue;
            }

            const int neighborIndex = grid.GetIndex(neighborColumn, neighborRow);
            if (grid.walkable[static_cast<size_t>(neighborIndex)] == 0 ||
                isClosed[static_cast<size_t>(neighborIndex)] != 0) {
                continue;
            }

            const float tentativeCost = costToStart[static_cast<size_t>(currentIndex)] + grid.step;
            if (tentativeCost >= costToStart[static_cast<size_t>(neighborIndex)]) {
                continue;
            }

            previousIndex[static_cast<size_t>(neighborIndex)] = currentIndex;
            costToStart[static_cast<size_t>(neighborIndex)] = tentativeCost;
            openSet.push(OpenNode{tentativeCost + heuristic(neighborIndex), neighborIndex});
        }
    }

    return {};
}

float GetResolutionDirection(float selfCenter, float otherCenter, float motion)
{
    if (selfCenter < otherCenter) {
        return -1.0f;
    }

    if (selfCenter > otherCenter) {
        return 1.0f;
    }

    return motion <= 0.0f ? 1.0f : -1.0f;
}

} // namespace

namespace project_playground::game::entities {

MazeRunner::MazeRunner(Cute::v2 startPosition, Cute::v2 goalPosition, float radius, CF_Color color, float speed)
    : m_position(startPosition), m_goalPosition(goalPosition), m_radius(radius), m_color(color), m_speed(speed)
{
}

std::string_view MazeRunner::GetTypeName() const
{
    return "MazeRunner";
}

bool MazeRunner::IsAlive() const
{
    return m_alive;
}

Cute::v2 MazeRunner::GetVelocity() const
{
    return m_velocity;
}

core::interfaces::CollisionBounds MazeRunner::GetCollisionBounds() const
{
    return {
        m_position.x - m_radius,
        m_position.x + m_radius,
        m_position.y - m_radius,
        m_position.y + m_radius,
        core::interfaces::CollisionBoundsType::Solid,
    };
}

void MazeRunner::SetCollisionProviders(std::vector<const core::interfaces::ICollisionable*> collisionProviders)
{
    const bool providersChanged = (m_collisionProviders != collisionProviders);
    m_collisionProviders = std::move(collisionProviders);
    if (providersChanged) {
        m_repathTimer = 0.0f;
    }
}

void MazeRunner::Update()
{
    const float deltaTime = CF_DELTA_TIME;
    if (deltaTime <= 0.0f) {
        return;
    }

    if (HasReachedGoal()) {
        m_alive = false;
        return;
    }

    m_repathTimer -= deltaTime;
    if (m_repathTimer <= 0.0f || m_navigationPath.empty()) {
        RebuildPath();
        m_repathTimer = m_repathInterval;
    }

    while (!m_navigationPath.empty() && IsPathNodeReached(m_navigationPath.front())) {
        m_navigationPath.erase(m_navigationPath.begin());
    }

    const Cute::v2 target = m_navigationPath.empty() ? m_goalPosition : m_navigationPath.front();
    const Cute::v2 delta = target - m_position;
    const float distanceSquared = delta.x * delta.x + delta.y * delta.y;
    if (distanceSquared <= 0.0001f) {
        m_velocity = Cute::v2(0.0f, 0.0f);
        return;
    }

    const float distance = std::sqrt(distanceSquared);
    const float moveDistance = std::min(m_speed * deltaTime, distance);
    const Cute::v2 direction(delta.x / distance, delta.y / distance);
    m_heading = direction;
    m_velocity = direction * m_speed;
    m_position += direction * moveDistance;
    ResolveCollisions();
}

void MazeRunner::Render() const
{
    Cute::draw_push_color(m_color);
    Cute::draw_circle_fill(m_position, m_radius);
    Cute::draw_pop_color();

    Cute::draw_push_color(Cute::make_color(0x10212d, 0xff));
    Cute::draw_circle_fill(m_position + m_heading * (m_radius * 0.45f), m_radius * 0.28f);
    Cute::draw_pop_color();
}

void MazeRunner::RebuildPath()
{
    std::vector<const core::interfaces::ICollisionable*> navigationProviders;
    navigationProviders.reserve(m_collisionProviders.size());
    for (const auto* provider : m_collisionProviders) {
        if (dynamic_cast<const core::interfaces::IEntity*>(provider) != nullptr) {
            continue;
        }
        navigationProviders.push_back(provider);
    }

    const NavigationGrid grid =
        BuildNavigationGrid(m_position, m_goalPosition, m_radius + 2.0f, m_navigationStep, navigationProviders);
    const int startIndex = FindClosestWalkableNode(grid, m_position);
    const int goalIndex = FindClosestWalkableNode(grid, m_goalPosition);
    m_navigationPath = BuildPath(grid, startIndex, goalIndex);

    if (!m_navigationPath.empty() && IsPathNodeReached(m_navigationPath.front())) {
        m_navigationPath.erase(m_navigationPath.begin());
    }
}

void MazeRunner::ResolveCollisions()
{
    for (const core::interfaces::ICollisionable* collisionProvider : m_collisionProviders) {
        if (collisionProvider == nullptr || collisionProvider == this) {
            continue;
        }

        const core::interfaces::CollisionBounds collisionBounds = collisionProvider->GetCollisionBounds();
        if (collisionBounds.type == core::interfaces::CollisionBoundsType::Containment) {
            m_position.x = std::clamp(m_position.x, collisionBounds.minX + m_radius, collisionBounds.maxX - m_radius);
            m_position.y = std::clamp(m_position.y, collisionBounds.minY + m_radius, collisionBounds.maxY - m_radius);
            continue;
        }

        const core::interfaces::CollisionBounds selfBounds = GetCollisionBounds();
        const float overlapX =
            std::min(selfBounds.maxX, collisionBounds.maxX) - std::max(selfBounds.minX, collisionBounds.minX);
        const float overlapY =
            std::min(selfBounds.maxY, collisionBounds.maxY) - std::max(selfBounds.minY, collisionBounds.minY);
        if (overlapX <= 0.0f || overlapY <= 0.0f) {
            continue;
        }

        const float selfCenterX = (selfBounds.minX + selfBounds.maxX) * 0.5f;
        const float selfCenterY = (selfBounds.minY + selfBounds.maxY) * 0.5f;
        const float otherCenterX = (collisionBounds.minX + collisionBounds.maxX) * 0.5f;
        const float otherCenterY = (collisionBounds.minY + collisionBounds.maxY) * 0.5f;

        if (overlapX < overlapY) {
            m_position.x += GetResolutionDirection(selfCenterX, otherCenterX, m_heading.x) * overlapX;
        } else {
            m_position.y += GetResolutionDirection(selfCenterY, otherCenterY, m_heading.y) * overlapY;
        }

        if (const auto* otherRunner = dynamic_cast<const MazeRunner*>(collisionProvider)) {
            const Cute::v2 otherVelocity = otherRunner->GetVelocity();
            const Cute::v2 normal(
                selfCenterX - otherCenterX,
                selfCenterY - otherCenterY);
            const float normalLengthSq = normal.x * normal.x + normal.y * normal.y;
            if (normalLengthSq > 0.0001f) {
                const float invLen = 1.0f / std::sqrt(normalLengthSq);
                const Cute::v2 n(normal.x * invLen, normal.y * invLen);
                const float relativeSpeed =
                    (m_velocity.x - otherVelocity.x) * n.x +
                    (m_velocity.y - otherVelocity.y) * n.y;
                if (relativeSpeed < 0.0f) {
                    m_velocity.x -= relativeSpeed * n.x;
                    m_velocity.y -= relativeSpeed * n.y;
                }
            }
        }
    }
}

bool MazeRunner::IsPathNodeReached(const Cute::v2& node) const
{
    const Cute::v2 delta = node - m_position;
    return delta.x * delta.x + delta.y * delta.y <= (m_navigationStep * 0.35f) * (m_navigationStep * 0.35f);
}

bool MazeRunner::HasReachedGoal() const
{
    const Cute::v2 delta = m_goalPosition - m_position;
    return delta.x * delta.x + delta.y * delta.y <= m_radius * m_radius;
}

}