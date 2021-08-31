add_executable(2_shader hello_shader.cpp)
target_link_libraries(2_shader
        ${OPENGL}
        glfw
        ${GLAD}
        )
