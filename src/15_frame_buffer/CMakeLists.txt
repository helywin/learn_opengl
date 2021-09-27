add_executable(15_frame_buffer frame_buffer.cpp)

target_link_libraries(15_frame_buffer
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )