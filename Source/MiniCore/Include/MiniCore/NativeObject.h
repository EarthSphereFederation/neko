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
}