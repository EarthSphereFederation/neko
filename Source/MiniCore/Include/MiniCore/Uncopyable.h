#pragma once

namespace Neko
{

    class FUncopyable
    {
    public:

        FUncopyable() = default;
        FUncopyable(const FUncopyable &) = delete;
        FUncopyable(FUncopyable &&) noexcept = delete;
        FUncopyable &operator=(const FUncopyable &) = delete;
        FUncopyable &operator=(FUncopyable &&) noexcept = delete;
    };

} // namespace Neko
