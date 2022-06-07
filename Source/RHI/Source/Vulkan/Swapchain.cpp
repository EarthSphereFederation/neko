#include "Backend.h"
#include <cassert>
#include <map>
#include <vector>
namespace Neko::RHI::Vulkan
{ 
    FSwapchain::FSwapchain(const FContext& Ctx) : Context(Ctx)
    {
    }

    FSwapchain::~FSwapchain()
    {
        if (Swapchain)
        {
            ImageCount = 0;
            vkDestroySwapchainKHR(Context.Device, Swapchain, Context.AllocationCallbacks);
            Swapchain = nullptr;
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

            VkExtent2D Size;
            VkFormat Format;

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

            std::vector<VkImage> Images;

            vkGetSwapchainImagesKHR(Context.Device, Swapchain, &ImageCount, nullptr);
            Images.resize(ImageCount);
            vkGetSwapchainImagesKHR(Context.Device, Swapchain, &ImageCount, Images.data());
            for (uint32_t i = 0; i < ImageCount; ++i)
            {
                FTextureDesc Desc;
                Desc.Width = Size.width;
                Desc.Height = Size.height;
                Desc.Depth = 1;
                Desc.ArraySize = 1;
                Desc.Depth = 1;
                Desc.Format = ConvertFromVkFormat(Format);
                Desc.MipNum = 1;
                auto Texture = RefCountPtr<FTexture>(new FTexture(Context, Images[i],Desc));
                Textures.push_back(Texture);
            }
            

            /*for (uint32_t i = 0; i < ImageCount; ++i)
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
            
                auto RT = RefCountPtr<FRenderTarget>(new FRenderTarget(Context, Images[i], ImageViews[i], Size, Format));
                RenderTargets.push_back(RT);

            }*/

            return true;
        }
        return false;
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

    uint32_t FSwapchain::AcquireNext(ISemaphore* InSemaphore, IFence* InFence)
    {
        auto Semaphore = reinterpret_cast<FSemaphore*>(InSemaphore);
        auto Fence = reinterpret_cast<FFence*>(InFence);

        VK_CHECK_THROW(vkAcquireNextImageKHR(Context.Device, Swapchain, UINT64_MAX, Semaphore ? Semaphore->GetSemaphore() : nullptr, Fence ? Fence->GetFence() : nullptr, &ImageIndex), "Failed to acquire next image");
        return ImageIndex;
    }

    void FSwapchain::Present(const FPresentDesc& Desc)
    {
        assert(Desc.Queue != nullptr);

        auto Queue = reinterpret_cast<FQueue*>(Desc.Queue);

        std::vector<VkSemaphore> WaitSemaphores;
        for (uint32_t i = 0; i < Desc.WaitSemaphoreArray.size(); ++i)
        {
            WaitSemaphores.push_back(reinterpret_cast<FSemaphore*>(Desc.WaitSemaphoreArray[i])->GetSemaphore());
        }

        VkPresentInfoKHR PresentInfo = {};
        PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.waitSemaphoreCount = (uint32_t)Desc.WaitSemaphoreArray.size();
        PresentInfo.pWaitSemaphores = WaitSemaphores.data();
        VkSwapchainKHR SwapChains[] = { Swapchain };
        uint32_t ImageIndices[] = { Desc.PresentIndex };
        PresentInfo.swapchainCount = 1;
        PresentInfo.pSwapchains = SwapChains;
        PresentInfo.pImageIndices = ImageIndices;
        VK_CHECK_THROW(auto result = vkQueuePresentKHR(Queue->GetQueue(), &PresentInfo), "Failed to present");
    }

    uint32_t FSwapchain::GetTextureNum()
    {
        return ImageCount;
    }

    std::vector<ITextureRef>  FSwapchain::GetTextures()
    {
        return Textures;
    }
}
