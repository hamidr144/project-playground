#pragma once

namespace project_playground::core::interfaces {

class ILogicable {
public:
    virtual ~ILogicable() = default;

    virtual void Update() = 0;
};

}