cmake_minimum_required(VERSION 3.20)

project(NEKO)

#============= options =============

option(NEKO_RHI_VULKAN "Enable Vulkan RHI backend" ON)

#============= global =============

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

function(neko_config_cxx_lang)
    set(TARGET_NAME ${ARGV0})
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        target_compile_definitions(${TARGET_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
        set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 23)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_SIMULATE_ID MATCHES "MSVC")
        target_compile_definitions(${TARGET_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
        set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 23)
    else()
    	set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
    endif()
    set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
endfunction()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Xclang -fexceptions -Xclang -fcxx-exceptions")
endif()

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
endif()

#============= neko =============

add_subdirectory(Source)

#============= samples =============

add_subdirectory(Samples)
