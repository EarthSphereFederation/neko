#include "Backend.h"
#include <cassert>
#include <map>
#include <vector>
namespace Neko::RHI::Vulkan
{  
    FQueue::FQueue(const FContext &Ctx, uint32_t InQueueFamliyIndex, uint32_t QueueIndex, ECmdQueueType InCmdType) : Context(Ctx),FamilyIndex(InQueueFamliyIndex), Type(InCmdType)
    { 
        vkGetDeviceQueue(Context.Device, InQueueFamliyIndex, QueueIndex, &Queue);
    }

    FQueue::~FQueue()
    {
    }

    std::vector<ICmdPoolRef> FQueue::CreateCmdPools(uint32_t Num)
    {
        std::vector<ICmdPoolRef> CmdPools;
        CmdPools.reserve(Num);
        
        for (uint32_t i = 0; i < Num; ++i)
        {
            CmdPools.push_back(CreateCmdPool());
        }

        return CmdPools;
    }

    ICmdPoolRef FQueue::CreateCmdPool()
    {
        return new FCmdPool(Context, *this);
    }
 
    FCmdPool::FCmdPool(const FContext& Ctx,FQueue& InQueue) :Context(Ctx),Queue(InQueue)
    {
        VkCommandPoolCreateInfo CommandPoolInfo = {};
        CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        CommandPoolInfo.queueFamilyIndex = Queue.GetFamilyIndex();
        CommandPoolInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VK_CHECK_THROW(vkCreateCommandPool(Context.Device, &CommandPoolInfo, Context.AllocationCallbacks, &CmdPool), "Failed to create command pool");
    };

    FCmdPool::~FCmdPool()
    {
        if (CmdPool)
        {
            vkDestroyCommandPool(Context.Device, CmdPool, Context.AllocationCallbacks);
            CmdPool = nullptr;
        }
    }

    ICmdListRef FCmdPool::CreateCmdList()
    {
        std::lock_guard Guard(Mutex);
        CmdLists.push_back(new FCmdList(Context, this));
        return CmdLists.back();
    }

    void FCmdPool::Free()
    {
        if (CmdLists.size() > 0)
        {
            std::vector<VkCommandBuffer> CmdBufs;
            CmdBufs.reserve(CmdLists.size());
            for (auto& CmdList : CmdLists)
            {
                CmdBufs.push_back(reinterpret_cast<FCmdList*>(CmdList.GetPtr())->GetCmdBuffer());
            }

            vkFreeCommandBuffers(Context.Device, CmdPool, CmdBufs.size(), CmdBufs.data());

            CmdLists.clear();
        }   
    }


    FCmdList::FCmdList(const FContext & Ctx, FCmdPool* InCmdPool): Context(Ctx), CmdPool(InCmdPool)
    {
        assert(CmdPool != nullptr);
        VkCommandBufferAllocateInfo CmdBufAllocateInfo = {};
        CmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        CmdBufAllocateInfo.commandPool = CmdPool->GetCmdPool();
        CmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        CmdBufAllocateInfo.commandBufferCount = 1;

        vkAllocateCommandBuffers(Context.Device, &CmdBufAllocateInfo, &CmdBuffer);
    }

    FCmdList::~FCmdList()
    {
    }

    void FCmdList::BeginCmd()
    {
        VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
        CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        CommandBufferBeginInfo.flags = 0;

        VK_CHECK_THROW(vkBeginCommandBuffer(CmdBuffer, &CommandBufferBeginInfo),"Failed to begin command buffer");
    }

    void FCmdList::EndCmd()
    {
        VK_CHECK_THROW(vkEndCommandBuffer(CmdBuffer), "Failed to end command buffer");
    }
    
