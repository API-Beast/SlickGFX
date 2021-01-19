set(SOURCES AttributeBuffer.cpp RenderPass.cpp Framebuffer.cpp GL.cpp Texture.cpp Types.cpp ShaderProgram.cpp)
list(TRANSFORM SOURCES REPLACE \.cpp \.hpp OUTPUT_VARIABLE HEADERS)

add_library(sGFXCore STATIC ${SOURCES} ${HEADERS} GL/flextGL.c GL/flextGL.h GL/khrplatform.h)

find_package(OpenGL REQUIRED)
target_link_libraries(sGFXCore PUBLIC glfw ${OPENGL_LIBRARIES})

# We use custom attributes
target_compile_options(sGFXCore PUBLIC "-Wno-attributes")