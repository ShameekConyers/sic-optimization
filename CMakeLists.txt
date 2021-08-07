cmake_minimum_required(VERSION 3.20.1)

set(CMAKE_CXX_STANDARD 20)

project(sic_tsp VERSION 0.1.0)

add_executable(sic_tsp)
target_sources(sic_tsp PRIVATE src/main.cpp src/Env.cpp)
add_compile_options(-O3)
