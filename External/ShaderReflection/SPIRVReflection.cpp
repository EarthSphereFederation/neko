#include "SPIRVReflection.h"

EShaderKind ConvertShaderKind(spv::ExecutionModel execution_model)
{
    switch (execution_model)
    {
        case spv::ExecutionModel::ExecutionModelVertex:
            return EShaderKind::kVertex;
        case spv::ExecutionModel::ExecutionModelFragment:
            return EShaderKind::kPixel;
        case spv::ExecutionModel::ExecutionModelGeometry:
            return EShaderKind::kGeometry;
        case spv::ExecutionModel::ExecutionModelGLCompute:
            return EShaderKind::kCompute;
        case spv::ExecutionModel::ExecutionModelRayGenerationNV:
            return EShaderKind::kRayGeneration;
        case spv::ExecutionModel::ExecutionModelIntersectionNV:
            return EShaderKind::kIntersection;
        case spv::ExecutionModel::ExecutionModelAnyHitNV:
            return EShaderKind::kAnyHit;
        case spv::ExecutionModel::ExecutionModelClosestHitNV:
            return EShaderKind::kClosestHit;
        case spv::ExecutionModel::ExecutionModelMissNV:
            return EShaderKind::kMiss;
        case spv::ExecutionModel::ExecutionModelCallableNV:
            return EShaderKind::kCallable;
        case spv::ExecutionModel::ExecutionModelTaskNV:
            return EShaderKind::kAmplification;
        case spv::ExecutionModel::ExecutionModelMeshNV:
            return EShaderKind::kMesh;
    }
    assert(false);
    return EShaderKind::kUnknown;
}

bool IsBufferDimension(spv::Dim dimension)
{
    switch (dimension)
    {
        case spv::Dim::DimBuffer:
            return true;
        case spv::Dim::Dim1D:
        case spv::Dim::Dim2D:
        case spv::Dim::Dim3D:
        case spv::Dim::DimCube:
            return false;
        default:
            assert(false);
            return false;
    }
}

EViewType GetViewType(const spirv_cross::Compiler& compiler, const spirv_cross::SPIRType& type, uint32_t resource_id)
{
    switch (type.basetype)
    {
        case spirv_cross::SPIRType::AccelerationStructure:
        {
            return EViewType::kAccelerationStructure;
        }
        case spirv_cross::SPIRType::SampledImage:
        case spirv_cross::SPIRType::Image:
        {
            bool is_readonly = (type.image.sampled != 2);
            if (IsBufferDimension(type.image.dim))
            {
                if (is_readonly)
                    return EViewType::kBuffer;
                else
                    return EViewType::kRWBuffer;
            }
            else
            {
                if (is_readonly)
                    return EViewType::kTexture;
                else
                    return EViewType::kRWTexture;
            }
        }
        case spirv_cross::SPIRType::Sampler:
        {
            return EViewType::kSampler;
        }
        case spirv_cross::SPIRType::Struct:
        {
            if (type.storage == spv::StorageClassStorageBuffer)
            {
                spirv_cross::Bitset flags = compiler.get_buffer_block_flags(resource_id);
                bool is_readonly = flags.get(spv::DecorationNonWritable);
                if (is_readonly)
                {
                    return EViewType::kStructuredBuffer;
                }
                else
                {
                    return EViewType::kRWStructuredBuffer;
                }
            }
            else if (type.storage == spv::StorageClassPushConstant || type.storage == spv::StorageClassUniform)
            {
                return EViewType::kConstantBuffer;
            }
            assert(false);
            return EViewType::kUnknown;
        }
        default:
            assert(false);
            return EViewType::kUnknown;
    }
}

EViewDimension GetDimension(spv::Dim dim, const spirv_cross::SPIRType& resource_type)
{
    switch (dim)
    {
        case spv::Dim::Dim1D:
        {
            if (resource_type.image.arrayed)
                return EViewDimension::kTexture1DArray;
            else
                return EViewDimension::kTexture1D;
        }
        case spv::Dim::Dim2D:
        {
            if (resource_type.image.arrayed)
                return EViewDimension::kTexture2DArray;
            else
                return EViewDimension::kTexture2D;
        }
        case spv::Dim::Dim3D:
        {
            return EViewDimension::kTexture3D;
        }
        case spv::Dim::DimCube:
        {
            if (resource_type.image.arrayed)
                return EViewDimension::kTextureCubeArray;
            else
                return EViewDimension::kTextureCube;
        }
        case spv::Dim::DimBuffer:
        {
            return EViewDimension::kBuffer;
        }
        default:
            assert(false);
            return EViewDimension::kUnknown;
    }
}

EViewDimension GetViewDimension(const spirv_cross::SPIRType& resource_type)
{
    if (resource_type.basetype == spirv_cross::SPIRType::BaseType::Image)
    {
        return GetDimension(resource_type.image.dim, resource_type);
    }
    else if (resource_type.basetype == spirv_cross::SPIRType::BaseType::Struct)
    {
        return EViewDimension::kBuffer;
    }
    else
    {
        return EViewDimension::kUnknown;
    }
}

