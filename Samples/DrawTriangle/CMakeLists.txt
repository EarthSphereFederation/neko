add_definitions(-DASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/")
add_executable(NekoDrawTriangle main.cpp)
target_link_libraries(NekoDrawTriangle PRIVATE
    Vulkan::Vulkan
    Neko
    glfw
    HLSLCompiler)
NEKO_CONFIG_CXX_LANG(NekoDrawTriangle)
