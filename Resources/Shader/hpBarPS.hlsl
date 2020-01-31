#include "Particle.hlsli"


Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	float2 uv = input.Tex;
	float4 output = tex.Sample(samLinear, uv);

	if (uv.x < 0.02f || uv.x > 0.98f)
	{
		output = float4(0, 0, 0, 1);
	}
	if (uv.y < 0.3f || uv.y > 0.7f)
	{
		output = float4(0, 0, 0, 1);
	}

	float now = Any.x;
	float pmax = Any.y;
	float px = now / pmax;

	if (output.r > 0.5f && output.g > 0.5f && output.b > 0.5f)
	{
		if (uv.x <= px)
		{
			output = float4(0, 1, 0, 1);
		}
	}

	return output;
}