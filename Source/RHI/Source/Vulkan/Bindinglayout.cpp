#include "Backend.h"
namespace Neko::Vulkan
{ 
    FBindingLayout::FBindingLayout(const VulkanContextPtr &ctx) : Context(ctx)
    {
    }

    FBindingLayout::~FBindingLayout()
    {
        if (DescriptorSetLayout)
        {
            vkDestroyDescriptorSetLayout(Context->Device, DescriptorSetLayout, Context->AllocationCallbacks);
            DescriptorSetLayout = nullptr;
        }
    }

    bool FBindingLayout::Initalize(const RHIBindingLayoutDesc &desc)
    {
        std::vector<VkDescriptorSetLayoutBinding> LayoutBindings;
        LayoutBindings.reserve(desc.BindingArray.size());

        for (auto &binding : desc.BindingArray)
        {
            VkDescriptorSetLayoutBinding BingDesc = {};
            BingDesc.binding = binding.Binding;
            BingDesc.descriptorType = ConvertToVkDescriptorType(binding.ResourceType);
            BingDesc.descriptorCount = 1;
            BingDesc.stageFlags = ConvertToVkShaderStageFlags(desc.ShaderStage);
            BingDesc.pImmutableSamplers = nullptr;
            LayoutBindings.push_back(BingDesc);
        }

        VkDescriptorSetLayoutCreateInfo LayoutInfo = {};
        LayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        LayoutInfo.bindingCount = (uint32_t)LayoutBindings.size();
        LayoutInfo.pBindings = LayoutBindings.data();

        VK_CHECK_RETURN_FALSE(vkCreateDescriptorSetLayout(Context->Device, &LayoutInfo, nullptr, &DescriptorSetLayout), "failed to create DescriptorSetLayout");
        return true;
    }

    RHIBindingLayoutRef FDevice::CreateBindingLayout(const RHIBindingLayoutDesc &Desc) const
    {
        auto BindingLayout = RefCountPtr<FBindingLayout>(new FBindingLayout(Context));
        if (!BindingLayout->Initalize(Desc))
        {
            BindingLayout = nullptr;
        }
        return BindingLayout;
    }
}