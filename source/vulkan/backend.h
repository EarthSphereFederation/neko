#pragma once
#include "neko.h"
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdexcept>
#include <iostream>
#pragma warning(disable:26812)
namespace neko
{
	namespace vk
	{
#define VK_CHECK_F(result,fmt,...) \
    if(result)\
	{\
		printf(fmt,__VA_ARGS__ );	\
		printf("\n"); \
		assert(false); \
	}\

#define VK_CHECK_RETURN_FALSE(result,fmt,...) \
    if(result)\
	{\
		printf(fmt,__VA_ARGS__ );	\
		printf("\n"); \
		return false;\
	}\
		
		constexpr uint32_t MAX_VULKAN_QUEUE_COUNT = 3;

		inline VkShaderStageFlagBits ConvertToVkShaderStageFlags(const RHIShaderStage& stage)
		{
			switch (stage)
			{
			case RHIShaderStage::VS:
			{
				return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
			}
			case RHIShaderStage::PS:
			{
				return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
			}
			case RHIShaderStage::All:
			{
				return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
			}
			default:
				CHECK(false);
				return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
			}
		}

		inline VkVertexInputRate ConvertToVkVertexInputRate(const RHIVertexRate& rate)
		{
			switch (rate)
			{
			case RHIVertexRate::Instance:
			{
				return VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE;
			}
			case RHIVertexRate::Vertex:
			{
				return VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
			}
			default:
				CHECK(false);
				return VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
			}
		}

		inline VkFormat ConvertToVkFormat(const RHIFormat& format)
		{
			switch (format)
			{
			case RHIFormat::B8G8R8A8_SNORM:
			{
				return VkFormat::VK_FORMAT_B8G8R8A8_SNORM;
			}
			default:
				CHECK(false);
				return VkFormat::VK_FORMAT_UNDEFINED;
			}
		}

		inline VkPrimitiveTopology ConvertToVkPrimitiveTopology(const RHIPrimitiveTopology& pt)
		{
			switch (pt)
			{
			case RHIPrimitiveTopology::TriangleList:
			{
				return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			}
			default:
				CHECK(false);
				return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			}
		}

		inline VkPolygonMode ConvertToVkPolygonMode(const RHIPolygonMode& pm)
		{
			switch (pm)
			{
			case RHIPolygonMode::Fill:
			{
				return VkPolygonMode::VK_POLYGON_MODE_FILL;
			}
			default:
				CHECK(false);
				return VkPolygonMode::VK_POLYGON_MODE_FILL;
			}
		}

		inline VkCullModeFlagBits ConvertToVkCullModeFlagBits(const RHICullMode& cm)
		{
			switch (cm)
			{
			case RHICullMode::None:
			{
				return VkCullModeFlagBits::VK_CULL_MODE_NONE;
			}
			case RHICullMode::Front:
			{
				return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
			}
			case RHICullMode::Back:
			{
				return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
			}
			default:
				CHECK(false);
				return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
			}
		}

		inline VkFrontFace ConvertVkFrontFace(const RHIFrontFace& ff)
		{
			switch (ff)
			{
			case RHIFrontFace::CW:
			{
				return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
			}
			case RHIFrontFace::CCW:
			{
				return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
			}
			default:
				CHECK(false);
				return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
			}
		}

		inline VkSampleCountFlagBits ConvertToVkSampleCountFlagBits(const RHISampleCount& sc)
		{
			switch (sc)
			{
			case RHISampleCount::SampleCount_1:
			{
				return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
			}
			case RHISampleCount::SampleCount_2:
			{
				return VkSampleCountFlagBits::VK_SAMPLE_COUNT_2_BIT;
			}
			case RHISampleCount::SampleCount_4:
			{
				return VkSampleCountFlagBits::VK_SAMPLE_COUNT_4_BIT;
			}
			case RHISampleCount::SampleCount_8:
			{
				return VkSampleCountFlagBits::VK_SAMPLE_COUNT_8_BIT;
			}
			case RHISampleCount::SampleCount_16:
			{
				return VkSampleCountFlagBits::VK_SAMPLE_COUNT_16_BIT;
			}
			default:
				CHECK(false);
				return VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
			}
		}

