struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 tex0: TEXCOORD0;
};

struct PSOutput
{
    float4 color :COLOR;
};

sampler mySampler :register(s0);

Texture2D <float4> difftex : register(t0);
Texture2D <float4> normtex : register(t1);
Texture2D <float4> spectex : register(t2);
Texture2D <float4> postex : register(t3);

PSOutput DirectionalLightPixelShader(PixelShaderInput input) : SV_TARGET
{
    PSOutput outp;
    //outp.color = float4(1.0f, 1.0f, 0.0f, 1.0f);
    outp.color = postex.Sample(mySampler, input.tex0);
    outp.color = outp.color * 0.03f;
    return outp;
}
