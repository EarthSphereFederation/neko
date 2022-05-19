#pragma once

#include <stdexcept>
#include <type_traits>

#include "MiniCore/AnonymousName.h"
#include "MiniCore/Uncopyable.h"

namespace Neko
{

template<typename T, typename = std::enable_if_t<std::is_invocable_v<T>>>
class TScopeGuard : public FUncopyable
{
    bool call_ = true;

    T func_;

public:

    explicit TScopeGuard(const T &func)
        : func_(func)
    {

    }

    explicit TScopeGuard(T &&func)
        : func_(std::move(func))
    {

    }

    ~TScopeGuard()
    {
        if(call_)
            func_();
    }

    void Dismiss()
    {
        call_ = false;
    }
};

template<typename F, bool ExecuteOnException>
class TExceptionScopeGuard : public FUncopyable
{
    F func_;

    int exceptions_;

public:

    explicit TExceptionScopeGuard(const F &func)
        : func_(func), exceptions_(std::uncaught_exceptions())
    {

    }

    explicit TExceptionScopeGuard(F &&func)
        : func_(std::move(func)), exceptions_(std::uncaught_exceptions())
    {

    }

    ~TExceptionScopeGuard()
    {
        const int now_exceptions = std::uncaught_exceptions();
        if((now_exceptions > exceptions_) == ExecuteOnException)
            func_();
    }
};

struct FScopeGuardBuilder {};
struct FScopeGuardOnFailBuilder {};
struct FScopeGuardOnSuccessBuilder {};

template<typename Func>
auto operator+(FScopeGuardBuilder, Func &&f)
{
    return TScopeGuard<std::decay_t<Func>>(std::forward<Func>(f));
}

template<typename Func>
auto operator+(FScopeGuardOnFailBuilder, Func &&f)
{
    return TExceptionScopeGuard<std::decay_t<Func>, true>(std::forward<Func>(f));
}

template<typename Func>
auto operator+(FScopeGuardOnSuccessBuilder, Func &&f)
{
    return TExceptionScopeGuard<std::decay_t<Func>, false>(std::forward<Func>(f));
}

#define NEKO_SCOPE_EXIT auto NEKO_ANONYMOUS_NAME(_nekoScopeExit) = ::Neko::FScopeGuardBuilder{} + [&]
#define NEKO_SCOPE_FAIL auto NEKO_ANONYMOUS_NAME(_nekoScopeFail) = ::Neko::FScopeGuardOnFailBuilder{} + [&]
#define NEKO_SCOPE_SUCCESS auto NEKO_ANONYMOUS_NAME(_nekoScopeSuccess) = ::Neko::FScopeGuardOnSuccessBuilder{} + [&]

} // namespace Neko
