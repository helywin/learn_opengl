add_executable(3_texture hello_texture.cpp)

target_link_libraries(3_texture
        ${OPENGL}
        glfw
        ${GLAD}
        )

target_compile_definitions(3_texture PRIVATE -DRES_DIR="${RES_DIR}")
