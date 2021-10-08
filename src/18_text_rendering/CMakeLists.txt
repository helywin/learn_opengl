add_executable(18_text_rendering text_rendering.cpp)
target_link_libraries(18_text_rendering
        ${OPENGL}
        ${GLAD}
        glfw
        freetype
        com
        )
