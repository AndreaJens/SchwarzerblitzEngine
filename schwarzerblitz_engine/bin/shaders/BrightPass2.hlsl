sampler2D ColorMapSampler : register(s0);

float2 mEffectParameters;

float4 pixelMain ( float4 Texcoords : TEXCOORD0 ) : COLOR0
{
	float4 finalVal = tex2D(ColorMapSampler, Texcoords.xy);
	float brightness = dot(finalVal.rgb, float3(0.2126, 0.7152, 0.0722));
	if (brightness > mEffectParameters[0] || brightness <  mEffectParameters[1]){
		finalVal *= 2;
		finalVal = pow(finalVal, 8.0);
	}
	return finalVal;
}