#include "core/headers/EntityContainer.h"

#include "core/interfaces/ICollisionHandler.h"
#include "core/interfaces/ICollisionProvider.h"

namespace project_playground::core {

EntityContainer::EntityContainer(std::vector<std::unique_ptr<interfaces::IEntity>> entities)
    : m_entities(std::move(entities))
{
    SynchronizeCollisionProviders();
}

void EntityContainer::AddEntity(std::unique_ptr<interfaces::IEntity> entity)
{
    m_entities.push_back(std::move(entity));
    SynchronizeCollisionProviders();
}

std::vector<std::unique_ptr<interfaces::IEntity>>& EntityContainer::GetEntities()
{
    return m_entities;
}

const std::vector<std::unique_ptr<interfaces::IEntity>>& EntityContainer::GetEntities() const
{
    return m_entities;
}

void EntityContainer::SynchronizeCollisionProviders()
{
    std::vector<const interfaces::ICollisionProvider*> collisionProviders;
    collisionProviders.reserve(m_entities.size());

    for (const auto& entity : m_entities) {
        if (auto* provider = dynamic_cast<interfaces::ICollisionProvider*>(entity.get())) {
            collisionProviders.push_back(provider);
        }
    }

    for (const auto& entity : m_entities) {
        if (auto* handler = dynamic_cast<interfaces::ICollisionHandler*>(entity.get())) {
            handler->SetCollisionProviders(collisionProviders);
        }
    }
}

}