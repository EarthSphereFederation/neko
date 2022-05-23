#include "Backend.h"
namespace Neko::RHI::Vulkan
{ 
    FBindingLayout::FBindingLayout(const FContext &ctx) : Context(ctx)
    {
    }

    FBindingLayout::~FBindingLayout()
    {
        if (DescriptorSetLayout)
        {
            vkDestroyDescriptorSetLayout(Context.Device, DescriptorSetLayout, Context.AllocationCallbacks);
            DescriptorSetLayout = nullptr;
        }
    }

    bool FBindingLayout::Initalize(const FBindingLayoutDesc &desc)
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

        if (vkCreateDescriptorSetLayout(Context.Device, &LayoutInfo, nullptr, &DescriptorSetLayout))
        {
            throw OS::FOSException("Failed to create DescriptorSetLayout");
        } 
        return true;
    }

    IBindingLayoutRef FDevice::CreateBindingLayout(const FBindingLayoutDesc &Desc)
    {
        auto BindingLayout = RefCountPtr<FBindingLayout>(new FBindingLayout(Context));
        if (!BindingLayout->Initalize(Desc))
        {
            BindingLayout = nullptr;
        }
        return BindingLayout;
    }
}