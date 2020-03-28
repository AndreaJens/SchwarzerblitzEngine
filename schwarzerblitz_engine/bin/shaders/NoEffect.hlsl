sampler2D ColorMapSampler : register(s0);

float4 pixelMain(float2 TexCoords : TEXCOORD0) : COLOR0
{
	float4 screenCol = tex2D(ColorMapSampler, TexCoords);
	
	return screenCol;
}