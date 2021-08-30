add_library(com STATIC
        Shader.cpp
        Texture2D.cpp)
target_link_libraries(com
        opengl32
        glfw
        glad::glad
        )