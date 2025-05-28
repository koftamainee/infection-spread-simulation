cmake_minimum_required(VERSION 3.16)
project(main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2)
FetchContent_MakeAvailable(json)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SOURCES src/*.cpp)

add_executable(main ${SOURCES})

add_subdirectory(src)

target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json)
