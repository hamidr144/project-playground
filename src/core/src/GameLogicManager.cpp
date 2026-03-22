#include "core/headers/GameLogicManager.h"

namespace project_playground::core {

GameLogicManager::GameLogicManager(interfaces::IEntityContainer& entityContainer)
    : m_entityContainer(entityContainer)
{
}

void GameLogicManager::Update()
{
    for (const auto& entity : m_entityContainer.GetEntities()) {
        entity->Update();
    }
}

}