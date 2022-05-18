#pragma once
#include "RHI/RHI.h"
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#pragma warning(disable : 26812)
namespace Neko::RHI::Vulkan
{ 
#define VK_CHECK_F(result, fmt, ...) \
	if (result)                      \
	{                                \
		printf(fmt, __VA_ARGS__);    \
		printf("\n");                \
		assert(false);               \
	}

#define VK_CHECK_RETURN_FALSE(result, fmt, ...) \
	if (result)                                 \
	{                                           \
		printf(fmt, __VA_ARGS__);               \
		printf("\n");                           \
		return false;                           \
	}

	constexpr uint32_t MAX_VULKAN_QUEUE_COUNT = 3;

	inline VkShaderStageFlagBits ConvertToVkShaderStageFlags(const EShaderStage &stage)
	{
		switch (stage)
		{
		case EShaderStage::VS:
		{
			return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
		}
		case EShaderStage::PS:
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

	struct VulkanContext final : public RefCounter<RHIResource>
	{
		VkInstance Instance = nullptr;
		VkPhysicalDevice PhysicalDevice = nullptr;
		VkDevice Device = nullptr;

		VkAllocationCallbacks *AllocationCallbacks = nullptr;

		VkPhysicalDeviceProperties2 PhyDeviceProperties = {};
		VkPhysicalDeviceMemoryProperties2 PhyDeviceMemoryProperties = {};
		VkDeviceCreateInfo DeviceInfo = {};

		~VulkanContext();
	};
	typedef RefCountPtr<VulkanContext> VulkanContextPtr;

	class FShader final : public RefCounter<RHIShader>
	{
	private:
		const VulkanContextPtr Context;
		VkShaderModule ShaderModule = nullptr;
		RHIShaderDesc Desc;

	public:
		FShader(const VulkanContextPtr &ctx);
		~FShader();
		bool Initalize(const RHIShaderDesc &desc);
		virtual const RHIShaderDesc &GetDesc() const override { return Desc; }

	public:
		const VkShaderModule GetVkShaderModule() const { return ShaderModule; }
	};

	class FCmdBuffer
	{
	private:
		VkCommandPool CmdPool = nullptr;
		VkCommandBuffer CmdBuf = nullptr;
		const VulkanContextPtr context;

	public:
		FCmdBuffer(const VulkanContextPtr &ctx) : context(ctx) {}
		~FCmdBuffer();
		friend class FQueue;
	};
	typedef std::shared_ptr<FCmdBuffer> VulkanCmdBufferPtr;

	class FQueue final : public RefCounter<RHIResource>
	{
	private:
		const VulkanContextPtr context;
		uint32_t FamilyIndex;
		ECmdQueueType Type;
		VkQueueFamilyProperties2 properties;

		std::list<VulkanCmdBufferPtr> RunningCmdBuffers;
		std::list<VulkanCmdBufferPtr> CmdBufferPool;

	public:
		FQueue(const VulkanContextPtr &ctx, uint32_t queueFamliyIndex, ECmdQueueType cmdType, VkQueueFamilyProperties2 properties);
		~FQueue();
		ECmdQueueType GetCmdQueueType() const { return Type; }
		uint32_t GetFamilyIndex() const { return FamilyIndex; }
		VkQueueFamilyProperties2 GetFamilyProperties() const { return properties; }

		VulkanCmdBufferPtr GetOrCreateCmdBuffer();
		VulkanCmdBufferPtr CreateCmdBuffer();
	};
	typedef RefCountPtr<FQueue> VulkanQueuePtr;

	class FGraphicPipeline final : public RefCounter<RHIGraphicPipeline>
	{
	private:
		const VulkanContextPtr Context;
		RHIGraphicPipelineDesc Desc;
		VkRenderPass RenderPass = nullptr;
		VkPipelineLayout PipelineLayout = nullptr;
		VkPipeline Pipeline = nullptr;

	public:
		FGraphicPipeline(const VulkanContextPtr &, const RHIGraphicPipelineDesc &);
		~FGraphicPipeline();

		bool Initalize(const RHIFrameBufferInfo&);
	};

	class FCmdList final : public RefCounter<RHICmdList>
	{
		VulkanCmdBufferPtr CmdBuf;
		VulkanContextPtr Context;

	public:
		FCmdList(const VulkanContextPtr &, const VulkanCmdBufferPtr &);
	};

	class FBindingLayout final : public RefCounter<RHIBindingLayout>
	{
		VkDescriptorSetLayout DescriptorSetLayout = nullptr;
		VulkanContextPtr Context;

	public:
		FBindingLayout(const VulkanContextPtr &);
		~FBindingLayout();
		bool Initalize(const RHIBindingLayoutDesc &desc);
		virtual NativeObject GetNativeObject() const override { return DescriptorSetLayout; }
	};

	class FSwapchain final : public RefCounter<RHISwapchain>
	{
	private:
		VkSwapchainKHR Swapchain = nullptr;
		uint32_t ImageCount = 0;
		std::vector<VkImage> Images;
		std::vector<VkImageView> ImageViews;
		VulkanContextPtr Context;
		VkFormat Format = VkFormat::VK_FORMAT_UNDEFINED;

	public:
		FSwapchain(const VulkanContextPtr &);
		~FSwapchain();
		bool Initalize(const RHISwapChainDesc &Desc);

		VkFormat GetFormat() const { return Format; }
		VkImage GetImage(uint32_t Index) const { CHECK(Index < ImageCount); return Images[Index]; }
		VkImageView GetImageView(uint32_t Index) const { CHECK(Index < ImageCount); return ImageViews[Index]; }

		virtual RHIFrameBufferRef GetFrameBuffer(uint32_t) const override;
	};

	class FFrameBuffer final : public RefCounter<RHIFrameBuffer>
	{
	private:
		RHIFrameBufferInfo Info;
	public:
		NEKO_PARAM_ARRAY_PRI_PARAM_PUB_FUNC(VkImage, Image, MAX_RENDER_TARGET_COUNT);
		NEKO_PARAM_ARRAY_PRI_PARAM_PUB_FUNC(VkImageView, ImageView, MAX_RENDER_TARGET_COUNT);
		NEKO_PARAM_ARRAY_PRI_PARAM_PUB_FUNC(VkFramebuffer, FrameBuffer, MAX_RENDER_TARGET_COUNT);
	public:
		FFrameBuffer(const FSwapchain& Swapchain,uint32_t Index);
		virtual const RHIFrameBufferInfo& GetInfo() const override { return Info; };
	};
	
	class FDevice final : public RefCounter<RHIDevice>
	{
	private:
		VulkanContextPtr Context;
		static_vector<VulkanQueuePtr, MAX_VULKAN_QUEUE_COUNT> Queues;

	public:
		FDevice();
		~FDevice() {}
		bool Initalize(const RHIDeviceDesc &desc);

		[[nodiscard]] virtual RHICmdListRef CreateCmdList(const RHICmdListDesc & = RHICmdListDesc()) const override;

		[[nodiscard]] virtual RHIShaderRef CreateShader(const RHIShaderDesc &) const override;

		[[nodiscard]] virtual RHIGraphicPipelineRef CreateGraphicPipeline(const RHIGraphicPipelineDesc &, const RHIFrameBufferRef&) const override;

		[[nodiscard]] virtual RHIBindingLayoutRef CreateBindingLayout(const RHIBindingLayoutDesc &desc) const override;

		[[nodiscard]] virtual RHISwapchainRef CreateSwapChain(const RHISwapChainDesc &desc) const override;

		[[nodiscard]] virtual NativeObject GetVkInstance() const override;
	};
};