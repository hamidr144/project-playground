target_sources(${PROJECT_NAME} PRIVATE
    src/DebugMenuHud.cpp
    src/FrameStatsOverlay.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)