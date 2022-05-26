#include "Backend.h"
#include <cassert>
#include <map>
#include <vector>
namespace Neko::RHI::Vulkan
{ 
    FSwapchain::FSwapchain(const FContext& Ctx) : Context(Ctx)
    {
        VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
        SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        vkCreateSemaphore(Context.Device, &SemaphoreCreateInfo, Context.AllocationCallbacks, &SwapchainSemaphore);
    }

    FSwapchain::~FSwapchain()
    {
        if (Swapchain)
        {
            for (uint32_t i = 0; i < ImageCount; ++i)
            {
                vkDestroyImageView(Context.Device, ImageViews[i], Context.AllocationCallbacks);
            }

            ImageCount = 0;
            auto Size = ImageViews.empty();
            Size = Images.empty();
            vkDestroySwapchainKHR(Context.Device, Swapchain, Context.AllocationCallbacks);
            Swapchain = nullptr;
        }

        if (SwapchainSemaphore)
        {
            vkDestroySemaphore(Context.Device, SwapchainSemaphore, Context.AllocationCallbacks);
        }

        if (Surface)
        {
            vkDestroySurfaceKHR(Context.Instance, Surface, Context.AllocationCallbacks);
            Surface = nullptr;
        }
    }
    bool FSwapchain::Initalize(const FSwapChainDesc &Desc)
    {
        if (Desc.WindowRawPtr)
        {
            Surface = static_cast<VkSurfaceKHR>(Desc.WindowRawPtr->CreateVulkanSurface(Context.Instance).pointer);

            VkSurfaceCapabilitiesKHR SurfaceCapabilities;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Context.PhysicalDevice, Surface, &SurfaceCapabilities);

            Size.width = SurfaceCapabilities.currentExtent.width;
            Size.height = SurfaceCapabilities.currentExtent.height;

            Format = ConvertToVkFormat(Desc.Format);
            auto ColorSpace = ConvertToVkColorSpaceKHR(Desc.Format);
            VkFormat FallbackFormat = VkFormat::VK_FORMAT_UNDEFINED;
            {
                uint32_t SupportedFormatsCount;
                std::vector<VkSurfaceFormatKHR> SurfaceFormats;
                vkGetPhysicalDeviceSurfaceFormatsKHR(Context.PhysicalDevice, Surface, &SupportedFormatsCount, nullptr);
                SurfaceFormats.resize(SupportedFormatsCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(Context.PhysicalDevice, Surface, &SupportedFormatsCount, SurfaceFormats.data());

                bool bColorSpaceFound = false;
                for (uint32_t i = 0; i < SupportedFormatsCount; ++i)
                {
                    if (SurfaceFormats[i].colorSpace == ColorSpace)
                    {
                        bColorSpaceFound = true;
                    }
                }

                if (!bColorSpaceFound)
                {
                    ColorSpace = SurfaceFormats[0].colorSpace;
                    FallbackFormat = SurfaceFormats[0].format;
                }

                bool bColorFormatFound = false;
                for (uint32_t i = 0; i < SupportedFormatsCount; ++i)
                {
                    if (SurfaceFormats[i].colorSpace == ColorSpace)
                    {
                        if (SurfaceFormats[i].format == Format)
                        {
                            bColorFormatFound = true;
                        }
                    }
                }

                if (!bColorFormatFound)
                {
                    Format = FallbackFormat;
                }
            }

            VkSwapchainCreateInfoKHR SwapchainInfo = {};
            SwapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            SwapchainInfo.surface = Surface;
            SwapchainInfo.minImageCount = 2;
            SwapchainInfo.imageFormat = Format;
            SwapchainInfo.imageExtent = Size;
            SwapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
            SwapchainInfo.compositeAlpha = VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            SwapchainInfo.imageArrayLayers = 1;
            SwapchainInfo.presentMode = Desc.VSync ? VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR : VkPresentModeKHR::VK_PRESENT_MODE_FIFO_RELAXED_KHR;
            SwapchainInfo.clipped = VK_FALSE;
            SwapchainInfo.imageColorSpace = ColorSpace;
            SwapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

            VK_CHECK_THROW(vkCreateSwapchainKHR(Context.Device, &SwapchainInfo, Context.AllocationCallbacks, &Swapchain), "failed to create swapchain");

            vkGetSwapchainImagesKHR(Context.Device, Swapchain, &ImageCount, nullptr);
            Images.resize(ImageCount);
            ImageViews.resize(ImageCount);
            vkGetSwapchainImagesKHR(Context.Device, Swapchain, &ImageCount, Images.data());

            for (uint32_t i = 0; i < ImageCount; ++i)
            {
                VkImageViewCreateInfo ImageViewInfo = {};
                ImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                ImageViewInfo.format = Format;
                ImageViewInfo.image = Images[i];
                ImageViewInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
                
                VkImageSubresourceRange SubresourceRange;
                {
                    SubresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
                    SubresourceRange.baseMipLevel = 0;
                    SubresourceRange.baseArrayLayer = 0;
                    SubresourceRange.layerCount = 1;
                    SubresourceRange.levelCount = 1;
                }
                ImageViewInfo.subresourceRange = SubresourceRange;

                VK_CHECK_THROW(vkCreateImageView(Context.Device, &ImageViewInfo, Context.AllocationCallbacks, &ImageViews[i]), "failed to create iamge view");
            
                auto FrameBuffer = RefCountPtr<FFrameBuffer>(new FFrameBuffer(Context, this, i));
                FrameBuffer->Initalize();
                FrameBuffers.push_back(FrameBuffer);

            }

            return true;
        }
        return false;
    }

