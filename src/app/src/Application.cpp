#include "app/headers/Application.h"

#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <random>
#include <stdexcept>
#include <vector>

#include "core/headers/EntityContainer.h"
#include "core/headers/GameLogicManager.h"
#include "core/headers/GameRenderer.h"
#include "core/headers/InputHandler.h"
#include "game/entities/headers/MazeRunner.h"
#include "game/headers/MazeLayout.h"
#include "game/objects/headers/MazeBoundary.h"
#include "game/objects/headers/WallObject.h"
#include "rendering/background/headers/BackgroundDrawable.h"
#include "rendering/hud/headers/DebugMenuHud.h"
#include "rendering/hud/headers/FrameStatsOverlay.h"
#include "cute.h"

namespace {
class AppLifetime {
public:
    AppLifetime() = default;
    AppLifetime(const AppLifetime&) = delete;
    AppLifetime& operator=(const AppLifetime&) = delete;

    ~AppLifetime()
    {
        Cute::destroy_app();
    }
};
}

namespace project_playground::app {

Application& Application::GetInstance()
{
    static Application instance;
    return instance;
}

namespace {
std::vector<std::unique_ptr<core::interfaces::IHud>> MakeDefaultHud(
    core::interfaces::IEntityContainer& entityContainer,
    core::interfaces::IObjectContainer& objectContainer)
{
    std::vector<std::unique_ptr<core::interfaces::IHud>> hudElements;
    hudElements.emplace_back(std::make_unique<rendering::hud::DebugMenuHud>(entityContainer, objectContainer));
    hudElements.emplace_back(std::make_unique<rendering::hud::FrameStatsOverlay>());
    return hudElements;
}

std::vector<std::reference_wrapper<core::interfaces::IInputable>> MakeInputables(
    std::vector<std::unique_ptr<core::interfaces::IHud>>& hudElements)
{
    std::vector<std::reference_wrapper<core::interfaces::IInputable>> inputables;
    inputables.reserve(hudElements.size());

    for (const auto& hudElement : hudElements) {
        inputables.emplace_back(*hudElement);
    }

    return inputables;
}
std::vector<std::unique_ptr<core::interfaces::IEntity>> MakeDefaultEntities()
{
    std::vector<std::unique_ptr<core::interfaces::IEntity>> entities;
    entities.emplace_back(std::make_unique<game::entities::MazeRunner>(
        game::GetMazeRunnerSpawnPosition(),
        game::GetMazeRunnerGoalPosition(),
        12.0f,
        Cute::make_color(0xff7a59, 0xff),
        144.0f));
    return entities;
}

std::vector<std::unique_ptr<core::interfaces::IObject>> MakeDefaultObjects()
{
    std::vector<std::unique_ptr<core::interfaces::IObject>> objects;
    const CF_Color wallColor = Cute::make_color(0x9fb3c8, 0xff);

    objects.emplace_back(std::make_unique<game::objects::MazeBoundary>(game::kMazeWidth, game::kMazeHeight));

    auto addWall = [&](float x, float y, float width, float height) {
        objects.emplace_back(std::make_unique<game::objects::WallObject>(x, y, width, height, wallColor));
    };

    auto addVerticalSegment = [&](int boundaryColumn, int startRow, int endRow) {
        const float x = game::kMazeLeftBoundaryCenterX + static_cast<float>(boundaryColumn) * game::kMazePitch;
        const float startY = game::kMazeTopBoundaryCenterY - static_cast<float>(startRow) * game::kMazePitch;
        const float endY = game::kMazeTopBoundaryCenterY - static_cast<float>(endRow + 1) * game::kMazePitch;
        const float centerY = (startY + endY) * 0.5f;
        const float height = (startY - endY) + game::kMazeWallThickness;
        addWall(x, centerY, game::kMazeWallThickness, height);
    };

    auto addHorizontalSegment = [&](int boundaryRow, int startColumn, int endColumn) {
        const float y = game::kMazeTopBoundaryCenterY - static_cast<float>(boundaryRow) * game::kMazePitch;
        const float startX = game::kMazeLeftBoundaryCenterX + static_cast<float>(startColumn) * game::kMazePitch;
        const float endX = game::kMazeLeftBoundaryCenterX + static_cast<float>(endColumn + 1) * game::kMazePitch;
        const float centerX = (startX + endX) * 0.5f;
        const float width = (endX - startX) + game::kMazeWallThickness;
        addWall(centerX, y, width, game::kMazeWallThickness);
    };

    // Outer frame with a top-left entrance and bottom-right exit.
    addHorizontalSegment(0, 1, game::kMazeColumnCount - 1);
    addHorizontalSegment(game::kMazeRowCount, 0, game::kMazeColumnCount - 2);
    addVerticalSegment(0, 0, game::kMazeRowCount - 1);
    addVerticalSegment(game::kMazeColumnCount, 0, game::kMazeRowCount - 1);

    // Generate interior maze walls using randomized DFS (recursive backtracking).
    // This guarantees all cells are reachable and a path from entrance to exit exists.
    constexpr int cols = game::kMazeColumnCount;
    constexpr int rows = game::kMazeRowCount;

    // Wall arrays: true = wall present.
    // horizontalWalls[r][c] = wall on the bottom edge of cell (c, r-1) / top edge of cell (c, r).
    // Boundary rows 1..rows-1 are interior horizontal walls.
    std::array<std::array<bool, cols>, rows + 1> horizontalWalls{};
    // verticalWalls[r][bc] = wall on the right edge of cell (bc-1, r) / left edge of cell (bc, r).
    // Boundary columns 1..cols-1 are interior vertical walls.
    std::array<std::array<bool, cols + 1>, rows> verticalWalls{};

    // Initialize all interior walls as present.
    for (int r = 1; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            horizontalWalls[r][c] = true;
        }
    }
    for (int r = 0; r < rows; ++r) {
        for (int bc = 1; bc < cols; ++bc) {
            verticalWalls[r][bc] = true;
        }
    }

