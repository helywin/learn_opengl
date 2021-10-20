add_executable(m7_displacement m7_displacement.cpp)
target_link_libraries(m7_displacement
        Corrade::Main
        Magnum::GlfwApplication
        Magnum::GL
        Magnum::Magnum
        Magnum::Shaders
        Magnum::SceneGraph
        Magnum::Trade
        )