#include "Particle.hlsli"


Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	float2 uv = input.Tex;
	float4 output = tex.Sample(samLinear, uv);

	output.a = Any.x;

	return output;
}