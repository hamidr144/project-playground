#include "rendering/hud/headers/DebugMenuHud.h"

#include <cctype>
#include <map>
#include <memory>
#include <random>
#include <string_view>

#include <imgui.h>

#include "game/entities/headers/PointEntity.h"

namespace project_playground::rendering::hud {

namespace {
std::string NormalizeEntityType(std::string_view entityType)
{
    std::string normalized;
    normalized.reserve(entityType.size());

    for (char ch : entityType) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }
    }

    return normalized;
}

CF_Color MakeRandomVisibleColor()
{
    static thread_local std::mt19937 generator(std::random_device{}());
    static thread_local std::uniform_int_distribution<int> channelDistribution(32, 255);

    while (true) {
        const int red = channelDistribution(generator);
        const int green = channelDistribution(generator);
        const int blue = channelDistribution(generator);
        const int brightness = red + green + blue;

        if (brightness >= 220) {
            return Cute::make_color(red << 16 | green << 8 | blue, 0xff);
        }
    }
}

std::unique_ptr<core::interfaces::IEntity> MakeEntityByType(std::string_view entityType, size_t existingEntityCount)
{
    const std::string normalizedType = NormalizeEntityType(entityType);
    if (normalizedType != "pointentity" && normalizedType != "point") {
        return nullptr;
    }

    const float horizontalIndex = static_cast<float>(existingEntityCount % 5);
    const float verticalIndex = static_cast<float>((existingEntityCount / 5) % 3);
    const float x = (horizontalIndex - 2.0f) * 80.0f;
    const float y = (1.0f - verticalIndex) * 80.0f;
    const CF_Color color = MakeRandomVisibleColor();

    return std::make_unique<game::entities::PointEntity>(x, y, 8.0f, color);
}
}

DebugMenuHud::DebugMenuHud(core::interfaces::IEntityContainer& entityContainer)
    : m_entityContainer(entityContainer)
{
}

void DebugMenuHud::HandleInput()
{
    if (Cute::key_just_pressed(m_toggleKey)) {
        m_isVisible = !m_isVisible;
    }
}

void DebugMenuHud::Render() const
{
    if (!m_isVisible) {
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(16.0f, 72.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(320.0f, 220.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Debug Menu", nullptr, ImGuiWindowFlags_NoCollapse);
    ImGui::Text("Entity counts by type");
    ImGui::Separator();

    const std::string summary = BuildEntitySummary();
    ImGui::TextUnformatted(summary.c_str());
    ImGui::Spacing();

    const float buttonWidth = ImGui::CalcTextSize("Add entity").x + ImGui::GetStyle().FramePadding.x * 2.0f;
    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    const float inputWidth = ImGui::GetContentRegionAvail().x - buttonWidth - spacing;

    ImGui::SetNextItemWidth(inputWidth > 100.0f ? 100.0f : inputWidth);
    if (ImGui::InputText("Entity type", const_cast<char*>(m_entityTypeInput.data()), m_entityTypeInput.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
        const_cast<DebugMenuHud*>(this)->AddEntityFromInput();
    }

    ImGui::SameLine();
    if (ImGui::Button("Add entity", ImVec2(buttonWidth, 0.0f))) {
        const_cast<DebugMenuHud*>(this)->AddEntityFromInput();
    }

    ImGui::TextDisabled("Supported: PointEntity");
    ImGui::TextDisabled("Toggle menu: F2");

    if (!m_feedbackMessage.empty()) {
        const ImVec4 color = m_lastActionSucceeded ? ImVec4(0.3f, 0.9f, 0.4f, 1.0f) : ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
        ImGui::TextColored(color, "%s", m_feedbackMessage.c_str());
    }

    ImGui::End();
}

void DebugMenuHud::AddEntityFromInput()
{
    const size_t existingEntityCount = m_entityContainer.GetEntities().size();
    std::unique_ptr<core::interfaces::IEntity> entity = MakeEntityByType(m_entityTypeInput.data(), existingEntityCount);
    if (!entity) {
        m_lastActionSucceeded = false;
        m_feedbackMessage = "Unsupported entity type.";
        return;
    }

    const std::string entityType(entity->GetTypeName());
    m_entityContainer.AddEntity(std::move(entity));
    m_lastActionSucceeded = true;
    m_feedbackMessage = "Added " + entityType + ".";
}

std::string DebugMenuHud::BuildEntitySummary() const
{
    std::map<std::string, int> counts;
    for (const auto& entity : m_entityContainer.GetEntities()) {
        ++counts[std::string(entity->GetTypeName())];
    }

    if (counts.empty()) {
        return "No entities";
    }

    std::string summary;
    for (const auto& [typeName, count] : counts) {
        if (!summary.empty()) {
            summary += "\n";
        }

        summary += typeName + ": " + std::to_string(count);
    }

    return summary;
}

}