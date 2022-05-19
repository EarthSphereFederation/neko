#include <array>
#include <atomic>
#include <cassert>

#ifdef NEKO_RHI_VULKAN
#include <vulkan/vulkan.h>
#endif
#include <GLFW/glfw3.h>

#include "MiniCore/ScopeGuard.h"
#include "OS/Window.h"

namespace Neko::OS
{

    struct FWindow::FImpl
    {
        GLFWwindow *GLFWWindow = nullptr;
        std::unique_ptr<FInput> Input;
        TSender<FWindowCloseEvent, FWindowResizeEvent, FWindowFocusEvent> Sender;
        bool bHasFocus = true;
    };

    namespace
    {
        
        struct GLFWKeyCodeTable
        {
            std::array<EKeyCode, GLFW_KEY_LAST + 1> Keys;

            GLFWKeyCodeTable()
                : Keys{}
            {
                std::fill(Keys.begin(), Keys.end(), EKeyCode::Unknown);

                Keys[GLFW_KEY_SPACE]      = EKeyCode::Space;
                Keys[GLFW_KEY_APOSTROPHE] = EKeyCode::Apostrophe;
                Keys[GLFW_KEY_COMMA]      = EKeyCode::Comma;
                Keys[GLFW_KEY_MINUS]      = EKeyCode::Minus;
                Keys[GLFW_KEY_PERIOD]     = EKeyCode::Period;
                Keys[GLFW_KEY_SLASH]      = EKeyCode::Slash;

                Keys[GLFW_KEY_SEMICOLON] = EKeyCode::Semicolon;
                Keys[GLFW_KEY_EQUAL]     = EKeyCode::Equal;

                Keys[GLFW_KEY_LEFT_BRACKET]  = EKeyCode::LeftBracket;
                Keys[GLFW_KEY_BACKSLASH]     = EKeyCode::Backslash;
                Keys[GLFW_KEY_RIGHT_BRACKET] = EKeyCode::RightBracket;
                Keys[GLFW_KEY_GRAVE_ACCENT]  = EKeyCode::GraveAccent;
                Keys[GLFW_KEY_ESCAPE]        = EKeyCode::Escape;

                Keys[GLFW_KEY_ENTER]     = EKeyCode::Enter;
                Keys[GLFW_KEY_TAB]       = EKeyCode::Tab;
                Keys[GLFW_KEY_BACKSPACE] = EKeyCode::Backspace;
                Keys[GLFW_KEY_INSERT]    = EKeyCode::Insert;
                Keys[GLFW_KEY_DELETE]    = EKeyCode::Delete;

                Keys[GLFW_KEY_RIGHT] = EKeyCode::Right;
                Keys[GLFW_KEY_LEFT]  = EKeyCode::Left;
                Keys[GLFW_KEY_DOWN]  = EKeyCode::Down;
                Keys[GLFW_KEY_UP]    = EKeyCode::Up;

                Keys[GLFW_KEY_HOME] = EKeyCode::Home;
                Keys[GLFW_KEY_END]  = EKeyCode::End;

                Keys[GLFW_KEY_F1]  = EKeyCode::F1;
                Keys[GLFW_KEY_F2]  = EKeyCode::F2;
                Keys[GLFW_KEY_F3]  = EKeyCode::F3;
                Keys[GLFW_KEY_F4]  = EKeyCode::F4;
                Keys[GLFW_KEY_F5]  = EKeyCode::F5;
                Keys[GLFW_KEY_F6]  = EKeyCode::F6;
                Keys[GLFW_KEY_F7]  = EKeyCode::F7;
                Keys[GLFW_KEY_F8]  = EKeyCode::F8;
                Keys[GLFW_KEY_F9]  = EKeyCode::F9;
                Keys[GLFW_KEY_F10] = EKeyCode::F10;
                Keys[GLFW_KEY_F11] = EKeyCode::F11;
                Keys[GLFW_KEY_F12] = EKeyCode::F12;

                Keys[GLFW_KEY_KP_0] = EKeyCode::NumPad0;
                Keys[GLFW_KEY_KP_1] = EKeyCode::NumPad1;
                Keys[GLFW_KEY_KP_2] = EKeyCode::NumPad2;
                Keys[GLFW_KEY_KP_3] = EKeyCode::NumPad3;
                Keys[GLFW_KEY_KP_4] = EKeyCode::NumPad4;
                Keys[GLFW_KEY_KP_5] = EKeyCode::NumPad5;
                Keys[GLFW_KEY_KP_6] = EKeyCode::NumPad6;
                Keys[GLFW_KEY_KP_7] = EKeyCode::NumPad7;
                Keys[GLFW_KEY_KP_8] = EKeyCode::NumPad8;
                Keys[GLFW_KEY_KP_9] = EKeyCode::NumPad9;

                Keys[GLFW_KEY_KP_DECIMAL]  = EKeyCode::NumPadDemical;
                Keys[GLFW_KEY_KP_DIVIDE]   = EKeyCode::NumPadDiv;
                Keys[GLFW_KEY_KP_MULTIPLY] = EKeyCode::NumPadMul;
                Keys[GLFW_KEY_KP_SUBTRACT] = EKeyCode::NumPadSub;
                Keys[GLFW_KEY_KP_ADD]      = EKeyCode::NumPadAdd;
                Keys[GLFW_KEY_KP_ENTER]    = EKeyCode::NumPadEnter;

                Keys[GLFW_KEY_LEFT_SHIFT]    = EKeyCode::LeftShift;
                Keys[GLFW_KEY_LEFT_CONTROL]  = EKeyCode::LeftCtrl;
                Keys[GLFW_KEY_LEFT_ALT]      = EKeyCode::LeftAlt;
                Keys[GLFW_KEY_RIGHT_SHIFT]   = EKeyCode::RightShift;
                Keys[GLFW_KEY_RIGHT_CONTROL] = EKeyCode::RightCtrl;
                Keys[GLFW_KEY_RIGHT_ALT]     = EKeyCode::RightAlt;

                for(int i = 0; i < 9; ++i)
                {
                    Keys['0' + i] = static_cast<EKeyCode>(static_cast<int>(EKeyCode::D0) + i);
                }

                for(int i = 0; i < 26; ++i)
                {
                    Keys['A' + i] = static_cast<EKeyCode>(static_cast<int>(EKeyCode::A) + i);
                }
            }
        };

