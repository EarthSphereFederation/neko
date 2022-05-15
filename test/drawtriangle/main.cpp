#include <iostream>
#include "neko.h"
#include <GLFW/glfw3.h>
using namespace neko;

int main(int, char**) {
    if (!glfwInit())
    {
        printf("GLFW initialization failed.\n");
        return 0;
    }

    uint32_t SurfaceExtensionCount;
    const char** SurfaceExtensionNames = glfwGetRequiredInstanceExtensions(&SurfaceExtensionCount);

    auto VkDesc = RHIVulkanDesc()
        .SetInstanceExtensions(SurfaceExtensionNames, SurfaceExtensionCount);
    
    auto Features = RHIFeatures().SetSwapchain(true);
    
    RHIDeviceDesc DevDesc;
    DevDesc.SetVulkanDesc(VkDesc).SetValidation(true)
        .SetGpuIndex(0)
        .SetFeatures(Features);

    auto Device = CreateDevice(DevDesc);

    if (Device)
    {
        RHIBindingLayoutDesc BindingLayoutDesc;
        BindingLayoutDesc.AddBinding({ 0,RHIResourceType::UniformBuffer });

        auto BindingLayout = Device->CreateBindingLayout(BindingLayoutDesc);
    }
       
    return 0;
}
 