sampler2D ColorMapSampler : register(s0);

float4 pixelMain(float4 Texcoords : TEXCOORD0) : COLOR0
{
	float4 finalVal = float4(0.0, 0.0, 0.0, 0.0);
	finalVal = tex2D(ColorMapSampler, Texcoords.xy);
	finalVal.a = 0.2;
	return finalVal;
}