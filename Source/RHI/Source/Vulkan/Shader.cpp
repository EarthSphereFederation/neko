#include "Backend.h"
namespace Neko::RHI::Vulkan
{ 
    FShader::FShader(const FContext &ctx) : Context(ctx)
    {
    }

    FShader::~FShader()
    {
        if (ShaderModule)
        {
            vkDestroyShaderModule(Context.Device, ShaderModule, Context.AllocationCallbacks);
            ShaderModule = nullptr;
        }
    }

    bool FShader::Initalize(const FShaderDesc &InDesc)
    {
        Desc = InDesc;

        VkShaderModuleCreateInfo ShaderInfo = {};
        ShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        ShaderInfo.flags = 0;
        ShaderInfo.codeSize = Desc.Size;
        ShaderInfo.pCode = reinterpret_cast<const uint32_t *>(Desc.Blob);

        VK_CHECK_THROW(vkCreateShaderModule(Context.Device, &ShaderInfo, nullptr, &ShaderModule), "failed to create shader \"%s\"", Desc.DebugName);
        return true;
    }

    IShaderRef FDevice::CreateShader(const FShaderDesc &Desc)
    {
        auto Shader = RefCountPtr<FShader>(new FShader(Context));
        if (!Shader->Initalize(Desc))
        {
            Shader = nullptr;
        }
        return Shader;
    }
}
