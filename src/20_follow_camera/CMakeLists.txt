add_executable(20_follow_camera follow_camera.cpp)

target_link_libraries(20_follow_camera
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )