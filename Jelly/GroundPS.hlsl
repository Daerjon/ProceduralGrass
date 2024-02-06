#include"Noise.hlsli"

struct PSin
{
    float3 GlobalPosition : Position;
    float4 Position : SV_Position;
};

float4 main(PSin i) : SV_TARGET
{
    float noise =
    snoise(i.GlobalPosition * 0.25) +
    snoise(i.GlobalPosition * 0.5) * 0.5 +
    snoise(i.GlobalPosition * 4) * 0.25 +
    snoise(i.GlobalPosition * 8) * 0.125 +
    0;
    float3 grass = float3(0.091f, 0.121f, 0.072f);
    float3 earth = float3(0.061f, 0.052f, 0.011f);
    float3 sand = float3(0.123f, 0.121f, 0.066f);
    return float4(noise > 0 ?
    grass * noise + earth * (1 - noise) :
    -sand * noise + earth * (1 + noise), 1);
}