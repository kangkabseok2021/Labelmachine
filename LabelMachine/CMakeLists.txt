# CMakeList.txt : CMake project for Seclous Assessment Server, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8.0...4.1.0)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

project ("LabelMachine")

# Add source to this project's executable.
add_executable (labelMachine "main.cpp")
target_include_directories(labelMachine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)


