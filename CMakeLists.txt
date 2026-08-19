cmake_minimum_required(VERSION 3.20)

project(cache_server LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(cache_core
  src/store/store.cpp
  src/parser/parser.cpp
  src/executor/executor.cpp
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

add_executable(store_test
  tests/store_test.cpp
)

target_link_libraries(store_test PRIVATE cache_core)

add_executable(parser_test
  tests/parser_test.cpp
)

target_link_libraries(parser_test PRIVATE cache_core)

add_executable(executor_test
  tests/executor_test.cpp
)
target_link_libraries(executor_test PRIVATE cache_core)

enable_testing()

add_test(NAME store_test COMMAND store_test)
add_test(NAME parser_test COMMAND parser_test)
add_test(NAME executor_test COMMAND executor_test)
