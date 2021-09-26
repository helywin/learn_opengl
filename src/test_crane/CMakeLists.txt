add_executable(test_crane test_crane.cpp)

target_link_libraries(test_crane
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )