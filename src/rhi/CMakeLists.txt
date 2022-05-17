#rhi module
add_library(RHI INTERFACE)

if(BACKEND_VK)
    target_link_libraries(RHI INTERFACE RHI_Vulkan) 
endif(BACKEND_VK)

include_directories(Public)
add_subdirectory(Private)

