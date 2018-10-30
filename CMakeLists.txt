cmake_minimum_required(VERSION 3.2)
project(lut3d)

add_executable(lut3d lut3d.c)

target_link_libraries(lut3d -lm)