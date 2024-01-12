#include"Jelly.hlsli"
#include"Phong.hlsli"

cbuffer cbColors : register(b0)
{
    float4 color;
}

float4 main(DS_OUTPUT i) : SV_TARGET
{
    return phong(i.globalPos,i.normal, i.view);
}