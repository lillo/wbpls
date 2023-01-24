cmake_minimum_required(VERSION 3.14.0)
project(wbpls VERSION 0.1.0 DESCRIPTION "An example of LLVM Transformation" LANGUAGES C)

set(CMAKE_C_STANDARD_REQUIRED 99)

add_subdirectory(src)

include(CTest)
enable_testing()



set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
