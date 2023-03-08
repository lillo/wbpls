cmake_minimum_required(VERSION 3.14.0)
project(wbpls VERSION 0.1.0 DESCRIPTION "A library providing physical layer security primitives" LANGUAGES C)

set(CMAKE_C_STANDARD_REQUIRED 99)

include(CTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