EReturnType GetReturnType(const spirv_cross::CompilerHLSL& compiler, const spirv_cross::SPIRType& resource_type)
{
    if (resource_type.basetype == spirv_cross::SPIRType::BaseType::Image)
    {
        decltype(auto) image_type = compiler.get_type(resource_type.image.type);
        switch (image_type.basetype)
        {
            case spirv_cross::SPIRType::BaseType::Float:
                return EReturnType::kFloat;
            case spirv_cross::SPIRType::BaseType::UInt:
                return EReturnType::kUint;
            case spirv_cross::SPIRType::BaseType::Int:
                return EReturnType::kInt;
            case spirv_cross::SPIRType::BaseType::Double:
                return EReturnType::kDouble;
        }
        assert(false);
    }
    return EReturnType::kUnknown;
}

FResourceBindingDesc GetBindingDesc(const spirv_cross::CompilerHLSL& compiler, const spirv_cross::Resource& resource)
{
    FResourceBindingDesc desc = {};
    decltype(auto) type = compiler.get_type(resource.type_id);
    desc.name = compiler.get_name(resource.id);
    desc.type = GetViewType(compiler, type, resource.id);
    desc.slot = compiler.get_decoration(resource.id, spv::DecorationBinding);
    desc.space = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
    desc.count = 1;
    if (!type.array.empty() && type.array.front() == 0)
    {
        desc.count = std::numeric_limits<uint32_t>::max();
    }
    desc.dimension = GetViewDimension(type);
    desc.return_type = GetReturnType(compiler, type);
    switch (desc.type)
    {
        case EViewType::kStructuredBuffer:
        case EViewType::kRWStructuredBuffer:
        {
            bool is_block = compiler.get_decoration_bitset(type.self).get(spv::DecorationBlock) ||
                            compiler.get_decoration_bitset(type.self).get(spv::DecorationBufferBlock);
            bool is_sized_block = is_block && (compiler.get_storage_class(resource.id) == spv::StorageClassUniform ||
                                               compiler.get_storage_class(resource.id) == spv::StorageClassUniformConstant ||
                                               compiler.get_storage_class(resource.id) == spv::StorageClassStorageBuffer);
            assert(is_sized_block);
            decltype(auto) base_type = compiler.get_type(resource.base_type_id);
            desc.structure_stride = compiler.get_declared_struct_size_runtime_array(base_type, 1) - compiler.get_declared_struct_size_runtime_array(base_type, 0);
            assert(desc.structure_stride);
            break;
        }
    }
    return desc;
}

FVariableLayout GetBufferMemberLayout(const spirv_cross::CompilerHLSL& compiler, const spirv_cross::TypeID& type_id)
{
    decltype(auto) type = compiler.get_type(type_id);
    FVariableLayout layout = {};
    layout.columns = type.vecsize;
    layout.rows = type.columns;
    if (!type.array.empty())
    {
        assert(type.array.size() == 1);
        layout.elements = type.array.front();
    }
    switch (type.basetype)
    {
        case spirv_cross::SPIRType::BaseType::Float:
            layout.type = EVariableType::kFloat;
            break;
        case spirv_cross::SPIRType::BaseType::Int:
            layout.type = EVariableType::kInt;
            break;
        case spirv_cross::SPIRType::BaseType::UInt:
            layout.type = EVariableType::kUint;
            break;
        case spirv_cross::SPIRType::BaseType::Boolean:
            layout.type = EVariableType::kBool;
            break;
        default:
            assert(false);
            break;
    }
    return layout;
}

FVariableLayout GetBufferLayout(EViewType view_type, const spirv_cross::CompilerHLSL& compiler, const spirv_cross::Resource& resource)
{
    if (view_type != EViewType::kConstantBuffer)
    {
        return {};
    }

    FVariableLayout layout = {};
    decltype(auto) type = compiler.get_type(resource.base_type_id);
    layout.name = compiler.get_name(resource.id);
    layout.size = compiler.get_declared_struct_size(type);
    assert(type.basetype == spirv_cross::SPIRType::BaseType::Struct);
    for (size_t i = 0; i < type.member_types.size(); ++i)
    {
        FVariableLayout member = GetBufferMemberLayout(compiler, type.member_types[i]);
        member.name = compiler.get_member_name(resource.base_type_id, i);
        member.offset = compiler.type_struct_member_offset(type, i);
        member.size = compiler.get_declared_struct_member_size(type, i);
        layout.members.emplace_back(member);
    }
    return layout;
}

