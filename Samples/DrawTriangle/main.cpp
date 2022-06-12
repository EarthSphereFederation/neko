#include "RHI/RHI.h"
#include "OS/Window.h"
#include "HLSLCompiler/Compiler.h"
#include "HLSLCompiler/SystemUtils.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <vector>
#include <filesystem>
#include <stdio.h>
#include <glm/glm.hpp>

using namespace Neko;

struct FVertex {
    glm::vec2 pos;
    glm::vec3 color;
};

int main(int, char **)
{
    RHI::RHIInit();

    uint32_t SurfaceExtensionCount;
    const char **SurfaceExtensionNames = OS::FWindow::GetRequiredVulkanInstanceExtensions(&SurfaceExtensionCount);

    auto VkDesc = RHI::FDeviceDesc::FVulkanDesc()
                      .SetInstanceExtensions(SurfaceExtensionNames, SurfaceExtensionCount);

    auto Features = RHI::FFeatures().SetSwapchain(true);

    RHI::FDeviceDesc DevDesc;
    DevDesc.SetVulkanDesc(VkDesc)
        .SetValidation(true)
        .SetFeatures(Features);

    auto Device = CreateDevice(DevDesc);
    auto GPUInfo = Device->GetGPUInfo();

    printf("GPU : %s is used\n", GPUInfo.Name);

    uint32_t WindowsWidth = 512, WindowsHeight = 512;
    auto Window = OS::FWindowBuilder()
                      .SetSize(WindowsWidth, WindowsHeight)
                      .SetTitle("neko_drawtriangle")
                      .CreateWindow();
    auto SwapchainDesc = RHI::FSwapChainDesc()
        .SetFormat(RHI::EFormat::B8G8R8A8_SNORM)
        .SetVSync(true).SetWindow(&Window);

    auto Swapchain = Device->CreateSwapChain(SwapchainDesc);
    auto SwapchainTextures = Swapchain->GetTextures();

    Window.Attach([&](const OS::FWindowResizeEvent& Event) 
    {
            auto SwapchainDesc = RHI::FSwapChainDesc()
                .SetFormat(RHI::EFormat::B8G8R8A8_SNORM)
                .SetVSync(true).SetWindow(&Window);
            Device->WaitIdle();
            Swapchain->Reset();
            Swapchain = Device->CreateSwapChain(SwapchainDesc);
            SwapchainTextures = Swapchain->GetTextures();

            WindowsWidth = Event.Width;
            WindowsHeight = Event.Height;
    });
   
    auto TextureCount = Swapchain->GetTextureNum();

#if NEKO_SHADER_DEV
    std::string AssetPath = std::filesystem::exists(ASSETS_PATH) ? ASSETS_PATH : GetExecutableDir();
#else
    std::string AssetPath = GetExecutableDir();
#endif

    ShaderDesc VertexShaderDesc = {
            AssetPath + "/shaders/DrawTriangle.hlsl",
            "mainVS",
            EShaderType::kVertex,
            EShaderFeatureLevel::k6_5};
    ShaderDesc PixelShaderDesc = {
            AssetPath + "/shaders/DrawTriangle.hlsl",
            "mainPS",
            EShaderType::kPixel,
            EShaderFeatureLevel::k6_5};

    auto VertexShaderCode = Compile(VertexShaderDesc, EShaderBlobType::kSPIRV);
    auto VSDesc = RHI::FShaderDesc()
        .SetBlob((char *)VertexShaderCode.data())
        .SetSize(VertexShaderCode.size())
        .SetEntryPoint("mainVS")
        .SetStage(RHI::EShaderStage::Vertex);
    auto VS = Device->CreateShader(VSDesc);

    auto PixelShaderCode = Compile(PixelShaderDesc, EShaderBlobType::kSPIRV);;
    auto PSDesc = RHI::FShaderDesc()
        .SetBlob((char *)PixelShaderCode.data())
        .SetSize(PixelShaderCode.size())
        .SetEntryPoint("mainPS")
        .SetStage(RHI::EShaderStage::Pixel);
    auto PS = Device->CreateShader(PSDesc);

    auto RasterState = RHI::FRasterSate().SetCullMode(RHI::ECullMode::None);

    auto SwapchainColorAttachmentDesc = RHI::FColorAttachmentDesc().SetTexture(SwapchainTextures[0]).SetFormat(SwapchainTextures[0]->GetDesc().Format);;
    
    auto GraphicQueue = Device->CreateQueue();

    auto CmdPools = GraphicQueue->CreateCmdPools(TextureCount);

    auto SubmissionFences = Device->CreateFences(RHI::EFenceFlag::Signal, TextureCount);
    auto AcquireSamephores = Device->CreateSemaphores(RHI::ESemaphoreType::Binary, TextureCount);
    auto ExcuteSamephores = Device->CreateSemaphores(RHI::ESemaphoreType::Binary, TextureCount);


    FVertex V0;
    V0.pos.x = 0.0f;
    V0.pos.y = -0.5f;
    V0.color.r = 1.0f;
    V0.color.g = 0.0f;
    V0.color.b = 0.0f;
    FVertex V1;
    V1.pos.x = 0.5f;
    V1.pos.y = 0.5f;
    V1.color.r = 0.0f;
    V1.color.g = 1.0f;
    V1.color.b = 0.0f;
    FVertex V2;
    V2.pos.x = -0.5f;
    V2.pos.y = 0.5f;
    V2.color.r = 0.0f;
    V2.color.g = 0.0f;
    V2.color.b = 1.0f;

    std::vector<FVertex> Vertices;
    Vertices.push_back(V0);
    Vertices.push_back(V1);
    Vertices.push_back(V2);

    std::vector<uint16_t> Indices = { 0,1,2 };

    auto VertexBufferDesc = RHI::FBufferDesc().SetSize(sizeof(FVertex) * Vertices.size()).SetBufferUsage(RHI::EBufferUsage::VertexBuffer | RHI::EBufferUsage::CPUAccess);
    auto VertexBuffer = Device->CreateBuffer(VertexBufferDesc);

    auto IndexBufferDesc = RHI::FBufferDesc().SetSize(sizeof(uint16_t) * Indices.size()).SetBufferUsage(RHI::EBufferUsage::IndexBuffer | RHI::EBufferUsage::CPUAccess);
    auto IndexBuffer = Device->CreateBuffer(IndexBufferDesc);

    auto VertexInputLayout = RHI::FVertexInputLayout().AddBinding({ 0,sizeof(FVertex),RHI::EVertexRate::Vertex })
        .AddAttribute({ "Position",RHI::EFormat::R32G32_SFLOAT,0,0,0})
        .AddAttribute({ "VertexColor",RHI::EFormat::R32G32B32_SFLOAT,0,1, sizeof(glm::vec2)});

   /* auto BindingLayoutDesc = RHI::FBindingLayoutDesc().AddBinding({ "MVPMatrix", 0 ,RHI::EResourceType::UniformBuffer }).SetShaderStage(RHI::EShaderStage::Vertex);
    auto BindingLayout = Device->CreateBindingLayout(BindingLayoutDesc);*/

    auto GraphicPipelineDesc = RHI::FGraphicPipelineDesc()
        .SetVertexShader(VS)
        .SetPixelShader(PS)
        .SetRasterState(RasterState)
        .AddColorAttachmentDesc(SwapchainColorAttachmentDesc)
        .SetVertexInputLayout(VertexInputLayout);
        //.AddBindingLayout(BindingLayout);

    auto GraphicPipeline = Device->CreateGraphicPipeline(GraphicPipelineDesc);

    // mainloop
      
    uint32_t FrameNumber = 0;
    while (!Window.ShouldClose())
    {
        OS::FWindow::DoEvents();
        if (Window.GetInput().IsKeyDown(OS::EKeyCode::Escape))
        {
            Window.SetCloseFlag(true);
        }

        uint32_t SwapchainTextureIndex = FrameNumber % TextureCount;

        auto SwapchainColorAttachmentDesc = RHI::FColorAttachmentDesc()
            .SetTexture(SwapchainTextures[SwapchainTextureIndex])
            .SetFormat(SwapchainTextures[SwapchainTextureIndex]->GetDesc().Format);
        auto SwapchainColorAttachment = Device->CreateColorAttachment(SwapchainColorAttachmentDesc);

        SubmissionFences[SwapchainTextureIndex]->Wait();
        SubmissionFences[SwapchainTextureIndex]->Reset();

        auto ImageIdex = Swapchain->AcquireNext(AcquireSamephores[SwapchainTextureIndex],nullptr);

        CmdPools[SwapchainTextureIndex]->Free();

        auto CmdList = CmdPools[SwapchainTextureIndex]->CreateCmdList();
       
        CmdList->BeginCmd();

        auto VertexBufferPtr = Device->MapBuffer(VertexBuffer, 0, sizeof(FVertex) * Vertices.size());
        std::memcpy(VertexBufferPtr, Vertices.data(), sizeof(FVertex) * Vertices.size());
        Device->UnmapBuffer(VertexBuffer);

        auto IndexBufferPtr = Device->MapBuffer(IndexBuffer, 0, sizeof(uint16_t) * Indices.size());
        std::memcpy(IndexBufferPtr, Indices.data(), sizeof(uint16_t) * Indices.size());
        Device->UnmapBuffer(IndexBuffer);
  
        auto Barrier_U2R = RHI::FTextureTransitionDesc()
            .SetTexture(SwapchainTextures[SwapchainTextureIndex])
            .SetSrcState(RHI::EResourceState::Undefined)
            .SetDestState(RHI::EResourceState::RenderTarget);
        CmdList->ResourceBarrier(Barrier_U2R);
        
        auto RenderPassDesc = RHI::FRenderPassDesc().AddColorAttachment(SwapchainColorAttachment);
        CmdList->BeginRenderPass(RenderPassDesc);
        CmdList->BindGraphicPipeline(GraphicPipeline);
        CmdList->SetViewportNoScissor(0, WindowsWidth,0, WindowsHeight);

        CmdList->BindVertexBuffer(VertexBuffer, 0, 0);
        CmdList->BindIndexBuffer(IndexBuffer, 0, RHI::EIndexBufferType::BIT16);
        CmdList->DrawIndexed(Indices.size(),0,0);
        CmdList->EndRenderPass();
        
        auto Barrier_R2P = RHI::FTextureTransitionDesc()
            .SetTexture(SwapchainTextures[SwapchainTextureIndex])
            .SetSrcState(RHI::EResourceState::RenderTarget)
            .SetDestState(RHI::EResourceState::Present);
        CmdList->ResourceBarrier(Barrier_R2P);
        
        CmdList->EndCmd();
        
        auto ExcuteDesc = RHI::FExcuteDesc().AddSignalSemaphore(ExcuteSamephores[SwapchainTextureIndex]).SetFence(SubmissionFences[SwapchainTextureIndex]);
        GraphicQueue->ExcuteCmdList(CmdList, ExcuteDesc);

        auto PresentDesc = RHI::FPresentDesc()
            .AddWaitSemaphore(ExcuteSamephores[SwapchainTextureIndex])
            .SetQueue(GraphicQueue)
            .SetPresentIndex(ImageIdex);
        Swapchain->Present(PresentDesc);

        
        FrameNumber++;
    }
    Device->WaitIdle();

    return 0;
}
