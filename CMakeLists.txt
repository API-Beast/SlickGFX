cmake_minimum_required(VERSION 3.16)
project(libSpringGFX LANGUAGES CXX C)

# This is an internal CMake variable, we just expose it to the GUI.
# It affects all library that have no type (e.g. no SHARED or STATIC)
option(BUILD_SHARED_LIBS "Build libraries as shared." OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

add_library(libSpringGFX)

# GLFW

add_subdirectory(dependencies/glfw)

add_subdirectory(sGFX)

target_link_libraries(libSpringGFX sGFXCore sGFXResources)
set_target_properties(libSpringGFX PROPERTIES
	WINDOWS_EXPORT_ALL_SYMBOLS ON
	PREFIX "")

install(TARGETS libSpringGFX)

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
	add_subdirectory(testing)
endif()