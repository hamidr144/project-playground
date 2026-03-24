#pragma once

#include <array>
#include <string>

#include "core/interfaces/IEntityContainer.h"
#include "core/interfaces/IHud.h"
#include "core/interfaces/IObjectContainer.h"
#include "cute.h"

namespace project_playground::rendering::hud {

class DebugMenuHud : public core::interfaces::IHud {
public:
    DebugMenuHud(
        core::interfaces::IEntityContainer& entityContainer,
        core::interfaces::IObjectContainer& objectContainer);

    void HandleInput() override;
    void Render() const override;

private:
    void AddEntityFromInput();
    std::string BuildEntitySummary() const;

private:
    core::interfaces::IEntityContainer& m_entityContainer;
    core::interfaces::IObjectContainer& m_objectContainer;
    std::array<char, 64> m_entityTypeInput{"MazeRunner"};
    std::string m_feedbackMessage;
    bool m_lastActionSucceeded = true;
    CF_KeyButton m_toggleKey = CF_KEY_F2;
    bool m_isVisible = true;
};

}