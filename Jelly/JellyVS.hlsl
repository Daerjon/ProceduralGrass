#include"Jelly.hlsli"

cbuffer cbView : register(b0)
{
    float4x4 viewMatrix;
}

VS_CONTROL_POINT_OUTPUT main(float3 pos : POSITION) : POSITION
{
    VS_CONTROL_POINT_OUTPUT o;
    o.pos = pos;
    return o;
}