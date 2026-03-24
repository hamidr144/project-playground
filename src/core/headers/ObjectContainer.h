#pragma once

#include <memory>
#include <vector>

#include "core/interfaces/IObjectContainer.h"

namespace project_playground::core {

class ObjectContainer : public interfaces::IObjectContainer {
public:
    ObjectContainer() = default;
    explicit ObjectContainer(std::vector<std::unique_ptr<interfaces::IObject>> objects);

    void AddObject(std::unique_ptr<interfaces::IObject> object) override;
    std::vector<std::unique_ptr<interfaces::IObject>>& GetObjects() override;
    const std::vector<std::unique_ptr<interfaces::IObject>>& GetObjects() const override;

private:
    std::vector<std::unique_ptr<interfaces::IObject>> m_objects;
};

}