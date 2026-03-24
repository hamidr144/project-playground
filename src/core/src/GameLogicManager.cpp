#include "core/headers/GameLogicManager.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "core/interfaces/ICollisionable.h"
#include "core/interfaces/ICollisionHandler.h"

namespace project_playground::core {

GameLogicManager::GameLogicManager(
    interfaces::IEntityContainer& entityContainer,
    const interfaces::IObjectContainer& objectContainer)
    : m_entityContainer(entityContainer), m_objectContainer(objectContainer)
{
}

void GameLogicManager::Update()
{
    SynchronizeCollisionProviders();

    for (const auto& entity : m_entityContainer.GetEntities()) {
        entity->Update();
    }

    auto& entities = m_entityContainer.GetEntities();
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
            [](const std::unique_ptr<interfaces::IEntity>& entity) {
                return !entity->IsAlive();
            }),
        entities.end());
}

void GameLogicManager::SynchronizeCollisionProviders()
{
    std::vector<const interfaces::ICollisionable*> collisionProviders;
    collisionProviders.reserve(m_entityContainer.GetEntities().size() + m_objectContainer.GetObjects().size());

    for (const auto& entity : m_entityContainer.GetEntities()) {
        if (auto* provider = dynamic_cast<interfaces::ICollisionable*>(entity.get())) {
            collisionProviders.push_back(provider);
        }
    }

    for (const auto& object : m_objectContainer.GetObjects()) {
        collisionProviders.push_back(object.get());
    }

    for (const auto& entity : m_entityContainer.GetEntities()) {
        if (auto* handler = dynamic_cast<interfaces::ICollisionHandler*>(entity.get())) {
            handler->SetCollisionProviders(collisionProviders);
        }
    }
}

}