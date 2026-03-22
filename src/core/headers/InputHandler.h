#pragma once

#include <functional>
#include <vector>

#include "core/interfaces/IInputable.h"

namespace project_playground::core {

class InputHandler {
public:
    InputHandler() = default;
    explicit InputHandler(std::vector<std::reference_wrapper<interfaces::IInputable>> inputables);

    void HandleInput();
    void AddInputable(interfaces::IInputable& inputable);

private:
    std::vector<std::reference_wrapper<interfaces::IInputable>> m_inputables;
};

}