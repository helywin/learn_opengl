add_executable(12_model hello_model.cpp)

target_link_libraries(12_model
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )