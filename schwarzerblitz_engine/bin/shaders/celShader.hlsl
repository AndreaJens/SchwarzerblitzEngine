//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4x4 mWorldViewProj;  // World * View * Projection transformation
float4x4 mInvWorld;       // Inverted world matrix
float3 mLightPos;         // Light position

struct VS_OUTPUT
{
	float4 Position: POSITION0;
	float2 TexCoord:   TEXCOORD0;
	float2 diffuse: TEXCOORD1;
};


VS_OUTPUT vertexMain(
	float4 Position: POSITION0,
	float2 TexCoord : TEXCOORD0,
	float2 diffuse : TEXCOORD1,
	float3 Normal : NORMAL0)
{
	VS_OUTPUT Output;

	// calculate light vector
	float3 N = normalize(Normal);
		float3 L = normalize(mLightPos - Position.xyz);

		// Calculate diffuse component
		diffuse = max(dot(N, L), 0);

	// Shading offset
	float offset = 0.1f;

	//Subtract the offset and clamp the texture to 0
	diffuse = clamp(diffuse + offset, 0, 1);

	Output.Position = mul(Position, mWorldViewProj);
	Output.diffuse = diffuse;
	Output.TexCoord = TexCoord;
	return(Output);
}

sampler2D tex0 : register(s0);
sampler1D diffuseRamp : register(s2);

float4 pixelMain(
	float2 TexCoord:     TEXCOORD0,
	float2 diffuseIn : TEXCOORD1
	) : COLOR0
{
	float4 color = float4(1, 1, 1, 1);
	float4 diffuse = float4(0.8f, 0.8f, 0.8f, 1.0f);
	// Step functions from textures
	diffuseIn = tex1D(diffuseRamp, diffuseIn).x;



	color = (diffuse * diffuseIn);
	float4 col = tex2D(tex0, TexCoord);
		col *= 2;
	return(color*col);

}