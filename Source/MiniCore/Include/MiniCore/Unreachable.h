#pragma once

namespace Neko
{

    [[noreturn]] inline void Unreachable()
    {
#if defined(_MSC_VER)
        __assume(0);
#elif defined(__clang__) || defined(__GNUC__)
        __builtin_unreachable();
#else
        std::terminate();
#endif
    }

} // namespace Neko
