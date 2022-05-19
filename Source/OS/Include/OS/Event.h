#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <set>
#include <tuple>

#include "MiniCore/Uncopyable.h"

namespace Neko::OS
{

    template<typename Event>
    class TReceiverSet;

    template<typename Event>
    class TReceiver : public std::enable_shared_from_this<TReceiver<Event>>, public FUncopyable
    {
    public:

        virtual ~TReceiver() = default;

        virtual void Handle(const Event &EventObject) = 0;

    private:

        friend class TReceiverSet<Event>;

        mutable std::set<TReceiverSet<Event> *> ContainedSets;
        mutable std::set<TReceiverSet<Event> *> ContainedSetsWithOwnership;
    };

    template<typename Event>
    class TReceiverSet : public FUncopyable
    {
    public:

        ~TReceiverSet();

        void Send(const Event &EventObject) const;

        void Attach(TReceiver<Event> *Handler);

        void Attach(std::shared_ptr<TReceiver<Event>> HandlerWithOwnership);

        void Detach(const TReceiver<Event> *Handler);

        void Detach(const std::shared_ptr<TReceiver<Event>> &Handler);

        void Detach(const std::shared_ptr<const TReceiver<Event>> &Handler);

        void DetachAll();

    private:

        std::set<TReceiver<Event> *, std::less<>> Handlers;
        std::set<std::shared_ptr<TReceiver<Event>>, std::less<>> HandlersWithOwnership;
    };

    template<typename...Events>
    class TSender : public FUncopyable
    {
    public:

        template<typename Event>
        void Send(const Event &EventObject) const;

        template<typename Event>
        void Attach(TReceiver<Event> *Handler);

        template<typename Event>
        void Attach(std::shared_ptr<TReceiver<Event>> HandlerWithOwnership);

        template<typename Event>
        void Detach(const TReceiver<Event> *Handler);

        template<typename Event>
        void Detach(const std::shared_ptr<TReceiver<Event>> &Handler);

        template<typename Event>
        void Detach(const std::shared_ptr<const TReceiver<Event>> &Handler);

        template<typename Event>
        void DetachAll();

        void DetachAllTypes();

    private:

        std::tuple<TReceiverSet<Events>...> ReceiverSets;
    };

    template<typename Event>
    class TFunctionalReceiver : public TReceiver<Event>
    {
    public:

        using FFunction = std::function<void(const Event &)>;

        explicit TFunctionalReceiver(std::function<void()> FunctionPtr);

        explicit TFunctionalReceiver(FFunction FunctionPtr = {});

        void SetFunction(std::function<void()> FunctionPtr);

        void SetFunction(FFunction FunctionPtr);

        void Handle(const Event &EventObject) override;

    private:

        FFunction FunctionPointer;
    };

    template<typename Event, typename Class>
    class TClassReceiver : public TReceiver<Event>
    {
    public:

        using MemberFunctionPointer = void(Class::*)(const Event &);

        TClassReceiver();

        TClassReceiver(Class *ClassPtr, MemberFunctionPointer FunctionPtr);

        void SetClassInstance(Class *ClassPtr);

        void SetMemberFunction(MemberFunctionPointer FunctionPtr);

        void Handle(const Event &EventObject) override;

    private:

        Class ClassPointer;
        MemberFunctionPointer FunctionPointer;
    };

    #define NEKO_DECLARE_EVENT_SENDER(Event)                                     \
        void Attach(Neko::OS::TReceiver<Event> *Handler);                        \
        void Attach(std::shared_ptr<Neko::OS::TReceiver<Event>> Handler);        \
        void Attach(std::function<void(const Event &)> f);                       \
        void Detach(Neko::OS::TReceiver<Event> *Handler);                        \
        void Detach(const std::shared_ptr<Neko::OS::TReceiver<Event>> &Handler); \
        void Detach(const std::shared_ptr<const Neko::OS::TReceiver<Event>> &Handler);

    #define NEKO_DEFINE_EVENT_SENDER(Class, Sender, Event)                                                  \
        void Class::Attach(Neko::OS::TReceiver<Event> *Handler)                                             \
            { Sender.Attach<Event>(Handler); }                                                              \
        void Class::Attach(std::shared_ptr<Neko::OS::TReceiver<Event>> Handler)                             \
            { Sender.Attach<Event>(std::move(Handler)); }                                                   \
        void Class::Attach(std::function<void(const Event &)> f)                                            \
            { Sender.Attach<Event>(std::make_shared<Neko::OS::TFunctionalReceiver<Event>>(std::move(f))); } \
        void Class::Detach(Neko::OS::TReceiver<Event> *Handler)                                             \
            { Sender.Detach<Event>(Handler); }                                                              \
        void Class::Detach(const std::shared_ptr<Neko::OS::TReceiver<Event>> &Handler)                      \
            { Sender.Detach<Event>(Handler); }                                                              \
        void Class::Detach(const std::shared_ptr<const Neko::OS::TReceiver<Event>> &Handler)                \
            { Sender.Detach<Event>(Handler); }

    template<typename Event>
    TReceiverSet<Event>::~TReceiverSet()
    {
        DetachAll();
    }

    template<typename Event>
    void TReceiverSet<Event>::Send(const Event &EventObject) const
    {
        for(auto H : Handlers)
            H->Handle(EventObject);
        for(auto &H : HandlersWithOwnership)
            H->Handle(EventObject);
    }

