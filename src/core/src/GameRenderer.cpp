#include "core/headers/GameRenderer.h"

namespace project_playground::core {

GameRenderer::GameRenderer(
    const interfaces::IEntityContainer& entityContainer,
    std::unique_ptr<interfaces::IRenderable> background,
    std::vector<std::unique_ptr<interfaces::IHud>> hudElements)
    : m_entityContainer(entityContainer), m_background(std::move(background)), m_hudElements(std::move(hudElements))
{
}

void GameRenderer::Render() const
{
    if (m_background) {
        m_background->Render();
    }

    RenderEntities();
    RenderHud();
}

void GameRenderer::RenderEntities() const
{
    for (const auto& entity : m_entityContainer.GetEntities()) {
        entity->Render();
    }
}

void GameRenderer::RenderHud() const
{
    for (const auto& hudElement : m_hudElements) {
        hudElement->Render();
    }
}

}