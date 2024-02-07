#include"BezierBasis.hlsli"
#include"Noise.hlsli"

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
};
struct PSin
{
    uint Color : Color;
    float3 ViewNormal : Normal;
    float3 ViewLight : Light;
    float2 UV : Texcoord;
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
    const float facingParam = -0.5f;
    const float3 vtx[15] =
    {
        float3(1, 0, 0),
        float3(-1, 0, 0),
        float3(1, 1.0f / 7, 0),
        float3(-1, 1.0f / 7, 0),
        float3(1, 2.0f / 7, 0),
        float3(-1, 2.0f / 7, 0),
        float3(1, 3.0f / 7, 0),
        float3(-1, 3.0f / 7, 0),
        float3(1, 4.0f / 7, 0),
        float3(-1, 4.0f / 7, 0),
        float3(0.8f, 5.0f / 7, 0),
        float3(-0.8f, 5.0f / 7, 0),
        float3(0.5f, 6.0f / 7, 0),
        float3(-0.5f, 6.0f / 7, 0),
        float3(0.0f, 1, 0)
    };
    Blade blade = Data[i.iid];
    blade.Wind *= blade.Height;
    
    float2 botFacing = normalize(blade.Facing * (1 - abs(facingParam)) + blade.ClumpFacing*facingParam);
    float windAlong = dot(windDirection, botFacing);
    float windAcros = 1 - abs(windAlong);
    float2 topFacing = normalize(windAcros * windDirection * blade.Wind +
        botFacing * (1 - blade.Wind));
    
    float s, c;
    sincos(radians(clamp(blade.Tilt + windAlong * blade.Wind/2, -1, 1) * 90), s, c);
    float3 tilted1 = float3(botFacing.x * s, c, botFacing.y * s);
    float3 tilted2 = float3(topFacing.x * s , c, topFacing.y * s);
    
    float3 l1 = cross(float3(0, 1, 0), float3(botFacing.x, 0, botFacing.y));
    float3 l2 = cross(float3(0, 1, 0), float3(topFacing.x, 0, topFacing.y));
    
    float b2[3], db2[3];
    BezierBasis2(b2, db2, vtx[i.vid].y);
    
    float3 cps[3];
    cps[0] = float3(0, 0, 0);
    
    cps[1] = tilted1 * 0.5 + 
    ((cross(tilted1, l1) * blade.Bend * 0.7 + float3(0, 1, 0) * (1 - blade.Bend) * 0.3))*blade.Bend;
    
    cps[2] = tilted2;
    
    float2 profile = float2(0.05f, 1);
    
    float3 pos =
    vtx[i.vid].x * profile.x * (l1 * (b2[0] + b2[1]) + l2 * b2[2]) * blade.Width +
    vtx[i.vid].y * profile.y * (b2[1] * cps[1] + b2[2] * cps[2]) * blade.Height +
    blade.Position ;
    float3 normal = normalize(cross(l1, db2[1] * cps[1]) + cross(l2, db2[2] * cps[2]));
    
    
    //float d = 0.001;
    //float dx = snoise((blade.Position + float3(d, 0, 0)) / 64) - snoise((blade.Position - float3(d, 0, 0)) / 64)/d*0.5;
    //float dz = snoise((blade.Position + float3(0, 0, d)) / 64) - snoise((blade.Position - float3(0, 0, d)) / 64)/d*0.5f;
    //float3 xn = normalize(float3(1, dx, 0));
    //float3 zn = normalize(float3(0, dz, 1));
    
    //float3x3 m = float3x3
    //(
    //xn,
    //cross(xn, zn),
    //zn
    //);
    //normal = mul(transpose(m), normal);
    
    PSin o;
    float3 ViewPosition = mul(viewMatrix, float4(pos, 1)).xyz;
    o.Position = mul(projMatrix, float4(ViewPosition, 1));
    o.Color = blade.ClumpColor;
    o.ViewNormal = mul(viewMatrix, float4(normal,0)).xyz;
    o.ViewLight = mul(viewMatrix, float4(0, 1, 0, 0)).xyz;
    o.UV = vtx[i.vid];
    return o;
}