set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
add_subdirectory(${project_root}/External/volk)
set_target_properties(volk PROPERTIES FOLDER "External/volk")