    // Visited grid for DFS.
    std::array<std::array<bool, cols>, rows> visited{};
    std::mt19937 rng(std::random_device{}());

    // Direction offsets: right, left, down, up.
    constexpr int dc[4] = {1, -1, 0, 0};
    constexpr int dr[4] = {0, 0, 1, -1};

    // Iterative DFS with explicit stack to avoid deep recursion.
    struct Cell { int col; int row; };
    std::vector<Cell> stack;
    stack.push_back({0, 0});
    visited[0][0] = true;

    while (!stack.empty()) {
        Cell& current = stack.back();
        const int cc = current.col;
        const int cr = current.row;

        // Collect unvisited neighbors.
        int neighbors[4];
        int neighborCount = 0;
        for (int d = 0; d < 4; ++d) {
            const int nc = cc + dc[d];
            const int nr = cr + dr[d];
            if (nc >= 0 && nc < cols && nr >= 0 && nr < rows && !visited[nr][nc]) {
                neighbors[neighborCount++] = d;
            }
        }

        if (neighborCount == 0) {
            stack.pop_back();
            continue;
        }

        // Pick a random unvisited neighbor.
        const int chosen = neighbors[std::uniform_int_distribution<int>(0, neighborCount - 1)(rng)];
        const int nc = cc + dc[chosen];
        const int nr = cr + dr[chosen];

        // Remove the wall between current and neighbor.
        if (chosen == 0) {        // Right: remove vertical wall at boundary column cc+1, row cr.
            verticalWalls[cr][cc + 1] = false;
        } else if (chosen == 1) { // Left: remove vertical wall at boundary column cc, row cr.
            verticalWalls[cr][cc] = false;
        } else if (chosen == 2) { // Down: remove horizontal wall at boundary row cr+1, column cc.
            horizontalWalls[cr + 1][cc] = false;
        } else {                  // Up: remove horizontal wall at boundary row cr, column cc.
            horizontalWalls[cr][cc] = false;
        }

        visited[nr][nc] = true;
        stack.push_back({nc, nr});
    }

