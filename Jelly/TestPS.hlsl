
struct PSin
{
    uint Color : Color;
    float3 ViewNormal : Normal;
    float3 ViewPosition : Position;
    float3 ViewLight : Light;
    float4 Position : SV_Position;
};

float4 main(PSin i) : SV_TARGET
{
    float3 Colors[] =
    {
        float3(0.3, 0.6, 0.1),
        float3(0.1, 0.6, 0.3),
        float3(0.2, 0.7, 0.2),
        float3(0.282f, 0.541f, 0.243f)
    };
    
 
    float diffuseLighting = saturate(dot(i.ViewNormal, -i.ViewLight));
 
 
    return float4(saturate(
    (Colors[i.Color % 4] * (diffuseLighting + 0.3) *0.5)
    ), 1);
}