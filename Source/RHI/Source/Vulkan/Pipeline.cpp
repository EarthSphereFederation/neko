#include "Backend.h"

namespace Neko::RHI::Vulkan
{ 
	FGraphicPipeline::FGraphicPipeline(const FContext &ctx, const FGraphicPipelineDesc &Desc) : Context(ctx), Desc(Desc)
	{
	}

	FGraphicPipeline::~FGraphicPipeline()
	{
		if (PipelineLayout)
		{
			vkDestroyPipelineLayout(Context.Device, PipelineLayout, Context.AllocationCallbacks);
			PipelineLayout = nullptr;
		}

		if (Pipeline)
		{
			vkDestroyPipeline(Context.Device, Pipeline, Context.AllocationCallbacks);
			Pipeline = nullptr;
		}
	}

	bool FGraphicPipeline::Initalize()
	{

		uint32_t ColorAttachmentDescCount = (uint32_t)Desc.ColorAttachmentDescArray.size();
		static_vector<VkPipelineShaderStageCreateInfo, MAX_SHADER_STAGE_COUNT> ShaderStages;

		static_vector<IShaderRef, MAX_SHADER_STAGE_COUNT> Shaders;
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
		for (uint32_t i = 0; i < Desc.VertexInputLayout.AttributeArray.size(); ++i)
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

		static_vector<VkPipelineColorBlendAttachmentState, MAX_COLOR_ATTACHMENT_COUNT> ColorBlendAttachmentStates;

		for (uint32_t i = 0; i < ColorAttachmentDescCount; ++i)
		{
			auto& ColorAttachmentBlendState = Desc.ColorAttachmentDescArray[i].BlendState;
			VkPipelineColorBlendAttachmentState ColorBlendAttachment = {};
			ColorBlendAttachment.blendEnable = ColorAttachmentBlendState.BlendEnable;
			ColorBlendAttachment.colorBlendOp = ConvertToVkBlendOp(ColorAttachmentBlendState.ColorOp);
			ColorBlendAttachment.srcColorBlendFactor = ConvertToVkBlendFactor(ColorAttachmentBlendState.SrcColor);
			ColorBlendAttachment.dstColorBlendFactor = ConvertToVkBlendFactor(ColorAttachmentBlendState.DestColor);
			ColorBlendAttachment.alphaBlendOp = ConvertToVkBlendOp(ColorAttachmentBlendState.AlphaOp);
			ColorBlendAttachment.srcAlphaBlendFactor = ConvertToVkBlendFactor(ColorAttachmentBlendState.SrcAlpha);
			ColorBlendAttachment.dstAlphaBlendFactor = ConvertToVkBlendFactor(ColorAttachmentBlendState.DestAlpha);
			ColorBlendAttachment.colorWriteMask = ConvertToVkColorComponentFlags(ColorAttachmentBlendState.WriteMask);
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
			auto BindingLayout = CAST(FBindingLayout,Desc.BindingLayoutArray[i]);
			DescriptorSetLayouts.push_back(BindingLayout->GetDescriptorSetLayout());
		}

		VkPipelineLayoutCreateInfo PipelineLayoutInfo = {};
		PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		PipelineLayoutInfo.setLayoutCount = (uint32_t)Desc.BindingLayoutArray.size();
		PipelineLayoutInfo.pSetLayouts = Desc.BindingLayoutArray.size() > 0 ? DescriptorSetLayouts.data() : nullptr; // Optional
		PipelineLayoutInfo.pushConstantRangeCount = 0;
		PipelineLayoutInfo.pPushConstantRanges = nullptr;
		
		vkCreatePipelineLayout(Context.Device, &PipelineLayoutInfo, Context.AllocationCallbacks, &PipelineLayout);

		

		VkDynamicState DynamicStates[4] = {
			VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT,
			VkDynamicState::VK_DYNAMIC_STATE_SCISSOR,
		};

		VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo = {};
		DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		DynamicStateCreateInfo.dynamicStateCount = 2;
		DynamicStateCreateInfo.pDynamicStates = DynamicStates;

		static_vector<VkFormat, MAX_COLOR_ATTACHMENT_COUNT>ColorAttachmentFormats;
		for (uint32_t i = 0; i < ColorAttachmentDescCount; ++i)
		{
			ColorAttachmentFormats.push_back(ConvertToVkFormat(Desc.ColorAttachmentDescArray[i].Format));
		}

		VkPipelineRenderingCreateInfoKHR PipelineRenderingCreateInfo = {};
		PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
		PipelineRenderingCreateInfo.colorAttachmentCount = ColorAttachmentDescCount;
		PipelineRenderingCreateInfo.pColorAttachmentFormats = ColorAttachmentFormats.data();
		

		VkGraphicsPipelineCreateInfo PipelineInfo = {};
		PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		PipelineInfo.pNext = &PipelineRenderingCreateInfo;
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
		PipelineInfo.renderPass = nullptr; // we use dynamic rendering
		PipelineInfo.subpass = 0;
		PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		PipelineInfo.basePipelineIndex = -1; 

		VK_CHECK_THROW(vkCreateGraphicsPipelines(Context.Device, VK_NULL_HANDLE, 1, &PipelineInfo, Context.AllocationCallbacks, &Pipeline), "failed to create graphics pipeline");

		return true;
	}

	IGraphicPipelineRef FDevice::CreateGraphicPipeline(const FGraphicPipelineDesc &pipelineDesc)
	{
		auto Pipeline = RefCountPtr<FGraphicPipeline>(new FGraphicPipeline(Context, pipelineDesc));
		if (!Pipeline->Initalize())
		{
			Pipeline = nullptr;
		}
		return Pipeline;
	}
}
