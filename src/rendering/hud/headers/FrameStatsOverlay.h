#pragma once

#include <string>

#include "core/interfaces/IHud.h"
#include "cute.h"

namespace project_playground::rendering::hud {

class FrameStatsOverlay : public core::interfaces::IHud {
public:
    void HandleInput() override;
    void Render() const override;

private:
    std::string BuildStatsText() const;

private:
    CF_Color m_statsColor = Cute::make_color(0xff0000, 0xff);
    CF_KeyButton m_toggleKey = CF_KEY_F1;
    int m_statsFontSize = 20;
    bool m_isVisible = true;
};

}