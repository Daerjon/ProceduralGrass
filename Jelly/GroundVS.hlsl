
cbuffer cbView : register(b0)
{
    float4x4 viewMatrix;
}
cbuffer cbProj : register(b1)
{
    float4x4 projMatrix;
}

struct PSin
{
    float3 GlobalPosition : Position;
    float4 Position : SV_Position;
};
PSin main(float3 pos : POSITION)
{
    PSin o;
    o.GlobalPosition = pos;
    o.Position = mul(projMatrix, mul(viewMatrix, float4(pos, 1)));
    return o;
}