		inline VkStencilOp ConvertToVkStencilOp(const RHIStencilOp& op)
		{
			switch (op)
			{
			case RHIStencilOp::Keep:
			{
				return VkStencilOp::VK_STENCIL_OP_KEEP;
			}
			default:
				CHECK(false);
				return VkStencilOp::VK_STENCIL_OP_KEEP;
			}
		}

		inline VkCompareOp ConvertToVkCompareOp(const RHICompareOp& op)
		{
			switch (op)
			{
			case RHICompareOp::Never:
			{
				return VkCompareOp::VK_COMPARE_OP_NEVER;
			}
			case RHICompareOp::Less:
			{
				return VkCompareOp::VK_COMPARE_OP_LESS;
			}
			case RHICompareOp::Always:
			{
				return VkCompareOp::VK_COMPARE_OP_ALWAYS;
			}
			default:
				CHECK(false);
				return VkCompareOp::VK_COMPARE_OP_ALWAYS;
			}
		}

		inline VkBlendFactor ConvertToVkBlendFactor(const RHIBlendFactor& op)
		{
			switch (op)
			{
			case RHIBlendFactor::One:
			{
				return VkBlendFactor::VK_BLEND_FACTOR_ONE;
			}
			case RHIBlendFactor::Zero:
			{
				return VkBlendFactor::VK_BLEND_FACTOR_ZERO;
			}
			default:
				CHECK(false);
				return VkBlendFactor::VK_BLEND_FACTOR_ONE;
			}
		}

		inline VkBlendOp ConvertToVkBlendOp(const RHIBlendOp& op)
		{
			switch (op)
			{
			case RHIBlendOp::Add:
			{
				return VkBlendOp::VK_BLEND_OP_ADD;
			}
			default:
				CHECK(false);
				return VkBlendOp::VK_BLEND_OP_ADD;
			}
		}

		inline VkColorComponentFlags ConvertToVkColorComponentFlags(const RHIColorComponent& op)
		{
			VkColorComponentFlags ret = 0;
			if ((op & RHIColorComponent::R) != 0)
			{
				ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT;
			}
			if ((op & RHIColorComponent::G) != 0)
			{
				ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT;
			}
			if ((op & RHIColorComponent::B) != 0)
			{
				ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT;
			}
			if ((op & RHIColorComponent::A) != 0)
			{
				ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
			}
			if ((op & RHIColorComponent::All) != 0)
			{
				ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT;
				ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT;
				ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT;
				ret |= VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
			}
			return ret;
		}

		inline VkDescriptorType ConvertToVkDescriptorType(const RHIResourceType& rType)
		{
			switch (rType)
			{
			case RHIResourceType::UniformBuffer:
			{
				return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			}
			default:
				CHECK(false);
				return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			}
		}

		inline VkColorSpaceKHR ConvertToVkColorSpaceKHR(const RHIFormat& Format)
		{
			switch (Format)
			{
			case RHIFormat::B8G8R8A8_SNORM:
			{
				return VkColorSpaceKHR::VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT;
			}
			default:
				CHECK(false);
				return VkColorSpaceKHR::VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT;
			}
		}

		struct VulkanContext final : public RefCounter<RHIResource>
		{
			VkInstance Instance = nullptr;
			VkPhysicalDevice PhysicalDevice = nullptr;
			VkDevice Device = nullptr;

			VkAllocationCallbacks* AllocationCallbacks = nullptr;

			VkPhysicalDeviceProperties2 PhyDeviceProperties = {};
			VkPhysicalDeviceMemoryProperties2  PhyDeviceMemoryProperties = {};
			VkDeviceCreateInfo DeviceInfo = {};

			~VulkanContext();
		};
		typedef RefCountPtr<VulkanContext> VulkanContextPtr;

		class VulkanShader final : public RefCounter<RHIShader>
		{
		private:
			const VulkanContextPtr Context;
			VkShaderModule ShaderModule = nullptr;
			RHIShaderDesc Desc;
		public:
			VulkanShader(const VulkanContextPtr& ctx);
			~VulkanShader();
			bool Initalize(const RHIShaderDesc& desc);
			virtual const RHIShaderDesc& GetDesc() const override { return Desc; }
		public:
			const VkShaderModule GetVkShaderModule() const {return ShaderModule;}
		};

