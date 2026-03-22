#pragma once

#include "core/interfaces/IRenderable.h"
#include "cute.h"

namespace project_playground::rendering::background {

class BackgroundDrawable : public core::interfaces::IRenderable {
public:
    explicit BackgroundDrawable(CF_Color color);

    void Render() const override;

    void SetColor(CF_Color color);

private:
    CF_Color m_color;
};

}