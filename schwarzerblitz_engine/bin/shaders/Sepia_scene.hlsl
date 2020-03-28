sampler2D ColorMapSampler : register(s0);
//sampler2D ScreenMapSampler : register(s1);

float mEffectParameters;

float4 pixelMain(float2 TexCoords : TEXCOORD0) : COLOR0
{
	bool applyEffect = mEffectParameters > 0.0f;
	float4 colorMap = tex2D(ColorMapSampler, TexCoords);
	float4 sepiaCol = tex2D(ColorMapSampler, TexCoords);
	float4 bwFilter = sepiaCol;
	if (applyEffect){
		//TexCoords.y = TexCoords.y + (sin(TexCoords.y * 100)*0.03);
		//bwFilter = tex2D(ColorMapSampler, TexCoords.xy);
		bwFilter.r = dot(sepiaCol, float3(.393, .769, .189));
		bwFilter.g = dot(sepiaCol, float3(.349, .686, .168));
		bwFilter.b = dot(sepiaCol, float3(.272, .534, .131));
		return bwFilter;
	}
	else{
		return colorMap;
	}
}