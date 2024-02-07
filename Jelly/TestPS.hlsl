
struct PSin
{
    uint Color : Color;
    float3 ViewNormal : Normal;
    float3 ViewLight : Light;
    float4 Position : SV_Position;
};

float4 main(PSin i) : SV_TARGET
{
    float3 Colors[] =
    {
        float3(0.2, 0.5, 0.1),
        float3(0.1, 0.5, 0.2),
        float3(0.2, 0.5, 0.2),
        float3(0.282f, 0.541f, 0.243f),
        float3(0.3, 0.4, 0.2)
    };
    
 
    float diffuseLighting = saturate(dot(i.ViewNormal, -i.ViewLight));
 
 
    return float4(saturate(
    (Colors[i.Color % 5] * (diffuseLighting + 0.3) *0.5)
    ), 1);
}