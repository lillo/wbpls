cmake_minimum_required(VERSION 3.14)


# testing binary
add_executable(wbpls-test test.c)
target_include_directories(wbpls-test PUBLIC ../include ../src)
target_link_libraries(wbpls-test PRIVATE wbpls)

add_test(wbpls-test wbpls-test)