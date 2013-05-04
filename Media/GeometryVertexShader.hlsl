struct VertexShaderInput
{
    float3 pos : POSITION;
    float3 norm: NORMAL;
    float2 texuv: TEXCOORD0;
    float3 tang : TANGENT;
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
    float2 texuv: TEXCOORD1;
    float4 posinworld: TEXCOORD2;
    float3 tang : TEXCOORD3;
};

PixelShaderInput GeometryVertexShader(VertexShaderInput input)
{
    PixelShaderInput vertexShaderOutput;

    vertexShaderOutput.pos = mul(float4(input.pos, 1.0f), svar.worldMatrix);
    vertexShaderOutput.pos = mul(vertexShaderOutput.pos, svar.viewMatrix);
    vertexShaderOutput.pos = mul(vertexShaderOutput.pos, svar.perspectiveMatrix);

    vertexShaderOutput.norm = mul(float4(input.norm, 0.0f), svar.worldMatrix);
    vertexShaderOutput.tang = mul(float4(input.tang, 0.0f), svar.worldMatrix);

    vertexShaderOutput.texuv = input.texuv;
    vertexShaderOutput.posinworld = float4(input.pos, 0.0f);

    return vertexShaderOutput;
}