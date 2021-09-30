add_executable(m8_tessellation m8_tessellation.cpp)
target_link_libraries(m8_tessellation
        Corrade::Main
        Magnum::GlfwApplication
        Magnum::GL
        Magnum::Magnum
        Magnum::Shaders
        glfw)