#pragma once

#include <string_view>

#include "core/interfaces/ILogicable.h"
#include "core/interfaces/IRenderable.h"

namespace project_playground::core::interfaces {

class IEntity : public IRenderable, public ILogicable {
public:
    ~IEntity() override = default;

    virtual std::string_view GetTypeName() const = 0;
};

}