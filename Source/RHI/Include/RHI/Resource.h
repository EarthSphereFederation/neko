#pragma once
namespace Neko
{ 
    class IResource
    {
    public:
        IResource() = default;
        virtual ~IResource() = default;

        IResource(const IResource &) = delete;
        IResource(const IResource &&) = delete;
        IResource &operator=(const IResource &) = delete;
        IResource &operator=(const IResource &&) = delete;

        virtual unsigned long AddRef() = 0;
        virtual unsigned long Release() = 0;
    };
}