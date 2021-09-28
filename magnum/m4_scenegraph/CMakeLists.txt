add_executable(m4_scenegraph m4_scenegraph.cpp)
target_link_libraries(m4_scenegraph
        Corrade::Main
        Magnum::Application
        Magnum::GL
        Magnum::Magnum
        Magnum::MeshTools
        Magnum::SceneGraph
        Magnum::Shaders
        Magnum::Trade
        )