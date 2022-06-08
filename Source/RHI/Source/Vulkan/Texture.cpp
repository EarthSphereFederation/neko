#include "Backend.h"
#include <cassert>
#include <map>
#include <vector>
namespace Neko::RHI::Vulkan
{
	FTexture::FTexture(const FContext& Ctx, VkImage InImage, const FTextureDesc& InDesc, bool InbAutoRelease) 
		: Context(Ctx), Image(InImage), Desc(InDesc), bAutoRelease(InbAutoRelease)
	{

	}

	FTexture2DView::FTexture2DView(const FContext& Ctx, const FTexture2DViewDesc& InDesc):Context(Ctx),Desc(InDesc)
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

   FTexture2DView:: ~FTexture2DView()
    {
        if (ImageView)
        {
            vkDestroyImageView(Context.Device, ImageView, Context.AllocationCallbacks);
            ImageView = nullptr;
        }
    }

    ITextureRef FTexture2DView::GetTexture()
    {
        return Desc.Texture;
    }

    const FTexture2DViewDesc& FTexture2DView::GetDesc()
    {
        return Desc;
    }

    ITexture2DViewRef FDevice::CreateTexture2DView(const FTexture2DViewDesc& Desc)
    {
        return  new FTexture2DView(Context, Desc);
    }

    ITexture2DViewRef FDevice::CreateTexture2DView(ITexture* InTexture)
    {
        assert(InTexture);
        assert(InTexture->GetDesc().TextureType == ETextureType::Texture2D);

        auto Desc = FTexture2DViewDesc().SetTexture(InTexture)
            .SetArrayOffset(0)
            .SetArraySize(InTexture->GetDesc().ArraySize)
            .SetFormat(InTexture->GetDesc().Format)
            .SetMipNum(InTexture->GetDesc().MipNum)
            .SetMipOffset(0);
        return CreateTexture2DView(Desc);
    }
}