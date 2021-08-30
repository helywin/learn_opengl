add_executable(2_shader hello_shader.cpp)
target_link_libraries(2_shader
        opengl32
        glfw
        glad::glad
        )