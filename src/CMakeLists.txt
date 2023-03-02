cmake_minimum_required(VERSION 3.14)

add_library(wbpls STATIC wbpls.c)
target_include_directories(wbpls PUBLIC ../include)

# testing binary
add_executable(mytest test.c wbpls.c)
target_link_libraries(mytest m)
target_include_directories(mytest PUBLIC ../include)

#target_link_libraries(cpp_test PRIVATE sum_integers)

# enable testing functionality
#enable_testing()

# define tests
add_test(mytest mytest)