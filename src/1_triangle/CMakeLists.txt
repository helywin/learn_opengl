add_executable(1_triangle hello_triangle.cpp)
target_link_libraries(1_triangle
        ${OPENGL}
        glfw
        ${GLAD}
        )
