
cbuffer cbView : register(b0)
{
    float4x4 viewMatrix;
}
cbuffer cbProj : register(b1)
{
    float4x4 projMatrix;
}

float4 main(float3 pos : POSITION) : SV_POSITION
{
    return mul(projMatrix, mul(viewMatrix, float4(pos, 1)));
}