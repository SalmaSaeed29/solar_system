cmake_minimum_required(VERSION 3.24)
project(solar_system)

set(CMAKE_CXX_STANDARD 17)

add_executable(solar_system main.cpp)
target_link_libraries(solar_system -lglew32 -lopengl32 -lglu32 -lfreeglut)
