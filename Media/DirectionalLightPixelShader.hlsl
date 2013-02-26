struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 tex0: TEXCOORD0;
};

struct PSOutput
{
    float4 color :COLOR;
};

sampler diffSampler :register(s0);

Texture2D <float4> difftex : register(t0);

PSOutput DirectionalLightPixelShader(PixelShaderInput input) : SV_TARGET
{
    PSOutput outp;
    //outp.color = float4(1.0f, 1.0f, 0.0f, 1.0f);
    outp.color = difftex.Sample(diffSampler, input.tex0);
    return outp;
}