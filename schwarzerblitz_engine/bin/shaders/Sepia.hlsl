sampler2D ColorMapSampler : register(s0);
sampler2D ScreenMapSampler : register(s1);

float4 pixelMain(float2 TexCoords : TEXCOORD0) : COLOR0
{
	float4 screenCol = tex2D(ScreenMapSampler, TexCoords);
	float4 sepiaCol = tex2D(ScreenMapSampler, TexCoords);
	float4 bwFilter = sepiaCol;
	bwFilter.r = dot(sepiaCol, float3(.393, .769, .189));
	bwFilter.g = dot(sepiaCol, float3(.349, .686, .168));
	bwFilter.b = dot(sepiaCol, float3(.272, .534, .131));
	return(bwFilter);
}