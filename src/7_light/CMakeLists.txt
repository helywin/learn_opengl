add_executable(7_light hello_light.cpp)

target_link_libraries(7_light
        ${OPENGL}
        glfw
        ${GLAD}
        com
        )