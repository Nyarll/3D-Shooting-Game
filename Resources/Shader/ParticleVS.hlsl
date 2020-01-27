#include "Particle.hlsli"

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Pos = float4(input.Pos, 1);
	output.Color = input.Color;
	output.Tex = input.Tex;

	return output;
}