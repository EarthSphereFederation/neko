#include "backend.h"
#include <assert.h>
#include <vector>
#include <map>
#pragma warning(disable:26812)
namespace neko
{
    namespace vk
    {
        VulkanContext::~VulkanContext()
        {
            if (Device)
            {
                vkDestroyDevice(Device, AllocationCallbacks);
                Device = nullptr;
            }
            if (Instance)
            {
                vkDestroyInstance(Instance, AllocationCallbacks);
                Instance = nullptr;
            }
        }

        VulkanDevice::VulkanDevice() :Context(new VulkanContext())
        {
        }  
        bool VulkanDevice::Initalize(const RHIDeviceDesc& desc)
        {
            VkApplicationInfo ApplicationInfo = {};
            ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            ApplicationInfo.apiVersion = VK_MAKE_VERSION(1, 3, 0);

            const char* LayerNames[] = { "VK_LAYER_KHRONOS_validation" };
            VkInstanceCreateInfo InstanceCreateInfo = {};
            InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
            InstanceCreateInfo.enabledExtensionCount = desc.VulkanDesc.InstanceExtensionNum;
            InstanceCreateInfo.ppEnabledExtensionNames = desc.VulkanDesc.InstanceExtensions;
            InstanceCreateInfo.enabledLayerCount = desc.Validation ? 1 : 0;
            InstanceCreateInfo.ppEnabledLayerNames = desc.Validation ? LayerNames : nullptr;

            VK_CHECK_RETURN_FALSE(vkCreateInstance(&InstanceCreateInfo, nullptr, &Context->Instance), "failed to create instance");

            uint32_t PhysicalDeviceCount;
            vkEnumeratePhysicalDevices(Context->Instance, &PhysicalDeviceCount, nullptr);
            std::vector<VkPhysicalDevice> PhysicalDevices;
            PhysicalDevices.resize(PhysicalDeviceCount);
            vkEnumeratePhysicalDevices(Context->Instance, &PhysicalDeviceCount, PhysicalDevices.data());

            CHECK(desc.GpuIndex < PhysicalDeviceCount);

            Context->PhysicalDevice = PhysicalDevices[desc.GpuIndex];
            Context->PhyDeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
            Context->PhyDeviceMemoryProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;

            vkGetPhysicalDeviceProperties2(Context->PhysicalDevice, &Context->PhyDeviceProperties);
            vkGetPhysicalDeviceMemoryProperties2(Context->PhysicalDevice, &Context->PhyDeviceMemoryProperties);

            uint32_t QueueFamilyCount;
            vkGetPhysicalDeviceQueueFamilyProperties2(Context->PhysicalDevice, &QueueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties2> queueFamilyProperties = {};
            queueFamilyProperties.resize(QueueFamilyCount);
            for (size_t i = 0; i < QueueFamilyCount; ++i)
            {
                queueFamilyProperties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
            }
            vkGetPhysicalDeviceQueueFamilyProperties2(Context->PhysicalDevice, &QueueFamilyCount, queueFamilyProperties.data());

            // find Graphic queue,Compute queue and Transfer queue
            struct QueueData
            {
                int32_t FamilyIndex;
                RHICmdQueueType Type;
                VkQueueFamilyProperties2 Properties;

                QueueData(int32_t familyIndex, RHICmdQueueType type, VkQueueFamilyProperties2 properties) :
                    FamilyIndex(familyIndex), Type(type), Properties(properties) {}
            };
            uint32_t UsedQueueFamliyMask = 0;
            std::vector<QueueData> QueueData;
            //graphic
            for (uint32_t q = 0; q < QueueFamilyCount; ++q)
            {
                auto queueFlags = queueFamilyProperties[q].queueFamilyProperties.queueFlags;
                if (queueFlags & (VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT))
                {
                    QueueData.emplace_back(q, RHICmdQueueType::Graphic, queueFamilyProperties[q]);
                    UsedQueueFamliyMask |= (1 << q);
                }
            }
            //compute
            for (uint32_t q = 0; q < QueueFamilyCount; ++q)
            {
                auto queueFlags = queueFamilyProperties[q].queueFamilyProperties.queueFlags;
                if (queueFlags & (VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT) && ((UsedQueueFamliyMask & (1 << q)) == 0))
                {
                    QueueData.emplace_back(q, RHICmdQueueType::Compute, queueFamilyProperties[q]);
                    UsedQueueFamliyMask |= (1 << q);
                }
            }
            //transfer
            for (uint32_t q = 0; q < QueueFamilyCount; ++q)
            {
                auto queueFlags = queueFamilyProperties[q].queueFamilyProperties.queueFlags;
                if (queueFlags & (VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT) && ((UsedQueueFamliyMask & (1 << q)) == 0))
                {
                    QueueData.emplace_back(q, RHICmdQueueType::Transfer, queueFamilyProperties[q]);
                    UsedQueueFamliyMask |= (1 << q);
                }
            }

            std::vector<VkDeviceQueueCreateInfo> QueueInfos = {};
            QueueInfos.resize(QueueData.size());

            const float Priorities[] = { 1.0f };
            for (int32_t i = 0; i < QueueData.size(); ++i)
            {
                auto& queueInfo = QueueInfos[i];
                auto& queue = QueueData[i];
                queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueInfo.pNext = nullptr;
                queueInfo.flags = 0;
                queueInfo.queueFamilyIndex = queue.FamilyIndex;
                queueInfo.queueCount = 1;
                queueInfo.pQueuePriorities = Priorities;
            }

            std::vector<const char*> Extensions;
            if (desc.Features.Swapchain)
            {
                Extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            }

            VkPhysicalDeviceFeatures Features = {};

            Context->DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            Context->DeviceInfo.pNext = nullptr;
            Context->DeviceInfo.queueCreateInfoCount = (uint32_t)QueueInfos.size();
            Context->DeviceInfo.pQueueCreateInfos = QueueInfos.data(),
                Context->DeviceInfo.enabledExtensionCount = (uint32_t)Extensions.size();
            Context->DeviceInfo.ppEnabledExtensionNames = Extensions.data();
            Context->DeviceInfo.pEnabledFeatures = &Features;
            Context->DeviceInfo.enabledLayerCount = 0;
            Context->DeviceInfo.ppEnabledLayerNames = nullptr;
            Context->DeviceInfo.flags = 0;

            VK_CHECK_RETURN_FALSE(vkCreateDevice(Context->PhysicalDevice, &Context->DeviceInfo, nullptr, &Context->Device), "failed to create device");
            
            for (auto& q : QueueData)
            {
                Queues.push_back(new VulkanQueue(Context, q.FamilyIndex, q.Type, q.Properties));
            }

            return true;
        }

        NativeObject VulkanDevice::GetVkInstance() const
        {
            return Context->Instance;
        }
    }

    RHIDeviceRef CreateDevice(const RHIDeviceDesc& desc)
    {
        auto DeviceRef = RefCountPtr<vk::VulkanDevice>(new vk::VulkanDevice());
        if (!DeviceRef->Initalize(desc))
        {
            DeviceRef = nullptr;
        }
        return DeviceRef;
    }
}