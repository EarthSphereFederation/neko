if(${NEKO_SHADER_DEV})
    add_definitions(-DNEKO_SHADER_DEV)
    add_definitions(-DASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
endif()
add_executable(NekoDrawTriangle main.cpp)
target_link_libraries(NekoDrawTriangle PRIVATE
    Neko
    glfw
    HLSLCompiler)
NEKO_CONFIG_CXX_LANG(NekoDrawTriangle)
