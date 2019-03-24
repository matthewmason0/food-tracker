cmake_minimum_required(VERSION 3.13)
project(food_tracker C)

set(CMAKE_C_STANDARD 11)

add_executable(food_tracker main.c)
target_link_libraries(food_tracker ncurses)