#include "app/headers/Application.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

#include "core/headers/EntityContainer.h"
#include "core/headers/GameLogicManager.h"
#include "core/headers/GameRenderer.h"
#include "core/headers/InputHandler.h"
#include "rendering/background/headers/BackgroundDrawable.h"
#include "rendering/hud/headers/DebugMenuHud.h"
#include "rendering/hud/headers/FrameStatsOverlay.h"
#include "cute.h"

namespace {
class AppLifetime {
public:
    AppLifetime() = default;
    AppLifetime(const AppLifetime&) = delete;
    AppLifetime& operator=(const AppLifetime&) = delete;

    ~AppLifetime()
    {
        Cute::destroy_app();
    }
};
}

namespace project_playground::app {

namespace {
std::vector<std::unique_ptr<core::interfaces::IHud>> MakeDefaultHud(core::interfaces::IEntityContainer& entityContainer)
{
    std::vector<std::unique_ptr<core::interfaces::IHud>> hudElements;
    hudElements.emplace_back(std::make_unique<rendering::hud::DebugMenuHud>(entityContainer));
    hudElements.emplace_back(std::make_unique<rendering::hud::FrameStatsOverlay>());
    return hudElements;
}

std::vector<std::reference_wrapper<core::interfaces::IInputable>> MakeInputables(
    std::vector<std::unique_ptr<core::interfaces::IHud>>& hudElements)
{
    std::vector<std::reference_wrapper<core::interfaces::IInputable>> inputables;
    inputables.reserve(hudElements.size());

    for (const auto& hudElement : hudElements) {
        inputables.emplace_back(*hudElement);
    }

    return inputables;
}
}

void Application::InitializeRuntimeObjects()
{
    m_entityContainer = core::EntityContainer();

    auto hudElements = MakeDefaultHud(m_entityContainer);
    m_inputHandler = std::make_unique<core::InputHandler>(MakeInputables(hudElements));
    m_logic = std::make_unique<core::GameLogicManager>(m_entityContainer);
    m_renderer = std::make_unique<core::GameRenderer>(
        m_entityContainer,
        std::make_unique<rendering::background::BackgroundDrawable>(Cute::make_color(0x00000000)),
        std::move(hudElements));
}

void Application::ShutdownRuntimeObjects()
{
    m_renderer.reset();
    m_logic.reset();
    m_inputHandler.reset();
    m_entityContainer = core::EntityContainer();
}

int Application::Run(const char* executablePath)
{
    CF_Result result = Cute::make_app(
        "Project Playground",
        0,
        0,
        0,
        1280,
        720,
        CF_APP_OPTIONS_WINDOW_POS_CENTERED_BIT,
        executablePath);
    if (Cute::is_error(result)) {
        throw std::runtime_error(result.details);
    }

    AppLifetime appLifetime;

    Cute::set_target_framerate(60);
    Cute::app_init_imgui();

    InitializeRuntimeObjects();

    while (Cute::app_is_running()) {
        Cute::app_update();

        m_inputHandler->HandleInput();
        m_logic->Update();
        m_renderer->Render();

        Cute::app_draw_onto_screen();
    }

    ShutdownRuntimeObjects();

    return 0;
}

}