add_executable(19_geometry_shader geometry_shader.cpp)
target_link_libraries(19_geometry_shader
        ${OPENGL}
        glfw
        ${GLAD}
        )
