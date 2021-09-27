add_executable(14_face_culling face_culling.cpp)

target_link_libraries(14_face_culling
        ${OPENGL}
        glfw
        ${GLAD}
        com
        imgui
        )