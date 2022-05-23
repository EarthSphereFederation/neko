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

#define NEKO_PARAM_ARRAY(ParamType, Param, Size)     \
    static_vector<ParamType, Size> Param##Array;     \
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
        Graphic,
        Compute,
        Transfer,
        Count
    };

    enum class EShaderStage : uint8_t
    {
        VS = 0x1,
        PS = 0x2,

        All = VS | PS,
    };
    NEKO_ENUM_CLASS_FLAG_OPERATORS(EShaderStage)

    enum class EVertexRate : uint8_t
    {
        Vertex = 0x1,
        Instance
    };

    enum class EPrimitiveTopology : uint8_t
    {
        TriangleList = 0x1
    };

    enum class ECullMode : uint8_t
    {
        Back = 0x1,
        Front,
        None
    };

    enum class EFrontFace : uint8_t
    {
        CCW = 0x1,
        CW
    };

    enum class EPolygonMode : uint8_t
    {
        Fill = 0x1
    };

    enum class ESampleCount : uint8_t
    {
        SampleCount_1 = 0x1,
        SampleCount_2,
        SampleCount_4,
        SampleCount_8,
        SampleCount_16,
    };

    enum class EStencilOp : uint8_t
    {
        Keep = 0x1,
    };

    enum class ECompareOp : uint8_t
    {
        Never = 0x1,
        Less,
        Always,
    };

    enum class EBlendFactor : uint8_t
    {
        Zero = 0x1,
        One,
    };

    enum class EBlendOp : uint8_t
    {
        Add = 0x1,
    };

    enum class EColorComponent : uint8_t
    {
        None = 0x0,
        R = 0x1,
        G = 0x2,
        B = 0x4,
        A = 0x8,
        All = 0Xf
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
        NEKO_PARAM_ARRAY(FVertexAttribute, Attribute, MAX_VERTEX_ATTRIBUTE_COUNT);
        NEKO_PARAM_ARRAY(FVertexBinding, Binding, MAX_VERTEX_BINDING_COUNT);
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
        NEKO_PARAM_ARRAY(FBindingLayoutBinding, Binding, MAX_BINDINGS_PER_LAYOUT); // 256 bytes
        NEKO_PARAM_WITH_DEFAULT(EShaderStage, ShaderStage, EShaderStage::All);   // 1 byte
    };

    class IBindingLayout : public IResource
    {
    };
    typedef RefCountPtr<IBindingLayout> IBindingLayoutRef;

    struct FFrameBufferInfo
    {
        NEKO_PARAM_ARRAY(EFormat, Format, MAX_RENDER_TARGET_COUNT);
        NEKO_PARAM_ARRAY(ELoadOp, LoadAction, MAX_RENDER_TARGET_COUNT);
        NEKO_PARAM_ARRAY(EStoreOp, StoreAction, MAX_RENDER_TARGET_COUNT);

        NEKO_PARAM_WITH_DEFAULT(EFormat, DSFormat, EFormat::Undefined);
        NEKO_PARAM_WITH_DEFAULT(ELoadOp, DSLoadAction, ELoadOp::Load);
        NEKO_PARAM_WITH_DEFAULT(EStoreOp, DSStoreAction, EStoreOp::Store);
    };

    class IFrameBuffer : public IResource
    {
    public:
        virtual const FFrameBufferInfo& GetInfo() = 0;
    };

    typedef RefCountPtr<IFrameBuffer> IFrameBufferRef;

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
        NEKO_PARAM_ARRAY(IBindingLayoutRef, BindingLayout, MAX_BINDING_LAYOUT_COUNT);
    };

    class IGraphicPipeline : public IResource
    {
    };
    typedef RefCountPtr<IGraphicPipeline> IGraphicPipelineRef;
    
    struct FCmdListDesc
    {
        NEKO_PARAM_WITH_DEFAULT(ECmdQueueType, Type, ECmdQueueType::Graphic);
    };

    class ICmdList : public IResource
    {
    public:
        virtual void BeginCmd() = 0;
        virtual void EndCmd() = 0;

        virtual void SetViewport(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height, float MinDepth = 0.0f, float MaxDepth = 1.0f) = 0;
        virtual void SetScissor(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height) = 0;
        virtual void SetViewportNoScissor(uint32_t X, uint32_t Width, uint32_t Y, uint32_t Height, float MinDepth = 0.0f, float MaxDepth = 1.0f) = 0;
       
        virtual void Draw(uint32_t VertexNum, uint32_t VertexOffset, uint32_t InstanceNum, uint32_t InstanceOffset) = 0;
        virtual void BindFrameBuffer(IFrameBuffer *) = 0;
        virtual void BindGraphicPipeline(IGraphicPipeline*) = 0;
    };
    typedef RefCountPtr<ICmdList> ICmdListRef;

    struct FSwapChainDesc
    {
        OS::FWindow* WindowRawPtr;
        FSwapChainDesc& SetWindow(OS::FWindow* InWindowRawPtr)
        {
            WindowRawPtr = InWindowRawPtr;
            return *this;
        }
        NEKO_PARAM_WITH_DEFAULT(EFormat, Format, EFormat::Undefined);
        NEKO_PARAM_WITH_DEFAULT(bool, VSync, true);
    };

    class ISwapchain : public IResource
    {
    public:
        virtual IFrameBufferRef GetFrameBuffer(uint32_t) = 0;
    };
    typedef RefCountPtr<ISwapchain> ISwapchainRef;

    struct FFeatures
    {
        NEKO_PARAM_WITH_DEFAULT(bool, Swapchain, false);
    };

    struct FDeviceDesc
    {
        NEKO_PARAM_WITH_DEFAULT(bool, Validation, false);
        NEKO_PARAM_WITH_DEFAULT(uint32_t, GpuIndex, 0);
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
    
    class IDevice : public IResource
    {
    public:
        [[nodiscard]] virtual ICmdListRef CreateCmdList(const FCmdListDesc & = FCmdListDesc()) = 0;

        [[nodiscard]] virtual IShaderRef CreateShader(const FShaderDesc &) = 0;

        [[nodiscard]] virtual IGraphicPipelineRef CreateGraphicPipeline(const FGraphicPipelineDesc &, IFrameBuffer* const) = 0;

        [[nodiscard]] virtual IBindingLayoutRef CreateBindingLayout(const FBindingLayoutDesc &desc) = 0;

        [[nodiscard]] virtual ISwapchainRef CreateSwapChain(const FSwapChainDesc &desc) = 0;
        [[nodiscard]] virtual IFrameBufferRef QueueWaitNextFrameBuffer(ISwapchain*, const ECmdQueueType& CmdQueueType = ECmdQueueType::Graphic) = 0;
        [[nodiscard]] virtual void QueueWaitPresent(ISwapchain*, IFrameBuffer*, const ECmdQueueType& CmdQueueType = ECmdQueueType::Graphic) = 0;

        virtual void ExcuteCmdLists(ICmdList** CmdLists, uint32_t CmdListNum, const ECmdQueueType& CmdQueueType = ECmdQueueType::Graphic) = 0;
        virtual void ExcuteCmdList(ICmdList* CmdLists, const ECmdQueueType& CmdQueueType = ECmdQueueType::Graphic) = 0;

        virtual bool IsCmdQueueValid(const ECmdQueueType&) = 0;
        virtual void GC() = 0;

    };

    typedef RefCountPtr<IDevice> IDeviceRef;

    extern IDeviceRef CreateDevice(const FDeviceDesc &desc);
}