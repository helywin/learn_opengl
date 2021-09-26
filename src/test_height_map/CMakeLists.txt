add_executable(test_height_map test_height_map.cpp)

target_link_libraries(test_height_map
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )