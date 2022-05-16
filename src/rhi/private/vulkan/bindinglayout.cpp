#include "backend.h"
#pragma warning(disable : 26812)
namespace neko::rhi::vk
{
    VulkanBindingLayout::VulkanBindingLayout(const VulkanContextPtr &ctx) : Context(ctx)
    {
    }

    VulkanBindingLayout::~VulkanBindingLayout()
    {
        if (DescriptorSetLayout)
        {
            vkDestroyDescriptorSetLayout(Context->Device, DescriptorSetLayout, Context->AllocationCallbacks);
            DescriptorSetLayout = nullptr;
        }
    }

    bool VulkanBindingLayout::Initalize(const RHIBindingLayoutDesc &desc)
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

    RHIBindingLayoutRef VulkanDevice::CreateBindingLayout(const RHIBindingLayoutDesc &Desc) const
    {
        auto BindingLayout = RefCountPtr<vk::VulkanBindingLayout>(new VulkanBindingLayout(Context));
        if (!BindingLayout->Initalize(Desc))
        {
            BindingLayout = nullptr;
        }
        return BindingLayout;
    }
}