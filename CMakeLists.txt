cmake_minimum_required(VERSION 3.20.1)

set(CMAKE_CXX_STANDARD 20)
find_package(Catch2 REQUIRED)

enable_testing()

project(sic_optim VERSION 0.1.0)

add_executable(sic_optim)
add_executable(sic_optim_test)
target_sources(sic_optim PRIVATE src/Utils.cpp src/main.cpp
	src/Env.cpp src/AntSystem.cpp src/NearestNeighbor.cpp src/ExactAlgorithms.cpp
	src/AntColonySystem.cpp src/SimulatedAnnealing.cpp)
target_sources(sic_optim_test PRIVATE src/Utils.cpp tests/main_test.cpp
	src/Env.cpp src/AntSystem.cpp src/NearestNeighbor.cpp src/ExactAlgorithms.cpp
	src/AntColonySystem.cpp src/SimulatedAnnealing.cpp)

add_test(NAME sic_optim_test COMMAND sic_optim_test)
set(CMAKE_CXX_FLAGS "-O3")
