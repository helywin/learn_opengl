add_executable(3_texture hello_texture.cpp)

target_link_libraries(3_texture
        opengl32
        glfw
        glad::glad
        )

target_compile_definitions(3_texture PRIVATE -DRES_DIR="${RES_DIR}")