cmake_minimum_required(VERSION 3.20...3.27 FATAL_ERROR)

project( lab2 )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -g) # убрать в случае ненадобности
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC src/)
add_subdirectory(src/) # исходный код
target_include_directories(src PUBLIC src/)
target_link_libraries(${PROJECT_NAME} src)

find_package(OpenGL REQUIRED)
target_link_libraries(src PUBLIC OpenGL::GL)

target_include_directories(src PUBLIC ext/)
add_subdirectory(ext/)  # файлы с библиотекой imgui
target_link_libraries(src PUBLIC imgui)

target_include_directories(${PROJECT_NAME} PUBLIC glfw/include/GLFW/)
target_include_directories(src PUBLIC glfw/include/GLFW/)
target_include_directories(imgui PUBLIC glfw/include/GLFW/)
add_subdirectory(glfw/) # библиотека с загрузкой окна
target_link_libraries(imgui PUBLIC glfw ${GLFW_LIBRARIES})
target_link_libraries(src PUBLIC glfw ${GLFW_LIBRARIES})
