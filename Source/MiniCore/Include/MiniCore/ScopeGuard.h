#pragma once

#include <stdexcept>
#include <type_traits>

#include "MiniCore/AnonymousName.h"
#include "MiniCore/Uncopyable.h"

namespace Neko
{

template<typename T> requires std::is_invocable_v<T>
class TScopeGuard : public FUncopyable
{
    bool bShouldCall = true;

    T Function;

public:

    explicit TScopeGuard(const T &Callable)
        : Function(Callable)
    {

    }

    explicit TScopeGuard(T &&Callable)
        : Function(std::move(Callable))
    {

    }

    ~TScopeGuard()
    {
        if(bShouldCall)
            Function();
    }

    void Dismiss()
    {
        bShouldCall = false;
    }
};

template<typename F, bool ExecuteOnException> requires std::is_invocable_v<F>
class TExceptionScopeGuard : public FUncopyable
{
    F Function;
    int ExceptionCounter;

public:

    explicit TExceptionScopeGuard(const F &Callable)
        : Function(Callable), ExceptionCounter(std::uncaught_exceptions())
    {

    }

    explicit TExceptionScopeGuard(F &&Callable)
        : Function(std::move(Callable)), ExceptionCounter(std::uncaught_exceptions())
    {

    }

    ~TExceptionScopeGuard()
    {
        const int NowExceptions = std::uncaught_exceptions();
        if((NowExceptions > ExceptionCounter) == ExecuteOnException)
            Function();
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
