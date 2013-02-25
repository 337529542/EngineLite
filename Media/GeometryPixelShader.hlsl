struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 norm: TEXCOORD0;
    float2 texuv: TEXCOORD1;
};

struct PSOutput
{
    float4 t0 : SV_Target0;
    float4 t1 : SV_Target1;
    float4 t2 : SV_Target2;
    float4 t3 : SV_Target3;
};

sampler diffSampler :register(s0);

Texture2D <float4> difftex : register(t0);

PSOutput GeometryPixelShader(PixelShaderInput input)
{
    // Draw the entire triangle yellow.
    //return float4(1.0f, 1.0f, 1.0f, 0.5f);
    //return difftex.Sample(diffSampler, input.texuv);

    PSOutput outp;
    outp.t0 = float4(1.0f, 0.0f, 0.0f, 1.0f);
    outp.t1 = float4(0.0f, 1.0f, 0.0f, 1.0f);
    outp.t2 = float4(0.0f, 0.0f, 1.0f, 1.0f);
    outp.t3 = float4(1.0f, 1.0f, 0.0f, 1.0f);
    return outp;
}