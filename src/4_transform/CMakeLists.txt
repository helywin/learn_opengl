add_executable(4_transform hello_transform.cpp)

target_link_libraries(4_transform
        ${OPENGL}
        glfw
        ${GLAD}
        )
