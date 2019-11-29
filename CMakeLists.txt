# CMakeLists.txt for Utf8Tests

cmake_minimum_required(VERSION 3.8)
set(This Utf8)

set(Headers
	include/Utf8/Utf8.hpp
)

set(Sources
	src/Utf8.cpp
)

add_library(${This} STATIC ${Sources} ${Headers})
set_target_properties(${This} PROPERTIES
    FOLDER Libraries
)

target_include_directories(${This} PUBLIC include)

add_subdirectory(test)