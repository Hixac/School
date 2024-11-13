cmake_minimum_required(VERSION 3.20...3.27 FATAL_ERROR)

project(lab1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -g) # убрать в случае ненадобности
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC src/)
add_subdirectory(src/) # исходный код
target_include_directories(src PUBLIC src/)
target_link_libraries(${PROJECT_NAME} src)
