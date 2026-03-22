#pragma once

namespace project_playground::core::interfaces {

class IInputable {
public:
    virtual ~IInputable() = default;

    virtual void HandleInput() = 0;
};

}