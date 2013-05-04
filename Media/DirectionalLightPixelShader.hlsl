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
    float3 lightdir = float3(-0.5f, -1.0f, -1.0f);
    float3 lightcolor = float3(1, 1, 1);
    
    lightdir = normalize(lightdir);

    float K_d = 1;
    float Ns = 30;
    float Ks = 2;
    float3 sV = float3(0, 0, -70);
    float3 Ka = float3(0.3, 0.3, 0.3);//ambi
    float3 ambi = difftex.Sample(mySampler, input.tex0);

    PSOutput outp;

    //caculate diffuse color
    //I_ldiff = K_d * I_l * (N©qL);
    float3 Kdiff= difftex.Sample(mySampler, input.tex0);
    float3 N = normalize(normtex.Sample(mySampler, input.tex0));
    float3 diffc =  K_d * Kdiff * lightcolor * max(dot(N, lightdir), 0);

    //Ispec = Ks * Il * ( dot(V, (2 * dot(N,L) * N ¨C L ) )^Ns
    float3 V = sV - postex.Sample(mySampler, input.tex0);
    V = normalize(V);
    float3 Il = spectex.Sample(mySampler, input.tex0);
    float3 Ispec = Ks  * lightcolor * Il * pow( max(dot(V, (max(dot(2 * N, lightdir), 0) * N - lightdir) ), 0) , Ns);

    //ambient
    float3 ambic = Ka * ambi;


    float3 finalc =ambic + diffc + Ispec;

    outp.color = float4(finalc, 1.0f);//float4(1.0f, 1.0f, 0.0f, 1.0f);
    //outp.color = postex.Sample(mySampler, input.tex0);
    //outp.color = outp.color * 0.03f;
    return outp;
}
