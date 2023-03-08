cmake_minimum_required(VERSION 3.14)

add_library(wbpls STATIC wbpls.c)
target_include_directories(wbpls PUBLIC ../include)