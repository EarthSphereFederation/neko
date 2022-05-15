#pragma once
#include <atomic>
#include <vector>
#include <memory>
#include <list>
#include <assert.h>
#include "resource.h"
#include "container.h"
namespace neko
{
#define CHECK(result) assert(result);
#define CHECK_F(result,fmt,...) printf(fmt,__VA_ARGS__);assert(result);

#define NEKO_ENUM_CLASS_FLAG_OPERATORS(T) \
    inline T operator | (T a, T b) { return T(uint32_t(a) | uint32_t(b)); } \
    inline T operator & (T a, T b) { return T(uint32_t(a) & uint32_t(b)); } \
    inline T operator ~ (T a) { return T(~uint32_t(a)); } \
    inline bool operator !(T a) { return uint32_t(a) == 0; } \
    inline bool operator ==(T a, uint32_t b) { return uint32_t(a) == b; } \
    inline bool operator !=(T a, uint32_t b) { return uint32_t(a) != b; }

#define NEKO_PARAM_WITH_DEFAULT(ClassType,ParamType,Param,Default) \
        ParamType Param = Default; \
        ClassType& Set##Param(const ParamType& value) \
        { \
            Param = value;\
            return *this; \
        } \
       

#define NEKO_PARAM_ARRAY(ClassType,ParamType,Param,Size) \
        static_vector<ParamType,Size> Param##Array; \
        ClassType& Add##Param(const ParamType& value) \
        { \
            Param##Array.push_back(value);\
            return *this; \
        } \

    constexpr uint32_t MAX_RENDER_TARGET_COUNT = 8;
    constexpr uint32_t MAX_VERTEX_ATTRIBUTE_COUNT = 15;
    constexpr uint32_t MAX_VERTEX_BINDING_COUNT = 15;
    constexpr uint32_t MAX_BINDING_LAYOUT_COUNT = 5;
    constexpr uint32_t MAX_BINDINGS_PER_LAYOUT = 128;
    constexpr uint32_t MAX_SHADER_STAGE_COUNT = 2; // vs,ps
    
    enum class RHIFormat : uint8_t
    {
        B8G8R8A8_SNORM,
        Undefined
    };
    
    enum class RHICmdQueueType : uint8_t
    {
        Graphic = 0x1,
        Compute,
        Transfer
    };

    enum class RHIShaderStage : uint8_t
    {
        VS = 0x1,
        PS = 0x2,

        All = VS | PS,
    };
    NEKO_ENUM_CLASS_FLAG_OPERATORS(RHIShaderStage)

    enum class RHIVertexRate : uint8_t
    {
        Vertex = 0x1,
        Instance
    };

    enum class RHIPrimitiveTopology : uint8_t
    {
        TriangleList = 0x1
    };

    enum class RHICullMode : uint8_t
    {
        Back = 0x1,
        Front,
        None
    };

    enum class RHIFrontFace : uint8_t
    {
        CCW = 0x1,
        CW
    };

    enum class RHIPolygonMode : uint8_t
    {
        Fill = 0x1
    };

    enum class RHISampleCount : uint8_t
    {
        SampleCount_1 = 0x1,
        SampleCount_2,
        SampleCount_4,
        SampleCount_8,
        SampleCount_16,
    };

    enum class RHIStencilOp : uint8_t
    {
        Keep = 0x1,
    };

    enum class RHICompareOp : uint8_t
    {
        Never = 0x1,
        Less,
        Always,
    };

    enum class RHIBlendFactor : uint8_t
    {
        Zero = 0x1,
        One,
    };

    enum class RHIBlendOp : uint8_t
    {
        Add = 0x1,
    };

    enum class RHIColorComponent : uint8_t
    {
        None = 0x0,
        R = 0x1,
        G = 0x2,
        B = 0x4,
        A = 0x8,
        All = 0Xf
    };

    NEKO_ENUM_CLASS_FLAG_OPERATORS(RHIColorComponent)

    enum class RHIResourceType : uint8_t
    {
        UniformBuffer
    };

    enum class RHILoadOp : uint8_t
    {
        DontCare
    };

