cmake_minimum_required(VERSION 3.20)
project(ecs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
    SOURCES
    src/main.cpp
    src/ecs/component.hpp
    src/ecs/entity.hpp
    src/ecs/system.hpp
    src/ecs/world.hpp
)

set(
    EXAMPLE_SOURCES
    src/demo/main.cpp
    src/demo/components.hpp
    src/demo/systems.hpp
    src/demo/simple_example.cpp
    src/ecs/component.hpp
    src/ecs/entity.hpp
    src/ecs/system.hpp
    src/ecs/world.hpp
)

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
)

target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/src
)

# Demo executables
add_executable(
    ecs_simple_example
    src/demo/simple_example.cpp
)

target_include_directories(
    ecs_simple_example
    PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/src
)

add_executable(
    ecs_demo
    src/demo/main.cpp
)

target_include_directories(
    ecs_demo
    PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/src
)