        class FGLFWDestroyer
        {
        public:

            bool bInitialized = false;

            ~FGLFWDestroyer()
            {
                if(bInitialized)
                {
                    glfwTerminate();
                }
            }
        };

        FGLFWDestroyer GGLFWDestroyer;

        void CheckGLFWInitialization()
        {
            if(!GGLFWDestroyer.bInitialized)
            {
                if(!glfwInit())
                {
                    throw FOSException("failed to initialize glfw");
                }
                GGLFWDestroyer.bInitialized = true;
            }
        }

        std::set<FInput *> GWindowInputs;

    } // namespace anonymous

    void FWindow::DoEvents()
    {
        CheckGLFWInitialization();
        for(auto Input : GWindowInputs)
        {
            Input->PreUpdate();
        }
        glfwPollEvents();
    }

    FWindow::~FWindow()
    {
        if(Impl)
        {
            return;
        }
        assert(Impl->GLFWWindow);
        glfwDestroyWindow(Impl->GLFWWindow);
        GWindowInputs.erase(Impl->Input.get());
        Impl.reset();
    }

    bool FWindow::IsInitialized() const
    {
        return Impl != nullptr;
    }

    bool FWindow::ShouldClose() const
    {
        assert(Impl);
        return glfwWindowShouldClose(Impl->GLFWWindow);
    }

    void FWindow::SetCloseFlag(bool Flag)
    {
        assert(Impl);
        glfwSetWindowShouldClose(Impl->GLFWWindow, Flag ? 1 : 0);
    }

    FInput &FWindow::GetInput() const
    {
        assert(Impl);
        return *Impl->Input;
    }

    const char **FWindow::GetRequiredVulkanInstanceExtensions(uint32_t *Count)
    {
#ifdef NEKO_RHI_VULKAN
        assert(Count);
        CheckGLFWInitialization();
        const char **Ret = glfwGetRequiredInstanceExtensions(Count);
        if(!*Count)
        {
            throw FOSException("failed to get vulkan instance extensions for window surface");
        }
        return Ret;
#else
        throw OSException("vulkan backend is not supported");
#endif
    }

    NativeObject FWindow::CreateVulkanSurface(NativeObject VulkanInstance)
    {
    #ifdef NEKO_RHI_VULKAN
        VkSurfaceKHR Surface;
        auto Instance = static_cast<VkInstance>(VulkanInstance.pointer);
        if(glfwCreateWindowSurface(Instance, Impl->GLFWWindow, nullptr, &Surface) != VK_SUCCESS)
        {
            throw FOSException("failed to create vulkan surface");
        }
        return Surface;
    #else
        throw FOSException("vulkan backend is not supported");
    #endif
    }

    NEKO_DEFINE_EVENT_SENDER(FWindow, Impl->Sender, FWindowCloseEvent)
    NEKO_DEFINE_EVENT_SENDER(FWindow, Impl->Sender, FWindowResizeEvent)
    NEKO_DEFINE_EVENT_SENDER(FWindow, Impl->Sender, FWindowFocusEvent)

    FWindow::FWindow(std::unique_ptr<FImpl> impl)
        : Impl(std::move(impl))
    {
        assert(Impl);
    }

    FWindowBuilder &FWindowBuilder::SetSize(int NewWidth, int NewHeight)
    {
        Width = NewWidth;
        Height = NewHeight;
        return *this;
    }

    FWindowBuilder &FWindowBuilder::SetMaximized(bool bShouldMaximized)
    {
        bMaximized = bShouldMaximized;
        return *this;
    }

    FWindowBuilder &FWindowBuilder::SetTitle(std::string NewTitle)
    {
        Title = std::move(NewTitle);
        return *this;
    }