    // Remove a few extra walls to create loops and alternate routes (makes it more interesting).
    const int extraOpenings = (cols * rows) / 6;
    for (int i = 0; i < extraOpenings; ++i) {
        if (std::uniform_int_distribution<int>(0, 1)(rng) == 0) {
            // Remove a random interior horizontal wall.
            const int r = std::uniform_int_distribution<int>(1, rows - 1)(rng);
            const int c = std::uniform_int_distribution<int>(0, cols - 1)(rng);
            horizontalWalls[r][c] = false;
        } else {
            // Remove a random interior vertical wall.
            const int r = std::uniform_int_distribution<int>(0, rows - 1)(rng);
            const int bc = std::uniform_int_distribution<int>(1, cols - 1)(rng);
            verticalWalls[r][bc] = false;
        }
    }

    // Convert wall data into wall segments (merge consecutive walls for efficiency).
    // Vertical wall segments: for each boundary column, merge consecutive rows.
    for (int bc = 1; bc < cols; ++bc) {
        int segmentStart = -1;
        for (int r = 0; r < rows; ++r) {
            if (verticalWalls[r][bc]) {
                if (segmentStart < 0) {
                    segmentStart = r;
                }
            } else {
                if (segmentStart >= 0) {
                    addVerticalSegment(bc, segmentStart, r - 1);
                    segmentStart = -1;
                }
            }
        }
        if (segmentStart >= 0) {
            addVerticalSegment(bc, segmentStart, rows - 1);
        }
    }

    // Horizontal wall segments: for each boundary row, merge consecutive columns.
    for (int br = 1; br < rows; ++br) {
        int segmentStart = -1;
        for (int c = 0; c < cols; ++c) {
            if (horizontalWalls[br][c]) {
                if (segmentStart < 0) {
                    segmentStart = c;
                }
            } else {
                if (segmentStart >= 0) {
                    addHorizontalSegment(br, segmentStart, c - 1);
                    segmentStart = -1;
                }
            }
        }
        if (segmentStart >= 0) {
            addHorizontalSegment(br, segmentStart, cols - 1);
        }
    }

    return objects;
}
}

void Application::InitializeRuntimeObjects()
{
    m_entityContainer = core::EntityContainer(MakeDefaultEntities());
    m_objectContainer = core::ObjectContainer(MakeDefaultObjects());

    auto hudElements = MakeDefaultHud(m_entityContainer, m_objectContainer);
    m_inputHandler = std::make_unique<core::InputHandler>(MakeInputables(hudElements));
    m_logic = std::make_unique<core::GameLogicManager>(m_entityContainer, m_objectContainer);
    m_renderer = std::make_unique<core::GameRenderer>(
        m_entityContainer,
        m_objectContainer,
        std::make_unique<rendering::background::BackgroundDrawable>(Cute::make_color(0x00000000)),
        std::move(hudElements));
}

void Application::ShutdownRuntimeObjects()
{
    m_renderer.reset();
    m_logic.reset();
    m_inputHandler.reset();
    m_entityContainer = core::EntityContainer();
    m_objectContainer = core::ObjectContainer();
}

int Application::Run(const char* executablePath)
{
    CF_Result result = Cute::make_app(
        "Project Playground",
        0,
        0,
        0,
        1280,
        720,
        CF_APP_OPTIONS_WINDOW_POS_CENTERED_BIT,
        executablePath);
    if (Cute::is_error(result)) {
        throw std::runtime_error(result.details);
    }

    AppLifetime appLifetime;

    Cute::set_target_framerate(60);
    Cute::app_init_imgui();

    InitializeRuntimeObjects();

    while (Cute::app_is_running()) {
        Cute::app_update();

        m_inputHandler->HandleInput();
        m_logic->Update();
        m_renderer->Render();

        Cute::app_draw_onto_screen();
    }

    ShutdownRuntimeObjects();

    return 0;
}

}