		class VulkanCmdBuffer
		{
		private:
			VkCommandPool CmdPool = nullptr;
			VkCommandBuffer CmdBuf = nullptr;
			const VulkanContextPtr context;
		public:
			VulkanCmdBuffer(const VulkanContextPtr& ctx) :context(ctx) {}
			~VulkanCmdBuffer();
			friend class VulkanQueue;
		};
		typedef std::shared_ptr<VulkanCmdBuffer> VulkanCmdBufferPtr;

		class VulkanQueue final : public RefCounter<RHIResource>
		{
		private:
			const VulkanContextPtr context;
			uint32_t FamilyIndex;
			RHICmdQueueType Type;
			VkQueueFamilyProperties2  properties;

			std::list<VulkanCmdBufferPtr> RunningCmdBuffers;
			std::list<VulkanCmdBufferPtr> CmdBufferPool;

		public:
			VulkanQueue(const VulkanContextPtr& ctx, uint32_t queueFamliyIndex, RHICmdQueueType cmdType, VkQueueFamilyProperties2  properties);
			~VulkanQueue();
			RHICmdQueueType GetCmdQueueType() const { return Type; }
			uint32_t GetFamilyIndex() const { return FamilyIndex; }
			VkQueueFamilyProperties2 GetFamilyProperties() const { return properties; }

			VulkanCmdBufferPtr GetOrCreateCmdBuffer();
			VulkanCmdBufferPtr CreateCmdBuffer();
		};
		typedef RefCountPtr<VulkanQueue> VulkanQueuePtr;

		class VulkanGraphicPipeline final : public RefCounter <RHIGraphicPipeline>
		{
		private:
			const VulkanContextPtr context;
			RHIGraphicPipelineDesc Desc;
		public:
			VulkanGraphicPipeline(const VulkanContextPtr&, const RHIGraphicPipelineDesc&);
			~VulkanGraphicPipeline();

			bool Initalize();
		};

		class VulkanCmdList final : public RefCounter<RHICmdList>
		{
			VulkanCmdBufferPtr CmdBuf;
			VulkanContextPtr Context;
		public:
			VulkanCmdList(const VulkanContextPtr&, const VulkanCmdBufferPtr&);
		};

		class VulkanBindingLayout final : public RefCounter <RHIBindingLayout>
		{
			VkDescriptorSetLayout DescriptorSetLayout = nullptr;
			VulkanContextPtr Context;
		public:
			VulkanBindingLayout(const VulkanContextPtr&);
			~VulkanBindingLayout();
			bool Initalize(const RHIBindingLayoutDesc& desc);
			virtual NativeObject GetNativeObject() const override{ return DescriptorSetLayout; }
		};
		
		class VulkanSwapchain final : public RefCounter<RHISwapchain>
		{
		private:
			VkSwapchainKHR Swapchain = nullptr;
			uint32_t ImageCount = 0;
			std::vector<VkImage> Images;
			std::vector<VkImageView> ImageViews;
			VulkanContextPtr Context;
		public:
			VulkanSwapchain(const VulkanContextPtr&);
			~VulkanSwapchain();
			bool Initalize(const RHISwapChainDesc& Desc);
		};
		

		class VulkanDevice final : public RefCounter<RHIDevice>
		{
		private:
			VulkanContextPtr Context;
			static_vector<VulkanQueuePtr, MAX_VULKAN_QUEUE_COUNT> Queues;
			
		public:
			VulkanDevice();
			~VulkanDevice() {}
			bool Initalize(const RHIDeviceDesc& desc);

			[[nodiscard]] virtual RHICmdListRef CreateCmdList(const RHICmdListDesc & = RHICmdListDesc()) const override;

			[[nodiscard]] virtual RHIShaderRef CreateShader(const RHIShaderDesc&) const override;
			
			[[nodiscard]] virtual RHIGraphicPipelineRef CreateGraphicPipeline(const RHIGraphicPipelineDesc&, const RHIFrameBuffer&) const override;

			[[nodiscard]] virtual RHIBindingLayoutRef CreateBindingLayout(const RHIBindingLayoutDesc& desc) const override;
			
			[[nodiscard]] virtual RHISwapchainRef CreateSwapChain(const RHISwapChainDesc& desc) const override;

			[[nodiscard]] virtual NativeObject GetVkInstance() const override;
		};
	}
};