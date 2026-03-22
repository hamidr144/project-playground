#pragma once

#include "core/interfaces/IInputable.h"
#include "core/interfaces/IRenderable.h"

namespace project_playground::core::interfaces {

class IHud : public IRenderable, public IInputable {
public:
    ~IHud() override = default;
};

}