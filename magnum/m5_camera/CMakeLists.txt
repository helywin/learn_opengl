add_executable(m5_camera m5_camera.cpp)
target_link_libraries(m5_camera
        Corrade::Main
        Magnum::GL
        Magnum::Magnum
        Magnum::MeshTools
        Magnum::SceneGraph
        Magnum::Shaders
        Magnum::Trade
        Magnum::GlfwApplication
        glfw
        )