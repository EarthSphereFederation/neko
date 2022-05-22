add_definitions(-DASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/")
file(GLOB HLSL_SHADERS Shaders/*.hlsl)
add_executable(ShaderReflectionSample main.cpp)
target_link_libraries(ShaderReflectionSample PRIVATE
    HLSLCompiler
    ShaderReflection)
NEKO_CONFIG_CXX_LANG(NekoDrawTriangle)