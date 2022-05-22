#pragma once
#include "ShaderReflection/ShaderReflection.h"
#include "spirv_hlsl.hpp"
#include <vector>
#include <string>

class FSPIRVReflection : public IShaderReflection
{
public:
    FSPIRVReflection(const void* data, size_t size);
    const std::vector<FEntryPoint>& GetEntryPoints() const override;
    const std::vector<FResourceBindingDesc>& GetBindings() const override;
    const std::vector<FVariableLayout>& GetVariableLayouts() const override;
    const std::vector<FInputParameterDesc>& GetInputParameters() const override;
    const std::vector<FOutputParameterDesc>& GetOutputParameters() const override;
    const FShaderFeatureInfo& GetShaderFeatureInfo() const override;

private:
    std::vector<uint32_t> m_blob;
    std::vector<FEntryPoint> m_entry_points;
    std::vector<FResourceBindingDesc> m_bindings;
    std::vector<FVariableLayout> m_layouts;
    std::vector<FInputParameterDesc> m_input_parameters;
    std::vector<FOutputParameterDesc> m_output_parameters;
    FShaderFeatureInfo m_shader_feature_info = {};
};
