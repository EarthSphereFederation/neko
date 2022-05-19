#pragma once

#include <type_traits>

namespace Neko
{ 
    template <typename T>
    class RefCountPtr
    {
    public:
        typedef T element_type;

    protected:
        element_type *ptr;
        template <class U>
        friend class RefCountPtr;

        unsigned long InternalAddRef()
        {
            unsigned long ret = 0;
            if (ptr != nullptr)
            {
                ret = ptr->AddRef();
            }
            return ret;
        }

        unsigned long InternalRelease()
        {
            unsigned long ret = 0;
            if (ptr != nullptr)
            {
                ret = ptr->Release();
                ptr = nullptr;
            }
            return ret;
        }

        void Swap(RefCountPtr &other)
        {
            element_type *tmp = other.ptr;
            other.ptr = ptr;
            ptr = tmp;
        }

    public:
        RefCountPtr() noexcept : ptr(nullptr) {}
        RefCountPtr(std::nullptr_t) noexcept : ptr(nullptr) {}
        template <class U>
        RefCountPtr(U *other) : ptr(other)
        {
            InternalAddRef();
        }

        RefCountPtr(const RefCountPtr &other) noexcept : ptr(other.ptr)
        {
            InternalAddRef();
        }
        template <class U> requires std::is_convertible_v<U *, T *>
        RefCountPtr(const RefCountPtr<U> &other) noexcept : ptr(other.ptr)
        {
            InternalAddRef();
        }
        RefCountPtr(RefCountPtr &&other) noexcept : ptr(nullptr)
        {
            if (this != &other)
            {
                Swap(other);
            }
        }
        template <class U> requires std::is_convertible_v<U *, T *>
        RefCountPtr(RefCountPtr<U> &&other) noexcept : ptr(other.ptr)
        {
            other.ptr = nullptr;
        }

        RefCountPtr &operator=(T *other) noexcept
        {
            if (other != ptr)
            {
                RefCountPtr(other).Swap(*this);
            }
            return *this;
        }
        RefCountPtr &operator=(const RefCountPtr &other) noexcept
        {
            if (other.ptr != ptr)
            {
                RefCountPtr(other).Swap(*this);
            }
            return *this;
        }
        RefCountPtr &operator=(RefCountPtr &&other) noexcept
        {
            if (other.ptr != ptr)
            {
                RefCountPtr(other).Swap(*this);
            }
            return *this;
        }
        ~RefCountPtr() noexcept
        {
            InternalRelease();
        }

        element_type *operator->() const noexcept
        {
            return ptr;
        }

        bool IsValid() const noexcept
        {
            return ptr != nullptr;
        }

        operator bool()
        {
            return IsValid();
        }

        element_type *GetPtr() const { return ptr; }
    };

    template <typename T>
    class RefCounter : public T
    {
    private:
        std::atomic<unsigned long> refCount = 0;

    public:
        virtual unsigned long AddRef() override
        {
            return ++refCount;
        }
        virtual unsigned long Release() override
        {
            auto ret = --refCount;
            if (ret == 0)
            {
                delete this;
            }
            return ret;
        }
    };
}