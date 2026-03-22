#pragma once

#include <vector>

namespace project_playground::core::interfaces {

class ICollisionProvider;

class ICollisionHandler {
public:
    virtual ~ICollisionHandler() = default;

    virtual void SetCollisionProviders(std::vector<const ICollisionProvider*> collisionProviders) = 0;
};

}