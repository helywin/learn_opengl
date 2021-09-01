add_executable(6_camera hello_camera.cpp)

target_link_libraries(6_camera
        ${OPENGL}
        glfw
        ${GLAD}
        com
        )

add_executable(6_camera1 hello_camera1.cpp)

target_link_libraries(6_camera1
        ${OPENGL}
        glfw
        ${GLAD}
        com
        )