cmake_minimum_required(VERSION 3.0.0)
project(neko VERSION 0.1.0)

include(CTest)
enable_testing()

option(BACKEND_VK "use vulkan as backend"  ON)
if(BACKEND_VK)
    add_definitions(-DNEKO_VULKAN)
endif(BACKEND_VK)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)
add_subdirectory(test)

set(GLFW_BUILD_DOCS False)
set(GLFW_BUILD_EXAMPLES False)
set(GLFW_BUILD_TESTS False)
set(GLFW_VULKAN_STATIC True)
add_subdirectory(ext/glfw) 

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
