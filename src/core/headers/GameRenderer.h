#pragma once

#include <memory>
#include <vector>

#include "core/interfaces/IEntityContainer.h"
#include "core/interfaces/IHud.h"
#include "core/interfaces/IRenderable.h"

namespace project_playground::core {

class GameRenderer {
public:
    GameRenderer(
        const interfaces::IEntityContainer& entityContainer,
        std::unique_ptr<interfaces::IRenderable> background,
        std::vector<std::unique_ptr<interfaces::IHud>> hudElements);

    void Render() const;

private:
    void RenderEntities() const;
    void RenderHud() const;

private:
    const interfaces::IEntityContainer& m_entityContainer;
    std::unique_ptr<interfaces::IRenderable> m_background;
    std::vector<std::unique_ptr<interfaces::IHud>> m_hudElements;
};

}