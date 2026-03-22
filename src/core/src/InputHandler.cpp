#include "core/headers/InputHandler.h"

namespace project_playground::core {

InputHandler::InputHandler(std::vector<std::reference_wrapper<interfaces::IInputable>> inputables)
    : m_inputables(std::move(inputables))
{
}

void InputHandler::HandleInput()
{
    for (interfaces::IInputable& inputable : m_inputables) {
        inputable.HandleInput();
    }
}

void InputHandler::AddInputable(interfaces::IInputable& inputable)
{
    m_inputables.emplace_back(inputable);
}

}