#include "GameSpritePolygon.hlsli"

Texture2D tex : register(t0);
SamplerState samLinear : register(s0);


float4 main(PS_INPUT input) : SV_TARGET
{
	float4 diff = tex.Sample(samLinear, input.Tex);
	return diff;
}