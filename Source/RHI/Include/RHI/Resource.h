#pragma once
namespace Neko
{ 
    struct NativeObject
    {
        union
        {
            uint64_t handle;
            void *pointer;
        };

        NativeObject(uint64_t i) : handle(i) {}
        NativeObject(void *p) : pointer(p) {}

        template <typename T>
        operator T *() const { return static_cast<T *>(pointer); }
    };

    class RHIResource
    {
    public:
        RHIResource() = default;
        virtual ~RHIResource() = default;

        RHIResource(const RHIResource &) = delete;
        RHIResource(const RHIResource &&) = delete;
        RHIResource &operator=(const RHIResource &) = delete;
        RHIResource &operator=(const RHIResource &&) = delete;

        virtual unsigned long AddRef() = 0;
        virtual unsigned long Release() = 0;

        virtual NativeObject GetNativeObject() const { return nullptr; }
    };
}