add_executable(screenspace_shader src/screenspace_shader.cpp)
target_link_libraries(screenspace_shader PUBLIC SlickGFX PRIVATE -static)

add_executable(textured_rectangle src/textured_rectangle.cpp)
target_link_libraries(textured_rectangle PUBLIC SlickGFX PRIVATE -static)

add_custom_target(
	copy_examples ALL
	COMMAND
		${CMAKE_COMMAND} -E copy 
		${CMAKE_CURRENT_SOURCE_DIR}/screen_vertex.glsl
		${CMAKE_CURRENT_SOURCE_DIR}/screen_texture.fragment.glsl
		${CMAKE_CURRENT_SOURCE_DIR}/screen_fragment.glsl
		${CMAKE_CURRENT_SOURCE_DIR}/sample.exr
		${CMAKE_CURRENT_BINARY_DIR}
	COMMENT
		"Copying files for running examples.")