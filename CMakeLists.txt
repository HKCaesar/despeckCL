cmake_minimum_required(VERSION 3.7)

ENABLE_LANGUAGE(C)

project(despeckCL CXX)

add_definitions(-DPERF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fPIC -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -Wextra")# -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


find_package(OpenCL REQUIRED)
if(OpenCL_FOUND)
    include_directories(${OpenCL_INCLUDE_DIRS})
    link_directories(${OpenCL_LIBRARY_DIR})
endif()

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

subdirs(src)

# Python Bindings
subdirs(swig)

# External libraries GoogleTest and clFFT
subdirs(ext)
enable_testing() 

# documentation
find_package(Sphinx)
if(Sphinx_FOUND OR SPHINX_FOUND)
    subdirs(doc)
endif()
