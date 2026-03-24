#pragma once

#include <memory>
#include <vector>

#include "core/interfaces/IObject.h"

namespace project_playground::core::interfaces {

class IObjectContainer {
public:
    virtual ~IObjectContainer() = default;

    virtual void AddObject(std::unique_ptr<IObject> object) = 0;
    virtual std::vector<std::unique_ptr<IObject>>& GetObjects() = 0;
    virtual const std::vector<std::unique_ptr<IObject>>& GetObjects() const = 0;
};

}