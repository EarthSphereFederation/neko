#include "backend.h"
#pragma warning(disable : 26812)
namespace neko::rhi::vk
{
    VulkanShader::VulkanShader(const VulkanContextPtr &ctx) : Context(ctx)
    {
    }

    VulkanShader::~VulkanShader()
    {
        if (ShaderModule)
        {
            vkDestroyShaderModule(Context->Device, ShaderModule, Context->AllocationCallbacks);
            ShaderModule = nullptr;
        }
    }

    bool VulkanShader::Initalize(const RHIShaderDesc &Desc)
    {
        VkShaderModuleCreateInfo ShaderInfo = {};
        ShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        ShaderInfo.flags = 0;
        ShaderInfo.codeSize = Desc.Size;
        ShaderInfo.pCode = reinterpret_cast<const uint32_t *>(Desc.Blob);

        VK_CHECK_RETURN_FALSE(vkCreateShaderModule(Context->Device, &ShaderInfo, nullptr, &ShaderModule), "failed to create shader \"%s\"", Desc.DebugName);
        return true;
    }

    RHIShaderRef VulkanDevice::CreateShader(const RHIShaderDesc &Desc) const
    {
        auto Shader = RefCountPtr<vk::VulkanShader>(new VulkanShader(Context));
        if (!Shader->Initalize(Desc))
        {
            Shader = nullptr;
        }
        return Shader;
    }
}
