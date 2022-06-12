#pragma once
#include "RHI/RHI.h"
#include "volk.h"
#include "OS/Window.h"
#include <list>
#include <iostream>
#include <vector>
#include <mutex>
#define VULKAN_H_ // workaround for macro pollution
#include "vk_mem_alloc.h"

namespace Neko::RHI::Vulkan
{ 

#define VK_CHECK_THROW(result, fmt, ...) \
	if (result)                          \
	{                                    \
		char Buf[256];                   \
		sprintf(Buf,fmt, __VA_ARGS__);   \
		throw OS::FOSException(Buf);     \
	}

#define CAST(T,Ref) RefCountPtr<T>(reinterpret_cast<T*>(Ref.GetPtr()))

	inline VkSemaphoreType ConvertToVkSemaphoreType(const ESemaphoreType& Type)
	{
		switch (Type)
		{
		case ESemaphoreType::Binary:
		{
			return VkSemaphoreType::VK_SEMAPHORE_TYPE_BINARY;
		}
		case ESemaphoreType::Timeline:
		{
			return VkSemaphoreType::VK_SEMAPHORE_TYPE_TIMELINE;
		}
		default:
			CHECK(false);
			return VkSemaphoreType::VK_SEMAPHORE_TYPE_BINARY;
		}
	}

