add_library(com STATIC
        Shader.cpp
        Texture2D.cpp)
target_link_libraries(com
        ${OPENGL}
        glfw
        ${GLAD}
        )

target_include_directories(com PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})