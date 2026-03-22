#include "rendering/background/headers/BackgroundDrawable.h"

#include "cute_draw.h"

namespace project_playground::rendering::background {

BackgroundDrawable::BackgroundDrawable(CF_Color color)
    : m_color(color)
{
}

void BackgroundDrawable::Render() const
{
    const float halfWidth = static_cast<float>(Cute::app_get_canvas_width()) * 0.5f;
    const float halfHeight = static_cast<float>(Cute::app_get_canvas_height()) * 0.5f;

    Cute::draw_push_color(m_color);
    Cute::draw_quad_fill(cf_make_aabb(Cute::v2(-halfWidth, -halfHeight), Cute::v2(halfWidth, halfHeight)), 0.0f);
    Cute::draw_pop_color();
}

void BackgroundDrawable::SetColor(CF_Color color)
{
    m_color = color;
}

}