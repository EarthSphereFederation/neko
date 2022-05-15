cmake_policy(SET CMP0076 NEW)
if(BACKEND_VK)
    add_subdirectory(vulkan)
endif(BACKEND_VK)