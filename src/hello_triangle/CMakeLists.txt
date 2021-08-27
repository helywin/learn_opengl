add_executable(hello_triangle hello_triangle.cpp)
target_link_libraries(hello_triangle
        opengl32
        glfw
        glad::glad
        )