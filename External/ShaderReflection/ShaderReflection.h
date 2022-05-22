#pragma once
#include "BaseTypes.h"
#include <vector>
#include <memory>
#include <string>
#include <tuple>

struct FEntryPoint
{
    std::string name;
    EShaderKind kind;
    uint32_t payload_size; // for DXR
    uint32_t attribute_size; // for DXR
};

inline bool operator== (const FEntryPoint& lhs, const FEntryPoint& rhs)
{
    return std::tie(lhs.name, lhs.kind) == std::tie(rhs.name, rhs.kind);
}

inline bool operator< (const FEntryPoint& lhs, const FEntryPoint& rhs)
{
    return std::tie(lhs.name, lhs.kind) < std::tie(rhs.name, rhs.kind);
}

struct FResourceBindingDesc
{
    std::string name;
    EViewType type;
    uint32_t slot;
    uint32_t space;
    uint32_t count;
    EViewDimension dimension;
    EReturnType return_type;
    uint32_t structure_stride;
};

inline auto MakeTie(const FResourceBindingDesc& desc)
{
    return std::tie(desc.name, desc.type, desc.slot, desc.space, desc.dimension);
};

inline bool operator== (const FResourceBindingDesc& lhs, const FResourceBindingDesc& rhs)
{
    return MakeTie(lhs) == MakeTie(rhs);
}

inline bool operator< (const FResourceBindingDesc& lhs, const FResourceBindingDesc& rhs)
{
    return MakeTie(lhs) < MakeTie(rhs);
}

struct FInputParameterDesc
{
    uint32_t location;
    std::string semantic_name;
    EFormat format;
};

struct FOutputParameterDesc
{
    uint32_t slot;
};

enum class EVariableType
{
    kStruct,
    kFloat,
    kInt,
    kUint,
    kBool,
};

struct FVariableLayout
{
    std::string name;
    EVariableType type;
    uint32_t offset;
    uint32_t size;
    uint32_t rows;
    uint32_t columns;
    uint32_t elements;
    std::vector<FVariableLayout> members;
};

inline auto MakeTie(const FVariableLayout& desc)
{
    return std::tie(desc.name, desc.type, desc.offset, desc.size, desc.rows, desc.columns, desc.elements, desc.members);
};

inline bool operator== (const FVariableLayout& lhs, const FVariableLayout& rhs)
{
    return MakeTie(lhs) == MakeTie(rhs);
}

inline bool operator< (const FVariableLayout& lhs, const FVariableLayout& rhs)
{
    return MakeTie(lhs) < MakeTie(rhs);
}

struct FShaderFeatureInfo
{
    bool resource_descriptor_heap_indexing = false;
    bool sampler_descriptor_heap_indexing = false;
};

class IShaderReflection
{
public:
    virtual ~IShaderReflection() = default;
    virtual const std::vector<FEntryPoint>& GetEntryPoints() const = 0;
    virtual const std::vector<FResourceBindingDesc>& GetBindings() const = 0;
    virtual const std::vector<FVariableLayout>& GetVariableLayouts() const = 0;
    virtual const std::vector<FInputParameterDesc>& GetInputParameters() const = 0;
    virtual const std::vector<FOutputParameterDesc>& GetOutputParameters() const = 0;
    virtual const FShaderFeatureInfo& GetShaderFeatureInfo() const = 0;
};

enum class EShaBlobType
{
    kDXIL,
    kSPIRV
};

std::shared_ptr<IShaderReflection> CreateShaderReflection(EShaBlobType type, const void* data, size_t size);