    IFrameBufferRef FSwapchain::GetFrameBuffer(uint32_t Index)
    {
        assert(Index < ImageCount);
        return FrameBuffers[Index];
    }

    uint32_t FSwapchain::GetFrameBufferIndex(IFrameBuffer* FrameBuffer) const
    {
        for (uint32_t i = 0; i < FrameBuffers.size(); ++i)
        {
            if (FrameBuffers[i].GetPtr() == FrameBuffer)
            {
                return i;
            }
        }
        assert(false);
        return 0;
    }

    ISwapchainRef FDevice::CreateSwapChain(const FSwapChainDesc &Desc)
    {
        auto SwapchainRef = RefCountPtr<FSwapchain>(new FSwapchain(Context));
        if (!SwapchainRef->Initalize(Desc))
        {
            SwapchainRef = nullptr;
        }
        return SwapchainRef;
    }

    IFrameBufferRef FDevice::QueueWaitNextFrameBuffer(ISwapchain* Swapchain, IQueue* InQueue)
    {
        auto& Queue = *reinterpret_cast<FQueue*>(InQueue);
        
        auto SignalSemaphore = reinterpret_cast<FSwapchain*>(Swapchain)->GetSemaphore();
        auto VkSwapchainPtr = reinterpret_cast<FSwapchain*>(Swapchain)->GetSwapchain();

        uint32_t ImageIndex = 0;
        VK_CHECK_THROW(vkAcquireNextImageKHR(Context.Device, VkSwapchainPtr, UINT64_MAX, SignalSemaphore, nullptr, &ImageIndex), "Failed to acquire next image");
        Queue.AddWaitSemaphore(SignalSemaphore, 0);
        return Swapchain->GetFrameBuffer(ImageIndex);
    }

    void FDevice::QueueWaitPresent(ISwapchain* InSwapchain, IFrameBuffer* FrameBuffer, IQueue* InQueue)
    {
       
        auto& Queue = *reinterpret_cast<FQueue*>(InQueue);
        
        auto Swapchain = reinterpret_cast<FSwapchain*>(InSwapchain);
        auto VkSwapchainPtr = Swapchain->GetSwapchain();
        auto WaitSemaphore = Queue.GetSemaphoreForSwapchain();

        VkPresentInfoKHR PresentInfo = {};
        PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.waitSemaphoreCount = 1;
        PresentInfo.pWaitSemaphores = &WaitSemaphore;
        VkSwapchainKHR SwapChains[] = { VkSwapchainPtr };
        uint32_t ImageIndices[] = { Swapchain->GetFrameBufferIndex(FrameBuffer) };
        PresentInfo.swapchainCount = 1;
        PresentInfo.pSwapchains = SwapChains;
        PresentInfo.pImageIndices = ImageIndices;
        VK_CHECK_THROW(auto result = vkQueuePresentKHR(Queue.GetQueue(), &PresentInfo), "Failed to present");
    }
}
