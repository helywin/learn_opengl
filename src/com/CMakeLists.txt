add_library(com STATIC
        Shader.cpp
        Texture2D.cpp)
target_link_libraries(com
        ${OPENGL}
        glfw
        ${GLAD}
        )
