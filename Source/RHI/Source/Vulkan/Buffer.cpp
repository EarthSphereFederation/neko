#include "Backend.h"
#include "vk_mem_alloc.h"
namespace Neko::RHI::Vulkan
{
    FBuffer::FBuffer(const FContext& Ctx, const FBufferDesc& InDesc):Context(Ctx),Desc(InDesc)
    {
        VkBufferCreateInfo BufferCreateInfo = {};
        BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferCreateInfo.size = Desc.Size;
        BufferCreateInfo.usage = ConvertToVkBufferUsageFlags(Desc.BufferUsage);
        BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo AllocInfo = {};
        AllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        AllocInfo.flags = ConvertToVmaAllocationCreateFlags(Desc.BufferUsage);

        vmaCreateBuffer(Context.Allocator, &BufferCreateInfo, &AllocInfo, &Buffer, &Allocation, nullptr);
    }

    FBuffer::~FBuffer()
    {
        if (Buffer)
        {
            vmaDestroyBuffer(Context.Allocator, Buffer, Allocation);
            Buffer = nullptr;
        }
    }

    uint8_t* FBuffer::Map(uint32_t Offset, uint32_t Size)
    {
        bMapped = true;
        void* Data;
        vmaMapMemory(Context.Allocator, Allocation,  &Data);
        return ((uint8_t*)Data + Offset);
    }
    void  FBuffer::Unmap()
    {
        vmaUnmapMemory(Context.Allocator, Allocation);
        bMapped = false;
    }

    IBufferRef FDevice::CreateBuffer(const FBufferDesc& InDesc)
    {
        return new FBuffer(Context, InDesc);
    }

    uint8_t* FDevice::MapBuffer(IBuffer* InBuffer, uint32_t Offset, uint32_t Size)
    {
        auto Buffer = reinterpret_cast<FBuffer*>(InBuffer);
        if (Buffer->IsMapped())
        {
            throw OS::FOSException("Buffer has been mapped");
        }
        return Buffer->Map(Offset, Size);
    }
    void  FDevice::UnmapBuffer(IBuffer* InBuffer)
    {
        auto Buffer = reinterpret_cast<FBuffer*>(InBuffer);
        if (!Buffer->IsMapped())
        {
            throw OS::FOSException("Buffer has not been mapped");
        }
        Buffer->Unmap();
    }

    void  FCmdList::CopyBuffer(IBuffer* InDestBuffer, IBuffer* InSrcBuffer, const FCopyBufferDesc& InDesc)
    {
        auto DestBuffer = reinterpret_cast<FBuffer*>(InDestBuffer);
        auto SrcBuffer = reinterpret_cast<FBuffer*>(InSrcBuffer);

        VkBufferCopy CopyRegion = {};
        CopyRegion.srcOffset = InDesc.SrcOffset;
        CopyRegion.dstOffset = InDesc.DestOffset;
        CopyRegion.size = InDesc.Size;
        vkCmdCopyBuffer(CmdBuffer, SrcBuffer->GetBuffer(), DestBuffer->GetBuffer(), 1, &CopyRegion);
    }

    void FCmdList::BindVertexBuffer(IBuffer* InBuffer,uint32_t Binding, uint64_t Offset)
    {
        auto Buffer = reinterpret_cast<FBuffer*>(InBuffer);
        VkBuffer Buffers[] = { Buffer->GetBuffer() };
        VkDeviceSize Offsets[] = { Offset };

        vkCmdBindVertexBuffers(CmdBuffer, Binding, 1, Buffers, Offsets);
    }

    void FCmdList::BindIndexBuffer(IBuffer* InBuffer, uint64_t Offset, const EIndexBufferType& Type)
    {
        auto Buffer = reinterpret_cast<FBuffer*>(InBuffer);
        vkCmdBindIndexBuffer(CmdBuffer, Buffer->GetBuffer(), Offset, Type == EIndexBufferType::BIT16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
    }
}