    enum class RHIStoreOp : uint8_t
    {
        Clear
    };
     
    struct RHIShaderDesc
    {
        NEKO_PARAM_WITH_DEFAULT(RHIShaderDesc, const char*, DebugName, "");
        NEKO_PARAM_WITH_DEFAULT(RHIShaderDesc, const char*, Blob, "");
        NEKO_PARAM_WITH_DEFAULT(RHIShaderDesc, const char*, EntryPoint, "");
        NEKO_PARAM_WITH_DEFAULT(RHIShaderDesc, RHIShaderStage, Stage, RHIShaderStage::All);
        NEKO_PARAM_WITH_DEFAULT(RHIShaderDesc, uint32_t, Size, 0);
    };

    class RHIShader : public RHIResource
    {
    public:
        virtual const RHIShaderDesc& GetDesc() const = 0;
    };
    typedef RefCountPtr<RHIShader> RHIShaderRef;

    struct RHIVertexAttribute
    {
        NEKO_PARAM_WITH_DEFAULT(RHIVertexAttribute, RHIFormat, Format, RHIFormat::Undefined);
        NEKO_PARAM_WITH_DEFAULT(RHIVertexAttribute, uint8_t, Binding,0);
        NEKO_PARAM_WITH_DEFAULT(RHIVertexAttribute, uint8_t, Location, 0);
        NEKO_PARAM_WITH_DEFAULT(RHIVertexAttribute, uint8_t, Offset, 0);
    };

    struct RHIVertexBinding
    {
        NEKO_PARAM_WITH_DEFAULT(RHIVertexBinding, uint8_t, Binding, 0);
        NEKO_PARAM_WITH_DEFAULT(RHIVertexBinding, uint8_t, Stride, 0);
        NEKO_PARAM_WITH_DEFAULT(RHIVertexBinding, RHIVertexRate, VertexRate, RHIVertexRate::Vertex);
    };

    struct RHIVertexInputLayout
    {
        NEKO_PARAM_ARRAY(RHIVertexInputLayout, RHIVertexAttribute, Attribute, MAX_VERTEX_ATTRIBUTE_COUNT);
        NEKO_PARAM_ARRAY(RHIVertexInputLayout, RHIVertexBinding, Binding, MAX_VERTEX_BINDING_COUNT);
        NEKO_PARAM_WITH_DEFAULT(RHIVertexInputLayout, uint8_t, AttributeCount, 0);
        NEKO_PARAM_WITH_DEFAULT(RHIVertexInputLayout, uint8_t, Unused, 0);
    };
        
    struct RHIRasterSate
    {
        NEKO_PARAM_WITH_DEFAULT(RHIRasterSate, RHICullMode, CullMode, RHICullMode::Back);
        NEKO_PARAM_WITH_DEFAULT(RHIRasterSate, RHIPolygonMode, PolygonMode, RHIPolygonMode::Fill);
        NEKO_PARAM_WITH_DEFAULT(RHIRasterSate, RHIFrontFace, FrontFace, RHIFrontFace::CCW);
        NEKO_PARAM_WITH_DEFAULT(RHIRasterSate, uint8_t, Unused, 0);
    };

    struct RHIDepthStencilState
    {
        struct RHIStencilState
        {
            NEKO_PARAM_WITH_DEFAULT(RHIStencilState, RHIStencilOp, FailOp, RHIStencilOp::Keep);
            NEKO_PARAM_WITH_DEFAULT(RHIStencilState, RHIStencilOp, DepthFailOp, RHIStencilOp::Keep);
            NEKO_PARAM_WITH_DEFAULT(RHIStencilState, RHIStencilOp, PassOp, RHIStencilOp::Keep);
            NEKO_PARAM_WITH_DEFAULT(RHIStencilState, RHICompareOp, StencilCompareOp, RHICompareOp::Always);
        };

