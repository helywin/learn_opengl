add_executable(3_texture hello_texture.cpp)

target_link_libraries(3_texture
        ${OPENGL}
        glfw
        ${GLAD}
        )
