add_library(com STATIC
        Shader.cpp
        Texture2D.cpp
        Model.cpp
        Mesh.cpp)
target_link_libraries(com
        ${OPENGL}
        glfw
        ${GLAD}
        assimp::assimp
        )

target_include_directories(com PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})