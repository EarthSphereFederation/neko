#include "Backend.h"
#include <cassert>
namespace Neko::RHI::Vulkan
{
    FSemaphore::FSemaphore(const FContext& Ctx, const ESemaphoreType& Type) :Context(Ctx)
    {
        VkSemaphoreTypeCreateInfo SemaphoreTypeCreateInfo = {};
        SemaphoreTypeCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
        SemaphoreTypeCreateInfo.initialValue = 0;
        SemaphoreTypeCreateInfo.semaphoreType = ConvertToVkSemaphoreType(Type);

        VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
        SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        SemaphoreCreateInfo.pNext = &SemaphoreTypeCreateInfo;

        vkCreateSemaphore(Context.Device, &SemaphoreCreateInfo, Context.AllocationCallbacks, &Semaphore);
    }

    FSemaphore::~FSemaphore()
    {
        if (Semaphore)
        {
            vkDestroySemaphore(Context.Device, Semaphore, Context.AllocationCallbacks);
        }
    }

    ISemaphoreRef FDevice::CreateSemaphore(const ESemaphoreType& Type)
    {
        return new FSemaphore(Context, Type);
    }

    std::vector<ISemaphoreRef> FDevice::CreateSemaphores(const ESemaphoreType& Type, uint32_t Size)
    {
        std::vector<ISemaphoreRef> Semaphores;
        Semaphores.reserve(Size);

        for (uint32_t i = 0; i < Size; ++i)
        {
            Semaphores.push_back(CreateSemaphore(Type));
        }
        return Semaphores;
    }
}