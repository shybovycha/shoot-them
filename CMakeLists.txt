cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(shoot-them VERSION 1.0.2 LANGUAGES CXX)

# generate JSON compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(game)
add_subdirectory(editor)
