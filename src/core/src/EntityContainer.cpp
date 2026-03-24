#include "core/headers/EntityContainer.h"

namespace project_playground::core {

EntityContainer::EntityContainer(std::vector<std::unique_ptr<interfaces::IEntity>> entities)
    : m_entities(std::move(entities))
{
}

void EntityContainer::AddEntity(std::unique_ptr<interfaces::IEntity> entity)
{
    m_entities.push_back(std::move(entity));
}

std::vector<std::unique_ptr<interfaces::IEntity>>& EntityContainer::GetEntities()
{
    return m_entities;
}

const std::vector<std::unique_ptr<interfaces::IEntity>>& EntityContainer::GetEntities() const
{
    return m_entities;
}

}