cmake_minimum_required(VERSION 3.9)
project(oslab2)

set(CMAKE_CXX_STANDARD 11)

add_executable(oslab2 main.cpp process.h process.cpp tokenizer.h tokenizer.cpp)