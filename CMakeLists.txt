cmake_minimum_required(VERSION 3.16)
project(Mandelbrot)

set(CMAKE_CXX_STANDARD 11)

add_executable(Mandelbrot main.cpp MessageHandler.cpp MessageHandler.h)