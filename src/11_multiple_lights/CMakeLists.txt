add_executable(11_multiple_lights hello_multiple_lights.cpp)

target_link_libraries(11_multiple_lights
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )