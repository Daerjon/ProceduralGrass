#include"Phong.hlsli"

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 globalPos : POSITION;
    float3 norm : NORMAL;
    float3 view : VIEWVEC;
};

float4 main(PSInput i) : SV_TARGET
{
    return phong(i.globalPos, i.norm, i.view);
}