#pragma once

#include "core/interfaces/ICollisionable.h"
#include "core/interfaces/IRenderable.h"

namespace project_playground::core::interfaces {

class IObject : public ICollisionable, public IRenderable {
public:
    virtual ~IObject() = default;
};

}