add_subdirectory(${project_root}/External/SPIRV-Cross EXCLUDE_FROM_ALL)
set_target_properties(spirv-cross PROPERTIES FOLDER "External/spirv-cross")
set_target_properties(spirv-cross-core PROPERTIES FOLDER "External/spirv-cross")
set_target_properties(spirv-cross-cpp PROPERTIES FOLDER "External/spirv-cross")
set_target_properties(spirv-cross-glsl PROPERTIES FOLDER "External/spirv-cross")
set_target_properties(spirv-cross-hlsl PROPERTIES FOLDER "External/spirv-cross")
set_target_properties(spirv-cross-msl PROPERTIES FOLDER "External/spirv-cross")
set_target_properties(spirv-cross-reflect PROPERTIES FOLDER "External/spirv-cross")
set_target_properties(spirv-cross-util PROPERTIES FOLDER "External/spirv-cross")
set_target_properties(spirv-cross-c PROPERTIES FOLDER "External/spirv-cross")
