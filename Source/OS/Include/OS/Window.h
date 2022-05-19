#pragma once

#include <stdexcept>

#include "OS/Input.h"
#include "RHI/RHI.h"

namespace Neko::OS
{

    class FOSException : public std::runtime_error
    {
    public:

        using runtime_error::runtime_error;
    };

    struct FWindowCloseEvent { };
    struct FWindowResizeEvent { int Width, Height; };
    struct FWindowFocusEvent { bool HasFocus; };

    class FWindow : public FUncopyable
    {
    public:

        static void DoEvents();

        FWindow() = default;

        ~FWindow();

        bool IsInitialized() const;

        bool ShouldClose() const;

        void SetCloseFlag(bool Flag);

        FInput &GetInput() const;

        // vulkan

        static const char **GetRequiredVulkanInstanceExtensions(uint32_t *Count);

        NativeObject CreateVulkanSurface(NativeObject VulkanInstance);

        // events

        NEKO_DECLARE_EVENT_SENDER(FWindowCloseEvent)
        NEKO_DECLARE_EVENT_SENDER(FWindowResizeEvent)
        NEKO_DECLARE_EVENT_SENDER(FWindowFocusEvent)

    private:

        friend class FWindowBuilder;

        struct FImpl;

        explicit FWindow(std::unique_ptr<FImpl> Impl);
        
        std::unique_ptr<FImpl> Impl;
    };

    class FWindowBuilder
    {
    public:

        FWindowBuilder &SetSize(int NewWidth, int NewHeight);

        FWindowBuilder &SetMaximized(bool bShouldMaximized);

        FWindowBuilder &SetTitle(std::string NewTitle);

        FWindow CreateWindow() const;

    private:

        int Width = 640;
        int Height = 480;
        bool bMaximized = false;
        std::string Title = "Neko";
    };

} // namespace Neko::OS
