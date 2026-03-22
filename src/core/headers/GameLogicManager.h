#pragma once

#include "core/interfaces/IEntityContainer.h"

namespace project_playground::core {

class GameLogicManager {
public:
    explicit GameLogicManager(interfaces::IEntityContainer& entityContainer);

    void Update();

private:
    interfaces::IEntityContainer& m_entityContainer;
};

}