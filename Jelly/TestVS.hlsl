
cbuffer cbView : register(b0)
{
    float4x4 viewMatrix;
}
cbuffer cbProj : register(b1)
{
    float4x4 projMatrix;
}
struct VSin
{
    float3 pos : POSITION;
    uint iid : SV_InstanceID;
    uint vid : SV_VertexID;
};

float4 main(VSin i) : SV_POSITION
{
    const float3 vtx[15] =
    {
        float3(0.05f, 0, 0),
        float3(-0.05f, 0, 0),
        float3(0.05f, 1.0f / 7, 0),
        float3(-0.05f, 1.0f / 7, 0),
        float3(0.05f, 2.0f / 7, 0),
        float3(-0.05f, 2.0f / 7, 0),
        float3(0.05f, 3.0f / 7, 0),
        float3(-0.05f, 3.0f / 7, 0),
        float3(0.05f, 4.0f / 7, 0),
        float3(-0.05f, 4.0f / 7, 0),
        float3(0.05f, 5.0f / 7, 0),
        float3(-0.05f, 5.0f / 7, 0),
        float3(0.04f, 6.0f / 7, 0),
        float3(-0.04f, 6.0f / 7, 0),
        float3(0.0f, 1, 0)
    };
    float3 pos = vtx[i.vid];
    return mul(projMatrix, mul(viewMatrix, float4(pos, 1)));
}