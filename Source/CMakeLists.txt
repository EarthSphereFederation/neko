# collect sources

file(GLOB_RECURSE SRC
		"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/*.inl"
		"${CMAKE_CURRENT_SOURCE_DIR}/*.h")
		
file(GLOB_RECURSE VULKAN_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/RHI/Source/Vulkan/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/RHI/Source/Vulkan/*.inl"
        "${CMAKE_CURRENT_SOURCE_DIR}/RHI/Source/Vulkan/*.h")

if(NOT NEKO_RHI_VULKAN)
    list(REMOVE_ITEM SRC ${VULKAN_SRC})
endif()

# add neko target

add_library(Neko STATIC ${SRC})

# group sources

foreach(_SRC IN ITEMS ${SRC})
    get_filename_component(SRC "${_SRC}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/MiniCore/Include/MiniCore" "MiniCore/Include" _GRP_PATH "${SRC}")
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/RHI/Include/RHI" "RHI/Include" _GRP_PATH "${_GRP_PATH}")
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/OS/Include/OS" "OS/Include" _GRP_PATH "${_GRP_PATH}")
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" _GRP_PATH "${_GRP_PATH}")
    string(REPLACE "/" "\\" _GRP_PATH "${_GRP_PATH}")
    source_group("${_GRP_PATH}" FILES "${_SRC}")
endforeach()

# specify compiler flags

neko_config_cxx_lang(Neko)

if(NEKO_RHI_VULKAN)
    target_compile_definitions(Neko PUBLIC NEKO_RHI_VULKAN)
endif()

# include & link

target_include_directories(Neko PUBLIC OS/Include)
target_include_directories(Neko PUBLIC RHI/Include)
target_include_directories(Neko PUBLIC MiniCore/Include)
target_link_libraries(Neko PRIVATE glfw)
target_link_libraries(Neko PUBLIC mimalloc-static)
if(NEKO_RHI_VULKAN)
	target_link_libraries(Neko PRIVATE Vulkan::Vulkan)
endif()
