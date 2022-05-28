#pragma once
#include "RHI/RHI.h"
#include "OS/Window.h"
#include <vulkan/vulkan.h>
#include <list>
#include <iostream>
#include <vector>
#include <mutex>
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

	constexpr uint32_t MAX_VULKAN_QUEUE_COUNT = 3;

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

	inline VkAttachmentStoreOp ConvertToVkVkAttachmentStoreOp(const EStoreOp& Op)
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

	struct FContext final : public RefCounter<IResource>
	{
		VkInstance Instance = nullptr;
		VkPhysicalDevice PhysicalDevice = nullptr;
		VkDevice Device = nullptr;

		VkAllocationCallbacks *AllocationCallbacks = nullptr;

		VkPhysicalDeviceProperties2 PhyDeviceProperties = {};
		VkPhysicalDeviceMemoryProperties2 PhyDeviceMemoryProperties = {};
		VkDeviceCreateInfo DeviceInfo = {};

		~FContext();
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

	class FQueue;
	class FReusableCmdPool
	{
	private:
		const FContext& Context;
		VkCommandPool CmdPool = nullptr;
	public:
		uint64_t SubmitID = 0;

		std::vector<VkCommandBuffer> CmdBuffers;

		FReusableCmdPool(const FContext&, uint32_t FamliyIndex);
		~FReusableCmdPool();
		
		VkCommandPool GetCmdPool() const { return CmdPool; }

		VkCommandBuffer AllocCmdBuffer();

		void Reset();
	};

	class FQueue final : public RefCounter<IQueue>
	{
	private:
		const FContext& Context;
		uint32_t FamilyIndex;
		ECmdQueueType Type;
		//VkQueueFamilyProperties2 properties;

		VkQueue Queue = nullptr;
		VkSemaphore QueueSemaphore = nullptr;

		std::list<std::shared_ptr<FReusableCmdPool>> UsedCmdPools;
		std::list<std::shared_ptr<FReusableCmdPool>> FreeCmdPools;

		uint64_t RecordingID = 0;
		uint64_t SubmitID = 0;
		uint64_t FinishedID = 0;

		std::vector<VkSemaphore> QueueWaitSemaphores;
		std::vector<uint64_t> QueueWaitSemaphoreValues;
		std::vector<VkSemaphore> QueueSignalSemaphores;
		std::vector<uint64_t> QueueSignalSemaphoreValues;

		std::mutex Mutex;
	public:
		FQueue(const FContext&, uint32_t queueFamliyIndex,uint32_t QueueIndex, ECmdQueueType cmdType);
		~FQueue();
		ECmdQueueType GetCmdQueueType() const { return Type; }
		uint32_t GetFamilyIndex() const { return FamilyIndex; }
		
		bool IsTypeFit(ECmdQueueType InType) { return (uint8_t)(InType & Type) > 0; }

		std::shared_ptr<FReusableCmdPool> GetOrCreateReusableCmdPool();
		std::shared_ptr<FReusableCmdPool> CreateReusableCmdPool();
		
		VkQueue GetQueue() const { return Queue; }
		uint64_t Submit(ICmdList**, uint32_t);
		uint64_t UpdateFinishedID();

		void AddWaitSemaphore(VkSemaphore InWait, uint64_t ID) { QueueWaitSemaphores.push_back(InWait); QueueWaitSemaphoreValues.push_back(ID); }
		void AddSignalSemaphore(VkSemaphore InSignal, uint64_t ID) { QueueSignalSemaphores.push_back(InSignal); QueueSignalSemaphoreValues.push_back(ID); }
		
		void GC();

		[[nodiscard]] virtual ICmdPoolRef CreateCmdPool() override;
		virtual void ExcuteCmdLists(ICmdList** CmdLists, uint32_t CmdListNum) override;
		virtual void ExcuteCmdList(ICmdList* CmdList) override;
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

		bool Initalize(IFrameBuffer* const);
	};

	class FCmdPool final : public RefCounter<ICmdPool>
	{
	private:
		const FContext& Context;
		FQueue& Queue;
		std::shared_ptr<FReusableCmdPool> ReusableCmdPool;
	public:
		FCmdPool(const FContext& Context,FQueue& InQueue);
		~FCmdPool();

		std::shared_ptr<FReusableCmdPool> GetReusableCmdPool() const { return ReusableCmdPool; }

		[[nodiscard]] virtual ICmdListRef CreateCmdList() override;
		virtual ECmdQueueType GetCmdQueueType() override { return Queue.GetCmdQueueType(); }
	};

	class FCmdList final : public RefCounter<ICmdList>
	{
		const FContext& Context;
		FCmdPool* CmdPool;
		//class FDevice* Device;
		VkCommandBuffer CmdBuffer = nullptr;

		RefCountPtr<IFrameBuffer> ActiveFrameBuffer;

	public:
		FCmdList(const FContext&, FCmdPool*);
		~FCmdList();
		VkCommandBuffer GetCmdBuffer() const { return CmdBuffer; }
		FCmdPool*  GetCmdPool() const { return CmdPool; }
		
		virtual void BeginCmd() override;
		virtual void EndCmd() override;
		virtual void SetViewport(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height, float MinDepth = 0.0f, float MaxDepth = 1.0f) override;
		virtual void SetScissor(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height) override;
		virtual void SetViewportNoScissor(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height, float MinDepth = 0.0f, float MaxDepth = 1.0f) override;
		virtual void Draw(uint32_t VertexNum, uint32_t VertexOffset, uint32_t InstanceNum, uint32_t InstanceOffset) override;
		
		virtual void BindFrameBuffer(IFrameBuffer*) override;
		virtual void BindGraphicPipeline(IGraphicPipeline*) override;
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
	
	class FSwapchain final : public RefCounter<ISwapchain>
	{
	private:
		VkSwapchainKHR Swapchain = nullptr;
		VkSurfaceKHR Surface = nullptr;
		uint32_t ImageCount = 0;
		std::vector<VkImage> Images;
		std::vector<VkImageView> ImageViews;
		const FContext& Context;
		VkFormat Format = VkFormat::VK_FORMAT_UNDEFINED;
		VkExtent2D Size = VkExtent2D();
		std::vector<IFrameBufferRef> FrameBuffers;


		uint64_t FrameIndex = 0;
		struct FFrameResource
		{
			VkFence Fence;
			VkSemaphore AcquireSemaphore;
			VkSemaphore PresentSemaphore;
		};
		std::vector<FFrameResource> FrameResources;
	public:
		FSwapchain(const FContext&);
		~FSwapchain();
		bool Initalize(const FSwapChainDesc &Desc);

		VkFormat GetFormat() const { return Format; }
		VkImage GetImage(uint32_t Index) const { CHECK(Index < ImageCount); return Images[Index]; }
		VkImageView GetImageView(uint32_t Index) const { CHECK(Index < ImageCount); return ImageViews[Index]; }
		VkExtent2D GetSize() const { return Size; }
		VkSwapchainKHR GetSwapchain() const { return Swapchain; }
		VkSemaphore GetAcquireSemaphore() const;
		VkSemaphore GetPresentSemaphore() const;

		VkFence GetFrameFence() const;

		uint32_t GetFrameBufferIndex(IFrameBuffer*) const;
		uint64_t NextFrame();

		virtual IFrameBufferRef GetFrameBuffer(uint32_t) override;
	};

	class FFrameBuffer final : public RefCounter<IFrameBuffer>
	{
	private:
		const FContext& Context;
		VkRenderPass RenderPass = nullptr;
		VkFramebuffer FrameBuffer = nullptr;
		FFrameBufferInfo Info;
		VkExtent2D Size;
	public:
		NEKO_PARAM_ARRAY_PRI_PARAM_PUB_FUNC(VkImage, Image, MAX_RENDER_TARGET_COUNT);
		NEKO_PARAM_ARRAY_PRI_PARAM_PUB_FUNC(VkImageView, ImageView, MAX_RENDER_TARGET_COUNT);
	public:
		FFrameBuffer(const FContext&,const RefCountPtr<FSwapchain>&,uint32_t);
		~FFrameBuffer();
		bool Initalize();
		VkRenderPass GetRenderPass() const { return RenderPass; }
		VkFramebuffer GetFrameBuffer() const { return FrameBuffer; }
		VkExtent2D GetSize() const { return Size; }

		virtual const FFrameBufferInfo& GetInfo() override { return Info; };
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

		[[nodiscard]] virtual IQueueRef CreateQueue(const ECmdQueueType& CmdQueueType = ECmdQueueType::Graphic) override;

		[[nodiscard]] virtual IShaderRef CreateShader(const FShaderDesc &) override;

		[[nodiscard]] virtual IGraphicPipelineRef CreateGraphicPipeline(const FGraphicPipelineDesc &, IFrameBuffer* const) override;

		[[nodiscard]] virtual IBindingLayoutRef CreateBindingLayout(const FBindingLayoutDesc &desc) override;

		[[nodiscard]] virtual ISwapchainRef CreateSwapChain(const FSwapChainDesc &desc) override;
		[[nodiscard]] virtual IFrameBufferRef QueueWaitNextFrameBuffer(ISwapchain*, IQueue*) override;
		[[nodiscard]] virtual void QueueWaitPresent(ISwapchain*, IFrameBuffer* , IQueue*) override;

		virtual void GC() override;
		
		virtual bool IsCmdQueueValid(const ECmdQueueType&) override;

		virtual void WaitIdle() override;

		virtual FGPUInfo GetGPUInfo() override;
    };
};