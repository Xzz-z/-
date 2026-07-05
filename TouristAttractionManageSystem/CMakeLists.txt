cmake_minimum_required(VERSION 3.29)
project(TouristAttractionManageSystem)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(TouristAttractionManageSystem
    Graph.cpp
    Graph.h
    Main.cpp
    Tourism.cpp
    Tourism.h)
