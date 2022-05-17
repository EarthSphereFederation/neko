#pragma once
#include <atomic>
#include <vector>
#include <memory>
#include <list>
#include <assert.h>
#include "Resource.h"
#include "Container.h"
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
        Graphic = 0x1,
        Compute,
        Transfer
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

    struct RHIShaderDesc
    {
        NEKO_PARAM_WITH_DEFAULT(const char *, DebugName, "");
        NEKO_PARAM_WITH_DEFAULT(const char *, Blob, "");
        NEKO_PARAM_WITH_DEFAULT(const char *, EntryPoint, "");
        NEKO_PARAM_WITH_DEFAULT(EShaderStage, Stage, EShaderStage::All);
        NEKO_PARAM_WITH_DEFAULT(uint32_t, Size, 0);
    };

    class RHIShader : public RHIResource
    {
    public:
        virtual const RHIShaderDesc &GetDesc() const = 0;
    };
    typedef RefCountPtr<RHIShader> RHIShaderRef;

    struct RHIVertexAttribute
    {
        NEKO_PARAM_WITH_DEFAULT(EFormat, Format, EFormat::Undefined);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Binding, 0);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Location, 0);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Offset, 0);
    };

    struct RHIVertexBinding
    {
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Binding, 0);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Stride, 0);
        NEKO_PARAM_WITH_DEFAULT(EVertexRate, VertexRate, EVertexRate::Vertex);
    };

    struct RHIVertexInputLayout
    {
        NEKO_PARAM_ARRAY(RHIVertexAttribute, Attribute, MAX_VERTEX_ATTRIBUTE_COUNT);
        NEKO_PARAM_ARRAY(RHIVertexBinding, Binding, MAX_VERTEX_BINDING_COUNT);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, AttributeCount, 0);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Unused, 0);
    };

    struct RHIRasterSate
    {
        NEKO_PARAM_WITH_DEFAULT(ECullMode, CullMode, ECullMode::Back);
        NEKO_PARAM_WITH_DEFAULT(EPolygonMode, PolygonMode, EPolygonMode::Fill);
        NEKO_PARAM_WITH_DEFAULT(EFrontFace, FrontFace, EFrontFace::CCW);
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Unused, 0);
    };

    struct RHIDepthStencilState
    {
        struct RHIStencilState
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
        NEKO_PARAM_WITH_DEFAULT(RHIStencilState, FrontStencil, RHIStencilState());
        NEKO_PARAM_WITH_DEFAULT(RHIStencilState, BackStencil, RHIStencilState());
    };

    struct RHIBlendState
    {
        struct RHIRenderTarget
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

        RHIRenderTarget renderTargets[MAX_RENDER_TARGET_COUNT];
        RHIBlendState &SetRenderTarget(uint32_t index, const RHIRenderTarget &target)
        {
            renderTargets[index] = target;
            return *this;
        }
    };

    struct RHIBindingLayoutBinding
    {
        NEKO_PARAM_WITH_DEFAULT(uint8_t, Binding, 0);
        NEKO_PARAM_WITH_DEFAULT(EResourceType, ResourceType, EResourceType::UniformBuffer);
    };

    struct RHIBindingLayoutDesc
    {
        NEKO_PARAM_ARRAY(RHIBindingLayoutBinding, Binding, MAX_BINDINGS_PER_LAYOUT); // 256 bytes
        NEKO_PARAM_WITH_DEFAULT(EShaderStage, ShaderStage, EShaderStage::All);   // 1 byte
    };

    class RHIBindingLayout : public RHIResource
    {
    };
    typedef RefCountPtr<RHIBindingLayout> RHIBindingLayoutRef;

    struct RHIFrameBufferInfo
    {
        NEKO_PARAM_ARRAY(EFormat, Format, MAX_RENDER_TARGET_COUNT);
        NEKO_PARAM_ARRAY(ELoadOp, LoadAction, MAX_RENDER_TARGET_COUNT);
        NEKO_PARAM_ARRAY(EStoreOp, StoreAction, MAX_RENDER_TARGET_COUNT);

        NEKO_PARAM_WITH_DEFAULT(EFormat, DSFormat, EFormat::Undefined);
        NEKO_PARAM_WITH_DEFAULT(ELoadOp, DSLoadAction, ELoadOp::Load);
        NEKO_PARAM_WITH_DEFAULT(EStoreOp, DSStoreAction, EStoreOp::Store);
    };

    class RHIFrameBuffer : public RHIResource
    {
    public:
        virtual const RHIFrameBufferInfo& GetInfo() const = 0;
    };

    typedef RefCountPtr<RHIFrameBuffer> RHIFrameBufferRef;

    struct RHIGraphicPipelineDesc
    {
        NEKO_PARAM_WITH_DEFAULT(EPrimitiveTopology, PrimitiveTopology, EPrimitiveTopology::TriangleList);
        NEKO_PARAM_WITH_DEFAULT(ESampleCount, SampleCount, ESampleCount::SampleCount_1);

        NEKO_PARAM_WITH_DEFAULT(RHIShaderRef, VertexShader, RHIShaderRef());
        NEKO_PARAM_WITH_DEFAULT(RHIShaderRef, PixelShader, RHIShaderRef());

        NEKO_PARAM_WITH_DEFAULT(RHIVertexInputLayout, VertexInputLayout, RHIVertexInputLayout());
        NEKO_PARAM_WITH_DEFAULT(RHIRasterSate, RasterState, RHIRasterSate());
        NEKO_PARAM_WITH_DEFAULT(RHIDepthStencilState, DepthStencilState, RHIDepthStencilState());
        NEKO_PARAM_WITH_DEFAULT(RHIBlendState, BlendState, RHIBlendState());
        NEKO_PARAM_ARRAY(RHIBindingLayoutRef, BindingLayout, MAX_BINDING_LAYOUT_COUNT);
    };

    class RHIGraphicPipeline : public RHIResource
    {
    };
    typedef RefCountPtr<RHIGraphicPipeline> RHIGraphicPipelineRef;

    struct RHICmdListDesc
    {
        NEKO_PARAM_WITH_DEFAULT(ECmdQueueType, type, ECmdQueueType::Graphic);
    };

    class RHICmdList : public RHIResource
    {
    };
    typedef RefCountPtr<RHICmdList> RHICmdListRef;

    struct RHISwapChainDesc
    {
        NEKO_PARAM_WITH_DEFAULT(NativeObject, Surface, nullptr);
        NEKO_PARAM_WITH_DEFAULT(EFormat, Format, EFormat::Undefined);
        NEKO_PARAM_WITH_DEFAULT(bool, VSync, true);
    };

    class RHISwapchain : public RHIResource
    {
    public:
        virtual RHIFrameBufferRef GetFrameBuffer(uint32_t) const = 0;
    };
    typedef RefCountPtr<RHISwapchain> RHISwapchainRef;

    struct RHIFeatures
    {
        NEKO_PARAM_WITH_DEFAULT(bool, Swapchain, false);
    };

    struct RHIDeviceDesc
    {
        NEKO_PARAM_WITH_DEFAULT(bool, Validation, false);
        NEKO_PARAM_WITH_DEFAULT(uint32_t, GpuIndex, 0);
        NEKO_PARAM_WITH_DEFAULT(RHIFeatures, Features, RHIFeatures());

#ifdef NEKO_VULKAN
        struct RHIVulkanDesc
        {
            const char **InstanceExtensions = nullptr;
            uint32_t InstanceExtensionNum = 0;
            RHIVulkanDesc &SetInstanceExtensions(const char **extensions, uint32_t num)
            {
                InstanceExtensions = extensions;
                InstanceExtensionNum = num;
                return *this;
            }
        };
        NEKO_PARAM_WITH_DEFAULT(RHIVulkanDesc, VulkanDesc, RHIVulkanDesc());
#endif
    };

    class RHIDevice : public RHIResource
    {

    public:
        [[nodiscard]] virtual RHICmdListRef CreateCmdList(const RHICmdListDesc & = RHICmdListDesc()) const = 0;

        [[nodiscard]] virtual RHIShaderRef CreateShader(const RHIShaderDesc &) const = 0;

        [[nodiscard]] virtual RHIGraphicPipelineRef CreateGraphicPipeline(const RHIGraphicPipelineDesc &, const RHIFrameBufferRef &) const = 0;

        [[nodiscard]] virtual RHIBindingLayoutRef CreateBindingLayout(const RHIBindingLayoutDesc &desc) const = 0;

        [[nodiscard]] virtual RHISwapchainRef CreateSwapChain(const RHISwapChainDesc &desc) const = 0;

    public:
#ifdef NEKO_VULKAN
        [[nodiscard]] virtual NativeObject GetVkInstance() const = 0;
#endif
    };

    typedef RefCountPtr<RHIDevice> RHIDeviceRef;

    extern RHIDeviceRef CreateDevice(const RHIDeviceDesc &desc);
}