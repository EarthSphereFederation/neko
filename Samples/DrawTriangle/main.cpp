#include "OS/Window.h"
#include "RHI/RHI.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <vector>

using namespace Neko;

std::vector<char> ReadBinaryFile(const std::string& path)
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

int main(int, char**)
{
    uint32_t SurfaceExtensionCount;
    const char** SurfaceExtensionNames = OS::FWindow::GetRequiredVulkanInstanceExtensions(&SurfaceExtensionCount);

    auto VkDesc = RHIDeviceDesc::RHIVulkanDesc()
        .SetInstanceExtensions(SurfaceExtensionNames, SurfaceExtensionCount);
    
    auto Features = RHIFeatures().SetSwapchain(true);
    
    RHIDeviceDesc DevDesc;
    DevDesc.SetVulkanDesc(VkDesc).SetValidation(true)
        .SetGpuIndex(0)
        .SetFeatures(Features);

    auto Device = CreateDevice(DevDesc);
    auto Instance = Device->GetVkInstance();

    auto Window = OS::FWindowBuilder()
        .SetSize(512, 512)
        .SetTitle("neko_drawtriangle")
        .CreateWindow();
    auto WindowSurface = Window.CreateVulkanSurface(Instance);

    if (Device)
    {
        auto SwapchainDesc = RHISwapChainDesc().SetFormat(EFormat::B8G8R8A8_SNORM)
            .SetVSync(true)
            .SetSurface(WindowSurface);

        auto Swapchain = Device->CreateSwapChain(SwapchainDesc);
        auto FrameBuffer = Swapchain->GetFrameBuffer(0);
        
        auto VertexShaderCode = ReadBinaryFile("./DrawTriangle.vert.spv");
        auto VSDesc = RHIShaderDesc().SetBlob(VertexShaderCode.data())
            .SetSize(VertexShaderCode.size())
            .SetEntryPoint("main").SetStage(EShaderStage::VS);
        auto VS = Device->CreateShader(VSDesc);

        auto PixelShaderCode = ReadBinaryFile("./DrawTriangle.frag.spv");
        auto PSDesc = RHIShaderDesc().SetBlob(PixelShaderCode.data())
            .SetSize(PixelShaderCode.size())
            .SetEntryPoint("main").SetStage(EShaderStage::PS);
        auto PS = Device->CreateShader(PSDesc);

        
        auto GraphicPipelineDesc = RHIGraphicPipelineDesc()
            .SetVertexShader(VS)
            .SetPixelShader(PS);

        auto GraphicPipeline = Device->CreateGraphicPipeline(GraphicPipelineDesc, FrameBuffer);
        
        RHIBindingLayoutDesc BindingLayoutDesc;
        BindingLayoutDesc.AddBinding({ 0,EResourceType::UniformBuffer });

        auto BindingLayout = Device->CreateBindingLayout(BindingLayoutDesc);
    }

    // mainloop

    while(!Window.ShouldClose())
    {
        OS::FWindow::DoEvents();
        if(Window.GetInput().IsKeyDown(OS::EKeyCode::Escape))
        {
            Window.SetCloseFlag(true);
        }
    }

    vkDestroySurfaceKHR(Instance, WindowSurface, nullptr);

    return 0;
}
