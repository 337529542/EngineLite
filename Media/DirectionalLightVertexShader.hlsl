struct VertexShaderInput
{
    float3 pos : POSITION;
    float2 tex0: TEXCOORD0;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 tex0: TEXCOORD0;
};

PixelShaderInput DirectionalLightVertexShader(VertexShaderInput input)
{
    PixelShaderInput vertexShaderOutput;

    vertexShaderOutput.pos = float4(input.pos, 1.0f);
    vertexShaderOutput.tex0 = float4(input.tex0, 0.0f, 0.0f);

    return vertexShaderOutput;
}