	inline VkShaderStageFlagBits ConvertToVkShaderStageFlags(const EShaderStage &stage)
	{
		switch (stage)
		{
		case EShaderStage::Vertex:
		{
			return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
		}
		case EShaderStage::Pixel:
		{
			return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		case EShaderStage::All:
		{
			return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		}
		default:
			CHECK(false);
			return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		}
	}

	inline VkVertexInputRate ConvertToVkVertexInputRate(const EVertexRate &rate)
	{
		switch (rate)
		{
		case EVertexRate::Instance:
		{
			return VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE;
		}
		case EVertexRate::Vertex:
		{
			return VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
		}
		default:
			CHECK(false);
			return VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
		}
	}

	inline VkFormat ConvertToVkFormat(const EFormat &format)
	{
		switch (format)
		{
		case EFormat::B8G8R8A8_SNORM:
		{
			return VkFormat::VK_FORMAT_B8G8R8A8_SNORM;
		}
		case EFormat::B8G8R8A8_UNORM:
		{
			return VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
		}
		case EFormat::R32G32_SFLOAT:
		{
			return VkFormat::VK_FORMAT_R32G32_SFLOAT;
		}
		case EFormat::R32G32B32_SFLOAT:
		{
			return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
		}
		default:
			CHECK(false);
			return VkFormat::VK_FORMAT_UNDEFINED;
		}
	}

	inline  EFormat ConvertFromVkFormat(const VkFormat& format)
	{
		switch (format)
		{
		case VkFormat::VK_FORMAT_B8G8R8A8_SNORM:
		{
			return EFormat::B8G8R8A8_SNORM;
		}
		case VkFormat::VK_FORMAT_B8G8R8A8_UNORM:
		{
			return EFormat::B8G8R8A8_UNORM;
		}
		case VkFormat::VK_FORMAT_R32G32_SFLOAT:
		{
			return EFormat::R32G32_SFLOAT;
		}
		case VkFormat::VK_FORMAT_R32G32B32_SFLOAT:
		{
			return EFormat::R32G32B32_SFLOAT;
		}
		default:
			CHECK(false);
			return EFormat::Undefined;
		}
	}

	inline VkPrimitiveTopology ConvertToVkPrimitiveTopology(const EPrimitiveTopology &pt)
	{
		switch (pt)
		{
		case EPrimitiveTopology::TriangleList:
		{
			return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
		default:
			CHECK(false);
			return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
	}

	inline VkPolygonMode ConvertToVkPolygonMode(const EPolygonMode &pm)
	{
		switch (pm)
		{
		case EPolygonMode::Fill:
		{
			return VkPolygonMode::VK_POLYGON_MODE_FILL;
		}
		default:
			CHECK(false);
			return VkPolygonMode::VK_POLYGON_MODE_FILL;
		}
	}

	inline VkCullModeFlagBits ConvertToVkCullModeFlagBits(const ECullMode &cm)
	{
		switch (cm)
		{
		case ECullMode::None:
		{
			return VkCullModeFlagBits::VK_CULL_MODE_NONE;
		}
		case ECullMode::Front:
		{
			return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
		}
		case ECullMode::Back:
		{
			return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
		}
		default:
			CHECK(false);
			return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
		}
	}

	inline VkFrontFace ConvertVkFrontFace(const EFrontFace &ff)
	{
		switch (ff)
		{
		case EFrontFace::CW:
		{
			return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
		}
		case EFrontFace::CCW:
		{
			return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}
		default:
			CHECK(false);
			return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}
	}

	inline VkSampleCountFlagBits ConvertToVkSampleCountFlagBits(const ESampleCount &sc)
	{
		switch (sc)
		{
		case ESampleCount::SampleCount_1:
		{
			return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		}
		case ESampleCount::SampleCount_2:
		{
			return VkSampleCountFlagBits::VK_SAMPLE_COUNT_2_BIT;
		}
		case ESampleCount::SampleCount_4:
		{
			return VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT;
		}
		case ESampleCount::SampleCount_8:
		{
			return VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT;
		}
		case ESampleCount::SampleCount_16:
		{
			return VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT;
		}
		default:
			CHECK(false);
			return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		}
	}

	inline VkStencilOp ConvertToVkStencilOp(const EStencilOp &op)
	{
		switch (op)
		{
		case EStencilOp::Keep:
		{
			return VkStencilOp::VK_STENCIL_OP_KEEP;
		}
		default:
			CHECK(false);
			return VkStencilOp::VK_STENCIL_OP_KEEP;
		}
	}

	inline VkCompareOp ConvertToVkCompareOp(const ECompareOp &op)
	{
		switch (op)
		{
		case ECompareOp::Never:
		{
			return VkCompareOp::VK_COMPARE_OP_NEVER;
		}
		case ECompareOp::Less:
		{
			return VkCompareOp::VK_COMPARE_OP_LESS;
		}
		case ECompareOp::Always:
		{
			return VkCompareOp::VK_COMPARE_OP_ALWAYS;
		}
		default:
			CHECK(false);
			return VkCompareOp::VK_COMPARE_OP_ALWAYS;
		}
	}

	inline VkBlendFactor ConvertToVkBlendFactor(const EBlendFactor &op)
	{
		switch (op)
		{
		case EBlendFactor::One:
		{
			return VkBlendFactor::VK_BLEND_FACTOR_ONE;
		}
		case EBlendFactor::Zero:
		{
			return VkBlendFactor::VK_BLEND_FACTOR_ZERO;
		}
		default:
			CHECK(false);
			return VkBlendFactor::VK_BLEND_FACTOR_ONE;
		}
	}

	inline VkBlendOp ConvertToVkBlendOp(const EBlendOp &op)
	{
		switch (op)
		{
		case EBlendOp::Add:
		{
			return VkBlendOp::VK_BLEND_OP_ADD;
		}
		default:
			CHECK(false);
			return VkBlendOp::VK_BLEND_OP_ADD;
		}
	}

	inline VkColorComponentFlags ConvertToVkColorComponentFlags(const EColorComponent &op)
	{
		VkColorComponentFlags ret = 0;
		if ((op & EColorComponent::R) != 0)
		{
			ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT;
		}
		if ((op & EColorComponent::G) != 0)
		{
			ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT;
		}
		if ((op & EColorComponent::B) != 0)
		{
			ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT;
		}
		if ((op & EColorComponent::A) != 0)
		{
			ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
		}
		if ((op & EColorComponent::All) != 0)
		{
			ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT;
			ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT;
			ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT;
			ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
		}
		return ret;
	}

	inline VkDescriptorType ConvertToVkDescriptorType(const EResourceType &rType)
	{
		switch (rType)
		{
		case EResourceType::UniformBuffer:
		{
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		}
		default:
			CHECK(false);
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		}
	}

	inline VkColorSpaceKHR ConvertToVkColorSpaceKHR(const EFormat &Format)
	{
		switch (Format)
		{
		case EFormat::B8G8R8A8_SNORM:
		{
			return VkColorSpaceKHR::VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT;
		}
		default:
			CHECK(false);
			return VkColorSpaceKHR::VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT;
		}
	}

	inline VkAttachmentLoadOp ConvertToVkAttachmentLoadOp(const ELoadOp& Op)
	{
		switch (Op)
		{
		case ELoadOp::Load:
		{
			return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
		}
		default:
			CHECK(false);
			return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
		}
	}

	inline VkAttachmentStoreOp ConvertToVkAttachmentStoreOp(const EStoreOp& Op)
	{
		switch (Op)
		{
		case EStoreOp::Store:
		{
			return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
		}
		default:
			CHECK(false);
			return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
		}
	}

	inline VkBufferUsageFlags ConvertToVkBufferUsageFlags(const EBufferUsage& Usage)
	{
		VkBufferUsageFlags ret = 0;
		if ((Usage & EBufferUsage::VertexBuffer) != 0)
		{
			ret |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}		
		if ((Usage & EBufferUsage::IndexBuffer) != 0)
		{
			ret |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if ((Usage & EBufferUsage::TransferSrc) != 0)
		{
			ret |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		}
		if ((Usage & EBufferUsage::TransferDest) != 0)
		{
			ret |= VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}
		return ret;
	}

	inline VmaAllocationCreateFlags ConvertToVmaAllocationCreateFlags(const EBufferUsage& Usage)
	{
		VmaAllocationCreateFlags ret = 0;
		if ((Usage & EBufferUsage::CPUAccess) != 0)
		{
			ret |= VmaAllocationCreateFlagBits::VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		}
		return ret;
	}


	inline VkAccessFlags ConvertToVkAccessFlags(const EResourceState& State)
	{
		switch (State)
		{
		case EResourceState::Undefined:
		{
			return VkAccessFlagBits::VK_ACCESS_NONE;
		}
		case EResourceState::RenderTarget:
		{
			return VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}
		case EResourceState::Present:
		{
			return VkAccessFlagBits::VK_ACCESS_MEMORY_READ_BIT;
		}
		default:
			CHECK(false);
			return VkAccessFlagBits::VK_ACCESS_NONE;
		}
	}

	inline VkImageLayout ConvertToVkImageLayout(const EResourceState& State)
	{
		switch (State)
		{
		case EResourceState::Undefined:
		{
			return VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
		}
		case EResourceState::RenderTarget:
		{
			return VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		case EResourceState::Present:
		{
			return VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}
		default:
			CHECK(false);
			return VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}

	inline VkPipelineStageFlags ConvertToVkPipelineStageFlags(const EResourceState& State)
	{
		switch (State)
		{
		case EResourceState::Undefined:
		{
			return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_NONE;
		}
		case EResourceState::RenderTarget:
		{
			return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		case EResourceState::Present:
		{
			return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}
		default:
			CHECK(false);
			return VkPipelineStageFlagBits::VK_PIPELINE_STAGE_NONE;
		}
	}

	struct FContext final : public RefCounter<IResource>
	{
		VkInstance Instance = nullptr;
		VkPhysicalDevice PhysicalDevice = nullptr;
		VkDevice Device = nullptr;

		VkAllocationCallbacks *AllocationCallbacks = nullptr;

		VkPhysicalDeviceProperties2 PhyDeviceProperties = {};
		VkPhysicalDeviceMemoryProperties2 PhyDeviceMemoryProperties = {};
		VkDeviceCreateInfo DeviceInfo = {};

		VmaAllocator Allocator;

		~FContext();
	};

	class FSemaphore : public RefCounter<ISemaphore>
	{
	private:
		const FContext& Context;
		VkSemaphore Semaphore = nullptr;
		float Counter = 0;
	public:
		FSemaphore(const FContext& ,const ESemaphoreType&);
		~FSemaphore();
		VkSemaphore GetSemaphore() const { return Semaphore; }
		virtual void SetCounter(float Value) override { Counter = Value; };
		virtual float GetCounter() override { return Counter; }
	};

	class FFence : public RefCounter<IFence>
	{
	private:
		const FContext& Context;
		VkFence Fence = nullptr;
	public:
		FFence(const FContext&,const EFenceFlag&);
		~FFence();
		VkFence GetFence() const { return Fence; }

		virtual void Wait() override;
		virtual void Reset() override;
	};

	class FShader final : public RefCounter<IShader>
	{
	private:
		const FContext& Context;
		VkShaderModule ShaderModule = nullptr;
		FShaderDesc Desc;

	public:
		FShader(const FContext&);
		~FShader();
		bool Initalize(const FShaderDesc &);
		virtual const FShaderDesc &GetDesc() override { return Desc; }

	public:
		const VkShaderModule GetVkShaderModule() const { return ShaderModule; }
	};

	class FQueue final : public RefCounter<IQueue>
	{
	private:
		const FContext& Context;
		uint32_t FamilyIndex;
		ECmdQueueType Type;

		VkQueue Queue = nullptr;
	public:
		FQueue(const FContext&, uint32_t queueFamliyIndex,uint32_t QueueIndex, ECmdQueueType cmdType);
		~FQueue();
		ECmdQueueType GetCmdQueueType() const { return Type; }
		uint32_t GetFamilyIndex() const { return FamilyIndex; }
		bool IsMatch(ECmdQueueType InType) { return (uint8_t)(InType & Type) > 0; }
		VkQueue GetQueue() { return Queue; }

		
		
		virtual void ExcuteCmdLists(ICmdList** CmdLists, uint32_t CmdListNum, const FExcuteDesc& Desc) override;
		virtual void ExcuteCmdList(ICmdList* CmdList, const FExcuteDesc& Desc) override;

		[[nodiscard]] virtual std::vector<ICmdPoolRef> CreateCmdPools(uint32_t) override;
		[[nodiscard]] virtual ICmdPoolRef CreateCmdPool() override;
	};

	class FGraphicPipeline final : public RefCounter<IGraphicPipeline>
	{
	private:
		const FContext& Context;
		FGraphicPipelineDesc Desc;
		VkPipelineLayout PipelineLayout = nullptr;
		VkPipeline Pipeline = nullptr;

	public:
		FGraphicPipeline(const FContext&, const FGraphicPipelineDesc&);
		~FGraphicPipeline();

		VkPipeline GetPipeline() const { return Pipeline; }

		bool Initalize();
	};

	class FCmdPool final : public RefCounter<ICmdPool>
	{
	private:
		std::mutex Mutex;
		const FContext& Context;
		FQueue& Queue;
		VkCommandPool CmdPool = nullptr;
		std::vector<ICmdListRef> CmdLists;
	public:
		FCmdPool(const FContext& Context,FQueue& InQueue);
		~FCmdPool();

		VkCommandPool GetCmdPool() { return CmdPool; }


		[[nodiscard]] virtual ICmdListRef CreateCmdList() override;
		virtual void Free() override;
		virtual ECmdQueueType GetCmdQueueType() override { return Queue.GetCmdQueueType(); }
	};

	class FCmdList final : public RefCounter<ICmdList>
	{
		const FContext& Context;
		FCmdPool* CmdPool;
		//class FDevice* Device;
		VkCommandBuffer CmdBuffer = nullptr;
	public:
		FCmdList(const FContext&, FCmdPool*);
		~FCmdList();
		VkCommandBuffer GetCmdBuffer() const { return CmdBuffer; }
		FCmdPool*  GetCmdPool() const { return CmdPool; }
		
		virtual void BeginCmd() override;
		virtual void EndCmd() override;
		virtual void BeginRenderPass(const FRenderPassDesc&) override;
		virtual void EndRenderPass() override;

		virtual void SetViewport(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height, float MinDepth = 0.0f, float MaxDepth = 1.0f) override;
		virtual void SetScissor(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height) override;
		virtual void SetViewportNoScissor(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height, float MinDepth = 0.0f, float MaxDepth = 1.0f) override;
		virtual void Draw(uint32_t VertexNum, uint32_t VertexOffset) override;
		virtual void DrawIndexed(uint32_t IndexCount, uint32_t FirstIndex, uint32_t VertexOffset) override;

		virtual void BindGraphicPipeline(IGraphicPipeline*) override;
		virtual void ResourceBarrier(const FTextureTransitionDesc&) override;

		virtual void CopyBuffer(IBuffer*, IBuffer*, const FCopyBufferDesc&) override;
		virtual void BindVertexBuffer(IBuffer* InBuffer, uint32_t Binding, uint64_t Offset) override;
		virtual void BindIndexBuffer(IBuffer* InBuffer, uint64_t Offset, const EIndexBufferType& Type) override;
	};

	class FBindingLayout final : public RefCounter<IBindingLayout>
	{
		VkDescriptorSetLayout DescriptorSetLayout = nullptr;
		const FContext& Context;
	public:
		FBindingLayout(const FContext&);
		~FBindingLayout();
		bool Initalize(const FBindingLayoutDesc &desc);

		VkDescriptorSetLayout GetDescriptorSetLayout() const { return DescriptorSetLayout; }
	};
	
	class FColorAttachment final : public RefCounter<IColorAttachment>
	{
	public:
		const FContext& Context;
		VkImageView ImageView = nullptr;
		FColorAttachmentDesc Desc;
	
		FColorAttachment(const FContext&, const FColorAttachmentDesc&);
		~FColorAttachment();
		VkImageView GetImageView() const { return ImageView; }
	public:
		virtual const FColorAttachmentDesc& GetDesc() override { return Desc; };
	};

	class FTexture final : public RefCounter <ITexture>
	{
	private:
		const FContext& Context;
		VkImage Image = nullptr;
		FTextureDesc Desc;
		bool bAutoRelease = false;
	public:
		FTexture(const FContext&, VkImage, const FTextureDesc&,bool InbAutoRelease = false);

		VkImage GetImage() const { return Image; }
	public:
		virtual const FTextureDesc& GetDesc() override { return Desc; };
	};

	class FTexture2DView final : public RefCounter<ITexture2DView>
	{
	private:
		const FContext& Context;
		VkImageView ImageView = nullptr;
		FTexture2DViewDesc Desc;
	public:
		FTexture2DView(const FContext&, const FTexture2DViewDesc&);
		~FTexture2DView();
		VkImageView GetImageView() const { return ImageView; }
	public:
		virtual ITextureRef GetTexture() override;
		virtual const FTexture2DViewDesc& GetDesc() override;
	};

	class FBuffer final : public RefCounter<IBuffer>
	{
	private:
		const FContext& Context;
		VkBuffer Buffer = nullptr;
		FBufferDesc Desc;
		VmaAllocation Allocation;

		bool bMapped = false;
	public:
		FBuffer(const FContext&, const FBufferDesc&);
		~FBuffer();
	
		uint8_t* Map(uint32_t Offset, uint32_t Size);
		void  Unmap();
		bool IsMapped() const { return bMapped; }
		VkBuffer GetBuffer() const { return Buffer; }
	public:
		virtual const FBufferDesc& GetDesc() override { return Desc; }

		friend class FDevice;
	};

	class FSwapchain final : public RefCounter<ISwapchain>
	{
	private:
		VkSwapchainKHR Swapchain = nullptr;
		VkSurfaceKHR Surface = nullptr;
		uint32_t ImageCount = 0;
		uint32_t ImageIndex = 0;
		const FContext& Context;
		std::vector<ITextureRef> Textures;
	public:
		FSwapchain(const FContext&);
		~FSwapchain();
		bool Initalize(const FSwapChainDesc &Desc);
		VkSwapchainKHR GetSwapchain() const { return Swapchain; }
	public:
		virtual uint32_t AcquireNext(ISemaphore*, IFence*) override;
		virtual void Present(const FPresentDesc&) override;
		virtual uint32_t GetTextureNum() override;
		virtual std::vector<ITextureRef> GetTextures() override;
		virtual void Reset() override;
	};

	class FDevice final : public RefCounter<IDevice>
	{
	private:
		FContext Context;
		std::vector<RefCountPtr<FQueue>> FreeQueues;
		std::vector<RefCountPtr<FQueue>> UsedQueues;
	public:
		FDevice();
		~FDevice() {}
		bool Initalize(const FDeviceDesc &desc);

		[[nodiscard]] virtual ISemaphoreRef CreateSemaphore(const ESemaphoreType&) override;
		[[nodiscard]] virtual std::vector<ISemaphoreRef> CreateSemaphores(const ESemaphoreType&,uint32_t) override;
		[[nodiscard]] virtual IFenceRef CreateFence(const EFenceFlag&) override;
		[[nodiscard]] virtual std::vector<IFenceRef> CreateFences(const EFenceFlag&, uint32_t) override;
		[[nodiscard]] virtual IQueueRef CreateQueue(const ECmdQueueType& CmdQueueType = ECmdQueueType::Graphic) override;
		[[nodiscard]] virtual IShaderRef CreateShader(const FShaderDesc &) override;
		[[nodiscard]] virtual IGraphicPipelineRef CreateGraphicPipeline(const FGraphicPipelineDesc &) override;
		[[nodiscard]] virtual IBindingLayoutRef CreateBindingLayout(const FBindingLayoutDesc &desc) override;
		[[nodiscard]] virtual ISwapchainRef CreateSwapChain(const FSwapChainDesc &desc) override;
		[[nodiscard]] virtual ITexture2DViewRef CreateTexture2DView(const FTexture2DViewDesc&) override;
		[[nodiscard]] virtual ITexture2DViewRef CreateTexture2DView(ITexture*) override;
		[[nodiscard]] virtual IColorAttachmentRef CreateColorAttachment(const FColorAttachmentDesc&) override;
		[[nodiscard]] virtual IBufferRef CreateBuffer(const FBufferDesc&) override;

		[[nodiscard]] virtual uint8_t* MapBuffer(IBuffer*, uint32_t Offset, uint32_t Size) override;
		[[nodiscard]] virtual void UnmapBuffer(IBuffer*) override;
		
		virtual bool IsCmdQueueValid(const ECmdQueueType&) override;

		virtual void WaitIdle() override;

		virtual FGPUInfo GetGPUInfo() override;
    };
};
