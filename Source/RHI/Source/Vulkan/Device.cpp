#include "Backend.h"
#include <cassert>
#include <map>
#include <vector>

namespace Neko::RHI
{ 
    namespace Vulkan
    {
        FContext::~FContext()
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
            VkPhysicalDeviceVulkan12Features  Vulkan12Features = {};
            Vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

            VkPhysicalDeviceFeatures2 Features2 = {};
            Features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            Features2.pNext = &Vulkan12Features;
           
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
                vkGetPhysicalDeviceFeatures2(PhysicalDevice, &Features2);

                // required features
                bFound = true;
                bFound &= Vulkan12Features.timelineSemaphore;
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

            struct FQueueFamliyInfo
            {
                VkQueueFamilyProperties2 Properties;
                uint32_t UsedQueueCount = 0;
                std::vector<float> Priorities;
                std::map<ECmdQueueType, uint32_t> TypeToQueueIndexMap;
                FQueueFamliyInfo(const VkQueueFamilyProperties2& InProperties):Properties(InProperties)
                {
                }

                bool IsValid() const
                {
                    return UsedQueueCount > 0;
                }
    
                bool HasFittedQueue(VkQueueFlags Flags)
                {
                    if (UsedQueueCount < Properties.queueFamilyProperties.queueCount)
                    {
                        if (Properties.queueFamilyProperties.queueFlags & Flags)
                        {
                            return true;
                        }
                    }
                    return false;
                }

                void UseQueue(const ECmdQueueType& CmdQueueType) 
                {
                    TypeToQueueIndexMap[CmdQueueType] = UsedQueueCount;
                    UsedQueueCount++;
                    Priorities.push_back(1.0f);
                }

                bool FindQueue(const ECmdQueueType& CmdQueueType, uint32_t& QueueIndex)
                {
                    for (auto& TypeToQueueIndex : TypeToQueueIndexMap)
                    {
                        if (TypeToQueueIndex.first == CmdQueueType)
                        {
                            QueueIndex = TypeToQueueIndex.second;
                            return true;
                        }
                    }
                    return false;
                }
            };

            std::vector<FQueueFamliyInfo> QueueFamliyInfos;

            for (size_t i = 0; i < QueueFamilyCount; ++i)
            {
                QueueFamliyInfos.push_back(QueueFamilyProperties[i]);
            }

            for (uint32_t QueueFamilyIndex = 0; QueueFamilyIndex < QueueFamilyCount; ++QueueFamilyIndex)
            {
                // graphic
                if (QueueFamliyInfos[QueueFamilyIndex].HasFittedQueue(VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT))
                {
                    QueueFamliyInfos[QueueFamilyIndex].UseQueue(ECmdQueueType::Graphic);
                    break;
                }
            }
            for (uint32_t QueueFamilyIndex = 0; QueueFamilyIndex < QueueFamilyCount; ++QueueFamilyIndex)
            {
                // compute
                if (QueueFamliyInfos[QueueFamilyIndex].HasFittedQueue(VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT))
                {
                    QueueFamliyInfos[QueueFamilyIndex].UseQueue(ECmdQueueType::Compute);
                    break;
                }
            }
            for (uint32_t QueueFamilyIndex = 0; QueueFamilyIndex < QueueFamilyCount; ++QueueFamilyIndex)
            {
                // transfer
                if (QueueFamliyInfos[QueueFamilyIndex].HasFittedQueue(VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT))
                {
                    QueueFamliyInfos[QueueFamilyIndex].UseQueue(ECmdQueueType::Transfer);
                    break;
                }
            }

            std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos = {};
            
            for (int32_t i = 0; i < QueueFamliyInfos.size(); ++i)
            {
                if (QueueFamliyInfos[i].IsValid())
                {
                    VkDeviceQueueCreateInfo QueueCreateInfo = {};
                    auto& QueueFamliyInfo = QueueFamliyInfos[i];
                    QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                    QueueCreateInfo.pNext = nullptr;
                    QueueCreateInfo.flags = 0;
                    QueueCreateInfo.queueFamilyIndex = i;
                    QueueCreateInfo.queueCount = QueueFamliyInfo.UsedQueueCount;
                    QueueCreateInfo.pQueuePriorities = QueueFamliyInfo.Priorities.data();
                    QueueCreateInfos.push_back(QueueCreateInfo);
                } 
            }

            // gather extensions
            std::vector<const char*> Extensions;
            // required
            {
                Extensions.push_back(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
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

            // create queues
            std::map<ECmdQueueType, std::pair<uint32_t, uint32_t>> QueueMap;
            uint32_t QueueFamliyIndex = 0;
            for (auto& QueueFamliyInfo : QueueFamliyInfos)
            {
                for (auto& TypeToQueueIndex : QueueFamliyInfo.TypeToQueueIndexMap)
                {
                    QueueMap[TypeToQueueIndex.first] = std::make_pair(QueueFamliyIndex, TypeToQueueIndex.second);
                }
                QueueFamliyIndex++;
            }
#define CREATE_QUEUE(CmdQueueType) \
            if(QueueMap.find(CmdQueueType) == QueueMap.end()) \
            { \
                Queues.push_back(nullptr); \
            } \
            else \
            { \
                Queues.push_back(std::make_unique<FQueue>(Context, QueueMap[CmdQueueType].first, QueueMap[CmdQueueType].second, CmdQueueType)); \
            } \
            
            CREATE_QUEUE(ECmdQueueType::Graphic);
            CREATE_QUEUE(ECmdQueueType::Compute);
            CREATE_QUEUE(ECmdQueueType::Transfer);
#undef CREATE_QUEUE
            return true;
        }

        FQueue& FDevice::GetQueue(const ECmdQueueType& Type)
        {
            assert((uint32_t)Type < (uint32_t)ECmdQueueType::Count);
            return *Queues[(uint32_t)Type];
        }

        bool FDevice::IsCmdQueueValid(const ECmdQueueType& CmdQueueType)
        {
            if (Queues[(uint32_t)CmdQueueType])
            {
                return true;
            }
            return false;
        }

        void FDevice::GC()
        {
            for (auto& Queue : Queues)
            {
                if (Queue)
                {
                    Queue->GC();
                }
            }
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