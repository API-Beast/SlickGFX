cmake_minimum_required(VERSION 3.16)
project(SlickGFX LANGUAGES CXX C)

# This is an internal CMake variable, we just expose it to the GUI.
# It affects all library that have no type (e.g. no SHARED or STATIC)
option(BUILD_SHARED_LIBS "Build libraries as shared." OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(GLFW_BUILD_DOCS OFF)
add_subdirectory(dependencies/glfw)
add_subdirectory(sGFX)

# Only build examples and testing if not a sub-project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	option(BUILD_EXAMPLES "Build examples." ON)
	option(BUILD_TESTS    "Build tests." ON)
else()
	set(BUILD_EXAMPLES OFF)
	set(BUILD_TESTS OFF)
endif()

if(BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

if(BUILD_TESTS)
	add_subdirectory(test)
endif()

# -------
# Library
# -------
add_library(SlickGFX slick/slick.hpp)
target_link_libraries(SlickGFX sGFXCore sGFXResources)
target_include_directories(SlickGFX INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(SlickGFX PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

install(TARGETS SlickGFX)