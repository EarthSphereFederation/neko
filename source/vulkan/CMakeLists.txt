cmake_policy(SET CMP0076 NEW)

find_package(Vulkan REQUIRED)

add_library(neko_vulkan)
target_link_libraries(neko_vulkan PRIVATE Vulkan::Vulkan)
target_sources(
    neko_vulkan PRIVATE
    backend.h
    device.cpp
    cmdlist.cpp
    swapchain.cpp
    shader.cpp
    pipeline.cpp
    bindinglayout.cpp
) 


