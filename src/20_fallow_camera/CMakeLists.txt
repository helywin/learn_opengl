add_executable(20_fallow_camera fallow_camera.cpp)

target_link_libraries(20_fallow_camera
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )