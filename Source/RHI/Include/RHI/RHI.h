#pragma once
#include <atomic>
#include <memory>
#include <cassert>
#include "MiniCore/RefCounter.h"
#include "MiniCore/Container.h"
#include "OS/Window.h"
#include "Resource.h"
namespace Neko::RHI
{ 
#define CHECK(result) assert(result);
#define CHECK_F(result, fmt, ...) \
    printf(fmt, __VA_ARGS__);     \
    assert(result);
#define BIT(N) 1 << N
#define NEKO_ENUM_CLASS_FLAG_OPERATORS(T)                                 \
    inline T operator|(T a, T b) { return T(uint32_t(a) | uint32_t(b)); } \
    inline T operator&(T a, T b) { return T(uint32_t(a) & uint32_t(b)); } \
    inline T operator~(T a) { return T(~uint32_t(a)); }                   \
    inline bool operator!(T a) { return uint32_t(a) == 0; }               \
    inline bool operator==(T a, uint32_t b) { return uint32_t(a) == b; }  \
    inline bool operator!=(T a, uint32_t b) { return uint32_t(a) != b; }

#define NEKO_PARAM_WITH_DEFAULT(ParamType, Param, Default) \
    ParamType Param = Default;                             \
    using _rhi##Param##Type = ParamType;                   \
    auto &Set##Param(const _rhi##Param##Type &value)       \
    {                                                      \
        Param = value;                                     \
        return *this;                                      \
    }

#define NEKO_PARAM_STATIC_ARRAY(ParamType, Param, Size)     \
    static_vector<ParamType, Size> Param##Array;     \
    using _rhi##Param##Type = ParamType;             \
    auto &Add##Param(const _rhi##Param##Type &value) \
    {                                                \
        Param##Array.push_back(value);               \
        return *this;                                \
    }

#define NEKO_PARAM_DYNAMIC_ARRAY(ParamType, Param)     \
    std::vector<ParamType> Param##Array;     \
    using _rhi##Param##Type = ParamType;             \
    auto &Add##Param(const _rhi##Param##Type &value) \
    {                                                \
        Param##Array.push_back(value);               \
        return *this;                                \
    }

#define NEKO_PARAM_ARRAY_PRI_PARAM_PUB_FUNC(ParamType, Param, Size)     \
    private:                                         \
    static_vector<ParamType, Size> Param##Array;     \
    using _rhi##Param##Type = ParamType;             \
    public:                                          \
    auto &Add##Param(const _rhi##Param##Type &value) \
    {                                                \
        Param##Array.push_back(value);               \
        return *this;                                \
    }

    constexpr uint32_t MAX_RENDER_TARGET_COUNT = 8;
    constexpr uint32_t MAX_VERTEX_ATTRIBUTE_COUNT = 15;
    constexpr uint32_t MAX_VERTEX_BINDING_COUNT = 15;
    constexpr uint32_t MAX_BINDING_LAYOUT_COUNT = 5;
    constexpr uint32_t MAX_BINDINGS_PER_LAYOUT = 128;
    constexpr uint32_t MAX_SHADER_STAGE_COUNT = 2; // vs,ps
    
    enum class EFormat : uint8_t
    {
        B8G8R8A8_SNORM,
        B8G8R8A8_UNORM,
        Undefined
    };

    enum class ECmdQueueType : uint8_t
    {
        Undefined = 0x0,
        Graphic  = BIT(0),
        Compute  = BIT(1),
        Transfer = BIT(2)
    };

    NEKO_ENUM_CLASS_FLAG_OPERATORS(ECmdQueueType);

    enum class EShaderStage : uint8_t
    {
        Vertex = BIT(0),
        Pixel  = BIT(1),

        All = Vertex | Pixel,
    };
    NEKO_ENUM_CLASS_FLAG_OPERATORS(EShaderStage)

    enum class EVertexRate : uint8_t
    {
        Vertex,
        Instance
    };

    enum class EPrimitiveTopology : uint8_t
    {
        TriangleList 
    };

    enum class ECullMode : uint8_t
    {
        Back,
        Front,
        None
    };

    enum class EFrontFace : uint8_t
    {
        CCW,
        CW
    };

    enum class EPolygonMode : uint8_t
    {
        Fill = 0x1
    };

    enum class ESampleCount : uint8_t
    {
        SampleCount_1,
        SampleCount_2,
        SampleCount_4,
        SampleCount_8,
        SampleCount_16,
    };

    enum class EStencilOp : uint8_t
    {
        Keep,
    };

    enum class ECompareOp : uint8_t
    {
        Never,
        Less,
        Always,
    };

    enum class EBlendFactor : uint8_t
    {
        Zero,
        One,
    };

    enum class EBlendOp : uint8_t
    {
        Add,
    };

    enum class EColorComponent : uint8_t
    {
        None = 0x0,
        R = BIT(0),
        G = BIT(1),
        B = BIT(2),
        A = BIT(3),
        All = R|G|B|A
    };

    NEKO_ENUM_CLASS_FLAG_OPERATORS(EColorComponent)

    enum class EResourceType : uint8_t
    {
        UniformBuffer
    };

    enum class ELoadOp : uint8_t
    {
        Load
    };

    enum class EStoreOp : uint8_t
    {
        Store
    };

    enum class ESemaphoreType : uint8_t
    {
        Binary,
        Timeline
    };

    enum class EFenceFlag : uint8_t
    {
        Signal,
        Unsignal
    };

    enum class ETextureType : uint8_t
    {
        Texture1D,
        Texture2D,
        Texture3D,
    };

    enum class ETextureUsage : uint8_t
    {
        ShaderResource  = BIT(0),
        UnorderedAccess = BIT(1),
        RenderTarget    = BIT(2)
    };
    NEKO_ENUM_CLASS_FLAG_OPERATORS(ETextureUsage);

    enum class EResourceState : uint16_t
    {
        Undefined    = BIT(0),
        RenderTarget = BIT(1),
        Present      = BIT(2),
    };
    NEKO_ENUM_CLASS_FLAG_OPERATORS(EResourceState);

    struct FTextureDesc
    {
        NEKO_PARAM_WITH_DEFAULT(ETextureType, TextureType, ETextureType::Texture2D);
        NEKO_PARAM_WITH_DEFAULT(ETextureUsage, TextureUsage, ETextureUsage::ShaderResource);
        NEKO_PARAM_WITH_DEFAULT(EFormat, Format, EFormat::B8G8R8A8_SNORM);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, Width, 1);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, Height, 1);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, Depth, 1);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, MipNum, 1);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, ArraySize, 1);
    };

    class ITexture : public IResource
    {
    public:
        virtual const FTextureDesc& GetDesc() = 0;
    };
    typedef RefCountPtr<ITexture> ITextureRef;

    struct FSubResourceRange
    {
        NEKO_PARAM_WITH_DEFAULT(uint16_t, MipOffset, 0);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, MipNum, 1);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, ArrayOffset, 0);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, ArraySize, 1);
    };

    struct FTexture2DViewDesc
    {
        NEKO_PARAM_WITH_DEFAULT(ITexture*, Texture, nullptr);
        NEKO_PARAM_WITH_DEFAULT(EFormat, Format, EFormat::B8G8R8A8_SNORM);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, MipOffset, 0);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, MipNum, 1);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, ArrayOffset, 0);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, ArraySize, 1);
    };

    class ITexture2DView : public IResource
    {
    public:
        virtual ITextureRef GetTexture() = 0;
        virtual const FTexture2DViewDesc& GetDesc() = 0;
    };
    typedef RefCountPtr<ITexture2DView> ITexture2DViewRef;

    struct FRenderTargetDesc
    {
        NEKO_PARAM_WITH_DEFAULT(ITexture*, Texture, nullptr);
        NEKO_PARAM_WITH_DEFAULT(EFormat, Format, EFormat::B8G8R8A8_SNORM);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, MipOffset, 0);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, MipNum, 1);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, ArrayOffset, 0);
        NEKO_PARAM_WITH_DEFAULT(uint16_t, ArraySize, 1);
        NEKO_PARAM_WITH_DEFAULT(ELoadOp, LoadAction, ELoadOp::Load);
        NEKO_PARAM_WITH_DEFAULT(EStoreOp, StoreAction, EStoreOp::Store);
    };

    struct IRenderTarget : public IResource
    {
    public:
        virtual const FRenderTargetDesc& GetDesc() = 0;
    };
    typedef RefCountPtr<IRenderTarget> IRenderTargetRef;

    struct FTextureTransitionDesc
    {
        NEKO_PARAM_WITH_DEFAULT(ITexture*, Texture, nullptr);
        NEKO_PARAM_WITH_DEFAULT(EResourceState, SrcState, EResourceState::RenderTarget);
        NEKO_PARAM_WITH_DEFAULT(EResourceState, DestState, EResourceState::Present);
        NEKO_PARAM_WITH_DEFAULT(FSubResourceRange, Range, FSubResourceRange());
    };

    class IFence : public IResource
    {
    public:
        virtual void Wait() = 0;
        virtual void Reset() = 0;
    };
    typedef RefCountPtr<IFence> IFenceRef;
    
    class ISemaphore : public IResource
    {
    public:
        virtual void SetCounter(float) = 0;
        virtual float GetCounter() = 0;
    };
    typedef RefCountPtr<ISemaphore> ISemaphoreRef;

    struct FShaderDesc
    {
        NEKO_PARAM_WITH_DEFAULT(const char *, DebugName, "");
        NEKO_PARAM_WITH_DEFAULT(const char *, Blob, "");
        NEKO_PARAM_WITH_DEFAULT(const char *, EntryPoint, "");
        NEKO_PARAM_WITH_DEFAULT(EShaderStage, Stage, EShaderStage::All);
        NEKO_PARAM_WITH_DEFAULT(uint32_t, Size, 0);
    };

    class IShader : public IResource
    {
    public:
        virtual const FShaderDesc &GetDesc() = 0;
    };
    typedef RefCountPtr<IShader> IShaderRef;

    struct FVertexAttribute
    {
        NEKO_PARAM_WITH_DEFAULT(EFormat, Format, EFormat::Undefined);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Binding, 0);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Location, 0);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Offset, 0);
    };

    struct FVertexBinding
    {
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Binding, 0);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Stride, 0);
        NEKO_PARAM_WITH_DEFAULT(EVertexRate, VertexRate, EVertexRate::Vertex);
    };

    struct FVertexInputLayout
    {
        NEKO_PARAM_STATIC_ARRAY(FVertexAttribute, Attribute, MAX_VERTEX_ATTRIBUTE_COUNT);
        NEKO_PARAM_STATIC_ARRAY(FVertexBinding, Binding, MAX_VERTEX_BINDING_COUNT);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, AttributeCount, 0);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Unused, 0);
    };

    struct FRasterSate
    {
        NEKO_PARAM_WITH_DEFAULT(ECullMode, CullMode, ECullMode::Back);
        NEKO_PARAM_WITH_DEFAULT(EPolygonMode, PolygonMode, EPolygonMode::Fill);
        NEKO_PARAM_WITH_DEFAULT(EFrontFace, FrontFace, EFrontFace::CCW);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Unused, 0);
    };

    struct FDepthStencilState
    {
        struct FStencilState
        {
            NEKO_PARAM_WITH_DEFAULT(EStencilOp, FailOp, EStencilOp::Keep);
            NEKO_PARAM_WITH_DEFAULT(EStencilOp, DepthFailOp, EStencilOp::Keep);
            NEKO_PARAM_WITH_DEFAULT(EStencilOp, PassOp, EStencilOp::Keep);
            NEKO_PARAM_WITH_DEFAULT(ECompareOp, StencilCompareOp, ECompareOp::Always);
        };

        NEKO_PARAM_WITH_DEFAULT(bool, DepthTest, true);
        NEKO_PARAM_WITH_DEFAULT(bool, DepthWrite, true);
        NEKO_PARAM_WITH_DEFAULT(ECompareOp, DepthCompareOp, ECompareOp::Less);
        NEKO_PARAM_WITH_DEFAULT(bool, StencilTest, false);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, ReadMask, 0xff);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, WriteMask, 0xff);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, StencilRef, 0u);
        NEKO_PARAM_WITH_DEFAULT(FStencilState, FrontStencil, FStencilState());
        NEKO_PARAM_WITH_DEFAULT(FStencilState, BackStencil, FStencilState());
    };

    struct FBlendState
    {
        struct FRenderTarget
        {
            NEKO_PARAM_WITH_DEFAULT(bool, BlendEnable, false);
            NEKO_PARAM_WITH_DEFAULT(EBlendFactor, SrcColor, EBlendFactor::One);
            NEKO_PARAM_WITH_DEFAULT(EBlendFactor, DestColor, EBlendFactor::Zero);
            NEKO_PARAM_WITH_DEFAULT(EBlendOp, ColorOp, EBlendOp::Add);
            NEKO_PARAM_WITH_DEFAULT(EBlendFactor, SrcAlpha, EBlendFactor::One);
            NEKO_PARAM_WITH_DEFAULT(EBlendFactor, DestAlpha, EBlendFactor::Zero);
            NEKO_PARAM_WITH_DEFAULT(EBlendOp, AlphaOp, EBlendOp::Add);
            NEKO_PARAM_WITH_DEFAULT(EColorComponent, WriteMask, EColorComponent::All);
        };

        FRenderTarget renderTargets[MAX_RENDER_TARGET_COUNT];
        FBlendState &SetRenderTarget(uint32_t index, const FRenderTarget &target)
        {
            renderTargets[index] = target;
            return *this;
        }
    };

    struct FBindingLayoutBinding
    {
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Binding, 0);
        NEKO_PARAM_WITH_DEFAULT(EResourceType, ResourceType, EResourceType::UniformBuffer);
    };

    struct FBindingLayoutDesc
    {
        NEKO_PARAM_STATIC_ARRAY(FBindingLayoutBinding, Binding, MAX_BINDINGS_PER_LAYOUT); // 256 bytes
        NEKO_PARAM_WITH_DEFAULT(EShaderStage, ShaderStage, EShaderStage::All);   // 1 byte
    };

    class IBindingLayout : public IResource
    {
    };
    typedef RefCountPtr<IBindingLayout> IBindingLayoutRef;

    struct FGraphicPipelineDesc
    {
        NEKO_PARAM_WITH_DEFAULT(EPrimitiveTopology, PrimitiveTopology, EPrimitiveTopology::TriangleList);
        NEKO_PARAM_WITH_DEFAULT(ESampleCount, SampleCount, ESampleCount::SampleCount_1);

        NEKO_PARAM_WITH_DEFAULT(IShaderRef, VertexShader, IShaderRef());
        NEKO_PARAM_WITH_DEFAULT(IShaderRef, PixelShader, IShaderRef());

        NEKO_PARAM_WITH_DEFAULT(FVertexInputLayout, VertexInputLayout, FVertexInputLayout());
        NEKO_PARAM_WITH_DEFAULT(FRasterSate, RasterState, FRasterSate());
        NEKO_PARAM_WITH_DEFAULT(FDepthStencilState, DepthStencilState, FDepthStencilState());
        NEKO_PARAM_WITH_DEFAULT(FBlendState, BlendState, FBlendState());
        NEKO_PARAM_STATIC_ARRAY(IBindingLayoutRef, BindingLayout, MAX_BINDING_LAYOUT_COUNT);

        NEKO_PARAM_STATIC_ARRAY(FRenderTargetDesc, ColorRenderTargetDesc, MAX_RENDER_TARGET_COUNT);
    };

    class IGraphicPipeline : public IResource
    {
    };
    typedef RefCountPtr<IGraphicPipeline> IGraphicPipelineRef;

    struct FRenderPassDesc
    {
        NEKO_PARAM_STATIC_ARRAY(IRenderTargetRef, ColorRenderTarget, MAX_RENDER_TARGET_COUNT);
    };

    class ICmdList : public IResource
    {
    public:
        virtual void BeginCmd() = 0;
        virtual void EndCmd() = 0;

        virtual void BeginRenderPass(const FRenderPassDesc&) = 0;
        virtual void EndRenderPass() = 0;

        virtual void SetViewport(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height, float MinDepth = 0.0f, float MaxDepth = 1.0f) = 0;
        virtual void SetScissor(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height) = 0;
        virtual void SetViewportNoScissor(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height, float MinDepth = 0.0f, float MaxDepth = 1.0f) = 0;
       
        virtual void Draw(uint32_t VertexNum, uint32_t VertexOffset, uint32_t InstanceNum, uint32_t InstanceOffset) = 0;
        virtual void BindGraphicPipeline(IGraphicPipeline*) = 0;

        virtual void ResourceBarrier(const FTextureTransitionDesc&) = 0;
    };
    typedef RefCountPtr<ICmdList> ICmdListRef;

    class ICmdPool : public IResource
    {
    private:
    public:
        [[nodiscard]] virtual ICmdListRef CreateCmdList() = 0;
        virtual void Free() = 0;
        
        virtual ECmdQueueType GetCmdQueueType() = 0;
    };
    typedef RefCountPtr<ICmdPool> ICmdPoolRef;
    
    struct FExcuteDesc
    {
        NEKO_PARAM_DYNAMIC_ARRAY(ISemaphore*,WaitSemaphore);
        NEKO_PARAM_DYNAMIC_ARRAY(ISemaphore*, SignalSemaphore);
        NEKO_PARAM_WITH_DEFAULT(IFence*, Fence, nullptr);
    };

    class IQueue : public IResource
    {
    public:
        [[nodiscard]] virtual ICmdPoolRef CreateCmdPool() = 0;
        [[nodiscard]] virtual std::vector<ICmdPoolRef> CreateCmdPools(uint32_t) = 0;

        virtual void ExcuteCmdLists(ICmdList** CmdLists, uint32_t CmdListNum, const FExcuteDesc& Desc) = 0;
        virtual void ExcuteCmdList(ICmdList* CmdList, const FExcuteDesc& Desc) = 0;
    };
    typedef RefCountPtr<IQueue> IQueueRef;

    struct FPresentDesc
    {
        NEKO_PARAM_WITH_DEFAULT(uint32_t, PresentIndex, 0);
        NEKO_PARAM_WITH_DEFAULT(IQueue*, Queue, nullptr);
        NEKO_PARAM_DYNAMIC_ARRAY(ISemaphore*, WaitSemaphore);
    };

    struct FSwapChainDesc
    {
        OS::FWindow* WindowRawPtr;
        FSwapChainDesc& SetWindow(OS::FWindow* InWindowRawPtr)
        {
            WindowRawPtr = InWindowRawPtr;
            return *this;
        }
        NEKO_PARAM_WITH_DEFAULT(uint32_t, ImageCount, 3);
        NEKO_PARAM_WITH_DEFAULT(EFormat, Format, EFormat::Undefined);
        NEKO_PARAM_WITH_DEFAULT(bool, VSync, true);
    };
    
    class ISwapchain : public IResource
    {
    public:
        virtual uint32_t AcquireNext(ISemaphore*,IFence*) = 0;
        virtual void Present(const FPresentDesc&) = 0;
        virtual uint32_t GetTextureNum() = 0;
        virtual std::vector<ITextureRef> GetTextures() = 0;
    };
    typedef RefCountPtr<ISwapchain> ISwapchainRef;

    struct FFeatures
    {
        NEKO_PARAM_WITH_DEFAULT(bool, Swapchain, false);
    };

    struct FDeviceDesc
    {
        NEKO_PARAM_WITH_DEFAULT(bool, Validation, false);
        NEKO_PARAM_WITH_DEFAULT(FFeatures, Features, FFeatures());

        struct FVulkanDesc
        {
            const char **InstanceExtensions = nullptr;
            uint32_t InstanceExtensionNum = 0;
            FVulkanDesc &SetInstanceExtensions(const char **extensions, uint32_t num)
            {
                InstanceExtensions = extensions;
                InstanceExtensionNum = num;
                return *this;
            }
        };
        NEKO_PARAM_WITH_DEFAULT(FVulkanDesc, VulkanDesc, FVulkanDesc());
    };

    struct FGPUInfo
    {
        const char* Name;
    };
    
    class IDevice : public IResource
    {
    public:

        [[nodiscard]] virtual ISemaphoreRef CreateSemaphore(const ESemaphoreType&) = 0;
        [[nodiscard]] virtual std::vector<ISemaphoreRef> CreateSemaphores(const ESemaphoreType&, uint32_t) = 0;
        [[nodiscard]] virtual IFenceRef CreateFence(const EFenceFlag& = EFenceFlag::Signal) = 0;
        [[nodiscard]] virtual std::vector<IFenceRef> CreateFences(const EFenceFlag&, uint32_t) = 0;
        [[nodiscard]] virtual IQueueRef CreateQueue(const ECmdQueueType& CmdQueueType = ECmdQueueType::Graphic) = 0;
        [[nodiscard]] virtual IShaderRef CreateShader(const FShaderDesc &) = 0;
        [[nodiscard]] virtual IGraphicPipelineRef CreateGraphicPipeline(const FGraphicPipelineDesc &) = 0;
        [[nodiscard]] virtual ISwapchainRef CreateSwapChain(const FSwapChainDesc&) = 0;
        [[nodiscard]] virtual ITexture2DViewRef CreateTexture2DView(const FTexture2DViewDesc&) = 0;
        [[nodiscard]] virtual ITexture2DViewRef CreateTexture2DView(ITexture*) = 0;
        [[nodiscard]] virtual IRenderTargetRef CreateRenderTarget(const FRenderTargetDesc&) = 0;
        

        [[nodiscard]] virtual IBindingLayoutRef CreateBindingLayout(const FBindingLayoutDesc &desc) = 0;

        
        
        virtual bool IsCmdQueueValid(const ECmdQueueType&) = 0;

        virtual void WaitIdle() = 0;
        virtual FGPUInfo GetGPUInfo() = 0;
    };

    typedef RefCountPtr<IDevice> IDeviceRef;

    extern bool GRHIInitalize;
    extern void RHIInit();
    extern IDeviceRef CreateDevice(const FDeviceDesc &desc);
}