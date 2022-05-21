cmake_minimum_required(VERSION 3.20)

project(NEKO)

#============= options =============

option(NEKO_RHI_VULKAN "Enable Vulkan RHI backend" ON)

#============= global =============

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(project_root "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(PREPEND CMAKE_MODULE_PATH "${project_root}/Cmake")
include(CompilerSettings)

#============= external =============

add_subdirectory(External/mimalloc)
set_target_properties(mimalloc-static PROPERTIES FOLDER "External")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(External/glfw) 
set_target_properties(glfw update_mappings PROPERTIES FOLDER "External")

if(NEKO_RHI_VULKAN)
	find_package(Vulkan REQUIRED)
	include(External/spirv-cross)
endif()

#============= neko =============

add_subdirectory(Source)

#============= samples =============

add_subdirectory(Samples)
