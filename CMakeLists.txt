cmake_minimum_required(VERSION 3.10)
project(SnakeGame C)

set(CMAKE_C_STANDARD 99)

add_executable(SnakeGame main.c snake.h snake.c)