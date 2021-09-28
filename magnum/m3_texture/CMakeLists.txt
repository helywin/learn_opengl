add_executable(m3_texture m3_texture.cpp)
target_link_libraries(m3_texture
        Corrade::Main
        Magnum::Application
        Magnum::GL
        Magnum::Magnum
        Magnum::Shaders
        Magnum::Trade)