    FWindow FWindowBuilder::CreateWindow() const
    {
        CheckGLFWInitialization();

        // create window

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_MAXIMIZED, bMaximized ? 1 : 0);
        GLFWwindow *GLFWWindow = glfwCreateWindow(Width, Height, Title.c_str(), nullptr, nullptr);
        if(!GLFWWindow)
        {
            throw FOSException("failed to create glfw window");
        }
        NEKO_SCOPE_FAIL{ glfwDestroyWindow(GLFWWindow); };

        // bind window <-> impl

        auto Impl = std::make_unique<FWindow::FImpl>();
        Impl->GLFWWindow = GLFWWindow;
        glfwSetWindowUserPointer(GLFWWindow, Impl.get());

        // set event callbacks

        auto WindowCloseCallback = +[](GLFWwindow *Window)
        {
            if(auto Impl = static_cast<FWindow::FImpl *>(glfwGetWindowUserPointer(Window)))
            {
                Impl->Sender.Send(FWindowCloseEvent{});
            }
        };
        glfwSetWindowCloseCallback(GLFWWindow, WindowCloseCallback);

        auto FramebufferResizeCallback = +[](GLFWwindow *Window, int NewWidth, int NewHeight)
        {
            if(auto Impl = static_cast<FWindow::FImpl *>(glfwGetWindowUserPointer(Window)))
            {
                Impl->Sender.Send(FWindowResizeEvent{ NewWidth, NewHeight });
            }
        };
        glfwSetFramebufferSizeCallback(GLFWWindow, FramebufferResizeCallback);

        auto FocusCallback = +[](GLFWwindow *Window, int Focused)
        {
            if(auto Impl = static_cast<FWindow::FImpl *>(glfwGetWindowUserPointer(Window)))
            {
                const bool HasFocus = Focused != 0;
                if(HasFocus != Impl->bHasFocus)
                {
                    Impl->bHasFocus = HasFocus;
                    Impl->Sender.Send(FWindowFocusEvent{ HasFocus });
                }
            }
        };
        glfwSetWindowFocusCallback(GLFWWindow, FocusCallback);

        auto ScrollBack = +[](GLFWwindow *Window, double, double YOffset)
        {
            if(auto Impl = static_cast<FWindow::FImpl *>(glfwGetWindowUserPointer(Window)))
            {
                Impl->Input->TriggerWheelScroll(static_cast<int>(YOffset));
            }
        };
        glfwSetScrollCallback(GLFWWindow, ScrollBack);

        auto MouseButtonCallback = +[](GLFWwindow *Window, int Button, int Action, int)
        {
            auto Impl = static_cast<FWindow::FImpl *>(glfwGetWindowUserPointer(Window));
            if(!Impl)
            {
                return;
            }

            EKeyCode KeyCode;
            if(Button == GLFW_MOUSE_BUTTON_LEFT)
            {
                KeyCode = EKeyCode::MouseLeft;
            }
            else if(Button == GLFW_MOUSE_BUTTON_MIDDLE)
            {
                KeyCode = EKeyCode::MouseMiddle;
            }
            else if(Button == GLFW_MOUSE_BUTTON_RIGHT)
            {
                KeyCode = EKeyCode::MouseRight;
            }
            else
            {
                return;
            }

            if(Action == GLFW_PRESS)
            {
                Impl->Input->TriggerKeyDown(KeyCode);
            }
            else if(Action == GLFW_RELEASE)
            {
                Impl->Input->TriggerKeyUp(KeyCode);
            }
        };
        glfwSetMouseButtonCallback(GLFWWindow, MouseButtonCallback);

        auto KeyCallback = [](GLFWwindow *Window, int Key, int, int Action, int)
        {
            auto Impl = static_cast<FWindow::FImpl *>(glfwGetWindowUserPointer(Window));
            if(!Impl)
            {
                return;
            }

            static const GLFWKeyCodeTable keyCodeTable;
            if(Key < 0 || Key >= static_cast<int>(keyCodeTable.Keys.size()))
            {
                return;
            }

            const EKeyCode keycode = keyCodeTable.Keys[Key];
            if(keycode == EKeyCode::Unknown)
            {
                return;
            }

            if(Action == GLFW_PRESS)
            {
                Impl->Input->TriggerKeyDown(keycode);
            }
            else if(Action == GLFW_RELEASE)
            {
                Impl->Input->TriggerKeyUp(keycode);
            }
        };
        glfwSetKeyCallback(GLFWWindow, KeyCallback);

        auto CharInputCallback = [](GLFWwindow *Window, unsigned int Char)
        {
            if(auto Impl = static_cast<FWindow::FImpl *>(glfwGetWindowUserPointer(Window)))
            {
                Impl->Input->TriggerCharInput(Char);
            }
        };
        glfwSetCharCallback(GLFWWindow, CharInputCallback);

        // initial state

        Impl->bHasFocus = glfwGetWindowAttrib(GLFWWindow, GLFW_FOCUSED);

        // input manager

        Impl->Input = std::unique_ptr<FInput>(new FInput(GLFWWindow));
        GWindowInputs.insert(Impl->Input.get());

        return FWindow(std::move(Impl));
    }

} // namespace Neko::OS
