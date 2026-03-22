target_sources(${PROJECT_NAME} PRIVATE
    src/EntityContainer.cpp
    src/GameLogicManager.cpp
    src/GameRenderer.cpp
    src/InputHandler.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
)