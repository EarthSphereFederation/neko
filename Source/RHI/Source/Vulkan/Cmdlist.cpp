#include "Backend.h"
#include <cassert>
#include <map>
#include <vector>
namespace Neko::Vulkan
{ 
    FCmdBuffer::~FCmdBuffer()
    {
        if (CmdPool)
        {
            vkDestroyCommandPool(context->Device, CmdPool, context->AllocationCallbacks);
            CmdPool = nullptr;
        }
    }

    FQueue::FQueue(const VulkanContextPtr &ctx, uint32_t queueFamliyIndex, ECmdQueueType cmdType, VkQueueFamilyProperties2 properties) : context(ctx),
                                                                                                                                                     FamilyIndex(queueFamliyIndex), Type(cmdType), properties(properties)
    {
    }

    FQueue::~FQueue()
    {
    }

    VulkanCmdBufferPtr FQueue::CreateCmdBuffer()
    {
        VulkanCmdBufferPtr CmdBuf = std::make_shared<FCmdBuffer>(context);
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.queueFamilyIndex = FamilyIndex;
        commandPoolInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VK_CHECK_F(vkCreateCommandPool(context->Device, &commandPoolInfo, nullptr, &CmdBuf->CmdPool), "failed to create cmd pool with queue type %d", (uint32_t)Type);

        VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
        cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdBufAllocateInfo.commandPool = CmdBuf->CmdPool;
        cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdBufAllocateInfo.commandBufferCount = 1;

        VK_CHECK_F(vkAllocateCommandBuffers(context->Device, &cmdBufAllocateInfo, &CmdBuf->CmdBuf), "failed to create cmd buffer");

        return CmdBuf;
    }

    VulkanCmdBufferPtr FQueue::GetOrCreateCmdBuffer()
    {
        if (CmdBufferPool.size() > 0)
        {
            auto CmdBuf = CmdBufferPool.front();
            CmdBufferPool.pop_front();
            return CmdBuf;
        }
        return CreateCmdBuffer();
    }

    FCmdList::FCmdList(const VulkanContextPtr &ctx, const VulkanCmdBufferPtr &buf) : Context(ctx), CmdBuf(buf)
    {
    }

    RHICmdListRef FDevice::CreateCmdList(const RHICmdListDesc &desc) const
    {
        for (auto &queue : Queues)
        {
            if (queue->GetCmdQueueType() == desc.type)
            {
                FCmdList *cmdlist = new FCmdList(Context, queue->GetOrCreateCmdBuffer());
                return cmdlist;
            }
        }

        return nullptr;
    }

}