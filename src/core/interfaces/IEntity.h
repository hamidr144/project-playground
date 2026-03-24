#pragma once

#include <string_view>

#include "core/interfaces/ICollisionable.h"
#include "core/interfaces/ILogicable.h"
#include "core/interfaces/IRenderable.h"

namespace project_playground::core::interfaces {

class IEntity : public IRenderable, public ILogicable, public ICollisionable {
public:
    ~IEntity() override = default;

    virtual std::string_view GetTypeName() const = 0;
    virtual bool IsAlive() const { return true; }
};

}