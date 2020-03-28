float4x4    mWorldViewProj;
float4x4    mWorld;
//float3      mLightPosition;
//float3      mCameraPosition;
float4		mAmbientColor;
float4		mLightColor;
float3		Normal: NORMAL;
float4		mMultiplicationColor;
float4		mAdditionColor;
//float3		Binormal : BINORMAL0;
//float3		Tangent : TANGENT0;

struct OUT
{
	float4 Diffuse : COLOR0;
	float4 Ambient : COLOR1;
	float4 Light : COLOR2;
	float4 Pos: POSITION;
	float3 L:   TEXCOORD0;
	float3 N:   TEXCOORD1;
	//float3 V:   TEXCOORD2;
	float3 T:   TEXCOORD3;
	//float3x3 WorldToTangentSpace : TEXCOORD4;
};

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

OUT vertexMain(OUT I)
{
	OUT Out = (OUT)0;
	Out.Pos = mul(I.Pos, mWorldViewProj);
	Out.T = I.L;
	//float3 LDir = float3(0.5f, 0.5f, 0.5f);
	//Out.L = normalize(mul(LDir, mWorld));
	//Out.N = normalize(mul(Normal, mWorld));
	//float4 PosWorld = mul(I.Pos, mWorld);
	//Out.V = normalize(mCameraPosition - PosWorld);
	Out.Diffuse = I.Diffuse;
	Out.Ambient = mAmbientColor;
	Out.Light = mLightColor;
	//Out.WorldToTangentSpace[0] = mul(normalize(Tangent), mWorld);
	//Out.WorldToTangentSpace[1] = mul(normalize(Binormal), mWorld);
	//Out.WorldToTangentSpace[2] = mul(normalize(Normal), mWorld);
	return Out;
}


float4 pixelMain(OUT I) : COLOR
{
	float4 color = tex2D(ColorMapSampler, I.T);
	//float val = saturate(dot(I.L, I.N));
	float4 color1 = color * I.Diffuse * mMultiplicationColor + mAdditionColor/* * (I.Ambient + I.Light * val)*/;
	color1.w = color.w;
	return color1;
}