#include "RHI/RHI.h"
#include "OS/Window.h"
#include "HLSLCompiler/Compiler.h"
#include "HLSLCompiler/SystemUtils.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <vector>
#include <filesystem>
#include <stdio.h>

using namespace Neko;

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

    auto Window = OS::FWindowBuilder()
                      .SetSize(512, 512)
                      .SetTitle("neko_drawtriangle")
                      .CreateWindow();
    auto SwapchainDesc = RHI::FSwapChainDesc()
        .SetFormat(RHI::EFormat::B8G8R8A8_SNORM)
        .SetVSync(true).SetWindow(&Window);

    auto Swapchain = Device->CreateSwapChain(SwapchainDesc);
    auto SwapchainTextures = Swapchain->GetTextures();

    Window.Attach([&](const OS::FWindowResizeEvent&) 
    {
            auto SwapchainDesc = RHI::FSwapChainDesc()
                .SetFormat(RHI::EFormat::B8G8R8A8_SNORM)
                .SetVSync(true).SetWindow(&Window);
            Device->WaitIdle();
            Swapchain->Reset();
            Swapchain = Device->CreateSwapChain(SwapchainDesc);
            SwapchainTextures = Swapchain->GetTextures();
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

    auto SwapchainRenderTargetDesc = RHI::FRenderTargetDesc().SetTexture(SwapchainTextures[0]).SetFormat(SwapchainTextures[0]->GetDesc().Format);;
    
    auto GraphicPipelineDesc = RHI::FGraphicPipelineDesc()
        .SetVertexShader(VS)
        .SetPixelShader(PS)
        .SetRasterState(RasterState)
        .AddColorRenderTargetDesc(SwapchainRenderTargetDesc);

    auto GraphicPipeline = Device->CreateGraphicPipeline(GraphicPipelineDesc);

    auto GraphicQueue = Device->CreateQueue();

    auto CmdPools = GraphicQueue->CreateCmdPools(TextureCount);

    auto SubmissionFences = Device->CreateFences(RHI::EFenceFlag::Signal, TextureCount);
    auto AcquireSamephores = Device->CreateSemaphores(RHI::ESemaphoreType::Binary, TextureCount);
    auto ExcuteSamephores = Device->CreateSemaphores(RHI::ESemaphoreType::Binary, TextureCount);

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

        auto SwapchainRenderTargetDesc = RHI::FRenderTargetDesc()
            .SetTexture(SwapchainTextures[SwapchainTextureIndex])
            .SetFormat(SwapchainTextures[SwapchainTextureIndex]->GetDesc().Format);
        auto SwapchainRenderTarget = Device->CreateRenderTarget(SwapchainRenderTargetDesc);

        SubmissionFences[SwapchainTextureIndex]->Wait();
        SubmissionFences[SwapchainTextureIndex]->Reset();

        auto ImageIdex = Swapchain->AcquireNext(AcquireSamephores[SwapchainTextureIndex],nullptr);

        CmdPools[SwapchainTextureIndex]->Free();
        auto CmdList = CmdPools[SwapchainTextureIndex]->CreateCmdList();
       
       
        CmdList->BeginCmd();
        
        auto Barrier_U2R = RHI::FTextureTransitionDesc()
            .SetTexture(SwapchainTextures[SwapchainTextureIndex])
            .SetSrcState(RHI::EResourceState::Undefined)
            .SetDestState(RHI::EResourceState::RenderTarget);
        CmdList->ResourceBarrier(Barrier_U2R);
        
        auto RenderPassDesc = RHI::FRenderPassDesc().AddColorRenderTarget(SwapchainRenderTarget);
        CmdList->BeginRenderPass(RenderPassDesc);
        CmdList->BindGraphicPipeline(GraphicPipeline);
        CmdList->SetViewportNoScissor(0,512,0,512);
        CmdList->Draw(3, 0, 1, 0);
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
