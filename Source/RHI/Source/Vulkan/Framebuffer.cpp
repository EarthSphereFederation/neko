#include "backend.h"
#pragma warning(disable : 26812)
namespace Neko::RHI::Vulkan
{ 
	FFrameBuffer::FFrameBuffer(const FSwapchain& Swapchain, uint32_t Index)
	{
		Info.AddFormat(ConvertFromVkFormat(Swapchain.GetFormat()));
		Info.AddLoadAction(ELoadOp::Load);
		Info.AddStoreAction(EStoreOp::Store);
		auto ImageView = Swapchain.GetImageView(Index);
		AddImage(Swapchain.GetImage(Index));
		AddImageView(ImageView);
	}
}