    void FCmdList::BeginRenderPass(const FRenderPassDesc& InDesc)
    {
        assert(InDesc.ColorRenderTargetArray.size() > 0);

        auto RTDesc = InDesc.ColorRenderTargetArray[0]->GetDesc();

        static_vector<VkRenderingAttachmentInfoKHR, MAX_RENDER_TARGET_COUNT> RenderingAttachmentInfos;
        for (uint32_t i = 0; i < InDesc.ColorRenderTargetArray.size(); ++i)
        {
            auto& ColorRT = InDesc.ColorRenderTargetArray[i];

            auto RenderTarget = reinterpret_cast<FRenderTarget*>(ColorRT.GetPtr());
            VkRenderingAttachmentInfoKHR RenderingAttachmentInfo = {};
            RenderingAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
            RenderingAttachmentInfo.imageView = RenderTarget->GetImageView();
            RenderingAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
            RenderingAttachmentInfo.loadOp = ConvertToVkAttachmentLoadOp(ColorRT->GetDesc().LoadAction);
            RenderingAttachmentInfo.storeOp = ConvertToVkAttachmentStoreOp(ColorRT->GetDesc().StoreAction);
            RenderingAttachmentInfos.push_back(RenderingAttachmentInfo);
        }

        VkRect2D RenderArea;
        RenderArea.offset.x = 0;
        RenderArea.offset.y = 0;
        RenderArea.extent.width = RTDesc.Texture->GetDesc().Width;
        RenderArea.extent.height = RTDesc.Texture->GetDesc().Height;

        VkRenderingInfoKHR RenderingInfo = {};
        RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
        RenderingInfo.renderArea = RenderArea;
        RenderingInfo.layerCount = 1; // TODO
        RenderingInfo.colorAttachmentCount = RenderingAttachmentInfos.size();
        RenderingInfo.pColorAttachments = RenderingAttachmentInfos.data();

        vkCmdBeginRenderingKHR(CmdBuffer, &RenderingInfo);
    }

    void FCmdList::EndRenderPass()
    {
        vkCmdEndRenderingKHR(CmdBuffer);
    }

    void FCmdList::BindGraphicPipeline(IGraphicPipeline* InGraphicPipeline)
    {
       auto GraphicPipeline = reinterpret_cast<FGraphicPipeline*>(InGraphicPipeline);
       vkCmdBindPipeline(CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline->GetPipeline());
    }

    void FCmdList::SetViewport(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height, float MinDepth, float MaxDepth)
    {
        VkViewport Viewport = {};
        Viewport.x = (float)X;
        Viewport.y = (float)Y;
        Viewport.width = (float)Width;
        Viewport.height = (float)Height;
        Viewport.minDepth = MinDepth;
        Viewport.maxDepth = MaxDepth;
        vkCmdSetViewport(CmdBuffer, 0, 1, &Viewport);
    }

    void FCmdList::SetScissor(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height)
    {
        VkRect2D Scissor = {};
        Scissor.offset.x = X;
        Scissor.offset.y = Y;
        Scissor.extent.width = Width;
        Scissor.extent.height = Height;
        vkCmdSetScissor(CmdBuffer, 0, 1, &Scissor);
    }

    void FCmdList::SetViewportNoScissor(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height, float MinDepth, float MaxDepth)
    {
        SetViewport(X, Width, Y, Height, MinDepth, MaxDepth);
        SetScissor(X, Width, Y, Height);
    }
    
    void FCmdList::Draw(uint32_t VertexNum, uint32_t VertexOffset, uint32_t InstanceNum, uint32_t InstanceOffset)
    {
        vkCmdDraw(CmdBuffer, VertexNum, InstanceNum, VertexOffset, InstanceOffset);
    }

    void FCmdList::ResourceBarrier(const FTextureTransitionDesc& Desc)
    {

        VkImageMemoryBarrier ImageMemoryBarrier = {};
        ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        ImageMemoryBarrier.srcAccessMask = ConvertToVkAccessFlags(Desc.SrcState);
        ImageMemoryBarrier.oldLayout = ConvertToVkImageLayout(Desc.SrcState);
        ImageMemoryBarrier.dstAccessMask = ConvertToVkAccessFlags(Desc.DestState);
        ImageMemoryBarrier.newLayout = ConvertToVkImageLayout(Desc.DestState);
        auto Texture = reinterpret_cast<FTexture*>(Desc.Texture);
        ImageMemoryBarrier.image = Texture->GetImage();
        VkImageSubresourceRange Range;
        {
            Range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            Range.baseArrayLayer = Desc.Range.ArrayOffset;
            Range.layerCount = Desc.Range.ArraySize;
            Range.baseMipLevel = Desc.Range.MipOffset;
            Range.levelCount = Desc.Range.MipNum;
        }
        ImageMemoryBarrier.subresourceRange = Range;

        vkCmdPipelineBarrier(
            CmdBuffer,
            ConvertToVkPipelineStageFlags(Desc.SrcState),  // srcStageMask
            ConvertToVkPipelineStageFlags(Desc.DestState), // dstStageMask
            0,
            0,
            nullptr,
            0,
            nullptr,
            1, // imageMemoryBarrierCount
            &ImageMemoryBarrier // pImageMemoryBarriers
        );
    }

