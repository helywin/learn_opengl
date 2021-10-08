add_executable(m9_box2d m9_box2d.cpp)
target_link_libraries(m9_box2d
        Corrade::Main
        Magnum::GlfwApplication
        Magnum::GL
        Magnum::Magnum
        Magnum::Shaders
        glfw
        Magnum::Trade
        box2d::box2d
        )