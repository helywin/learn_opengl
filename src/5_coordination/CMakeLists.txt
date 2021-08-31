add_executable(5_coordination hello_coordination.cpp)

target_link_libraries(5_coordination
        ${OPENGL}
        glfw
        ${GLAD}
        com
        )

target_compile_definitions(5_coordination PRIVATE -DRES_DIR="${RES_DIR}")
target_compile_definitions(5_coordination PRIVATE -DROOT_DIR="${CMAKE_SOURCE_DIR}")