#include"BezierBasis.hlsli"
cbuffer cbView : register(b0)
{
    float4x4 viewMatrix;
}
cbuffer cbProj : register(b1)
{
    float4x4 projMatrix;
}
cbuffer cbModel : register(b2)
{
    float4x4 modelMatrix;
}
cbuffer cbBezierCube : register(b3)
{
    float3 cPoints[64];
}

struct VSInput
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD;
};
struct PSInput
{
    float4 pos : SV_POSITION;
    float3 globalPos : POSITION;
    float3 norm : NORMAL;
    float3 view : VIEWVEC;
};

PSInput main( VSInput i )
{
    float3 ipos = mul(modelMatrix, float4(i.pos, 1)).xyz;
    float3 inorm = normalize(mul(modelMatrix, float4(i.norm, 0)).xyz);
    float bx[4], by[4], bz[4];
    float dbx[4], dby[4], dbz[4];
    BezierBasis(bx, dbx, ipos.x);
    BezierBasis(by, dby, ipos.y);
    BezierBasis(bz, dbz, ipos.z);
    
    float3 pos, dx, dy, dz;
    pos = dx = dy = dz = float3(0, 0, 0);
    for (int z = 0; z < 4; z++)
        for (int y = 0; y < 4; y++)
            for (int x = 0; x < 4; x++)
            {
                pos += bx[x] * by[y] * bz[z] * cPoints[x + y * 4 + z * 16].xyz;
                dx += dbx[x] * by[y] * bz[z] * cPoints[x + y * 4 + z * 16].xyz;
                dy += bx[x] * dby[y] * bz[z] * cPoints[x + y * 4 + z * 16].xyz;
                dz += bx[x] * by[y] * dbz[z] * cPoints[x + y * 4 + z * 16].xyz;
            }
    PSInput o;
    o.globalPos = float4(pos, 1);
    o.pos = mul(projMatrix, mul(viewMatrix, float4(pos, 1)));
    float3x3 m = float3x3
    (
    cross(dy, dz),
    cross(dz, dx),
    cross(dx, dy)
    );
    o.norm = normalize(mul(transpose(m), inorm));
    o.view = mul(transpose(viewMatrix), float4(0, 0, -1, 0)).xyz;
    return o;
}