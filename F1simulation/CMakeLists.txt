# CMakeList.txt : CMake project for Seclous Assessment Server, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.9.0...4.1.0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

project ("F1Simulation")

# Add source to this project's executable.
add_executable (F1Simulation "main.cpp" "src/laptimesim.cpp" "src/simulationstep.cpp")

target_sources(F1Simulation PRIVATE "src/laptimelog.cpp")

target_include_directories(F1Simulation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(F1Simulation PRIVATE OpenMP::OpenMP_CXX)
endif()