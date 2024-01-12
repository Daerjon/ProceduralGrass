#include"Jelly.hlsli"
#include"BezierBasis.hlsli"


cbuffer cbView : register(b0)
{
    float4x4 viewMatrix;
}
cbuffer cbProj : register(b1)
{
    float4x4 projMatrix;
}




[domain("quad")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	//Output.pos = float4(
	//	(1 - domain.x) * (1 - domain.y) * patch[0].pos +
	//	domain.x * (1 - domain.y) * patch[3].pos +
	//	(1 - domain.x) * domain.y * patch[12].pos +
	//	domain.x * domain.y * patch[15].pos
	//,1);
    
    float bx[4], by[4];
    float dbx[4], dby[4];
    BezierBasis(bx, dbx, domain.x);
    BezierBasis(by, dby, domain.y);

    float3 pos;
    float3 dx, dy;
    pos = dx = dy = float3(0, 0, 0);
    
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
        {
            pos += bx[x] * by[y] * patch[x + y * 4].pos;
            dx += dbx[x] * by[y] * patch[x + y * 4].pos;
            dy += bx[x] * dby[y] * patch[x + y * 4].pos;
        }
            
    Output.globalPos = pos;
    Output.pos = mul(projMatrix, mul(viewMatrix, float4(pos, 1)));
    Output.pos /= abs(Output.pos.w);
    Output.normal = normalize(cross(dx, dy));
    Output.view = mul(transpose(viewMatrix), float4(0, 0, -10, 0));
    
	return Output;
}
