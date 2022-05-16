#include <iostream>
#include "../../src/rhi/public/rhi.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
using namespace neko::rhi;

int main(int, char**) {
    if (!glfwInit())
    {
        printf("GLFW initialization failed.\n");
        return 0;
    }
    else
    {
        printf("GLFW initialization successfully!\n");
    }

    uint32_t SurfaceExtensionCount;
    const char** SurfaceExtensionNames = glfwGetRequiredInstanceExtensions(&SurfaceExtensionCount);

    auto VkDesc = RHIDeviceDesc::RHIVulkanDesc()
        .SetInstanceExtensions(SurfaceExtensionNames, SurfaceExtensionCount);
    
    auto Features = RHIFeatures().SetSwapchain(true);
    
    RHIDeviceDesc DevDesc;
    DevDesc.SetVulkanDesc(VkDesc).SetValidation(true)
        .SetGpuIndex(0)
        .SetFeatures(Features);

    auto Device = CreateDevice(DevDesc);

    auto Instance = Device->GetVkInstance();
    GLFWwindow* window;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(512, 512, "neko_drawtriangle", nullptr, nullptr);
    VkSurfaceKHR GLFWSurface;
    glfwCreateWindowSurface(Instance, window, nullptr, &GLFWSurface);

    if (Device)
    {
        auto SwapchainDesc = RHISwapChainDesc().SetFormat(RHIFormat::B8G8R8A8_SNORM)
            .SetVSync(true)
            .SetSurface(GLFWSurface);

        auto Swapchain = Device->CreateSwapChain(SwapchainDesc);
        
        RHIBindingLayoutDesc BindingLayoutDesc;
        BindingLayoutDesc.AddBinding({ 0,RHIResourceType::UniformBuffer });

        auto BindingLayout = Device->CreateBindingLayout(BindingLayoutDesc);
    }

    vkDestroySurfaceKHR(Instance, GLFWSurface, nullptr);
       
    return 0;
}
 