    IQueueRef FDevice::CreateQueue(const ECmdQueueType& CmdQueueType)
    {
        RefCountPtr<FQueue> Queue = nullptr;

        auto Queues = std::move(FreeQueues);
        for (uint32_t i = 0; i < Queues.size(); ++i)
        {
            if (Queues[i]->IsMatch(CmdQueueType) && !Queue)
            {
                Queue = Queues[i];
                UsedQueues.push_back(Queues[i]);
            }
            else
            {
                FreeQueues.push_back(Queues[i]);
            }
        }
        if (!Queue)
        {
            throw OS::FOSException("Failed to find queue");
        }
        return Queue;
    }
    
    void FQueue::ExcuteCmdLists(ICmdList** CmdLists, uint32_t CmdListNum, const FExcuteDesc& Desc)
    {
       assert(CmdListNum > 0);

       std::vector<VkCommandBuffer> CmdBufs;
       CmdBufs.reserve(CmdListNum);
       for (uint32_t i = 0; i < CmdListNum; ++i)
       {
           if (CmdLists[i])
           {
               auto CmdBuf = reinterpret_cast<FCmdList*>(CmdLists[i])->GetCmdBuffer();
               CmdBufs.push_back(CmdBuf);
           }
       }

       std::vector<VkSemaphore> SignalSemaphores;
       std::vector<uint64_t> SignalSemaphoreValues;

       for (uint32_t i = 0; i < Desc.SignalSemaphoreArray.size(); ++i)
       {
           auto Semaphore = (FSemaphore*)Desc.SignalSemaphoreArray[i];
           SignalSemaphores.push_back(Semaphore->GetSemaphore());
           SignalSemaphoreValues.push_back(Semaphore->GetCounter());
       }

       std::vector<VkSemaphore> WaitSemaphores;
       std::vector<uint64_t> WaitSemaphoreValues;

       for (uint32_t i = 0; i < Desc.WaitSemaphoreArray.size(); ++i)
       {
           auto Semaphore = (FSemaphore*)Desc.WaitSemaphoreArray[i];
           WaitSemaphores.push_back(Semaphore->GetSemaphore());
           WaitSemaphoreValues.push_back(Semaphore->GetCounter());
       }

       std::vector<VkPipelineStageFlags> WaitDstStageMasks;
       WaitDstStageMasks.reserve(Desc.WaitSemaphoreArray.size());
       for (uint32_t i = 0; i < Desc.WaitSemaphoreArray.size(); ++i)
       {
           WaitDstStageMasks.push_back(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
       }

       auto Fence = reinterpret_cast<FFence*>(Desc.Fence);

       VkTimelineSemaphoreSubmitInfo TimelineSemaphoreSubmitInfo = {};
       TimelineSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
       TimelineSemaphoreSubmitInfo.signalSemaphoreValueCount = (uint32_t)SignalSemaphoreValues.size();
       TimelineSemaphoreSubmitInfo.pSignalSemaphoreValues = SignalSemaphoreValues.data();
       TimelineSemaphoreSubmitInfo.waitSemaphoreValueCount = (uint32_t)WaitSemaphoreValues.size();
       TimelineSemaphoreSubmitInfo.pWaitSemaphoreValues = WaitSemaphoreValues.data();

       VkSubmitInfo SubmitInfo = {};
       SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
       SubmitInfo.pNext = &TimelineSemaphoreSubmitInfo;
       SubmitInfo.signalSemaphoreCount = (uint32_t)SignalSemaphores.size();
       SubmitInfo.pSignalSemaphores = SignalSemaphores.data();
       SubmitInfo.waitSemaphoreCount = (uint32_t)WaitSemaphores.size();
       SubmitInfo.pWaitSemaphores = WaitSemaphores.data();
       SubmitInfo.pWaitDstStageMask = WaitDstStageMasks.data();
       SubmitInfo.commandBufferCount = (uint32_t)CmdBufs.size();
       SubmitInfo.pCommandBuffers = CmdBufs.data();

       vkQueueSubmit(Queue, 1, &SubmitInfo, Fence->GetFence());
    }

    void FQueue::ExcuteCmdList(ICmdList* CmdList, const FExcuteDesc& Desc)
    {
        ICmdList* CmdLists[1] = {CmdList};
        ExcuteCmdLists(CmdLists, 1, Desc);
    }
}