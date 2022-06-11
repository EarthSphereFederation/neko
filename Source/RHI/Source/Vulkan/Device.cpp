#include "Backend.h"
#include <cassert>
#include <map>
#include <vector>
#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS  0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include "vk_mem_alloc.h"

namespace Neko::RHI
{ 
    namespace Vulkan
    {
        FContext::~FContext()
        {
            if (Allocator)
            {
                vmaDestroyAllocator(Allocator);
            }
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

        FDevice::FDevice()
        {
        }
        bool FDevice::Initalize(const FDeviceDesc &desc)
        {
            VkApplicationInfo ApplicationInfo = {};
            ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            ApplicationInfo.apiVersion = VK_MAKE_VERSION(1, 3, 0);

            const char *LayerNames[] = {"VK_LAYER_KHRONOS_validation"};
            VkInstanceCreateInfo InstanceCreateInfo = {};
            InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
            InstanceCreateInfo.enabledExtensionCount = desc.VulkanDesc.InstanceExtensionNum;
            InstanceCreateInfo.ppEnabledExtensionNames = desc.VulkanDesc.InstanceExtensions;
            InstanceCreateInfo.enabledLayerCount = desc.Validation ? 1 : 0;
            InstanceCreateInfo.ppEnabledLayerNames = desc.Validation ? LayerNames : nullptr;

            VK_CHECK_THROW(vkCreateInstance(&InstanceCreateInfo, nullptr, &Context.Instance), "failed to create instance");

            volkLoadInstance(Context.Instance);

            // enumerate layer Properties
            uint32_t LayerPropertiesCount = 0;
            std::vector<VkLayerProperties> LayerProperties;
            vkEnumerateInstanceLayerProperties(&LayerPropertiesCount, nullptr);
            LayerProperties.resize(LayerPropertiesCount);
            vkEnumerateInstanceLayerProperties(&LayerPropertiesCount, LayerProperties.data());

            // enumerate physical devices
            uint32_t PhysicalDeviceCount;
            vkEnumeratePhysicalDevices(Context.Instance, &PhysicalDeviceCount, nullptr);
            std::vector<VkPhysicalDevice> PhysicalDevices;
            PhysicalDevices.resize(PhysicalDeviceCount);
            vkEnumeratePhysicalDevices(Context.Instance, &PhysicalDeviceCount, PhysicalDevices.data());
            

            VkPhysicalDeviceFeatures Features = {};

            VkPhysicalDeviceVulkan13Features  Vulkan13Features = {};
            Vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

            VkPhysicalDeviceVulkan12Features  Vulkan12Features = {};
            Vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

            VkPhysicalDeviceFeatures2 Features2 = {};
            Features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            
           
            // select Physicla device
            bool bFound = false;
            for(auto& PhysicalDevice : PhysicalDevices)
            {
               
                VkPhysicalDeviceProperties2 PhyDeviceProperties = {};
                PhyDeviceProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
                vkGetPhysicalDeviceProperties2(PhysicalDevice, &PhyDeviceProperties);
                VkPhysicalDeviceMemoryProperties2 PhyDeviceMemoryProperties = {};
                PhyDeviceMemoryProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
                vkGetPhysicalDeviceMemoryProperties2(PhysicalDevice, &PhyDeviceMemoryProperties);

                //vkGetPhysicalDeviceFeatures2(PhysicalDevice, &Vulkan12Features);
                vkGetPhysicalDeviceFeatures(PhysicalDevice, &Features);

                Vulkan12Features.pNext = &Vulkan13Features;
                Features2.pNext = &Vulkan12Features;
                vkGetPhysicalDeviceFeatures2(PhysicalDevice, &Features2);
                
                // required features
                bFound = true;
                bFound = bFound && Vulkan12Features.timelineSemaphore;
                bFound = bFound && Vulkan13Features.dynamicRendering;
                if (bFound)
                {
                    Context.PhysicalDevice = PhysicalDevice;
                    Context.PhyDeviceProperties = PhyDeviceProperties;
                    Context.PhyDeviceMemoryProperties = PhyDeviceMemoryProperties;
                    break;
                }
            }

            if (!bFound)
            {
                throw OS::FOSException("Failed to find physical device");
            }
          

            uint32_t QueueFamilyCount;
            vkGetPhysicalDeviceQueueFamilyProperties2(Context.PhysicalDevice, &QueueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties2> QueueFamilyProperties = {};
            QueueFamilyProperties.resize(QueueFamilyCount);
            for (size_t i = 0; i < QueueFamilyCount; ++i)
            {
                QueueFamilyProperties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
            }
            vkGetPhysicalDeviceQueueFamilyProperties2(Context.PhysicalDevice, &QueueFamilyCount, QueueFamilyProperties.data());

            uint32_t QueueFamilyIndex = 0;
            std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos = {};
            std::vector<std::vector<float>> PrioritiesArray = {};
            for (auto& QueueFamilyProperty : QueueFamilyProperties)
            {
                auto QueueCount = QueueFamilyProperty.queueFamilyProperties.queueCount;
                std::vector<float>& Priorities = PrioritiesArray.emplace_back();
                Priorities.resize(QueueCount);
                for (uint32_t i = 0; i < QueueCount; ++i)
                {
                    Priorities[i] = 1.0f;
                }
                    
                VkDeviceQueueCreateInfo QueueCreateInfo = {};
                QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                QueueCreateInfo.pNext = nullptr;
                QueueCreateInfo.flags = 0;
                QueueCreateInfo.queueFamilyIndex = QueueFamilyIndex;
                QueueCreateInfo.queueCount = QueueFamilyProperty.queueFamilyProperties.queueCount;
                QueueCreateInfo.pQueuePriorities = Priorities.data();
                QueueCreateInfos.push_back(QueueCreateInfo);
                QueueFamilyIndex++;
            }
            
            // gather extensions
            std::vector<const char*> Extensions;
            // required
            {
                Extensions.push_back(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
                Extensions.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
            }
            if (desc.Features.Swapchain)
            {
                Extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            }

            Context.DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            Context.DeviceInfo.pNext = &Vulkan12Features;
            Context.DeviceInfo.queueCreateInfoCount = (uint32_t)QueueCreateInfos.size();
            Context.DeviceInfo.pQueueCreateInfos = QueueCreateInfos.data(),
            Context.DeviceInfo.enabledExtensionCount = (uint32_t)Extensions.size();
            Context.DeviceInfo.ppEnabledExtensionNames = Extensions.data();
            Context.DeviceInfo.pEnabledFeatures = &Features;
            Context.DeviceInfo.enabledLayerCount = 0;
            Context.DeviceInfo.ppEnabledLayerNames = nullptr;
            Context.DeviceInfo.flags = 0;

            VK_CHECK_THROW(vkCreateDevice(Context.PhysicalDevice, &Context.DeviceInfo, nullptr, &Context.Device), "failed to create device");

            QueueFamilyIndex = 0;
            for (auto& QueueFamilyProperty : QueueFamilyProperties)
            {
                ECmdQueueType QueueType = ECmdQueueType::Undefined;
                if (QueueFamilyProperty.queueFamilyProperties.queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)
                {
                    QueueType = QueueType | ECmdQueueType::Graphic;
                }
                if (QueueFamilyProperty.queueFamilyProperties.queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT)
                {
                    QueueType = QueueType | ECmdQueueType::Compute;
                }
                if (QueueFamilyProperty.queueFamilyProperties.queueFlags & VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT)
                {
                    QueueType = QueueType | ECmdQueueType::Transfer;
                }

                for (uint32_t QueueIndex = 0; QueueIndex < (uint32_t)QueueFamilyProperty.queueFamilyProperties.queueCount; ++QueueIndex)
                {
                    FreeQueues.push_back(new FQueue(Context, QueueFamilyIndex, QueueIndex, QueueType));
                }

                QueueFamilyIndex++;
            }

            VmaVulkanFunctions VulkanFunctions = {};
            VulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
            VulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

            VmaAllocatorCreateInfo AllocatorCreateInfo = {};
            AllocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
            AllocatorCreateInfo.physicalDevice = Context.PhysicalDevice;
            AllocatorCreateInfo.device = Context.Device;
            AllocatorCreateInfo.instance = Context.Instance;
            AllocatorCreateInfo.pVulkanFunctions = &VulkanFunctions;

           
            vmaCreateAllocator(&AllocatorCreateInfo, &Context.Allocator);

            return true;
        }

        bool FDevice::IsCmdQueueValid(const ECmdQueueType& CmdQueueType)
        {
            return false;
        }

        void FDevice::WaitIdle()
        {
            vkDeviceWaitIdle(Context.Device);
        }

        FGPUInfo FDevice::GetGPUInfo()
        {
            FGPUInfo Ret = { Context.PhyDeviceProperties.properties.deviceName };
            return Ret;
        }
    }

    bool GRHIInitalize = false;

    void RHIInit()
    {
        if (!GRHIInitalize)
        {
            VK_CHECK_THROW(volkInitialize(), "Failed to init volk");
            GRHIInitalize = true;
        }
    }
 
    IDeviceRef CreateDevice(const FDeviceDesc &desc)
    {
        auto DeviceRef = RefCountPtr<Vulkan::FDevice>(new Vulkan::FDevice());
        if (!DeviceRef->Initalize(desc))
        {
            DeviceRef = nullptr;
        }
        return DeviceRef;
    }
}