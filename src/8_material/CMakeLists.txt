add_executable(8_material hello_material.cpp)

target_link_libraries(8_material
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )