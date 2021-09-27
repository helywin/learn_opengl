add_executable(16_cube_map cube_map.cpp)

target_link_libraries(16_cube_map
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )