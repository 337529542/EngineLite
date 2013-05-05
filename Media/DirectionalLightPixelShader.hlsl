struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 tex0: TEXCOORD0;
};

struct PSOutput
{
    float4 color :COLOR;
};

struct ShaderVars
{
    float4 lightrdir;
    float4 lightcolor;
	float4 ViewPosInWorld;
};
ShaderVars svar : register(c0);

sampler mySampler :register(s0);

Texture2D <float4> difftex : register(t0);
Texture2D <float4> normtex : register(t1);
Texture2D <float4> spectex : register(t2);
Texture2D <float4> postex : register(t3);

PSOutput DirectionalLightPixelShader(PixelShaderInput input) : SV_TARGET
{
	PSOutput outp;

	float4 tv3 = postex.Sample(mySampler, input.tex0);
	if(tv3.w == -1.0f)//not used
	{
		outp.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
		return outp;
	}

	float4 tv2 = spectex.Sample(mySampler, input.tex0);
	float4 tv1 = normtex.Sample(mySampler, input.tex0);
	float4 tv0 = difftex.Sample(mySampler, input.tex0);


    float3 lightdir = svar.lightrdir.xyz;//float3(-200.0f, 0.0f, -100.0f);
	lightdir = normalize(lightdir);
    float3 lightcolor = svar.lightcolor.xyz;//float3(0.8, 0.8, 0.8);
	float3 sV = svar.ViewPosInWorld.xyz;//float3(0, 0, -70);
    
    float K_d = tv0.w;//0.8;
    float Ns = tv1.w;//10;
    float Ks = tv2.w;//1;
    float Ka = tv3.w;//0.0;//ambi
    float3 ambi = tv0.xyz;//difftex.Sample(mySampler, input.tex0);

    //caculate diffuse color
    //I_ldiff = K_d * I_l * (N©qL);
    float3 Kdiff= tv0.xyz;
    float3 N = normalize(tv1.xyz);
    float3 diffc =  K_d * Kdiff * lightcolor * max(dot(N, lightdir), 0);

    //Ispec = Ks * Il * ( dot(V, (2 * dot(N,L) * N ¨C L ) )^Ns
    float3 V = sV - tv3.xyz;
    V = normalize(V);
    float3 Il = tv2.xyz;
    //float3 Ispec = Ks  * lightcolor * Il * pow( saturate(dot( (saturate(dot(2 * N, lightdir)) * N - lightdir) , V)) , Ns);
	float3 Ispec = Ks  * lightcolor * Il * pow( saturate(dot( reflect( -lightdir, N ) , V)) , Ns);

    //ambient
    float3 ambic = Ka * ambi;

    float3 finalc =ambic + diffc + Ispec;
    //float3 finalc =ambic + diffc;
	//float3 finalc =Ispec;

    outp.color = float4(finalc, 1.0f);//float4(1.0f, 1.0f, 0.0f, 1.0f);
    return outp;
}
