#include "Backend.h"
namespace Neko::RHI::Vulkan
{ 
	FRenderTarget::FRenderTarget(const FContext& Ctx, const FRenderTargetDesc& InDesc)
		:Context(Ctx),Desc(InDesc)
	{
	}

	FRenderTarget::~FRenderTarget()
	{
	}

	IRenderTargetRef FDevice::CreateRenderTarget(const FRenderTargetDesc& InDesc)
	{
		return new FRenderTarget(Context, InDesc);
	}
}