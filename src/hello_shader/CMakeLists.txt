add_executable(hello_shader hello_shader.cpp)
target_link_libraries(hello_shader
        opengl32
        glfw
        glad::glad
        )