add_executable(7_light hello_light.cpp)

target_link_libraries(7_light
        ${OPENGL}
        glfw
        ${GLAD}
        com
        )

add_executable(7_light_imgui hello_light_imgui.cpp)

target_link_libraries(7_light_imgui
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )