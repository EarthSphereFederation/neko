#include "backend.h"

namespace Neko::Vulkan
{ 
	FGraphicPipeline::FGraphicPipeline(const VulkanContextPtr &ctx, const RHIGraphicPipelineDesc &Desc) : Context(ctx), Desc(Desc)
	{
	}

	FGraphicPipeline::~FGraphicPipeline()
	{
		if (RenderPass)
		{
			vkDestroyRenderPass(Context->Device, RenderPass, Context->AllocationCallbacks);
			RenderPass = nullptr;
		}

		if (PipelineLayout)
		{
			vkDestroyPipelineLayout(Context->Device, PipelineLayout, Context->AllocationCallbacks);
			PipelineLayout = nullptr;
		}

		if (Pipeline)
		{
			vkDestroyPipeline(Context->Device, Pipeline, Context->AllocationCallbacks);
			Pipeline = nullptr;
		}
	}

	bool FGraphicPipeline::Initalize(const RHIFrameBufferInfo& FrameBufferInfo)
	{

		uint32_t RTCount = (uint32_t)FrameBufferInfo.FormatArray.size();
		static_vector<VkPipelineShaderStageCreateInfo, MAX_SHADER_STAGE_COUNT> ShaderStages;

		static_vector<RHIShaderRef, MAX_SHADER_STAGE_COUNT> Shaders;
		Shaders.push_back(Desc.VertexShader);
		Shaders.push_back(Desc.PixelShader);

		for (auto &shader : Shaders)
		{
			if (shader.IsValid())
			{
				VkPipelineShaderStageCreateInfo ShaderStageInfo = {};
				ShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				ShaderStageInfo.stage = ConvertToVkShaderStageFlags(shader->GetDesc().Stage);
				ShaderStageInfo.module = reinterpret_cast<FShader *>(shader.GetPtr())->GetVkShaderModule();
				ShaderStageInfo.pName = shader->GetDesc().EntryPoint;
				ShaderStages.push_back(ShaderStageInfo);
			}
		}

		static_vector<VkVertexInputBindingDescription, MAX_VERTEX_BINDING_COUNT> VertexBings;
		for (auto &vb : Desc.VertexInputLayout.BindingArray)
		{
			VkVertexInputBindingDescription bindingDesc = {};
			bindingDesc.binding = vb.Binding;
			bindingDesc.stride = vb.Stride;
			bindingDesc.inputRate = ConvertToVkVertexInputRate(vb.VertexRate);
			VertexBings.push_back(bindingDesc);
		}

		static_vector<VkVertexInputAttributeDescription, MAX_VERTEX_ATTRIBUTE_COUNT> VertexAttributes;
		for (uint32_t i = 0; i < Desc.VertexInputLayout.AttributeCount; ++i)
		{
			const auto &va = Desc.VertexInputLayout.AttributeArray[i];

			VkVertexInputAttributeDescription attributeDesc = {};
			attributeDesc.binding = va.Binding;
			attributeDesc.format = ConvertToVkFormat(va.Format);
			attributeDesc.location = va.Location;
			attributeDesc.offset = va.Offset;
			VertexAttributes.push_back(attributeDesc);
		}

		VkPipelineVertexInputStateCreateInfo VertexInputInfo = {};
		VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		VertexInputInfo.vertexBindingDescriptionCount = (uint32_t)VertexBings.size();
		VertexInputInfo.pVertexBindingDescriptions = VertexBings.data();
		VertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)VertexAttributes.size();
		VertexInputInfo.pVertexAttributeDescriptions = VertexAttributes.data();

		VkPipelineInputAssemblyStateCreateInfo InputAssembly{};
		InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssembly.topology = ConvertToVkPrimitiveTopology(Desc.PrimitiveTopology);
		InputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo ViewportState = {};
		ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		ViewportState.viewportCount = 1;
		ViewportState.pViewports = nullptr;
		ViewportState.scissorCount = 1;
		ViewportState.pScissors = nullptr;

		VkPipelineRasterizationStateCreateInfo Rasterizer = {};
		Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		Rasterizer.depthClampEnable = VK_FALSE;
		Rasterizer.rasterizerDiscardEnable = VK_FALSE;
		Rasterizer.polygonMode = ConvertToVkPolygonMode(Desc.RasterState.PolygonMode);
		Rasterizer.lineWidth = 1.0f;
		Rasterizer.cullMode = ConvertToVkCullModeFlagBits(Desc.RasterState.CullMode);
		Rasterizer.frontFace = ConvertVkFrontFace(Desc.RasterState.FrontFace);
		Rasterizer.depthBiasEnable = VK_FALSE;
		Rasterizer.depthBiasConstantFactor = 0.0f;
		Rasterizer.depthBiasClamp = 0.0f;
		Rasterizer.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo Multisampling = {};
		Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		Multisampling.sampleShadingEnable = VK_FALSE;
		Multisampling.rasterizationSamples = ConvertToVkSampleCountFlagBits(Desc.SampleCount);
		Multisampling.minSampleShading = 1.0f;
		Multisampling.pSampleMask = nullptr;
		Multisampling.alphaToCoverageEnable = VK_FALSE;
		Multisampling.alphaToOneEnable = VK_FALSE;

		VkStencilOpState FrontStencilOpSate = {};
		FrontStencilOpSate.writeMask = Desc.DepthStencilState.WriteMask;
		FrontStencilOpSate.compareMask = Desc.DepthStencilState.ReadMask;
		FrontStencilOpSate.compareOp = ConvertToVkCompareOp(Desc.DepthStencilState.FrontStencil.StencilCompareOp);
		FrontStencilOpSate.depthFailOp = ConvertToVkStencilOp(Desc.DepthStencilState.FrontStencil.DepthFailOp);
		FrontStencilOpSate.passOp = ConvertToVkStencilOp(Desc.DepthStencilState.FrontStencil.PassOp);
		FrontStencilOpSate.failOp = ConvertToVkStencilOp(Desc.DepthStencilState.FrontStencil.FailOp);
		FrontStencilOpSate.reference = Desc.DepthStencilState.StencilRef;

		VkStencilOpState BackStencilOpSate = {};
		BackStencilOpSate.writeMask = Desc.DepthStencilState.WriteMask;
		BackStencilOpSate.compareMask = Desc.DepthStencilState.ReadMask;
		BackStencilOpSate.compareOp = ConvertToVkCompareOp(Desc.DepthStencilState.BackStencil.StencilCompareOp);
		BackStencilOpSate.depthFailOp = ConvertToVkStencilOp(Desc.DepthStencilState.BackStencil.DepthFailOp);
		BackStencilOpSate.passOp = ConvertToVkStencilOp(Desc.DepthStencilState.BackStencil.PassOp);
		BackStencilOpSate.failOp = ConvertToVkStencilOp(Desc.DepthStencilState.BackStencil.FailOp);
		BackStencilOpSate.reference = Desc.DepthStencilState.StencilRef;

		VkPipelineDepthStencilStateCreateInfo DepthStencilState = {};
		DepthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		DepthStencilState.pNext = nullptr;
		DepthStencilState.depthTestEnable = Desc.DepthStencilState.DepthTest;
		DepthStencilState.depthWriteEnable = Desc.DepthStencilState.DepthWrite;
		DepthStencilState.depthCompareOp = ConvertToVkCompareOp(Desc.DepthStencilState.DepthCompareOp);
		DepthStencilState.flags = 0;
		DepthStencilState.depthBoundsTestEnable = VK_FALSE;
		DepthStencilState.minDepthBounds = 0.0f;
		DepthStencilState.minDepthBounds = 1.0f;
		DepthStencilState.stencilTestEnable = Desc.DepthStencilState.StencilTest;
		DepthStencilState.front = FrontStencilOpSate;
		DepthStencilState.back = BackStencilOpSate;

		static_vector<VkPipelineColorBlendAttachmentState, MAX_RENDER_TARGET_COUNT> ColorBlendAttachmentStates;

		for (uint32_t i = 0; i < RTCount; ++i)
		{
			auto rt = Desc.BlendState.renderTargets[i];
			VkPipelineColorBlendAttachmentState ColorBlendAttachment = {};
			ColorBlendAttachment.blendEnable = rt.BlendEnable;
			ColorBlendAttachment.colorBlendOp = ConvertToVkBlendOp(rt.ColorOp);
			ColorBlendAttachment.srcColorBlendFactor = ConvertToVkBlendFactor(rt.SrcColor);
			ColorBlendAttachment.dstColorBlendFactor = ConvertToVkBlendFactor(rt.DestColor);
			ColorBlendAttachment.alphaBlendOp = ConvertToVkBlendOp(rt.AlphaOp);
			ColorBlendAttachment.srcAlphaBlendFactor = ConvertToVkBlendFactor(rt.SrcAlpha);
			ColorBlendAttachment.dstAlphaBlendFactor = ConvertToVkBlendFactor(rt.DestAlpha);
			ColorBlendAttachment.colorWriteMask = ConvertToVkColorComponentFlags(rt.WriteMask);
			ColorBlendAttachmentStates.push_back(ColorBlendAttachment);
		}

		VkPipelineColorBlendStateCreateInfo ColorBlending = {};
		ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		ColorBlending.logicOpEnable = VK_FALSE;
		ColorBlending.logicOp = VK_LOGIC_OP_COPY;
		ColorBlending.attachmentCount = (uint32_t)ColorBlendAttachmentStates.size();
		ColorBlending.pAttachments = ColorBlendAttachmentStates.data();
		ColorBlending.blendConstants[0] = 0.0f;
		ColorBlending.blendConstants[1] = 0.0f;
		ColorBlending.blendConstants[2] = 0.0f;
		ColorBlending.blendConstants[3] = 0.0f;

		static_vector<VkDescriptorSetLayout, MAX_BINDING_LAYOUT_COUNT> DescriptorSetLayouts;

		for (int i = 0; i < Desc.BindingLayoutArray.size(); ++i)
		{
			DescriptorSetLayouts.push_back(Desc.BindingLayoutArray[i]->GetNativeObject());
		}

		VkPipelineLayoutCreateInfo PipelineLayoutInfo = {};
		PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		PipelineLayoutInfo.setLayoutCount = (uint32_t)Desc.BindingLayoutArray.size();
		PipelineLayoutInfo.pSetLayouts = Desc.BindingLayoutArray.size() > 0 ? DescriptorSetLayouts.data() : nullptr; // Optional
		PipelineLayoutInfo.pushConstantRangeCount = 0;
		PipelineLayoutInfo.pPushConstantRanges = nullptr;
		
		vkCreatePipelineLayout(Context->Device, &PipelineLayoutInfo, Context->AllocationCallbacks, &PipelineLayout);

		static_vector<VkAttachmentDescription, MAX_RENDER_TARGET_COUNT> ColorAttachments;
		static_vector<VkAttachmentReference, MAX_RENDER_TARGET_COUNT> ColorAttachmentRefs;

		for (uint32_t i = 0; i < FrameBufferInfo.FormatArray.size(); ++i)
		{
			VkAttachmentDescription Attachment = {};
			Attachment.format = ConvertToVkFormat(FrameBufferInfo.FormatArray[i]);
			Attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			Attachment.loadOp = ConvertToVkAttachmentLoadOp(FrameBufferInfo.LoadActionArray[i]);
			Attachment.storeOp = ConvertToVkVkAttachmentStoreOp(FrameBufferInfo.StoreActionArray[i]);
			Attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			Attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			Attachment.initialLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
			Attachment.finalLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
			ColorAttachments.push_back(Attachment);

			VkAttachmentReference Ref = {};
			Ref.attachment = i;
			Ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			ColorAttachmentRefs.push_back(Ref);
		}

		VkSubpassDescription Subpass = {};
		Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		Subpass.colorAttachmentCount = (uint32_t)ColorAttachmentRefs.size();
		Subpass.pColorAttachments = ColorAttachmentRefs.data();

		VkSubpassDependency Dependency = {};
		Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		Dependency.dstSubpass = 0;
		Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependency.srcAccessMask = 0;
		Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = (uint32_t)ColorAttachments.size();
		renderPassInfo.pAttachments = ColorAttachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &Subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &Dependency;

		VK_CHECK_RETURN_FALSE(vkCreateRenderPass(Context->Device, &renderPassInfo, Context->AllocationCallbacks, &RenderPass), "failed to create render pass");

		VkDynamicState DynamicStates[4] = {
			VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT,
			VkDynamicState::VK_DYNAMIC_STATE_SCISSOR,
		};

		VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo = {};
		DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		DynamicStateCreateInfo.dynamicStateCount = 2;
		DynamicStateCreateInfo.pDynamicStates = DynamicStates;

		VkGraphicsPipelineCreateInfo PipelineInfo = {};
		PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		PipelineInfo.stageCount = (uint32_t)ShaderStages.size();
		PipelineInfo.pStages = ShaderStages.data();
		PipelineInfo.pVertexInputState = &VertexInputInfo;
		PipelineInfo.pInputAssemblyState = &InputAssembly;
		PipelineInfo.pViewportState = &ViewportState;
		PipelineInfo.pRasterizationState = &Rasterizer;
		PipelineInfo.pMultisampleState = &Multisampling;
		PipelineInfo.pDepthStencilState = &DepthStencilState;
		PipelineInfo.pColorBlendState = &ColorBlending;
		PipelineInfo.pDynamicState = &DynamicStateCreateInfo; 
		PipelineInfo.layout = PipelineLayout;
		PipelineInfo.renderPass = RenderPass;
		PipelineInfo.subpass = 0;
		PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		PipelineInfo.basePipelineIndex = -1; 

		VK_CHECK_RETURN_FALSE(vkCreateGraphicsPipelines(Context->Device, VK_NULL_HANDLE, 1, &PipelineInfo, Context->AllocationCallbacks, &Pipeline), "failed to create graphics pipeline");

		return true;
	}

	RHIGraphicPipelineRef FDevice::CreateGraphicPipeline(const RHIGraphicPipelineDesc &pipelineDesc, const RHIFrameBufferRef & FrameBuffer) const
	{
		auto Pipeline = RefCountPtr<FGraphicPipeline>(new FGraphicPipeline(Context, pipelineDesc));
		if (!Pipeline->Initalize(FrameBuffer->GetInfo()))
		{
			Pipeline = nullptr;
		}
		return Pipeline;
	}
}
