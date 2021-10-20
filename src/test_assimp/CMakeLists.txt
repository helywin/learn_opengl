add_executable(test_assimp
        test_assimp.cpp
        )

target_link_libraries(test_assimp
        ${ASSIMP}
        com
        )