#include "OS/Window.h"
#include "RHI/RHI.h"
#include "HLSLCompiler/Compiler.h"
#include "HLSLCompiler/SystemUtils.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <vector>
#include <filesystem>
#include <stdio.h>

using namespace Neko;

std::vector<char> ReadBinaryFile(const std::string &path)
{
    std::vector<char> buffer;
    std::ifstream fileStream(path, std::ios::binary | std::ios::ate);
    if (!fileStream.is_open())
    {
        throw std::runtime_error("failed to open file");
    }

    uint32_t fileSize = fileStream.tellg();
    buffer.resize(fileSize);
    fileStream.seekg(0);
    fileStream.read(buffer.data(), fileSize);
    return buffer;
}

int main(int, char **)
{
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

    auto FrameBufferForPipeline = Swapchain->GetFrameBuffer(0);

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

    auto GraphicPipelineDesc = RHI::FGraphicPipelineDesc()
        .SetVertexShader(VS)
        .SetPixelShader(PS)
        .SetRasterState(RasterState);

    auto GraphicPipeline = Device->CreateGraphicPipeline(GraphicPipelineDesc, FrameBufferForPipeline);

    // mainloop

    while (!Window.ShouldClose())
    {
        OS::FWindow::DoEvents();
        if (Window.GetInput().IsKeyDown(OS::EKeyCode::Escape))
        {
            Window.SetCloseFlag(true);
        }

        auto FrameBuffer = Device->QueueWaitNextFrameBuffer(Swapchain);

        auto CmdPool = Device->CreateCmdPool();
        auto CmdDesc = RHI::FCmdListDesc().SetCmdPool(CmdPool);

        auto CmdList = Device->CreateCmdList(CmdDesc);
        CmdList->BeginCmd();

        CmdList->BindFrameBuffer(FrameBuffer);

        CmdList->BindGraphicPipeline(GraphicPipeline);

        CmdList->SetViewportNoScissor(0,512,0,512);

        CmdList->Draw(3, 0, 1, 0);

        CmdList->EndCmd();

        Device->ExcuteCmdList(CmdList);

        Device->QueueWaitPresent(Swapchain, FrameBuffer);

        Device->GC();
    }
    Device->WaitIdle();

    return 0;
}
