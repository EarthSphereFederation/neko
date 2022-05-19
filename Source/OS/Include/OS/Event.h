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

        virtual void Handle(const Event &e) = 0;

    private:

        friend class TReceiverSet<Event>;

        mutable std::set<TReceiverSet<Event> *> containedSets_;
        mutable std::set<TReceiverSet<Event> *> containedSetsWithOwnership_;
    };

    template<typename Event>
    class TReceiverSet : public FUncopyable
    {
    public:

        ~TReceiverSet();

        void Send(const Event &e) const;

        void Attach(TReceiver<Event> *handler);

        void Attach(std::shared_ptr<TReceiver<Event>> handlerWithOwnership);

        void Detach(const TReceiver<Event> *handler);

        void Detach(const std::shared_ptr<TReceiver<Event>> &handler);

        void Detach(const std::shared_ptr<const TReceiver<Event>> &handler);

        void DetachAll();

    private:

        std::set<TReceiver<Event> *, std::less<>> handlers_;
        std::set<std::shared_ptr<TReceiver<Event>>, std::less<>> handlersWithOwnership_;
    };

    template<typename...Events>
    class TSender : public FUncopyable
    {
    public:

        template<typename Event>
        void Send(const Event &e) const;

        template<typename Event>
        void Attach(TReceiver<Event> *handler);

        template<typename Event>
        void Attach(std::shared_ptr<TReceiver<Event>> handlerWithOwnership);

        template<typename Event>
        void Detach(const TReceiver<Event> *handler);

        template<typename Event>
        void Detach(const std::shared_ptr<TReceiver<Event>> &handler);

        template<typename Event>
        void Detach(const std::shared_ptr<const TReceiver<Event>> &handler);

        template<typename Event>
        void DetachAll();

        void DetachAllTypes();

    private:

        std::tuple<TReceiverSet<Events>...> receiverSets_;
    };

    template<typename Event>
    class TFunctionalReceiver : public TReceiver<Event>
    {
    public:

        using Function = std::function<void(const Event &)>;

        explicit TFunctionalReceiver(std::function<void()> f);

        explicit TFunctionalReceiver(Function f = {});

        void SetFunction(std::function<void()> f);

        void SetFunction(Function f);

        void Handle(const Event &e) override;

    private:

        Function f_;
    };

    template<typename Event, typename Class>
    class TClassReceiver : public TReceiver<Event>
    {
    public:

        using MemberFunctionPointer = void(Class::*)(const Event &);

        TClassReceiver();

        TClassReceiver(Class *c, MemberFunctionPointer f);

        void SetClassInstance(Class *c);

        void SetMemberFunction(MemberFunctionPointer f);

        void Handle(const Event &e) override;

    private:

        Class c_;
        MemberFunctionPointer f_;
    };

    #define NEKO_DECLARE_EVENT_SENDER(Event)                                     \
        void Attach(Neko::OS::TReceiver<Event> *handler);                        \
        void Attach(std::shared_ptr<Neko::OS::TReceiver<Event>> handler);        \
        void Attach(std::function<void(const Event &)> f);                       \
        void Detach(Neko::OS::TReceiver<Event> *handler);                        \
        void Detach(const std::shared_ptr<Neko::OS::TReceiver<Event>> &handler); \
        void Detach(const std::shared_ptr<const Neko::OS::TReceiver<Event>> &handler);

    #define NEKO_DEFINE_EVENT_SENDER(Class, Sender, Event)                                                  \
        void Class::Attach(Neko::OS::TReceiver<Event> *handler)                                             \
            { Sender.Attach<Event>(handler); }                                                              \
        void Class::Attach(std::shared_ptr<Neko::OS::TReceiver<Event>> handler)                             \
            { Sender.Attach<Event>(std::move(handler)); }                                                   \
        void Class::Attach(std::function<void(const Event &)> f)                                            \
            { Sender.Attach<Event>(std::make_shared<Neko::OS::TFunctionalReceiver<Event>>(std::move(f))); } \
        void Class::Detach(Neko::OS::TReceiver<Event> *handler)                                             \
            { Sender.Detach<Event>(handler); }                                                              \
        void Class::Detach(const std::shared_ptr<Neko::OS::TReceiver<Event>> &handler)                      \
            { Sender.Detach<Event>(handler); }                                                              \
        void Class::Detach(const std::shared_ptr<const Neko::OS::TReceiver<Event>> &handler)                \
            { Sender.Detach<Event>(handler); }

    template<typename Event>
    TReceiverSet<Event>::~TReceiverSet()
    {
        DetachAll();
    }

    template<typename Event>
    void TReceiverSet<Event>::Send(const Event &e) const
    {
        for(auto h : handlers_)
            h->Handle(e);
        for(auto &h : handlersWithOwnership_)
            h->Handle(e);
    }

    template<typename Event>
    void TReceiverSet<Event>::Attach(TReceiver<Event> *handler)
    {
        assert(handler);
        handler->containedSets_.insert(this);
        handlers_.insert(handler);
    }

    template<typename Event>
    void TReceiverSet<Event>::Attach(std::shared_ptr<TReceiver<Event>> handlerWithOwnership)
    {
        assert(handlerWithOwnership);
        handlerWithOwnership->containedSetsWithOwnership_.insert(this);
        handlersWithOwnership_.insert(std::move(handlerWithOwnership));
    }

    template<typename Event>
    void TReceiverSet<Event>::Detach(const TReceiver<Event> *handler)
    {
        assert(handler);
        assert(handler->containedSets_.contains(this));
        assert(handlers_.contains(handler));
        handler->containedSets_.erase(this);
        handlers_.erase(handler);
    }

    template<typename Event>
    void TReceiverSet<Event>::Detach(const std::shared_ptr<TReceiver<Event>> &handler)
    {
        assert(handler);
        assert(handler->containedSetsWithOwnership_.contains(this));
        assert(handlersWithOwnership_.contains(handler));
        handler->containedSetsWithOwnership_.erase(this);
        handlersWithOwnership_.erase(handler);
    }

    template<typename Event>
    void TReceiverSet<Event>::Detach(const std::shared_ptr<const TReceiver<Event>> &handler)
    {
        assert(handler);
        assert(handler->containedSetsWithOwnership_.contains(this));
        assert(handlersWithOwnership_.contains(handler));
        handler->containedSetsWithOwnership_.erase(this);
        handlersWithOwnership_.erase(handler);
    }

    template<typename Event>
    void TReceiverSet<Event>::DetachAll()
    {
        while(!handlers_.empty())
            this->Detach(*handlers_.begin());
        while(!handlersWithOwnership_.empty())
            this->Detach(*handlersWithOwnership_.begin());
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::Send(const Event &e) const
    {
        std::get<TReceiverSet<Event>>(receiverSets_).Send(e);
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::Attach(TReceiver<Event> *handler)
    {
        std::get<TReceiverSet<Event>>(receiverSets_).Attach(handler);
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::Attach(std::shared_ptr<TReceiver<Event>> handlerWithOwnership)
    {
        std::get<TReceiverSet<Event>>(receiverSets_).Attach(std::move(handlerWithOwnership));
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::Detach(const TReceiver<Event> *handler)
    {
        std::get<TReceiverSet<Event>>(receiverSets_).Detach(handler);
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::Detach(const std::shared_ptr<TReceiver<Event>> &handler)
    {
        std::get<TReceiverSet<Event>>(receiverSets_).Detach(handler);
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::Detach(const std::shared_ptr<const TReceiver<Event>> &handler)
    {
        std::get<TReceiverSet<Event>>(receiverSets_).Detach(handler);
    }

    template<typename...Events>
    template<typename Event>
    void TSender<Events...>::DetachAll()
    {
        std::get<TReceiverSet<Event>>(receiverSets_).DetachAll();
    }

    template<typename...Events>
    void TSender<Events...>::DetachAllTypes()
    {
        ((std::get<TReceiverSet<Events>>(receiverSets_).DetachAll()), ...);
    }

    template<typename Event>
    TFunctionalReceiver<Event>::TFunctionalReceiver(std::function<void()> f)
    {
        SetFunction(std::move(f));
    }

    template<typename Event>
    TFunctionalReceiver<Event>::TFunctionalReceiver(Function f)
        : f_(std::move(f))
    {
        
    }

    template<typename Event>
    void TFunctionalReceiver<Event>::SetFunction(std::function<void()> f)
    {
        if(f)
            f_ = [nf = std::move(f)](const Event &){ nf(); };
        else
            f_ = {};
    }

    template<typename Event>
    void TFunctionalReceiver<Event>::SetFunction(Function f)
    {
        f_ = std::move(f);
    }

    template<typename Event>
    void TFunctionalReceiver<Event>::Handle(const Event &e)
    {
        if(f_)
            f_(e);
    }

    template<typename Event, typename Class>
    TClassReceiver<Event, Class>::TClassReceiver()
        : TClassReceiver({}, {})
    {
        
    }

    template<typename Event, typename Class>
    TClassReceiver<Event, Class>::TClassReceiver(Class *c, MemberFunctionPointer f)
        : c_(c), f_(f)
    {
        
    }

    template<typename Event, typename Class>
    void TClassReceiver<Event, Class>::SetClassInstance(Class *c)
    {
        c_ = c;
    }

    template<typename Event, typename Class>
    void TClassReceiver<Event, Class>::SetMemberFunction(MemberFunctionPointer f)
    {
        f_ = f;
    }

    template<typename Event, typename Class>
    void TClassReceiver<Event, Class>::Handle(const Event &e)
    {
        if(c_ && f_)
            (c_->*f_)(e);
    }

} // namespace Neko::OS
