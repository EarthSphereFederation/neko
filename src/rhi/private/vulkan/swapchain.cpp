#include "backend.h"
#include <assert.h>
#include <vector>
#include <map>
#pragma warning(disable : 26812)
namespace neko::rhi::vk
{
    VulkanSwapchain::VulkanSwapchain(const VulkanContextPtr &ctx) : Context(ctx)
    {
    }
    VulkanSwapchain::~VulkanSwapchain()
    {
        if (Swapchain)
        {
            for (uint32_t i = 0; i < ImageCount; ++i)
            {
                vkDestroyImageView(Context->Device, ImageViews[i], Context->AllocationCallbacks);
            }

            ImageCount = 0;
            auto Size = ImageViews.empty();
            Size = Images.empty();
            vkDestroySwapchainKHR(Context->Device, Swapchain, Context->AllocationCallbacks);
            Swapchain = nullptr;
        }
    }
    bool VulkanSwapchain::Initalize(const RHISwapChainDesc &Desc)
    {
        if (Desc.Surface.pointer)
        {
            auto Surface = static_cast<VkSurfaceKHR>(Desc.Surface.pointer);

            VkSurfaceCapabilitiesKHR SurfaceCapabilities;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Context->PhysicalDevice, Surface, &SurfaceCapabilities);

            VkExtent2D extent;
            extent.width = SurfaceCapabilities.currentExtent.width;
            extent.height = SurfaceCapabilities.currentExtent.height;

            auto Format = ConvertToVkFormat(Desc.Format);
            auto ColorSpace = ConvertToVkColorSpaceKHR(Desc.Format);
            VkFormat FallbackFormat = VkFormat::VK_FORMAT_UNDEFINED;
            {
                uint32_t SupportedFormatsCount;
                std::vector<VkSurfaceFormatKHR> SurfaceFormats;
                vkGetPhysicalDeviceSurfaceFormatsKHR(Context->PhysicalDevice, Surface, &SupportedFormatsCount, nullptr);
                SurfaceFormats.resize(SupportedFormatsCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(Context->PhysicalDevice, Surface, &SupportedFormatsCount, SurfaceFormats.data());

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
            SwapchainInfo.imageExtent = extent;
            SwapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
            SwapchainInfo.compositeAlpha = VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            SwapchainInfo.imageArrayLayers = 1;
            SwapchainInfo.presentMode = Desc.VSync ? VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR : VkPresentModeKHR::VK_PRESENT_MODE_FIFO_RELAXED_KHR;
            SwapchainInfo.clipped = VK_FALSE;
            SwapchainInfo.imageColorSpace = ColorSpace;
            SwapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

            VK_CHECK_RETURN_FALSE(vkCreateSwapchainKHR(Context->Device, &SwapchainInfo, Context->AllocationCallbacks, &Swapchain), "failed to create swapchain");

            vkGetSwapchainImagesKHR(Context->Device, Swapchain, &ImageCount, nullptr);
            Images.resize(ImageCount);
            ImageViews.resize(ImageCount);
            vkGetSwapchainImagesKHR(Context->Device, Swapchain, &ImageCount, Images.data());

            for (uint32_t i = 0; i < ImageCount; ++i)
            {
                VkImageViewCreateInfo ImageViewInfo = {};
                ImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                ImageViewInfo.format = Format;
                ImageViewInfo.image = Images[i];
                ImageViewInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
                VkImageSubresourceRange SubresourceRange;
                SubresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
                SubresourceRange.baseMipLevel = 0;
                SubresourceRange.baseArrayLayer = 0;
                SubresourceRange.layerCount = 1;
                SubresourceRange.levelCount = 1;
                ImageViewInfo.subresourceRange = SubresourceRange;

                VK_CHECK_RETURN_FALSE(vkCreateImageView(Context->Device, &ImageViewInfo, Context->AllocationCallbacks, &ImageViews[i]), "failed to create iamge view");
            }

            return true;
        }
        return false;
    }

    RHISwapchainRef VulkanDevice::CreateSwapChain(const RHISwapChainDesc &Desc) const
    {
        auto SwapchainRef = RefCountPtr<vk::VulkanSwapchain>(new vk::VulkanSwapchain(Context));
        if (!SwapchainRef->Initalize(Desc))
        {
            SwapchainRef = nullptr;
        }
        return SwapchainRef;
    }
}
