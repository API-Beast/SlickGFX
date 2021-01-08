set(SOURCES RenderPass.cpp Framebuffer.cpp GL.cpp Texture.cpp)
list(TRANSFORM SOURCES REPLACE \.cpp \.hpp OUTPUT_VARIABLE HEADERS)
list(APPEND HEADERS Types.hpp)

add_library(sGFXCore STATIC ${SOURCES} ${HEADERS} GL/flextGL.c GL/flextGL.h GL/khrplatform.h)

find_package(OpenGL REQUIRED)
target_link_libraries(sGFXCore PUBLIC glfw ${OPENGL_LIBRARIES})