    template<typename Event>
    void TReceiverSet<Event>::Attach(TReceiver<Event> *Handler)
    {
        assert(Handler);
        Handler->ContainedSets.insert(this);
        Handlers.insert(Handler);
    }

    template<typename Event>
    void TReceiverSet<Event>::Attach(std::shared_ptr<TReceiver<Event>> HandlerWithOwnership)
    {
        assert(HandlerWithOwnership);
        HandlerWithOwnership->ContainedSetsWithOwnership.insert(this);
        HandlersWithOwnership.insert(std::move(HandlerWithOwnership));
    }

    template<typename Event>
    void TReceiverSet<Event>::Detach(const TReceiver<Event> *Handler)
    {
        assert(Handler);
        assert(Handler->ContainedSets.contains(this));
        assert(Handlers.contains(Handler));
        Handler->ContainedSets.erase(this);
        Handlers.erase(Handler);
    }

    template<typename Event>
    void TReceiverSet<Event>::Detach(const std::shared_ptr<TReceiver<Event>> &Handler)
    {
        assert(Handler);
        assert(Handler->ContainedSetsWithOwnership.contains(this));
        assert(HandlersWithOwnership.contains(Handler));
        Handler->ContainedSetsWithOwnership.erase(this);
        HandlersWithOwnership.erase(Handler);
    }

    template<typename Event>
    void TReceiverSet<Event>::Detach(const std::shared_ptr<const TReceiver<Event>> &Handler)
    {
        assert(Handler);
        assert(Handler->ContainedSetsWithOwnership.contains(this));
        assert(HandlersWithOwnership.contains(Handler));
        Handler->ContainedSetsWithOwnership.erase(this);
        HandlersWithOwnership.erase(Handler);
    }

    template<typename Event>
    void TReceiverSet<Event>::DetachAll()
    {
        while(!Handlers.empty())
            this->Detach(*Handlers.begin());
        while(!HandlersWithOwnership.empty())
            this->Detach(*HandlersWithOwnership.begin());
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::Send(const Event &EventObject) const
    {
        std::get<TReceiverSet<Event>>(ReceiverSets).Send(EventObject);
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::Attach(TReceiver<Event> *Handler)
    {
        std::get<TReceiverSet<Event>>(ReceiverSets).Attach(Handler);
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::Attach(std::shared_ptr<TReceiver<Event>> HandlerWithOwnership)
    {
        std::get<TReceiverSet<Event>>(ReceiverSets).Attach(std::move(HandlerWithOwnership));
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::Detach(const TReceiver<Event> *Handler)
    {
        std::get<TReceiverSet<Event>>(ReceiverSets).Detach(Handler);
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::Detach(const std::shared_ptr<TReceiver<Event>> &Handler)
    {
        std::get<TReceiverSet<Event>>(ReceiverSets).Detach(Handler);
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::Detach(const std::shared_ptr<const TReceiver<Event>> &Handler)
    {
        std::get<TReceiverSet<Event>>(ReceiverSets).Detach(Handler);
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::DetachAll()
    {
        std::get<TReceiverSet<Event>>(ReceiverSets).DetachAll();
    }

    template<typename...Events>
    void TSender<Events...>::DetachAllTypes()
    {
        ((std::get<TReceiverSet<Events>>(ReceiverSets).DetachAll()), ...);
    }

    template<typename Event>
    TFunctionalReceiver<Event>::TFunctionalReceiver(std::function<void()> Function)
    {
        SetFunction(std::move(Function));
    }

    template<typename Event>
    TFunctionalReceiver<Event>::TFunctionalReceiver(FFunction Function)
        : FunctionPointer(std::move(Function))
    {
        
    }

    template<typename Event>
    void TFunctionalReceiver<Event>::SetFunction(std::function<void()> Function)
    {
        if(Function)
            FunctionPointer = [InterfaceFunction = std::move(Function)](const Event &){ InterfaceFunction(); };
        else
            FunctionPointer = {};
    }

    template<typename Event>
    void TFunctionalReceiver<Event>::SetFunction(FFunction f)
    {
        FunctionPointer = std::move(f);
    }

    template<typename Event>
    void TFunctionalReceiver<Event>::Handle(const Event &EventObject)
    {
        if(FunctionPointer)
            FunctionPointer(EventObject);
    }

    template<typename Event, typename Class>
    TClassReceiver<Event, Class>::TClassReceiver()
        : TClassReceiver({}, {})
    {
        
    }

    template<typename Event, typename Class>
    TClassReceiver<Event, Class>::TClassReceiver(Class *ClassPtr, MemberFunctionPointer FunctionPtr)
        : ClassPointer(ClassPtr), FunctionPointer(FunctionPtr)
    {
        
    }

    template<typename Event, typename Class>
    void TClassReceiver<Event, Class>::SetClassInstance(Class *ClassPtr)
    {
        ClassPointer = ClassPtr;
    }

    template<typename Event, typename Class>
    void TClassReceiver<Event, Class>::SetMemberFunction(MemberFunctionPointer FunctionPtr)
    {
        FunctionPointer = FunctionPtr;
    }

    template<typename Event, typename Class>
    void TClassReceiver<Event, Class>::Handle(const Event &EventObject)
    {
        if(ClassPointer && FunctionPointer)
            (ClassPointer->*FunctionPointer)(EventObject);
    }

} // namespace Neko::OS
