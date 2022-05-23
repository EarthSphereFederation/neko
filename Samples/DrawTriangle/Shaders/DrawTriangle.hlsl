struct VS_INPUT
{
    uint vertexId : SV_VertexID;
};

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float3 col : COLOR;
};

const static float2 positions[3] = {
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5)
};

const static float3 colors[3] = {
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
};

VS_OUTPUT mainVS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = float4(positions[input.vertexId], 0.0f, 1.0f);
    output.col = colors[input.vertexId];
    return output;
}

float4 mainPS(VS_OUTPUT input) : SV_TARGET
{
    return float4(input.col, 1.0f);
}
