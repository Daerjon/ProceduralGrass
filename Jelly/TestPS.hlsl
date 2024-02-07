
struct PSin
{
    uint Color : Color;
    float3 ViewNormal : Normal;
    float3 ViewLight : Light;
    float2 UV : Texcoord;
    float4 Position : SV_Position;
};

float4 main(PSin i) : SV_TARGET
{
    float3 Colors[] =
    {
        float3(0.2, 0.5, 0.1),
        float3(0.1, 0.5, 0.2),
        float3(0.2, 0.5, 0.2),
        float3(0.1, 0.45, 0.2),
        float3(0.15, 0.4, 0.2),
        float3(0.15, 0.4, 0.15)
    };
    float3 EndColors[] =
    {
        float3(0.3, 0.45, 0.20),
        float3(0.35, 0.45, 0.35),
        float3(0.4, 0.45, 0.25),
        float3(0.2, 0.45, 0.10),
        float3(0.15, 0.45, 0.15),
        float3(0.9, 0.9, 0.8)
    };
    //float3 Colors[] =
    //{
    //    float3(1,0,0),
    //    float3(0,1,0),
    //    float3(0,0,1),
    //    float3(0,1,1),
    //    float3(1,0,1),
    //    float3(1,1,0)
    //};
    
 
    float diffuseLighting = saturate(dot(i.ViewNormal, -i.ViewLight));
    float t = max(i.UV.x * i.UV.x, i.UV.y * i.UV.y);
    float3 colorBlend = t * EndColors[(i.Color / 6) % 6] +
    (1 - t) * Colors[i.Color % 6];
 
    return float4(saturate(
    (colorBlend * (diffuseLighting + 0.3) * 0.7)
    ), 1);
}