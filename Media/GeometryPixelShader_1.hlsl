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

PSOutput GeometryPixelShader(PixelShaderInput input) : SV_TARGET
{
    // Draw the entire triangle yellow.
    //return float4(1.0f, 1.0f, 0.0f, 1.0f);

    //Lambert
    float3 l_a = float3(0.8f, 0.8f, 0.8f);
    float3 l_pos = float3(0.0f, 0.0f, -1.0f);

    float3 norm = normalize(input.norm.xyz);

    float3 diff = max(dot(norm , l_pos) , 0);

    PSOutput outp;
    outp.color = float4(diff, 1.0f);
    //outp.dep = 0.5;

    return outp;
}