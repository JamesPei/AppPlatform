cmake_minimum_required(VERSION 3.10)

project(TimerTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")
set(CMAKE_C_STANDARD 99)

add_subdirectory(src)

add_executable(${PROJECT_NAME} test.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC Timer)

enable_testing()
add_subdirectory(tests)
