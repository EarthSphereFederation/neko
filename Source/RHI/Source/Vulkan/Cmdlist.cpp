#include "Backend.h"
#include <cassert>
#include <map>
#include <vector>
namespace Neko::RHI::Vulkan
{ 
    FCmdBuffer::~FCmdBuffer()
    {
        if (CmdPool)
        {
            vkDestroyCommandPool(Context.Device, CmdPool, Context.AllocationCallbacks);
            CmdPool = nullptr;
        }
    }

    FQueue::FQueue(const FContext &Ctx, uint32_t InQueueFamliyIndex, uint32_t QueueIndex, ECmdQueueType InCmdType) : Context(Ctx),FamilyIndex(InQueueFamliyIndex), Type(InCmdType)
    { 
        vkGetDeviceQueue(Context.Device, InQueueFamliyIndex, QueueIndex, &Queue);
        {
            VkSemaphoreTypeCreateInfo SemaphoreTypeCreateInfo = {};
            SemaphoreTypeCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
            SemaphoreTypeCreateInfo.initialValue = 0;
            SemaphoreTypeCreateInfo.semaphoreType = VkSemaphoreType::VK_SEMAPHORE_TYPE_TIMELINE;

            VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
            SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            SemaphoreCreateInfo.pNext = &SemaphoreTypeCreateInfo;

            vkCreateSemaphore(Context.Device, &SemaphoreCreateInfo, Context.AllocationCallbacks, &QueueSemaphore);
        }
       
        {
            VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
            SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            vkCreateSemaphore(Context.Device, &SemaphoreCreateInfo, Context.AllocationCallbacks, &QueueSemaphoreForSwapchain);
        }
    }

    FQueue::~FQueue()
    {
        if (QueueSemaphore)
        {
            vkDestroySemaphore(Context.Device, QueueSemaphore, Context.AllocationCallbacks);
            QueueSemaphore = nullptr;
        }

        if (QueueSemaphoreForSwapchain)
        {
            vkDestroySemaphore(Context.Device, QueueSemaphoreForSwapchain, Context.AllocationCallbacks);
            QueueSemaphoreForSwapchain = nullptr;
        }
    }

    uint64_t FQueue::UpdateFinishedID()
    {
        uint64_t _FinishedID;
        vkGetSemaphoreCounterValue(Context.Device, QueueSemaphore, &_FinishedID);
        FinishedID = _FinishedID;
        return FinishedID;
    }

    void FQueue::GC()
    {
        auto _FinishedID = UpdateFinishedID();
        
        auto Submitions = std::move(SubmitedCmdBuffers);
        for (auto& Submition : Submitions)
        {
            if (Submition->SubmitID <= _FinishedID)
            {
                Submition->SubmitID = 0;
                FreeCmdBuffers.push_back(Submition);
            }
            else
            {
                SubmitedCmdBuffers.push_back(Submition);
            }
        }
    }

    std::shared_ptr<FCmdBuffer> FQueue::CreateCmdBuffer()
    {
        auto CmdBuf = std::make_shared<FCmdBuffer>(Context);
        VkCommandPoolCreateInfo CommandPoolInfo = {};
        CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        CommandPoolInfo.queueFamilyIndex = FamilyIndex;
        CommandPoolInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VK_CHECK_THROW(vkCreateCommandPool(Context.Device, &CommandPoolInfo, nullptr, &CmdBuf->CmdPool), "Failed to create cmd pool with queue type %d", (uint32_t)Type);

        VkCommandBufferAllocateInfo CmdBufAllocateInfo = {};
        CmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        CmdBufAllocateInfo.commandPool = CmdBuf->CmdPool;
        CmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        CmdBufAllocateInfo.commandBufferCount = 1;

        VK_CHECK_THROW(vkAllocateCommandBuffers(Context.Device, &CmdBufAllocateInfo, &CmdBuf->CmdBuf), "Failed to create cmd buffer");
        return CmdBuf;
    }

