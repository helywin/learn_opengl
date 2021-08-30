add_library(shader STATIC Shader.cpp)
target_link_libraries(shader
        opengl32
        glfw
        glad::glad
        )