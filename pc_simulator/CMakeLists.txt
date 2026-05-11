cmake_minimum_required(VERSION 3.10)
project(ThreeKingdomsSim)

set(CMAKE_CXX_STANDARD 17)

# Add define so code knows it's running on PC
add_compile_definitions(PC_SIMULATOR)

# Find SDL2 package
find_package(SDL2 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../application/sources/app/game_rts
)

# Export compile commands for VSCode intellisense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Source and header files
set(SOURCES
    main.cpp
    scr_map_mock.cpp
    ak_mock.h
    screens/scr_map.h
    ../application/sources/app/game_rts/game_logic.cpp
    ../application/sources/app/game_rts/game_logic.h
    ../application/sources/app/game_rts/game_types.h
    ../application/sources/app/game_rts/game_assets.cpp
    ../application/sources/app/game_rts/game_assets.h
    ../application/sources/app/game_rts/game_card.cpp
    ../application/sources/app/game_rts/game_card.h
    ../application/sources/app/game_rts/game_node.cpp
    ../application/sources/app/game_rts/game_node.h
    ../application/sources/app/game_rts/game_combat.cpp
    ../application/sources/app/game_rts/game_combat.h
    ../application/sources/app/game_rts/game_hero.cpp
    ../application/sources/app/game_rts/game_hero.h
    ../application/sources/app/game_rts/game_map.cpp
    ../application/sources/app/game_rts/game_map.h
    ../application/sources/app/game_rts/game_ai.cpp
    ../application/sources/app/game_rts/game_ai.h
    ../application/sources/app/game_rts/task_game.cpp
    ../application/sources/app/game_rts/task_game.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Link SDL2 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main SDL2::SDL2)
