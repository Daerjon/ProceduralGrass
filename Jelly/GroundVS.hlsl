
cbuffer cbView : register(b0)
{
    float4x4 viewMatrix;
}
cbuffer cbProj : register(b1)
{
    float4x4 projMatrix;
}
struct VS_CONTROL_POINT_OUTPUT
{
    float3 vPosition : WORLDPOS;
};
VS_CONTROL_POINT_OUTPUT main(float3 pos : POSITION)
{
    VS_CONTROL_POINT_OUTPUT o;
    o.vPosition = pos;
    return o;
}