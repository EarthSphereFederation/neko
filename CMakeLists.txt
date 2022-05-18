CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

PROJECT(NEKO)

#============= options =============

OPTION(NEKO_RHI_VULKAN "Enable Vulkan RHI backend" ON)

#============= global =============

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

function(NEKO_CONFIG_CXX_LANG)
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

#============= neko =============
find_package(Vulkan REQUIRED)

set(GLFW_BUILD_DOCS False)
set(GLFW_BUILD_EXAMPLES False)
set(GLFW_BUILD_TESTS False)
set(GLFW_VULKAN_STATIC True)
add_subdirectory(External/glfw) 

add_library(NEKO STATIC Source/Neko.cpp)
target_link_libraries(NEKO PRIVATE Vulkan::Vulkan)
NEKO_CONFIG_CXX_LANG(NEKO)

include_directories(Source/RHI/Include)
add_subdirectory(Source/RHI)

include_directories(Source/MiniCore/Include)
add_subdirectory(Source/MiniCore)

#============= samples =============

add_subdirectory(Samples)
