add_executable(m2_shader m2_shader.cpp)
target_link_libraries(m2_shader
        Corrade::Main
        Magnum::Sdl2Application
        Magnum::GL
        Magnum::Magnum
        Magnum::Shaders)