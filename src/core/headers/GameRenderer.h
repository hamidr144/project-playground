#pragma once

#include <memory>
#include <vector>

#include "core/interfaces/IEntityContainer.h"
#include "core/interfaces/IHud.h"
#include "core/interfaces/IObjectContainer.h"
#include "core/interfaces/IRenderable.h"

namespace project_playground::core {

class GameRenderer {
public:
    GameRenderer(
        const interfaces::IEntityContainer& entityContainer,
        const interfaces::IObjectContainer& objectContainer,
        std::unique_ptr<interfaces::IRenderable> background,
        std::vector<std::unique_ptr<interfaces::IHud>> hudElements);

    void Render() const;

private:
    void RenderCollisionObjects() const;
    void RenderEntities() const;
    void RenderHud() const;

private:
    const interfaces::IEntityContainer& m_entityContainer;
    const interfaces::IObjectContainer& m_objectContainer;
    std::unique_ptr<interfaces::IRenderable> m_background;
    std::vector<std::unique_ptr<interfaces::IHud>> m_hudElements;
};

}