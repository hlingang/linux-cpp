cmake_minimum_required(VERSION 3.10)
project("swig_test")
cmake_policy(SET CMP0078 NEW)
cmake_policy(SET CMP0086 NEW)

find_package(Python3 REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include 
${PYTHON_INCLUDE_PATH})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)

file(GLOB_RECURSE c_files src/*.cpp)
file(GLOB_RECURSE swig_files swig/*.i)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra -Wno-dev -fPIC -g -ggdb -O0")

add_library(test_lib_static STATIC ${c_files})
add_library(test_lib_shared SHARED ${c_files})
set_target_properties(test_lib_static PROPERTIES OUTPUT_NAME ctestlib)
set_target_properties(test_lib_shared PROPERTIES OUTPUT_NAME ctestlib)

set_source_files_properties(${swig_files} PROPERTIES CPLUSPLUS ON)
# set_property(SOURCE ${swig_files} PROPERTY CPLUSPLUS ON)
swig_add_library(ctestlib TYPE MODULE 
LANGUAGE python SOURCES ${swig_files} 
OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/python 
OUTFILE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/python/cxx)
swig_link_libraries(ctestlib libctestlib.a ${PYTHON_LIBRARIES})
