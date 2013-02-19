struct VertexShaderInput
{
    float3 pos : POSITION;
    float3 norm: NORMAL;
};

struct ShaderVars
{
    float4x4 viewMatrix;
    float4x4 worldMatrix;
    float4x4 perspectiveMatrix;
};
ShaderVars svar : register(c0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 norm: TEXCOORD0;
};

PixelShaderInput GeometryVertexShader(VertexShaderInput input)
{
    PixelShaderInput vertexShaderOutput;

    vertexShaderOutput.pos = mul(float4(input.pos, 1.0f), svar.worldMatrix);
    vertexShaderOutput.pos = mul(vertexShaderOutput.pos, svar.viewMatrix);
    vertexShaderOutput.pos = mul(vertexShaderOutput.pos, svar.perspectiveMatrix);

    vertexShaderOutput.norm = mul(float4(input.norm, 0.0f), svar.worldMatrix);

    return vertexShaderOutput;
}