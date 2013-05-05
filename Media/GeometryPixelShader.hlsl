struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 norm: TEXCOORD0;
    float2 texuv: TEXCOORD1;
    float4 posinworld: TEXCOORD2;
    float3 tang : TEXCOORD3;
};

struct PSOutput
{
    float4 t0 : SV_Target0;
    float4 t1 : SV_Target1;
    float4 t2 : SV_Target2;
    float4 t3 : SV_Target3;
};

struct PixelShaderVars
{
    float K_d;
	float Ns;
	float K_s;
	float K_a;
};
PixelShaderVars psvar : register(c0);

sampler mySampler :register(s0);

Texture2D <float4> difftex : register(t0);
Texture2D <float4> normtex : register(t1);
Texture2D <float4> spectex : register(t2);

PSOutput GeometryPixelShader(PixelShaderInput input)
{
    // Draw the entire triangle yellow.
    //return float4(1.0f, 1.0f, 1.0f, 0.5f);
    //return difftex.Sample(diffSampler, input.texuv);

    //cacl normal
    float3 bnormal = normalize(cross(input.tang, input.norm.xyz));

    float3x3 TBN;
    TBN[0] = input.tang;
    TBN[1] = bnormal;
    TBN[2] = input.norm.xyz;

    float3 tbnnorm = normtex.Sample(mySampler, input.texuv).xyz;

    float3 finalnorm = mul(tbnnorm , TBN);

    PSOutput outp;
    outp.t0 = float4(difftex.Sample(mySampler, input.texuv).xyz, psvar.K_d);
    outp.t1 = float4(finalnorm, psvar.Ns);
    outp.t2 = float4(spectex.Sample(mySampler, input.texuv).xyz, psvar.K_s);
    outp.t3 = float4(input.posinworld.xyz, psvar.K_a);
    return outp;
}
