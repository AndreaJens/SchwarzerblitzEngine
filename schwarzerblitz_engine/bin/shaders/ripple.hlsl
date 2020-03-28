sampler2D ColorMapSampler : register(s0);

float4 mEffectParameters = { 1.0f, 0.5f, 0.5f, 0.0f };

float2 Ripple(float2 xy)
{
	float2 result;
	float mRippleCenterX = mEffectParameters[1];
	float mRippleCenterY = mEffectParameters[2];
	float mTimeProgression = mEffectParameters[3];
	float2 cp = { mRippleCenterX, mRippleCenterY };
		float2 v1 = cp - xy;
		float d = length(v1);
	v1 = normalize(v1);
	result = cp + v1 * (cos((d*6.28) + mTimeProgression) * d);
	/*float2 p = -1.0 + 2.0 * xy;
	float len = length(p);
	result = xy + (p / len)*cos(len*12.0 - mTimeProgression*4.0)*0.03;*/
	return frac(result);
	return result;
}

float4 pixelMain(float2 TexCoords : TEXCOORD0) : COLOR0
{
	float4 screenCol = tex2D(ColorMapSampler, TexCoords);
	if (mEffectParameters[0] > 0){
		float2 uv = TexCoords.xy;
		uv = Ripple(uv);
		float4 result = tex2D(ColorMapSampler, uv);
			return result;
	}
	else{
		return screenCol;
	}
}