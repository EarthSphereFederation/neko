set(ASSIMP_NO_EXPORT ON CACHE BOOL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "")
add_subdirectory(External/assimp)
set_target_properties(assimp PROPERTIES FOLDER "External/assimp")