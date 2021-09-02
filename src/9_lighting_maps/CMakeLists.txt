add_executable(9_lighting_maps hello_lighting_maps.cpp)

target_link_libraries(9_lighting_maps
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )