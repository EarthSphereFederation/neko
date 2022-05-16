cmake_policy(SET CMP0076 NEW)

find_package(Vulkan REQUIRED)

add_executable(neko_drawtriangle main.cpp)
target_link_libraries(neko_drawtriangle PRIVATE Vulkan::Vulkan neko_rhi glfw) 