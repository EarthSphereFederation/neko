#include "ShaderReflection.h"
#ifdef DIRECTX_SUPPORT
#include "ShaderReflection/DXILReflection.h" // MAYBE never impl it >_<
#endif
#ifdef VULKAN_SUPPORT
#include "ShaderReflection/SPIRVReflection.h"
#endif
#include <cassert>

std::shared_ptr<IShaderReflection> CreateShaderReflection(EShaBlobType type, const void* data, size_t size)
{
    switch (type)
    {
#ifdef DIRECTX_SUPPORT
        case EShaBlobType::kDXIL:
            return std::make_shared<FDXILReflection>(data, size);
#endif
#ifdef VULKAN_SUPPORT
        case EShaBlobType::kSPIRV:
            return std::make_shared<FSPIRVReflection>(data, size);
#endif
    }
    assert(false);
    return nullptr;
}
