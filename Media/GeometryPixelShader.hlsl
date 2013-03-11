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

sampler mySampler :register(s0);

Texture2D <float4> difftex : register(t0);
Texture2D <float4> normtex : register(t1);
Texture2D <float4> spectex : register(t2);

PSOutput GeometryPixelShader(PixelShaderInput input)
{
    // Draw the entire triangle yellow.
    //return float4(1.0f, 1.0f, 1.0f, 0.5f);
    //return difftex.Sample(diffSampler, input.texuv);

    PSOutput outp;
    outp.t0 = difftex.Sample(mySampler, input.texuv);
    outp.t1 = normtex.Sample(mySampler, input.texuv);
    outp.t2 = spectex.Sample(mySampler, input.texuv);
    outp.t3 = input.pos;
    return outp;
}