        NEKO_PARAM_WITH_DEFAULT(RHIDepthStencilState, bool, DepthTest, true);
        NEKO_PARAM_WITH_DEFAULT(RHIDepthStencilState, bool, DepthWrite, true);
        NEKO_PARAM_WITH_DEFAULT(RHIDepthStencilState, RHICompareOp, DepthCompareOp, RHICompareOp::Less);
        NEKO_PARAM_WITH_DEFAULT(RHIDepthStencilState, bool, StencilTest, false);
        NEKO_PARAM_WITH_DEFAULT(RHIDepthStencilState, uint8_t, ReadMask, 0xff);
        NEKO_PARAM_WITH_DEFAULT(RHIDepthStencilState, uint8_t, WriteMask, 0xff);
        NEKO_PARAM_WITH_DEFAULT(RHIDepthStencilState, uint8_t, StencilRef, 0u);
        NEKO_PARAM_WITH_DEFAULT(RHIDepthStencilState, RHIStencilState, FrontStencil, RHIStencilState());
        NEKO_PARAM_WITH_DEFAULT(RHIDepthStencilState, RHIStencilState, BackStencil, RHIStencilState());
    };

    struct RHIBlendState
    {
        struct RHIRenderTarget
        {
            NEKO_PARAM_WITH_DEFAULT(RHIRenderTarget, bool, BlendEnable, false);
            NEKO_PARAM_WITH_DEFAULT(RHIRenderTarget, RHIBlendFactor, SrcColor, RHIBlendFactor::One);
            NEKO_PARAM_WITH_DEFAULT(RHIRenderTarget, RHIBlendFactor, DestColor, RHIBlendFactor::Zero);
            NEKO_PARAM_WITH_DEFAULT(RHIRenderTarget, RHIBlendOp, ColorOp, RHIBlendOp::Add);
            NEKO_PARAM_WITH_DEFAULT(RHIRenderTarget, RHIBlendFactor, SrcAlpha, RHIBlendFactor::One);
            NEKO_PARAM_WITH_DEFAULT(RHIRenderTarget, RHIBlendFactor, DestAlpha, RHIBlendFactor::Zero);
            NEKO_PARAM_WITH_DEFAULT(RHIRenderTarget, RHIBlendOp, AlphaOp, RHIBlendOp::Add);
            NEKO_PARAM_WITH_DEFAULT(RHIRenderTarget, RHIColorComponent, WriteMask, RHIColorComponent::All);
        };

        RHIRenderTarget renderTargets[MAX_RENDER_TARGET_COUNT];
        RHIBlendState& SetRenderTarget(uint32_t index, const RHIRenderTarget& target) { renderTargets[index] = target; return *this; }
    };
    
    struct  RHIBindingLayoutBinding
    {
        NEKO_PARAM_WITH_DEFAULT(RHIBindingLayoutBinding, uint8_t, Binding, 0);
        NEKO_PARAM_WITH_DEFAULT(RHIBindingLayoutBinding, RHIResourceType, ResourceType, RHIResourceType::UniformBuffer);
    };

    struct RHIBindingLayoutDesc
    {
        NEKO_PARAM_ARRAY(RHIBindingLayoutDesc, RHIBindingLayoutBinding, Binding, MAX_BINDINGS_PER_LAYOUT);// 256 bytes
        NEKO_PARAM_WITH_DEFAULT(RHIBindingLayoutDesc, RHIShaderStage, ShaderStage, RHIShaderStage::All);// 1 byte
       
    };
        
    class RHIBindingLayout  : public RHIResource
    {
    };
    typedef RefCountPtr<RHIBindingLayout> RHIBindingLayoutRef;
  
    class RHIFrameBuffer : public RHIResource
    {
    };

    struct RHIGraphicPipelineDesc
    {
        NEKO_PARAM_WITH_DEFAULT(RHIGraphicPipelineDesc, RHIPrimitiveTopology, PrimitiveTopology, RHIPrimitiveTopology::TriangleList);
        NEKO_PARAM_WITH_DEFAULT(RHIGraphicPipelineDesc, RHISampleCount, SampleCount, RHISampleCount::SampleCount_1);
        
        NEKO_PARAM_WITH_DEFAULT(RHIGraphicPipelineDesc, RHIShaderRef, VertexShader, RHIShaderRef());
        NEKO_PARAM_WITH_DEFAULT(RHIGraphicPipelineDesc, RHIShaderRef, PixelShader, RHIShaderRef());
        
