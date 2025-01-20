cmake_minimum_required(VERSION 3.14)

project(NuclearSimulation VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Vendor
add_subdirectory(Vendor/EppoCore)

# Files
file(GLOB_RECURSE SOURCES "Source/*.cpp")

set(INCLUDE_DIRS
        "Source"
        "../Vendor/EppoCore/EppoCore/Source"
        "../Vendor/EppoCore/EppoCore/Vendor"
        "../Vendor/EppoCore/EppoCore/Vendor/glm"
        "../Vendor/EppoCore/EppoCore/Vendor/spdlog/include"
)

add_executable(${PROJECT_NAME} ${SOURCES})

if (CMAKE_HOST_LINUX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE EPPO_LINUX)
elseif (CMAKE_HOST_WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE EPPO_WINDOWS)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE EPPO_GUI)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE EppoCore_GUI)