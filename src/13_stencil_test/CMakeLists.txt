add_executable(13_stencil_test stencil_test.cpp)

target_link_libraries(13_stencil_test
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )