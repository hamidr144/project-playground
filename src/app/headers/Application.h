#pragma once

#include <memory>

#include "core/headers/EntityContainer.h"
#include "core/headers/GameLogicManager.h"
#include "core/headers/GameRenderer.h"
#include "core/headers/InputHandler.h"

namespace project_playground::app {

class Application {
public:
    int Run(const char* executablePath);

private:
    void InitializeRuntimeObjects();
    void ShutdownRuntimeObjects();

private:
    core::EntityContainer m_entityContainer;
    std::unique_ptr<core::InputHandler> m_inputHandler;
    std::unique_ptr<core::GameLogicManager> m_logic;
    std::unique_ptr<core::GameRenderer> m_renderer;
};

}