void ParseBindings(const spirv_cross::CompilerHLSL& compiler, std::vector<FResourceBindingDesc>& bindings, std::vector<FVariableLayout>& layouts)
{
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();
    auto enumerate_resources = [&](const spirv_cross::SmallVector<spirv_cross::Resource>& resources)
    {
        for (const auto& resource : resources)
        {
            bindings.emplace_back(GetBindingDesc(compiler, resource));
            layouts.emplace_back(GetBufferLayout(bindings.back().type, compiler, resource));
        }
    };
    enumerate_resources(resources.uniform_buffers);
    enumerate_resources(resources.storage_buffers);
    enumerate_resources(resources.storage_images);
    enumerate_resources(resources.separate_images);
    enumerate_resources(resources.separate_samplers);
    enumerate_resources(resources.atomic_counters);
    enumerate_resources(resources.acceleration_structures);
}

std::vector<FInputParameterDesc> ParseInputParameters(const spirv_cross::Compiler& compiler)
{
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();
    std::vector<FInputParameterDesc> input_parameters;
    for (const auto& resource : resources.stage_inputs)
    {
        FInputParameterDesc input = {};
        input.location = compiler.get_decoration(resource.id, spv::DecorationLocation);
        input.semantic_name = compiler.get_decoration_string(resource.id, spv::DecorationHlslSemanticGOOGLE);
        if (!input.semantic_name.empty() && input.semantic_name.back() == '0')
        {
            input.semantic_name.pop_back();
        }
        decltype(auto) type = compiler.get_type(resource.base_type_id);
        if (type.basetype == spirv_cross::SPIRType::Float)
        {
            if (type.vecsize == 1)
                input.format = EFormat::FORMAT_R32_SFLOAT_PACK32;
            else if (type.vecsize == 2)
                input.format = EFormat::FORMAT_RG32_SFLOAT_PACK32;
            else if (type.vecsize == 3)
                input.format = EFormat::FORMAT_RGB32_SFLOAT_PACK32;
            else if (type.vecsize == 4)
                input.format = EFormat::FORMAT_RGBA32_SFLOAT_PACK32;
        }
        else if (type.basetype == spirv_cross::SPIRType::UInt)
        {
            if (type.vecsize == 1)
                input.format = EFormat::FORMAT_R32_UINT_PACK32;
            else if (type.vecsize == 2)
                input.format = EFormat::FORMAT_RG32_UINT_PACK32;
            else if (type.vecsize == 3)
                input.format = EFormat::FORMAT_RGB32_UINT_PACK32;
            else if (type.vecsize == 4)
                input.format = EFormat::FORMAT_RGBA32_UINT_PACK32;
        }
        else if (type.basetype == spirv_cross::SPIRType::Int)
        {
            if (type.vecsize == 1)
                input.format = EFormat::FORMAT_R32_SINT_PACK32;
            else if (type.vecsize == 2)
                input.format = EFormat::FORMAT_RG32_SINT_PACK32;
            else if (type.vecsize == 3)
                input.format = EFormat::FORMAT_RGB32_SINT_PACK32;
            else if (type.vecsize == 4)
                input.format = EFormat::FORMAT_RGBA32_SINT_PACK32;
        }
        input_parameters.emplace_back(input);
    }
    return input_parameters;
}

std::vector<FOutputParameterDesc> ParseOutputParameters(const spirv_cross::Compiler& compiler)
{
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();
    std::vector<FOutputParameterDesc> output_parameters;
    for (const auto& resource : resources.stage_outputs)
    {
        FOutputParameterDesc output = {};
        output.slot = compiler.get_decoration(resource.id, spv::DecorationLocation);
        output_parameters.emplace_back();
    }
    return output_parameters;
}

FSPIRVReflection::FSPIRVReflection(const void* data, size_t size)
        : m_blob((const uint32_t*)data, (const uint32_t*)data + size / sizeof(uint32_t))
{
    spirv_cross::CompilerHLSL compiler(m_blob);
    auto entry_points = compiler.get_entry_points_and_stages();
    for (const auto& entry_point : entry_points)
    {
        m_entry_points.push_back({ entry_point.name, ConvertShaderKind(entry_point.execution_model) });
    }
    ParseBindings(compiler, m_bindings, m_layouts);
    m_input_parameters = ParseInputParameters(compiler);
    m_output_parameters = ParseOutputParameters(compiler);
}

const std::vector<FEntryPoint>& FSPIRVReflection::GetEntryPoints() const
{
    return m_entry_points;
}

const std::vector<FResourceBindingDesc>& FSPIRVReflection::GetBindings() const
{
    return m_bindings;
}

const std::vector<FVariableLayout>& FSPIRVReflection::GetVariableLayouts() const
{
    return m_layouts;
}

const std::vector<FInputParameterDesc>& FSPIRVReflection::GetInputParameters() const
{
    return m_input_parameters;
}

const std::vector<FOutputParameterDesc>& FSPIRVReflection::GetOutputParameters() const
{
    return m_output_parameters;
}

const FShaderFeatureInfo& FSPIRVReflection::GetShaderFeatureInfo() const
{
    return m_shader_feature_info;
}
