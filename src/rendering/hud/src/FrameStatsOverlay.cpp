#include "rendering/hud/headers/FrameStatsOverlay.h"

#include <iomanip>
#include <sstream>

namespace project_playground::rendering::hud {

void FrameStatsOverlay::HandleInput()
{
    if (Cute::key_just_pressed(m_toggleKey)) {
        m_isVisible = !m_isVisible;
    }
}

void FrameStatsOverlay::Render() const
{
    if (!m_isVisible) {
        return;
    }

    const std::string statsText = BuildStatsText();
    const float width = static_cast<float>(Cute::app_get_canvas_width());
    const float height = static_cast<float>(Cute::app_get_canvas_height());

    Cute::push_font_size(m_statsFontSize);
    Cute::draw_push_color(m_statsColor);
    Cute::draw_text(statsText.c_str(), Cute::v2(-width * 0.5f + 5.0f, height * 0.5f - 5.0f));
    Cute::draw_pop_color();
    Cute::pop_font_size();
}

std::string FrameStatsOverlay::BuildStatsText() const
{
    const float fps = CF_DELTA_TIME > 0.0f ? 1.0f / CF_DELTA_TIME : 0.0f;
    const float deltaTimeMilliseconds = CF_DELTA_TIME * 1000.0f;
    const float elapsedSeconds = static_cast<float>(CF_SECONDS);

    std::ostringstream statsStream;
    statsStream << "FPS: " << std::fixed << std::setprecision(1) << fps
                << " | DeltaTime: " << std::setprecision(4) << deltaTimeMilliseconds << " ms"
                << " | Time: " << std::setprecision(2) << elapsedSeconds << " s";
    return statsStream.str();
}

}