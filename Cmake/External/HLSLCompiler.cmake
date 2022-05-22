set(HLSLCOMPILER_ENABLE_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory(External/HLSLCompiler)
set_target_properties(HLSLCompiler PROPERTIES FOLDER "External/HLSLCompiler")