#pragma once

#include <memory>
#include <vector>

#include "core/interfaces/IEntity.h"

namespace project_playground::core::interfaces {

class IEntityContainer {
public:
    virtual ~IEntityContainer() = default;

    virtual void AddEntity(std::unique_ptr<IEntity> entity) = 0;
    virtual std::vector<std::unique_ptr<IEntity>>& GetEntities() = 0;
    virtual const std::vector<std::unique_ptr<IEntity>>& GetEntities() const = 0;
};

}