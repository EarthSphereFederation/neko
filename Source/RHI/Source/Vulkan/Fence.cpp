#include "Backend.h"
#include <cassert>
namespace Neko::RHI::Vulkan
{
    FFence::FFence(const FContext& Ctx, const EFenceFlag& State) :Context(Ctx)
    {
        VkFenceCreateInfo FenceCreateInfo = {};
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        FenceCreateInfo.flags = State == EFenceFlag::Signal ? VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT : 0;

        vkCreateFence(Context.Device, &FenceCreateInfo, Context.AllocationCallbacks, &Fence);
    }
    FFence::~FFence()
    {
        if (Fence)
        {
            vkDestroyFence(Context.Device, Fence, Context.AllocationCallbacks);
            Fence = nullptr;
        }
    }

    void FFence::Wait()
    {
        vkWaitForFences(Context.Device, 1, &Fence, VK_FALSE, UINT64_MAX);
    }

    void FFence::Reset()
    {
        vkResetFences(Context.Device, 1, &Fence);
    }

    IFenceRef FDevice::CreateFence(const EFenceFlag& FenceState)
    {
        return new FFence(Context, FenceState);
    }

    std::vector<IFenceRef> FDevice::CreateFences(const EFenceFlag& State, uint32_t Size)
    {
        std::vector<IFenceRef> Fences;
        Fences.reserve(Size);

        for (uint32_t i = 0; i < Size; ++i)
        {
            Fences.push_back(CreateFence(State));
        }
        return Fences;
    }

}