    std::shared_ptr<FCmdBuffer> FQueue::GetOrCreateCmdBuffer()
    {
        RecordingID++;
        std::shared_ptr<FCmdBuffer> CmdBuffer;
        if (FreeCmdBuffers.size() > 0)
        {
            CmdBuffer = FreeCmdBuffers.front();
            FreeCmdBuffers.pop_front();
        }
        else
        {
            CmdBuffer = CreateCmdBuffer();
        }
        CmdBuffer->RecordingID = RecordingID;
        return CmdBuffer;
    }

    uint64_t FQueue::Submit(ICmdList** CmdLists, uint32_t CmdListNum)
    {
        assert(CmdListNum >= 0);

        SubmitID++;

        std::vector<VkPipelineStageFlags> WaitDstStageMasks;
        WaitDstStageMasks.reserve(QueueWaitSemaphores.size());
        for (uint32_t i = 0; i < QueueWaitSemaphores.size(); ++i)
        {
            WaitDstStageMasks.push_back(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
        }

        std::vector<VkCommandBuffer> CmdBufs;
        CmdBufs.reserve(CmdListNum);
        for (uint32_t i = 0; i < CmdListNum; ++i)
        {
            if (CmdLists[i])
            {
                auto CmdBuf = reinterpret_cast<FCmdList*>(CmdLists[i])->GetCurrentCmdBuffer();
                if (CmdBuf)
                {
                    CmdBufs.push_back(CmdBuf->CmdBuf);
                    SubmitedCmdBuffers.push_back(CmdBuf);
                }
            }
        }

        std::vector<VkSemaphore> SignalSemaphores;
        std::vector<uint64_t> SignalSemaphoreValues;
        
        SignalSemaphores.push_back(QueueSemaphore);
        SignalSemaphoreValues.push_back(SubmitID);

        SignalSemaphores.push_back(QueueSemaphoreForSwapchain);
        SignalSemaphoreValues.push_back(0);

        for (uint32_t i=0;i< QueueSignalSemaphores.size();++i)
        {
            SignalSemaphores.push_back(QueueSignalSemaphores[i]);
            SignalSemaphoreValues.push_back(QueueSignalSemaphoreValues[i]);
        }

        std::vector<VkSemaphore> WaitSemaphores;
        std::vector<uint64_t> WaitSemaphoreValues;

        for (uint32_t i = 0; i < QueueWaitSemaphores.size(); ++i)
        {
            WaitSemaphores.push_back(QueueWaitSemaphores[i]);
            WaitSemaphoreValues.push_back(QueueWaitSemaphoreValues[i]);
        }

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

        vkQueueSubmit(Queue, 1, &SubmitInfo, nullptr);

        bool Ret = QueueWaitSemaphores.empty();
        Ret = QueueWaitSemaphoreValues.empty();
        Ret = QueueSignalSemaphores.empty();
        Ret = QueueSignalSemaphoreValues.empty();

        return SubmitID;
    }

    FCmdList::FCmdList(FDevice* InDevice, const FContext & Ctx, const FCmdListDesc& InDesc):Device(InDevice), Context(Ctx), Desc(InDesc)
    {
    }

    FCmdList::~FCmdList()
    {

    }

    void FCmdList::BeginCmd()
    {
        if (!Device->IsCmdQueueValid(Desc.Type))
        {
            throw OS::FOSException("Invalid command queue type");
        }
        auto& Queue = Device->GetQueue(Desc.Type);
        CurrentCmdBufferPtr = Queue.GetOrCreateCmdBuffer();
       
        VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
        CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        CommandBufferBeginInfo.flags = 0;

        VK_CHECK_THROW(vkBeginCommandBuffer(CurrentCmdBufferPtr->GetCommandBuffer(), &CommandBufferBeginInfo),"Failed to begin command buffer");
    }

    void FCmdList::EndCmd()
    {
        if (ActiveFrameBuffer)
        {
            vkCmdEndRenderPass(CurrentCmdBufferPtr->GetCommandBuffer());
            ActiveFrameBuffer = nullptr;
        }

        VK_CHECK_THROW(vkEndCommandBuffer(CurrentCmdBufferPtr->GetCommandBuffer()), "Failed to end command buffer");
    }
    
    void FCmdList::BindFrameBuffer(IFrameBuffer* InFrameBuffer)
    {
        if (ActiveFrameBuffer)
        {
            vkCmdEndRenderPass(CurrentCmdBufferPtr->GetCommandBuffer());
            ActiveFrameBuffer = nullptr;
        }

        ActiveFrameBuffer = InFrameBuffer;

        FFrameBuffer* FrameBuffer = reinterpret_cast<FFrameBuffer*>(InFrameBuffer);
        auto Info = FrameBuffer->GetInfo();
        VkRenderPassBeginInfo RenderPassBeginInfo = {};
        RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        RenderPassBeginInfo.framebuffer = FrameBuffer->GetFrameBuffer();
        RenderPassBeginInfo.renderPass = FrameBuffer->GetRenderPass();
        RenderPassBeginInfo.renderArea = { 0,0,(uint32_t)FrameBuffer->GetSize().width,FrameBuffer->GetSize().height };
        
        static_vector<VkClearValue, MAX_RENDER_TARGET_COUNT + 1> ClearValues;
        for (uint32_t i = 0; i < Info.FormatArray.size(); ++i)
        {
            ClearValues.push_back({ 0.0f,0.0f ,0.0f ,1.0f });;
        }
        
        RenderPassBeginInfo.clearValueCount = (uint32_t)ClearValues.size();
        RenderPassBeginInfo.pClearValues = ClearValues.data();

        vkCmdBeginRenderPass(CurrentCmdBufferPtr->GetCommandBuffer(), &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void FCmdList::BindGraphicPipeline(IGraphicPipeline* InGraphicPipeline)
    {
       auto GraphicPipeline = reinterpret_cast<FGraphicPipeline*>(InGraphicPipeline);
       vkCmdBindPipeline(CurrentCmdBufferPtr->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline->GetPipeline());
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
        vkCmdSetViewport(CurrentCmdBufferPtr->GetCommandBuffer(), 0, 1, &Viewport);
    }

    void FCmdList::SetScissor(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height)
    {
        VkRect2D Scissor = {};
        Scissor.offset.x = X;
        Scissor.offset.y = Y;
        Scissor.extent.width = Width;
        Scissor.extent.height = Height;
        vkCmdSetScissor(CurrentCmdBufferPtr->GetCommandBuffer(), 0, 1, &Scissor);
    }

    void FCmdList::SetViewportNoScissor(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height, float MinDepth, float MaxDepth)
    {
        SetViewport(X, Width, Y, Height, MinDepth, MaxDepth);
        SetScissor(X, Width, Y, Height);
    }
    
    void FCmdList::Draw(uint32_t VertexNum, uint32_t VertexOffset, uint32_t InstanceNum, uint32_t InstanceOffset)
    {
        vkCmdDraw(CurrentCmdBufferPtr->GetCommandBuffer(), VertexNum, InstanceNum, VertexOffset, InstanceOffset);
    }

    ICmdListRef FDevice::CreateCmdList(const FCmdListDesc &InDesc)
    {
        return new FCmdList(this, Context, InDesc);
    }

    void FDevice::ExcuteCmdLists(ICmdList** CmdLists, uint32_t CmdListNum, const ECmdQueueType& CmdQueueType)
    {
        if (IsCmdQueueValid(CmdQueueType))
        {
            auto& Queue = GetQueue(CmdQueueType);
            auto SubmitionId = Queue.Submit(CmdLists, CmdListNum);

            for (uint32_t CmdListIndex = 0; CmdListIndex < CmdListNum; ++CmdListIndex)
            {
                reinterpret_cast<FCmdList*>(CmdLists[CmdListIndex])->PostExcute(SubmitionId);
            }
        }
    }

    void FDevice::ExcuteCmdList(ICmdList* CmdList, const ECmdQueueType& CmdQueueType)
    {
        ICmdList* CmdLists[1] = {CmdList};
        ExcuteCmdLists(CmdLists, 1, CmdQueueType);
    }
}