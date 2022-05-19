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

    void FInput::LockCursor(bool bLocked)
    {
        if(bLocked != bLocked)
        {
            auto window = static_cast<GLFWwindow *>(GLFWWindow);
            const int mode = bLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
            glfwSetInputMode(window, GLFW_CURSOR, mode);
            bLocked = bLocked;
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
        auto [newX, newY] = QueryCursorPosition();
        RelativeX = newX - AbsoluteX;
        RelativeY = newY - AbsoluteY;
        AbsoluteX = newX;
        AbsoluteY = newY;

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
        auto window = static_cast<GLFWwindow *>(GLFWWindow);
        double newXD, newYD;
        glfwGetCursorPos(window, &newXD, &newYD);
        const float newX = static_cast<float>(newXD);
        const float newY = static_cast<float>(newYD);
        return { newX, newY };
    }

} // namespace NekoOS
