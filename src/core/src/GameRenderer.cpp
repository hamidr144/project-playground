#include "core/headers/GameRenderer.h"

namespace project_playground::core {

GameRenderer::GameRenderer(
    const interfaces::IEntityContainer& entityContainer,
    const interfaces::IObjectContainer& objectContainer,
    std::unique_ptr<interfaces::IRenderable> background,
    std::vector<std::unique_ptr<interfaces::IHud>> hudElements)
    : m_entityContainer(entityContainer),
      m_objectContainer(objectContainer),
      m_background(std::move(background)),
      m_hudElements(std::move(hudElements))
{
}

void GameRenderer::Render() const
{
    if (m_background) {
        m_background->Render();
    }

    RenderCollisionObjects();
    RenderEntities();
    RenderHud();
}

void GameRenderer::RenderCollisionObjects() const
{
    for (const auto& object : m_objectContainer.GetObjects()) {
        object->Render();
    }
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