#pragma once

#include <cstddef>

#include "cute.h"

namespace project_playground::game {

inline constexpr float kMazeWallThickness = 16.0f;
inline constexpr float kMazeCellSize = 80.0f;
inline constexpr float kMazePitch = kMazeCellSize + kMazeWallThickness;
inline constexpr int kMazeColumnCount = 13;
inline constexpr int kMazeRowCount = 7;
inline constexpr float kMazeWidth =
    kMazeColumnCount * kMazeCellSize + (kMazeColumnCount + 1) * kMazeWallThickness;
inline constexpr float kMazeHeight =
    kMazeRowCount * kMazeCellSize + (kMazeRowCount + 1) * kMazeWallThickness;
inline constexpr float kMazeLeftBoundaryCenterX = -kMazeWidth * 0.5f + kMazeWallThickness * 0.5f;
inline constexpr float kMazeTopBoundaryCenterY = kMazeHeight * 0.5f - kMazeWallThickness * 0.5f;

inline Cute::v2 GetMazeCellCenter(int column, int row)
{
    const float firstCellCenterX = -kMazeWidth * 0.5f + kMazeWallThickness + kMazeCellSize * 0.5f;
    const float firstCellCenterY = kMazeHeight * 0.5f - kMazeWallThickness - kMazeCellSize * 0.5f;
    return Cute::v2(
        firstCellCenterX + static_cast<float>(column) * kMazePitch,
        firstCellCenterY - static_cast<float>(row) * kMazePitch);
}

inline Cute::v2 GetMazeRunnerSpawnPosition(std::size_t spawnIndex = 0)
{
    const Cute::v2 entranceCell = GetMazeCellCenter(0, 0);
    const float spreadX = static_cast<float>(spawnIndex % 3) * 12.0f - 12.0f;
    const float spreadY = static_cast<float>((spawnIndex / 3) % 3) * 12.0f - 12.0f;
    return Cute::v2(entranceCell.x + spreadX, entranceCell.y + spreadY);
}

inline Cute::v2 GetMazeRunnerGoalPosition()
{
    return GetMazeCellCenter(kMazeColumnCount - 1, kMazeRowCount - 1);
}

} // namespace project_playground::game