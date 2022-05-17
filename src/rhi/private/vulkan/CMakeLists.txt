cmake_policy(SET CMP0076 NEW)

find_package(Vulkan REQUIRED)

add_library(RHI_Vulkan)
target_link_libraries(RHI_Vulkan PRIVATE Vulkan::Vulkan)
target_sources(
    RHI_Vulkan PRIVATE
    Backend.h
    Device.cpp
    Cmdlist.cpp
    Swapchain.cpp
    Shader.cpp
    Pipeline.cpp
    Bindinglayout.cpp
    Framebuffer.cpp
) 


