cmake_minimum_required(VERSION 3.20)
project(te-bonfire VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(/home/sin/Projects/twilight-elysium ${CMAKE_BINARY_DIR}/twilight-elysium)

add_executable(te-bonfire src/main.cpp src/world.cpp src/boss.cpp src/hidden_boss.cpp)
target_include_directories(te-bonfire PRIVATE src)
target_link_libraries(te-bonfire PRIVATE te-engine)

enable_testing()
add_test(NAME te-bonfire-smoke COMMAND te-bonfire --sim)
set_tests_properties(te-bonfire-smoke PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})