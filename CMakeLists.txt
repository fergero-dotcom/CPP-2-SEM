cmake_minimum_required(VERSION 3.16)
project(CPP_2_SEM LANGUAGES CXX)

add_executable(TacticalRPG main.cpp)
target_compile_features(TacticalRPG PRIVATE cxx_std_17)
set_target_properties(TacticalRPG PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(TacticalRPG PRIVATE /utf-8 /W4)
else()
    target_compile_options(TacticalRPG PRIVATE -Wall -Wextra -pedantic)
endif()
