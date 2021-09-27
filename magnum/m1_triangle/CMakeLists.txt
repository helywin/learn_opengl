add_executable(m1_triangle m1_triangle.cpp)
target_link_libraries(m1_triangle
        Corrade::Main
        Magnum::Application
        Magnum::GL
        Magnum::Magnum
        Magnum::Shaders)