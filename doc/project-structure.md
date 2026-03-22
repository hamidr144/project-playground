# Project Structure

This document keeps the internal codebase notes that do not belong in the root README.

## Directory Layout

```text
src/
  main.cpp
  core/
    CMakeLists.txt
    headers/
      EntityContainer.h
      GameLogicManager.h
      GameRenderer.h
      InputHandler.h
    interfaces/
      IEntity.h
      IEntityContainer.h
      IHud.h
      IInputable.h
      ILogicable.h
      IRenderable.h
    src/
      EntityContainer.cpp
      GameLogicManager.cpp
      GameRenderer.cpp
      InputHandler.cpp
  app/
    CMakeLists.txt
    headers/
      Application.h
    src/
      Application.cpp
  game/
    CMakeLists.txt
    entities/
      CMakeLists.txt
      headers/
        PointEntity.h
      src/
        PointEntity.cpp
  rendering/
    CMakeLists.txt
    background/
      CMakeLists.txt
      headers/
        BackgroundDrawable.h
      src/
        BackgroundDrawable.cpp
    hud/
      CMakeLists.txt
      headers/
        FrameStatsOverlay.h
      src/
        FrameStatsOverlay.cpp
```

## Responsibilities

### `src/core`

`src/core` owns the abstract contracts and the orchestration layer.

- `core/interfaces/IRenderable.h`: base render contract
- `core/interfaces/ILogicable.h`: update contract
- `core/interfaces/IInputable.h`: input contract
- `core/interfaces/IEntity.h`: gameplay object contract combining render and logic
- `core/interfaces/IEntityContainer.h`: interface for the shared game-object container
- `core/interfaces/IHud.h`: HUD contract combining render, logic, and input
- `core/headers/EntityContainer.h`: concrete container for all game objects
- `core/headers/GameLogicManager.h`: updates game objects through `IEntityContainer`
- `core/headers/GameRenderer.h`: renders game objects through `IEntityContainer`
- `core/headers/InputHandler.h`: calls `HandleInput()` on registered inputable objects

`core` should depend only on interfaces, not concrete gameplay or rendering types.

### `src/app`

`src/app` assembles the concrete implementations and drives the app loop.

- `Application` creates concrete entities, background renderables, and HUD elements
- `Application` injects them into the core container and managers

### `src/game`

`src/game` contains concrete gameplay implementations.

- `game/entities/PointEntity` is a concrete `IEntity`

### `src/rendering`

`src/rendering` contains concrete rendering implementations.

- `rendering/background/BackgroundDrawable` is a concrete `IRenderable`
- `rendering/hud/FrameStatsOverlay` is a concrete `IHud`

## Namespaces

Namespaces mirror the folder layout.

- `project_playground::core`
- `project_playground::core::interfaces`
- `project_playground::app`
- `project_playground::game::entities`
- `project_playground::rendering::background`
- `project_playground::rendering::hud`

Local includes should also follow the folder layout, for example `core/headers/GameLogicManager.h` or `core/interfaces/IEntity.h`.

## Rendering Model

Rendering uses Cute Framework. The canvas is centered at `(0, 0)`, so full-screen geometry is usually built from half-width and half-height extents.

Examples:

- left edge: `-width * 0.5f`
- right edge: `width * 0.5f`
- top edge: `height * 0.5f`
- bottom edge: `-height * 0.5f`

`FrameStatsOverlay` lives under `src/rendering/hud` and can be toggled with `F1`.

## Data Flow

- `EntityContainer` owns all game objects as `IEntity`
- `GameLogicManager` updates those objects through `ILogicable` behavior exposed by `IEntity`
- `GameRenderer` renders those same objects through `IRenderable` behavior exposed by `IEntity`
- `InputHandler` calls `HandleInput()` on registered `IInputable` objects such as HUD elements

## Adding New Code

### Add a new interface

1. Add the header to `src/core/interfaces`
2. Keep the namespace under `project_playground::core::interfaces`

### Add a new core orchestrator or manager

1. Add the header to `src/core/headers`
2. Add the implementation to `src/core/src`
3. Register the new `.cpp` file in `src/core/CMakeLists.txt`
4. Keep dependencies pointed at interfaces, not concrete implementations

### Add a new game-object container implementation

1. Implement `core::interfaces::IEntityContainer`
2. Keep ownership of all game objects there
3. Pass the container to `GameLogicManager` and `GameRenderer`

### Add a gameplay entity

1. Add headers to `src/game/entities/headers`
2. Add implementation files to `src/game/entities/src`
3. Implement `core::interfaces::IEntity`
4. Register the new `.cpp` file in `src/game/entities/CMakeLists.txt`

### Add a draw-only scene object

1. Add headers to `src/rendering/background/headers`
2. Add implementation files to `src/rendering/background/src`
3. Implement `core::interfaces::IRenderable`
4. Register the new `.cpp` file in `src/rendering/background/CMakeLists.txt`

### Add a HUD element

1. Add headers to `src/rendering/hud/headers`
2. Add implementation files to `src/rendering/hud/src`
3. Implement `core::interfaces::IHud`
4. Register the new `.cpp` file in `src/rendering/hud/CMakeLists.txt`

### Update app composition

1. Add wiring in `src/app/src/Application.cpp`
2. Instantiate concrete implementations there
3. Inject entities into `EntityContainer`
4. Inject HUD input targets into `InputHandler`
5. Inject the shared container and render targets into the core managers

## CMake Structure

The main `CMakeLists.txt` stays intentionally small.

- it creates the executable
- it adds subdirectories for `core`, `app`, `game`, and `rendering`
- it links shared dependencies such as `cute`

Each domain owns its own file list through `target_sources(${PROJECT_NAME} PRIVATE ...)`, and nested subdomains such as `game/entities` or `rendering/hud` extend the same executable target.