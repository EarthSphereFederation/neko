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

#============= external =============

list(PREPEND CMAKE_MODULE_PATH "${project_root}/Cmake")
include(CompilerSettings)
include(External/mimalloc)
include(External/glfw)

if(NEKO_RHI_VULKAN)
	find_package(Vulkan REQUIRED)
	include(External/spirv-cross)
endif()

#============= neko =============

add_subdirectory(Source)

#============= samples =============

add_subdirectory(Samples)
