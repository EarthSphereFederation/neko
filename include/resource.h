#pragma once
namespace neko
{
    template<typename T>
    class RefCountPtr
    {
    public:
        typedef T element_type;

    protected:
        element_type* ptr;
        template<class U> friend class RefCountPtr;

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

        void Swap(RefCountPtr& other)
        {
            element_type* tmp = other.ptr;
            other.ptr = ptr;
            ptr = tmp;
        }

        void Swap(RefCountPtr&& other)
        {
            element_type* tmp = other.ptr;
            other.ptr = ptr;
            ptr = tmp;
        }

    public:
        RefCountPtr() noexcept : ptr(nullptr) {}
        RefCountPtr(std::nullptr_t) noexcept :ptr(nullptr) {}
        template<class U>
        RefCountPtr(U* other) :ptr(other)
        {
            InternalAddRef();
        }
    
        RefCountPtr(const RefCountPtr& other) noexcept :ptr(other.ptr)
        {
            InternalAddRef();
        }
        template<class U>
        RefCountPtr(const RefCountPtr<U>& other, typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type* = nullptr) noexcept :ptr(other.ptr)
        {
            InternalAddRef();
        }
        RefCountPtr(RefCountPtr&& other) noexcept :ptr(nullptr)
        {
            if (this != reinterpret_cast<RefCountPtr*>(&reinterpret_cast<unsigned char&>(other)))
            {
                Swap(other);
            }   
        }
        template<class U>
        RefCountPtr(RefCountPtr<U>&& other, typename std::enable_if<std::is_convertible<U*, T*>::value, void*>::type* = nullptr) noexcept : ptr(other.ptr)
        {
            other.ptr = nullptr;
        }

        RefCountPtr& operator=(T* other) noexcept
        {
            if (other != ptr)
            {
                RefCountPtr(other).Swap(*this);
            }
            return *this;
        }
        RefCountPtr& operator=(const RefCountPtr& other) noexcept
        {
            if (other.ptr != ptr)
            {
                RefCountPtr(other).Swap(*this);
            }
            return *this;
        }
        RefCountPtr& operator=(const RefCountPtr&& other) noexcept
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

        element_type* operator->() const noexcept
        {
            return ptr;
        }

        bool IsValid() const noexcept
        {
            return ptr!= nullptr;
        }

        operator bool()
        {
            return IsValid();
        }

        element_type* GetPtr() const { return ptr; }
    };

    template<typename T>
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

    struct NativeObject
    {
        union {
            uint64_t handle;
            void* pointer;
        };

        NativeObject(uint64_t i) : handle(i) { }  
        NativeObject(void* p) : pointer(p) { }  

        template<typename T> operator T* () const { return static_cast<T*>(pointer); }
    };

    class RHIResource
    {
    public:
        RHIResource() = default;
        virtual ~RHIResource() = default;

        RHIResource(const RHIResource&) = delete;
        RHIResource(const RHIResource&&) = delete;
        RHIResource& operator=(const RHIResource&) = delete;
        RHIResource& operator=(const RHIResource&&) = delete;

        virtual unsigned long AddRef() = 0;
        virtual unsigned long Release() = 0;

        virtual NativeObject GetNativeObject() const { return nullptr; }
    };
}