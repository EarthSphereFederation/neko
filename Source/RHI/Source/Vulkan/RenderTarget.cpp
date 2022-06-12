#include "Backend.h"
namespace Neko::RHI::Vulkan
{ 
    FColorAttachment::FColorAttachment(const FContext& Ctx, const FColorAttachmentDesc& InDesc)
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

    FColorAttachment::~FColorAttachment()
	{
        if (ImageView)
        {
            vkDestroyImageView(Context.Device, ImageView, Context.AllocationCallbacks);
            ImageView = nullptr;
        }
	}

	IColorAttachmentRef FDevice::CreateColorAttachment(const FColorAttachmentDesc& InDesc)
	{
		return new FColorAttachment(Context, InDesc);
	}
}