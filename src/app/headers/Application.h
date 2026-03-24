#pragma once

#include <memory>

#include "core/headers/EntityContainer.h"
#include "core/headers/GameLogicManager.h"
#include "core/headers/GameRenderer.h"
#include "core/headers/InputHandler.h"
#include "core/headers/ObjectContainer.h"

namespace project_playground::app {

class Application {
public:
    static Application& GetInstance();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    int Run(const char* executablePath);

private:
    Application() = default;

    void InitializeRuntimeObjects();
    void ShutdownRuntimeObjects();

private:
    core::EntityContainer m_entityContainer;
    core::ObjectContainer m_objectContainer;
    std::unique_ptr<core::InputHandler> m_inputHandler;
    std::unique_ptr<core::GameLogicManager> m_logic;
    std::unique_ptr<core::GameRenderer> m_renderer;
};

}