        NEKO_PARAM_WITH_DEFAULT(RHIGraphicPipelineDesc, RHIVertexInputLayout, VertexInputLayout, RHIVertexInputLayout());
        NEKO_PARAM_WITH_DEFAULT(RHIGraphicPipelineDesc, RHIRasterSate, RasterState, RHIRasterSate());
        NEKO_PARAM_WITH_DEFAULT(RHIGraphicPipelineDesc, RHIDepthStencilState, DepthStencilState, RHIDepthStencilState());
        NEKO_PARAM_WITH_DEFAULT(RHIGraphicPipelineDesc, RHIBlendState, BlendState, RHIBlendState());
        NEKO_PARAM_ARRAY(RHIGraphicPipelineDesc, RHIBindingLayoutRef, BindingLayout, MAX_BINDING_LAYOUT_COUNT);
    };
    
    class RHIGraphicPipeline : public RHIResource
    {
    };
    typedef RefCountPtr<RHIGraphicPipeline> RHIGraphicPipelineRef;

    struct RHICmdListDesc
    {
        NEKO_PARAM_WITH_DEFAULT(RHICmdListDesc, RHICmdQueueType, type, RHICmdQueueType::Graphic);
    }; 

    class RHICmdList : public RHIResource
    {
    };
    typedef RefCountPtr<RHICmdList> RHICmdListRef;
    
    struct RHISwapChainDesc
    {
        NEKO_PARAM_WITH_DEFAULT(RHISwapChainDesc, RHIFormat, Format, RHIFormat::Undefined);
        NEKO_PARAM_WITH_DEFAULT(RHISwapChainDesc, bool, VSync, true);
    };

    class RHISwapChain : public RHIResource
    {
    };
    typedef RefCountPtr<RHISwapChain> RHISwapChainRef;

#ifdef NEKO_VULKAN
    struct RHIVulkanDesc
    { 
        const char** InstanceExtensions = nullptr;
        uint32_t InstanceExtensionNum = 0;
        RHIVulkanDesc& SetInstanceExtensions(const char** extensions, uint32_t num)
        {
            InstanceExtensions = extensions;
            InstanceExtensionNum = num;
            return *this;
        }
    };
#endif

    struct RHIFeatures
    {
        NEKO_PARAM_WITH_DEFAULT(RHIFeatures, bool, Swapchain, false);
    };

    struct RHIDeviceDesc
    {
        NEKO_PARAM_WITH_DEFAULT(RHIDeviceDesc, bool, Validation, false);
        NEKO_PARAM_WITH_DEFAULT(RHIDeviceDesc, uint32_t, GpuIndex, 0);
        NEKO_PARAM_WITH_DEFAULT(RHIDeviceDesc, RHIFeatures, Features, RHIFeatures());
#ifdef NEKO_VULKAN
        NEKO_PARAM_WITH_DEFAULT(RHIDeviceDesc, RHIVulkanDesc, VulkanDesc, RHIVulkanDesc());
#endif
    };

    class RHIDevice : public RHIResource
    {
   
    public:
        [[nodiscard]] virtual RHICmdListRef CreateCmdList(const RHICmdListDesc& = RHICmdListDesc()) const = 0;

        [[nodiscard]] virtual RHIShaderRef CreateShader(const RHIShaderDesc &) const = 0;

        [[nodiscard]] virtual RHIGraphicPipelineRef CreateGraphicPipeline(const RHIGraphicPipelineDesc&, const RHIFrameBuffer&) const = 0;

        [[nodiscard]] virtual RHIBindingLayoutRef CreateBindingLayout(const RHIBindingLayoutDesc& desc) const = 0;
        
        //[[nodiscard]] virtual RHISwapChainRef CreateSwapChain(const RHISwapChainDesc& desc) const = 0;
    };

    typedef RefCountPtr<RHIDevice> RHIDeviceRef;
    
    extern RHIDeviceRef CreateDevice(const RHIDeviceDesc& desc);
}