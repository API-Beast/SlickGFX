add_executable(screenspace_shader src/screenspace_shader.cpp)
target_link_libraries(screenspace_shader PUBLIC SlickGFX)

if(NOT BUILD_SHARED_LIBS)
	target_link_libraries(screenspace_shader PRIVATE -static-libstdc++ -static-libgcc -static winpthread)
endif()

add_custom_target(
	copy_examples ALL
	COMMAND
		${CMAKE_COMMAND} -E copy 
		${CMAKE_CURRENT_SOURCE_DIR}/screen_vertex.glsl
		${CMAKE_CURRENT_SOURCE_DIR}/screen_fragment.glsl
		${CMAKE_CURRENT_BINARY_DIR}
	COMMENT
		"Copying files for running examples.")