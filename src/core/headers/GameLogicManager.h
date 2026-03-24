#pragma once

#include "core/interfaces/IEntityContainer.h"
#include "core/interfaces/IObjectContainer.h"

namespace project_playground::core {

class GameLogicManager {
public:
    GameLogicManager(
        interfaces::IEntityContainer& entityContainer,
        const interfaces::IObjectContainer& objectContainer);

    void Update();

private:
    void SynchronizeCollisionProviders();

private:
    interfaces::IEntityContainer& m_entityContainer;
    const interfaces::IObjectContainer& m_objectContainer;
};

}