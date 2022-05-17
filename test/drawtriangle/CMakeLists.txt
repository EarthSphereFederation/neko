cmake_policy(SET CMP0076 NEW)

find_package(Vulkan REQUIRED)

add_executable(NekoDrawTriangle main.cpp)
target_link_libraries(NekoDrawTriangle PRIVATE Vulkan::Vulkan RHI glfw) 

file(GLOB VERTEX_SHADERS Shaders/*.vert)
file(GLOB FRAGMENT_SHADERS Shaders/*.frag)
  
set(SHADER_COMPILER glslc)
foreach(SHADER_PATH ${VERTEX_SHADERS})
    string(REGEX REPLACE ".+/(.+)" "\\1" SHADER_NAME ${SHADER_PATH})
    execute_process(COMMAND ${SHADER_COMPILER} ${SHADER_PATH} -o ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.spv) 
endforeach(SHADER_PATH)

foreach(SHADER_PATH ${FRAGMENT_SHADERS})
    string(REGEX REPLACE ".+/(.+)" "\\1" SHADER_NAME ${SHADER_PATH})
    execute_process(COMMAND ${SHADER_COMPILER} ${SHADER_PATH} -o ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.spv) 
endforeach(SHADER_PATH) 