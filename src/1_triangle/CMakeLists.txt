add_executable(1_triangle hello_triangle.cpp)
target_link_libraries(1_triangle
        opengl32
        glfw
        glad::glad
        )