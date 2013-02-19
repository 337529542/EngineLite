struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 norm: TEXCOORD0;
};

struct PSOutput
{
    float4 color :COLOR;
    //float  dep   : DEPTH;
};

float4 GeometryPixelShader(PixelShaderInput input) : SV_TARGET
{
    // Draw the entire triangle yellow.
    return float4(1.0f, 1.0f, 1.0f, 0.5f);
}