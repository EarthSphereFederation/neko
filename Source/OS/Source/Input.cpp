#include <GLFW/glfw3.h>

#include "OS/Input.h"

namespace Neko::OS
{

    FInput::~FInput()
    {
        if(bLocked)
        {
            LockCursor(false);
        }
    }

    void FInput::Clear()
    {
        LastKeys.reset();
        CurrKeys.reset();
        AbsoluteX = AbsoluteY = 0;
        RelativeX = RelativeY = 0;
        LockCursor(false);
    }

    bool FInput::IsKeyPressed(EKeyCode Key) const
    {
        return CurrKeys[static_cast<int>(Key)];
    }

    bool FInput::IsKeyDown(EKeyCode Key) const
    {
        return !LastKeys[static_cast<int>(Key)] && CurrKeys[static_cast<int>(Key)];
    }

    bool FInput::IsKeyUp(EKeyCode Key) const
    {
        return LastKeys[static_cast<int>(Key)] && !CurrKeys[static_cast<int>(Key)];
    }

    float FInput::GetCursorAbsolutePositionX() const
    {
        return AbsoluteX;
    }

    float FInput::GetCursorAbsolutePositionY() const
    {
        return AbsoluteY;
    }

    float FInput::GetCursorRelativePositionX() const
    {
        return RelativeX;
    }

    float FInput::GetCursorRelativePositionY() const
    {
        return RelativeY;
    }

    void FInput::LockCursor(bool ShouldLock)
    {
        if(ShouldLock != bLocked)
        {
            auto Window = static_cast<GLFWwindow *>(GLFWWindow);
            const int Mode = ShouldLock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
            glfwSetInputMode(Window, GLFW_CURSOR, Mode);
            bLocked = ShouldLock;
            std::tie(AbsoluteX, AbsoluteY) = QueryCursorPosition();
        }
    }

    bool FInput::IsCursorLocked() const
    {
        return bLocked;
    }

    NEKO_DEFINE_EVENT_SENDER(FInput, Sender, FCursorMoveEvent)
    NEKO_DEFINE_EVENT_SENDER(FInput, Sender, FWheelScrollEvent)
    NEKO_DEFINE_EVENT_SENDER(FInput, Sender, FKeyDownEvent)
    NEKO_DEFINE_EVENT_SENDER(FInput, Sender, FKeyUpEvent)
    NEKO_DEFINE_EVENT_SENDER(FInput, Sender, FCharInputEvent)

    void FInput::PreUpdate()
    {
        auto [NewX, NewY] = QueryCursorPosition();
        RelativeX = NewX - AbsoluteX;
        RelativeY = NewY - AbsoluteY;
        AbsoluteX = NewX;
        AbsoluteY = NewY;

        if(RelativeX != 0 || RelativeY != 0)
        {
            Sender.Send(FCursorMoveEvent{ 
                AbsoluteX,
                AbsoluteY,
                RelativeX,
                RelativeY
            });
        }

        LastKeys = CurrKeys;
    }

    void FInput::TriggerWheelScroll(int Offset)
    {
        Sender.Send(FWheelScrollEvent{ Offset });
    }

    void FInput::TriggerKeyDown(EKeyCode Key)
    {
        CurrKeys[static_cast<int>(Key)] = true;
        Sender.Send(FKeyDownEvent{ Key });
    }

    void FInput::TriggerKeyUp(EKeyCode Key)
    {
        CurrKeys[static_cast<int>(Key)] = false;
        Sender.Send(FKeyUpEvent{ Key });
    }

    void FInput::TriggerCharInput(uint32_t Char)
    {
        Sender.Send(FCharInputEvent{ Char });
    }

    FInput::FInput(void *GLFWWindow)
    {
        assert(GLFWWindow);
        this->GLFWWindow = GLFWWindow;

        std::tie(AbsoluteX, AbsoluteY) = QueryCursorPosition();
        RelativeX = 0;
        RelativeY = 0;

        glfwSetInputMode(static_cast<GLFWwindow *>(GLFWWindow), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        bLocked = false;

        LastKeys.reset();
        CurrKeys.reset();
    }

    std::pair<float, float> FInput::QueryCursorPosition() const
    {
        auto Window = static_cast<GLFWwindow *>(GLFWWindow);
        double NewXD, NewYD;
        glfwGetCursorPos(Window, &NewXD, &NewYD);
        const float NewX = static_cast<float>(NewXD);
        const float NewY = static_cast<float>(NewYD);
        return { NewX, NewY };
    }

} // namespace NekoOS
