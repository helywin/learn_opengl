add_executable(m6_height_map m6_height_map.cpp)
target_link_libraries(m6_height_map
        Corrade::Main
        Magnum::GlfwApplication
        Magnum::GL
        Magnum::Magnum
        Magnum::Shaders
        Magnum::SceneGraph
        glfw
        Magnum::Trade
        )