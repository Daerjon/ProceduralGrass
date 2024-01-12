struct VS_CONTROL_POINT_OUTPUT
{
    float3 pos : POSITION;
};

#define NUM_CONTROL_POINTS 16
struct HS_CONTROL_POINT_OUTPUT
{
    float3 pos : POSITION;
};
struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[4] : SV_TessFactor;
    float InsideTessFactor[2] : SV_InsideTessFactor;
};

struct DS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 globalPos : POSITION;
    float3 normal : NORMAL;
    float3 view : VIEWVEC;
};

