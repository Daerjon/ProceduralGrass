#include"BezierBasis.hlsli"

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
    uint iid : SV_InstanceID;
    uint vid : SV_VertexID;
    float3 Position : Position;
    float2 Facing : Facing;
    float Wind : Wind;
    uint Hash : Hash;
    uint Type : Type;
    float2 ClumpFacing : ClumpFacing;
    uint ClumpColor : ClumpColor;
    float Height : Height;
    float Width : Width;
    float Tilt : Tilt;
    float Bend : Bend;
    uint SideCurve : SideCurve;
};
struct PSin
{
    uint Color : Color;
    float3 ViewNormal : Normal;
    float3 ViewLight : Light;
    float4 Position : SV_Position;
};

struct Blade
{
    float3 Position;
    float2 Facing;
    float Wind;
    uint Hash;
    uint Type;
    float2 ClumpFacing;
    uint ClumpColor;
    float Height;
    float Width;
    float Tilt;
    float Bend;
    uint SideCurve;
};

StructuredBuffer<Blade> Data : register(t1);

PSin main(VSin i)
{
    const float2 windDirection = float2(1, 0);
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
    Blade blade = Data[i.iid];
    float windAlong = dot(windDirection, blade.Facing);
    float windAcros = 1 - windAlong;
    float2 topFacing = -windAcros * windDirection + windAlong * blade.Facing;
    
    float s, c;
    sincos(radians((blade.Tilt + windAlong*blade.Wind)*0.5 * 90), s, c);
    float3 tilted1 = float3(blade.Facing.x * s, c, blade.Facing.y * s);
    float3 tilted2 = float3(topFacing.x * s, c, topFacing.y * s);
    
    float3 l1 = cross(float3(0, 1, 0), float3(blade.Facing.x, 0, blade.Facing.y));
    float3 l2 = cross(float3(0, 1, 0), float3(topFacing.x, 0, topFacing.y));
    
    float b2[3], db2[3];
    BezierBasis2(b2, db2, vtx[i.vid].y);
    
    float3 cps[3];
    cps[0] = float3(0, 0, 0);
    
    cps[1] = tilted1 * 0.6 + 
    (cross(tilted1, l1) * blade.Tilt + float3(0, 2, 0) * (1 - blade.Tilt)) * blade.Bend * 0.4;
    
    cps[2] = tilted2;
    
    float3 pos =
    vtx[i.vid].x * (l1 * b2[0] + l2 * b2[2]) * blade.Width +
    vtx[i.vid].y * (b2[1] * cps[1] + b2[2] * cps[2]) * blade.Height +
    blade.Position;
    float3 normal = normalize(cross(l1, db2[1] * cps[1]) + cross(l2, db2[2] * cps[2]));
    PSin o;
    float3 ViewPosition = mul(viewMatrix, float4(pos, 1)).xyz;
    o.Position = mul(projMatrix, float4(ViewPosition, 1));
    o.Color = blade.ClumpColor;
    o.ViewNormal = mul(viewMatrix, float4(normal,0)).xyz;
    o.ViewLight = mul(viewMatrix, float4(0, 1, 0, 0)).xyz;
    return o;
}