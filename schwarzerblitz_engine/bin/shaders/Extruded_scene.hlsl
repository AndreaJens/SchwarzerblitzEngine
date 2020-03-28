sampler2D ColorMapSampler : register(s0);
//sampler2D ScreenMapSampler : register(s1);

float mEffectParameters;

float4 pixelMain(float2 TexCoords : TEXCOORD0) : COLOR0
{
	bool applyEffect = mEffectParameters > 0.0f;
	float4 colorMap = tex2D(ColorMapSampler, TexCoords);
	float4 newColor = tex2D(ColorMapSampler, TexCoords);
	if (applyEffect){
		newColor -= tex2D(ColorMapSampler, TexCoords.xy - 0.003)*2.7f;
		newColor += tex2D(ColorMapSampler, TexCoords.xy + 0.003)*2.7f;
		newColor.rgb = (newColor.r + newColor.g + newColor.b) / 3.0f;
		return(newColor);
	}
	else{
		return(colorMap);
	}
}