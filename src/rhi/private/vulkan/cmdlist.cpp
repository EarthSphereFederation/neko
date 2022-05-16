#include "backend.h"
#include <assert.h>
#include <vector>
#include <map>
#pragma warning(disable : 26812)
namespace neko::rhi::vk
{
    VulkanCmdBuffer::~VulkanCmdBuffer()
    {
        if (CmdPool)
        {
            vkDestroyCommandPool(context->Device, CmdPool, context->AllocationCallbacks);
            CmdPool = nullptr;
        }
    }

    VulkanQueue::VulkanQueue(const VulkanContextPtr &ctx, uint32_t queueFamliyIndex, RHICmdQueueType cmdType, VkQueueFamilyProperties2 properties) : context(ctx),
                                                                                                                                                     FamilyIndex(queueFamliyIndex), Type(cmdType), properties(properties)
    {
    }

    VulkanQueue::~VulkanQueue()
    {
    }

    VulkanCmdBufferPtr VulkanQueue::CreateCmdBuffer()
    {
        VulkanCmdBufferPtr CmdBuf = std::make_shared<VulkanCmdBuffer>(context);
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

    VulkanCmdBufferPtr VulkanQueue::GetOrCreateCmdBuffer()
    {
        if (CmdBufferPool.size() > 0)
        {
            auto CmdBuf = CmdBufferPool.front();
            CmdBufferPool.pop_front();
            return CmdBuf;
        }
        return CreateCmdBuffer();
    }

    VulkanCmdList::VulkanCmdList(const VulkanContextPtr &ctx, const VulkanCmdBufferPtr &buf) : Context(ctx), CmdBuf(buf)
    {
    }

    RHICmdListRef VulkanDevice::CreateCmdList(const RHICmdListDesc &desc) const
    {
        for (auto &queue : Queues)
        {
            if (queue->GetCmdQueueType() == desc.type)
            {
                VulkanCmdList *cmdlist = new VulkanCmdList(Context, queue->GetOrCreateCmdBuffer());
                return cmdlist;
            }
        }

        return nullptr;
    }

}