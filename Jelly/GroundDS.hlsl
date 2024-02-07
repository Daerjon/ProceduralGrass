#include"Noise.hlsli"

struct PSin
{
    float3 GlobalPosition : Position;
    float4 Position : SV_Position;
};
struct HS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : WORLDPOS; 
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4]			: SV_TessFactor;
	float InsideTessFactor[2]			: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 4

cbuffer cbView : register(b0)
{
    float4x4 viewMatrix;
}
cbuffer cbProj : register(b1)
{
    float4x4 projMatrix;
}

[domain("quad")]
PSin main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
    PSin o;

    o.GlobalPosition = float4(
		patch[0].vPosition * (1 - domain.x) * (1 - domain.y) +
		patch[1].vPosition * (domain.x) * (1 - domain.y) +
		patch[2].vPosition * (1 - domain.x) * (domain.y) +
		patch[3].vPosition * (domain.x) * (domain.y),
	0);
    o.GlobalPosition += float4(0, snoise(o.GlobalPosition.xyz/64), 0, 0);
    o.Position = mul(projMatrix, mul(viewMatrix, float4(o.GlobalPosition, 1)));
	return o;
}
