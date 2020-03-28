float4x4 matWorldViewProj;
float4x4 matWorld;
float4 vecLightDir;
float4 vecEye;

struct VS_OUTPUT
{
	float4 Diffuse : COLOR0;
	float4 Specular : COLOR1;
	float4 Pos : POSITION;
	float3 Light : TEXCOORD0;
	float3 Norm : TEXCOORD1;
	float3 View : TEXCOORD2;
};

VS_OUTPUT vertexMain(float4 Pos : POSITION, float3 Normal : NORMAL)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	Out.Pos = mul(Pos, matWorldViewProj); // transform Position
	Out.Light = vecLightDir; // L
	float3 PosWorld = normalize(mul(Pos, matWorld));
		Out.View = vecEye - PosWorld; // V
	Out.Norm = mul(Normal, matWorld); // N
	Out.Diffuse = Diffuse;
	Out.Ambient = Ambient;
	Out.Specular = Specular;
	return Out;
}

texture2D ColorMap;
sampler2D ColorMapSampler
{
	Texture = <ColorMap>;   MinFilter = linear; MagFilter = linear; MipFilter = linear;
};

texture2D NormalMap;
sampler2D NormalMapSampler
{
	Texture = <NormalMap>;  MinFilter = linear; MagFilter = linear; MipFilter = linear;
};

float4 pixelMain(float3 Light: TEXCOORD0, float3 Norm : TEXCOORD1,
	float3 View : TEXCOORD2, VS_OUTPUT In) : COLOR
{
	float4 diffuse = In.Diffuse;
	float4 ambient = In.Ambient;
	float4 color = tex2D(ColorMapSampler, I.T);

	float3 Normal = normalize(Norm);
	float3 LightDir = normalize(Light);
	float3 ViewDir = normalize(View);
	float4 diff = saturate(dot(Normal, LightDir)); // diffuse component

	// R = 2 * (N.L) * N - L
	float3 Reflect = normalize(2 * diff * Normal - LightDir);
	float4 specular = pow(saturate(dot(Reflect, ViewDir)), 8); // R.V^n

	// I = Acolor + Dcolor * N.L + (R.V)n
	return ambient + color + diffuse * diff + specular;
}