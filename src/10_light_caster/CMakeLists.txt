add_executable(10_light_caster hello_light_caster.cpp)

target_link_libraries(10_light_caster
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )