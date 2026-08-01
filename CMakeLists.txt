cmake_minimum_required(VERSION 3.10)

project(
  2048

  VERSION 1.0.0.0
  DESCRIPTION "Simple 2048 game"
  LANGUAGES C
)

set(AUTHOR_NAME shaun)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_executable(
  # Name of executable. Using project name.
  ${PROJECT_NAME}

  # Source of executable
  main.c
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC "include"
)

