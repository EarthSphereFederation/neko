#rhi module
add_library(neko_rhi INTERFACE)

if(BACKEND_VK)
    target_link_libraries(neko_rhi INTERFACE neko_rhi_vulkan) 
endif(BACKEND_VK)

include_directories(public)
add_subdirectory(private)

