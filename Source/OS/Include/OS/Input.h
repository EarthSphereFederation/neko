#pragma once

#include <bitset>

#include "OS/Event.h"
#include "OS/KeyCode.h"

namespace Neko::OS
{

    struct FCursorMoveEvent  { float AbsoluteX, AbsoluteY, RelativeX, RelativeY; };
    struct FWheelScrollEvent { int RelativeOffset; };
    struct FKeyDownEvent     { EKeyCode Key; };
    struct FKeyUpEvent       { EKeyCode Key; };
    struct FCharInputEvent   { uint32_t Char; };

    class FInput : public FUncopyable
    {
    public:

        ~FInput();

        void Clear();

        bool IsKeyPressed(EKeyCode Key) const;
        bool IsKeyDown   (EKeyCode Key) const;
        bool IsKeyUp     (EKeyCode Key) const;

        float GetCursorAbsolutePositionX() const;
        float GetCursorAbsolutePositionY() const;

        float GetCursorRelativePositionX() const;
        float GetCursorRelativePositionY() const;

        void LockCursor(bool ShouldLock);
        bool IsCursorLocked() const;

        NEKO_DECLARE_EVENT_SENDER(FCursorMoveEvent)
        NEKO_DECLARE_EVENT_SENDER(FWheelScrollEvent)
        NEKO_DECLARE_EVENT_SENDER(FKeyDownEvent)
        NEKO_DECLARE_EVENT_SENDER(FKeyUpEvent)
        NEKO_DECLARE_EVENT_SENDER(FCharInputEvent)

        void PreUpdate();
        void TriggerWheelScroll(int Offset);
        void TriggerKeyDown  (EKeyCode Key);
        void TriggerKeyUp    (EKeyCode Key);
        void TriggerCharInput(uint32_t Char);

    private:

        using FEventSender = TSender<FCursorMoveEvent, FWheelScrollEvent, FKeyDownEvent, FKeyUpEvent, FCharInputEvent>;
        using FKeyStates = std::bitset<static_cast<int>(EKeyCode::MaxValue) + 1>;

        friend class FWindowBuilder;

        explicit FInput(void *GLFWWindow);

        std::pair<float, float> QueryCursorPosition() const;

        void *GLFWWindow;

        float AbsoluteX;
        float AbsoluteY;
        float RelativeX;
        float RelativeY;

        bool bLocked;

        FKeyStates LastKeys;
        FKeyStates CurrKeys;

        FEventSender Sender;
    };

} // namespace Neko::OS
