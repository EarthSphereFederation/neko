#include "Backend.h"
namespace Neko::RHI::Vulkan
{ 
	FRenderTarget::FRenderTarget(const FContext& Ctx, const FRenderTargetDesc& InDesc)
		:Context(Ctx),Desc(InDesc)
	{
        FTexture* Texture = reinterpret_cast<FTexture*>(Desc.Texture);
        assert(Texture != nullptr);
        VkImageViewCreateInfo ImageViewInfo = {};
        ImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewInfo.format = ConvertToVkFormat(Desc.Format);
        ImageViewInfo.image = Texture->GetImage();
        ImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

        VkImageSubresourceRange SubresourceRange;
        {
            SubresourceRange.aspectMask = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
            SubresourceRange.baseMipLevel = Desc.MipOffset;
            SubresourceRange.levelCount = Desc.MipNum;
            SubresourceRange.baseArrayLayer = Desc.ArrayOffset;
            SubresourceRange.layerCount = Desc.ArraySize;

        }
        ImageViewInfo.subresourceRange = SubresourceRange;

        VK_CHECK_THROW(vkCreateImageView(Context.Device, &ImageViewInfo, Context.AllocationCallbacks, &ImageView), "Failed to create iamge view");
	}

	FRenderTarget::~FRenderTarget()
	{
        if (ImageView)
        {
            vkDestroyImageView(Context.Device, ImageView, Context.AllocationCallbacks);
            ImageView = nullptr;
        }
	}

	IRenderTargetRef FDevice::CreateRenderTarget(const FRenderTargetDesc& InDesc)
	{
		return new FRenderTarget(Context, InDesc);
	}
}