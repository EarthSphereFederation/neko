#include "Backend.h"
namespace Neko::RHI::Vulkan
{ 
	FFrameBuffer::FFrameBuffer(const FContext& ctx, const RefCountPtr<FSwapchain>& Swapchain, uint32_t Index):Context(ctx)
	{
		Info.AddFormat(ConvertFromVkFormat(Swapchain->GetFormat()));
		Info.AddLoadAction(ELoadOp::Load);
		Info.AddStoreAction(EStoreOp::Store);
		auto ImageView = Swapchain->GetImageView(Index);
		AddImage(Swapchain->GetImage(Index));
		AddImageView(ImageView);
		Size = Swapchain->GetSize();

		static_vector<VkAttachmentDescription, 1> Attachments;
		static_vector<VkAttachmentReference, 1> AttachmentRefs;

		for (uint32_t i = 0; i < Info.FormatArray.size(); ++i)
		{
			VkAttachmentDescription Attachment = {};
			Attachment.format = ConvertToVkFormat(Info.FormatArray[i]);
			Attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			Attachment.loadOp = ConvertToVkAttachmentLoadOp(Info.LoadActionArray[i]);
			Attachment.storeOp = ConvertToVkVkAttachmentStoreOp(Info.StoreActionArray[i]);
			Attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			Attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			Attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			Attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			Attachments.push_back(Attachment);

			VkAttachmentReference Ref = {};
			Ref.attachment = i;
			Ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			AttachmentRefs.push_back(Ref);
		}

		VkSubpassDescription Subpass = {};
		Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		Subpass.colorAttachmentCount = (uint32_t)Attachments.size();
		Subpass.pColorAttachments = AttachmentRefs.data();

		VkSubpassDependency Dependency = {};
		Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		Dependency.dstSubpass = 0;
		Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependency.srcAccessMask = 0;
		Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo RenderPassInfo = {};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RenderPassInfo.attachmentCount = (uint32_t)Attachments.size();
		RenderPassInfo.pAttachments = Attachments.data();
		RenderPassInfo.subpassCount = 1;
		RenderPassInfo.pSubpasses = &Subpass;
		RenderPassInfo.dependencyCount = 1;
		RenderPassInfo.pDependencies = &Dependency;

		VK_CHECK_THROW(vkCreateRenderPass(Context.Device, &RenderPassInfo, Context.AllocationCallbacks, &RenderPass), "failed to create render pass");


		VkFramebufferCreateInfo FramebufferCreateInfo = {};
		FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FramebufferCreateInfo.renderPass = RenderPass;
		FramebufferCreateInfo.attachmentCount = (uint32_t)ImageViewArray.size();
		FramebufferCreateInfo.pAttachments = ImageViewArray.data();
		FramebufferCreateInfo.width = Size.width;
		FramebufferCreateInfo.height = Size.height;
		FramebufferCreateInfo.layers = 1;

		VK_CHECK_THROW(vkCreateFramebuffer(Context.Device, &FramebufferCreateInfo, nullptr, &FrameBuffer), "Failed to create framebuffer");
	}

	FFrameBuffer::~FFrameBuffer()
	{
		if (RenderPass)
		{
			vkDestroyRenderPass(Context.Device, RenderPass, Context.AllocationCallbacks);
			RenderPass = nullptr;
		}

		if (FrameBuffer)
		{
			vkDestroyFramebuffer(Context.Device, FrameBuffer, Context.AllocationCallbacks);
			FrameBuffer = nullptr;
		}
	}
	
	bool FFrameBuffer::Initalize()
	{
		

		return true;
	}
}