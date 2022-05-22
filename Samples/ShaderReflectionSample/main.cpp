#include "HLSLCompiler/Compiler.h"
#include "ShaderReflection/ShaderReflection.h"
#include <iostream>
#include <cassert>
#include <algorithm>

struct FResourceBindingTestDesc
{
    std::string name;
    uint32_t slot;
};
inline auto MakeTie(const FResourceBindingTestDesc& desc)
{
    return std::tie(desc.name, desc.slot);
}
inline bool operator== (const FResourceBindingTestDesc& lhs, const FResourceBindingTestDesc& rhs)
{
    return MakeTie(lhs) == MakeTie(rhs);
}
inline bool operator< (const FResourceBindingTestDesc& lhs, const FResourceBindingTestDesc& rhs)
{
    return MakeTie(lhs) < MakeTie(rhs);
}

int main()
{
    ShaderDesc vertexShaderDesc = {
            ASSETS_PATH"shaders/VertexShader.hlsl",
            "mainVS",
            EShaderType::kVertex,
            EShaderFeatureLevel::k6_5};
    ShaderDesc pixelShaderDesc = {
            ASSETS_PATH"shaders/PixelShader.hlsl",
            "mainPS",
            EShaderType::kPixel,
            EShaderFeatureLevel::k6_5};
    ShaderDesc rayTracingShaderDesc = {
            ASSETS_PATH"shaders/RayTracing.hlsl",
            "",
            EShaderType::kLibrary,
            EShaderFeatureLevel::k6_5};
    ShaderDesc meshShaderDesc = {
            ASSETS_PATH"shaders/MeshletMS.hlsl",
            "mainMS",
            EShaderType::kMesh,
            EShaderFeatureLevel::k6_5};

    auto vs_blob = Compile(vertexShaderDesc, EShaderBlobType::kSPIRV);
    auto ps_blob = Compile(pixelShaderDesc, EShaderBlobType::kSPIRV);
    auto rt_sbt_blob = Compile(rayTracingShaderDesc, EShaderBlobType::kSPIRV);
    auto ms_blob = Compile(meshShaderDesc, EShaderBlobType::kSPIRV);

    auto vs_reflection = CreateShaderReflection(EShaBlobType::kSPIRV, vs_blob.data(), vs_blob.size());
    auto ps_reflection = CreateShaderReflection(EShaBlobType::kSPIRV, ps_blob.data(), ps_blob.size());
    auto rt_sbt_reflection = CreateShaderReflection(EShaBlobType::kSPIRV, rt_sbt_blob.data(), rt_sbt_blob.size());
    auto ms_reflection = CreateShaderReflection(EShaBlobType::kSPIRV, ms_blob.data(), ms_blob.size());

    // Check vs
    std::vector<FEntryPoint> vs_expect = {
            { "mainVS", EShaderKind::kVertex },
    };
    auto vs_entry_points = vs_reflection->GetEntryPoints();
    assert(vs_entry_points == vs_expect);

    // Check ps
    std::vector<FEntryPoint> ps_expect = {
            { "mainPS", EShaderKind::kPixel },
    };
    auto ps_entry_points = ps_reflection->GetEntryPoints();
    assert(ps_entry_points == ps_expect);

    auto bindings = ps_reflection->GetBindings();
    assert(bindings.size() == 1);
    assert(bindings.front().name == "Settings");

    // Check raytracing shader
    std::vector<FEntryPoint> rt_expect = {
            { "ray_gen", EShaderKind::kRayGeneration },
            { "miss",    EShaderKind::kMiss },
            { "closest", EShaderKind::kClosestHit },
    };
    auto rt_entry_points = rt_sbt_reflection->GetEntryPoints();
    sort(rt_entry_points.begin(), rt_entry_points.end());
    sort(rt_expect.begin(), rt_expect.end());
    assert(rt_entry_points.size() == rt_expect.size());
    for (size_t i = 0; i < rt_entry_points.size(); ++i)
    {
        assert(rt_entry_points[i].name == rt_expect[i].name);
        assert(rt_entry_points[i].kind == rt_expect[i].kind);
    }

    // Check mesh shader
    std::vector<FEntryPoint> ms_expect = {
            { "mainMS", EShaderKind::kMesh },
    };
    auto ms_entry_points = ms_reflection->GetEntryPoints();
    assert(ms_entry_points == ms_expect);
    auto ms_bindings = ms_reflection->GetBindings();

    std::vector<FResourceBindingTestDesc> Bindings = {
            {"Constants", 0 },
            {"MeshInfo", 1},
            {"Position", 2},
            {"Normal", 3},
            {"VertexIndices", 4},
    };
    sort(ms_bindings.begin(), ms_bindings.end());
    sort(Bindings.begin(), Bindings.end());
    assert(ms_bindings.size() == Bindings.size());
    for (size_t i = 0; i < ms_bindings.size(); ++i)
    {
        assert(ms_bindings[i].name == Bindings[i].name);
        assert(ms_bindings[i].slot == Bindings[i].slot);
    }

    return 0;
}