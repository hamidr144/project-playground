#pragma once

#include <memory>
#include <vector>

#include "core/interfaces/IEntityContainer.h"

namespace project_playground::core {

class EntityContainer : public interfaces::IEntityContainer {
public:
    EntityContainer() = default;
    explicit EntityContainer(std::vector<std::unique_ptr<interfaces::IEntity>> entities);

    void AddEntity(std::unique_ptr<interfaces::IEntity> entity) override;

    std::vector<std::unique_ptr<interfaces::IEntity>>& GetEntities() override;
    const std::vector<std::unique_ptr<interfaces::IEntity>>& GetEntities() const override;

private:
    void SynchronizeCollisionProviders();

    std::vector<std::unique_ptr<interfaces::IEntity>> m_entities;
};

}