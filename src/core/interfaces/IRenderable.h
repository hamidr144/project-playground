#pragma once

namespace project_playground::core::interfaces {

class IRenderable {
public:
    virtual ~IRenderable() = default;

    virtual void Render() const = 0;
};

}