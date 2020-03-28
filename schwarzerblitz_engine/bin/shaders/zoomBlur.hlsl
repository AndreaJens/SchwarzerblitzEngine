sampler2D ColorMapSampler : register(s0);
//sampler2D ScreenMapSampler : register(s1);

float4 mEffectParameters;

float4 pixelMain(float2 TexCoords	: TEXCOORD0) : COLOR0
{
	float2 mCenter = { 0.5f, 0.5f }; ///center of the screen (could be any place)
	float mBlurStart = 1.0f; /// blur offset
	float mBlurWidth = mEffectParameters[1] * 
		(0.5 - 0.5 * cos(6.28f * mEffectParameters[3] / mEffectParameters[2])); ///how big it should be
	int mNsamples = 40;

	bool applyEffect = mEffectParameters[0] > 0.0f;
	float4 colorMap = tex2D(ColorMapSampler, TexCoords);
	float2 uv = TexCoords.xy - mCenter;
	float4 newColor = 0;
	if (applyEffect){
		for (int i = 0; i < mNsamples; i++) {
			float scale = mBlurStart + mBlurWidth*(i / (float)(mNsamples - 1));
			newColor += tex2D(ColorMapSampler, uv * scale + mCenter) / (float)mNsamples;
		}
		//c /= nsamples;
		//newColor = 0.5 * tex2D(ColorMapSampler, uv * 2.0 + mCenter);
		//newColor += 0.5 * tex2D(ColorMapSampler, uv * 0.5 + mCenter);
		return newColor;
	}
	else{
		return colorMap;
	}
}