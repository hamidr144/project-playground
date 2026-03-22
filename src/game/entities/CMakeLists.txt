target_sources(${PROJECT_NAME} PRIVATE
    src/PointEntity.cpp
    src/ScreenBorderCollisionProvider.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)