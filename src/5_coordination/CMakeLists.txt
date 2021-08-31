add_executable(5_coordination hello_coordination.cpp)

target_link_libraries(5_coordination
        ${OPENGL}
        glfw
        ${GLAD}
        com
        )

add_executable(5_coordination_cube hello_coordination_cube.cpp)

target_link_libraries(5_coordination_cube
        ${OPENGL}
        glfw
        ${GLAD}
        com
        )