if(NEKO_RHI_VULKAN)
    set(VULKAN_SUPPORT ON)
    add_definitions(-DVULKAN_SUPPORT)
endif()

list(APPEND headers
    BaseTypes.h
    ShaderReflection.h
    )
list(APPEND sources
    ShaderReflection.cpp
    )

if (VULKAN_SUPPORT)
    list(APPEND headers
        SPIRVReflection.h
        )
    list(APPEND sources
        SPIRVReflection.cpp
        )
endif()

add_library(ShaderReflection ${headers} ${sources})

target_include_directories(ShaderReflection
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    )

if (VULKAN_SUPPORT)
    target_link_libraries(ShaderReflection
        spirv-cross-core
        spirv-cross-hlsl
        )
endif()
