add_executable(m10_text m10_text.cpp)
target_link_libraries(m10_text
        Corrade::Main
        Magnum::Sdl2Application
        Magnum::GL
        Magnum::Magnum
        Magnum::Text
        Magnum::Shaders
        glfw
        Magnum::Trade
        freetype)