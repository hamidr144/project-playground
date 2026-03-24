#pragma once

#include <vector>

namespace project_playground::core::interfaces {

class ICollisionable;

class ICollisionHandler {
public:
    virtual ~ICollisionHandler() = default;

    virtual void SetCollisionProviders(std::vector<const ICollisionable*> collisionProviders) = 0;
};

}