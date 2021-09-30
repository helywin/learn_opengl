add_executable(17_tessellation tessllation.cpp)
target_link_libraries(17_tessellation
        ${OPENGL}
        ${GLAD}
        glfw
        )
