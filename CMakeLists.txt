cmake_minimum_required(VERSION 3.20)

project(cache_server LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cache_core
  src/server/server.cpp
  src/server/socket.cpp
)

target_include_directories(cache_core PUBLIC src)

target_compile_features(cache_core PUBLIC cxx_std_20)

target_compile_options(cache_core PRIVATE
  -Wall
  -Wextra
  -pedantic
)

add_executable(cache_server
  src/main.cpp
)

target_link_libraries(cache_server PRIVATE cache_core)
